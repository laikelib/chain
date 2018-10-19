/*****************************************************************************
 *
 * FileName: blockindexmap.h
 *
 * Author: Tom Hui, tomhui1009@yahoo.com, 8613760232170
 *
 * Create Date: Sun Sep 30 13:23 2018
 *
 * Brief:
 *
 *
 *****************************************************************************/



#ifndef __BLOCKINDEXMAP_H__
#define __BLOCKINDEXMAP_H__

#include <huibase.h>
#include "blockindex.h"

#include <unordered_map>

using namespace HUIBASE;

class hash256_hash {
 public:
    size_t operator() (const uint256& hash) const { return std::hash<std::string>{} (hash.GetHex());  }

};

class hash256_equal {
 public:
    bool operator() (const uint256& a, const uint256& b) const {
        return a == b;
    }
};

class CBlockIndexMap {
 public:
    using BLOCKINDEXMAP = std::unordered_map<uint256, CBlockIndex*, hash256_hash, hash256_equal>;
    using INDEXSET = std::vector<CBlockIndex*>;
    using BLOCKINDEXHEIGHTMAP = std::unordered_map<HUINT, INDEXSET>;

    friend class CAddrMng;

 public:
    CBlockIndexMap () { }

    ~CBlockIndexMap ();

 public:
    CBlockIndex* GetIndex (const uint256& hash);

    const CBlockIndex* GetIndex (const uint256& hash) const;

    CBlockIndex* GetIndex (HUINT height);

    const CBlockIndex* GetIndex (HUINT height) const;

    HRET InsertIndex (const uint256& hash, CBlockIndex* index);

    HRET SetIndexHeight (const uint256& hash, HUINT height);

    const BLOCKINDEXHEIGHTMAP& GetHeightIndex () const { return m_mapHeight; }

    BLOCKINDEXMAP& GetMap () { return m_map; }


 public:
    // for check
    HRET CheckGenesisInfo (const uint256& hashGenesis) const;

    HRET CheckBlocks () const;

    HRET CheckRootBestHeight(const uint256& hashBest, const uint256& hashBestRoot, HUINT nHeight) const;


 public:
    CBlockIndex* newBlockIndex(const uint256& hash);

    HULL GetCount () const { return m_map.size(); }

    HULL GetHeightMapCount() const { return m_mapHeight.size(); }

    HBOOL IsEmpty() const { HRET_BOOL(m_map.empty()); }

    HBOOL HasBlock (const uint256& hash) const ;

 private:
    void insertHeightIndex(HUINT nHeight, CBlockIndex* pIndex);

 public:
    static CBlockIndex* GetMainIndex (INDEXSET& sets);

    static const CBlockIndex* GetMainIndex (const INDEXSET& sets);


 private:
    BLOCKINDEXMAP m_map;

    BLOCKINDEXHEIGHTMAP m_mapHeight;
};





#endif //__BLOCKINDEXMAP_H__
