#include "../main.h"
#include "CNetwork.h"
#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"SPARK RUSSIA | RED",
	"SPARK RUSSIA | BLUE"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create(OBFUSCATE("51.75.232.71"), 1, 16, 8899, 1), // 0
	CSetServer::create(OBFUSCATE("51.75.232.71"), 1, 16, 8899, 1) // 1
};