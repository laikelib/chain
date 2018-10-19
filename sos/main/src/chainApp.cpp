

#include <huibase.h>
#include <hadaptation.h>
#include <hfname.h>
#include <chainApp.h>
#include <glog/logging.h>
#include <txdb-leveldb.h>
#include <base58.h>
#include <util.h>
#include <hash.h>
#include <serialize.h>

#include <hcrypto.h>


using namespace HUIBASE;


static inline string MakeJsonReturn (HCSTRR str) {

    return string("{\"errno\": 0, \"errmsg\": \"ok\", \"result\":\"" ) + str + "\"}";

}

CMainChain::CMainChain ()
    : m_isInited(HFALSE){


}


CMainChain::~CMainChain () {


}



HSTR CMainChain::GetBlockTemplate (HCSTRR strAddr) {

    /*

      > get block template;
      > get txs;
      > set bits;
      > make merkleroot;

     */

    using namespace HUIBASE::CRYPTO;

    LOG(INFO) << "CMainChain::GetBlockTemplate() Begin... with [" << strAddr << "]";

    // makesure it will not product repeat entry.
    sleep(1);

    shared_ptr<CBlock> pbt (GetNewBlock(strAddr));

    LOG(INFO) << "block template hash: [" << pbt->GetHash().ToString() << "]";

    LOG(INFO) << "BLOCK: " << pbt->GetBlockJson();

    CDataStream ds(SER_NETWORK, NODE_VERSION);
    ds << (*pbt);

    HSTR res;
    HEncode(ds.str(), res);

    LOG(INFO) << "CMainChain::GetBlockTemplate() End...";

    return res;

}


HSTR CMainChain::PostWork(HCSTRR str) {
    LOG(INFO) << "CMainChain::PostWork() Begin...";

    using namespace HUIBASE::CRYPTO;

    HSTR strIn;
    HIF_NOTOK(HDecode(str, strIn)) {

        LOG(ERROR) << "decode input string failed";
        return "";

    }

    HBUF hbuf;
    hstr_vs(strIn, hbuf);
    CDataStream ds(hbuf.begin(), hbuf.end(), SER_NETWORK, NODE_VERSION);

    HSTR res;

    CBlock* pbk = new CBlock();
    CHECK_NEWPOINT(pbk);
    shared_ptr<CBlock> sharedptr(pbk);

    ds >> (*pbk);

    LOG(INFO) << "block information: " << pbk->ToJsonString();

    uint256 block_hash = pbk->GetHash();
    LOG(INFO) << "hash: [" << block_hash.ToString() << "]";

    if (not chainCheckBlock(pbk) ) {

        LOG(ERROR) << "chainCheckBlock failed....";
        return "";

    }

    if (not pbk->CheckBlock ()) {

        LOG(ERROR) << "check block failed...";
        return "";

    }


    // save block to file
    if (not AcceptBlock (pbk)) {

        LOG(ERROR) << "save block to file failed...";
        return "";

    }

    if (not ProcessBlock(pbk)) {

        LOG(ERROR) << "process block failed";
        return "";

    }

    LOG(INFO) << "CMainChain::PostWork() End...";

    res = HCStr::Format("{\"height\":%d}", GetBestHeight());
    return res;

}


HSTR CMainChain::GetHeight() {

    HINT nHeight = GetBestHeight ();

    HSTR str = HCStr::Format("%d", nHeight);

    return str;

}


HSTR CMainChain::HeightBlock (HCSTRR str) {

    using namespace HUIBASE::CRYPTO;

    HINT nHeight = HCStr::stoi(str);

    if (nHeight > GetBestHeight()) {

        LOG(ERROR) << "invalid param. Height: " << nHeight << ", best height: " << GetBestHeight();

        return "";

    }

    CBlockIndex* pindex = GetBlockIndex(nHeight);
    if (pindex == nullptr) {
        LOG(ERROR) << "cann't find the block index";
        return "";
    }

    CBlock* pbk = new CBlock();
    CHECK_NEWPOINT(pbk);

    shared_ptr<CBlock> sp(pbk);

    pbk->ReadFromDisk (pindex);

    LOG(INFO) << "BLOCK: " << pbk->GetBlockJson();

    CDataStream ds(SER_NETWORK, NODE_VERSION);
    ds << (*pbk);

    HSTR res;
    HEncode(ds.str(), res);

    return res;

}


