



#include "syncmng.h"
#include <hlog.h>

#include <sys/select.h>
#include <json/json.h>


CSyncMng::CSyncMng (bool bBind)
    : CLkInfoCgi("sync"), m_bBind(bBind){

}


CSyncMng::CSyncMng(HCSTRR strBindIp, HUINT nBindPort, HUINT nListenLen, bool bBind)
    : CLkInfoCgi("sync"), m_bBind(bBind), m_strBindIp (strBindIp), m_nBindPort (nBindPort), m_nListenLen(nListenLen) {

}


CSyncMng::~CSyncMng () {

    unInit();

}


HRET CSyncMng::Init() throw (HCException) {
    HFUN_BEGIN;

    HNOTOK_RETURN(bind());

    HNOTOK_RETURN(getInfo());

    HFUN_END;
    HRETURN_OK;
}


HRET CSyncMng::ReConnect() throw (HCException) {

    bool bEm = false;

    MUTEXHOLDER holder(m_mutexNodes);
    bEm = m_nodes.empty();

    if (bEm && not m_main_ip.empty()) {

        LOG_NORMAL("reconnect to [%s:%d]", m_main_ip.c_str(), m_main_port);

        CNode* pnode = new CNode();
        try {

            HNOTOK_RETURN(pnode->Init());
            HCIp4Addr addr(m_main_ip, m_main_port);
            HNOTOK_RETURN(pnode->ConnectWithTimeOut(addr));
            pnode->SetNonblocked();
            pnode->SetFirstMsg(this);

            m_nodes.push_back(pnode);

        } catch (...) {

            delete pnode;

        }
    }

    HRETURN_OK;
}


HRET CSyncMng::UpdateInfo() throw (HCException) {

    MUTEXHOLDER holder(m_mutexInfo);

    getInfo();

    HRETURN_OK;
}


CLaikelibInfo CSyncMng::GetLkInfo() {

    MUTEXHOLDER holder(m_mutexInfo);

    return m_info;

}


HRET CSyncMng::HandleNet() throw (HCException) {

    (void) heartCheck();

    (void) handleNet();

    HRETURN_OK;
}


ENS_MAP CSyncMng::GetEns() {

    MUTEXHOLDER hold(m_ens.second);

    ENS_MAP res = m_ens.first;
    m_ens.first.clear();
    return res;

}


BLS_MAP CSyncMng::GetBls () {

    MUTEXHOLDER hold(m_bls.second);

    BLS_MAP res = m_bls.first;
    m_bls.first.clear();
    return res;
}


void CSyncMng::NewEn(const CEntry &en) {

    MUTEXHOLDER hold(m_ens.second);

    m_ens.first.insert(make_pair(en.GetEntryHash(), en));

}


void CSyncMng::NewBl(const CBlock &bl) {

    MUTEXHOLDER hold(m_bls.second);

    m_bls.first.insert(make_pair(bl.GetHash(), bl));

}


void CSyncMng::unInit() {



}


HRET CSyncMng::bind() throw (HCException) {
    HFUN_BEGIN;

    HNOTOK_RETURN(m_service.Init());

    HNOTOK_RETURN(m_service.SetReuseAddr());

    HNOTOK_RETURN(m_service.SetNonblocked());

    HCIp4Addr addr(m_strBindIp, m_nBindPort);
    HNOTOK_RETURN(m_service.Bind(addr));

    HNOTOK_RETURN(m_service.Listen(m_nListenLen));

    HFUN_END;
    HRETURN_OK;
}


HRET CSyncMng::getInfo() throw (HCException) {

    Work ();

    Json::Reader reader;
    Json::Value root;

    HASSERT_RETURN(reader.parse(m_res, root), INVL_RES);

    m_info.SetVersion(root["version"].asString());
    m_info.SetBestHash(root["bestHash"].asString());
    m_info.SetHeight (root["height"].asInt());
    m_info.SetGenesisHash(root["genesisHash"].asString());

    LOG_NORMAL("version: [%s], bestHash: [%s], height: [%d], genesisHash: [%s]", m_info.GetVersion().c_str(), m_info.GetBestHash().c_str(), m_info.GetHeight(), m_info.GetGenesisHash().c_str());

    HRETURN_OK;
}


