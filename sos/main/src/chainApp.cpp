

#include <huibase.h>
#include <hfname.h>
#include <chainApp.h>
#include <glog/logging.h>
#include <wallet.h>
#include <txdb-leveldb.h>
#include <base58.h>
#include <util.h>
#include <hash.h>

using namespace HUIBASE;


static inline string MakeJsonReturn (HCSTRR str) {

    return string("{\"errno\": 0, \"errmsg\": \"ok\", \"result\":\"" ) + str + "\"}";

}

CMainChain::CMainChain ()
    : m_isInited(HFALSE){


}


CMainChain::~CMainChain () {

    HDELP(m_pWallet);

}



HSTR CMainChain::GetBlockTemplate (HCSTRR strAddr) {

    /*

      > get block template;
      > get txs;
      > set bits;
      > make merkleroot;

     */

    LOG(INFO) << "CMainChain::GetBlockTemplate() Begin... with [" << strAddr << "]";

    shared_ptr<CBlock> pbt (GetNewBlock(strAddr));

    LOG(INFO) << "block template hash: [" << pbt->GetHash().ToString() << "]";
        
    HSTR str = pbt->ToJsonString();

    LOG(INFO) << "CMainChain::GetBlockTemplate() End...";

    return str;
    
}


HSTR CMainChain::PostWork(HCSTRR str) {

    LOG(INFO) << "CMainChain::PostWork() Begin...";

    HSTR res;

    CBlock* pbk = new CBlock();
    CHECK_NEWPOINT(pbk);
    shared_ptr<CBlock> sharedptr(pbk);

    if (not pbk->FromJson(str)) {
	
	LOG(ERROR) << "block import failed ...";
	return res;
	
    }

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

    m_pWallet->InitAccountBalance();
    m_pWallet->SetAccountsBalance();

    LOG(INFO) << "CMainChain::PostWork() End...";

    res = HCStr::Format("{\"height\":%d}", GetBestHeight());
    return res;

}


HSTR CMainChain::NewAccount() {

    CPubKey pubkey = m_pWallet->GenerateNewKey();

    CPrivKey prikey = m_pWallet->GetPrivate(pubkey);

    // addr|private hash|private
    CLKAddress addr;
    addr.Set(pubkey.GetID());
    HSTR strAddr = addr.ToString();

    HSTR strPub = EncodeBase64(&pubkey[0], pubkey.size());

    HSTR strPri = EncodeBase64(&prikey[0], prikey.size());

    return strAddr + "|" + strPub + "|" + strPri;

}


HSTR CMainChain::AccountInfo (HCSTRR str) {

    CKey key = m_pWallet->GetKey (CLKAddress(str));

    CPubKey pubkey = key.GetPubKey();
    CPrivKey prikey = key.GetPrivKey();
    
    HSTR strPub = EncodeBase64(&pubkey[0], pubkey.size());

    HSTR strPri = EncodeBase64(&prikey[0], prikey.size());

    return str + "|" + strPub + "|" + strPri;
}


HSTR CMainChain::GetHeight() {

    HINT nHeight = GetBestHeight ();

    HSTR str = HCStr::Format("%d", nHeight);

    return str;

}


HSTR CMainChain::HeightBlock (HCSTRR str) {

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

    return sp->GetBlockJson();
    
}


HSTR CMainChain::QueryAccount(HCSTRR str) {

    CLKAddress addr(str);

    if (not addr.IsValid()) {
	return "";
    }

    HN64 ret = m_pWallet->GetAccountBal(addr);

    double dd = (double) ret / LKT_COIN;

    HSTR res = HCStr::Format("%0.8f", dd);

    return res;
}


HSTR CMainChain::GetTxs(HCSTRR str) {

    CLKAddress addr(str);


    if (not addr.IsValid()) {

	return "";

    }

    std::vector<CEntry> ens;

    m_pWallet->GetTxs(addr, ens);

    std::stringstream ss;

    ss << "{ txs: " << CEntry::GetJsonFromEntrys(ens) << "}";

    return ss.str();
}


