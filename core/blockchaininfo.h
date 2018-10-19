/*****************************************************************************
 *
 * FileName: blockchaininfo.h
 *
 * Author: Tom Hui, tomhui1009@yahoo.com, 8613760232170
 *
 * Create Date: Sun Sep 30 14:29 2018
 *
 * Brief:
 *
 *
 *****************************************************************************/


#ifndef __BLOCKCHAININFO_H__
#define __BLOCKCHAININFO_H__


#include <huibase.h>

#include "blockindex.h"
#include "uint256.h"
#include "serialize.h"

using namespace HUIBASE;

class CLaikelibInfo {
 public:
    CLaikelibInfo () { }

    void SetVersion (HCSTRR strVersion) { m_strVersion = strVersion; }
    HCSTRR GetVersion () const { return m_strVersion; }

    void SetBestHash (HCSTRR strBestHash) { m_strBestHash = strBestHash; }
    HCSTRR GetBestHash () const { return m_strBestHash;  }

    void SetHeight (HINT nHeight) { m_nHeight = nHeight; }
    HINT GetHeight () const { return m_nHeight; }

    void SetGenesisHash (HCSTRR strGenesisHash) { m_strGenesisHash = strGenesisHash; }
    HCSTRR GetGenesisHash () const { return m_strGenesisHash; }

    HSTR ToString() const;

 public:
    IMPLEMENT_SERIALIZE(
                        READWRITE(m_strVersion);
                        READWRITE(m_strBestHash);
                        READWRITE(m_nHeight);
                        READWRITE(m_strGenesisHash);
                        )

 private:
    HSTR m_strVersion;
    HSTR m_strBestHash;
    HINT m_nHeight;
    HSTR m_strGenesisHash;
};


class CBlockChainInfo {
 public:
    CBlockChainInfo() { }

    CBlockIndex* GetGenesisIndex () { return m_pGenesisIndex; }

    const CBlockIndex* GetGenesisIndex () const { return m_pGenesisIndex; }

    const uint256& GetBestHash () const { return m_hashBest; }

    CBlockIndex* GetBestIndex () { return m_pBestIndex; }

    const CBlockIndex* GetBestIndex () const { return m_pBestIndex; }

    HINT GetCurrentHeight () const { if (m_pBestIndex != nullptr) { return m_pBestIndex->GetHeight(); } return 0; }

    const uint256& GetBestRootHash () const { return m_hashBestRoot; }

    void SetBestRootHash (const uint256& hashBestRoot) { m_hashBestRoot = hashBestRoot; }

    void SetGenesisIndex (CBlockIndex* index) { m_pGenesisIndex = index; }

    void SetBestHash (const uint256& hash) { m_hashBest = hash; }

    void SetBestIndex (CBlockIndex* index) { m_pBestIndex = index; }

    HSTR GetNodeInfo () const;

    HSTR GetVersionInfo () const;


 private:
    CBlockIndex* m_pGenesisIndex = nullptr;

    uint256 m_hashBest;

    CBlockIndex* m_pBestIndex = nullptr;

    uint256 m_hashBestRoot;
};


#endif //__BLOCKCHAININFO_H__
