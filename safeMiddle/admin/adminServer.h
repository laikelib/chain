

#ifndef __ADMINSERVER_H__
#define __ADMINSERVER_H__

#include <huibase.h>
#include "../common/middleSocket.h"
#include <hmq.h>
#include <shmInfo.h>



using namespace HUIBASE;

class CAdminApp;
class CAdminServer {
public:
	using shm_info_t = CShmInfo::CInfo;

	static const HUINT SERVER_INST_TIMEOUT = 168;

	static const HUINT LAST_CID = 99999;

public:
	CAdminServer ();
	~ CAdminServer();

	HRET Init () throw (HCException);

	HRET RunServer () throw (HCException);

	HRET ClientTimeOutCheck () throw (HCException);

	HRET CheckShmControl () throw (HCException);

	HRET StopChild (HUINT cid = LAST_CID) throw (HCException);

	HRET StopServer () throw (HCException);

	void SetAdminApp (CAdminApp* app) { m_pAdminApp = app; }

	HBOOL ProcessExists (HUINT cid);

	HRET GetConfInfo (std::vector<HCStr>& vstrs);

private:

	HRET initShmConfig ();

	HRET initMq ();

	HRET initSocket();	
	
private:
	HRET ProcessRecv (HINT iSockNo) throw (HCException);

	HRET AcceptConnect () throw (HCException);

	HRET WorkRequest (HINT iSockNo) throw (HCException);

private:
	HSYS_T m_conf_key{0};
	HUINT m_app_count{0};

	HSYS_T m_mq_key{0};
	HUINT m_mq_bk_count{0};
	HINT m_mq_valid_time{0};
	HINT m_mq_try_time{0};
	HINT m_sleep_time{0};
	HINT m_sleep_timeu{0};

	HINT m_client_len{0};
	HINT m_listen_len{0};
	HINT m_client_time_out{0};

	HSTR m_admin_ip;
	HINT m_admin_port{0};
	

	CMiddleSocket m_listenSocket;
	CMiddleSocket* m_pClients = nullptr;
	
	HCMq* m_pMq = nullptr;
	CShmInfo* m_pShmInfo = nullptr;
	CAdminApp* m_pAdminApp{nullptr};
	
};

#endif //__ADMINSERVER_H__


