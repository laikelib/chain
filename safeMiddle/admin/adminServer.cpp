

#include <hlog.h>
#include <hstr.h>
#include <htime.h>
#include <hprocess.h>
#include "adminapp.h"
#include "adminServer.h"
#include "../common/middleConfig.h"
#include <hstr.h>
#include <sys/select.h>
#include "haddr.h"
#include "loadCommand.h"

CAdminServer::CAdminServer () {

	m_mq_key =  HCStr::stoi(middle_config->GetValue("mq_key"));
	
	m_conf_key = HCStr::stoi(middle_config->GetValue("conf_key"));

	m_app_count = HCStr::stoi(middle_config->GetValue("app_count"));

	m_mq_bk_count = HCStr::stoi(middle_config->GetValue("mq_bk_count"));

	m_mq_valid_time = HCStr::stoi(middle_config->GetValue("mq_validTime"));

	m_mq_try_time = HCStr::stoi(middle_config->GetValue("mq_tryTime"));	

	m_sleep_time = HCStr::stoi(middle_config->GetValue("mq_sleepTime"));

	m_sleep_timeu = HCStr::stoi(middle_config->GetValue("mq_SleepTimeu"));

	m_client_len = HCStr::stoi(middle_config->GetValue("client_len"));

	m_listen_len = HCStr::stoi(middle_config->GetValue("listen_len"));

	m_client_time_out = HCStr::stoi(middle_config->GetValue("client_timeout"));

	m_admin_port = HCStr::stoi(middle_config->GetValue("admin_port"));

	m_admin_ip = middle_config->GetValue("admin_ip");
	
}


CAdminServer::~CAdminServer () {

	if (m_pShmInfo != nullptr) {

		m_pShmInfo->Remove();

		delete m_pShmInfo;
	
	}


	if (m_pMq != nullptr) {

		m_pMq->Clear();

		delete m_pMq;
	
	}

	if (m_pClients != nullptr) {

		for (HINT i = 0; i < m_client_len; ++i) {
			m_pClients[i].Close();
		}
		
		delete [] m_pClients;
	
	}

}


HRET CAdminServer::Init () throw (HCException) {
	HFUN_BEGIN;
	/*
	 initialize command class;
	 initialize share memory config;
	 initialize message queue;
	 initialize socket;
	 run listenning socket;
	*/

	// share momory config
	HNOTOK_RETURN(initShmConfig());


	// message queue.
	HNOTOK_RETURN(initMq());


	// init sockets.
	HNOTOK_RETURN(initSocket());
	
	HFUN_END;
	HRETURN_OK;
}

HRET CAdminServer::initShmConfig () {
	HFUN_BEGIN;

	m_pShmInfo = new CShmInfo(m_conf_key, m_app_count);
	assert(m_pShmInfo);
	
	HNOTOK_RETURN(m_pShmInfo->Create());

	HNOTOK_RETURN(m_pShmInfo->Open());

	HNOTOK_RETURN(m_pShmInfo->InitData());

	HFUN_END;
	HRETURN_OK;
	
}

HRET CAdminServer::initMq () {
	HFUN_BEGIN;

	m_pMq = new HCMq(m_mq_key, m_app_count, m_mq_bk_count,
		HCMq::SShmControl(m_mq_valid_time, m_mq_try_time, m_sleep_time,
		m_sleep_timeu));
	assert(m_pMq);

	HNOTOK_RETURN(m_pMq->Init());

	HFUN_END;
	HRETURN_OK;
		
}

HRET CAdminServer::initSocket() {
	HFUN_BEGIN;

	m_pClients = new CMiddleSocket[m_client_len];
	assert(m_pClients);

	HNOTOK_RETURN(m_listenSocket.Init());

	HNOTOK_RETURN(m_listenSocket.SetNonblocked(HTRUE));

	HNOTOK_RETURN(m_listenSocket.SetReuseAddr());

	HCIp4Addr addr(m_admin_ip, m_admin_port);
	HNOTOK_RETURN(m_listenSocket.Bind(addr));
	
	HNOTOK_RETURN(m_listenSocket.Listen(m_listen_len));

	LOG_NORMAL("Listen IP[%s] Port[%d]", m_admin_ip.c_str(), m_admin_port);

	HFUN_END;
	HRETURN_OK;
	
}

