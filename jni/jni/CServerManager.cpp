#include "CServerManager.h"

#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"{ff9729}[1]{ffffff} OUR RUSSIA",
};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	// �������
	//CServerInstance::create("141.95.234.21", 1, 20, 1573, false), // 2 dev
	//CServerInstance::create("141.95.234.21", 1, 20, 1573, false), // 1
	CServerInstance::create("141.95.234.21", 1, 20, 1573, false), // 2 dev
	//CServerInstance::create("141.95.234.21", 1, 14, 1573, false) // 1  Айпи СВОЙ IP
	CServerInstance::create("141.95.234.21", 1, 20, :1573, false)
};
