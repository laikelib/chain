

#ifndef __RPCAPP_H__
#define __RPCAPP_H__

#include <huibase.h>
#include <happ.h>

#include <hmq.h>

#include <sys/socket.h>

#include <netinet/in.h>
#include <event.h>
#include <evhttp.h>

using namespace HUIBASE;

class CRpcApp : public HCHapp {
 public:
    CRpcApp(HINT argc, HCHAR* argv[]);

    ~ CRpcApp ();

    HBOOL Run ();

    HCMq* GetMq () { return m_pmq; }

 private:
    virtual void init();

    HRET setupSignal ();

    HRET setupParam ();

    HRET initMq () throw (HCException);

    HRET initServer () throw (HCException);

    //int httpserver_bindocket (HINT port);

 private:
    static void httpserver_handler(struct evhttp_request* req, void* arg);

 private:
    HSYS_T m_mq_key {0};

    HUINT m_app_count{0};

    HUINT m_mq_bk_count{0};

    HINT m_mq_valid_time{0};

    HINT m_mq_try_time{0};

    HINT m_sleep_time{0};

    HINT m_sleep_timeu{0};

    HSTR m_rpc_ip;

    HUINT m_rpc_port {0};

    HCMq* m_pmq = nullptr;

    //HFD_T m_fd {0};

    struct event_base* m_base = nullptr;

};


#endif // __RPCAPP_H__
