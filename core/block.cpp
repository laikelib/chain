
#include <huibase.h>
#include "block.h"
#include "ifnames.h"
#include "serialize.h"
#include <hstr.h>
#include <glog/logging.h>
#include "txdb-leveldb.h"
#include "util.h"
#include <sstream>
#include "base58.h"
#include <json/json.h>

using namespace HUIBASE;

static string BlockFilePath(unsigned int nFile)
{
    string strBlockFn = HCStr::Format("blk%04u.dat", nFile);
    return "/data/laikelib/chain/sos/main/data/" + strBlockFn;
}


FILE* OpenBlockFile(unsigned int nFile, unsigned int nBlockPos, const char* pszMode)
{
    if ((nFile < 1) || (nFile == (unsigned int) -1))
        return NULL;
    FILE* file = fopen(BlockFilePath(nFile).c_str(), pszMode);
    if (!file)
        return NULL;
    if (nBlockPos != 0 && !strchr(pszMode, 'a') && !strchr(pszMode, 'w'))
    {
        if (fseek(file, nBlockPos, SEEK_SET) != 0)
        {
            fclose(file);
            return NULL;
        }
    }
    return file;
}


FILE* AppendBlockFile(unsigned int& nFileRet)
{

    static unsigned int nCurrentBlockFile = 1;
    nFileRet = 0;
    while (true)
    {
        FILE* file = OpenBlockFile(nCurrentBlockFile, 0, "ab");
        if (!file)
            return NULL;
        if (fseek(file, 0, SEEK_END) != 0)
            return NULL;
        // FAT32 file size max 4GB, fseek and ftell max 2GB, so we must stay under 2GB
        if (ftell(file) < (long)(0x7F000000 - MAX_SIZE))
        {
            nFileRet = nCurrentBlockFile;
            return file;
        }
        fclose(file);
        nCurrentBlockFile++;
    }
}


CBlock::CBlock() {

    


}


bool CBlock::WriteToDisk(unsigned int &nFileRet, unsigned int &nBlockPosRet) {

    CAutoFile fileout = CAutoFile(AppendBlockFile(nFileRet), SER_DISK, NODE_VERSION);
    HASSERT_THROW_MSG(fileout != nullptr, "CBlock::WriteToDisk(): AppendBlockFile Failed", ILL_PT);

    // Write index header
    unsigned int nSize = fileout.GetSerializeSize(*this);
    fileout << nSize;

    // Write block
    long fileOutPos = ftell(fileout);
    HASSERT_THROW_MSG(fileOutPos >= 0, "CBlock::WriteToDisk(): ftell failed", ERR_STATUS);

    LOG(INFO) << "Write block position: " << fileOutPos;

    nBlockPosRet = fileOutPos;
    fileout << *this;

    // Flush stdio buffers and commit to disk before returning
    fflush(fileout);

    fsync(fileno(fileout));

    return true;

}


bool CBlock::ReadFromDisk (const CBlockIndex* pindex) {

    if (not ReadFromDisk(pindex->GetFileIn(), pindex->GetBlockPos())) {

	LOG(ERROR) << "Block read from disk failed, file[" << pindex->GetFileIn()
		   << "], pos[" << pindex->GetBlockPos() << "]";

	return false;
	
    }
    
    if (GetHash() != pindex->GetHash()) {
	
	LOG(ERROR) << "block read from disk failed. block hash: ["
		   << GetHash().ToString() << "], index hash: ["
		   << pindex->GetHash().ToString() << "]";

	return false;
    }

    return true;
}


bool CBlock::ReadFromDisk (unsigned int nFile, unsigned int nBlockPos) {

    // Open history file to read
    CAutoFile filein = CAutoFile(OpenBlockFile(nFile, nBlockPos, "rb"), SER_DISK, NODE_VERSION);
    if (!filein) {

	LOG(ERROR) << "CBlock::ReadFromDisk() : OpenBlockFile failed";
	return false;
	
    }
    
    // Read block
    try {
	
	filein >> *this;
	
    }
    catch (std::exception &e) {
	
	LOG(ERROR) << "read block, get an exception";
	return false;
	
    }


    return true;

}


void CBlock::SetBaseEntry(const CEntry &entry) {

    ++m_nCount;

    m_entrys.push_back(entry);

}


bool CBlock::BlockEntry(const CEntry &entry) {

    // check whether with a base entry
    if (not HasBaseEntry()) {
	return false;
    }

    // TODO: check entry
    AddFees(entry.GetFee());
    
    m_entrys.push_back(entry);

    ++m_nCount;

    return true;
}


bool CBlock::BlockEntrys(const std::vector<CEntry>& entrys) {

    if (not HasBaseEntry()) {

	return false;
	
    }

    HN64 nFee = 0;
    for (size_t i = 0; i < entrys.size(); ++i) {

	const CEntry& cr = entrys[i];

	nFee += cr.GetFee();

	m_entrys.push_back(cr);
	
	++m_nCount; 

    }

    AddFees(nFee);

    return true;
}


