#include "../main.h"
#include "CNetwork.h"
#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"FUSS RUSSIA | RED",
	"FUSS RUSSIA | TEST"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create(OBFUSCATE("80.242.59.112"), 1, 16, 1901, 1), // 0
	CSetServer::create(OBFUSCATE("80.242.59.112"), 1, 16, 1901, 1) // 1
};