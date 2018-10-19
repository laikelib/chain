

#ifndef __SERVERAPP_H__
#define __SERVERAPP_H__

#include <huibase.h>
#include <happ.h>
#include "serverServer.h"

#include <atomic>

using namespace HUIBASE;


class CServerApp : public HCApp {
public:
	CServerApp ();

	~CServerApp ();

	void init();

	HBOOL Run ();

	void setDeamonApp();

	HRET lockApp ();

private:
	CServerServer* m_pServer = nullptr;

};



#endif 


