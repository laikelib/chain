

#ifndef __NEWACCOUNT_H__
#define __NEWACCOUNT_H__

#include <huibase.h>
#include "command.h"

using namespace HUIBASE;

class CNewAccountCgi : public CCommand {
 public:
    CNewAccountCgi (HCSTRR strName)
        : CCommand(strName) { }

    virtual HRET Work () throw (HCException)  override;


};




#endif //__NEWACCOUNT_H__