HRET CAdminServer::ClientTimeOutCheck () throw (HCException){

	static HTIME tLastCheckTime = 0;
	static const HINT CHECK_INTERVAL = 2;

	time_t _tnow = time(nullptr);
	if (_tnow - tLastCheckTime < CHECK_INTERVAL) {
		HRETURN_OK;
	}

	tLastCheckTime = time(nullptr);
	for (HINT i = 0; i < m_client_len; ++i) {

		CMiddleSocket* pSock = m_pClients + i;
		if (HIS_FALSE(pSock->IsGoodSocket())) {

			continue;
		
		}

		if (_tnow - pSock->GetAccessTime() > m_client_time_out) {

			LOG_WARNNING("client [%s] timeout", pSock->ToString().c_str());
			pSock->Close();
				
		}

	}
	
	HRETURN_OK;
}

HRET CAdminServer::CheckShmControl () throw (HCException) {

	static constexpr HUINT sTimeOut = 600;
	HTIME nowtt = time(nullptr);

	for (HUINT i = 0; i < m_pShmInfo->GetCount(); ++i) {

		const shm_info_t* pInfo = m_pShmInfo->GetInfo(i);

		CShmInfo::HPID_T _pid = pInfo->GetPid();		

		// we only check the running process.
		if (pInfo->GetState() != CShmInfo::si_running) {
			continue;
		}

		HSTR strSoName;
		IF_FALSE (middle_config->HasSo(pInfo->GetCid(), strSoName)) {
			LOG_WARNNING("%d cann't get info", pInfo->GetCid());
			continue;
		}

		LOG_NORMAL("server process cid %d. exists", pInfo->GetCid());

		bool isDowned = false;

		do {
		
			// check timeout;
			if (nowtt - pInfo->GetAccess() > sTimeOut) {
				// timeout
				LOG_WARNNING("conf %d, classid %d timeout", pInfo->GetIndex(), 
					pInfo->GetCid());
				
				isDowned = true;
				break;
				
			}

			// check process exists;			
			IF_FALSE(HCProcess::ProcessExists(_pid)) {

				m_pShmInfo->FreeInfo(i);
			
				LOG_WARNNING("conf %d, classid %d not exists", pInfo->GetIndex(), 
					pInfo->GetCid());
				isDowned = true;
				break;
					
			}
			
			
		} while(0);

		if (isDowned) {
			LOG_WARNNING("Restart %s", strSoName.c_str());
			// set conf to normal
			
		
			// restart server.
			(void)HCProcess::KillProcess(_pid);

			CCommand* cmdobj = maker_facetory::Instance()->newCommand("load");
			std::shared_ptr<CCommand> ptr(cmdobj);		

			cmdobj->Work(strSoName);
			
		}
		
	
	}
	
	HRETURN_OK;
}


HRET CAdminServer::StopChild (HUINT cid) throw (HCException) {

	HFUN_BEGIN;

	for (HUINT i = 0; i < m_pShmInfo->GetCount(); ++i) {

		const shm_info_t* pInfo = m_pShmInfo->GetInfo(i);

		CShmInfo::HPID_T _pid = pInfo->GetPid();		

		// we only check the running process.
		if (pInfo->GetState() == CShmInfo::si_free) {
			continue;
		}		

		if (cid != LAST_CID && pInfo->GetCid() != cid) {
			continue;	
		}

		HSTR strSoName;
		IF_FALSE (middle_config->HasSo(pInfo->GetCid(), strSoName)) {
			LOG_WARNNING("%d cann't get info", pInfo->GetCid());
			continue;
		}

		LOG_WARNNING("Stop process %d at conf[%d]", _pid, i);

		(void)HCProcess::KillProcess(_pid);

		m_pShmInfo->ClearStateConf(pInfo->GetIndex());		
		
	
	}

	HFUN_END;

	HRETURN_OK;
}


