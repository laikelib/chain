

#include <huibase.h>
#include <hfname.h>
#include <chainApp.h>
#include <glog/logging.h>
#include <wallet.h>
#include <txdb-leveldb.h>

#include "cointxdb.h"

using namespace HUIBASE;

CMainChain::CMainChain ()
    : m_isInited(HFALSE){


}


CMainChain::~CMainChain () {


}

HRET CMainChain::initLast() {

    try {

	(void) verifyDatabase();

	(void) loadBlockChain();
    
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

	CDBEnv::VerifyResult r = bitdb.Verify(m_strWalletDicFileName, CWalletDB::Recover);

	if (r == CDBEnv::RECOVER_OK) {
	    LOG(INFO) << "Recover wallet.dat Success";
	}

	if (r == CDBEnv::RECOVER_FAIL) {
	    LOG(ERROR) <<"wallet.dat corrupt, salvage failed";
	    HRETURN(MISS_FILE);
	}

    }

    HRETURN_OK;

}


HRET CMainChain::loadBlockChain() {

    LOG(INFO) << "CMainChain::loadBlockChain...";

    CCoinTxdb ctxdb("cr+");

    if (not ctxdb.LoadBlockIndex()) {

	LOG(ERROR) << "ctxdb.LoadBlockIndex() failed";

	HRETURN(SRC_FAIL);
	
    }
    

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
	return fit->second;
    }
    
    // create new blockindex, and return.
    CBlockIndex* pIndexNew = new CBlockIndex();
    CHECK_NEWPOINT(pIndexNew);

    m_mapBlockIndex.insert(make_pair(hash, pIndexNew));

    return pIndexNew;
    
}

CBlockIndex* CMainChain::GetBlockIndex (const uint256& hash) {

    BLOCKINDEXMAP::iterator fit = m_mapBlockIndex.find(hash);

    if (fit == m_mapBlockIndex.end()) {
	return nullptr;
    }

    return fit->second;
}








