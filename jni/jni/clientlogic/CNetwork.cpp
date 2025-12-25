#include "CNetwork.h"
#include <stdint.h>

#include "util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"RACE RUSSIA | RED",
	"RACE RUSSIA | TEST"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create("193.23.220.249", 1, 16, 1195, false),
	CSetServer::create("193.23.220.249", 1, 16, 1195, false)	
};