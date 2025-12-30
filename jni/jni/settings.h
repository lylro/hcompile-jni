#ifndef SETTINGS_H
#define SETTINGS_H

// Структура с настройками
struct stSettings {
    char hostname[256];  // Имя хоста
    int port;            // Порт
};

class CSettings {
public:
    CSettings();
    ~CSettings();

    // Установка настроек по умолчанию
    void ToDefaults(int iCategory = 0);
    
    // Сохранение настроек
    void Save(int iIgnoreCategory = 0);
    
    // Загрузка настроек
    void LoadSettings(const char* szNickName = "", int iChatLines = 8);
    
    // Получение настроек (только для чтения)
    const stSettings& GetReadOnly();
    
    // Получение настроек (для записи)
    stSettings& GetWrite();

private:
    stSettings m_Settings; // Основная структура с настройками
};

#endif // SETTINGS_H