

#include <blockindex.h>
#include "cointxdb.h"
#include <serialize.h>
#include <ifnames.h>
#include "chainApp.h"

CCoinTxdb::CCoinTxdb(const char* pszmode)
    : CTxDB(pszmode) {}



bool CCoinTxdb::LoadBlockIndex() {

    IF_FALSE(so_config->IsBlockIndexEmpty()) {
        LOG(INFO) << "block index map is empty...";
        // Already loaded in this session.
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

        if (strType != "blockindex") {

            LOG(INFO) << "no blockindex found..., break";
            break;

        }

        CDiskBlockIndex diskindex;
        dsValue >> diskindex;

        uint256 blockHash = diskindex.GetHash ();

        CBlockIndex* pIndexNew = so_config->InsertBlockIndex(blockHash);
        pIndexNew->SetPrevIndex (so_config->InsertBlockIndex(diskindex.hashPrev));
        pIndexNew->SetNextIndex (so_config->InsertBlockIndex(diskindex.hashNext));
        pIndexNew->SetFileIn (diskindex.GetFileIn());
        pIndexNew->SetBlockPos (diskindex.GetBlockPos ());
        pIndexNew->SetHeight (diskindex.GetHeight());
        pIndexNew->SetMoneySupply(diskindex.GetMoneySupply());

        pIndexNew->SetVersion (diskindex.GetVersion());
        pIndexNew->SetType (diskindex.GetType());
        pIndexNew->SetHashPrevBlock(diskindex.GetHashPrevBlock());
        pIndexNew->SetHashMerkleRoot (diskindex.GetMerkleRoot());
        pIndexNew->SetTime (diskindex.GetTime());
        pIndexNew->SetBits (diskindex.GetBits ());
        pIndexNew->SetNonce (diskindex.GetNonce ());
        pIndexNew->SetCount (diskindex.GetCount());

        pIndexNew->SetRoot (diskindex.GetRoot());
        pIndexNew->SetRootHeight (diskindex.GetRootHeight());
        pIndexNew->SetMain (diskindex.IsMain());

        so_config->SetIndexHeight(blockHash, pIndexNew->GetHeight());

        if (so_config->GetGenesisIndex() == nullptr && blockHash ==
            so_config->GetGenesisBlockHash()) {

            LOG(INFO) << "found the genesis block index: " << pIndexNew->GetHash().ToString();
            so_config->SetGenesisIndex (pIndexNew);

        }

        iterator->Next ();

    }

    delete iterator;

    uint256 hashBestChain, hashBestRoot;

    if (not ReadHashBestChain(hashBestChain)) {

        if (so_config->GetGenesisIndex() == nullptr) {
            LOG(INFO) << "need init block chain...";
            return true;
        }

        LOG(ERROR) << "read hash best chain failed";
        return false;

    }

    LOG(INFO) << "best hash: " << hashBestChain.ToString();

    IF_FALSE(so_config->HasBlock(hashBestChain)) {

        LOG(ERROR) << "best hash not loaded";
        return false;

    }

    if (not ReadHashBestRoot(hashBestRoot)) {

        LOG(ERROR) << "read hash best root failed";
        return false;

    }

    LOG(INFO) << "best root: " << hashBestRoot.ToString();

    so_config->SetHashBestRoot(hashBestRoot);

    so_config->SetHashBest(hashBestChain);

    // bestindex, bestheight
    so_config->initBestIndex ();

    LOG(INFO) << "CCoinTxdb LoadBlockIndex Success";
    return true;
}


