#include "CServerManager.h"

#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"Минскиновка | 01",
	"Brilliant Rp | Cullinan"

};

// указывать 2 раза!!

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("185.207.214.14", 1, 16, 3409, false), // 1
	CServerInstance::create("185.207.214.14", 1, 16, 3409, false) // 2
};