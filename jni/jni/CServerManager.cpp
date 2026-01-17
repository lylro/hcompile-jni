#include "CServerManager.h"

#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"Harsh Russia | 01",
	"Harsh Russia"

};

// указывать 2 раза!!

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("164.132.219.35", 1, 16, 2209, false), // 1
	CServerInstance::create("164.132.219.35", 1, 16, 2209, false) // 2
};