

#include <hlog.h>
#include "node.h"
#include <hadaptation.h>
#include "syncmng.h"

#include <hcrypto.h>

using namespace HUIBASE::CRYPTO;

HRET CNetMessage::ReadMsg(HCPSZ sz, HUINT nLen) {

    HASSERT_RETURN(nLen > 4, INVL_PARA);

    HINT nn = *((HINT*)sz);
    m_type = (MSG_TYPE)nn;
    HCPSZ pos = sz + sizeof(HINT);
    nLen -= sizeof(HINT);

    m_stream.write(pos, nLen);

    m_nTime = time(nullptr);

    HRETURN_OK;
}



HRET CNetMessage::SendMsg(HMEMR mem) {

    mem.clear();
    mem.resize(sizeof(HINT));

    HINT n = m_type;
    std::memcpy(mem.data(), &n, sizeof(n));

    hstr_uvs(m_stream.str(), mem);

    m_nTime = time(nullptr);

    HRETURN_OK;
}



HINT CNetMessage::GetReqHeight() {

    if (m_type != mt_get_block) {
        return -1;
    }

    HINT res;
    m_stream >> res;

    return res;
}


CNode::CNode () {

    m_timeTouch = time(nullptr);

    m_bError = false;

}



HRET CNode::Disconnect() {

    Close();

    // In case this fails, we will still clear the recv buffer and
    // send buffer when the node is deleted;

    HRETURN_OK;
}



HBOOL CNode::IsRecvEmpty()  {

    MUTEXHOLDER holder(m_mutexRecv);

    HRET_BOOL(m_vRecvMsgs.empty());

}


HBOOL CNode::IsSendEmpty()  {

    MUTEXHOLDER holder(m_mutexSend);

    HRET_BOOL(m_vSendMsgs.empty());

}



HRET CNode::ReadMsg(HCPSZ sz, HUINT nLen) {

    MUTEXHOLDER holder(m_mutexRecv);

    CNetMessage nm(SER_NETWORK, NODE_VERSION);
    nm.ReadMsg(sz, nLen);

    m_vRecvMsgs.push(nm);

    touch();

    HRETURN_OK;
}


HRET CNode::HandleRecv() {

    //typical socket buffer is 8k-64k
    HCHAR buf[0x10000] = {0};

    HRET ret = HERR_NO(OK);
    HSYS_RET cb = 0;
    try {

        cb = Recv(buf, sizeof(buf), MSG_DONTWAIT);

    }  catch(const HCException& ex) {

        LOG_ES("socket recv failed");
        SetError(true);
        ret = HERR_NO(NET_RES);

    }

    if (cb == 0) {

        LOG_WARNNING("socket [%s] close", ToString().c_str());
        SetError(true);
        ret = HERR_NO(NET_RES);

    }

    if (cb <= 4) {

        LOG_ERROR("socket [%s] recv error", ToString().c_str());
        SetError(true);
        ret = HERR_NO(NET_RES);

    }

    HIF_NOTOK(ret) {

        LOG_WS("socket is error. RETURN");
        return ret;

    }

    ReadMsg(buf, cb);

    HRETURN_OK;
}


HRET CNode::HandleSend() {

    MUTEXHOLDER holder(m_mutexSend);

    while (not m_vSendMsgs.empty()) {

        CNetMessage& nm = m_vSendMsgs.front();

        HMEM mem;
        nm.SendMsg(mem);

        try {

            (void) Send(mem.data(), mem.size(), MSG_NOSIGNAL | MSG_DONTWAIT);

        } catch (const HCException& ex) {

            SetError();
            break;

        }

        m_vSendMsgs.pop();

    }

    HRETURN_OK;
}


HRET CNode::HandleMsg (CSyncMng* pMng) {

    while (not m_vRecvMsgs.empty()) {

        CNetMessage& cnm = m_vRecvMsgs.front();

        switch (cnm.GetMsgType()) {
        case mt_ping:
        case mt_pong:
        case mt_lkinfo_ping: {

            // lkinfo;
            responseInfoPong(pMng);

            break;

        }

            // TODO: need more carefully.
        case mt_tx_result:
        case mt_bc_result:
            //case mt_res_block:
        case mt_end:
        case mt_lkinfo_pong: {

            newRequest(pMng);

            break;
        }

        case mt_get_block : {

            LOG_NORMAL("%s handleGetBlock", ToString().c_str());
            responseBlock(pMng);
            pMng->UpdateInfo();
            break;

        }

        case mt_put_tx: {
            // new txs?
            LOG_NORMAL("%s handle save tx", ToString().c_str());
            handlePutTxReq(pMng);
            responseInfoPong(pMng);
            break;
        }

        case mt_res_block: {
            handleResBl(pMng);
            pMng->UpdateInfo();
            requestInfoPing(pMng);
            break;
        } 

        case mt_put_bc: {
            // new block?
            handlePutBlReq(pMng);
            requestInfoPing(pMng);
            break;
        }

            break;
        }

        m_vRecvMsgs.pop();

    }

    HRETURN_OK;
}


void CNode::requestInfoPing(CSyncMng *pMng) {

    sendInfo(pMng, true);

}


void CNode::responseInfoPong(CSyncMng* pMng) {

    sendInfo(pMng, false);

}


void CNode::sendInfo(CSyncMng *pMng, bool isReq) {

    CNetMessage msg(SER_NETWORK, NODE_VERSION);
    CLaikelibInfo info = pMng->GetLkInfo();

    if (isReq) {
        msg.SetAsMsg(mt_lkinfo_ping, info);
    } else {
        msg.SetAsMsg(mt_lkinfo_pong, info);
    }

    insertSend(msg);

}


