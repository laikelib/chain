
#include <hlog.h>
#include "testserver.h"
#include <hipcdata.h>
#include "../safeMiddle/common/middleConfig.h"
#include <uint256.h>
#include <bignum.h>

CTestServer::CTestServer() {

    

}


CTestServer::~CTestServer() {




}


HRET CTestServer::Init() throw (HCException) {
    HFUN_BEGIN;

    /*
      > load xml config;
      > init shminfo, mq;
      > run
          test: call
	  
     */

    HNOTOK_RETURN(setupParam());

    HNOTOK_RETURN(initMq ());    

    HFUN_END;

    HRETURN_OK;
}

HRET CTestServer::setupParam() {

    m_mq_key = middle_config->GetInt("mq_key", 1688);

    m_app_count = middle_config->GetInt("app_count", 32);

    m_mq_bk_count = middle_config->GetInt("mq_bk_count", 8);

    m_mq_valid_time = middle_config->GetInt("mq_validTime", 600);

    m_mq_try_time = middle_config->GetInt("mq_tryTime", 5);

    m_sleep_time = middle_config->GetInt("mq_sleepTime", 0);

    m_sleep_timeu = middle_config->GetInt("mq_SleepTimeu", 5);

    LOG_NORMAL("mq_key: [%d], app_count: [%d], mq_bk_count: [%d], mq_valid_time: [%d], mq_try_time: [%d], sleep_time: [%d], sleep_timeu: [%d]", m_mq_key, m_app_count, m_mq_bk_count, m_mq_valid_time, m_mq_try_time, m_sleep_time, m_sleep_timeu);
    
    HRETURN_OK;

}



HRET CTestServer::initMq() throw (HCException) {
    HFUN_BEGIN;

    m_pMq = new HCMq (m_mq_key, m_app_count, m_mq_bk_count, HCMq::SShmControl(m_mq_valid_time, m_mq_try_time, m_sleep_time, m_sleep_timeu));

    assert(m_pMq);

    HNOTOK_RETURN(m_pMq->Open());

    HFUN_END;
    HRETURN_OK;
}


HRET CTestServer::SendRecvMq(HSTRR str) throw (HCException) {
    HFUN_BEGIN;
    
    static constexpr HINT CLEN = 10240;
    static char rbuf[CLEN] = {0};
    memset(rbuf, 0, CLEN);

    std::vector<HSTR> ps;
    ps.push_back("block_template");
    ps.push_back(str);

    HUINT len = 0;
    HPSZ buf = HCIpcData::MakeBuf (ps, len);
    if (buf == nullptr) {
	LOG_ES("HCIpcData::MakeBuf make a null pointer");
	HRETURN(ILL_PT);
    }

    LOG_NORMAL("buf length: [%d]", len);

    HCMq::CMsgControl control(1, 0, len, time(nullptr));
    control.SetType (89);

    bool bfailed = false;
    do {

	LOG_NORMAL("send contro: {type:[%d], nid: [%d], bid: [%d], len: [%d]}", control.GetType(), control.GetNid(), control.GetBid(), control.GetLen());
	
	HRET cb = m_pMq->Send(control, buf, 0);
    
	HIF_NOTOK(cb) {
	    LOG_ES("send message queue failed");
	    bfailed = true;
	    break;
	}

	control.SetType(289);
	control.SetNid(3);
	control.SetLen(CLEN);
	control.SetCTime(time(nullptr));
	cb = m_pMq->Recv(control, rbuf, 5, 0);

	LOG_NORMAL("return control: {type: [%d], nid: [%d], bid: [%d], len: [%d]}", control.GetType(), control.GetNid(), control.GetBid(), control.GetLen());

	HIF_NOTOK(cb) {

	    LOG_ES("recv message queue failed");
	    bfailed = true;
	    break;

	}

	std::vector<HCIpcData> req_datas;
	HCIpcData::ParseDatas(rbuf, control.GetLen(), req_datas);
	LOG_NORMAL("req_datas size: [%d]", req_datas.size());

	const HCIpcData& res = req_datas[0];

	m_strBlockTemp = res.GetString();
	
	LOG_NORMAL("mq return: [%s]", m_strBlockTemp.c_str());
	
    } while (0);

    free(buf);

    if(bfailed) {
	HRETURN(ERR_STATUS);
    }

    HFUN_END;
    HRETURN_OK;
}



