

#ifndef __TRANSFER_H__
#define __TRANSFER_H__

#include <huibase.h>
#include "command.h"

using namespace HUIBASE;

class CTransferCgi : public CCommand {
public:
    CTransferCgi(HCSTRR strName) : CCommand(strName) {}

    virtual HRET Init (HCSTRR ) throw (HCException) override;

    HRET Work () throw (HCException);

    virtual HSTR GetRes () const throw (HCException) override;

 private:
    HSTR m_strTo;
    HSTR m_strFo;

    HDOUBLE m_dVal;
    HDOUBLE m_dFee;

    HSTR m_strPri;
    HSTR m_strPub;

    HSTR m_str;

};



#endif
