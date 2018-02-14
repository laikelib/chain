
#include "db.h"


#include "ifnames.h"
#include "util.h"

#include <openssl/rand.h>
#include <huibase.h>
#include <hfname.h>
#include <glog/logging.h>
using namespace HUIBASE;

CDBEnv bitdb;

void CDBEnv::EnvShutdown()
{
    if (!fDbEnvInit)
        return;

    fDbEnvInit = false;
    int ret = dbenv.close(0);
    if (ret != 0) {
	LOG(ERROR) << "EnvShutdown exception:" << DbEnv::strerror(ret);
    }
    
    if (!fMockDb) {
        DbEnv(0).remove(m_dicEnv.GetName().c_str(), 0);
    }
}

CDBEnv::CDBEnv() : dbenv(DB_CXX_NO_EXCEPTIONS)
{
    fDbEnvInit = false;
    fMockDb = false;
}

CDBEnv::~CDBEnv()
{
    EnvShutdown();
}

void CDBEnv::Close()
{
    LOG(INFO) << "CDBEnv::Close()";
    EnvShutdown();
}

bool CDBEnv::Open(HCSTRR strDicFileName)
{
    LOG(INFO) << "CDBEnv::Open, strDicFileName: " << strDicFileName;
    
    if (fDbEnvInit)
        return true;

    m_dicEnv.SetName (strDicFileName);
    HCDic _dbdic = m_dicEnv;
    _dbdic.Append ("database");
    IF_FALSE (_dbdic.IsExactExists()) {
	HIF_NOTOK(_dbdic.CreateDic()) {
	    return false;
	}
    }

    HSTR strDbLogFile = _dbdic.GetName () + "/db.log";
    LOG(INFO) << "dbenv.open LogDir=" << _dbdic.GetName() << ", ErrorFile=" << strDbLogFile;
    
    unsigned int nEnvFlags = 0;
    nEnvFlags = nEnvFlags |  DB_PRIVATE;
    
    dbenv.set_lg_dir(_dbdic.GetName().c_str());
    dbenv.set_cachesize(25 / 1024, (25 % 1024)*1048576, 1);
    dbenv.set_lg_bsize(1048576);
    dbenv.set_lg_max(10485760);
    dbenv.set_lk_max_locks(10000);
    dbenv.set_lk_max_objects(10000);
    dbenv.set_errfile(fopen(strDbLogFile.c_str(), "a")); 
    dbenv.set_flags(DB_AUTO_COMMIT, 1);
    dbenv.set_flags(DB_TXN_WRITE_NOSYNC, 1);
#ifdef DB_LOG_AUTO_REMOVE
    dbenv.log_set_config(DB_LOG_AUTO_REMOVE, 1);
#endif
    int ret = dbenv.open(m_dicEnv.GetName().c_str(),
                     DB_CREATE     |
                     DB_INIT_LOCK  |
                     DB_INIT_LOG   |
                     DB_INIT_MPOOL |
                     DB_INIT_TXN   |
                     DB_THREAD     |
                     DB_RECOVER    |
                     nEnvFlags,
                     S_IRUSR | S_IWUSR);
    if (ret != 0) {
	LOG(ERROR) << "CDB() : error " << DbEnv::strerror(ret) << "(" << ret  <<  ") opening database environment";
	return false;
    }

    fDbEnvInit = true;
    fMockDb = false;

    return true;
}

void CDBEnv::MakeMock() throw (HCException)
{
    if (fDbEnvInit) {
	HASSERT_THROW(false, UN_INIT);
    }

    LOG(INFO) << "CDBEnv::MakeMock()";

    dbenv.set_cachesize(1, 0, 1);
    dbenv.set_lg_bsize(10485760*4);
    dbenv.set_lg_max(10485760);
    dbenv.set_lk_max_locks(10000);
    dbenv.set_lk_max_objects(10000);
    dbenv.set_flags(DB_AUTO_COMMIT, 1);
#ifdef DB_LOG_IN_MEMORY
    dbenv.log_set_config(DB_LOG_IN_MEMORY, 1);
#endif
    int ret = dbenv.open(NULL,
                     DB_CREATE     |
                     DB_INIT_LOCK  |
                     DB_INIT_LOG   |
                     DB_INIT_MPOOL |
                     DB_INIT_TXN   |
                     DB_THREAD     |
                     DB_PRIVATE,
                     S_IRUSR | S_IWUSR);

    HASSERT_THROW_MSG(ret == 0, "CDBEnv::MakeMock(): error opening database envirment", UN_INIT);

    fDbEnvInit = true;
    fMockDb = true;
}

CDBEnv::VerifyResult CDBEnv::Verify(std::string strFile, bool (*recoverFunc)(CDBEnv& dbenv, std::string strFile))
{
    assert(mapFileUseCount.count(strFile) == 0);

    Db db(&dbenv, 0);
    int result = db.verify(strFile.c_str(), NULL, NULL, 0);
    if (result == 0)
        return VERIFY_OK;
    else if (recoverFunc == NULL)
        return RECOVER_FAIL;

    // Try to recover:
    bool fRecovered = (*recoverFunc)(*this, strFile);
    return (fRecovered ? RECOVER_OK : RECOVER_FAIL);
}

