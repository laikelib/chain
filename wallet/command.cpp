
#include <hstr.h>
#include <hlog.h>
#include "command.h"

HSTR CCommand::GetRes() const throw (HCException) {

    return m_res;

}


HPS CCommand::parseInput(HCSTRR str) const {

    HPS res;

    HVSTRS vs;
    HINT cb = HCStr::Split (str, "&", vs);

    HIF_NOTOK(cb) {

        return res;

    }

    for (size_t i = 0; i < vs.size(); ++i) {

        HSTRR ss = vs[i];
        HCStr::Trim (ss);
        HVSTRS ii;

        cb = HCStr::Split(ss, "=", ii);

        HIF_NOTOK(cb) {

            continue;

        }

        if(ii.size() < 2) {
            continue;
        }

        HSTRR skey = ii[0];
        HCStr::Trim(skey);

        HSTRR sval = ii[1];
        HCStr::Trim(sval);

        res[skey] = sval;

    }

    return res;

}



CCommand* CCommandFactory::newCommand(HCSTRR strName){

	std::map<HSTR, command_maker>::iterator fit = m_makers.find (strName);
	if (fit == m_makers.end()) {
		return nullptr;
	}

	command_maker pfun = fit->second;

	if (pfun != nullptr) {
		return pfun();
	}

	return nullptr;

}


void CCommandFactory::RegisteCommand (HCSTRR strName, command_maker maker) {

	if (m_makers.find(strName) != m_makers.end()) {
		//cout << "CCommandFactory::RegisteCommand command name [" + strName
		//	+ "] hash registered!";
		exit(-1);
	}

	m_makers.insert(std::map<HSTR, command_maker>::value_type(strName, maker) );

}
