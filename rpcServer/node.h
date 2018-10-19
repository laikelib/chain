

#ifndef __NODE_H__
#define __NODE_H__


#include <huibase.h>
#include <hsocket.h>
#include <hmutex.h>
#include <htime.h>

#include <block.h>
#include <serialize.h>

#include <queue>
#include <blockchaininfo.h>

using namespace HUIBASE;

class CSyncMng;
typedef enum {
    mt_ping,
    mt_pong,
    mt_lkinfo_ping,
    mt_lkinfo_pong,

    mt_get_block,
    mt_res_block,

    mt_put_tx,
    mt_tx_result,

    mt_put_bc,
    mt_bc_result,

    mt_end

} MSG_TYPE;

using ENS_MAP = std::map<uint256, CEntry>;
using BLS_MAP = std::map<uint256, CBlock>;

using MEM_ENS = std::pair<ENS_MAP, HCMutex>;
using MEM_BLS = std::pair<BLS_MAP, HCMutex>;

class CNetMessage {
 public:
    CNetMessage (HINT nType, HINT nVersion)
        :m_type(mt_end), m_stream(nType, nVersion), m_nTime(time(nullptr)) { }

    HRET ReadMsg (HCPSZ sz, HUINT nLen);

    HRET SendMsg (HMEMR mem);

    MSG_TYPE GetMsgType () const { return m_type; }

    template<class T>
    HRET SetAsMsg (MSG_TYPE type, const T& msg){
        m_type = type;
        m_stream << msg;
        HRETURN_OK;
    }

    HINT GetReqHeight ();

    void GetNewstInfo (CLaikelibInfo& info) { m_stream >> info; }

    void GetEns (ENS_MAP& ens) { m_stream >> ens; }

    void GetBls (BLS_MAP& bls) { m_stream >> bls; }

    void GetResBl(CBlock& block) { m_stream >> block; }

 private:
    MSG_TYPE m_type;

    CDataStream m_stream;

    HINT m_nTime;
};

class CNode : public HCTcpSocket {
 public:
    CNode ();

    HRET Disconnect ();

 public:
    HTIME GetLastTouchTime () const { return m_timeTouch; }

    HBOOL IsRecvEmpty() ;

    HBOOL IsSendEmpty() ;

    HRET ReadMsg (HCPSZ sz, HUINT nLen);

    void SetError (bool bErr = true) { m_bError = bErr; }

    bool IsError () const { return m_bError; }

    HRET HandleRecv ();

    HRET HandleSend ();

    HRET HandleMsg (CSyncMng* pSync);

    void SetFirstMsg (CSyncMng* pMng) { requestInfoPing(pMng); }

 private:
    void requestInfoPing (CSyncMng* pMng);

    void responseInfoPong (CSyncMng* pMng);

    void sendInfo(CSyncMng* pMng, bool isReq = true);

    void newRequest (CSyncMng* pMng);

    HBOOL broadcastPutBlock (CSyncMng* pMng);

    HBOOL broadcastPubTx (CSyncMng* pMng);

    HBOOL requestGetBlock(CSyncMng* pMng);

    void responseBlock (CSyncMng* pMng);

    void handlePutTxReq (CSyncMng* pMng);

    void handlePutBlReq (CSyncMng* pMng);

    void handleResBl (CSyncMng* pMng);

    void touch () { m_timeTouch = time(nullptr); }

    void insertSend (const CNetMessage& msg);


 private:
    HCMutex m_mutexRecv;
    HCMutex m_mutexSend;
    std::queue<CNetMessage> m_vRecvMsgs;
    std::queue<CNetMessage> m_vSendMsgs;

    HTIME m_timeTouch;

    bool m_bError = false;

};


#endif //__NODE_H__
