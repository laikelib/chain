

#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <huibase.h>
#include <hsingleton.hpp>

using namespace HUIBASE;

struct RpcMq;
class CRpcApp;

class CCommand {
 public:
    CCommand (HCSTRR strCommand): m_strName (strCommand) { }

    virtual ~CCommand () {}

    HCSTRR GetName () const { return m_strName; }

    void SetMq (RpcMq* pmq) { m_pmq = pmq; }

    void SetApp (CRpcApp* pApp) { m_pApp = pApp; }

    virtual HRET Init (HCSTRR) throw (HCException) { HRETURN_OK; }

    virtual HRET Work() throw (HCException) = 0;

    virtual HSTR GetRes () const throw (HCException);

    HRET sendAndRecv(HCSTRR strCmd, HSTRR str) throw (HCException);

    HPS parseInput (HCSTRR str) const;


 protected:
    HSTR m_strName;
    HSTR m_res;

    RpcMq* m_pmq = nullptr;
    CRpcApp* m_pApp = nullptr;
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


#define REGISTE_COMMAND(name,cmdobj)                                    \
	static CCommand* class_##name_##cmdobj () {                         \
		return new cmdobj(#name);                                       \
	}                                                                   \
	class __CCommandCreate_##name_##cmdobj {                            \
	public:                                                             \
		__CCommandCreate_##name_##cmdobj () {                           \
			maker_facetory::Instance()->RegisteCommand(#name,class_##name_##cmdobj); \
		}                                                               \
	};                                                                  \
	static const __CCommandCreate_##name_##cmdobj __creator_##name_##cmdobj_maker

#endif
