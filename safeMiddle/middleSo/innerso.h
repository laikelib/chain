

#ifndef __INNERSO_H__
#define __INNERSO_H__

#include "innerdef.h"
#include <hsingleton.hpp>


using namespace HUIBASE;

#ifdef __cplusplus
#define SCMEXPORT extern "C"
#else 
#define SCMEXPORT
#endif 


class ServiceFactory {
public:
	typedef std::map<HSTR, declare_fun_t> SCM_FUNS;
public:
	void RegisterService (HCSTRR strName, declare_fun_t fun);

	declare_fun_t GetService (HCSTRR strName);

	HSTR GetAllService ();

private:
	SCM_FUNS m_services;
};

typedef HCSingleton<ServiceFactory> g_services;


SCMEXPORT HRET INIT_FUN_NAME ();

SCMEXPORT void UNINIT_FUN_NAME ();

#define _DECLARE_INIT() SCMEXPORT HRET INIT_FUN_NAME ()


#define _DECLARE_UNINIT() SCMEXPORT void UNINIT_FUN_NAME ()


SCMEXPORT declare_fun_t GET_FUN_NAME (HCSTRR strFunName);



#define _DECLARE_FUN(funName) \
	SCMEXPORT HRET sc_##funName (SCM_DATA* data); \
	class __CSC_##funName {\
	public: \
		__CSC_##funName () { \
			g_services::Instance()->RegisterService(#funName, sc_##funName); \
		} \
	}; \
	static const __CSC_##funName __creator__##funName;  \
	SCMEXPORT HRET sc_##funName (SCM_DATA* data) 


#endif 