HSTR CMainChain::GetBal(HCSTRR strAddr) {

    CLKAddress addr(strAddr);
    HN64 nBal = m_mngBal.GetBal(addr);

    LOG(INFO) << strAddr << ": " << nBal;

    return HCStr::lltos(nBal);

}


HSTR CMainChain::HashTx(HCSTRR strHash) {

    uint256 hash;
    hash.SetHex(strHash);
    LOG(INFO) << "input hash: " << hash.ToString();

    const CEntryIndex* cei = m_history.GetEntryIndex(hash);
    LOG(ERROR) << strHash << " can't matched any block index";

    if (cei) {
        return cei->ToString();
    }

    return "\"DROP\"";

}


HSTR CMainChain::LkInfo() {

    HSTR res = m_info.GetNodeInfo ();

    return res;

}


HSTR CMainChain::newTx(HCSTRR str) {

    using namespace HUIBASE::CRYPTO;
    HSTR strIn;

    HIF_NOTOK(HDecode(str, strIn)){
        LOG(ERROR) << "decode input string failed";
        return "";
    }

    HBUF hbuf;
    hstr_vs(strIn, hbuf);
    CDataStream ds(hbuf.begin(), hbuf.end(), SER_NETWORK, NODE_VERSION);

    CEntry new_en;
    ds >> new_en;

    LOG(INFO) << "new entry: " << new_en.ToJsonString();

    if (not checkNewEntry(new_en)) {

        LOG(ERROR) << "check new entry balance";
        return "";

    }

    if (not new_en.Check()) {

        LOG(ERROR) << "check new entry failed";
        return "";

    }

    if (not new_en.VerifySign()) {

        LOG(ERROR) << "new entry check signature failed";
        return "";

    }

    m_mngEntrys.AddNewEntry(new_en);

    LOG(INFO) << "new entry: " << new_en.GetEntryHash().ToString();

    return new_en.GetEntryHash().ToString();

}




HRET CMainChain::initLast() {

	(void) loadBlockChain();

    (void) checkBlockChain();

    (void) initHistory();

    (void) initBalance();

    (void) checkBalance();

    m_mngBal.Display();

    displayBlockChain();

    m_isInited = HTRUE;

    HRETURN_OK;

}

HRET CMainChain::loadBlockChain() {

    LOG(INFO) << "CMainChain::loadBlockChain...";

    CCoinTxdb ctxdb("cr+");

    if (not ctxdb.LoadBlockIndex()) {

        LOG(ERROR) << "ctxdb.LoadBlockIndex() failed";

        HRETURN(SRC_FAIL);

    }

    IF_TRUE(m_mapBlockIndex.IsEmpty()) {

        LOG(INFO) << "Initialize genesis block";

        CBlock block = GenesisBlock();

        LOG(INFO) << "genesis block hash: [" << block.GetHash().ToString() << "]";
        LOG(INFO) << "block: [" << block.ToJsonString() << "];";

        unsigned int nFile = 0, nBlockPos = 0;

        if (not block.WriteToDisk (nFile, nBlockPos)) {

            LOG(ERROR) << "block.Write disk failed";

            HRETURN(IO_ERR);

        }

        LOG(INFO) << "Write file: " << nFile << ", position: " << nBlockPos;

        if (not AddToBlockIndex (block, nFile, nBlockPos) ){

            LOG(ERROR) << "block add to block index failed...";

            HRETURN(ILL_PT);

        }

        LOG(INFO) << "Init block chain success...";

    }

    HRETURN_OK;

}


HRET CMainChain::checkBlockChain() {
    LOG(INFO) << "checkBlockChain BEGIN ...";

    // check genesis info;
    LOG(INFO) << "check genesis information...";
    HIF_NOTOK(m_mapBlockIndex.CheckGenesisInfo (GetGenesisBlockHash())) {

        LOG(INFO) << "genesis hash:" << GetGenesisBlockHash().ToString();
        LOG(ERROR) << "check blockchain genesis information failed";
        HRETURN(SRC_FAIL);

    }

    // check blocks;
    LOG(INFO) << "check block ...";
    HIF_NOTOK(m_mapBlockIndex.CheckBlocks()) {

        LOG(ERROR) << "check blocks failed";
        HRETURN(ERR_STATUS);

    }

    // check root info;
    LOG(INFO) << "check root, best hash, height information...";
    HIF_NOTOK(m_mapBlockIndex.CheckRootBestHeight(GetHashBest(), GetHashBestRoot(), GetBestHeight())) {

        LOG(ERROR) << "check root, hest hash, height information failed";
        HRETURN(ERR_STATUS);

    }

    LOG(INFO) << "checkBlockChain END ...";
    HRETURN_OK;
}


