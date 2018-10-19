/*****************************************************************************
 *
 * FileName: entrymng.h
 *
 * Author: Tom Hui, tomhui1009@yahoo.com, 8613760232170
 *
 * Create Date: Thu Oct 04 17:38 2018
 *
 * Brief:
 *
 *
 *****************************************************************************/


#ifndef __ENTRYMNG_H__
#define __ENTRYMNG_H__

#include <huibase.h>
#include "entry.h"
#include "block.h"
#include "base58.h"

using namespace HUIBASE;

class CEntryMng {
 public:
    using ENTRY_HOLDER = std::set<CEntry>;

 public:
    CEntryMng () { }

    ~ CEntryMng () { }

 public:
    HRET AddNewEntry (const CEntry& entry);

    virtual HRET GetUnBlockedEntrys (std::vector<CEntry>& entrys);

    HRET GetAllEntrys (std::vector<CEntry>& entrys);

    HRET AcceptBlock (const CBlock& block);

    HN64 GetPoolPay (const CLKAddress& addr) const ;

 private:
    HRET removeEntry (const CEntry& entry);

 private:
    ENTRY_HOLDER m_entrys;

};



#endif //__ENTRYMNG_H__
