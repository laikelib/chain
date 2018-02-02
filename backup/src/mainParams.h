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

class CMainParams : public CChainParams {
public:
    CMainParams();

    uint256 GetGenesisBlockHash () const;

    const CBlock& GenesisBlock () const { return m_genesis; }

private:
    CBlock m_genesis;
}; 


#endif //__MAINPARAMS_H__









