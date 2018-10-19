


#include "lktkey.h"
#include "walletmng.h"
#include <hlog.h>
#include <hfile.h>
#include <hstr.h>
#include <hfname.h>

#include <urlencode.h>
#include <des.h>
#include <base64.h>
#include <hadaptation.h>

using namespace HUIBASE::CRYPTO;

HRET CAddrInfo::Load(HCSTRR strAddr, HCSTRR strPass) throw (HCException) {

    HSTR strFileName = g_manage->GetDataPath() + "/" + strAddr + ".key";

    HASSERT_THROW_MSG(HIS_TRUE(HCFileName::IsExists(strFileName)), "key file is not here", INDEX_OUT);

    HSTR strContent;
    (void) HCFile::JustReadFile(strFileName, strContent);

    LOG_NORMAL("src content: [%s] length: [%d]", strContent.c_str(), strContent.length());

    HSTR strUnUrl = HUIBASE::CRYPTO::UrlDecode(strContent);

    HMEM base_mem, des_mem, src_mem;
    hstr_uvs(strUnUrl, base_mem);

    HASSERT_THROW_MSG(HIS_OK(CBase64::Decode(base_mem, des_mem)), "base64 decode failed", INVL_RES);

    HASSERT_THROW_MSG(HIS_OK(Des3Decrypt(des_mem, src_mem, strPass)), "des decrypt failed", SSL_ERR);

    strContent = huvs_str(src_mem);

    LOG_NORMAL("dst content: [%s]", strContent.c_str());

    HVSTRS strs;
    (void)HCStr::Split(strContent, "|", strs);
    HASSERT_THROW_MSG(strs.size() == 3, "key file content is invalid", SRC_FAIL);

    m_strAddr = strs[0];
    HASSERT_THROW_MSG(m_strAddr == strAddr, "key file content addr is invalid", SRC_FAIL);

    m_strPub = strs[1];
    m_strPri = strs[2];

    LOG_NORMAL("addr[%s], pub[%s], pri[%s]", m_strAddr.c_str(), m_strPub.c_str(), m_strPri.c_str());
    HRETURN_OK;
}
