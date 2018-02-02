
#include <bignum.h>
#include <ifnames.h>
#include "mainParams.h"

CMainParams::CMainParams() {

    m_genesis.SetVersion (BLOCK_VERSION);

    m_genesis.SetType (0);

    m_genesis.SetLength (0);

    m_genesis.SetHashPrevBlock (0);

    m_genesis.SetHashMerkleRoot (0);

    m_genesis.SetTime (time(nullptr));

    CBigNum bnGenesisBits = CBigNum(~uint256(0) >> 24);

    m_genesis.SetBits(bnGenesisBits.GetCompact());
    
    m_genesis.SetNonce(0);

    CLKAddress addr;
    m_genesis.SetWhoAddress (addr);

    m_genesis.SetCount (0);

}


uint256 CMainParams::GetGenesisBlockHash() const {

    return m_genesis.GetHash ();

}


