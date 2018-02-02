
#ifndef __CHAIN_CORE_H__
#define __CHAIN_CORE_H__

#include <huibase.h>
#include <hconf.h>


using namespace HUIBASE;

// some directory name.
#define SO_DIR "so_dir"
#define SO_BIN_DIR "so_bin_dir"
#define SO_CONF_DIR "so_conf_dir"
#define SO_LOG_DIR "so_log_dir"
#define SO_DATA_DIR "so_data_dir"

#define CONF_APP_NAME "app_name"
#define CONF_LOG_NAME "log_name"


#define CONF_BK_FILE "bk_file"
#define CONF_WL_FILE "wl_file"



class CChainCore {
public:
	CChainCore () = default;

	~CChainCore ();

	HRET Init ();

	virtual HRET InitChain ();

	virtual HRET Shutdown ();

	HSTR GetValue (HCSTRR strKey);

	HSTR GetValue (HCSTRR strKey, HCSTRR def);

	void SetParm(HCSTRR strKey, HCSTRR strValue);	

	
	

protected:
	virtual HRET initFirst () { HRETURN_OK; }

	virtual HRET initDic ();

	virtual HRET initConf ();

	virtual HRET initLog ();

	virtual HRET initLast () { HRETURN_OK; }	

	void testInit();


private:
	HCMapConf m_conf;
};



#endif //__CHAIN_CORE_H__


