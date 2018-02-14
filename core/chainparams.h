/****************************************************************************
 *
 * File Name: chainparams.h
 *
 * Create Author: Tom Hui, tomhui1009@yahoo.com, 8613760232170
 *
 * Create Date: Mon Jan 22 12:19 2018
 *
 * Description:
 *	
 *
 ****************************************************************************/

#ifndef __CHAINPARAMS_H__
#define __CHAINPARAMS_H__

#include <huibase.h>

#include "block.h"
#include "uint256.h"
#include "bignum.h"

using namespace HUIBASE;

class CChainParams {
public:
    enum NetWorkType {
	MAIN,
	TEST,
    };

public:
    CChainParams ();

    virtual uint256 GetGenesisBlockHash () const = 0;

    virtual const CBlock& GenesisBlock () const = 0; 

    NetWorkType GetNetWorkType () const { return MAIN; }

    const CBigNum& GetPowLimit () const { return m_bnPowLimit; }

    virtual HN64 GetTargetTimeSpan () const { return 24 * 60 * 60; }

    virtual HINT GetTargetSpacing () const { return 60; }

    virtual HINT GetTargetSpacingWorkMax () const { return 12 * GetTargetSpacing(); }

private:
    CBigNum m_bnPowLimit;
    
};

#endif //__CHAINPARAMS_H__