bool CDBEnv::Salvage(std::string strFile, bool fAggressive,
                     std::vector<CDBEnv::KeyValPair >& vResult)
{
    assert(mapFileUseCount.count(strFile) == 0);

    u_int32_t flags = DB_SALVAGE;
    if (fAggressive) flags |= DB_AGGRESSIVE;

    stringstream strDump;

    Db db(&dbenv, 0);
    int result = db.verify(strFile.c_str(), NULL, &strDump, flags);
    if (result == DB_VERIFY_BAD)
    {
	LOG(FATAL) << "Error: Salvage found errors, all data may not be recoverable.";
    
        if (!fAggressive)
        {
	    LOG(FATAL) << "Error: Rerun with aggressive mode to ignore errors and continue.";
            return false;
        }
    }
    if (result != 0 && result != DB_VERIFY_BAD)
    {
	LOG(FATAL) << "db salvage failed: " << result;
        return false;
    }

    // Format of bdb dump is ascii lines:
    // header lines...
    // HEADER=END
    // hexadecimal key
    // hexadecimal value
    // ... repeated
    // DATA=END

    string strLine;
    while (!strDump.eof() && strLine != "HEADER=END")
        getline(strDump, strLine); // Skip past header

    std::string keyHex, valueHex;
    while (!strDump.eof() && keyHex != "DATA=END")
    {
        getline(strDump, keyHex);
        if (keyHex != "DATA=END")
        {
            getline(strDump, valueHex);
            vResult.push_back(make_pair(ParseHex(keyHex),ParseHex(valueHex)));
        }
    }

    return (result == 0);
}


void CDBEnv::CheckpointLSN(const std::string& strFile)
{
    dbenv.txn_checkpoint(0, 0, 0);
    if (fMockDb)
        return;
    dbenv.lsn_reset(strFile.c_str(), 0);
}


CDB::CDB(const std::string& strFilename, const char* pszMode) :
    pdb(nullptr), activeTxn(nullptr)
{
    int ret;
    fReadOnly = (!strchr(pszMode, '+') && !strchr(pszMode, 'w'));
    if (strFilename.empty()) {
        return;
    }

    bool fCreate = strchr(pszMode, 'c');
    unsigned int nFlags = DB_THREAD;
    if (fCreate) {
        nFlags |= DB_CREATE;
    }

    HCDic dataDic(strFilename);
    (void)dataDic.GoToParent();
    LOG(INFO) << "bitdb env dic: " << dataDic.GetName ();
    HASSERT_THROW_MSG(bitdb.Open(dataDic.GetName().c_str()), "env open failed", SRC_FAIL);

    LOG(INFO) << "db file: " << strFilename;

    strFile = strFilename;
    ++bitdb.mapFileUseCount[strFile];
    pdb = bitdb.mapDb[strFile];
    if (pdb == NULL){

	LOG(INFO) << "new open db";
	pdb = new Db(&bitdb.dbenv, 0);

	bool fMockDb = bitdb.IsMock();
	if (fMockDb){
	    LOG(INFO) << "mackdb...";
	    DbMpoolFile*mpf = pdb->get_mpf();
	    ret = mpf->set_flags(DB_MPOOL_NOFILE, 1);
	    HASSERT_THROW_MSG(ret == 0, "CDB: Failed to configure for no temp file backing for database", MISS_FILE);
	}
	ret = pdb->open(NULL, // Txn pointer
			fMockDb ? NULL : strFile.c_str(), // Filename
			fMockDb ? strFile.c_str() : "main", // Logical db name
			DB_BTREE, // Database type
			nFlags, // Flags
			0);

	if (ret != 0){
	    LOG(ERROR) << "Open db failed...";
	    delete pdb;
	    pdb = NULL;
	    --bitdb.mapFileUseCount[strFile];
	    strFile = "";
	    HASSERT_THROW_MSG(ret == 0, "CDB: cann't open database", SRC_FAIL);

	}

	if (fCreate && !Exists(string("version"))) {

	    bool fTmp = fReadOnly;
	    fReadOnly = false;
	    WriteVersion(NODE_VERSION);
	    fReadOnly = fTmp;

	    LOG(INFO) << "Opened db success, write version code to db";
	}

	bitdb.mapDb[strFile] = pdb;
    }

    LOG(INFO) << "pdb is: " << pdb;
}



void CDB::Close()
{
    if (!pdb) {
        return;
    }
    
    if (activeTxn) {
        activeTxn->abort();
    }
    activeTxn = NULL;
    pdb = NULL;

    // Flush database activity from memory pool to disk log
    unsigned int nMinutes = 0;
    if (fReadOnly) {
        nMinutes = 1;
    }

    bitdb.dbenv.txn_checkpoint(nMinutes ? 100 * 1024 : 0, nMinutes, 0);

    {
        --bitdb.mapFileUseCount[strFile];
    }
}

