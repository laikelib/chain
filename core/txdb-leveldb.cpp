
#include <map>

#include <leveldb/env.h>
#include <leveldb/cache.h>
#include <leveldb/filter_policy.h>
#include <memenv/memenv.h>

#include <huibase.h>
#include <hfname.h>
#include <hstr.h>

#include "txdb-leveldb.h"

using namespace std;


leveldb::DB *txdb; // global pointer for LevelDB object instance

static leveldb::Options GetOptions() {
    leveldb::Options options;
    int nCacheSizeMB = 25;
    options.block_cache = leveldb::NewLRUCache(nCacheSizeMB * 1048576);
    options.filter_policy = leveldb::NewBloomFilterPolicy(10);
    return options;
}

static void init_blockindex(leveldb::Options& options, bool fRemoveOld = false, bool fCreateBootstrap = false) {
    // First time init.
    HCDic curDic = HCDic::GetCurrentDict ();
    curDic.GoToParent ();
    curDic.Append("data");
    HCDic dataDic = curDic;
    curDic.Append("laikelibindex");

    LOG(INFO) << "leveldb database: " << curDic.GetName();

    if (fRemoveOld) {
	
	LOG(INFO) << "Remove old level database";
	
	HCDic::Remove (curDic.GetName());
        unsigned int nFile = 1;

        while (true)
        {
	    dataDic.Append(HCStr::Format("blk%04u.dat", nFile));
	    HSTR strBlockFile = dataDic.GetName ();

            // Break if no such file
	    IF_FALSE(HCFileName::IsExists(strBlockFile)) {
                break;
	    }

            if (fCreateBootstrap && nFile == 1) {
		HCFileName::Remove(strBlockFile);
            }

            nFile++;
        }
    }

    curDic.CreateDic();
    leveldb::Status status = leveldb::DB::Open(options, curDic.GetName().c_str(), &txdb);
    LOG(INFO) << "Opening levelDB in " << curDic.GetName() << ", status: " << status.ok();
    HASSERT_THROW_MSG (status.ok(), "init_blockindex(): error opening database environment", MISS_FILE);
}

// CDB subclasses are created and destroyed VERY OFTEN. That's why
// we shouldn't treat this as a free operations.
CTxDB::CTxDB(const char* pszMode)
{
    assert(pszMode);
    activeBatch = NULL;
    fReadOnly = (!strchr(pszMode, '+') && !strchr(pszMode, 'w'));

    if (txdb) {

        pdb = txdb;

        return;
    }

    bool fCreate = strchr(pszMode, 'c');

    options = GetOptions();
    options.create_if_missing = fCreate;
    options.filter_policy = leveldb::NewBloomFilterPolicy(10);

    init_blockindex(options); // Init directory
    pdb = txdb;

    if (Exists(string("version")))
    {
        ReadVersion(nVersion);
	LOG(INFO) << "Transaction index version is " << nVersion;

        if (nVersion < LDB_VERSION)
        {
	    LOG(FATAL) << "Required index version is " << LDB_VERSION <<", removing old databases";

            // Leveldb instance destruction
            delete txdb;
            txdb = pdb = NULL;
            delete activeBatch;
            activeBatch = NULL;

            init_blockindex(options, true, true); // Remove directory and create new database
            pdb = txdb;

            bool fTmp = fReadOnly;
            fReadOnly = false;
            WriteVersion(LDB_VERSION); // Save transaction index version
            fReadOnly = fTmp;
        }
    }
    else if (fCreate)
    {
        bool fTmp = fReadOnly;
        fReadOnly = false;
        WriteVersion(LDB_VERSION);
        fReadOnly = fTmp;
    }

    LOG(INFO) << "Opened LevelDB successfully";

}

void CTxDB::Close()
{

    LOG(INFO) << "CTxDB::Close()";
    delete txdb;
    txdb = pdb = NULL;
    delete options.filter_policy;
    options.filter_policy = NULL;
    delete options.block_cache;
    options.block_cache = NULL;
    delete activeBatch;
    activeBatch = NULL;
}

bool CTxDB::TxnBegin()
{
    assert(!activeBatch);
    activeBatch = new leveldb::WriteBatch();
    return true;
}

bool CTxDB::TxnCommit()
{
    assert(activeBatch);
    leveldb::Status status = pdb->Write(leveldb::WriteOptions(), activeBatch);
    delete activeBatch;
    activeBatch = NULL;
    if (!status.ok()) {
	LOG(FATAL) << "LevelDB batch commit failure: " << status.ToString();
        return false;
    }
    return true;
}

class CBatchScanner : public leveldb::WriteBatch::Handler {
public:
    std::string needle;
    bool *deleted;
    std::string *foundValue;
    bool foundEntry;

    CBatchScanner() : foundEntry(false) {}

    virtual void Put(const leveldb::Slice& key, const leveldb::Slice& value) {
        if (key.ToString() == needle) {
            foundEntry = true;
            *deleted = false;
            *foundValue = value.ToString();
        }
    }

    virtual void Delete(const leveldb::Slice& key) {
        if (key.ToString() == needle) {
            foundEntry = true;
            *deleted = true;
        }
    }
};

// When performing a read, if we have an active batch we need to check it first
// before reading from the database, as the rest of the code assumes that once
// a database transaction begins reads are consistent with it. It would be good
// to change that assumption in future and avoid the performance hit, though in
// practice it does not appear to be large.
bool CTxDB::ScanBatch(const CDataStream &key, string *value, bool *deleted) const {
    assert(activeBatch);
    *deleted = false;
    CBatchScanner scanner;
    scanner.needle = key.str();
    scanner.deleted = deleted;
    scanner.foundValue = value;
    leveldb::Status status = activeBatch->Iterate(&scanner);
    HASSERT_THROW_MSG(status.ok(), "Scan Batch failed", SRC_FAIL);

    return scanner.foundEntry;
}


bool CTxDB::WriteBlockIndex(const CDiskBlockIndex &blockindex) {

    return Write(make_pair(string("blockindex"), blockindex.GetHash()), blockindex);

}

bool CTxDB::ReadHashBestChain(uint256 &hashBestChain) {

    return Read(string("hashBestChain"), hashBestChain);

}

bool CTxDB::WriteHashBestChain(uint256 hashBestChain) {

    return Write(string("hashBestChain"), hashBestChain);

}


bool CTxDB::WriteHashBestRoot(const uint256 &hashBestRoot) {

    return Write(string("hashBestRoot"), hashBestRoot);

}


bool CTxDB::ReadHashBestRoot(uint256 &hashBestRoot) {

    return Read(string("hashBestRoot"), hashBestRoot);

}
