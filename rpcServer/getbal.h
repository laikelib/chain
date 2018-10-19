


#ifndef __GET_BAL_H__
#define __GET_BAL_H__

#include <huibase.h>
#include "command.h"


using namespace HUIBASE;


class CGetBalCgi : public CCommand {
 public:
    CGetBalCgi (HCSTRR strAddr) : CCommand (strAddr) {  }

    virtual HRET Init (HCSTRR strAddr) throw (HCException) override;

    HRET Work () throw (HCException);

    virtual HSTR GetRes () const throw (HCException);

 private:
    HSTR m_str;

};







#endif // __GET_BAL_H__
