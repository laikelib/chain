
#ifndef __STOP_COMMAND_H__
#define __STOP_COMMAND_H__

#include <huibase.h>
#include "command.h"

using namespace HUIBASE;

class CStopCommand : public CCommand {
public:
	CStopCommand (HCSTRR strName ) : CCommand(strName) {}

	HRET Work (HSTRR strRequest) throw (HCException);

};




#endif // __STOP_COMMAND_H__

