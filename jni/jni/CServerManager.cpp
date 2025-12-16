#include "CServerManager.h"

#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"Moscow",
	"Краснодар"

};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("193.23.220.249", 1, 16, 1165, true), // указывать тут
	CServerInstance::create("193.23.220.249", 1, 16, 1165, true) // и тут тоже что сверху
};