HRET CMainChain::initHistory() {
    LOG(INFO) << __FUNCTION__ << " BEGIN...";

    using HEIGHTINDEXMAP = CBlockIndexMap::BLOCKINDEXHEIGHTMAP;
    using INDEXSET = CBlockIndexMap::INDEXSET;

    const HEIGHTINDEXMAP& index_map = m_mapBlockIndex.GetHeightIndex();

    for (HEIGHTINDEXMAP::const_iterator cit = index_map.cbegin(); cit != index_map.cend(); ++cit) {

        const INDEXSET& cvs = cit->second;

        for (INDEXSET::const_iterator citv = cvs.cbegin(); citv != cvs.cend(); ++citv) {

            const CBlockIndex* pIndex = *citv;

            m_history.OverIndex(const_cast<CBlockIndex*>(pIndex));

        }

    }


    LOG(INFO) << __FUNCTION__ << " END...";
    HRETURN_OK;
}


HRET CMainChain::initBalance() {
    LOG(INFO) << __FUNCTION__ << " BEGIN...";
    // init balmap;
    m_mngBal.OverBlockChain(m_mapBlockIndex);

    LOG(INFO) << __FUNCTION__ << " END...";

    HRETURN_OK;

}


HRET CMainChain::checkBalance() const {

    HUINT nHeight = GetBestHeight();
    HN64 sum = 0;

    if (nHeight <= m_params.FirstHalfPoint() ) {

        sum = nHeight * m_params.FirstPay();

    } else if (nHeight > m_params.FirstHalfPoint() && nHeight <= m_params.SecondHalfPoint()) {

        sum = m_params.FirstHalfPoint() * m_params.FirstPay ();
        sum += (nHeight - m_params.FirstHalfPoint()) * m_params.SecondPay();

    } else if (nHeight > m_params.SecondHalfPoint() && nHeight <= m_params.LastPoint()) {

        sum = m_params.FirstHalfPoint() * m_params.FirstPay ();
        sum += (m_params.SecondHalfPoint() - m_params.FirstHalfPoint()) * m_params.SecondPay();
        sum += (nHeight - m_params.LastPoint()) * m_params.LastPay();

    } else if (nHeight > m_params.LastPoint()) {

        sum += m_params.GetSupplySum ();

    }

    LOG(INFO) << "height: " << nHeight << ", height-sum: " << sum <<", now-sum: " << m_mngBal.GetBalSum ();

    HASSERT_RETURN(sum == m_mngBal.GetBalSum(), ERR_STATUS);

    HASSERT_RETURN(sum <= m_params.GetSupplySum(), ERR_STATUS);

    HRETURN_OK;

}


HRET CMainChain::Shutdown() {

    LOG(INFO) << "CMainChain shutdown...";

    //return CChainCore::Shutdown();
    HRETURN_OK;

}


CBlockIndex* CMainChain::InsertBlockIndex(const uint256 &hash) {

    return m_mapBlockIndex.newBlockIndex(hash);

}


CBlock* CMainChain::GetNewBlock(HCSTRR strAddr) {

    // Create a new block without proof-of-work.
    LOG(INFO) << "CMainChain::GetNewBlock ...";

    CBlock* pret = new CBlock();
    CHECK_NEWPOINT(pret);

    pret->SetHashPrevBlock (GetHashBest());

    CLKAddress addr(strAddr);
    LOG(INFO) << "default benifit address: " << addr.ToString();

    pret->SetTime(time(nullptr));

    pret->SetNonce (0);

    // create coinbase entry
    // 1> add coinbase;
    // TODO: coinbase 10' lkt
    CEntry baseEntry = CreateCoinBase(addr);

    pret->SetBaseEntry(baseEntry);

    // 2> add other txs;
    std::vector<CEntry> es;
    m_mngEntrys.GetUnBlockedEntrys(es);

    LOG(INFO) << "will block " << es.size() << "'s entry to new block...";
    pret->BlockEntrys (es);

    // set bits
    LOG(INFO) << "set new block's bits...";
    pret->SetBits (GetTargetBits());
    LOG(INFO) << "bits: " << pret->GetBits();

    // set merkleroot
    LOG(INFO) << "Build merkleroot...";
    pret->BuildMerkleRootTree ();

    LOG(INFO) << "CMainChain::NewBlock end....";
    return pret;

}


