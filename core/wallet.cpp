
#include <huibase.h>
#include <hstr.h>
#include "db.h"
#include "wallet.h"
#include "util.h"
#include <glog/logging.h>

using namespace HUIBASE;

bool CWalletDB::Recover(CDBEnv &dbenv, std::string strFileName) {

    return CWalletDB::Recover(dbenv, strFileName, false);
    
}


bool CWalletDB::Recover(CDBEnv &dbenv, std::string strFileName, bool bOnlyKeys) {

    LOG(INFO) << "Recover wallet: " << strFileName;

    (void) bOnlyKeys;

    int64_t now = time(nullptr);
    std::string newFilename = HCStr::Format("wallet.%d.bak", now);

    int result = dbenv.dbenv.dbrename(NULL, strFileName.c_str(), NULL,
                                      newFilename.c_str(), DB_AUTO_COMMIT);
    if (result == 0){
	LOG(INFO) << "Renamed " << strFileName << " to " << newFilename;
    }
    else
    {
	LOG(FATAL) << "Failed to rename " << strFileName << " to " << newFilename;
        return false;
    }

    std::vector<CDBEnv::KeyValPair> salvagedData;
    bool allOK = dbenv.Salvage(newFilename, true, salvagedData);
    if (salvagedData.empty())
    {
	LOG(FATAL) << "Salvage(aggressive) found no records in " << newFilename;
        return false;
    }
    LOG(INFO) << "Salvage(aggressive) found " << salvagedData.size() << " records";

    bool fSuccess = allOK;
    Db* pdbCopy = new Db(&dbenv.dbenv, 0);
    int ret = pdbCopy->open(NULL,                 // Txn pointer
                            strFileName.c_str(),   // Filename
                            "main",    // Logical db name
                            DB_BTREE,  // Database type
                            DB_CREATE,    // Flags
                            0);
    if (ret > 0)
    {
	LOG(FATAL) << "Cann't create database file " << strFileName;
        return false;
    }

    pdbCopy->close(0);
    delete pdbCopy;

    return fSuccess;
}

DBErrors CWalletDB::LoadWallet(CWallet* pWallet) {

    DBErrors ret = DB_LOAD_OK;
    LOG(INFO) << "CWalletDB::LoadWallet...";

    try {

	int nVersion = 0;

	if (Read(string("version"), nVersion)) {

	    if (nVersion > WALLET_VERSION) {
		return DB_TOO_NEW;
	    }

	    LOG(INFO) << "Wallet Version: " << nVersion;

	    pWallet->SetWalletVersion(nVersion);
	    
	}

	Dbc* pcursor = GetCursor ();
	if (not pcursor) {

	    LOG(ERROR) << "error getting wallet database cursor";
	    return DB_CORRUPT;
	    
	}

	while (true) {

	    CDataStream dsKey (SER_DISK, NODE_VERSION);
	    CDataStream dsVal (SER_DISK, NODE_VERSION);
	    int ret = ReadAtCursor (pcursor, dsKey, dsVal);

	    if (ret == DB_NOTFOUND) {

		LOG(INFO) << "not found any data in wallet.";
		break;

	    } else if (ret != 0) {
		
		LOG(ERROR) << "error reading next record from wallet database";
		return DB_CORRUPT;
		    
	    }

	    std::string strType;
	    // read from bdb.
	    if (not ReadKeyValue(pWallet, dsKey, dsVal, strType)) {

		ret = DB_CORRUPT;
		
	    }
	    
	}

	pcursor->close();
	

    } catch(...) {

	ret = DB_CORRUPT;
	
    }

    if (ret != DB_LOAD_OK) {
	LOG(ERROR) << "read wallet database failed";
	return ret;
    }

    LOG(INFO) << "CWalletDB finished load wallet";
    return ret;

}


