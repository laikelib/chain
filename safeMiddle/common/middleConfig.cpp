
#include <hlog.h>
#include <hstr.h>
#include <libconfig.h++>
#include <hfname.h>
#include "middleConfig.h"


HINT CMiddleConfig::GetInt (HCSTRR key, HINT def) {

	if(HIS_FALSE(m_conf.IsHere(key))) {

		return def;
	
	}

	return HCStr::stoi(GetValue(key));

}

void CMiddleConfig::SetNewSoenty (HCSTRR strName, const SSoEntry& so) 
	throw(HCException) {

	std::map<HSTR, SSoEntry>::iterator fit = m_mapsos.find(strName);

	HASSERT_THROW(fit == m_mapsos.end(), INVL_PARA);

	m_mapsos.insert(std::map<HSTR, SSoEntry>::value_type(strName, so));

}


HRET CMiddleConfig::LoadSoConfig () {

	using namespace libconfig;
	Config cfg;

	try {
		
		cfg.readFile(middle_config->GetValue("admin_conf").c_str());
		
	} catch (const FileIOException& fiex) {

		LOG_ERROR("I/O error while read file. msg: [%s]. conf: [%s]", fiex.what(),
			GetValue("admin_conf").c_str());
		HRETURN(IO_ERR);

	} catch (const ParseException& pex ) {

		LOG_ERROR("Parse config file failed at %s:%d--%s", pex.getFile(),
			pex.getLine(), pex.getError());
		HRETURN(IO_ERR);
		
	}

	{

		const Setting& root = cfg.getRoot();
		const Setting& sos = root["sos"];

		for (int i = 0; i < sos.getLength(); ++i) {

			SSoEntry se;
			sos[i].lookupValue("index", 	se.m_index);
			sos[i].lookupValue("name", 		se.m_name);
			sos[i].lookupValue("pcount", 	se.m_pcount);			
			sos[i].lookupValue("tcount", 	se.m_tcount);
			sos[i].lookupValue("path", 		se.m_path);
			SetNewSoenty(se.m_name, se);

		}
		
	}
	
	HRETURN_OK;

}


void CMiddleConfig::ShowSoConfig () {

	for (std::map<HSTR, SSoEntry>::const_iterator cit = m_mapsos.begin();
		cit != m_mapsos.end(); ++cit ) {

		const SSoEntry& _so = cit->second;
		LOG_NORMAL("==========So: %s==========", cit->first.c_str());			
		LOG_NORMAL("index: %d", _so.m_index);
		LOG_NORMAL("name: %s", _so.m_name.c_str());
		LOG_NORMAL("pcount: %d", _so.m_pcount);
		LOG_NORMAL("tcount: %d", _so.m_tcount);
		LOG_NORMAL("path: %s", _so.m_path.c_str());

	}

}

HBOOL CMiddleConfig::HasSo (HCSTRR strSoName) const {

	HRET_BOOL(m_mapsos.count(strSoName));

}

HBOOL CMiddleConfig::HasSo (HUINT cid, HSTRR strSoName) const {

	for (std::map<HSTR, SSoEntry>::const_iterator cit = m_mapsos.begin(); 
		cit != m_mapsos.end(); ++cit) {

		const SSoEntry& cse = cit->second;

		if (cse.m_index == cid) {

			strSoName = cse.m_name;

			return HTRUE;

		}

	}

	return HFALSE;
}

const SSoEntry& CMiddleConfig::GetEntry (HCSTRR strSoName) const throw(HCException) {

	std::map<HSTR, SSoEntry>::const_iterator cit = m_mapsos.find(strSoName);

	HASSERT_THROW(cit != m_mapsos.end(), POS_END);

	const SSoEntry& se = cit->second;

	return se;

}




