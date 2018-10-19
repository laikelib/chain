


#ifndef __SYNCMNG_H__
#define __SYNCMNG_H__

#include <huibase.h>
#include <map>
#include <utility>
#include <blockchaininfo.h>
#include <entry.h>

#include "rpcapp.h"
#include "node.h"
#include "lkinfo.h"

using namespace HUIBASE;

class CNode;
class CSyncMng : public CLkInfoCgi{
 public:
    using NODES = std::vector<CNode*>;

 public:
    CSyncMng (bool bBind = false);

    CSyncMng (HCSTRR strBindIp, HUINT nBindPort, HUINT nListenLen = 32, bool bBind = true);

    ~ CSyncMng ();

    HRET ConnectTo (HCSTRR strIp, HUINT nPort);

    HRET Init () throw (HCException);

    HRET ReConnect () throw (HCException);

    HRET UpdateInfo () throw (HCException);

    CLaikelibInfo GetLkInfo ();

    HRET HandleNet() throw (HCException);

    HRET HandleWork () throw (HCException);

    ENS_MAP GetEns();

    BLS_MAP GetBls();

    void NewEn (const CEntry& en);

    void NewBl (const CBlock& bl);

    void SetMainIp (HCSTRR strIp) { m_main_ip = strIp; }

    void SetMainPort (HINT nPort) { m_main_port = nPort; }

 private:
    void unInit ();

    HRET bind () throw (HCException);

    HRET getInfo () throw (HCException);

    HRET heartCheck () throw (HCException);

    HRET handleNet () throw (HCException);

    void copyNodes (NODES& nodes);

    void pushNodes (CNode* pNode);

 private:
    bool m_bBind = true;
    HSTR m_strBindIp;
    HUINT m_nBindPort = 10100;
    HUINT m_nListenLen = 32;

    HCTcpSocket m_service;

    HCMutex m_mutexNodes;
    NODES m_nodes;

    CLaikelibInfo m_info;
    HCMutex m_mutexInfo;

    MEM_ENS m_ens;
    MEM_BLS m_bls;

    HSTR m_main_ip;
    HINT m_main_port;


    //RpcMq * m_mq = nullptr;

};


#endif
