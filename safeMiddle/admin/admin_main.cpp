
#include "adminapp.h"


int main(int argc, char*argv[]) {

	CAdminApp app(argc, argv);

	app.Init();

	app.Run();

	return 0;
}


