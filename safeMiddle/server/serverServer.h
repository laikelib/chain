
#ifndef __SERVERSERVER_H__
#define __SERVERSERVER_H__


#include <huibase.h>
#include <happ.h>
#include <hmq.h>
#include <shmInfo.h>
#include "../common/middleParams.h"
#include "serverso.h"

using namespace HUIBASE;



class CServerServer {
public:
	using MqControl = HCMq::CMsgControl;
	using CINFO = CShmInfo::CInfo;
	static constexpr HUINT BUF_LEN = HLEN1024_C;
	static constexpr HUINT TIMEOUT = 5;
	static constexpr HUINT SERVER_REQ_PS_COUNT = 2;

	typedef enum {
		rf_return = 1<<0,

		rf_end,
	} mq_msg_flags;

	inline bool MSG_NEED_RETURN (HINT flag) { return flag && rf_return; }
		
public:
	CServerServer (HCSTRR strSid, HCSTRR strCid);
	~CServerServer ();

	HRET Init () throw (HCException);

	HRET RunServer () throw (HCException);

private:
	HRET setupParam () ;

	HRET checkConf ();

	HRET runMsg () throw (HCException);

	HRET runHeartBeat () throw (HCException);

	HRET recvRequest () throw (HCException);

	HRET sendResponse () throw (HCException);

private:
	HRET readConf () throw (HCException);

	HRET initShmInfo () throw (HCException);

	HRET initMq () throw (HCException);

	HRET initSo () throw (HCException);
	
	
private:
	HSTR  m_strSid;
	HUINT m_sid {0};
	HSTR m_strCid;
	HUINT m_cid{0};
	
	HUINT m_shmIndex{0};

	HSTR m_strSoName;
	SSoEntry m_soEntry;

	MqControl m_ctl;
	HCHAR m_buf[BUF_LEN]{0};
	SCM_DATA m_data;
	
	pid_t m_pid;
	HINT m_shm_index{0};

	HSYS_T m_conf_key{0};
	HUINT m_app_count{0};

	HSYS_T m_mq_key{0};
	HUINT m_mq_bk_count{0};
	HINT m_mq_valid_time{0};
	HINT m_mq_try_time{0};
	HINT m_sleep_time{0};
	HINT m_sleep_timeu{0};

	CServerSo m_so;

	HCMq* m_pMq = nullptr;
	CShmInfo* m_pShmInfo = nullptr;
};


#endif 


