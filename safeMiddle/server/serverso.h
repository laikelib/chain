
#ifndef __SERVER_SO_H__
#define __SERVER_SO_H__

#include <huibase.h>
#include <hfname.h>
#include "../middleSo/innerdef.h"

using namespace HUIBASE;


class CServerSo {
public:
	CServerSo () = default;

	explicit CServerSo (const HCFileName& fn) ;

	~CServerSo ();

	void SetSoFile (const HCFileName& fn);

	HRET Open () ;

	HRET Close ();

	declare_fun_t GetCallFun (HCSTRR strName) throw (HCException);

private:
	HCFileName m_soName;
	void* m_pSoHandle{nullptr};

	so_init_fun m_init{nullptr};
	so_uninit_fun m_uninit{nullptr};
	so_get_fun m_get{nullptr};
};


#endif //__SERVER_SO_H__


