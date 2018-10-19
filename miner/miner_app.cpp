
#include <hlog.h>
#include <huibase.h>
#include "minerapp.h"

using namespace HUIBASE;


int main(int argc, const char* argv[]) {

    CMinerApp app(argc, argv);

    try {

        app.Init();

	    HBOOL cb = app.Run();
	    IF_FALSE(cb) {
            LOG_NS("CMinerApp run error");
	        cout << hbase_err(cb) << endl;
	        return -1;
	    }

    } catch(HCException& ex) {

        //LOG_ERROR("server app get a exception. %s", ex.what());
	    printf("server app get a exception: %s", ex.what());

    }

    return 0;
}

