

#include <hlog.h>
#include <huibase.h>
#include "rpcapp.h"

using namespace HUIBASE;

int main(int argc, const char* argv[]) {

    CRpcApp app(argc, argv);

    try {

        app.Init ();

        HBOOL cb = app.Run ();

        IF_FALSE(cb) {

            LOG_NS("CRpcApp run error");
            return -1;

        }

    } catch (HCException& ex) {

        LOG_ERROR("rpc app get an exception [%s]", ex.what());

    }

    return 0;

}
