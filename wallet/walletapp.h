

#ifndef __WALLETAPP_H__
#define __WALLETAPP_H__

#include <huibase.h>
#include <happ.h>

#include <sys/socket.h>

#include <netinet/in.h>
#include <event.h>
#include <evhttp.h>

using namespace HUIBASE;

class CWalletApp : public HCApp {
 public:
    CWalletApp(HINT argc, const HCHAR* argv[]);

    ~ CWalletApp ();

    HBOOL Run ();

    HCSTRR GetDataPath () const { return m_strDataPath; }

    HSTR GetConfVal (HCSTRR strKey) { return m_conf.GetValue(strKey); }


 private:
    virtual void init();

    HRET setupSignal ();

    HRET setupParam ();

    HRET initServer () throw (HCException);

    HRET initDataPath();


 private:
    static void httpserver_handler(struct evhttp_request* req, void* arg);


 private:
    HSTR m_rpc_ip;

    HUINT m_rpc_port {0};

    struct event_base* m_base = nullptr;

    HSTR m_strDataPath;

};


#endif // __WALLETAPP_H__