bool CBlock::HasBaseEntry() const{

    for (size_t i = 0; i < m_entrys.size(); ++i) {

	const CEntry& ce = m_entrys[i];

	if (ce.IsBaseSign()) {

	    return true;
	    
	}

    }

    return false;

}


bool CBlock::CheckBlock() const {

    if (m_nCount != m_entrys.size()) {

        LOG(ERROR) << "check block failed, count is not match the entry size";

        return false;

    }

    if (GetEntrysSize() < 1) {

        LOG(ERROR) << "check block failed, entry size: " << GetEntrysSize();

        return false;

    }

    if (not CheckPow()) {

        LOG(ERROR) << "check pow failed.";

        return false;

    }

    if (not CheckCoinBase()) {

        LOG(ERROR) << "check coinbase failed...";
        return false;

    }

    if (not CheckEntrys()) {

        LOG(ERROR) << "check entrys failed ...";
        return false;

    }

    if (not CheckMerkleRoot()) {

        LOG(ERROR) << "check merkle root failed ...";
        return false;

    }

    return true;
}


bool CBlock::CheckCoinBase() const {

    int basecount = 0;
    
    for (size_t i = 0; i < m_entrys.size(); ++i) {

	const CEntry& cen = m_entrys[i];

	if (cen.IsBaseSign()) {

	    ++basecount;

	}

    }

    return basecount == 1;

}



const CEntry& CBlock::GetBaseEntry() const {

    for (size_t i = 0; i < m_entrys.size(); ++i) {

	const CEntry& ce = m_entrys[i];

	if (ce.IsBaseSign ()) {

	    return ce;

	}

    }

    HASSERT_THROW(HasBaseEntry(), POS_END);

    static CEntry temp;

    return temp;
}


void CBlock::AddFees(HN64 nFee) {

    for (size_t i = 0; i < m_entrys.size(); ++i) {

	CEntry& en = m_entrys[i];

	if (en.IsBaseSign ()) {

	    en.PostFee(nFee);

	    return;

	}

    }
    
}

bool CBlock::BuildMerkleRootTree() {

    m_hashMerkleRoot = MakeMerkleRoot();

    return true;
}

string CBlock::ToJsonString() const {

    std::stringstream ss;

    ss << "{ \"version\":" << m_nVersion << ", \"type\":"<< m_nType
       << ", \"hashPrevBlock\":\"" << m_hashPrevBlock.ToString()
       << "\", \"hashMerkleRoot\":\"" << m_hashMerkleRoot.ToString() << "\",\"time\": " << m_nTime
       << ", \"bits\":" << m_nBits <<", \"nonce\":"<< m_nNonce << ",\"count\":" << m_nCount << ", \"entrys\" : [ ";

    for (size_t i = 0; i < m_entrys.size(); ++i) {

	const CEntry& en = m_entrys[i];

	ss << "{ \"type\": " << en.GetType() << ", \"createTime\":"
	   << en.GetCreateTime() << ", \"sender\":\"" << en.GetSender().ToString()
	   << "\", \"receiver\":\"" << en.GetReceiver().ToString() << "\", \"sign\":\"" << en.GetSign().ToString() << "\","
	   << "\"value\":" << en.GetValue() << ", \"fee\":"<< en.GetFee()
	   << "},";

    }

    string res = ss.str();

    res = res.substr(0, res.length() - 1);

    res += "]}";

    return res;
   
}


string CBlock::GetBlockJson () const {

    std::stringstream ss;

    ss << "{ \"hash\":\"" << GetHash().ToString() << "\",\"createBy\":\""
       << GetCreateBy().ToString() << "\", \"amount\":"
       << GetAmount() << ",\"version\":" << m_nVersion << ", \"type\":"<< m_nType
       << ", \"hashPrevBlock\":\"" << m_hashPrevBlock.ToString()
       << "\", \"hashMerkleRoot\":\"" << m_hashMerkleRoot.ToString() << "\",\"time\": " << m_nTime
       << ", \"bits\":" << m_nBits <<", \"nonce\":"<< m_nNonce << ",\"count\":" << m_nCount << ", \"entrys\" : [ ";

    for (size_t i = 0; i < m_entrys.size(); ++i) {

	const CEntry& en = m_entrys[i];

	ss << "{ \"hash\": \"" << en.GetEntryHash().ToString() << "\",  \"type\": " << en.GetType() << ", \"createTime\":"
	   << en.GetCreateTime() << ", \"sender\":\"" << en.GetSender().ToString()
	   << "\", \"receiver\":\"" << en.GetReceiver().ToString() << "\", \"sign\":\"" << en.GetSign().ToString() << "\","
	   << "\"value\":" << en.GetValue() << ", \"fee\":"<< en.GetFee()
	   << "},";

    }

    string res = ss.str();

    res = res.substr(0, res.length() - 1);

    res += "]}";

    return res;

}


