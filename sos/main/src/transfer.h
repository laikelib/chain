


#ifndef __TRANSFER_H__
#define __TRANSFER_H__


#include <sodef.h>


class CTransfer : public CSoServiceBase {
public:
    CTransfer (HCSTRR strName)
	: CSoServiceBase (strName) {  }


    virtual int ParseInput (SCM_DATA* data) throw (HCException);

    virtual int Excute () throw (HCException);
    
    virtual int PackReturn (SCM_DATA* data) throw (HCException);

private:
    HDOUBLE m_dValue;
    HDOUBLE m_dFee;
    HSTR m_strSender;
    HSTR m_strReceiver;
    HSTR m_strPublic;
    HSTR m_strPrivate;

    HSTR m_result;
}; 

class CInnerTransfer : public CSoServiceBase {
public:
    CInnerTransfer (HCSTRR strName)
	: CSoServiceBase (strName) { }

    virtual int ParseInput (SCM_DATA* data) throw (HCException);

    virtual int Excute () throw (HCException);

    virtual int PackReturn (SCM_DATA* data) throw (HCException);

private:
    HDOUBLE m_dValue;
    HDOUBLE m_dFee;
    HSTR m_strSender;
    HSTR m_strReceiver;
    
    HSTR m_result;
};

#endif // __TRANSFER_H__
