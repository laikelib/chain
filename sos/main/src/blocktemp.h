

#ifndef __BLOCKTEMP_H__
#define __BLOCKTEMP_H__

#include <sodef.h>


class CBlockTempService : public CSoServiceBase {
public:

    CBlockTempService(HCSTRR strName)
	: CSoServiceBase (strName) {}

    virtual int ParseInput (SCM_DATA* data) throw (HCException);

    virtual int Excute () throw (HCException);

    virtual int PackReturn (SCM_DATA* data) throw (HCException);

protected:
    HSTR m_strIn;
    HSTR m_strTemp;

};


class CGetBalService : public CBlockTempService {
 public:
    CGetBalService (HCSTRR strAddr)
        : CBlockTempService (strAddr) { }

    virtual int Excute () throw (HCException);


};


class CHashTxService : public CBlockTempService {
 public:
    CHashTxService (HCSTRR strHash)
        : CBlockTempService (strHash) {  }

    virtual int Excute () throw (HCException);
};



#endif 






