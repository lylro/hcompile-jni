#include "CServerManager.h"

#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"Harsh Russia | 01",
	"Harsh Russia"

};

// указывать 2 раза!!

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("51.75.232.69", 1, 16, 1185, false), // 1
	CServerInstance::create("51.75.232.69", 1, 16, 1185, false) // 2
};