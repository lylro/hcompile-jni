//rick was here
#include "CNetwork.h"
#include <stdint.h>

#include "util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"RED",
	"TEST"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create("195.18.27.241", 1, 16, 2796, false),
	CSetServer::create("195.18.27.241", 1, 16, 2796, false)	
};