#include "CNetwork.h"
#include <stdint.h>

#include "util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"Cerlonez RP | sapphire",
	"Cerlonez RP | sapphire"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create("141.95.190.145", 1, 16, 1117, false),
	CSetServer::create("141.95.190.145", 1, 16, 1117, false),	
};