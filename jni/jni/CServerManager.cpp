//
// Created by weikton on 24.04.2025.
//
#include "main.h"
#include "CServerManager.h"

const char* g_szServerNames[] = {
        weikton("[1] BRUKEN RUSSIA (Москва)"),
        weikton("[2] BRUKEN RUSSIA (Махачкала | Скоро)")
        weikton("[3] BRUKEN RUSSIA (Чечня | Скоро)")
        weikton("[4] BRUKEN RUSSIA (Тестовый)")
};
constexpr size_t MAX_SERVERS = sizeof(g_szServerNames)
        / sizeof(g_szServerNames[0]);

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
        // CServerInstance::create(weikton("хост"), 1, колво строк в хосте, порт, войс)
     CServerInstance::create(weikton("188.127.241.74"), 1, 14, 2793, true),
     CServerInstance::create(weikton("188.127.241.74"), 1, 14, 5562, true)
     CServerInstance::create(weikton("188.127.241.74"), 1, 14, 5562, true)
     CServerInstance::create(weikton("188.127.241.74"), 1, 14, 5562, true)
};