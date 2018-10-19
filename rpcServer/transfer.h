

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

 private:
    HSTR m_str;

    HSTR m_src;
};



#endif
