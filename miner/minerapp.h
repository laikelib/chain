

#ifndef __MINERAPP_H__
#define __MINERAPP_H__

#include <huibase.h>

#include <happ.h>

using namespace HUIBASE;


class CMinerServer;
class CMinerApp : public HCApp {
public:
    CMinerApp(HINT argc, const HCHAR* argv[]);

    ~CMinerApp();

    HBOOL Run ();

private:
    virtual void init();

private:
    CMinerServer* m_pServer = nullptr;

};


#endif //__MINERAPP_H__
