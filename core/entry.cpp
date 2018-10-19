

#include "entry.h"
#include "hash.h"

#include <glog/logging.h>

CEntry::CEntry() {

    SetNull();

}


CEntry::CEntry(const CEntry& entry)
    : m_type(entry.m_type), m_createTime(entry.m_createTime), m_value(entry.m_value), m_fee(entry.m_fee), m_sender(entry.m_sender), m_receiver(entry.m_receiver), m_sign(entry.m_sign){


}


CEntry::CEntry(HN64 nValue, HN64 nFee, const CLKAddress& addrSender, const CLKAddress& addrReceiver) {

    m_type = 1;

    m_createTime = time(nullptr);

    m_sender = addrSender;

    m_receiver = addrReceiver;

    m_value = nValue;

    m_fee = nFee;

}


uint256 CEntry::GetEntryHash() const {

    return SerializeHash (*this);

}


bool CEntry::IsBaseSign() const {

    return m_sender.IsEmpty() && m_sign.EmptySignature();

}


bool CEntry::Signature(const CKey &key) {

    return m_sign.Sinature(key, m_sender, getSignHash());

}


bool CEntry::VerifySign() const {

    return m_sign.VerifySign (m_sender, getSignHash());
    
}


bool CEntry::Check() const {

    if (not m_receiver.IsValid()) {
	LOG(ERROR) << "reciver is invalid";
	return false;
    }

    if ( m_value > MAX_TRAN_LKT) {
	LOG(ERROR) << "value is invalid, value: "<< m_value;
	return false;
    }

    if (not IsBaseSign()) {
	if (m_fee > MAX_LKC_FEE || m_fee < MIN_LKC_FEE) {
	    LOG(ERROR) << "fee is invalid";
	    return false;
	}
    }

    if (not IsBaseSign()) {

	if (not VerifySign()) {
	    LOG(ERROR) << "verify sign failed ...";
	    return false;

	}

    }

    return true;
}



void CEntry::SetNull() {

    m_type = 1;

    m_createTime = 0;

    m_value = m_fee = 0;
     
}


CEntryDB::CEntryDB() {

    

}


string CEntry::ToJsonString () const {

    std::stringstream ss;

    ss << "{ \"type\": " << GetType() << ", \"createTime\":"
       << GetCreateTime() << ", \"sender\": \"" << GetSender().ToString()
       << "\", \"receiver\":\"" << GetReceiver().ToString()
       << "\", \"sign\":\"" << GetSign().ToString() << "\",\"value\":"
       << GetValue() << ", \"fee\":" << GetFee() << "}";


    return ss.str();

}


string CEntry::GetJsonFromEntrys (const std::vector<CEntry>& entrys) {

    std::stringstream ss;

    ss << "[ ";

    for (size_t i = 0; i < entrys.size(); ++i) {

	const CEntry& cen = entrys[i];

	ss << cen.ToJsonString() << ",";
	
    }

    string res = ss.str();

    res = res.substr(0, res.length() - 1);

    res += "]";

    return res;
    
}

uint256 CEntry::getSignHash() const {

    stringstream ss;
    ss << m_type << m_createTime << m_value << m_fee << m_sender.ToString() << m_receiver.ToString();

    HSTR str = ss.str();

    return Hash(str.begin(), str.end());

}
