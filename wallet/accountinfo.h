

#ifndef __ACCOUNTINFO_H__
#define __ACCOUNTINFO_H__

#include <huibase.h>
#include "command.h"

using namespace HUIBASE;


class CAccountInfoCgi : public CCommand {
 public:
    CAccountInfoCgi (HCSTRR strName) : CCommand(strName) { }

    virtual HRET Init (HCSTRR str) throw (HCException) override;

    HRET Work () throw (HCException);

 private:
    HSTR m_strAddr;

    HSTR m_strPass;


};


#endif //__ACCOUNTINFO_H__
