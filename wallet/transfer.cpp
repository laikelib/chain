
#include "util.h"
#include "transfer.h"
#include <hlog.h>
#include <hstr.h>

#include <base58.h>
#include <ifnames.h>
#include <serialize.h>

#include <hcrypto.h>
#include <httphelper.h>

using namespace HUIBASE::CRYPTO;

HRET CTransferCgi::Init(HCSTRR str) throw (HCException) {

    HPS ps = parseInput(str);

    HASSERT_RETURN(ps.count("pass"), INVL_PARA);
    m_strPass = ps["pass"];
    HASSERT_RETURN(m_strPass.length() > 8, INVL_PARA);
    HASSERT_RETURN(HIS_TRUE(HCStr::IsDigitChar(m_strPass)), INVL_PARA);

    HASSERT_RETURN(ps.count("amount"), INVL_PARA);
    m_strAmount = ps["amount"];
    HASSERT_RETURN(HIS_TRUE(HCStr::IsDigitAndDos(m_strAmount)), INVL_PARA);
    m_value = HCStr::stod(m_strAmount) * LKT_COIN;
    HASSERT_RETURN(m_value > 0, INVL_PARA);

    HASSERT_RETURN(ps.count("fee"), INVL_PARA);
    m_strFee = ps["fee"];
    HASSERT_RETURN(HIS_TRUE(HCStr::IsDigitAndDos(m_strFee)), INVL_PARA);
    m_fee = HCStr::stod(m_strFee) * LKT_COIN;
    HASSERT_RETURN(m_fee > 0, INVL_PARA);

    HASSERT_RETURN(ps.count("from"), INVL_PARA);
    m_strFrom = ps["from"];
    HASSERT_RETURN(m_strFrom.length() > 20, INVL_PARA);
    HASSERT_RETURN(HIS_TRUE(HCStr::IsDigitChar(m_strFrom)), INVL_PARA);

    HASSERT_RETURN(ps.count("tos"), INVL_PARA);
    m_strTo = ps["tos"];
    HASSERT_RETURN(m_strTo.length() > 20, INVL_PARA);
    HASSERT_RETURN(HIS_TRUE(HCStr::IsDigitChar(m_strTo)), INVL_PARA);

    LOG_NORMAL("amount[%lld] fee[%lld] from[%s] to[%ss]", m_value, m_fee, m_strFrom.c_str(), m_strTo.c_str());

    HRETURN_OK;

}



HRET CTransferCgi::Work() throw (HCException) {
    HFUN_BEGIN;

    getAddrInfo();

    setEntry();

    signEntry();

    CDataStream ds(SER_NETWORK, NODE_VERSION);
    ds << m_entry;

    m_strEn.clear();
    HFAILED_MSG(HEncode(ds.str(), m_strEn), "encode result failed");

    LOG_NORMAL("data: [%s]", m_strEn.c_str());

    callRpc();

    HASSERT_THROW_MSG(m_strRes != "DROP", "transfer is failed", EX_GET);

    stringstream ss;
    ss << "\"" << m_strRes << "\"";
    m_res = ss.str();

    HFUN_END;
    HRETURN_OK;
}



void CTransferCgi::getAddrInfo() throw (HCException) {

    m_addrInfo.Load(m_strFrom, m_strPass);

}


void CTransferCgi::setEntry() throw (HCException) {

    m_entry.SetCreateTime(time(nullptr));
    m_entry.SetValue (m_value);
    m_entry.SetFee (m_fee);
    CLKAddress sender(m_strFrom);
    CLKAddress receiver(m_strTo);

    HASSERT_THROW_MSG(sender.IsValid(), "sender address is invalid", INVL_PARA);

    HASSERT_THROW_MSG(receiver.IsValid(), "receiver address is invalid", INVL_PARA);

    m_entry.SetSenderAddr(sender);
    m_entry.SetReceiver(receiver);

}



void CTransferCgi::signEntry() throw (HCException) {

    CPrivKey prikey = DecodeBase64(m_addrInfo.GetPri().c_str());
    std::vector<unsigned char> uvc = DecodeBase64(m_addrInfo.GetPub().c_str());
    CPubKey pubkey(uvc);

    CKey key;
    HASSERT_THROW_MSG(key.Load(prikey, pubkey, true), "CKey load private, public key failed", SRC_FAIL);

    CLKAddress _addr;
    CPubKey _pub = key.GetPubKey();
    _addr.Set(_pub.GetID());
    HASSERT_THROW_MSG(_addr == m_entry.GetSender(), "private key is invalid", SRC_FAIL);


    HASSERT_THROW_MSG(m_entry.Signature(key), "signature transaction failed", INVL_RES);

    HASSERT_THROW_MSG(m_entry.VerifySign(), "try to verify transaction failed", INVL_RES);

    LOG_NORMAL("new transaction: [%s]", m_entry.ToJsonString().c_str());

}


void CTransferCgi::callRpc() throw (HCException) {

    CWalletApp * pApp = GetApp();
    HASSERT_THROW_MSG(pApp != nullptr, "app point is null-point", ILL_PT);

    HSTR strRpcIp = pApp->GetConfVal("rpc_ip");
    HSTR strRpcPort = pApp->GetConfVal("rpc_port");
    LOG_NORMAL("rpc-ip: [%s] port[%s]", strRpcIp.c_str(), strRpcPort.c_str());

    HSTR strData = HCStr::Format("entry=%s", m_strEn.c_str());
    HSTR strUrl = HCStr::Format("http://%s:%s/transfer", strRpcIp.c_str(), strRpcPort.c_str());

    LOG_NORMAL("url[%s] data[%s]", strUrl.c_str(), strData.c_str());

    using namespace HUIBASE::HTTP;

    HttpPost(strUrl, strData, m_strRes);

    LOG_NORMAL("rpc return: [%s]", m_strRes.c_str());

}


HSTR CTransferCgi::GetRes() const throw (HCException) {

    return m_strRes;

}


REGISTE_COMMAND(transfer, CTransferCgi);
