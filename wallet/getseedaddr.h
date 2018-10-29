

#ifndef __GETSEEDADDR_H__
#define __GETSEEDADDR_H__

#include <huibase.h>
#include "command.h"

#include <bip.h>
#include <key.h>
#include <base58.h>


using namespace HUIBASE;


class CGetSeedAddrCgi : public CCommand {
 public:
    CGetSeedAddrCgi (HCSTRR strName) : CCommand (strName) { }

    virtual HRET Init (HCSTRR str) throw (HCException) override;

    HRET Work () throw (HCException);

 private:
    HSTR m_strSeed;
    HSTR m_strPath;

    CKey m_key;
    CPubKey m_pubKey;

    CBip44Path m_path;

    CKey m_indexKey;

    HSTR m_strAddr;

};




#endif //__GETSEEDADDR_H__