HRET CAdminServer::StopServer () throw (HCException) {
	HFUN_BEGIN;

	HASSERT_THROW_MSG(m_pAdminApp != nullptr,"m_pAdminApp is nullptr", ILL_PT);

	m_pAdminApp->Stop();

	HFUN_END;

	HRETURN_OK;
}

HBOOL CAdminServer::ProcessExists (HUINT cid) {

	for (HUINT i = 0; i < m_pShmInfo->GetCount(); ++i) {
		
		const shm_info_t* pInfo = m_pShmInfo->GetInfo(i);

		// we only check the running process.
		if (pInfo->GetState() == CShmInfo::si_free) {
			continue;
		}			

		if (pInfo->GetCid() == cid) {
			return HTRUE;
		}

	}

	return HFALSE;

}


HRET CAdminServer::GetConfInfo (std::vector<HCStr>& vstrs) {

	HCStr title;
	title.appendFormat("\n%-20s%-15s%-30s%-30s\n", "So Name", "Status", 
		"CreateTime","ModifyTime");
	vstrs.push_back(title);

	auto __status_name = [=](CShmInfo::si_state st) -> HSTR {
		switch(st) {
			case CShmInfo::si_free:
				return "free";
				break;
			case CShmInfo::si_stop:
				return "stop";
				break;
			case CShmInfo::si_running:
				return "running";
				break;
			default:
				return "unknown";
				break;
		}
		return "unknown"; 
	};

	for (HUINT i = 0; i < m_pShmInfo->GetCount(); ++i) {
		
		const shm_info_t* pInfo = m_pShmInfo->GetInfo(i);

		// we only check the running process.
		if (pInfo->GetState() == CShmInfo::si_free) {
			continue;
		}	

		HSTR strSoName;
		IF_FALSE (middle_config->HasSo(pInfo->GetCid(), strSoName)) {
			LOG_WARNNING("%d cann't get info", pInfo->GetCid());
			continue;
		}

		HCTime ct(pInfo->GetCTime());
		HCTime mt(pInfo->GetAccess());

		HCStr str;
		str.appendFormat("%-20s%-15s%-30s%-30s\n", strSoName.c_str(), __status_name(pInfo->GetState()).c_str(),
			ct.tostr().c_str(), mt.tostr().c_str());

		vstrs.push_back(str);

	}
	
	HRETURN_OK;
}


HRET CAdminServer::RunServer () throw (HCException) {

	fd_set read_set;
	int max_fds = m_listenSocket.GetSocket();
	FD_ZERO(&read_set);
	FD_SET(m_listenSocket.GetSocket(), &read_set);

	for (HINT i = 0; i < m_listen_len; ++i) {

		CMiddleSocket* pSock = m_pClients + i;

		if (HIS_TRUE(pSock->IsGoodSocket())) {

			FD_SET(pSock->GetSocket(), &read_set);

			max_fds = HMAX(pSock->GetSocket(), max_fds);
		
		}
	
	}

	timeval tv = {0,0};
	tv.tv_sec = 5;
	//tv.tv_usec = 500000;
	tv.tv_usec = 0;

	auto ret = select (max_fds + 1, &read_set, nullptr, nullptr, &tv);
	if (ret > 0) {

		for (HINT i = 0; i < m_client_len; ++i) {

			CMiddleSocket* pSock = m_pClients + i;

			if (HIS_FALSE(pSock->IsGoodSocket())) {

				continue;
			
			}

			if (FD_ISSET(pSock->GetSocket(), &read_set)) {
				ProcessRecv(i);
			}

		}

		if (FD_ISSET(m_listenSocket.GetSocket(), &read_set)) {
				AcceptConnect();
		}
	
	}
	
	HRETURN_OK;
}


