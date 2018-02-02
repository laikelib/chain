
#include <hstr.h>
#include "blockindex.h"


CBlockIndex::CBlockIndex() {

    m_pHashBlock = nullptr;

    m_pPrev = m_pNext = nullptr;

    m_nFile = 0;

    m_nBlockPos = 0;

    m_nHeight = 0;

    m_nMoneySupply = 0;

}


CBlockIndex::CBlockIndex(HUINT nFileIn, HUINT nBlockPosIn, const CBlockHeader& bh) : CBlockHeader(bh), m_pHashBlock(nullptr), m_pPrev(nullptr), m_pNext(nullptr), m_nFile(nFileIn), m_nBlockPos (nBlockPosIn), m_nHeight(0), m_nMoneySupply(0){

    

}


std::string CBlockIndex::ToString() const {

    HSTR str1 =  HCStr::Format ("pprev=%p, pnext=%p, nFile=%u, nBlockPos=%d, \
nHeight=%d, nMoneySupply=%lld",m_pPrev,  m_pNext, m_nFile, m_nBlockPos, m_nHeight,
			  m_nMoneySupply);
    
    HSTR str2 = HCStr::Format("nVersion=%d, nType=%d, hashprev=%s, \
hashMerkleRoot=%s, time=%d, bits=%d, count=%d, nonce=%lld", GetVersion(), GetType(),  GetHashPrevBlock().ToString().c_str(),  GetMerkleRoot().ToString().c_str(), GetTime(), GetBits(), GetCount(),  GetNonce());

    return str1 + "|" + str2;


}

const CBlockIndex* GetLastBlockIndex (const CBlockIndex* pindex) {

    while (pindex && pindex->GetPrevIndex()) {

	pindex = pindex->GetPrevIndex ();
	
    }

    return pindex;

}










