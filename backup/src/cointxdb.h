


#ifndef __COIN_TXDB_H__
#define __COIN_TXDB_H__

#include <chainApp.h>
#include <txdb-leveldb.h>

class CCoinTxdb : public CTxDB {
public:
    CCoinTxdb(const char* pszMode = "r+");

    bool LoadBlockIndex ();

    bool ReadHashBestChain (uint256& hashBestChain);




};


#endif //__COIN_TXDB_H__


