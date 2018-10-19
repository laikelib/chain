


#ifndef __LK_INFO_H__
#define __LK_INFO_H__

#include <huibase.h>
#include "command.h"

using namespace HUIBASE;


class CLkInfoCgi : public CCommand {
 public:
    CLkInfoCgi (HCSTRR strName) : CCommand (strName) {}

    HRET Work () throw (HCException);

    virtual HSTR GetRes () const throw (HCException);

};



#endif
