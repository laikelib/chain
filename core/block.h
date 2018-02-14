/****************************************************************************
 *
 * File Name: block.h
 *
 * Create Author: Tom Hui, tomhui1009@yahoo.com, 8613760232170
 *
 * Create Date: Thu Jan 18 21:19 2018
 *
 * Description:
 *	
 *
 ****************************************************************************/
#ifndef __BLOCK_H__
#define __BLOCK_H__

#include "blockheader.h"
#include "blockindex.h"
#include "entry.h"


FILE* OpenBlockFile(unsigned int nFile, unsigned int nBlockPos, const char* pszMode="rb");
FILE* AppendBlockFile(unsigned int& nFileRet);

class CBlock : public CBlockHeader  {
public:
    CBlock ();

    bool WriteToDisk (unsigned int& nFileRet, unsigned int& nBlockPosRet);

    bool ReadFromDisk (const CBlockIndex* pindex);
    bool ReadFromDisk (unsigned int nFile, unsigned int nBlockPos);

    void SetBaseEntry (const CEntry& entry);

    bool BlockEntry (const CEntry& entry);

    bool BlockEntrys (const std::vector<CEntry> & entrys);

    bool HasBaseEntry () const;

    bool CheckBlock () const;

    const CEntry& GetBaseEntry () const;

    void AddFees (HN64 nFee);

    uint256 MakeMerkleRoot () const;
    bool BuildMerkleRootTree ();

    string ToJsonString () const;

    string GetBlockJson () const;

    bool FromJson (const string& strJson);

    HUINT GetEntrysSize () const { return m_entrys.size(); }

    bool CheckCoinBase () const;

    bool CheckEntrys () const;

    bool CheckMerkleRoot () const;

    const std::vector<CEntry>& GetEntrys () const;

    const CLKAddress& GetCreateBy () const;

    HN64 GetAmount () const;

private:
    bool CheckPow () const;

public:
    IMPLEMENT_SERIALIZE(
			READWRITE(m_nVersion);
			READWRITE(m_nType);
			READWRITE(m_hashPrevBlock);
			READWRITE(m_hashMerkleRoot);
			READWRITE(m_nTime);
			READWRITE(m_nBits);
			READWRITE(m_nCount);
			READWRITE(m_nNonce);
			READWRITE(m_entrys);
)

private:
    std::vector<CEntry> m_entrys;
	
};



#endif //__BLOCK_H__










