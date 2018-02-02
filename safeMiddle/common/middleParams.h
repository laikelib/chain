
#ifndef __MIDDLE_PARAMS_H__
#define __MIDDLE_PARAMS_H__

#include <huibase.h>


using namespace HUIBASE;

typedef struct tagSoEntry {
	HUINT m_index {0};
	HSTR m_name;
	HUINT m_pcount{0};
	HUINT m_tcount{0};
	HSTR m_path;
} SSoEntry;


static constexpr HUINT gs_uMaxSoNum = 129;


enum soinput_count {
	sic_default,
	sic_sid,
	sic_cid,
	sic_conf,
	sic_app,

	sic_counts,
};

#define PS_CONF_INDEX "PS_CONF_INDEX"
#define PS_SID 	"PS_SID"
#define PS_CID 	"PS_CID"
#define PS_CONF "PS_CONF"
#define PS_APP 	"PS_APP"


#endif //__MIDDLE_PARAMS_H__