bool CWalletDB::ReadKeyValue(CWallet *pwallet, CDataStream &dsKey, CDataStream &dsVal, string &strType) {

    try {

	dsKey >> strType;

	if (strType == "entry") {

	    uint256 hash;
	    dsKey >> hash;

	    CEntry entry;
	    dsVal >> entry;

	    pwallet->AddEntry(hash, entry);

	} else if (strType == "key") {

	    //LOG(INFO) << "read a key";
	    CPubKey pubkey;
	    dsKey >> pubkey;

	    /*
	    CLKAddress addr;
	    addr.Set(pubkey.GetID());

	    LOG(INFO) <<"address: " << addr.ToString();*/

	    if (not pubkey.IsValid()) {
		LOG(ERROR) << "Error reading wallet database: CPubkey corrupt";
		return false;
	    }

	    CPrivKey prikey;
	    dsVal >> prikey;

	    CKey key;
	    if (not key.Load(prikey, pubkey, true)) {

		LOG(ERROR) << "Errror reading wallet database: CPrivate corrupt";
		return false;
		
	    }

	    if (not pwallet->LoadKey(key, pubkey)) {

		LOG(ERROR) << "Error reading wallet database: LoadKey failed";
		return false;
		
	    }
	    
	}

    } catch (...) {
	
	return false;
	
    }

    return true;
    
}


bool CWalletDB::WriteKey(const CPubKey &pubKey, const CPrivKey &priKey) {

    std::vector<unsigned char> vchkey;
    vchkey.reserve(pubKey.size() + priKey.size());
    vchkey.insert(vchkey.end(), pubKey.begin(), pubKey.end());
    vchkey.insert(vchkey.end(), priKey.begin(), priKey.end());

    return Write(std::make_pair(string("key"), pubKey),
		 std::make_pair(priKey, string("key_end")));
    
}


bool CWalletDB::WriteEntry(const uint256 &hash, const CEntry &entry) {

    return Write(std::make_pair(string("entry"), hash), entry);

}


CWallet::CWallet() {
    SetNull();
}


CWallet::CWallet (const string & strFileName)
    : m_strWalletFileName(strFileName){
    SetNull();

    m_bFileBacked = true;
}


bool CWallet::InnerTransfer (CEntry& en) {


    ACCOUNT_BAL::const_iterator cfit = m_account_bal.find(en.GetSender());
    
    HN64 nUserBal = cfit->second;
    if (en.GetAmount() > nUserBal) {

	LOG(ERROR) << "too more value than balance, value: [" << en.GetValue()
		   << "], fee: [" << en.GetFee() << "], user balance: ["
		   << nUserBal << "]";

	return false;

    }

    LOG(INFO) << "transaction: " << en.ToJsonString();

    CKey ckey = GetKey(en.GetSender());

    if (not en.Signature(ckey)) {

	LOG(ERROR) << "signture transaction failed";
	return false;

    }

    LOG(INFO) << "signature success";

    if (not en.VerifySign()) {

	LOG(ERROR) << "verify transaction failed";
	return false;

    }

    LOG(INFO) << "verifysign success";

    return true;
}


bool CWallet::Transfer (CEntry& en, HCSTRR strPublic, HCSTRR strPrivate) {

    // check sign
    // TODO: signture at client side.
    CPrivKey prikey = DecodeBase64(strPrivate.c_str());
    std::vector<unsigned char> uvc = DecodeBase64(strPublic.c_str());
    CPubKey pubkey(uvc);

    LOG(INFO) << "sign information: {public: [" << strPublic << "], private: ["
	      << strPrivate << "]}";
    
    CKey key;
    if (not key.Load(prikey, pubkey, true)) {

	LOG(ERROR) << "CKey load private, public failed";
	return false;

    }

    CLKAddress _addr;
    CPubKey _pub = key.GetPubKey();
    _addr.Set(_pub.GetID());
    if (not (_addr == en.GetSender())) {

	LOG(ERROR) << "sender addr: [" << en.GetSender().ToString() << "], sign addr: [" << _addr.ToString() << "]";
	return false;

    }

    LOG(INFO) << "check sign success! sender addr:[" << en.GetSender().ToString() <<"], sign addr: [" << _addr.ToString() << "]";

    // TODO: use can send money anywhere.
    ACCOUNT_BAL::const_iterator cfit = m_account_bal.find(en.GetSender());
    if (cfit == m_account_bal.end()) {

	LOG(ERROR) << "cann't find the user balance";
	return false;

    }

    HN64 nUserBal = cfit->second;
    if (en.GetAmount() > nUserBal) {

	LOG(ERROR) << "too more value than balance, value: [" << en.GetValue()
		   << "], fee: [" << en.GetFee() << "], user balance: ["
		   << nUserBal << "]";
	return false;

    }

    LOG(INFO) << "transaction: " << en.ToJsonString();

    if (not en.Signature(key)) {

	LOG(ERROR) << "signture transaction failed";
	return false;

    }

    // try to verify transaction.
    if (not en.VerifySign()) {

	LOG(ERROR) << "try to verify transaction failed";
	return false;

    }

    LOG(INFO) << "transaction: " << en.ToJsonString();
    
    return true;
}


