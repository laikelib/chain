
#include <sys/types.h>
#include <sys/wait.h>
#include <hlog.h>
#include "adminapp.h"

#include "../common/middleConfig.h"

CAdminApp::CAdminApp(HINT argc, const HCHAR* argv[])
	: HCApp(argc, argv){


}


CAdminApp::~CAdminApp() {

	HDELP(m_server);

}


HBOOL CAdminApp::Run () {

	while (IsRunning()) {

		try {

			(void)m_server->ClientTimeOutCheck();

			(void)m_server->CheckShmControl();

			(void)m_server->RunServer();

		} catch(HCException& e ) {

			LOG_ERROR("CAdminApp::Run get a exception: [%s]", e.what());

		}

	}

	LOG_WS("Adminapp stop...");

	return HTRUE;

}


void CAdminApp::init() {
	HFUN_BEGIN;

	signal(SIGCHLD, chlsig_handler);

	// save config to g_conf.
	middle_config->SetConf(m_conf);

	HFAILED_THROW(middle_config->LoadSoConfig());

	middle_config->ShowSoConfig();

	m_server = new CAdminServer;

	m_server->SetAdminApp(this);

	HRET cb =  m_server->Init();
	HIF_NOTOK(cb) {
		LOG_ERROR("CAminApp::init failed. errno:[%d], errmsg: [%s]", hlast_err(), hbase_err(cb));
		exit(-1);
	}

	HFUN_END;
}


void CAdminApp::chlsig_handler(int signo) {

	(void)signo;
	pid_t pid = 0;
	int stat = 0;

	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
		LOG_WARNNING("child[%d] exit.", pid);
	}

}



