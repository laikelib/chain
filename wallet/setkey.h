

#ifndef __SETKEY_H__
#define __SETKEY_H__


#include <huibase.h>
#include "command.h"


using namespace HUIBASE;


class CSetKeyCgi : public CCommand {
 public:
    CSetKeyCgi (HCSTRR strName)
        : CCommand(strName) {  }


    virtual HRET Init (HCSTRR strPass) throw (HCException) override;

    virtual HRET Work () throw (HCException) override;

 private:
    HSTR m_strAddr;

    HSTR m_strPub;

    HSTR m_strPri;

    HSTR m_strPass;

};




#endif //__SETKEY_H__