void CWallet::GetTxs(const CLKAddress &addr, std::vector<CEntry>& entrys) const {

    std::vector<std::pair<HINT, CEntry> > sort_vs;

    ACCOUNT_TXS::const_iterator cfit = m_account_ins.find(addr);
    if (cfit != m_account_ins.end()) {
	const std::set<uint256>& in_256 = cfit->second;

	for(std::set<uint256>::const_iterator cs = in_256.begin();
	    cs != in_256.end(); ++cs) {

	    const uint256& ini = *cs;

	    std::map<uint256, CEntry>::const_iterator cc = m_entrys.find(ini);

	    if (cc == m_entrys.end()) {
		LOG(ERROR) << "[" << ini.ToString() << "] is not in m_entrys";
		continue;
	    }
		
	    const CEntry& cen = cc->second;

	    sort_vs.push_back(make_pair(cen.GetCreateTime(), cen));
	    
	}

    }


    ACCOUNT_TXS::const_iterator cfit2 = m_account_outs.find(addr);
    if (cfit2 != m_account_outs.end()) {
	
	const std::set<uint256>& out_256 = cfit2->second;

	for(std::set<uint256>::const_iterator cs = out_256.begin();
	    cs != out_256.end(); ++cs) {

	    const uint256& ini = *cs;

	    std::map<uint256, CEntry>::const_iterator cc = m_entrys.find(ini);

	    if (cc == m_entrys.end()) {
		LOG(ERROR) << "[" << ini.ToString() << "] is not in m_entrys";
		continue;
	    }
		
	    const CEntry& cen = cc->second;

	    sort_vs.push_back(make_pair(cen.GetCreateTime(), cen));
	    
	}

    }

    sort(sort_vs.begin(), sort_vs.end());

    entrys.clear();

    for (size_t i = 0; i < sort_vs.size(); ++i) {

	std::pair<HINT, CEntry>& item = sort_vs[i];

	entrys.push_back(item.second);

    }

}


HN64 CWallet::GetAccountBal (const CLKAddress& addr)  const {

    ACCOUNT_BAL::const_iterator cfit = m_account_bal.find(addr);

    if (cfit == m_account_bal.end()) {
	return 0;
    }

    return cfit->second;

}


CKey CWallet::GetKey (const CLKAddress& addr) const {

    for (KeyMap::const_iterator cit = mapKeys.begin();
	 cit != mapKeys.end(); ++cit) {

	const CKey& ckey = cit->second;

	CLKAddress _add;
	_add.Set(ckey.GetPubKey().GetID());

	if (_add == addr) {

	    return ckey;

	}

    }

    HASSERT_THROW_MSG(false, "cann't find key associte addr", INDEX_OUT);

    CKey __ret;

    return __ret;

}



CPrivKey CWallet::GetPrivate (const CPubKey& pub) const {

    KeyMap::const_iterator fit = mapKeys.find(pub.GetID());

    HASSERT_THROW_MSG(fit != mapKeys.end(), "get private failed", EX_GET);

    const CKey& ckey = fit->second;
    
    return ckey.GetPrivKey();
}


