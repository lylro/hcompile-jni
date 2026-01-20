#include "CNetwork.h"
#include <stdint.h>

#include "util/CJavaWrapper.h"

const char* g_szServerNames[MAX_SERVERS] = {
	"Vesoma | House",
	"Vesoma  | House"
};

const CSetServer::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CSetServer::create("80.242.59.112", 1, 16, 4001, false),
	CSetServer::create("80.242.59.112", 1, 16, 4001, false)	
};