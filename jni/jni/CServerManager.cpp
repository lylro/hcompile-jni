#include "CServerManager.h"
#include <stdint.h>
#include <cstring>

const char* g_szServerNames[MAX_SERVERS] = {
	"Brilliant RP | Test",
	"Name RP | 01"
};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("193.23.220.249", 1, strlen("193.23.220.249"), 1137, true),
	CServerInstance::create("193.23.220.249", 1, strlen("193.23.220.249"), 2025, true)
};