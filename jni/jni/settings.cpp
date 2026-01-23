#include "main.h"
#include "settings.h"
#include "game/game.h"
#include "vendor/ini/config.h"
#include "game/CAdjustableHudColors.h"
#include "game/CAdjustableHudPosition.h"
#include "game/CAdjustableHudScale.h"

static void ClearBackslashN(char* pStr, size_t size)
{
	for (size_t i = 0; i < size; i++)
	{
		if (pStr[i] == '\n' || pStr[i] == 13)
		{
			pStr[i] = 0;
		}
	}
}

CSettings::CSettings()
{
	LoadSettings(nullptr);
}

CSettings::~CSettings()
{
}

void CSettings::ToDefaults(int iCategory)
{
	char buff[0x7F];
	sprintf(buff, OBFUSCATE("%sSAMP/settings.ini"), g_pszStorage);

	FILE* pFile = fopen(buff, "w");

	fwrite("[gui]", 1, 6, pFile);

	fclose(pFile);

	Save(iCategory);
	LoadSettings(m_Settings.szNickName, m_Settings.iChatMaxMessages);
}

void CSettings::Save(int iIgnoreCategory)
{
	char buff[0x7F];
	sprintf(buff, OBFUSCATE("%sSAMP/settings.ini"), g_pszStorage);
	remove(buff);

	ini_table_s* config = ini_table_create();
	
	ini_table_create_entry(config, "client", "name", m_Settings.szNickName);
	ini_table_create_entry(config, "client", "password", m_Settings.szPassword);
	ini_table_create_entry_as_int(config, "client", "last_server", m_Settings.last_server);
	ini_table_create_entry(config, "gui", "Font", m_Settings.szFont);

	ini_table_write_to_file(config, buff);
	ini_table_destroy(config);
}

const stSettings& CSettings::GetReadOnly()
{
	return m_Settings;
}

stSettings& CSettings::GetWrite()
{
	return m_Settings;
}

void CSettings::LoadSettings(const char* szNickName, int iChatLines)
{
	char tempNick[40];
	if (szNickName)
	{
		strcpy(tempNick, szNickName);
	}

	char buff[0x7F];
	sprintf(buff, OBFUSCATE("%sSAMP/settings.ini"), g_pszStorage);

	ini_table_s* config = ini_table_create();
	if (!ini_table_read_from_file(config, buff))
	{
		Log(OBFUSCATE("Cannot load settings, exiting..."));
		std::terminate();
		return;
	}

	snprintf(m_Settings.szNickName, sizeof(m_Settings.szNickName), "__android_%d%d", rand() % 1000, rand() % 1000);
	memset(m_Settings.szPassword, 0, sizeof(m_Settings.szPassword));
	snprintf(m_Settings.szFont, sizeof(m_Settings.szFont), "Arial.ttf");

	const char* szName = ini_table_get_entry(config, "client", "name");
	const char* szPassword = ini_table_get_entry(config, "client", "password");

	const char* szFontName = ini_table_get_entry(config, "gui", "Font");

	if (szName)
	{
		strcpy(m_Settings.szNickName, szName);
	}
	if (szPassword)
	{
		strcpy(m_Settings.szPassword, szPassword);
	}
	if (szFontName)
	{
		strcpy(m_Settings.szFont, szFontName);
	}

	ClearBackslashN(m_Settings.szNickName, sizeof(m_Settings.szNickName));
	ClearBackslashN(m_Settings.szPassword, sizeof(m_Settings.szPassword));
	ClearBackslashN(m_Settings.szFont, sizeof(m_Settings.szFont));

	if (szNickName)
	{
		strcpy(m_Settings.szNickName, tempNick);
	}

	m_Settings.fFontSize = 32.0f;
	m_Settings.iFontOutline = 1;

	m_Settings.fChatPosX = 540.0f;
	m_Settings.fChatPosY = 65.0f;
	m_Settings.fChatSizeX = 1150.0f;
	m_Settings.fChatSizeY = 250.0f;
	m_Settings.iChatMaxMessages = 8;

	m_Settings.fHealthBarWidth = 60.0f;
	m_Settings.fHealthBarHeight = 10.0f;

	m_Settings.fButtonEnterPassengerSize = 150.0f;
	m_Settings.fButtonEnterPassengerX = 50.0f;
	m_Settings.fButtonEnterPassengerY = 420.0f;

	m_Settings.fButtonMicrophoneSize = 0.0f;
	m_Settings.fButtonMicrophoneX = 1067.0f;
	m_Settings.fButtonMicrophoneY = 260.0f;

	m_Settings.fButtonCameraCycleX = 1807.0f;
	m_Settings.fButtonCameraCycleY = 260.0f;
	m_Settings.fButtonCameraCycleSize = 90.0f;

	m_Settings.fScoreBoardSizeX = 0.0f;
	m_Settings.fScoreBoardSizeY = 0.0f;

	m_Settings.iVoiceList = 1;

	m_Settings.iFPS = 120;
	m_Settings.last_server = 0;

	m_Settings.iCutout = 0;
	m_Settings.iAndroidKeyboard = 0;

	m_Settings.iFPSCounter = 0;
	m_Settings.iPCMoney = 0;
	m_Settings.iSkyBox = 1;
	m_Settings.iSnow = 0;
	m_Settings.iHud = 1;
	m_Settings.iDialog = 1;

	ini_table_destroy(config);
}
