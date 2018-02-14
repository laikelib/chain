

#ifndef __TESTAPP_H__
#define __TESTAPP_H__

#include <huibase.h>

#include <happ.h>

using namespace HUIBASE;


class CTestServer;
class CTestApp : public HCHapp {
public:
    CTestApp(HINT argc, HCHAR* argv[]);

    ~CTestApp();

    HBOOL Run ();

private:
    virtual void init();

private:
    CTestServer* m_pServer = nullptr;

};


#endif //__TESTAPP_H__
