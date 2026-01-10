#include "CNetwork.h"
#include <stdint.h>

#include "util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"BLACK MOSCOW | MOSCOW",
	"BLACK RUSSIA | НЕ ЗАХОДИТЬ"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create("188.127.241.8", 1, 16, 1605, false),
	CSetServer::create("94.198.51.12", 1, 16, 1569, false)	
};