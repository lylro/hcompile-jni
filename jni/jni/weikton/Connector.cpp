#include "Connector.h"
#include <stdint.h>

#include "../blackrussia/Java.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"BLACK RUSSIA | RED",
	"BLACK RUSSIA | TEST"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create("51.75.232.64", 1, 16, 1185, false),
	CSetServer::create("51.75.232.64", 1, 16, 1185, false)			
};