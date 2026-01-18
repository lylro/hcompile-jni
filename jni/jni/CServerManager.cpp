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
	"Wiki Online | YELLOW",
	"Wiki Online | TEST"

};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("188.127.241.8", 1, 16, 1633, true), // указывать тут
	CServerInstance::create("188.127.241.8", 1, 16, 1633, true) // и тут тоже что сверху
};