HINT CMainChain::GetTargetBits () {

    CBigNum bnTargetLimit = m_params.GetPowLimit ();
    LOG(INFO) << "bnTargetLimit: " << bnTargetLimit.ToString();

    if (GetBestHeight() < 3) {

        LOG(INFO) << "height: " << GetBestHeight() << ", set lessest bits";

        return bnTargetLimit.GetCompact(); // genesis block

    }

    LOG(INFO) << "height: " << GetBestHeight() <<", will set new bits";

    const CBlockIndex* pindexPrev = GetLastBlockIndex(GetBestIndex());
    if (pindexPrev->GetPrevIndex() == nullptr) {
        // TODO: alawys here.
        return bnTargetLimit.GetCompact();
    }

    LOG(INFO) << "pindexPrev height: " << pindexPrev->GetHeight();

    const CBlockIndex* pindexPrevPrev = GetLastBlockIndex (pindexPrev->GetPrevIndex());
    if (pindexPrevPrev->GetPrevIndex() == nullptr) {

        return bnTargetLimit.GetCompact();

    }

    LOG(INFO) << "pindexPrevPrev height: " << pindexPrevPrev->GetHeight();

    int64_t nActualSpacing = pindexPrev->GetBlockTime() - pindexPrevPrev->GetBlockTime();

    LOG(INFO) << "actual spacing: " << nActualSpacing;

    if(nActualSpacing < 0)
    {
        nActualSpacing = 1;
    }
    else if(nActualSpacing > (int64_t)m_params.GetTargetTimeSpan())
    {
        nActualSpacing = m_params.GetTargetTimeSpan();
    }

    CBigNum bnNew;
    bnNew.SetCompact(pindexPrev->GetBits());
    int64_t nTargetSpacing = min((int64_t)m_params.GetTargetSpacingWorkMax(), (int64_t)m_params.GetTargetSpacing () * (1 + GetBestIndex()->GetHeight() - pindexPrev->GetHeight()));
    int64_t nInterval = (int64_t)m_params.GetTargetTimeSpan() / nTargetSpacing;
    bnNew *= ((nInterval - 1) * nTargetSpacing + nActualSpacing + nActualSpacing);
    bnNew /= ((nInterval + 1) * nTargetSpacing);

    if (bnNew <= 0 || bnNew > bnTargetLimit)
        bnNew = bnTargetLimit;

    LOG(INFO) << "new bits: " << bnNew.GetCompact();
    return bnNew.GetCompact();

}


bool CMainChain::chainCheckBlock(const CBlock* pbk) const {
    LOG(INFO) << "CMainChain::chainCheckBlock";

    uint256 block_hash = pbk->GetHash();

    // not this hash
    IF_TRUE(m_mapBlockIndex.HasBlock(block_hash)) {

        LOG(ERROR) << "hash [" << block_hash.ToString() << "] is exists...";
        return false;

    }

    // TODO: if the block is not match the prev hash, add it to a banch list.
    // TODO: for now, we only accept the best chain. NOT BRANCH CHAIN.
    // check prev hash == besthash
    const uint256& prev_hash = pbk->GetHashPrevBlock();
    if (prev_hash != GetHashBest()) {

        LOG(ERROR) << "error block to chain. best hash: [" <<
	    GetHashBest().ToString() << "], prev hash: [" << prev_hash.ToString() << "]";
        return false;

    }

    // block time check
    const CBlockIndex * bestIndex = GetBestIndex ();
    if (bestIndex == nullptr) {

        LOG(ERROR) << "bestindex is null";
        return false;

    }

    HUINT best_time = bestIndex->GetTime ();
    if (best_time > pbk->GetTime()) {

        LOG(ERROR) << "new block time is error";
        return false;

    }

    LOG(INFO) << "CMainChain::chainCheckBlock";
    return true;
}


