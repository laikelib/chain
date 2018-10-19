
#include <hlog.h>
#include <huibase.h>
#include "../common/middleConfig.h"
#include <hfname.h>
#include "serverapp.h"

using namespace HUIBASE;

// sid, cid
int main(int argc, const char* argv[]) {

	// params check.
	if (argc < sic_counts ) {
		printf("Invalid params");
		exit(-1);
	}

	// close the file descriptor inherit parent process.
	for (int fd = 3; fd < 64; ++fd) {
        close(fd);
    }
	
	for (HINT i = 1; i < sic_counts; ++i) {
		
		if (i == sic_sid) {
			middle_config->SetValue(PS_SID, argv[i]);
		}

		if (i == sic_cid) {
			middle_config->SetValue(PS_CID, argv[i]);
		}
		
		if (i == sic_conf) {
			middle_config->SetValue(PS_CONF, argv[i]);
		}

		if (i == sic_app) {
			middle_config->SetValue(PS_APP, argv[i]);
		}

	}

	CServerApp app;
	app.SetConfValue("conf", middle_config->GetValue(PS_CONF));	
	app.SetConfValue(EXE_NAME, middle_config->GetValue(PS_APP));	

	try {
		
		app.Init();

		HBOOL cb = app.Run();
		IF_FALSE(cb) {
			cout << hbase_err(cb) << endl;
			return -1;
		}
		
	} catch(HCException& ex) {
	
		LOG_ERROR("server app get a exception. %s", ex.what());		
		
	}

	return 0;
}

