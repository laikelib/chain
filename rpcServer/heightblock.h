


#ifndef __HEIGHTBLOCK_H__
#define __HEIGHTBLOCK_H__

#include <huibase.h>
#include "command.h"

using namespace HUIBASE;

class CHeightBlockCgi : public CCommand {
 public:
    CHeightBlockCgi (HCSTRR strName) : CCommand (strName) { }

    virtual HRET Init (HCSTRR strName) throw (HCException) override;

    HRET Work () throw (HCException);

    virtual HSTR GetRes () const throw (HCException) override;

 private:
    HSTR m_str;


};



#endif //__HEIGHTBLOCK_H__
