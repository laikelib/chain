

#ifndef __ACCOUNTINFO_H__
#define __ACCOUNTINFO_H__

#include <huibase.h>
#include "command.h"

using namespace HUIBASE;


class CAccountInfoCgi : public CCommand {
 public:
    CAccountInfoCgi (HCSTRR strName) : CCommand(strName) { }

    virtual HRET Init (HCSTRR strName) throw (HCException) override;

    HRET Work () throw (HCException);


    //virtual HSTR GetRes () const throw (HCException) override;

 private:
    HSTR m_str;


};


#endif //__ACCOUNTINFO_H__
