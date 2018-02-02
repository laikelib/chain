

#include <blockindex.h>
#include "cointxdb.h"
#include <serialize.h>
#include <ifnames.h>

CCoinTxdb::CCoinTxdb(const char* pszmode)
    : CTxDB(pszmode) {}



bool CCoinTxdb::LoadBlockIndex() {

    if (so_config->GetBlockIndexMap().size() > 0) {

	// Already loaded in this session. It can happen during
	// migration from BDB.
	return true;

    }

    // Read block index from leveldb that on the disk to memory.
    leveldb::Iterator * iterator = pdb->NewIterator(leveldb::ReadOptions());

    CDataStream dsStartKey(SER_DISK, NODE_VERSION);
    dsStartKey << make_pair(string("blockindex"), uint256(0));
    iterator->Seek(dsStartKey.str());

    while (iterator->Valid()) {

	CDataStream dsKey (SER_DISK, NODE_VERSION);
	dsKey.write(iterator->key().data(), iterator->key().size());

	CDataStream dsValue(SER_DISK, NODE_VERSION);
	dsValue.write(iterator->value().data(), iterator->value().size());

	string strType;
	dsKey >> strType;

	LOG(INFO) << "type: " << strType;

	// if strType is not "blockindex", that means we meet the
	// end of the data.
	if (strType != "blockindex") {
	    break;
	}

	CDiskBlockIndex diskindex;
	dsValue >> diskindex;

	uint256 blockHash = diskindex.GetHash ();

	LOG(INFO) << "hash: " << blockHash.ToString();

	CBlockIndex* pIndexNew = so_config->InsertBlockIndex(blockHash);
	pIndexNew->SetPrevIndex (so_config->InsertBlockIndex(diskindex.hashPrev));
	pIndexNew->SetNextIndex (so_config->InsertBlockIndex(diskindex.hashNext));
	pIndexNew->SetFileIn (diskindex.GetFileIn());
	pIndexNew->SetBlockPos (diskindex.GetBlockPos ());
	pIndexNew->SetHeight (diskindex.GetHeight());
	pIndexNew->SetMoneySupply(diskindex.GetMoneySupply());

	pIndexNew->SetVersion (diskindex.GetVersion());
	pIndexNew->SetType (diskindex.GetType());
	pIndexNew->SetLength(diskindex.GetLength());
	pIndexNew->SetHashPrevBlock(diskindex.GetHashPrevBlock());
	pIndexNew->SetHashMerkleRoot (diskindex.GetMerkleRoot());
	pIndexNew->SetTime (diskindex.GetTime());
	pIndexNew->SetBits (diskindex.GetBits ());
	pIndexNew->SetNonce (diskindex.GetNonce ());
	pIndexNew->SetWhoAddress (diskindex.GetAddre());
	pIndexNew->SetCount (diskindex.GetCount());

	if (so_config->GetGenesisIndex() == nullptr && blockHash ==
	    so_config->GetGenesisBlockHash()) {
	    so_config->SetGenesisIndex (pIndexNew);
	}

	iterator->Next ();
		
    }

    delete iterator;

    uint256 hashBestChain;

    if (not ReadHashBestChain(hashBestChain)) {

	if (so_config->GetGenesisIndex() == nullptr) {
	    LOG(INFO) << "need init block chain...";
	    return true;
	}
	LOG(ERROR) << "read hash best chain failed";
	return false;
	
    }

    if (not so_config->GetBlockIndexMap ().count(hashBestChain)) {
	LOG(ERROR) << "best hash not loaded";
	return false;
    }
    
    so_config->SetHashBest(hashBestChain);

    // bestindex, bestheight
    so_config->SetBestIndex (so_config->GetBlockIndex(hashBestChain));
    so_config->SetBestHeight (so_config->GetBestIndex()->GetHeight());

    LOG(INFO) << "CCoinTxdb LoadBlockIndex Success";
    return true;
}


bool CCoinTxdb::ReadHashBestChain(uint256& hashBestChain) {

    return Read(string("hashBestChain"), hashBestChain);
    
}














