

#include <hlog.h>

#include "../safeMiddle/common/middleConfig.h"

#include <arpa/inet.h>

#include <signal.h>

#include "command.h"

#include "syncmng.h"
#include "rpcapp.h"

struct event_base* g_base = nullptr;

CRpcApp::CRpcApp (HINT argc, const HCHAR* argv[])
    : HCApp(argc, argv){


}


CRpcApp::~CRpcApp () {

    if (m_pmq != nullptr) {

        m_pmq->mq.Close();

        HDELP(m_pmq);

    }

    if (nullptr != m_pSync) {

        HDELP(m_pSync);

    }

}


HBOOL CRpcApp::Run() {

    HFUN_BEGIN;

    g_base = m_base;

    event_base_dispatch(m_base);

    uninit();

    HFUN_END;

    return HTRUE;

}


void CRpcApp::newEn(const CEntry &en) {

    m_pSync->NewEn(en);

}


void CRpcApp::NewBl(const CBlock& bl) {

    m_pSync->NewBl(bl);

}


void CRpcApp::RunMngUpdate() {
    HFUN_BEGIN;

    while (HIS_TRUE(IsRunning())) {

        m_pSync->UpdateInfo();

        sleep(15);
    }

    HFUN_END;
}


void CRpcApp::RunMngNet() {
    HFUN_BEGIN;

    while (HIS_TRUE(IsRunning())) {

        m_pSync->HandleNet();

    }

    HFUN_END;
}


void CRpcApp::RunMngWork() {
    HFUN_BEGIN;

    while (HIS_TRUE(IsRunning())) {

        m_pSync->HandleWork();

    }

    HFUN_END;
}


void CRpcApp::RunMngReConnect() {
    HFUN_BEGIN;

    while (HIS_TRUE(IsRunning())) {

        m_pSync->ReConnect();

        sleep(60);
    }

    HFUN_END;
}


void CRpcApp::init() {

    // signal
    setupSignal();

    // setup param
    setupParam();

    // mq
    initMq();

    // http server
    initServer();

    initSync();

}


void CRpcApp::uninit() {

    Stop();

    m_threadInfo.Join(nullptr);
    m_threadNet.Join(nullptr);
    m_threadWork.Join(nullptr);
    m_threadRec.Join(nullptr);
}


void signal_handler(int sig) {

    switch(sig) {
    case SIGTERM:
    case SIGHUP:
    case SIGQUIT:
    case SIGINT:
        LOG_NS("signal break...");
        //releaseLock ();
        //event_loopbreak ();
        event_base_loopbreak(g_base);

        break;
    }

}


HRET CRpcApp::setupSignal() {

    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);

    HRETURN_OK;
}


HRET CRpcApp::setupParam() {

    m_mq_key = m_conf.GetIntValue("mq_key", 1688);

    m_app_count = m_conf.GetIntValue ("app_count", 32);

    m_mq_bk_count = m_conf.GetIntValue("mq_bk_count", 8);

    m_mq_valid_time = m_conf.GetIntValue("mq_validTime", 600);

    m_mq_try_time = m_conf.GetIntValue("mq_tryTime", 5);

    m_sleep_time = m_conf.GetIntValue("mq_sleepTime", 0);

    m_sleep_timeu = m_conf.GetIntValue("mq_SleepTimeu", 5);

    m_rpc_port = m_conf.GetIntValue("rpc_port", 10088);

    m_rpc_ip = m_conf.GetValue ("rpc_ip", "127.0.0.1");

    m_lk_ip = m_conf.GetValue ("lk_ip");

    m_main_ip = m_conf.GetValue("main_ip", "");

    m_main_port = m_conf.GetIntValue ("main_port", 11099);

    m_lk_port = m_conf.GetIntValue("lk_port", 11099);

    LOG_NORMAL("mq_key: [%d], app_count: [%d], mq_bk_count: [%d], mq_valid_time: [%d], mq_try_time: [%d], sleep_time: [%d], sleep_timeu: [%d]", m_mq_key, m_app_count, m_mq_bk_count, m_mq_valid_time, m_mq_try_time, m_sleep_time, m_sleep_timeu);

    LOG_NORMAL("rpc port: [%d]", m_rpc_port);

    LOG_NORMAL("laikelib ip[%s] port[%d]", m_lk_ip.c_str(), m_lk_port);

    LOG_NORMAL("laikelib mainip[%s] mainport[%d]", m_main_ip.c_str(), m_main_port);

    HRETURN_OK;

}


HRET CRpcApp::initMq() throw (HCException){

    HFUN_BEGIN;

    m_pmq = new RpcMq (m_mq_key, m_app_count, m_mq_bk_count, HCMq::SShmControl(m_mq_valid_time, m_mq_try_time, m_sleep_time, m_sleep_timeu));

    assert(m_pmq);

    HNOTOK_RETURN(m_pmq->mq.Open());

    HFUN_END;
    HRETURN_OK;

}


