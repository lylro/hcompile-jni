//
// Created by weikton on 24.04.2025.
//
#include "main.h"
#include "CServerManager.h"

const char* g_szServerNames[] = {
        weikton("[1] ABOBA RP (MOSCOW)"),
        weikton("[2] ABOBA RP (VERHATURIE)")
};
constexpr size_t MAX_SERVERS = sizeof(g_szServerNames)
        / sizeof(g_szServerNames[0]);

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
        // CServerInstance::create(weikton("хост"), 1, колво строк в хосте, порт, войс)
     CServerInstance::create(weikton("185.189.255.97"), 1, 15, 3086, true),
     CServerInstance::create(weikton("185.189.255.97"), 1, 15, 3086, true)
};