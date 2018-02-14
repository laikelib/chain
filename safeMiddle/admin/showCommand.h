
#ifndef __SHOW_COMMAND_H__
#define __SHOW_COMMAND_H__

#include <huibase.h>
#include "command.h"

using namespace HUIBASE;


class CShowCommand : public CCommand{
public:
	CShowCommand (HCSTRR strName ) : CCommand(strName) {}

	HRET Work (HSTRR strRequest) throw (HCException);

	HRET GetConfInfo (std::vector<HCStr>& vstrs);

	void PrepareRes (const std::vector<HCStr>& vstrs, HSTRR res);
};

#endif //__SHOW_COMMAND_H__

