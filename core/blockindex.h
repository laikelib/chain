/****************************************************************************
 *
 * File Name: blockindex.h
 *
 * Create Author: Tom Hui, tomhui1009@yahoo.com, 8613760232170
 *
 * Create Date: Sat Jan 20 20:19 2018
 *
 * Description:
 *	
 *
 ****************************************************************************/


#ifndef __BLOCKINDEX_H__
#define __BLOCKINDEX_H__


#include <huibase.h>
#include "serialize.h"
#include "uint256.h"
#include "blockheader.h"

using namespace HUIBASE;

class CBlockIndex: public CBlockHeader {
public:
    CBlockIndex();

    CBlockIndex(HUINT nFileIn, HUINT nBlockPosIn, const CBlockHeader& bh);

    const uint256& GetBlockHash () const { return *m_pHashBlock; }

    HUINT GetBlockTime () const { return GetTime(); }

    std::string ToString () const;

    void SetPrevNext () { m_pPrev->m_pNext = this; }

public:

    const CBlockIndex* GetPrevIndex() const { return m_pPrev; }
    const CBlockIndex* GetNextIndex() const { return m_pNext; }

    HUINT GetFileIn () const { return m_nFile; }
    HUINT GetBlockPos () const { return m_nBlockPos; }
    HUINT GetHeight () const { return m_nHeight; }
    HLLINT GetMoneySupply () const { return m_nMoneySupply; }

    const uint256& GetRoot () const { return m_hashRoot; }
    HUINT GetRootHeight () const { return m_nRootHeight; }
    bool IsMain () const { return m_nIsMain != 0;  }

    void SetRoot (const uint256& hashRoot ) { m_hashRoot = hashRoot; }
    void SetRootHeight (HUINT nRootHeight) { m_nRootHeight = nRootHeight; }
    void SetMain (bool bMain = true) { m_nIsMain = bMain ? 1 : 0; }

    void SetBlockHash (const uint256* pHash) { m_pHashBlock = pHash; }
    void SetPrevIndex (CBlockIndex* prev) { m_pPrev = prev; }
    void SetNextIndex (CBlockIndex* next) { m_pNext = next; }
    void SetFileIn (HUINT nFile) { m_nFile = nFile; }
    void SetBlockPos (HUINT nBlockPos) { m_nBlockPos = nBlockPos; }
    void SetHeight (HUINT nHeight) { m_nHeight = nHeight; }
    void SetMoneySupply (HLLINT nMoneySupply) { m_nMoneySupply = nMoneySupply; }

protected:
    const uint256* m_pHashBlock{nullptr};
    CBlockIndex* m_pPrev{nullptr};
    CBlockIndex* m_pNext{nullptr};
    
    HUINT m_nFile {0};
    HUINT m_nBlockPos {0};
    HUINT m_nHeight {0};

    HLLINT m_nMoneySupply {0};

    uint256 m_hashRoot {0};
    HUINT m_nRootHeight {0};
    HINT m_nIsMain{0};
};


class CDiskBlockIndex : public CBlockIndex {
public:
    uint256 hashBlock;
    uint256 hashPrev;
    uint256 hashNext;

    CDiskBlockIndex() {

	hashBlock = hashPrev = hashNext = 0;
	
    }

    explicit CDiskBlockIndex (CBlockIndex* pIndex)
	: CBlockIndex(*pIndex) {

	hashBlock = (pIndex ? pIndex->GetHash() : 0);

	hashPrev = (m_pPrev ? m_pPrev->GetHash () : 0);

	hashNext = (m_pNext ? m_pNext->GetHash () : 0);
	
    }

    IMPLEMENT_SERIALIZE(
			READWRITE(m_nVersion);
			READWRITE(m_nType);
			READWRITE(m_hashPrevBlock);
			READWRITE(m_hashMerkleRoot);
			READWRITE(m_nTime);
			READWRITE(m_nBits);
			READWRITE(m_nNonce);
			READWRITE(m_nCount);

			READWRITE(hashBlock);
			READWRITE(hashPrev);
			READWRITE(hashNext);			
			READWRITE(m_nFile);
			READWRITE(m_nBlockPos);
			READWRITE(m_nHeight);
			READWRITE(m_nMoneySupply);

            READWRITE(m_hashRoot);
            READWRITE(m_nRootHeight);
            READWRITE(m_nIsMain);
                        )


};


const CBlockIndex* GetLastBlockIndex (const CBlockIndex* pindex) ;



#endif //__BLOCKINDEX_H__