void CWallet::displayWallet() {

    LOG(INFO) << "display user transaction...";

    for (ACCOUNT_TXS::const_iterator cit = m_account_ins.begin();
	 cit != m_account_ins.end(); ++cit ) {

	const CLKAddress& addr = cit->first;

	const std::set<uint256>& txs = cit->second;

	LOG(INFO) << "addr [" << addr.ToString() << "], has "
		  << txs.size() << " txs";
#if 0	
	for(std::set<uint256>::const_iterator csit = txs.begin();
	    csit != txs.end(); ++csit) {

	    const uint256& tid = *csit;

	    const CEntry& cet = m_entrys[tid];

	    LOG(INFO) << addr.ToString() << ": " << cet.GetValue() << "["
		      << tid.ToString() << "]";

	}
#endif 	

    }

}


void CWallet::ProcessBlock(const CBlock& block) {

    LOG(INFO) << "CWallet::ProcessBlock begin...";

    const std::vector<CEntry> & entrys = block.GetEntrys();

    for (size_t i = 0; i < entrys.size(); ++i) {

	const CEntry& cen = entrys[i];

	if(isWithMe(cen)) {

	    m_entrys.insert(std::map<uint256, CEntry>::value_type(cen.GetEntryHash(), cen));

	    CWalletDB(m_strWalletFileName).WriteEntry(cen.GetEntryHash(), cen);

	}




    }

}


void CWallet::SetNull() {

    m_nWalletVersion = WALLET_VERSION;
    
    m_bFileBacked = false;

}


bool CWallet::isWithMe(const CEntry &cen) const {

    if (not cen.IsBaseSign()) {
	const CLKAddress& saddr = cen.GetSender();
	if (IsWithMe(saddr)) {
	    return true;
	}

    }

    const CLKAddress & raddr = cen.GetReceiver();
    if (IsWithMe(raddr)) {
	return true;
    }

    return false;
}


DBErrors CWallet::LoadWallet(bool& bFirstLoad) {

    LOG(INFO) << "CWallet::LoadWallet, Wallet File: " << m_strWalletFileName;

    bFirstLoad = false;

    if (not m_bFileBacked) {
	
	LOG(ERROR) << "m_bFileBacked is faled";
	return DB_LOAD_OK;
	
    }

    DBErrors cb = CWalletDB(m_strWalletFileName, "cr+").LoadWallet(this);

    if (IsFirstLoad()) {
	LOG(INFO) << "first load...";
	bFirstLoad = true;
    }

    return cb;
}

CPubKey CWallet::GenerateNewKey() {

    RandAddSeed();

    CKey secret;
    secret.MakeNewKey(true);
    CPubKey pubkey = secret.GetPubKey();

    HASSERT_THROW(AddKeyPubKey(secret,pubkey), SRC_FAIL);

    CWalletDB(m_strWalletFileName).WriteKey(pubkey, secret.GetPrivKey());

    return pubkey;
    
}


void CWallet::DefaultAddress(CLKAddress& addr ) const{

    assert(not mapKeys.empty());

    KeyMap::const_iterator cit = mapKeys.cbegin();

    addr.Set(cit->first);

}


void CWallet::MakeCoinBase(CEntry &entry, HCSTRR strAddr) const{

    entry.SetType (NODE_VERSION);

    entry.SetCreateTime (time(nullptr));

    entry.ClearSender();

    CLKAddress addr(strAddr);

    entry.SetReceiver (addr);
 
    // TODO: coinbase is 10 LKT;
    entry.SetValue (10 * LKT_COIN);

    entry.SetFee(0);
}


void CWallet::InitAccountBalance () {

    LOG(INFO) << "CWallet::InitAccountBalance begin...";

    for (KeyMap::const_iterator cit = mapKeys.begin();
	 cit != mapKeys.end(); ++cit) {

	const CKeyID& kid = cit->first;
	    
	CLKAddress addr;
	addr.Set(kid);

	m_account_bal[addr] = 0;

    }

    // display accounts
    /*
    for (ACCOUNT_BAL::const_iterator it = m_account_bal.begin();
	 it != m_account_bal.end(); ++it) {

	const CLKAddress& addr = it->first;
	HN64 val = it->second;

	LOG(INFO) << "account [" << addr.ToString() << "], value: [" << val << "]";

	}*/

}


