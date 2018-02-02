
#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <huibase.h>
#include <hsingleton.hpp>


using namespace HUIBASE;

class CAdminServer;
class CCommand {
public:
	CCommand(HCSTRR strName);
	virtual ~CCommand() {};

	HCSTRR GetName () const { return m_strName; }

	virtual HRET Init () { HRETURN_OK; }

	virtual HRET Work (HSTRR strRequest) throw (HCException) = 0;

	void SetAdmin (CAdminServer* admin) { m_pAdmin = admin; }

protected:
	HBOOL HasSo (HCSTRR strName) const;
	CAdminServer* GetAdmin() { return m_pAdmin; }

private:
	HSTR m_strName;
	CAdminServer* m_pAdmin{nullptr};

};

class CCommandFactory {
public:
	typedef CCommand* (*command_maker) ();
public:

	CCommand* newCommand(HCSTRR strName) ;

	void RegisteCommand (HCSTRR strName, command_maker maker);


private:	
	std::map<HSTR, command_maker> m_makers;

};

typedef HCSingleton<CCommandFactory> maker_facetory;

#define REGISTE_COMMAND(name,cmdobj) \
	static CCommand* class_##name_##cmdobj () { \
		return new cmdobj(#name); \
	} \
	class __CCommandCreate_##name_##cmdobj { \
	public: \
		__CCommandCreate_##name_##cmdobj () { \
			maker_facetory::Instance()->RegisteCommand(#name,class_##name_##cmdobj); \
		} \
	}; \
	static const __CCommandCreate_##name_##cmdobj __creator_##name_##cmdobj_maker



#endif //__COMMAND_H__


