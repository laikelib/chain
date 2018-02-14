

#include <happ.h>
#include <hfname.h>
#include "showCommand.h"
#include <hlog.h>
#include "../common/middleConfig.h"
#include <hstr.h>
#include "adminServer.h"

HRET CShowCommand::Work (HSTRR strRequest) throw (HCException) {
	HFUN_BEGIN;

	HSTR strSoName = strRequest;
	strRequest.clear();
	LOG_NORMAL("Get a new request: [%s]", strSoName.c_str());

	std::vector<HCStr> strs;
	HIF_NOTOK(GetConfInfo(strs)) {
		LOG_ES("Get process info failed.");
		HRETURN(LOGIC_ERR);
	}

	PrepareRes(strs, strRequest);

	HFUN_END;
	HRETURN_OK;
}


HRET CShowCommand::GetConfInfo (std::vector<HCStr>& vstrs) {

	CAdminServer* pAdmin = GetAdmin();
	HASSERT_THROW_MSG(pAdmin != nullptr, "pAdmin is null", ILL_PT);

	return pAdmin->GetConfInfo(vstrs);
}


void CShowCommand::PrepareRes (const std::vector<HCStr>& vstrs, HSTRR res) {

	res.clear();

	for (size_t i = 0; i < vstrs.size(); ++i) {
		const HCStr & str = vstrs[i];
		res.insert(res.end(), str.begin(), str.end());
	}

}

REGISTE_COMMAND(show, CShowCommand);


