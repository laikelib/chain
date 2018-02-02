

#ifndef __MIDDLECONFIG_H__
#define __MIDDLECONFIG_H__

#include <huibase.h>
#include <hsingleton.hpp>
#include <hconf.h>
#include "../common/middleParams.h"


using namespace HUIBASE;

class CMiddleConfig {
public:
	CMiddleConfig ()  = default;
	~ CMiddleConfig() = default;

	HCSTRR GetValue (HCSTRR key) const throw(HCException) {
		return m_conf.GetValue (key);
	}

	HSTR GetValue (HCSTRR key) throw(HCException) {
		return m_conf.GetValue (key);
	}

	HINT GetInt (HCSTRR key, HINT def);

	void SetValue (HCSTRR strKey, HCSTRR value) {
		m_conf.SetValue (strKey, value);
	}

	HBOOL IsHere(HCSTRR key) const {
		return m_conf.IsHere(key);
	}

	void SetConf (const HCMapConf& conf) { m_conf.Merge(conf); }

	HCPSR GetData () const {
		return m_conf.GetData();
	}

	HINT NewId () { return ++m_id; }

	void SetNewSoenty (HCSTRR strName, const SSoEntry& so) throw(HCException);

	HRET LoadSoConfig ();

	void ShowSoConfig ();

	HBOOL HasSo (HCSTRR strSoName) const;

	HBOOL HasSo (HUINT cid, HSTRR strSoName) const;

	const SSoEntry& GetEntry (HCSTRR strSoName) const throw(HCException);

private:
	HCMapConf m_conf;
	HINT m_id = 0;
	std::map<HSTR, SSoEntry> m_mapsos;
};

typedef HCSingleton<CMiddleConfig> smiddle_config;

#ifndef middle_config
#define middle_config smiddle_config::Instance()
#endif


#endif 

