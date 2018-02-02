

#include <happ.h>
#include <hfname.h>
#include "shutdownCommand.h"
#include <hlog.h>
#include "../common/middleConfig.h"
#include <hstr.h>
#include "adminServer.h"

HRET CShutdownCommand::Work (HSTRR strRequest) throw (HCException) {
	HFUN_BEGIN;

	HSTR strSoName = strRequest;
	strRequest.clear();
	LOG_NORMAL("Get a new request: [%s]", strSoName.c_str());

	/*
		1. stop child process;
		2. clear conf;
		3. all stop me;
	*/

	// stop child and clear conf
	HIF_NOTOK(stopChild()) {

		strRequest = "stop child process failed";
		HRETURN(LOGIC_ERR);

	}

	// TODO: if stop server failed, we exit anyway.
	// stop serverapp
	HIF_NOTOK(stopApp()) {

		strRequest = "stop server failed";
		LOG_ES("Stop server failed, process exit anyway.");
		exit(-1);

	}
	

	HFUN_END;
	HRETURN_OK;
}


HRET CShutdownCommand::stopChild () throw (HCException) {
	HFUN_BEGIN;


	CAdminServer* pAdmin = GetAdmin();
	HASSERT_THROW_MSG(pAdmin != nullptr, "pAdmin is null", ILL_PT);

	HRET cb = pAdmin->StopChild();

	HFUN_END;

	return cb;
}


HRET CShutdownCommand::stopApp () throw (HCException) {

	CAdminServer* pAdmin = GetAdmin();
	HASSERT_THROW_MSG(pAdmin != nullptr, "pAdmin is null", ILL_PT);

	return pAdmin->StopServer();
	
}



REGISTE_COMMAND(shutdown, CShutdownCommand);