bool CMainChain::AcceptBlock (CBlock* pbk) {

    LOG(INFO) << "accept block, hash: [" << pbk->GetHash().ToString() << "]";

    unsigned int nFile = 0, nBlockPos = 0;

    if ( not pbk->WriteToDisk(nFile, nBlockPos)) {

        LOG(ERROR) << "block write disk failed";
        return false;

    }

    LOG(INFO) << "write file: " << nFile << ", pos: " << nBlockPos;

    if (not AddToBlockIndex(*pbk, nFile, nBlockPos)) {

        LOG(ERROR) << "block add to block index failed...";
        return false;

    }

    LOG(INFO) << "Write block chain success...";

    return true;

}


bool CMainChain::ProcessBlock(CBlock* pbk) {

    LOG(INFO) << "CMainChain::ProcessBlock begin...";

    // add to history;
    m_history.OverBlock(pbk, const_cast<CBlockIndex*>(GetBestIndex()));

    // remove txs;
    m_mngEntrys.AcceptBlock(*pbk);

    // update balmng;
    m_mngBal.OverNewBlock(*pbk);

    LOG(INFO) << "CMainChain::ProcessBlock end...";

    return true;
}


HRET CMainChain::SetIndexHeight (const uint256& hash, HUINT nHeight) {

    return m_mapBlockIndex.SetIndexHeight(hash, nHeight);

}


bool CMainChain::AddToBlockIndex (CBlock& block, unsigned int nFile, unsigned int nBlockPos) {
    LOG(INFO) << "CMainChain::AddToBlockIndex...";

    // Check for duplicate
    uint256 hash = block.GetHash();
    IF_TRUE(m_mapBlockIndex.HasBlock(hash)) {
        LOG(ERROR) << "AddToBlockIndex(): " << hash.ToString() << " already exists";
        return false;
    }

    LOG(INFO) << "AddToBlockIndex, File: " << nFile << ", position: " << nBlockPos << ", hash: [" << hash.ToString() << "]";

    // Construct new block index object
    CBlockIndex* pindexNew = new CBlockIndex(nFile, nBlockPos, block);
    CHECK_NEWPOINT(pindexNew);

    LOG(INFO) << "AddToBlockIndex, index information: (" << pindexNew->ToString() << ")";

    CBlockIndex* pPrevIndex = m_mapBlockIndex.GetIndex(block.GetHashPrevBlock());

    LOG(INFO) << "map size: " << m_mapBlockIndex.GetCount() << ", prev index: " << pPrevIndex;

    IF_FALSE(m_mapBlockIndex.IsEmpty()) {
        HASSERT_THROW_MSG(pPrevIndex != nullptr, "prev block is null", ERR_STATUS);
    }

    pindexNew->SetMain(true);
    pindexNew->SetPrevIndex(pPrevIndex);

    pindexNew->SetHeight(pPrevIndex ? pindexNew->GetPrevIndex()->GetHeight() + 1 : 0);

    pindexNew->SetMoneySupply(block.GetAmount());

    LOG(INFO) << "AddToBlockIndex hash: [" << hash.ToString() << "], height: " << pindexNew->GetHeight();

    // Add to mapBlockIndex
    m_mapBlockIndex.InsertIndex(hash, pindexNew);
    LOG(INFO) << "pindexNew hash: [" << pindexNew->GetBlockHash ().ToString() << "]";

    // Write to disk block index
    CCoinTxdb txdb;
    if (not txdb.TxnBegin()) {
        LOG(ERROR) << "txdb.TxnBegin failed";
        return false;
    }

    txdb.WriteBlockIndex(CDiskBlockIndex(pindexNew));
    if (not txdb.TxnCommit()) {
        LOG(ERROR) << "txdb.TxnCommit failed";
        return false;
    }

    return SetBestChain(txdb, block, pindexNew);

}


