
#include "hash.h"
#include "util.h"
#include "blockheader.h"

#include <glog/logging.h>

CBlockHeader::CBlockHeader() {

    m_nVersion = NODE_VERSION;

    m_nType = (HINT)BT_MAIN;
    
    m_hashPrevBlock = 0;

    m_hashMerkleRoot = 0;
    
}


/*CBlockHeader::CBlockHeader(const CBlockHeader& bh)
    : m_nVersion(bh.GetVersion()),
      m_nType(bh.GetType()),
      m_hashPrevBlock(bh.GetHashPrevBlock()),
      m_hashMerkleRoot(bh.GetMerkleRoot()),
      m_nTime(bh.GetTime()),
      m_nBits(bh.GetBits()),
      m_nCount(GetCount()),
      m_nNonce(bh.GetNonce()){


      }*/


string CBlockHeader::GetHeaderJson() const {

    std::stringstream ss;

    ss << "{ \"version\":" << m_nVersion << ", \"type\":"<< m_nType
       << ", \"hashPrevBlock\":\"" << m_hashPrevBlock.ToString()
       << "\", \"hashMerkleRoot\":\"" << m_hashMerkleRoot.ToString() << "\",\"time\": " << m_nTime
       << ", \"bits\":" << m_nBits <<", \"nonce\":"<< m_nNonce << ",\"count\":" << m_nCount << "}";

    return ss.str();

}


string CBlockHeader::GetHeaderBuf() const {

    string res;
    res.resize(END(m_nCount) - BEGIN(m_nVersion));

    LOG(INFO) << "header buf size: " << (END(m_nNonce) - BEGIN(m_nVersion));

    res.insert(res.end(), BEGIN(m_nVersion), END(m_nNonce));

    return res;
}


uint256 CBlockHeader::TryHash() {

    ++m_nNonce;

    return GetHash();

}


uint256 CBlockHeader::GetHash () const {

    return Hash(BEGIN(m_nVersion), END(m_nNonce));

}