bool CBlock::FromJson(const string &strJson) {


    Json::Reader reader;
    Json::Value root;

    if (not reader.parse (strJson, root)) {

	LOG(ERROR) << "parse json failed...";
	return false;

    }

    SetVersion(root["version"].asInt());
    LOG(INFO) << "version: " << root["version"].asInt();
    SetType(root["type"].asInt());
    LOG(INFO) << "type: " << root["type"].asInt();

    uint256 temp;
    temp.SetHex(root["hashPrevBlock"].asString());
    LOG(INFO) << "hash prev block: [" << temp.ToString() << "]";
    SetHashPrevBlock(temp);

    temp.SetNull ();
    temp.SetHex(root["hashMerkleRoot"].asString());
    LOG(INFO) << "hash merkle root: [" <<  temp.ToString() << "]";
    SetHashMerkleRoot(temp);

    SetTime(root["time"].asInt());
    LOG(INFO) << "time: " << root["time"].asInt();
    SetBits(root["bits"].asInt());
    LOG(INFO) << "bits: " << root["bits"].asInt();
    SetNonce(root["nonce"].asUInt());
    LOG(INFO) << "nonce: " <<  root["nonce"].asUInt();
    SetCount(root["count"].asInt());
    LOG(INFO) << "count: " << root["count"].asInt();

    m_entrys.clear();

    Json::Value entrys = root["entrys"];
    CLKAddress tempaddr;

    LOG(INFO) << "entrys size: " << entrys.size();
    for(unsigned int i = 0; i < entrys.size(); ++i) {

	CEntry en;
	LOG(INFO) << "entry <"<< i <<">: ";
	en.SetType(entrys[i]["type"].asInt());
	LOG(INFO) << "    " << i << "> createTime: " << entrys[i]["createTime"].asInt();
	en.SetCreateTime(entrys[i]["createTime"].asInt());

	tempaddr.SetEmpty();
	tempaddr.SetString(entrys[i]["sender"].asString());
	LOG(INFO) << "    " << i << "> sender: " << entrys[i]["sender"].asString();

	en.SetSenderAddr(tempaddr);

	tempaddr.SetEmpty();
	tempaddr.SetString(entrys[i]["receiver"].asString());
	LOG(INFO) << "    " << i << "> receiver: " << entrys[i]["receiver"].asString();

	en.SetReceiver(tempaddr);

	CSign tempsign;

	tempsign.FromString(entrys[i]["sign"].asString());

	en.SetSign(tempsign);
	LOG(INFO) << "    " << i << "> sign: " << entrys[i]["sign"].asString();

	en.SetValue(entrys[i]["value"].asInt64());
	LOG(INFO) << "    " << i << "> value: " << entrys[i]["value"].asInt64();

	en.SetFee(entrys[i]["fee"].asInt64());
	LOG(INFO) << "    " << i << "> fee: " << entrys[i]["fee"].asInt64();

	m_entrys.push_back(en);
    }

    return true;
}


bool CBlock::CheckPow() const {
    
    CBigNum bnTarget;
    bnTarget.SetCompact(GetBits());

    
    return GetHash() < bnTarget.getuint256();

}


bool CBlock::CheckEntrys() const {

    for (size_t i = 0; i < m_entrys.size(); ++i) {

	const CEntry& cen = m_entrys[i];

	if (not cen.Check()) {

	    return false;

	}
	
    }

    return true;

}

uint256 CBlock::MakeMerkleRoot () const {

    std::vector<uint256> tree;

    for (const auto &en : m_entrys) {

	tree.push_back(en.GetEntryHash ());

    }

    int j = 0;
    
    for (int nSize = m_entrys.size(); nSize > 1; nSize = (nSize + 1) / 2) {

	for (int i = 0; i < nSize; i += 2) {

	    int i2 = std::min(i+1, nSize-1);
	    
	    tree.push_back(Hash(BEGIN(tree[j+i]),  END(tree[j+i]),
				BEGIN(tree[j+i2]), END(tree[j+i2])));
	}
	
	j += nSize;
	
    }

    return tree.empty() ? 0 : tree.back();

}


bool CBlock::CheckMerkleRoot() const {

    return GetMerkleRoot() == MakeMerkleRoot();

}



const std::vector<CEntry>& CBlock::GetEntrys() const {

    return m_entrys;
    
}

const CLKAddress& CBlock::GetCreateBy () const {

    const CEntry& baseEn = GetBaseEntry();

    return baseEn.GetReceiver();

}

HN64 CBlock::GetAmount () const {

    HN64 ret = 0;

    for (const auto& en : m_entrys) {

	ret += en.GetAmount();

    }

    return ret;

}
