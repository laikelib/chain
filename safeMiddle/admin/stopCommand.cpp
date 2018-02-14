

#include <happ.h>
#include <hfname.h>
#include "stopCommand.h"
#include <hlog.h>
#include "../common/middleConfig.h"
#include <hstr.h>
#include "adminServer.h"

HRET CStopCommand::Work (HSTRR strRequest) throw (HCException) {
	HFUN_BEGIN;

	if (strRequest.empty()) {
		strRequest = "miss param.";
		HRETURN(INVL_PARA);
	}

	HSTR strSoName = strRequest;
	strRequest.clear();
	LOG_NORMAL("Get a new request: [%s]", strSoName.c_str());

	IF_FALSE(HasSo(strSoName)) {
		strRequest = "miss so name.";
		HRETURN(INVL_PARA);
	}
	
	// 1. Read Conf;
	const SSoEntry& cse = middle_config->GetEntry(strSoName);

	CAdminServer* pAdmin = GetAdmin();
	HASSERT_THROW_MSG(pAdmin != nullptr, "pAdmin is null", ILL_PT);

	HIF_NOTOK(pAdmin->StopChild(cse.m_index)) {

		strRequest = "stop server failed";
		LOG_ERROR("stop process[%s] index[%d] failed", strSoName.c_str(), cse.m_index);
		HRETURN(LOGIC_ERR);

	}


	HFUN_END;
	HRETURN_OK;
}

REGISTE_COMMAND(stop, CStopCommand);


