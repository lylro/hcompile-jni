//
// Created by weikton on 24.04.2025.
//
#include "main.h"
#include "CServerManager.h"

const char* g_szServerNames[] = {
        weikton("[1] BRILLIANT RP (Тестовый сервер)"),
        weikton("[2] BRILLIANT RP (Production)")
};
constexpr size_t MAX_SERVERS = sizeof(g_szServerNames)
        / sizeof(g_szServerNames[0]);

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
        // CServerInstance::create(weikton("80.242.59.112"), 1, 11, 1725, true)
     CServerInstance::create(weikton("80.242.59.112"), 1, 11, 1725, true),
     CServerInstance::create(weikton("80.242.59.112"), 1, 11, 1725, true)
};