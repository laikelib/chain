

#ifndef __RPCAPP_H__
#define __RPCAPP_H__

#include <huibase.h>
#include <happ.h>
#include <hmutex.h>
#include <hmq.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <event.h>
#include <evhttp.h>

#include <entry.h>
#include <block.h>

#include <thread.h>

using namespace HUIBASE;

class CSyncMng;

struct RpcMq {
    RpcMq (HSYS_T ipckey, HUINT uMaxNodeNum, HUINT uMaxBlockNum,
           const HCMq::SShmControl& control)
    : mq (ipckey, uMaxNodeNum, uMaxBlockNum, control) { }

    HCMq mq;
    HCMutex mutex;
};

class CRpcApp : public HCApp {
 public:
    typedef struct {
        class CRpcApp* pApp;
        //class CSyncMng* pMng;
        int op;
    } ThreadArg;
 public:
    CRpcApp(HINT argc, const HCHAR* argv[]);

    ~ CRpcApp ();

    HBOOL Run ();

    RpcMq* GetMq () { return m_pmq; }

    CSyncMng* GetSyncMng() { return m_pSync; }

    void newEn (const CEntry& en);

    void NewBl (const CBlock& block);

 public:
    void RunMngUpdate();

    void RunMngNet();

    void RunMngWork ();

    void RunMngReConnect();


 private:
    virtual void init();

    void uninit();

    HRET setupSignal ();

    HRET setupParam ();

    HRET initMq () throw (HCException);

    HRET initServer () throw (HCException);

    HRET initSync () throw (HCException);


 private:
    static void httpserver_handler(struct evhttp_request* req, void* arg);

    static void* run_sync (void* arg);

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

    RpcMq* m_pmq = nullptr;

    //HFD_T m_fd {0};

    HSTR m_lk_ip;
    HUINT m_lk_port{0};

    struct event_base* m_base = nullptr;

    class CSyncMng* m_pSync = nullptr;

    CThread m_threadInfo;

    CThread m_threadNet;

    CThread m_threadWork;

    CThread m_threadRec;

    ThreadArg m_arg;

    HSTR m_main_ip;

    HUINT m_main_port;

};


#endif // __RPCAPP_H__