HSTR CMainChain::Transfer(HDOUBLE dValue, HDOUBLE dFee, HCSTRR strSender,
			  HCSTRR strReceiver, HCSTRR strPublic,  HCSTRR strPrivate) {

    HN64 nValue = dValue * LKT_COIN;
    HN64 nFee = dFee * LKT_COIN;

    LOG(INFO) << "CMainChain::Transfer (value: " << nValue << ", fee: "
	      << nFee << ", sender: " << strSender << ", receiver: "
	      << strReceiver << ", private: " << strPrivate;


    HASSERT_THROW_MSG(nValue < MAX_TRAN_LKT, "too more transfer value", INVL_PARA);
    
    HASSERT_THROW_MSG(nFee < MAX_LKC_FEE, "too more transfer fee", INVL_PARA);

    HASSERT_THROW_MSG(nFee > MIN_LKC_FEE, "too less transfer fee", INVL_PARA);

    CLKAddress addrSender(strSender);

    CLKAddress addrReceiver(strReceiver);

    LOG(INFO) << "sender address: " << addrSender.ToString();

    LOG(INFO) << "receiver address: " << addrReceiver.ToString();

    HASSERT_THROW_MSG(addrSender.IsValid(), "sender address is invalid", INVL_PARA);

    HASSERT_THROW_MSG(addrReceiver.IsValid(), "receiver address is invalid", INVL_PARA);

    // make a entry
    CEntry newtx(nValue, nFee, addrSender, addrReceiver);
    if (not m_pWallet->Transfer(newtx, strPublic, strPrivate)) {

	LOG(ERROR) << "wallet transfer transaction {" << newtx.ToJsonString() << "}, hash: [" << newtx.GetEntryHash().ToString() << "], failed";
	return "";
	
    }

    // add to unblocked list;
    m_listUnentry.push_back(newtx);

    return MakeJsonReturn(newtx.GetEntryHash().ToString());

}


HSTR CMainChain::InnerTransfer(HDOUBLE dValue, HDOUBLE dFee, HCSTRR strSender, HCSTRR strReceiver) {
    
    HN64 nValue = dValue * LKT_COIN;
    HN64 nFee = dFee * LKT_COIN;

    LOG(INFO) << "CMainChain::InnerTransfer (value: " << nValue << ", fee: "
	      << nFee << ", sender: " << strSender  <<  ", receiver: "
	      << strReceiver;

    HASSERT_THROW_MSG(nValue < MAX_TRAN_LKT, "too more transfer value", INVL_PARA);
    
    HASSERT_THROW_MSG(nFee < MAX_LKC_FEE, "too more transfer fee", INVL_PARA);

    HASSERT_THROW_MSG(nFee > MIN_LKC_FEE, "too less transfer fee", INVL_PARA);

    CLKAddress addrSender(strSender);

    LOG(INFO) << "sender address: " << addrSender.ToString();

    HASSERT_THROW_MSG(addrSender.IsValid(), "sender address is invalid", INVL_PARA);

    CLKAddress addrReceiver(strReceiver);

    LOG(INFO) << "receiver address: " << addrReceiver.ToString();

    HASSERT_THROW_MSG(addrReceiver.IsValid(), "receiver address is invalid", INVL_PARA);

    // make a entry
    CEntry newtx;
    newtx.SetType(1);
    newtx.SetCreateTime(time(nullptr));
    newtx.SetSenderAddr(addrSender);
    newtx.SetReceiver(addrReceiver);
    newtx.SetValue(nValue);
    newtx.SetFee(nFee);

    if (not m_pWallet->InnerTransfer(newtx)) {

	LOG(ERROR) << "wallet transfer transaction {" << newtx.ToJsonString() << "}, hash: [" << newtx.GetEntryHash().ToString() << "], failed";
	return "";
	
    }

    // add to unblocked list;
    m_listUnentry.push_back(newtx);

    return MakeJsonReturn(newtx.GetEntryHash().ToString());

}


HRET CMainChain::initLast() {

    try {

	(void) verifyDatabase();

	(void) loadBlockChain();

	(void) loadWallet();

	displayBlockChain();
    
    } catch (...) {

    }

    m_isInited = HTRUE;

    HRETURN_OK;
    
}