HRET CAdminServer::ProcessRecv (HINT iSockNo) throw (HCException) {
	HFUN_BEGIN;

	CMiddleSocket* pSock = m_pClients + iSockNo;
	pSock->SetAccessTime(time(nullptr));

	try {

		CMiddleSocket::SOCK_RET ret = pSock->ReadRequest();

		switch (ret){
		case CMiddleSocket::SR_CLOSE:
			LOG_NORMAL("%s close", pSock->ToString().c_str());
			pSock->Close();
			HRETURN_OK;
			break;
		case CMiddleSocket::SR_OK:
			break;
		case CMiddleSocket::SR_ERROR:
			LOG_WARNNING("%s error", pSock->ToString().c_str());
			pSock->Close();
			HRETURN(ERR_STATUS);
			break;
		}


	} catch(HCException& e) {

		LOG_ERROR("Client Socket [%s] Recv Get A Exception [%s].", 
			pSock->ToString().c_str(), e.what());
		pSock->Close();
	}

	auto wcb = WorkRequest(iSockNo);
	HIF_NOTOK(wcb){
		LOG_ERROR("socket work failed. response[%s]", pSock->GetData());
		(void)pSock->SendResponse();
		pSock->Close();
		return wcb;
	}

	auto cb = pSock->SendResponse();
	if (cb != CMiddleSocket::SR_OK) {
		pSock->Close();
		HRETURN(SYS_FAILED);
	}

	HFUN_END;
	HRETURN_OK;
}


HRET CAdminServer::AcceptConnect () throw (HCException) {

	HCIp4Addr addr{0};

	for (HINT i = 0; i < m_client_len; ++i) {

		CMiddleSocket* pSock = m_pClients + i;

		if (HIS_FALSE( pSock->IsGoodSocket() )) {
			
			m_listenSocket.Accept (*pSock, addr);

			pSock->SetNonblocked(HTRUE);

			pSock->SetAccessTime(time(nullptr));

			LOG_NORMAL("New Client Connection: %s", pSock->ToString().c_str());

			HRETURN_OK;
			
		}
	
	}

	CMiddleSocket _sock;
	m_listenSocket.Accept(_sock, addr);
	_sock.Close();

	HRETURN(SRC_FAIL);
}


HRET CAdminServer::WorkRequest (HINT iSockNo) throw (HCException) {

	static const HSTR s_inva_para ("Invalid Params");
	static const HSTR s_pos_end ("Not This Command");

	CMiddleSocket * pSock = m_pClients + iSockNo;
	assert(pSock);

	HSTR strRequest(pSock->GetData());	

	std::vector<HSTR> cs;
	HCStr::Trim(strRequest);

	LOG_NORMAL("Recv Request [%s]", strRequest.c_str());	
	
	HCStr::Split(strRequest, " ", cs); 

	if (cs.empty()) {
		pSock->SetResponse(s_inva_para);
		LOG_ERROR("%s invalid params. [NOT CMD]", pSock->ToString().c_str());
		HRETURN(INVL_PARA);
	}

	HCSTRR strCmd = cs[0];
	strRequest.clear();
	
	if (cs.size() > 1) {
		strRequest = cs[1];
	}

	CCommand* cmdobj = maker_facetory::Instance()->newCommand(strCmd);
	std::shared_ptr<CCommand> ptr(cmdobj);
	if (cmdobj == nullptr) {
		pSock->SetResponse(s_pos_end);
		LOG_ERROR("%s Cann't find cmd[%s].", pSock->ToString().c_str(), strCmd.c_str());
		HRETURN(POS_END);
	}

	cmdobj->SetAdmin(this);

	try {
		HRET cb = cmdobj->Work(strRequest);
		HIF_NOTOK(cb) {
			pSock->SetResponse(strRequest + ". BaseMsg: '" + hbase_err(cb) + "'");
			LOG_ERROR("%s Work with logic error.", pSock->ToString().c_str());
			HRETURN(LOGIC_ERR);
		}
	} catch (HCException& e) {
		LOG_ERROR("admin command work throw a exception: %s", e.what());
		HRETURN(EX_GET);
	}	
	
	pSock->SetResponse("Success." + strRequest);
	
	HRETURN_OK;
}



