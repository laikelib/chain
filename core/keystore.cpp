
#include "keystore.h"



bool CKeyStore::GetPubKey(const CKeyID &address, CPubKey &vchPubKeyOut) const
{
    CKey key;
    if (!GetKey(address, key))
        return false;
    vchPubKeyOut = key.GetPubKey();
    return true;
}

bool CKeyStore::AddKey(const CKey &key) {
    return AddKeyPubKey(key, key.GetPubKey());
}


bool CBasicKeyStore::IsWithMe(const CLKAddress &addr) const{
    
    for (KeyMap::const_iterator cit = mapKeys.begin();
	 cit != mapKeys.end(); ++cit) {

	CLKAddress temp;
	temp.Set(cit->first);
	if (temp == addr) {

	    return true;
		
	}

    }

    return false;
}


bool CBasicKeyStore::AddKeyPubKey(const CKey& key, const CPubKey &pubkey)
{
    mapKeys[pubkey.GetID()] = key;
    return true;
}


