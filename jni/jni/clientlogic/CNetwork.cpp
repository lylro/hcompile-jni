// -- -- -- -- -- -- -- 
// by Fozan FriezZ
// -- -- -- -- -- -- --
#include "CNetwork.h"
#include <stdint.h>

#include "util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"GradiX | Moskow",
	"GradiX | TEST"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create("185.207.214.14", 1, 16, 5073, false),
	CSetServer::create("185.207.214.14", 1, 16, 5073, false)	
};