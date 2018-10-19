

#include <hmutex.h>

#include "walletmng.h"

#include "util.h"
#include "key.h"
#include "base58.h"

#include <hlog.h>
#include <hfile.h>

#include <urlencode.h>
#include <des.h>
#include <base64.h>
#include <hadaptation.h>

using namespace HUIBASE::CRYPTO;

HSTR CWalletMng::NewAccount(HCSTRR strPass) {

    /*
     * 1> Make a new key and address;
     * 2> address|pub|pri ==> content;
     * 3> des5 content;
     * 4> write to address.key;
     */

    static HCMutex s_mutex;

    {
        MUTEXHOLDER _mutex(s_mutex);

        RandAddSeed();

    }

    CKey secret;
    secret.MakeNewKey(true);
    CPubKey pubkey = secret.GetPubKey();
    CPrivKey prikey = secret.GetPrivKey();

    CLKAddress addr;
    addr.Set(pubkey.GetID());

    HSTR strAddr = addr.ToString();

    HSTR strPub = EncodeBase64(&pubkey[0], pubkey.size());

    HSTR strPri = EncodeBase64(&prikey[0], prikey.size());

    return SetAccount(strAddr, strPub, strPri, strPass);

}



HSTR CWalletMng::SetAccount(HCSTRR strAddr, HCSTRR strPub, HCSTRR strPri, HCSTRR strPass) {

    HSTR strContent = strAddr + "|" + strPub + "|" + strPri;

    LOG_NORMAL("content: [%s]", strContent.c_str());

    HMEM des_mem;
    HASSERT_THROW_MSG(HIS_OK(Des3Encrypt(strContent, des_mem, strPass)), "des3 crypto failed", SSL_ERR);

    HMEM base64_mem;
    HASSERT_THROW_MSG(HIS_OK(CBase64::Encode(des_mem, base64_mem)), "base64 encode failed", SSL_ERR);

    HSTR strBase = huvs_str(base64_mem);
    strContent.clear();
    strContent = UrlEncode(strBase);

    LOG_NORMAL("content: [%s] length: [%d]", strContent.c_str(), strContent.length());

    HSTR strFileName = m_strDataPath + "/" + strAddr + ".key";
    HCFile::JustSaveFile(strFileName, strContent);

    return strAddr;
}
