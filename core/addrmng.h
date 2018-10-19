/*****************************************************************************
 *
 * FileName: addrmng.h
 *
 * Author: Tom Hui, tomhui1009@yahoo.com, 8613760232170
 *
 * Create Date: Wed Oct 03 21:18 2018
 *
 * Brief:
 *
 *
 *****************************************************************************/


#ifndef __ADDRMNG_H__
#define __ADDRMNG_H__

#include <huibase.h>
#include "lktmap.h"
#include "block.h"


class CAddrMng {
 public:
    CAddrMng () { }

    virtual ~ CAddrMng( ) { }

 public:
    HRET OverBlockChain (const CBlockIndexMap& index_map);

    virtual HRET OverNewBlock (const CBlock& block) = 0;

 protected:
    virtual HRET overBlock (const CBlock& block) = 0;

    virtual HRET overEntry (const CEntry& entry) = 0;



};





#endif //__ADDRMNG_H__