bool CMainChain::SetBestChain(CCoinTxdb& txdb, CBlock& block, CBlockIndex *pindexNew) {

    //  TODO: for now, the root index must be genesis index;

    uint256 hash = block.GetHash();

    LOG(INFO) << "laikelib will set new best block, hash: [" << hash.ToString() << "]";

    if (not txdb.TxnBegin()) {
        LOG(ERROR) << "CMainChain::SetBestChain txdb.TxnBegin() failed";
        return false;
    }

    txdb.WriteHashBestChain(hash);

    if (not txdb.TxnCommit()) {

	    LOG(ERROR) << "SetBestChain() : TxnCommit failed";
	    return false;

    }

    if (GetGenesisIndex() == nullptr && hash == GenesisBlock().GetHash()) {

        if (not txdb.TxnBegin()) {
            LOG(ERROR) << "CMainChain::SetBestChain txdb.TxnBegin() Set Root failed";
            return false;
        }

        txdb.WriteHashBestRoot(hash);

        if (not txdb.TxnCommit()) {

            LOG(ERROR) << "SetBestChain() : TxnCommit Set Root failed";
            return false;

        }

        SetHashBestRoot(hash);

        // make this as gensis block index.
        SetGenesisIndex(pindexNew);

        LOG(INFO) << "Set genesis block, hash: " << hash.ToString() <<", height: " << pindexNew->GetHeight ();

    } else if (pindexNew->GetHashPrevBlock() == GetHashBest()) {
        // set new best, add the new block to the end of the chain.

        pindexNew->SetPrevNext();

        LOG(INFO) << "new block index information: " << pindexNew->ToString();

    }

    // new best block
    m_info.SetBestHash(hash);
    m_info.SetBestIndex(pindexNew);

    LOG(INFO) << "NOW hashBest: " << hash.ToString() << ", bestIndex: " << pindexNew << ", bestheight: " << pindexNew->GetHeight();

    return true;

}


void CMainChain::displayBlockChain() {

    LOG(INFO) << __FUNCTION__;

    LOG(INFO) << "genesis index pointer: " << m_info.GetGenesisIndex() << ", genesis hash: [" << m_info.GetGenesisIndex()->GetHash().ToString() << "]";

    LOG(INFO) << "genesis index string: [" << m_info.GetGenesisIndex()->ToString() << "]";

    LOG(INFO) << "best hash: [" << GetHashBest().ToString() << "]";

    LOG(INFO) << "best block index: " << m_info.GetBestIndex() << ", best block hash: [" << m_info.GetBestIndex()->GetHash().ToString() << "]";

    LOG(INFO) << "best height: [" << m_info.GetCurrentHeight() << "]";

    LOG(INFO) << "mapblockindex size: " << m_mapBlockIndex.GetCount();

    LOG(INFO) << "all entry size: " << m_history.GetEntrySum();

}


void CMainChain::initBestIndex () {

    CBlockIndex* pbi = m_mapBlockIndex.GetIndex(GetHashBest());

    if (pbi == nullptr) {

        LOG(ERROR) << "best blockindex is nullptr";
        return ;

    }

    m_info.SetBestIndex(pbi);

}


CBlockIndex* CMainChain::GetBlockIndex(HUINT nHeight) {

    return m_mapBlockIndex.GetIndex(nHeight);

}


CEntry CMainChain::CreateCoinBase(const CLKAddress &addr) const {

    CEntry entry;
    entry.SetType (NODE_VERSION);

    entry.SetCreateTime(time(nullptr));

    entry.ClearSender();

    entry.SetReceiver(addr);

    entry.SetValue(m_params.CoinBasePay(GetBestHeight() + 1));

    entry.SetFee(0);

    return entry;

}


bool CMainChain::checkNewEntry(const CEntry &new_en) {

    uint256 hashNewEn = new_en.GetEntryHash();
    IF_FALSE(m_history.IsRepeate(new_en.GetEntryHash())) {

        LOG(ERROR) << "new entry is repeate";
        return false;

    }

    m_history.NewTxHash(hashNewEn);

    if (new_en.GetValue() < 0) {
        LOG(ERROR) << "transfer value is invalid";
        return false;
    }


    if (new_en.GetValue() > MAX_TRAN_LKT) {
        LOG(ERROR) << "too more transfer value";
        return false;
    }


    if (new_en.GetFee() > MAX_LKC_FEE) {
        LOG(ERROR) << "too more transfer fee";
        return false;
    }


    if (new_en.GetFee() < MIN_LKC_FEE) {
        LOG(ERROR) << "too less transfer fee";
        return false;
    }


    if (not new_en.IsBaseSign()) {

        HN64 bal = m_mngBal.GetBal(new_en.GetSender());

        HN64 pool_pay = m_mngEntrys.GetPoolPay (new_en.GetSender());

        bal -= pool_pay;

        if (bal < new_en.GetAmount()) {

            LOG(ERROR) << "maybe en double pay, bal:[" << bal << "]";
            return false;

        }

    }

    return true;
}
