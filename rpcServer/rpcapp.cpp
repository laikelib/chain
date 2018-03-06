

#include <hlog.h>
#include "rpcapp.h"
#include "../safeMiddle/common/middleConfig.h"

#include <arpa/inet.h>

#include <signal.h>

#include "command.h"

CRpcApp::CRpcApp (HINT argc, HCHAR* argv[])
    : HCHapp(argc, argv){


}


CRpcApp::~CRpcApp () {

    if (m_pmq != nullptr) {

        m_pmq->Close();

        delete m_pmq;

    }

}


HBOOL CRpcApp::Run() {

    HFUN_BEGIN;

    event_base_dispatch(m_base);

    HFUN_END;

    return HTRUE;

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

}

void signal_handler(int sig) {

    switch(sig) {
    case SIGTERM:
    case SIGHUP:
    case SIGQUIT:
    case SIGINT:
        LOG_NS("signal break...");
        //releaseLock ();
        event_loopbreak ();

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

    LOG_NORMAL("mq_key: [%d], app_count: [%d], mq_bk_count: [%d], mq_valid_time: [%d], mq_try_time: [%d], sleep_time: [%d], sleep_timeu: [%d]", m_mq_key, m_app_count, m_mq_bk_count, m_mq_valid_time, m_mq_try_time, m_sleep_time, m_sleep_timeu);

    LOG_NORMAL("rpc port: [%d]", m_rpc_port);

    HRETURN_OK;

}


HRET CRpcApp::initMq() throw (HCException){

    HFUN_BEGIN;

    m_pmq = new HCMq (m_mq_key, m_app_count, m_mq_bk_count, HCMq::SShmControl(m_mq_valid_time, m_mq_try_time, m_sleep_time, m_sleep_timeu));

    assert(m_pmq);

    HNOTOK_RETURN(m_pmq->Open());

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

/*
int CRpcApp::httpserver_bindocket(HINT port) {

    m_fd = socket(AF_INET, SOCK_STREAM, 0);
    HASSERT_THROW_MSG(m_fd > 0, "httpserver_bindsocket socket failed", SYS_FAILED);


    int opt = 1;
    int ret = setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(int));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(m_rpc_ip.c_str());
    addr.sin_port = htons(port);

    ret = bind(m_fd, (struct sockaddr*)&addr, sizeof(addr));
    HASSERT_THROW_MSG(ret == 0, "httpserver_bindocket bind failed", SYS_FAILED);

    ret = listen(m_fd, 32);
    HASSERT_THROW_MSG(ret == 0, "httpserver_bindocket listen failed", SYS_FAILED);

    //int flags = fcntl(m_fd, F_GETFL, 0);
    //fcntl(m_fd, F_SETFL, flags | O_NONBLOCK);

    return m_fd;

}
*/

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
