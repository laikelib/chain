


#ifndef __COIN_TXDB_H__
#define __COIN_TXDB_H__

#include <txdb-leveldb.h>

class CCoinTxdb : public CTxDB {
public:
    CCoinTxdb(const char* pszMode = "r+");

    bool LoadBlockIndex ();

};


#endif //__COIN_TXDB_H__