HRET CRpcApp::initServer() throw (HCException){
    HFUN_BEGIN;

    //httpserver_bindocket(m_rpc_port);
    //LOG_NORMAL("http socket: [%d]", m_fd);

    m_base = event_base_new();
    HASSERT_THROW_MSG(m_base != nullptr, "event init failed", SRC_FAIL);

    struct evhttp* httpd = evhttp_new(m_base);
    HASSERT_THROW_MSG(httpd != nullptr, "evhttp_new failed", SRC_FAIL);

    auto cb = evhttp_bind_socket(httpd, m_rpc_ip.c_str(), m_rpc_port);
    //auto cb = evhttp_accept_socket(httpd, m_fd);
    HASSERT_THROW_MSG(cb == 0, "evhttp_bind_socket failed", SRC_FAIL);

    evhttp_set_gencb(httpd, httpserver_handler, this);

    HFUN_END;
    HRETURN_OK;
}


HRET CRpcApp::initSync() throw (HCException) {
    HFUN_BEGIN;

    m_pSync = new CSyncMng(m_lk_ip, m_lk_port);
    CHECK_NEWPOINT(m_pSync);

    m_pSync->SetMq(GetMq());
    m_pSync->SetApp(this);
    m_pSync->SetMainIp(m_main_ip);
    m_pSync->SetMainPort(m_main_port);

    HFAILED_MSG(m_pSync->Init(), "init sync manger failed");

    m_arg.pApp = this;

    m_arg.op = 1; // update info;
    HFAILED_MSG(m_threadInfo.Create(run_sync, &m_arg), "create sync info thread failed");
    sleep(1);

    m_arg.op = 2;
    HFAILED_MSG(m_threadNet.Create(run_sync, &m_arg), "create sync net thread failed");
    sleep(1);

    m_arg.op = 3;
    HFAILED_MSG(m_threadWork.Create(run_sync, &m_arg), "create sync work thread failed");
    sleep(1);

    m_arg.op = 4;
    HFAILED_MSG(m_threadRec.Create(run_sync, &m_arg), "create sync reconnect thread failed");
    sleep(1);

    HFUN_END;
    HRETURN_OK;
}


void CRpcApp::httpserver_handler(struct evhttp_request *req, void * arg) {

    HFUN_BEGIN;

    struct evbuffer* buf = evbuffer_new();
    HASSERT_THROW_MSG(buf != nullptr, "evbuffer_new failed", SRC_FAIL);

    HSTR cmdtype;
    switch (evhttp_request_get_command(req))
    {
        case EVHTTP_REQ_GET:    cmdtype = "GET";    break;
        case EVHTTP_REQ_POST:   cmdtype = "POST";   break;
        case EVHTTP_REQ_HEAD:   cmdtype = "HEAD";   break;
        case EVHTTP_REQ_PUT:    cmdtype = "PUT";    break;
        case EVHTTP_REQ_DELETE: cmdtype = "DELETE"; break;
        case EVHTTP_REQ_OPTIONS:cmdtype = "OPTIONS";break;
        case EVHTTP_REQ_TRACE:  cmdtype = "TRACE";  break;
        case EVHTTP_REQ_CONNECT:cmdtype = "CONNECT";break;
        case EVHTTP_REQ_PATCH:  cmdtype = "PATCH";  break;
        default: cmdtype = "unknown"; break;
    }

    HSTR strCgi (evhttp_request_get_uri(req));
    strCgi = strCgi.substr(1);
    LOG_NORMAL("type: [%s], cgi: [%s]", cmdtype.c_str(), strCgi.c_str());


    size_t post_len = EVBUFFER_LENGTH(req->input_buffer);
    LOG_NORMAL("buffer length: [%d]", post_len);

    static constexpr unsigned int LEN = 102400;
    static char rbuf[LEN] = {0};
    memset(rbuf, 0, LEN);
    memcpy(rbuf, EVBUFFER_DATA(req->input_buffer), post_len);

    LOG_NORMAL("post data: [%s]", rbuf);

    stringstream ss;
    HSTR res;
    CCommand* pcgi = maker_facetory::Instance()->newCommand(strCgi);
    std::shared_ptr<CCommand> autop(pcgi);
    if (pcgi == nullptr) {

        ss << "{ \"ret_code\":\"999\", \"ret_msg\":\"have no this cgi\"  }";

    } else {

        CRpcApp* app = (CRpcApp*)arg;
        pcgi->SetMq(app->GetMq());
        pcgi->SetApp(app);

        try {

            HFAILED_THROW(pcgi->Init(rbuf));
            HFAILED_THROW(pcgi->Work());
            res = pcgi->GetRes();
            ss << "{\"ret_code\":\"0\", \"ret_msg\":\"ok\", \"result\": "
               << res <<"}";

        } catch (const HCException& ex) {

            ss << "{\"ret_code\":\"88\", \"ret_msg\":\""
               << ex.what() <<"\"}";

        } catch (...) {

            ss << "{ \"ret_code\":\"99\", \"ret_msg\":\"unkown exception\"  }";

        }
    }

    res = ss.str();

    LOG_NORMAL("cgi response: [%s]", res.c_str());
    evbuffer_add_printf(buf, "%s", res.c_str());

    evhttp_send_reply(req, 200, "OK", buf);

    HFUN_END;
}


void* CRpcApp::run_sync(void *arg) {

    ThreadArg* pArg = (ThreadArg*) arg;

    switch(pArg->op) {
    case 1:
        pArg->pApp->RunMngUpdate();
        break;

    case 2:
        pArg->pApp->RunMngNet();
        break;

    case 3:
        pArg->pApp->RunMngWork();
        break;

    case 4:
        pArg->pApp->RunMngReConnect();
        break;
    }

    return nullptr;

}
