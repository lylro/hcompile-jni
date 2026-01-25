#include "../main.h"
#include "CNetwork.h"
#include <stdint.h>

#include "../util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"by viski // by viski // by viski",
	"by viski // by viski // by viski"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create(OBFUSCATE("51.75.232.64"), 14, 18, 1185, 1),
	CSetServer::create(OBFUSCATE("51.75.232.64"), 1, 16, 1185, 1)
};