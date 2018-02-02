
#ifndef __SHUTDOWN_COMMAND_H__
#define __SHUTDOWN_COMMAND_H__

#include <huibase.h>
#include "command.h"

using namespace HUIBASE;


class CShutdownCommand : public CCommand{
public:
	CShutdownCommand (HCSTRR strName ) : CCommand(strName) {}

	HRET Work (HSTRR strRequest) throw (HCException);

private:
	HRET stopChild () throw (HCException);

	HRET stopApp () throw (HCException);
};

#endif //__SHUTDOWN_COMMAND_H__

