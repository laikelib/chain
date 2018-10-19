


#include "lktmap.h"
#include "block.h"


CBlockIndexMap::~CBlockIndexMap () {

    for (BLOCKINDEXMAP::iterator it = m_map.begin(); it != m_map.end(); ++it) {

        CBlockIndex* p = it->second;
        delete p;

    }

    m_map.clear();

    m_mapHeight.clear();

}


CBlockIndex* CBlockIndexMap::GetIndex(const uint256 &hash) {

    BLOCKINDEXMAP::iterator fit = m_map.find(hash);
    if (fit == m_map.end()) {
        return nullptr;
    }

    return fit->second;

}


const CBlockIndex* CBlockIndexMap::GetIndex(const uint256 &hash) const {

    BLOCKINDEXMAP::const_iterator fit = m_map.find(hash);
    if (fit == m_map.end()) {
        return nullptr;
    }

    return fit->second;

}


CBlockIndex* CBlockIndexMap::GetIndex(HUINT nHeight)  {

    BLOCKINDEXHEIGHTMAP::iterator fit = m_mapHeight.find(nHeight);
    if (fit == m_mapHeight.end()) {
        return nullptr;
    }

    INDEXSET& indexs = fit->second;

    return GetMainIndex(indexs);

}


const CBlockIndex* CBlockIndexMap::GetIndex(HUINT nHeight) const {

    BLOCKINDEXHEIGHTMAP::const_iterator fit = m_mapHeight.find(nHeight);
    if (fit == m_mapHeight.end()) {
        return nullptr;
    }

    const INDEXSET& indexs = fit->second;

    return GetMainIndex(indexs);

}


HRET CBlockIndexMap::InsertIndex(const uint256 &hash, CBlockIndex *index) {

    BLOCKINDEXMAP::iterator fit = m_map.find(hash);
    if (fit != m_map.end()) {
        HRETURN(RECREATE);
    }

    BLOCKINDEXMAP::iterator rit = m_map.insert(make_pair(hash, index)).first;
    index->SetBlockHash(&(rit->first));

    //m_mapHeight.insert(make_pair(index->GetHeight(), index));
    insertHeightIndex(index->GetHeight(), index);

    HRETURN_OK;
}


HRET CBlockIndexMap::SetIndexHeight(const uint256 &hash, HUINT height) {

    CBlockIndex* pIndex = GetIndex(hash);
    if (pIndex != nullptr) {
        insertHeightIndex(height, pIndex);
        //m_mapHeight[height] = pIndex;
    }

    HRETURN_OK;
}



HRET CBlockIndexMap::CheckGenesisInfo(const uint256& hashGenesis) const {

    IF_FALSE(HasBlock(hashGenesis)) {
        HRETURN(INDEX_OUT);
    }

    const CBlockIndex* pIndex = GetIndex(hashGenesis);
    HASSERT_RETURN(pIndex != nullptr, INDEX_OUT);

    HASSERT_RETURN(pIndex->GetHeight() == 0, ERR_STATUS);

    const CBlockIndex* pHeightIndex = GetIndex(0);
    HASSERT_RETURN(pHeightIndex->GetBlockHash() == pIndex->GetBlockHash(), ERR_STATUS);

    HRETURN_OK;
}



HRET CBlockIndexMap::CheckBlocks() const {

    for (BLOCKINDEXMAP::const_iterator cit = m_map.begin(); cit != m_map.end(); ++cit) {

        const CBlockIndex* pIndex = cit->second;

        if (pIndex->GetHeight() == 0) {
            continue;
        }

        CBlock block;
        block.ReadFromDisk(pIndex);

        HASSERT_RETURN(block.CheckBlock(), ERR_STATUS);

    }

    HRETURN_OK;

}


HRET CBlockIndexMap::CheckRootBestHeight(const uint256 &hashBest, const uint256 &hashBestRoot, HUINT nHeight) const {

    // check root;
    const uint256& hashGen = GetIndex(0)->GetBlockHash();
    HASSERT_RETURN(hashGen != hashBest, ERR_STATUS);

    const CBlockIndex* pRootIndex = GetIndex(hashGen);
    const CBlockIndex* pBestIndex = GetIndex(hashBest);

    if (hashGen != hashBestRoot) {
        // if the genesis block hash is the best root hash,
        // that means there is only a chain in Laikelib.

        HUINT nRootHeight = pRootIndex->GetHeight();
        const INDEXSET& vs = m_mapHeight.at(nRootHeight);

        HASSERT_RETURN(vs.size() > 1, ERR_STATUS);

    }

    (void) pBestIndex;
    (void) nHeight;



    HRETURN_OK;
}



CBlockIndex* CBlockIndexMap::newBlockIndex (const uint256& hash) {

    if (hash == 0) {
        return nullptr;
    }

    // Return exists.
    BLOCKINDEXMAP::iterator fit = m_map.find(hash);
    if (fit != m_map.end()) {
        return fit->second;
    }

    // create new blockindex, and return.
    CBlockIndex* pIndexNew = new CBlockIndex();
    CHECK_NEWPOINT(pIndexNew);

    BLOCKINDEXMAP::iterator rit = m_map.insert(make_pair(hash, pIndexNew)).first;
    pIndexNew->SetBlockHash(&(rit->first));

    return pIndexNew;

}



HBOOL CBlockIndexMap::HasBlock (const uint256& hash) const  {

    HRET_BOOL(m_map.count(hash));

}


void CBlockIndexMap::insertHeightIndex(HUINT nHeight, CBlockIndex* pIndex) {

    BLOCKINDEXHEIGHTMAP::iterator fit = m_mapHeight.find(nHeight);

    if (fit == m_mapHeight.end()) {

        INDEXSET vs;
        vs.push_back(pIndex);

        m_mapHeight.insert(make_pair(nHeight, vs));

    } else {

        INDEXSET& vs = fit->second;

        vs.push_back(pIndex);

    }

}

CBlockIndex* CBlockIndexMap::GetMainIndex(INDEXSET& sets) {

    for (INDEXSET::iterator it = sets.begin(); it != sets.end(); ++it) {

        CBlockIndex* p = *it;

        if (p->IsMain()) {
            return p;

            break;
        }

    }

    return nullptr;

}



const CBlockIndex* CBlockIndexMap::GetMainIndex(const INDEXSET &sets) {

    for (INDEXSET::const_iterator it = sets.cbegin(); it != sets.cend(); ++it) {

        const CBlockIndex* p = *it;
        if (p->IsMain()) {
            return p;

            break;
        }

    }

    return nullptr;

}
