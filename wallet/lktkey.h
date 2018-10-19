


#ifndef __LKTKEY_H__
#define __LKTKEY_H__


#include <huibase.h>



using namespace HUIBASE;


class CAddrInfo {
 public:
    CAddrInfo () {  }

 public:
    HRET Load (HCSTRR strAddr, HCSTRR strPass) throw (HCException);

 public:
    HCSTRR GetAddr () const { return m_strAddr; }
    HCSTRR GetPub () const { return m_strPub; }
    HCSTRR GetPri () const { return m_strPri; }

    void SetAddr(HCSTRR strAddr) { m_strAddr = strAddr; }
    void SetPub (HCSTRR strPub) { m_strPub = strPub; }
    void SetPri (HCSTRR strPri) { m_strPri = strPri; }

 private:
    HSTR m_strAddr;
    HSTR m_strPub;
    HSTR m_strPri;
};



#endif //__LKTKEY_H__
