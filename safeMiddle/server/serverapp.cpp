
#include <hlog.h>
#include "../common/middleConfig.h"
#include "serverapp.h"

CServerApp::CServerApp() {

	m_pServer = new CServerServer(middle_config->GetValue(PS_SID),
		middle_config->GetValue(PS_CID));
	assert(m_pServer);

}


CServerApp::~CServerApp () {

	HDELP(m_pServer);

}


void CServerApp::init() {

	middle_config->SetConf(m_conf);

	// sos.cfg's path
	HCFileName confName (middle_config->GetValue(PS_CONF));
	HCDic confDic = confName.GetParent();
	HSTR strSosCfg = confDic.GetName() + "/" + "sos.cfg";
	middle_config->SetValue("admin_conf", strSosCfg);	

	HFAILED_THROW(middle_config->LoadSoConfig());

	middle_config->ShowSoConfig();

	HFAILED_THROW(m_pServer->Init());

}



HBOOL CServerApp::Run ()  {

    while (HIS_TRUE(IsRunning())) {

		try {

			m_pServer->RunServer();


		} catch(HCException& ex) {

		}

	}

	LOG_WS("ServerApp is over");

	return HTRUE;
}


void CServerApp::setDeamonApp() {

	LOG_WS("DON'T SET AS DAEMON PROCESS");
		
}


HRET CServerApp::lockApp () {
	HFUN_BEGIN;

	LOG_WS("Server process, we neednot load process.");

	HFUN_END;
	HRETURN_OK;
}




