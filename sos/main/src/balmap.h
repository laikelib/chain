/*****************************************************************************
 *
 * FileName: balmap.h
 *
 * Author: Tom Hui, tomhui1009@yahoo.com, 8613760232170
 *
 * Create Date: Wed Oct 03 15:07 2018
 *
 * Brief:
 *
 *
 *****************************************************************************/



#ifndef __BALMAP_H__
#define __BALMAP_H__

#include <huibase.h>
#include <base58.h>

#include <unordered_map>
#include <lktmap.h>
#include <addrmng.h>
#include <entry.h>

using namespace HUIBASE;

class lktaddr_hash{
 public:
    size_t operator() (const CLKAddress& addr) const {
        return std::hash<std::string>{ } (addr.ToString());
    }
};

class lktaddr_equal{
 public:
    bool operator() (const CLKAddress& addr1, const CLKAddress& addr2) const {
        return addr1 == addr2;
    }
};

class CBalMng : public CAddrMng {
 public:
    using BALMAP = std::unordered_map<CLKAddress, HN64, lktaddr_hash, lktaddr_equal>;

 public:
    CBalMng () { }

    ~CBalMng () { }

    HN64 GetBalSum () const;

    HRET SetAddrBal (const CLKAddress& addr, HN64 bal);

    HRET PostBal (const CLKAddress& addr, HN64 bal);

    HRET DelBal (const CLKAddress& addr, HN64 bal);

    HN64 GetBal (const CLKAddress& addr) const;

    void Display ();

    HRET OverNewBlock (const CBlock& block) override;

 protected:
    HRET overBlock (const CBlock& block) override;

    HRET overEntry (const CEntry& entry) override;


 private:
    BALMAP m_mapBal;

};



#endif //__BALMAP_H__
