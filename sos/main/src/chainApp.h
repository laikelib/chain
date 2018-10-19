
#ifndef __CHAIN_APP_H__
#define __CHAIN_APP_H__

#include <huibase.h>
#include <hsingleton.hpp>
#include <chainCore.h>
#include <list>

#include <uint256.h>
#include <blockindex.h>
#include <lktmap.h>
#include <blockchaininfo.h>
#include <balmap.h>
#include <entrymng.h>
#include <entryhistory.h>

#include "mainParams.h"
#include "cointxdb.h"

using namespace HUIBASE;

class CMainChain : public CChainCore {
public:
    using UNENTRYLIST = std::list<CEntry>;

public:
    CMainChain ();

    ~CMainChain ();

public:
    // main interfaces
    HSTR GetBlockTemplate (HCSTRR str);

    HSTR PostWork(HCSTRR str);

    HSTR GetHeight ();

    HSTR HeightBlock (HCSTRR str);

    HSTR GetBal (HCSTRR strAddr);

    HSTR HashTx (HCSTRR strHash);

    HSTR LkInfo ();

    HSTR newTx (HCSTRR str);

 private:
    virtual HRET initLast ();

    HRET loadBlockChain ();

    HRET checkBlockChain();

    HRET initHistory ();

    HRET initBalance ();

    HRET checkBalance () const;

 public:

    HRET Shutdown ();

    CBlockIndex* InsertBlockIndex (const uint256& hash);

    CBlock* GetNewBlock (HCSTRR strName);

    HINT GetTargetBits ();

    bool chainCheckBlock (const CBlock* pbk) const;

public:

    bool AcceptBlock (CBlock* pbk);

    bool ProcessBlock (CBlock* pbk);

    // get genesis information
    uint256 GetGenesisBlockHash () const { return m_params.GetGenesisBlockHash(); }


    const CBlock& GenesisBlock () const { return m_params.GenesisBlock (); }

    void SetGenesisIndex (CBlockIndex* pGenesisIndex) {
        m_info.SetGenesisIndex(pGenesisIndex);
    }

    const CBlockIndex* GetGenesisIndex () const {
        return m_info.GetGenesisIndex();
    }

public:
    void initBestIndex ();

    HBOOL IsBlockIndexEmpty() const { return m_mapBlockIndex.IsEmpty(); }

    HBOOL HasBlock (const uint256& hash) const { return m_mapBlockIndex.HasBlock(hash); }

    void SetHashBest (const uint256& hashBest) {
        m_info.SetBestHash(hashBest);
    }

    const uint256& GetHashBest () const {
        return m_info.GetBestHash();
    }

    void SetBestIndex (CBlockIndex* pBestIndex) {
        m_info.SetBestIndex(pBestIndex);
    }

    const CBlockIndex* GetBestIndex () const {
        return m_info.GetBestIndex();
    }

    const uint256& GetHashBestRoot () const {

        return m_info.GetBestRootHash ();

    }

    void SetHashBestRoot (const uint256& hashBestRoot) {

        m_info.SetBestRootHash(hashBestRoot);

    }

    HINT GetBestHeight () const { return m_info.GetCurrentHeight(); }

    HRET SetIndexHeight (const uint256& hash, HUINT nHeight);

    bool AddToBlockIndex (CBlock& block, unsigned int nFile, unsigned int nBlockPos);

    bool SetBestChain (CCoinTxdb& txdb, CBlock& block, CBlockIndex* pindexNew);

 private:
    void displayBlockChain ();

    CBlockIndex* GetBlockIndex (HUINT nHeight);

    CEntry CreateCoinBase (const CLKAddress& addr) const;

    bool checkNewEntry (const CEntry& new_en);

 private:
    HBOOL m_isInited;

    CMainParams m_params;

    CBlockIndexMap m_mapBlockIndex;

    CBlockChainInfo m_info;

    CBalMng m_mngBal;

    CEntryMng m_mngEntrys;

    CEntryHistory m_history;
};


typedef HCSingleton<CMainChain> sgt_config;

#ifndef so_config
#define so_config sgt_config::Instance()
#endif 



#endif // __CHAIN_APP_H__

