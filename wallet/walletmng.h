

#ifndef __WALLETMNG_H__
#define __WALLETMNG_H__

#include <huibase.h>
#include <hfname.h>
#include <hsingleton.hpp>
#include <event.h>

using namespace HUIBASE;


class CWalletMng {
 public:
    CWalletMng () { }

    HSTR NewAccount (HCSTRR strPass);

    HSTR SetAccount (HCSTRR strAddr, HCSTRR strPub, HCSTRR strPri, HCSTRR strPass);

 public:
    void SetDataPath (HCSTRR strDataPath) { m_strDataPath = strDataPath; }

    HCSTRR GetDataPath () const { return m_strDataPath; }

    void SetEventBase (struct event_base* _base) { m_base = _base; }

    struct event_base* GetEventBase () { return m_base; }

 private:
    HSTR m_strDataPath;

    struct event_base* m_base = nullptr;
};


typedef HCSingleton<CWalletMng> sgt_manage;

#ifndef g_manage
#define g_manage sgt_manage::Instance()
#endif


#endif //__WALLETMNG_H__
