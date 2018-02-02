
#ifndef __CHAIN_APP_H__
#define __CHAIN_APP_H__

#include <huibase.h>
#include <hsingleton.hpp>
#include <chainCore.h>
#include <db.h>

#include <uint256.h>
#include <blockindex.h>

#include "mainParams.h"

using namespace HUIBASE;

class CMainChain : public CChainCore {
public:
    using BLOCKINDEXMAP = std::map<uint256, CBlockIndex*>;
    
public:
    CMainChain ();

    ~CMainChain ();

    virtual HRET initLast ();

    HRET verifyDatabase ();

    HRET loadBlockChain ();

    HRET loadWallet ();

    HRET Shutdown ();

    CBlockIndex* InsertBlockIndex (const uint256& hash);

public:
    // get genesis information
    uint256 GetGenesisBlockHash () const { return m_params.GetGenesisBlockHash(); }

    
    const CBlock& GenesisBlock () const { return m_params.GenesisBlock (); }

    void SetGenesisIndex (CBlockIndex* pGenesisIndex) {
	m_pGenesisIndex = pGenesisIndex;
    }

    const CBlockIndex* GetGenesisIndex () const {
	return m_pGenesisIndex; 
    }

public:
    // blockchain 
    const BLOCKINDEXMAP& GetBlockIndexMap () const { return m_mapBlockIndex; }

    CBlockIndex* GetBlockIndex (const uint256& hash);

    void SetHashBest (const uint256& hashBest) {
	m_hashBest = hashBest;
    }

    const uint256& GetHashBest () const {
	return m_hashBest;
    }

    void SetBestIndex (CBlockIndex* pBestIndex) {
	m_pBestIndex = pBestIndex;
    }

    const CBlockIndex* GetBestIndex () const {
	return m_pBestIndex;
    }

    void SetBestHeight (HINT nHeight) { m_nBestHeight = nHeight; }

    HINT GetBestHeight () const { return m_nBestHeight; }
    
private:
    HBOOL m_isInited;

    HSTR m_strWalletDicFileName;

    BLOCKINDEXMAP m_mapBlockIndex;

    CMainParams m_params;

    CBlockIndex* m_pGenesisIndex = nullptr;

    uint256 m_hashBest;

    CBlockIndex* m_pBestIndex = nullptr;

    HINT m_nBestHeight = 0;
};


typedef HCSingleton<CMainChain> sgt_config;

#ifndef so_config
#define so_config sgt_config::Instance()
#endif 



#endif // __CHAIN_APP_H__

