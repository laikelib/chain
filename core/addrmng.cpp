


#include "addrmng.h"
#include <glog/logging.h>

HRET CAddrMng::OverBlockChain(const CBlockIndexMap &index_map) {

    using BLOCKINDEXHEIGHTMAP = CBlockIndexMap::BLOCKINDEXHEIGHTMAP;
    const BLOCKINDEXHEIGHTMAP height_index = index_map.m_mapHeight;
    using INDEXSET = CBlockIndexMap::INDEXSET;

    vector<pair<HUINT, std::vector<CBlockIndex*> > > vSortByHeight;

    for (BLOCKINDEXHEIGHTMAP::const_iterator cit = height_index.cbegin(); cit != height_index.cend(); ++cit) {

        //const CBlockIndex* pIndex = cit->second;
        //vSortByHeight.push_back(make_pair(pIndex->GetHeight(), pIndex));

        HUINT nHeight = cit->first;
        const INDEXSET& sets = cit->second;

        vSortByHeight.push_back(make_pair(nHeight, sets));

    }

    std::sort(vSortByHeight.begin(), vSortByHeight.end());

    for (size_t i = 0; i < vSortByHeight.size(); ++i) {

        const INDEXSET& sets = vSortByHeight[i].second;

        const CBlockIndex* pIndex = CBlockIndexMap::GetMainIndex(sets);

        CBlock block;
        block.ReadFromDisk(pIndex);

        overBlock(block);

    }

    HRETURN_OK;

}



