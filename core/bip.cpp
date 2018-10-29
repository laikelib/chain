


#include "bip.h"

#include <hstr.h>
#include <binbcd.h>

using namespace HUIBASE::CRYPTO;


HRET CBip44Path::Parse (HCSTRR strPath) {

    HASSERT_RETURN(strPath[0] == 'm', INVL_PARA);

    HVSTRS strs;
    HNOTOK_RETURN(HCStr::Split(strPath, "/", strs));

    HASSERT_RETURN(strs.size() == BIP_PART_COUNT, INVL_PARA);

    // purpose must be 44 for bip44.
    HASSERT_RETURN(strs[1] == "44", INVL_PARA);
    m_purpose = strs[1];

    // coin_type must be 772
    HASSERT_RETURN(strs[2] == "722", INVL_PARA);
    m_cointype = strs[2];

    m_account = strs[3];
    HASSERT_RETURN(HIS_TRUE(HCStr::IsDigit(m_account)), INVL_PARA);

    m_change = strs[4];
    HASSERT_RETURN(m_change == "0" || m_change == "1" || m_change == "2", INVL_PARA);

    m_index = strs[5];
    HASSERT_RETURN(HIS_TRUE(HCStr::IsDigit(m_index)), INVL_PARA);

    HRETURN_OK;
}


HRET DecodeSeedHex (HCSTRR strSeed, CKey& key) {
    HMEM data;

    ParseHex(strSeed, data);

    HASSERT_RETURN(data.size() == 32, INVL_PARA);

    key.Set(data.begin(), data.end(), false);

    HRETURN_OK;
}
