

#ifndef __NEWTX_H__
#define __NEWTX_H__

#include <sodef.h>

class CNewTxService : public CSoServiceBase {
 public:

 CNewTxService(HCSTRR strName)
     : CSoServiceBase (strName) {}

    virtual int ParseInput (SCM_DATA* data) throw (HCException);

    virtual int Excute () throw (HCException);

    virtual int PackReturn (SCM_DATA* data) throw (HCException);

 protected:
    HSTR m_str;

};




#endif
