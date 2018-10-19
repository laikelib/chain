

#include <hlog.h>
#include "walletapp.h"
#include "../safeMiddle/common/middleConfig.h"

#include <arpa/inet.h>

#include <signal.h>

#include "command.h"
#include "walletmng.h"

CWalletApp::CWalletApp (HINT argc, const HCHAR* argv[])
    : HCApp(argc, argv){


}


CWalletApp::~CWalletApp () {

}


HBOOL CWalletApp::Run() {
    HFUN_BEGIN;

    event_base_dispatch(m_base);

    HFUN_END;
    return HTRUE;
}



void CWalletApp::init() {

    // init data path;
    initDataPath();

    // signal
    setupSignal();

    // setup param
    setupParam();

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

        event_base_loopbreak(g_manage->GetEventBase());
        //event_loopbreak ();

        break;
    }

}


HRET CWalletApp::setupSignal() {

    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);

    HRETURN_OK;
}


HRET CWalletApp::setupParam() {

    m_rpc_port = m_conf.GetIntValue("wallet_port", 10090);

    m_rpc_ip = m_conf.GetValue ("wallet_ip", "127.0.0.1");

    LOG_NORMAL("rpc info [%s:%d]", m_rpc_ip.c_str(), m_rpc_port);

    HRETURN_OK;

}


HRET CWalletApp::initServer() throw (HCException){
    HFUN_BEGIN;

    m_base = event_base_new();
    HASSERT_THROW_MSG(m_base != nullptr, "event init failed", SRC_FAIL);

    struct evhttp* httpd = evhttp_new(m_base);
    HASSERT_THROW_MSG(httpd != nullptr, "evhttp_new failed", SRC_FAIL);

    auto cb = evhttp_bind_socket(httpd, m_rpc_ip.c_str(), m_rpc_port);
    HASSERT_THROW_MSG(cb == 0, "evhttp_bind_socket failed", SRC_FAIL);

    evhttp_set_gencb(httpd, httpserver_handler, this);

    g_manage->SetEventBase(m_base);

    HFUN_END;
    HRETURN_OK;
}


HRET CWalletApp::initDataPath() {
    HFUN_BEGIN;

    HCDic curDic = HCDic::GetCurrentDict();
    HCDic rootDic = curDic;
    rootDic.GoToParent();
    rootDic.Append("keys");

    IF_FALSE(HCDic::IsExactExists(rootDic)) {

        HNOTOK_RETURN(HCDic::CreateDic(rootDic.GetName()));

    }

    m_strDataPath = rootDic.GetName();

    LOG_NORMAL("data path: [%s]", m_strDataPath.c_str());

    g_manage->SetDataPath (m_strDataPath);


    HFUN_END;
    HRETURN_OK;
}


void CWalletApp::httpserver_handler(struct evhttp_request *req, void * arg) {
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

        CWalletApp* app = (CWalletApp*)arg;
        pcgi->SetApp(app);

        try {

            HFAILED_THROW(pcgi->Init(rbuf));
            HFAILED_THROW(pcgi->Work());
            res = pcgi->GetRes();
            ss << "{\"ret_code\":\"0\", \"ret_msg\":\"ok\", \"result\":"
               << res <<"}";

        } catch (const HCException& ex) {

            ss << "{\"ret_code\":\"99\", \"ret_msg\":\""
               << ex.what() <<"\"}";

        } catch (...) {

            ss << "{ \"ret_code\":\"999\", \"ret_msg\":\"unkown exception\"  }";

        }
    }

    res = ss.str();

    LOG_NORMAL("cgi response: [%s]", res.c_str());
    evbuffer_add_printf(buf, "%s", res.c_str());

    evhttp_send_reply(req, 200, "OK", buf);

    HFUN_END;
}
