#include "CServerManager.h"
//
// УКАЗЫВАТЬ ХОСТ В SERVERS.JSON
//
//
// УКАЗЫВАТЬ ХОСТ В SERVERS.JSON
//
//
// УКАЗЫВАТЬ ХОСТ В SERVERS.JSON
//
//
// УКАЗЫВАТЬ ХОСТ В SERVERS.JSON
//
//
// УКАЗЫВАТЬ ХОСТ В SERVERS.JSON
//
//
// УКАЗЫВАТЬ ХОСТ В SERVERS.JSON
//
//
// УКАЗЫВАТЬ ХОСТ В SERVERS.JSON
//
//
// УКАЗЫВАТЬ ХОСТ В SERVERS.JSON
//
//
// УКАЗЫВАТЬ ХОСТ В SERVERS.JSON
//
//
// УКАЗЫВАТЬ ХОСТ В SERVERS.JSON
//
//
// УКАЗЫВАТЬ ХОСТ В SERVERS.JSON
//
//
// УКАЗЫВАТЬ ХОСТ В SERVERS.JSON
//
//
// УКАЗЫВАТЬ ХОСТ В SERVERS.JSON
//
//
// УКАЗЫВАТЬ ХОСТ В SERVERS.JSON
//

#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"Brilliant RP | Weiktonovka",
	"Brilliant RP | Cullinan"

};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("5.39.108.50", 1, 16, 1125, true), // указывать тут
	CServerInstance::create("5.39.108.50", 1, 16, 1125, true) // и тут тоже что сверху
};