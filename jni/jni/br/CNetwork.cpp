#include "../main.h"
#include "CNetwork.h"
#include <stdint.h>

#include "../util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"FUSS RUSSIA | RED",
	"FUSS RUSSIA | TEST"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create(OBFUSCATE("188.127.241.8"), 14, 18, 1550, 1), // 0
	CSetServer::create(OBFUSCATE("188.127.241.8"), 14, 18, 1550, 1) // 1
};