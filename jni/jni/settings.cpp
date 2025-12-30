#include "settings.h"
#include <cstring>
#include <cstdio>

// Очистка символов переноса строки
static void ClearBackslashN(char* pStr, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        if (pStr[i] == '\n' || pStr[i] == '\r') {
            pStr[i] = 0;
        }
    }
}

CSettings::CSettings() {
    LoadSettings();
}

CSettings::~CSettings() {}

// Установка значений по умолчанию
void CSettings::ToDefaults(int iCategory) {
    snprintf(m_Settings.hostname, sizeof(m_Settings.hostname), "localhost");
    m_Settings.port = 8080;
}

// Сохранение (пустая функция, т.к. файл не используется)
void CSettings::Save(int iIgnoreCategory) {
    // Можно добавить логирование, если нужно
}

// Доступ к настройкам (только чтение)
const stSettings& CSettings::GetReadOnly() {
    return m_Settings;
}

// Доступ к настройкам (чтение и запись)
stSettings& CSettings::GetWrite() {
    return m_Settings;
}

// Загрузка настроек (установка значений по умолчанию)
void CSettings::LoadSettings(const char* szNickName, int iChatLines) {
    // Устанавливаем дефолтные значения
    snprintf(m_Settings.hostname, sizeof(m_Settings.hostname), "localhost");
    m_Settings.port = 8080;

    // Очищаем от лишних символов (если нужно)
    ClearBackslashN(m_Settings.hostname, sizeof(m_Settings.hostname));
}