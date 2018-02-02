
#include <happ.h>
#include <hfname.h>
#include "loadCommand.h"
#include <hlog.h>
#include <hstr.h>
#include "adminServer.h"

HRET CLoadCommand::Work (HSTRR strRequest) throw (HCException) {
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

	// TODO: make sure there is an free conf place.
	/* 
		> read conf;
		> check process name;
		> start server process;
		> write share memory conf;  [at child process]
	*/
	// 1. Read Conf;
	const SSoEntry& cse = middle_config->GetEntry(strSoName);

	// if process is exists, you can run another any more.
	IF_TRUE(WhetherExists(cse)) {
		LOG_WARNNING("process [%s] is exists. Cann't run any more.", strSoName.c_str());
		strRequest = "server is running...";
		HRETURN(RECREATE);
	}
	
	// check server exists, if not exists, return miss_file.
	HSTR serverPathFile = middle_config->GetValue("server_pathfile");
	IF_FALSE(HCFileName::IsExactExists(HCFileName(serverPathFile))) {
		LOG_ERROR("server bin file[%s] is miss", serverPathFile.c_str());
		strRequest = "server bin is miss.";
		HRETURN(MISS_FILE);
	}

	// check so file exists.
 	HSTR strSosHome = middle_config->GetValue("sos_home");
	HSTR strSoLibPathFile = strSosHome + "/" + strSoName + "/lib/" + cse.m_path;
 	IF_FALSE(HCFileName::IsExactExists(HCFileName(strSoLibPathFile))) {
		LOG_ERROR("so file[%s] is miss", strSoLibPathFile.c_str());
		strRequest = "so file is miss.";
		HRETURN(MISS_FILE);
	}
	
	// fork server process
	auto pid = fork ();
	if (pid == 0) {
		// child. server process
		LOG_NORMAL("fork success for %s server", cse.m_name.c_str());

		// prepare params
		HCDic curDic = HCDic::GetCurrentDict();
		HSTR strServerName = middle_config->GetValue("server_filename");
		HSTR strServerPathFileName = curDic.GetName() + "/" + strServerName;

		// So far, we set a process run a so.
		HSTR strPsSid = "1";
		HSTR strPsCid = HCStr::itos(cse.m_index);

		HCDic confDic = HCDic::GetCurrentDict();
		confDic.GoToParent();
		confDic.Append("conf");
		HSTR strConfPathFile = confDic.GetName() + "/conf";

		LOG_NORMAL("params for child: {sid: %s, cid: %s, conf:%s, name: %s}", strPsSid.c_str(),
			strPsCid.c_str(), strConfPathFile.c_str(), strSoName.c_str());

		// change direct to so's bin.
		HCDic _tdic = HCDic::GetCurrentDict();
		_tdic.GoToParent();
		_tdic.Append("sos");
		_tdic.Append(strSoName);
		_tdic.Append("lib");
		HIF_NOTOK(HCDic::GoToDic(_tdic)) {
			LOG_ERROR("child process go to %s FAILED", _tdic.GetName().c_str());
			exit(-1);
		}	

		_tdic = HCDic::GetCurrentDict();
		LOG_NORMAL("current work dir: %s", _tdic.GetName().c_str());

		execl(strServerPathFileName.c_str(), strServerName.c_str(), strPsSid.c_str(),
			strPsCid.c_str(), strConfPathFile.c_str(), strSoName.c_str(), nullptr);

		LOG_ERROR("execl failed after fork in %d.", pid);
		exit(-1);
		
	} else if (pid > 0) {

		; // command do nothing for fork.

	} else {

		LOG_ERROR("fork failed, msg: %s", strerror(errno));

		HRETURN(SYS_FAILED);

	}

	HFUN_END;
	HRETURN_OK;
}


HBOOL CLoadCommand::WhetherExists (const SSoEntry& cse) {
	
	CAdminServer* pServer = GetAdmin();
	if (pServer == nullptr) {
		return HFALSE;
	}

	return pServer->ProcessExists(cse.m_index);

}


REGISTE_COMMAND(load, CLoadCommand);



