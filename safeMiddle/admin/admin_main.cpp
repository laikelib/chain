
#include "adminapp.h"


int main(int argc, const char*argv[]) {

	CAdminApp app(argc, argv);

	app.Init();

	app.Run();

	return 0;
}


