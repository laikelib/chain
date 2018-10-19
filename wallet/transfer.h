


#ifndef __TRANSFER_H__
#define __TRANSFER_H__

#include <huibase.h>
#include "command.h"
#include "lktkey.h"

#include <entry.h>

using namespace HUIBASE;


class CTransferCgi : public CCommand {
 public:
    CTransferCgi (HCSTRR strName): CCommand (strName) { }

    virtual HRET Init (HCSTRR str) throw (HCException) override;

    HRET Work () throw (HCException);

    virtual HSTR GetRes () const throw (HCException);

 private:
    void getAddrInfo () throw (HCException);

    void setEntry () throw (HCException);

    void signEntry () throw (HCException);

    void callRpc () throw (HCException);

 private:
    HSTR m_strPass;

    HSTR m_strAmount;

    HSTR m_strFee;

    HSTR m_strFrom;

    HSTR m_strTo;

    CAddrInfo m_addrInfo;

    CEntry m_entry;

    HN64 m_value;

    HN64 m_fee;

    HSTR m_strEn;

    HSTR m_strRes;


};







#endif
