/****************************************************************************
 *
 * File Name: entry.h
 *
 * Create Author: Tom Hui, tomhui1009@yahoo.com, 8613760232170
 *
 * Create Date: Thu Jan 18 19:19 2018
 *
 * Description:
 *	
 *
 ****************************************************************************/

#ifndef __ENTRY_H__
#define __ENTRY_H__

#include <huibase.h>
#include <htime.h>

#include "base58.h"

#include "uint256.h"

#include "serialize.h"

#include "sign.h"

#include <huibase.h>

using namespace HUIBASE;

class CEntry {
public:
    CEntry ();

    CEntry(const CEntry& entry);

    CEntry (HN64 nValue, HN64 nFee, const CLKAddress& addrSender, const CLKAddress& addrReceiver);

public:
    uint256 GetEntryHash ()const;

    bool IsBaseSign () const;

    bool Signature (const CKey& key);

    bool VerifySign () const;

    bool Check () const;

    bool operator< (const CEntry& entry) const{
	return GetEntryHash() < entry.GetEntryHash();
    }

    bool operator== (const CEntry& entry) const{
	return GetEntryHash() == entry.GetEntryHash();
    }

    IMPLEMENT_SERIALIZE(
			READWRITE(m_type);
			READWRITE(m_createTime);
			READWRITE(m_sender);
			READWRITE(m_receiver);
			READWRITE(m_sign);
			READWRITE(m_value);
			READWRITE(m_fee);
)

protected:    
    void SetNull ();

public:
    HUINT GetType () const { return m_type; }
    void SetType (HUINT nType) { m_type = nType; }

    HINT GetCreateTime () const { return m_createTime; }
    void SetCreateTime (HINT nCreateTime) { m_createTime = nCreateTime; }

    const CLKAddress& GetSender () const { return m_sender; }
    void SetSenderAddr (const CLKAddress& senderAddr) { m_sender = senderAddr; }

    const CLKAddress& GetReceiver () const { return m_receiver; }
    void SetReceiver (const CLKAddress& receiverAddr) { m_receiver = receiverAddr; }

    void ClearSender () { m_sender.SetEmpty(); }
    void ClearReceiver () { m_receiver.SetEmpty(); }

    // TODO: need to check sign
    const CSign& GetSign () const { return m_sign; }
    void SetSign (const CSign & sign ) { m_sign = sign; }

    HN64 GetValue () const { return m_value; }
    void SetValue (HN64 nValue) { m_value = nValue; }

    HN64 GetFee () const { return m_fee; }
    void SetFee (HN64 nFee) { m_fee = nFee; }

    HN64 GetAmount () const { return m_fee + m_value; }

    void PostFee (HN64 nFee) { m_value += nFee; }

    string ToJsonString () const;

    static string GetJsonFromEntrys (const std::vector<CEntry>& entrys);

private:
    uint256 getSignHash () const;
    
protected:
    //TODO: maybe add version;
    //TODO: change m_type to m_nflags;
    HUINT m_type;
    HINT m_createTime;
    HN64 m_value;
    HN64 m_fee;
    CLKAddress m_sender;
    CLKAddress m_receiver;
    CSign m_sign;
};

class CEntryDB : public CEntry {
public:
    CEntryDB() ;

    

public:
    const uint256& GetBlockHash ()const { return m_hashBlock; }

    void SetBlockHash (const uint256& hash) { m_hashBlock = hash; }

    HUINT GetHeight () const { return m_nHeight; }

    void SetHeight (HUINT nHeight) { m_nHeight = nHeight; }

    IMPLEMENT_SERIALIZE(
			READWRITE(m_type);
			READWRITE(m_createTime);
			READWRITE(m_sender);
			READWRITE(m_receiver);
			READWRITE(m_sign);
			READWRITE(m_value);
			READWRITE(m_fee);
			READWRITE(m_hashBlock);
			READWRITE(m_nHeight);
)
    
private:
    uint256 m_hashBlock;
    HUINT m_nHeight;
}; 

#endif 