void CWallet::SetAccountsBalance() {

    LOG(INFO) << "CWallet::SetAccountBalance begin...";

    for (std::map<uint256, CEntry>::const_iterator it = m_entrys.begin();
	 it != m_entrys.end(); ++it) {

	const uint256& hash = it->first;

	const CEntry& en = it->second;

	if (en.GetSender().IsValid()) {

	    AddOut(en.GetSender(), hash);
	    
	}


	if (en.GetReceiver().IsValid()) {

	    AddIn(en.GetReceiver(), hash);

	}

    }


    // change account value
    ChangeValForTxs(m_account_ins);

    ChangeValForTxs(m_account_outs, false);

    // display accounts
    /*
    for (ACCOUNT_BAL::const_iterator it = m_account_bal.begin();
	 it != m_account_bal.end(); ++it) {

	const CLKAddress& addr = it->first;
	HN64 val = it->second;

	LOG(INFO) << "account [" << addr.ToString() << "], value: [" << val << "]";

	}*/

}

void CWallet::ChangeValForTxs (const ACCOUNT_TXS& txs, bool isAdd) {
    
    for (ACCOUNT_TXS::const_iterator cit = txs.begin();
	 cit != txs.end(); ++cit) {

	const CLKAddress& addr = cit->first;
	const std::set<uint256> & txs = cit->second;

	HN64 val = 0;
	for (std::set<uint256>::const_iterator cs = txs.begin();
	     cs != txs.end(); ++cs) {

	    const uint256& hash = *cs;

	    const CEntry& en = m_entrys[hash];

	    val += en.GetValue();

	}

	ChangeAccountValue(addr, val, isAdd);

    }
    
}


void CWallet::AddIn(const CLKAddress& addr, const uint256& hash) {

    AddTx(m_account_ins, addr, hash);

}


void CWallet::AddOut(const CLKAddress& addr, const uint256& hash) {

    AddTx(m_account_outs, addr, hash);
    
}


void CWallet::AddTx(ACCOUNT_TXS& old_txs, const CLKAddress& addr,
		    const uint256& hash) {

    ACCOUNT_TXS::iterator cfit = old_txs.find(addr);

    if (cfit == old_txs.end()) {

	std::set<uint256> sets;
	sets.insert (hash);

	old_txs.insert(std::map<CLKAddress, std::set<uint256> >::value_type(addr, sets));

    } else {

	std::set<uint256>& sets = cfit->second;

	sets.insert(hash);

    }

}


void CWallet::ChangeAccountValue(const CLKAddress &addr, HN64 val, bool isAdd) {

    //LOG(INFO) << "CWallet::ChangeAccountVaue";

    ACCOUNT_BAL::iterator fit = m_account_bal.find(addr);

    if (fit == m_account_bal.end()) {
	
	if (isAdd) {

	    // input
	    m_account_bal.insert(ACCOUNT_BAL::value_type(addr, val));

	} else {

	    // output
	    HASSERT_THROW_MSG(false, "not this account, but output", LOGIC_ERR);
		
	}

    } else {

	HN64& old_val = fit->second;

	if (isAdd) {

	    old_val += val;

	} else {

	    HASSERT_THROW_MSG(old_val > val, "val is bigger than old_val", LOGIC_ERR);

	    old_val -= val;

	}
	
    }
    

}


bool CWallet::LoadKey(const CKey& key, const CPubKey &pubKey) {

    return CBasicKeyStore::AddKeyPubKey(key, pubKey);
    
}


void CWallet::AddEntry(const uint256 &hash, const CEntry &entry) {

    std::map<uint256, CEntry>::const_iterator cfit = m_entrys.find(hash);

    if (cfit != m_entrys.end()) {

	LOG(ERROR) << hash.ToString() << " is exists";
	return;

    }


    m_entrys.insert(std::map<uint256, CEntry>::value_type(hash, entry));

}

