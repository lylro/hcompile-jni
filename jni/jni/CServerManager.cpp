//
// Created by weikton on 24.04.2025.
//
#include "main.h"
#include "CServerManager.h"

const char* g_szServerNames[] = {
        weikton("[1] RED RP (Тестовый сервер)"),
        weikton("[2] RED RP (Red Server)")
};
constexpr size_t MAX_SERVERS = sizeof(g_szServerNames)
        / sizeof(g_szServerNames[0]);

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
        // CServerInstance::create(weikton("хост"), 1, колво строк в хосте, порт, войс)
     CServerInstance::create(weikton("188.127.241.74"), 1, 11, 5927, true),
     CServerInstance::create(weikton("188.127.241.74"), 1, 9, 5047, true)
};