#include "CServerManager.h"
#include <stdint.h>
#include <cstring>

const char* g_szServerNames[MAX_SERVERS] = {
	"Brilliant RP | Test",
	"Name RP | 01"
};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("185.207.214.14", 1, strlen("185.207.214.14"), 3721, true),
	CServerInstance::create("185.207.214.14", 1, strlen("185.207.214.14"), 3721, true)
};