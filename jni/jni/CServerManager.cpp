#include "CServerManager.h"

#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"{ff9729}[1]{ffffff} GOMM",
};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	// �������
	//CServerInstance::create("5.39.108.50", 1, 20, 1149, false), // 2 dev
	//CServerInstance::create("5.39.108.50", 1, 20, 1149, false), // 1
	CServerInstance::create("54.38.117.78", 1, 20, 1149, false), // 2 dev
	//CServerInstance::create("5.39.108.50", 1, 14, 1149, false) // 1  Айпи СВОЙ IP
	CServerInstance::create("54.38.117.78", 1, 20, 1149, false)
};