HRET CSyncMng::heartCheck() throw (HCException) {

    /*
    static HTIME tLastCheckTime = 0;
    static constexpr HINT CHECK_INTERVAL = 60;
    static constexpr HINT MAX_UNTOUCH_TIME = 600;

    if (_tnow - tLastCheckTime < CHECK_INTERVAL) {
        HRETURN_OK;
    }
    tLastCheckTime = time(nullptr);*/
    //HTIME _tnow = time(nullptr);

    //LOG_NORMAL("there are %d connections", m_nodes.size());

    std::vector<CNode*> set_nodes;
    for (size_t i = 0; i < m_nodes.size(); ++i) {

        CNode* pNode = m_nodes[i];
        if (pNode->IsError()) {
            set_nodes.push_back(pNode);
        }

    }

    for (size_t i = 0; i < m_nodes.size(); ++i) {

        CNode* pNode = m_nodes[i];

        IF_FALSE(pNode->IsGoodSocket()) {

            set_nodes.push_back(pNode);
            continue;

        }

        /*if (_tnow - pNode->GetLastTouchTime() > MAX_UNTOUCH_TIME) {

            set_nodes.push_back(pNode);

            continue;
            }*/

    }

    if (set_nodes.size() > 0) {
        LOG_WARNNING("[%d] need to close", set_nodes.size());
    }

    MUTEXHOLDER _holder(m_mutexNodes);
    for (size_t i = 0; i < set_nodes.size(); ++i) {

        CNode* pNode = set_nodes[i];

        m_nodes.erase(std::remove(m_nodes.begin(), m_nodes.end(), pNode), m_nodes.end());

        LOG_NORMAL("remoce node [%p]", pNode);
        delete pNode;

    }

    HRETURN_OK;
}


HRET CSyncMng::handleNet() throw (HCException) {
    struct timeval timeout {1, 0};

    fd_set fsRead;
    fd_set fsSend;
    fd_set fsError;

    FD_ZERO(&fsRead);
    FD_ZERO(&fsSend);
    FD_ZERO(&fsError);

    int maxSocket = 0;
    FD_SET(m_service.GetSocket(), &fsRead);
    maxSocket = HMAX(m_service.GetSocket(), maxSocket);

    NODES nodes = m_nodes;
    for (size_t i = 0; i < nodes.size(); ++i) {

        CNode* pNode = nodes[i];

        IF_FALSE(pNode->IsSendEmpty()) {
            FD_SET(pNode->GetSocket(), &fsSend);
        } else {
            FD_SET(pNode->GetSocket(), &fsRead);
        }

        maxSocket = HMAX(pNode->GetSocket(), maxSocket);

    }

    auto scb = select(maxSocket + 1, &fsRead, &fsSend, &fsError, &timeout);
    //LOG_NORMAL("select return [%d]", scb);
    (void)scb;

    // accept new connection
    if (FD_ISSET(m_service.GetSocket(), &fsRead)) {

        HCIp4Addr addr{0};
        CNode* pNew = new CNode();
        m_service.Accept(*pNew, addr);
        pushNodes(pNew);

        LOG_NORMAL("new client connection: %s", pNew->ToString().c_str());
    }

    // receive and send
    do {

        for (size_t i = 0; i < nodes.size(); ++i) {

            CNode* pNode = nodes[i];

            if (FD_ISSET(pNode->GetSocket(), &fsRead) || FD_ISSET(pNode->GetSocket(), &fsError)) {

                pNode->HandleRecv();

            }

            if (FD_ISSET(pNode->GetSocket(), &fsSend)) {

                pNode->HandleSend();

            }

        }

    } while(0);

    HRETURN_OK;
}


HRET CSyncMng::HandleWork() throw (HCException) {

    bool bem = false;
    do {

        MUTEXHOLDER _holder(m_mutexNodes);

        for (size_t i = 0; i < m_nodes.size(); ++i) {

            CNode* pNode = m_nodes[i];

            pNode->HandleMsg(this);

        }

        bem = m_nodes.empty();

    } while (0);

    if (bem) {
        sleep(1);
    }

    HRETURN_OK;

}


void CSyncMng::copyNodes(NODES& nodes) {

    MUTEXHOLDER holder(m_mutexInfo);

    nodes = m_nodes;

}


void CSyncMng::pushNodes(CNode *pNode) {

    MUTEXHOLDER holder(m_mutexNodes);

    m_nodes.push_back(pNode);

}
