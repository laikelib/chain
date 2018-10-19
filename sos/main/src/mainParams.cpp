
#include <bignum.h>
#include <ifnames.h>
#include "mainParams.h"

#include <huibase.h>
#include <htime.h>

using namespace HUIBASE;

CMainParams::CMainParams() {

    m_genesis.SetVersion (BLOCK_VERSION);

    m_genesis.SetType (0);

    m_genesis.SetHashPrevBlock (0);

    m_genesis.SetHashMerkleRoot (0);

    HCTime thetime(2017,6,18,6,6,18);
    m_genesis.SetTime (thetime.getTime());

    CBigNum bnGenesisBits = CBigNum(~uint256(0) >> 24);

    m_genesis.SetBits(bnGenesisBits.GetCompact());
    
    m_genesis.SetNonce(0);

    m_genesis.SetCount (0);

}


uint256 CMainParams::GetGenesisBlockHash() const {

    return m_genesis.GetHash ();

}



HN64 CMainParams::CoinBasePay(HUINT nHeight) const{

    HN64 res = 0;

    if (nHeight <= FirstHalfPoint() ) {

        res = FirstPay();

    } else if (nHeight > FirstHalfPoint() && nHeight <= SecondHalfPoint()) {

        res = SecondPay();

    } else if (nHeight > SecondHalfPoint() && nHeight <= LastPoint()) {

        res = LastPay();

    } else if (nHeight > LastPoint()) {

        res = 0;

    }

    return res;

}
