// -- -- -- -- -- -- -- 
// by ГРОМ 2.0 X Barsikkk
// -- -- -- -- -- -- --
#include "CNetwork.h"
#include <stdint.h>

#include "util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"БЕТА БЕЛАРУСЬ | СОЛИГОРСК",
	"БЕТА БЕЛАРУСЬ | ТЕСТ"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create("188.127.241.8", 1, 16, 1284, false),
	CSetServer::create("188.127.241.8", 1, 16, 1537, false)	
};