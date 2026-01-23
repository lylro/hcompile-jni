#include "../main.h"
#include "CNetwork.h"
#include <stdint.h>

#include "../util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"HISANNO RP",
	"HISANNO RP"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create(OBFUSCATE("your ip"), 14, 18, port, 1),
	CSetServer::create(OBFUSCATE("your ip"), 1, 16, port, 1)
};