HRET CMainChain::verifyDatabase() {

    LOG(INFO) << "Verifying database integrity...";

    m_strWalletDicFileName = GetValue(SO_DATA_DIR) + "/wallet.dat";

    if (not bitdb.Open (GetValue (SO_DATA_DIR))) {

	LOG(ERROR) << "Initializing wallet database environment failed";

	HRETURN(ERR_STATUS);

    }

    LOG(INFO) << "wallet file name: " << m_strWalletDicFileName;
    
    IF_TRUE(HCFileName::IsExists(m_strWalletDicFileName)) {

	LOG(INFO) << "wallet is exists, verify..";
	CDBEnv::VerifyResult r = bitdb.Verify(m_strWalletDicFileName, CWalletDB::Recover);

	if (r == CDBEnv::RECOVER_OK) {
	    LOG(INFO) << "Recover wallet.dat Success";
	}

	if (r == CDBEnv::RECOVER_FAIL) {
	    LOG(ERROR) <<"wallet.dat corrupt, salvage failed";
	    HRETURN(MISS_FILE);
	}

    }

    LOG(INFO) << "verifyDatabase end...";

    HRETURN_OK;

}


HRET CMainChain::loadBlockChain() {

    LOG(INFO) << "CMainChain::loadBlockChain...";

    CCoinTxdb ctxdb("cr+");

    if (not ctxdb.LoadBlockIndex()) {

	LOG(ERROR) << "ctxdb.LoadBlockIndex() failed";

	HRETURN(SRC_FAIL);
	
    }

    if (GetBlockIndexMap().empty()) {

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

HRET CMainChain::loadWallet () {

    LOG(INFO) << "CMainChain::loadWallet";

    m_pWallet = new CWallet(GetValue(SO_DATA_DIR) + "/wallet.dat");
    CHECK_NEWPOINT(m_pWallet);

    bool bFirstLoad = false;
    DBErrors cb = m_pWallet->LoadWallet(bFirstLoad);

    HASSERT_THROW (cb == DB_LOAD_OK, ERR_STATUS);

    if (bFirstLoad) {
	
	LOG(INFO) << "First load...";

	// make a new account
	CPubKey pubKey = m_pWallet->GenerateNewKey();

	CKeyID keyID = pubKey.GetID();

	CLKAddress addr;
	addr.Set(keyID);
	LOG(INFO) << addr.ToString();
	
    }

    m_pWallet->InitAccountBalance();
    m_pWallet->SetAccountsBalance();

    HRETURN_OK;

}


HRET CMainChain::Shutdown() {

    LOG(INFO) << "CMainChain shutdown...";

    return CChainCore::Shutdown();

}


CBlockIndex* CMainChain::InsertBlockIndex(const uint256 &hash) {

    if (hash == 0) {
	return nullptr;
    }

    // Return exists.
    BLOCKINDEXMAP::iterator fit = m_mapBlockIndex.find(hash);
    if (fit != m_mapBlockIndex.end()) {
	//LOG(INFO) << "find the index, need not insert";
	return fit->second;
    }
    
    // create new blockindex, and return.
    CBlockIndex* pIndexNew = new CBlockIndex();
    CHECK_NEWPOINT(pIndexNew);

    m_mapBlockIndex.insert(make_pair(hash, pIndexNew));

    return pIndexNew;
    
}


CBlock* CMainChain::GetNewBlock(HCSTRR strName) {

    LOG(INFO) << "CMainChain::GetNewBlock ...";

    CBlock* pret = new CBlock();
    CHECK_NEWPOINT(pret);

    pret->SetHashPrevBlock (GetHashBest());

    CLKAddress addr;
    m_pWallet->DefaultAddress (addr);
    LOG(INFO) << "default benifit address: " << addr.ToString();

    pret->SetTime(time(nullptr));

    pret->SetNonce (0);


    LOG(INFO) << "add entry to new block ...";

    // add entry
    // 1> add coinbase;
    // TODO: coinbase 10' lkt
    CEntry baseEntry;
    m_pWallet->MakeCoinBase(baseEntry, strName);

    LOG(INFO) << "Set Base Entry ...";
    pret->SetBaseEntry(baseEntry);

    // 2> add other txs;
    std::vector<CEntry> es;
    GetWillBlockEntrys(es);

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


void CMainChain::GetWillBlockEntrys(std::vector<CEntry>& entrys) {
    LOG(INFO) << __FUNCTION__ << " begin...";

    int ncount = 0;

    // TODO: entry list size
    for (UNENTRYLIST::iterator it = m_listUnentry.begin();
	 it != m_listUnentry.end() && ncount < 10; ++it) {

	const CEntry& cr = *it;

	entrys.push_back(cr);

	++ ncount;

    }


}


HINT CMainChain::GetTargetBits () {
    
    LOG(INFO) << "CMainChain::GetTargetBits begin...";

    CBigNum bnTargetLimit = m_params.GetPowLimit ();
    LOG(INFO) << "bnTargetLimit: " << bnTargetLimit.ToString();

    if (GetBestHeight() < 3) {
	LOG(INFO) << "height: " << GetBestHeight() << ", set lessest bits";

	return bnTargetLimit.GetCompact(); // genesis block

    }

    LOG(INFO) << "height: " << GetBestHeight() <<", will set new bits";
    
    const CBlockIndex* pindexPrev = GetLastBlockIndex(GetBestIndex());
    if (pindexPrev->GetPrevIndex() == nullptr) {
	//LOG(ERROR) << "CMainChain::SetTargetBits pindexPrev is nullptr";
	//LOG(ERROR) << "pindexPrev: [" << pindexPrev->ToString() << "]";
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
    if (m_mapBlockIndex.count(block_hash)) {

	LOG(ERROR) << "hash [" << block_hash.ToString() << "] is exists...";
	return false;
	
    }

    // TODO: if the block is not match the prev hash, add it to a banch list.
    // check prev hash == besthash
    const uint256& prev_hash = pbk->GetHashPrevBlock();
    if (prev_hash != GetHashBest()) {
	LOG(ERROR) << "error block to chain. best hash: [" <<
	    GetHashBest().ToString() << "], prev hash: [" << prev_hash.ToString()
		   << "]";
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
    LOG(INFO) << "block: " << pbk->ToJsonString();

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
    
    // save txs;
    m_pWallet->ProcessBlock(*pbk);

    // remove txs;
    removeBlockedEntrys(pbk->GetEntrys());

    LOG(INFO) << "CMainChain::ProcessBlock end...";

    return true;
}



bool CMainChain::AddToBlockIndex (CBlock& block, unsigned int nFile, unsigned int nBlockPos) {

    LOG(INFO) << "CMainChain::AddToBlockIndex...";
        // Check for duplicate
    uint256 hash = block.GetHash();

    LOG(INFO) << "AddToBlockIndex, File: " << nFile << ", position: " << nBlockPos << ", hash: [" << hash.ToString() << "]";

    // Construct new block index object
    CBlockIndex* pindexNew = new CBlockIndex(nFile, nBlockPos, block);
    CHECK_NEWPOINT(pindexNew);

    LOG(INFO) << "AddToBlockIndex, index information: (" << pindexNew->ToString() << ")";
    LOG(INFO) << "block index hash: " << pindexNew->GetHash().ToString();
    
    pindexNew->SetBlockHash (&hash);

    const map<uint256, CBlockIndex*>::iterator fit = m_mapBlockIndex.find(block.GetHashPrevBlock());

    if (fit != m_mapBlockIndex.end())
    {
	LOG(INFO) << "Find out prev block, prev hash: " << fit->second->GetHash().ToString();
	pindexNew->SetPrevIndex(fit->second);
	pindexNew->SetHeight(pindexNew->GetPrevIndex()->GetHeight() + 1);
    }

    LOG(INFO) << "AddToBlockIndex hash: [" << hash.ToString() << "], height: " << pindexNew->GetHeight(); 

    // Add to mapBlockIndex
    map<uint256, CBlockIndex*>::iterator it =  m_mapBlockIndex.insert(make_pair(hash, pindexNew)).first;

    pindexNew->SetBlockHash(&((*it).first));

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

    LOG(INFO) << "SetBestChain, height: " << pindexNew->GetHeight();

    uint256 hash = block.GetHash();

    LOG(INFO) << "laikelib will set new best block, hash: [" << hash.ToString() << "]";

    if (not txdb.TxnBegin()) {
	LOG(ERROR) << "CMainChain::SetBestChain txdb.TxnBegin() failed";
	return false;
    }

    const CBlock& const_gb = GenesisBlock();
    LOG(INFO) << "const block: [" << const_gb.ToJsonString()<< "]";
    LOG(INFO) << "genesis block hash: " << const_gb.GetHash().ToString();
    LOG(INFO) << "hash: " << hash.ToString() << ", genesis hash: " << GenesisBlock().GetHash().ToString();

    txdb.WriteHashBestChain(hash);

    if (not txdb.TxnCommit()) {

	    LOG(ERROR) << "SetBestChain() : TxnCommit failed";
	    return false;
	    
    }
    
    if (GetGenesisIndex() == nullptr && hash == GenesisBlock().GetHash()) {

	// make this as gensis block index.
	SetGenesisIndex(pindexNew);

	LOG(INFO) << "Set genesis block, hash: " << hash.ToString() <<", height: " << pindexNew->GetHeight ();
	
    } else if (pindexNew->GetHashPrevBlock() == m_hashBest) {
	// set new best, add the new block to the end of the chain.

	pindexNew->SetPrevNext();

	LOG(INFO) << "new block index information: " << pindexNew->ToString();
	
    }

    // new best block
    m_hashBest = hash;
    m_pBestIndex = pindexNew;
    m_nBestHeight = pindexNew->GetHeight();

    LOG(INFO) << "NOW hashBest: " << hash.ToString() << ", bestIndex: " << pindexNew << ", bestheight: " << pindexNew->GetHeight();

    return true;

}


void CMainChain::displayBlockChain() {

    LOG(INFO) << __FUNCTION__;

    LOG(INFO) << "wallet direction file name: [" << m_strWalletDicFileName << "]";

    LOG(INFO) << "genesis index pointer: " << m_pGenesisIndex << ", genesis hash: [" << m_pGenesisIndex->GetHash().ToString() << "]";

    LOG(INFO) << "genesis index string: [" << m_pGenesisIndex->ToString() << "]";
    
    LOG(INFO) << "best hash: [" << m_hashBest.ToString() << "]";

    LOG(INFO) << "best block index: " << m_pBestIndex << ", best block hash: [" << m_pBestIndex->GetHash().ToString() << "]";

    LOG(INFO) << "best height: [" << m_nBestHeight << "]";

    LOG(INFO) << "mapblockindex size: " << m_mapBlockIndex.size();

#if 0
    for (BLOCKINDEXMAP::const_iterator cfit = m_mapBlockIndex.begin(); cfit != m_mapBlockIndex.end(); ++cfit) {

	const uint256& hash = cfit->first;

	const CBlockIndex* p = cfit->second;

	LOG(INFO) << "height: [" << p->GetHeight() << "], block hash: [" << hash.ToString() << "]";

    }
#endif     

    //m_pWallet->displayWallet();
    
}


void CMainChain::removeBlockedEntrys (const std::vector<CEntry>& entrys) {

    LOG(INFO) << "CMainChain::removeBlockedEntrys begin entrys size: [" << m_listUnentry.size() << "]";
    for(size_t i = 0; i < entrys.size(); ++i) {

	const CEntry & cen = entrys[i];

	m_listUnentry.remove(cen);

    }

    LOG(INFO) << "CMainChain::removeBlockedEntrys end entrys size: [" << m_listUnentry.size() << "]";

}


void CMainChain::initBestIndexAndHeight () {

    CBlockIndex* pbi = m_mapBlockIndex[m_hashBest];
    if (pbi == nullptr) {

	LOG(ERROR) << "best blockindex is nullptr";
	return ;
	
    }

    m_nBestHeight = pbi->GetHeight ();

    m_pBestIndex = pbi;

}


CBlockIndex* CMainChain::GetBlockIndex(HUINT nHeight) {

    for(BLOCKINDEXMAP::iterator it = m_mapBlockIndex.begin();
	it != m_mapBlockIndex.end(); ++it) {

	CBlockIndex* pos = it->second;

	if (pos->GetHeight() == nHeight) {

	    return pos;

	}

    }

    return nullptr;
    
}