void CNode::newRequest(CSyncMng *pMng) {

    /*IF_TRUE(broadcastPutBlock(pMng)) {

        LOG_NORMAL("%s broadcastPutBlock for new block", ToString().c_str());

        return ;

        }*/

    IF_TRUE(broadcastPubTx(pMng)) {

        LOG_NORMAL("%s broadcastPubTx for new tx", ToString().c_str());

        return;

    }

    IF_TRUE(requestGetBlock(pMng)) {

        LOG_NORMAL("%s requestGetBlock for new tx", ToString().c_str());

        return;

    }

    requestInfoPing(pMng);

}


HBOOL CNode::broadcastPutBlock(CSyncMng *pMng) {

    BLS_MAP res_map = pMng->GetBls();

    if (res_map.empty()) {
        return HFALSE;
    }

    CNetMessage msg(SER_NETWORK, NODE_VERSION);
    msg.SetAsMsg(mt_put_bc, res_map);
    insertSend(msg);

    return HTRUE;
}


HBOOL CNode::broadcastPubTx(CSyncMng *pMng) {

    ENS_MAP res_map = pMng->GetEns();

    if (res_map.empty()) {
        return HFALSE;
    }

    CNetMessage msg(SER_NETWORK, NODE_VERSION);
    msg.SetAsMsg(mt_put_tx, res_map);
    insertSend(msg);

    return HTRUE;

}


HBOOL CNode::requestGetBlock(CSyncMng *pMng) {

    CNetMessage& cnm = m_vRecvMsgs.front();

    CLaikelibInfo new_info;
    cnm.GetNewstInfo(new_info);

    CLaikelibInfo me_info = pMng->GetLkInfo();

    //LOG_NORMAL("myinfo[%s] youinfo[%s]", me_info.ToString().c_str(), new_info.ToString().c_str());

    HINT nNeedHeight = me_info.GetHeight() + 1;
    if (nNeedHeight > new_info.GetHeight()) {

        return HFALSE;

    }

    CNetMessage msg(SER_NETWORK, NODE_VERSION);

    LOG_NORMAL("requestGetBlock need height: [%d]", nNeedHeight);
    msg.SetAsMsg(mt_get_block, (HINT)nNeedHeight);

    insertSend(msg);

    return HTRUE;

}


void CNode::responseBlock(CSyncMng *pMng) {

    CNetMessage& cnm = m_vRecvMsgs.front();

    HINT nHeight = cnm.GetReqHeight();
    LOG_NORMAL("request height: [%d]", nHeight);
    HASSERT_THROW_MSG(nHeight >0, "request get block, nHeight is invalid", INVL_PARA);

    HSTR hid = HCStr::Format("%d", nHeight);
    HFAILED_MSG(pMng->sendAndRecv("height_block", hid), "call laikelib height_block failed");
    LOG_NORMAL("laikelib height_block return: [%s]", hid.c_str());

    if (hid == "DROP") {

        responseInfoPong(pMng);
        return;

    }

    HSTR src;
    HFAILED_MSG(HDecode(hid, src), "decode failed");

    HBUF hbuf;
    hstr_vs(src, hbuf);
    CDataStream ds(hbuf.begin(), hbuf.end(), SER_NETWORK, NODE_VERSION);

    CBlock block;
    ds >> block;

    LOG_NORMAL("response block: [%s]", block.ToJsonString().c_str());

    CNetMessage msg(SER_NETWORK, NODE_VERSION);
    msg.SetAsMsg(mt_res_block, block);

    insertSend(msg);

}


void CNode::handlePutTxReq(CSyncMng *pMng) {

    ENS_MAP req;
    CNetMessage& cnm = m_vRecvMsgs.front();

    cnm.GetEns(req);

    for (ENS_MAP::iterator it = req.begin(); it != req.end(); ++it) {

        CEntry& en = it->second;
        CDataStream ds(SER_NETWORK, NODE_VERSION);
        ds << en;

        HSTR str;
        HFAILED_MSG(HEncode(ds.str(), str), "encode result failed");

        HFAILED_THROW(pMng->sendAndRecv("new_tx", str));

    }

    responseInfoPong(pMng);

}


void CNode::handlePutBlReq(CSyncMng *pMng) {

    BLS_MAP req;
    CNetMessage& cnm = m_vRecvMsgs.front();

    cnm.GetBls(req);

    for (BLS_MAP::iterator it = req.begin(); it != req.end(); ++it) {

        CBlock& bl = it->second;
        CDataStream ds(SER_NETWORK, NODE_VERSION);
        ds << bl;

        HSTR str;
        HFAILED_MSG(HEncode(ds.str(), str), "encode result failed");

        HFAILED_THROW(pMng->sendAndRecv("post_work", str));

    }

}


void CNode::handleResBl (CSyncMng* pMng) {
    using namespace HUIBASE::CRYPTO;

    CNetMessage& cnm = m_vRecvMsgs.front();

    CBlock blk;
    cnm.GetResBl(blk);

    //LOG_NORMAL("handle block: [%s]", blk.ToJsonString().c_str());
    CDataStream ds(SER_NETWORK, NODE_VERSION);
    ds << blk;

    HSTR str;
    HFAILED_MSG(HEncode(ds.str(), str), "decode failed");

    HFAILED_THROW(pMng->sendAndRecv("post_work", str));

}


void CNode::insertSend(const CNetMessage &msg) {

    MUTEXHOLDER holder(m_mutexSend);

    m_vSendMsgs.push(msg);
}
