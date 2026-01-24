// -- -- -- -- -- -- -- 
// by borypro
// -- -- -- -- -- -- --
#include "CNetwork.h"
#include <stdint.h>

#include "util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"LOTUS MOBILE | RED",
	"LOTUS MOBILE | TEST"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create("51.75.232.64", 1, 16, 1185, false),
	CSetServer::create("51.75.232.64", 1, 16, 1185, false)	
};