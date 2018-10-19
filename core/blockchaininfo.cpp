


#include "blockchaininfo.h"
#include "ifnames.h"



HSTR CLaikelibInfo::ToString() const {

    std::stringstream ss;
    ss << "{version:" << GetVersion() << ", besthash:" << GetBestHash()<< ", height:" << GetHeight() << ", genesishash:" << GetGenesisHash()<<"}";

    return ss.str();

}



HSTR CBlockChainInfo::GetNodeInfo() const {

    std::stringstream ss;
    ss << "{ \"version\": \"" << GetVersionInfo() << "\", "
       << "\"bestHash\": \"" << m_hashBest.ToString() << "\", "
       << "\"height\": " << m_pBestIndex->GetHeight() << ","
       << "\"genesisHash\" : \"" << m_pGenesisIndex->GetBlockHash().ToString() <<"\"}";

    return ss.str();

}


HSTR CBlockChainInfo::GetVersionInfo() const {

    std::stringstream ss;

    ss << LK_VERSION << "." << LK_CP_VERSION << "." << PTL_VERSION << "."
       << BLOCK_VERSION;

    return ss.str();

}
