

#ifndef __POSTWORK_H__
#define __POSTWORK_H__

#include <sodef.h>


class CPostWorkService : public CSoServiceBase {
public:
    CPostWorkService(HCSTRR strName)
	: CSoServiceBase (strName) {}


    virtual int ParseInput (SCM_DATA* data) throw (HCException);


    virtual int Excute () throw (HCException);


    virtual int PackReturn (SCM_DATA* data) throw (HCException);

protected:
    HSTR m_str;
    

};


class CHeightBlockService : public CPostWorkService {
public:
    CHeightBlockService(HCSTRR strName)
	: CPostWorkService(strName) { }


    virtual int Excute () throw (HCException);
};


#endif
