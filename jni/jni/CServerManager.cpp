#include "CServerManager.h"

#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"skydust | 01",
	"skydust RP | 01"

};

// указывать 2 раза!!

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("188.127.241.8", 1, 16, 1446, false), // 1
	CServerInstance::create("188.127.241.8", 1, 16, 1446, false) // 2
};