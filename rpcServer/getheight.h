

#ifndef __GET_HEIGHT_H__
#define __GET_HEIGHT_H__

#include <huibase.h>
#include "command.h"

using namespace HUIBASE;

class CGetHeightCgi : public CCommand {
 public:
    CGetHeightCgi (HCSTRR strName) : CCommand (strName) {}

    HRET Work () throw (HCException);

    virtual HSTR GetRes () const throw (HCException);

};


#endif
