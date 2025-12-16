#include "CServerManager.h"

#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"GRIEF RUSSIA | Moscow",
};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	//        
	//CServerInstance::create("51.75.232.64", 1, 20, 1453, false), // 2 dev
	//CServerInstance::create("51.75.232.64", 1, 20, 1453, false), // 1
	CServerInstance::create("193.23.220.249", 1, 20, 1165, false), // 2 dev
	//CServerInstance::create("51.75.232.64", 1, 14, 1453, false) // 1
	CServerInstance::create("193.23.220.249", 1, 20, 1165, false)
};
