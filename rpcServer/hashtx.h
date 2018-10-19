


#ifndef __HASH_TX_H__
#define __HASH_TX_H__

#include <huibase.h>
#include "command.h"

using namespace HUIBASE;

class CHashTxCgi : public CCommand {
 public:
    CHashTxCgi (HCSTRR strHash)
        : CCommand (strHash) { }

    virtual HRET Init (HCSTRR strHash) throw (HCException) override;

    HRET Work () throw (HCException);

    virtual HSTR GetRes () const throw (HCException);

 private:
    HSTR m_str;
};



#endif //__HASH_TX_H__
