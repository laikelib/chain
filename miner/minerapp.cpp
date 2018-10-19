

#include "minerapp.h"
#include "minerserver.h"
#include "../safeMiddle/common/middleConfig.h"

#include <hlog.h>
#include <hfname.h>

CMinerApp::CMinerApp(HINT argc, const HCHAR* argv[])
    : HCApp(argc, argv){

    m_pServer = new CMinerServer();
    CHECK_NEWPOINT(m_pServer);

}


CMinerApp::~CMinerApp() {

    HDELP(m_pServer);

}


HBOOL CMinerApp::Run() {

    while (HIS_TRUE(IsRunning())) {

    	try {

	        m_pServer->RunServer ();

            //sleep(36);

        } catch(HCException& ex) {

            LOG_ERROR("CMinerApp run get an exception. [%s]", ex.what());
            // throw a exception;

	    }

        sleep(36);

    }

    // cancel son threads;

    LOG_WS("MinerApp is over");

    return HTRUE;
}


void CMinerApp::init() {

    middle_config->SetConf(m_conf);

    HSTR strSosCfg = middle_config->GetValue("admin_conf");
    LOG_NORMAL("sos config: [%s]", strSosCfg.c_str());

    HFAILED_THROW(middle_config->LoadSoConfig());

    middle_config->ShowSoConfig();

    HFAILED_THROW(m_pServer->Init());

}
