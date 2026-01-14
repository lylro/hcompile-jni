#include "../main.h"
#include "CNetwork.h"
#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"FUSS RUSSIA | LIMGRAD",
	"FUSS RUSSIA | TEST"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create(OBFUSCATE("188.127.241.74"), 1, 16, 6314, 1), // 0
	CSetServer::create(OBFUSCATE("194.93.2.226"), 1, 16, 5276, 1) // 1
};