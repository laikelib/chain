/****************************************************************************
 *
 * File Name: blockheader.h
 *
 * Create Author: Tom Hui, tomhui1009@yahoo.com, 8613760232170
 *
 * Create Date: Thu Jan 18 21:19 2018
 *
 * Description:
 *	
 *
 ****************************************************************************/
#ifndef __BLOCKHEADER_H__
#define __BLOCKHEADER_H__

#include "base58.h"
#include "uint256.h"

class CBlockHeader {
public:
    CBlockHeader();

    //CBlockHeader(const CBlockHeader& bh);

public:
    string GetHeaderJson () const;

    string GetHeaderBuf () const;

    uint256 TryHash ();
    
    HINT GetVersion () const { return m_nVersion; }

    HINT GetType () const { return m_nType; }

    const uint256& GetHashPrevBlock () const { return m_hashPrevBlock; }

    const uint256& GetMerkleRoot () const { return m_hashMerkleRoot; }

    HUINT GetTime () const { return m_nTime; }

    HINT GetBits () const { return m_nBits; }

    HUINT GetNonce () const { return m_nNonce; }

    HUINT GetCount () const { return m_nCount; }

    uint256 GetHash () const;

    void SetVersion (HINT nVersionIn) { m_nVersion = nVersionIn; }

    void SetType (HINT nType) { m_nType = nType; }

    void SetHashPrevBlock (const uint256& hashPrevBlock) { m_hashPrevBlock = hashPrevBlock; }

    void SetHashMerkleRoot (const uint256& hashMerkleRoot) { m_hashMerkleRoot = hashMerkleRoot; }

    void SetTime (HUINT nTime) { m_nTime = nTime; }

    void SetBits (HINT nBits) { m_nBits = nBits; }

    void SetNonce (HUINT nNonce) { m_nNonce = nNonce; }

    void SetCount (HUINT nCount) { m_nCount = nCount; }
    
protected:
    HINT m_nVersion{0};
    HINT m_nType{0};
    uint256 m_hashPrevBlock{0};
    uint256 m_hashMerkleRoot{0};
    HUINT m_nTime{0};
    HINT m_nBits{0};
    HUINT m_nCount{0};
    HUINT m_nNonce{0};

};

#endif //__BLOCKHEADER_H__











