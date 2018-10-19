
#ifndef __IFNAMES_H__
#define __IFNAMES_H__

#include <huibase.h>

using namespace HUIBASE;

// interface name "block_check"
#define IFN_BK_CHECK block_check

#define IFN_TX_CHECK transaction_check

#define IFN_TX transaction


// version code for node client.
static constexpr HINT NODE_VERSION = 1;

// version code for level db.
static constexpr HINT LDB_VERSION = 1;


// version code for laikelib
static constexpr HINT LK_VERSION = 1;

// version code for compile
static constexpr HINT LK_CP_VERSION = 1;

// version code for protocol
static constexpr HINT PTL_VERSION = 1;

// version code for block
static constexpr HINT BLOCK_VERSION = 1;



// version code for wallet
static constexpr HINT WALLET_VERSION = 1;


static constexpr HN64 LKT_COIN = 100000000;


static constexpr HN64 MAX_TRAN_LKT = 1000000 * LKT_COIN;


static constexpr HN64 MIN_LKC_FEE = 0.0001 * LKT_COIN;


static constexpr HN64 MAX_LKC_FEE = 10 * LKT_COIN;


static constexpr HINT MAX_ENTRY_COUNT = 10;


static constexpr HINT BLOCK_ENTRY_NUM = 10;


typedef enum {
    BT_MAIN,

    BT_END
} BLOCK_TYPE_FLAG;

#endif //__IFNAMES_H__


