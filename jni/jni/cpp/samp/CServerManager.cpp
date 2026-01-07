#include "CServerManager.h"

#include <stdint.h>

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("141.94.184.111", 1, 20, 1717, false),	// основа
                  CServerInstance::create("141.94.184.111", 1, 20, 1717, false),	// release
	CServerInstance::create("141.94.184.111", 1, 20, 1717, false)
};

