


#ifndef __LOAD_COMMAND_H__
#define __LOAD_COMMAND_H__


#include <huibase.h>
#include "command.h"
#include "../common/middleConfig.h"


using namespace HUIBASE;


class CLoadCommand   : public CCommand {
public:
	CLoadCommand (HCSTRR strName ) : CCommand(strName) {}

	HRET Work (HSTRR strRequest) throw (HCException);

	HBOOL WhetherExists (const SSoEntry& cse) ;

};


#endif // __LOAD_COMMAND_H__


