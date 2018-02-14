

#include "innerso.h"


void ServiceFactory::RegisterService (HCSTRR strName, declare_fun_t fun) {

    
    SCM_FUNS::const_iterator fit = m_services.find(strName);
    
    HASSERT_THROW(fit == m_services.end(), INVL_PARA);

    m_services.insert(SCM_FUNS::value_type(strName, fun));

}


declare_fun_t ServiceFactory::GetService (HCSTRR strName) {
    
	if (not m_services.count(strName)) {

	    return nullptr;
	    
	}


	SCM_FUNS::iterator fit = m_services.find(strName);

	return fit->second;
}


declare_fun_t GetSCMFun (HCSTRR strFunName) {

    return g_services::Instance()->GetService(strFunName);

}


HSTR ServiceFactory::GetAllService () {

    HSTR res;
    
    for(SCM_FUNS::const_iterator cit = m_services.begin();
	cit != m_services.end(); ++cit) {

	res += cit->first + "|";

    }

    return res;

}










