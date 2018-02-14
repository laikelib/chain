
#include <hlog.h>
#include "command.h"
#include "../common/middleConfig.h"


CCommand::CCommand(HCSTRR strName) 
	: m_strName (strName) {}


HBOOL CCommand::HasSo (HCSTRR strName) const {

	IF_FALSE(middle_config->HasSo(strName)) {

		LOG_ERROR("miss so name: [%s]", strName.c_str());
		return HFALSE;
	
	}

	return HTRUE;

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
		cout << "CCommandFactory::RegisteCommand command name [" + strName
			+ "] hash registered!";
		exit(-1);
	}

	m_makers.insert(std::map<HSTR, command_maker>::value_type(strName, maker) );
	
}




