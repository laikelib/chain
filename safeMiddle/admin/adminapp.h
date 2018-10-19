
#ifndef __ADMINAPP_H__
#define __ADMINAPP_H__

#include <huibase.h>
#include <happ.h>

#include <vector>
#include "adminServer.h"



using namespace HUIBASE;

class CAdminApp : public HCApp {
public:
	CAdminApp (HINT argc, const HCHAR* argv[]);

	~ CAdminApp();

	HBOOL Run ();

private:

	virtual void init();

private:
	CAdminServer* m_server = nullptr;

	static void chlsig_handler(int signo);

};

#endif // __ADMINAPP_H__


