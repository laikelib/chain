

#ifndef __POSTWORK_H__
#define __POSTWORK_H__


#include <huibase.h>
#include "command.h"

using namespace HUIBASE;

class CPostWorkCgi : public CCommand {
 public:
    CPostWorkCgi (HCSTRR strName) : CCommand(strName) {  }

    virtual HRET Init (HCSTRR str) throw (HCException) override;

    virtual HRET Work () throw (HCException) override;

    //virtual HSTR GetRes () const throw (HCException) override;

 private:
    HSTR m_str;

    HSTR m_src;
};



#endif //__POSTWORK_H__
