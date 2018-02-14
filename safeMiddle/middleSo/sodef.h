

#ifndef __SODEF_H__
#define __SODEF_H__

#include <string>
#include "innerso.h"
#include <glog/logging.h>


using namespace std;

class CSoServiceBase {
public:
    CSoServiceBase(const string& strName)
	: m_name(strName) {}

    virtual ~CSoServiceBase () {}

    virtual int ParseInput (SCM_DATA* data) throw (HCException)  = 0;

    virtual int Excute () = 0;

    virtual int PackReturn (SCM_DATA* data) throw (HCException)  = 0;

    HCSTRR GetName () const { return m_name; }

private:
    string m_name;
};

template<class T>
int DeclareService(SCM_DATA* data, HCSTR servName) {

    LOG(INFO) << servName << " start...";
    
    try {

	T service(servName);

	service.ParseInput (data);

	service.Excute ();

	service.PackReturn (data);

    } catch (HCException& ex) {

	LOG(ERROR) << servName << " get an exception: [" << ex.what() << "]";

    } catch (...) {

	LOG(ERROR) << servName << " get an unknown exception";
	
    }

    LOG(INFO) << servName << " end...";
    return 0;
}



#define DECLARE_INIT() _DECLARE_INIT()

#define DECLARE_UNINIT() _DECLARE_UNINIT()

#define DECLARE_FUN(funName) _DECLARE_FUN(funName)


#endif 


