void CDBEnv::CloseDb(const string& strFile)
{
    {
        if (mapDb[strFile] != NULL)
        {
            // Close the database handle
            Db* pdb = mapDb[strFile];
            pdb->close(0);
            delete pdb;
            mapDb[strFile] = NULL;
        }
    }
}

bool CDBEnv::RemoveDb(const string& strFile)
{
    this->CloseDb(strFile);

    int rc = dbenv.dbremove(NULL, strFile.c_str(), NULL, DB_AUTO_COMMIT);
    return (rc == 0);
}

bool CDB::Rewrite(const string& strFile, const char* pszSkip)
{
    while (true)
    {
        {

            if (!bitdb.mapFileUseCount.count(strFile) || bitdb.mapFileUseCount[strFile] == 0)
            {
                // Flush log data to the dat file
                bitdb.CloseDb(strFile);
                bitdb.CheckpointLSN(strFile);
                bitdb.mapFileUseCount.erase(strFile);

                bool fSuccess = true;
		LOG(INFO) << "Rewriting";

                string strFileRes = strFile + ".rewrite";
                { // surround usage of db with extra {}
                    CDB db(strFile.c_str(), "r");
                    Db* pdbCopy = new Db(&bitdb.dbenv, 0);

                    int ret = pdbCopy->open(NULL,                 // Txn pointer
                                            strFileRes.c_str(),   // Filename
                                            "main",    // Logical db name
                                            DB_BTREE,  // Database type
                                            DB_CREATE,    // Flags
                                            0);
                    if (ret > 0)
                    {
			LOG(FATAL) << "Cannot create database file";

                        fSuccess = false;
                    }

                    Dbc* pcursor = db.GetCursor();
                    if (pcursor)
                        while (fSuccess)
                        {
                            CDataStream ssKey(SER_DISK, NODE_VERSION);
                            CDataStream ssValue(SER_DISK, NODE_VERSION);
                            int ret = db.ReadAtCursor(pcursor, ssKey, ssValue, DB_NEXT);
                            if (ret == DB_NOTFOUND)
                            {
                                pcursor->close();
                                break;
                            }
                            else if (ret != 0)
                            {
                                pcursor->close();
                                fSuccess = false;
                                break;
                            }
                            if (pszSkip &&
                                strncmp(&ssKey[0], pszSkip, std::min(ssKey.size(), strlen(pszSkip))) == 0)
                                continue;
                            if (strncmp(&ssKey[0], "\x07version", 8) == 0)
                            {
                                // Update version:
                                ssValue.clear();
                                ssValue << NODE_VERSION;
                            }
                            Dbt datKey(&ssKey[0], ssKey.size());
                            Dbt datValue(&ssValue[0], ssValue.size());
                            int ret2 = pdbCopy->put(NULL, &datKey, &datValue, DB_NOOVERWRITE);
                            if (ret2 > 0)
                                fSuccess = false;
                        }
                    if (fSuccess)
                    {
                        db.Close();
                        bitdb.CloseDb(strFile);
                        if (pdbCopy->close(0))
                            fSuccess = false;
                        delete pdbCopy;
                    }
                }
                if (fSuccess)
                {
                    Db dbA(&bitdb.dbenv, 0);
                    if (dbA.remove(strFile.c_str(), NULL, 0))
                        fSuccess = false;
                    Db dbB(&bitdb.dbenv, 0);
                    if (dbB.rename(strFileRes.c_str(), NULL, strFile.c_str(), 0))
                        fSuccess = false;
                }
                if (!fSuccess){
		    LOG(FATAL) << "Rewriting failed!";
		}
                return fSuccess;
            }
        }

    }
    return false;
}


void CDBEnv::Flush(bool fShutdown)
{

    // Flush log data to the actual data file
    //  on all files that are not in use
    LOG(INFO) << "Flush...." ;

    if (!fDbEnvInit)
        return;
    {

        map<string, int>::iterator mi = mapFileUseCount.begin();
        while (mi != mapFileUseCount.end())
        {
            string strFile = (*mi).first;
            int nRefCount = (*mi).second;
	    LOG(INFO) << strFile <<  "refcount=" << nRefCount;

            if (nRefCount == 0)
            {
                // Move log data to the dat file
                CloseDb(strFile);
		LOG(INFO) << strFile << " checkpoint" ;
                dbenv.txn_checkpoint(0, 0, 0);
                if (!fMockDb)
                    dbenv.lsn_reset(strFile.c_str(), 0);
                mapFileUseCount.erase(mi++);
            }
            else
                mi++;
        }

        if (fShutdown)
        {
            char** listp;
            if (mapFileUseCount.empty())
            {
                dbenv.log_archive(&listp, DB_ARCH_REMOVE);
                Close();
            }
        }
    }
}

