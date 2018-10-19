
#ifndef __INNERDEF_H__
#define __INNERDEF_H__

#include <huibase.h>

using namespace HUIBASE;

static constexpr HUINT SCM_MAX_LEN = 1024*1024*2;  //2M


struct SCM_DATA {
	const HCHAR* idata;
	HSIZE ilen;
	HCHAR odata[SCM_MAX_LEN];
	HSIZE olen;
};

inline void RESET_ODATA(SCM_DATA& data) {
    if (data.odata != nullptr) {
	memset(data.odata, 0, SCM_MAX_LEN);
    }
}

inline void RESET_ODATAP(SCM_DATA* p) {
    if (p != nullptr && p->odata != nullptr) {
        memset(p->odata, 0, SCM_MAX_LEN);
    }
}


typedef HRET (*declare_fun_t)(SCM_DATA*);

typedef HRET (*so_init_fun)();
typedef void (*so_uninit_fun) ();
typedef declare_fun_t (*so_get_fun)(HCSTRR);



#define INIT_FUN_NAME scm_server_init

#define UNINIT_FUN_NAME scm_server_exit

#define GET_FUN_NAME GetSCMFun

#define __SO_FUN_NAME(nn) #nn
#define SO_FUN_NAME(nn) __SO_FUN_NAME(nn)

#endif //__INNERDEF_H__

