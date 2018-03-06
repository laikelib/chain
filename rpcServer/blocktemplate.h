


#ifndef __BLOCKTEMPLATE_H__
#define __BLOCKTEMPLATE_H__

#include <huibase.h>
#include "command.h"


using namespace HUIBASE;


class CBlockTemplateCgi : public CCommand {
 public:
    CBlockTemplateCgi (HCSTRR strName) : CCommand (strName) { }

    virtual HRET Init (HCSTRR strName) throw (HCException) override;

    virtual HRET Work () throw (HCException);

    virtual HSTR GetRes () const throw (HCException);


 private:
    HSTR m_str;


};



#endif //__BLOCKTEMPLATE_H__
