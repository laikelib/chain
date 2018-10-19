/****************************************************************************
 *
 * File Name: mainparams.h
 *
 * Create Author: Tom Hui, tomhui1009@yahoo.com, 8613760232170
 *
 * Create Date: Mon Jan 22 12:19 2018
 *
 * Description:
 *	
 *
 ****************************************************************************/


#ifndef __MAINPARAMS_H__
#define __MAINPARAMS_H__

#include <chainparams.h>
#include <block.h>
#include <ifnames.h>

class CMainParams : public CChainParams {
public:
    CMainParams();

    uint256 GetGenesisBlockHash () const;

    const CBlock& GenesisBlock () const { return m_genesis; }


    HN64 GetSupplySum () const { return (HN64)45000000 * LKT_COIN;  }

    HUINT FirstHalfPoint () const { return 2000000; }

    HUINT SecondHalfPoint() const { return 6000000; }

    HUINT LastPoint() const { return 8000000; }

    HN64 FirstPay () const { return 10 * LKT_COIN; }

    HN64 SecondPay () const { return 5 * LKT_COIN; }

    HN64 LastPay () const { return 2.5 * LKT_COIN; }

    HN64 CoinBasePay (HUINT nHeight) const;


private:
    CBlock m_genesis;
}; 


#endif //__MAINPARAMS_H__









