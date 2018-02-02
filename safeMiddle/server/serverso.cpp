
#include "serverso.h"
#include <dlfcn.h> 
#include <hstr.h>
#include <hlog.h>

CServerSo::CServerSo (const HCFileName& fn) {
	HCDic curDic = HCDic::GetCurrentDict();
	m_soName.SetName(curDic.GetName() + "/" + fn.GetName());
}

CServerSo::~CServerSo () {

	(void)Close();

}


void CServerSo::SetSoFile (const HCFileName& fn) {

	HCDic curDic = HCDic::GetCurrentDict();
	m_soName.SetName(curDic.GetName() + "/" + fn.GetName());

}


static HSTR _getDlerror() {
	HSTR ret;
	HCHAR* cb = nullptr;
	cb = dlerror();
	if (cb != nullptr) {
		ret = HCStr::Format("Call dlsym failed. Error: [%s]", cb);
	}

	return ret;

}

HRET CServerSo::Open () {
	HFUN_BEGIN;
	
	LOG_NORMAL("Load so file [%s].", m_soName.GetName().c_str());

	m_pSoHandle = dlopen(m_soName.GetName().c_str(), RTLD_LAZY);
	HASSERT_MSG_THROW(m_pSoHandle != nullptr, _getDlerror(), HERR_NO(SYS_FAILED));

	LOG_NORMAL("so file [%s] loaded, errno: %d, msg: %s. Any question: %s", 
		m_soName.GetName().c_str(), errno, strerror(errno), _getDlerror().c_str());

	dlerror(); //clear any existing error.

	LOG_NORMAL("load init fun [%s]", SO_FUN_NAME(INIT_FUN_NAME));

	m_init = (so_init_fun)dlsym(m_pSoHandle, SO_FUN_NAME(INIT_FUN_NAME));
	HASSERT_MSG_THROW(m_init != nullptr, _getDlerror(), HERR_NO(SYS_FAILED));

	LOG_NS("dlsym init function success");

	m_uninit = (so_uninit_fun)dlsym(m_pSoHandle, SO_FUN_NAME(UNINIT_FUN_NAME));
	HASSERT_MSG_THROW(m_uninit != nullptr, _getDlerror(), HERR_NO(SYS_FAILED));
		
	m_get = (so_get_fun)dlsym(m_pSoHandle, SO_FUN_NAME(GET_FUN_NAME));
	HASSERT_MSG_THROW(m_get != nullptr, _getDlerror(), HERR_NO(SYS_FAILED));
		
	HNOTOK_RETURN(m_init());

	HFUN_END;
	HRETURN_OK;
}



HRET CServerSo::Close () {

	if (nullptr != m_uninit) {
		m_uninit();
	}

	if (m_pSoHandle != nullptr) {
		dlclose(m_pSoHandle);
	}

	HRETURN_OK;

}

declare_fun_t CServerSo::GetCallFun (HCSTRR strName) throw (HCException) {

	declare_fun_t ret = nullptr;

	if (m_get != nullptr) {

	    ret = m_get(strName);
	
	}

	return ret;

}




