#include "CServerManager.h"

#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"Brilliant RP | Phantom",
	"Brilliant RP | Cullinan"

};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("185.207.214.14", 1, 16, 5073, false), // 1
	CServerInstance::create("185.207.214.14", 1, 16, 5073, false) // 2
};