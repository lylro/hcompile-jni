#include "main.h"
#include "settings.h"
#include "vendor/ini/config.h"

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
	LoadSettings();
}

CSettings::~CSettings()
{
}

void CSettings::ToDefaults(int iCategory)
{
	char buff[0x7F];
	sprintf(buff, "%sSAMP/edgar.ini", "/storage/emulated/0/Android/data/blackrussia.online/files/");

	FILE* pFile = fopen(buff, "w");

	fwrite("[gui]", 1, 6, pFile);

	fclose(pFile);

	Save(iCategory);
	LoadSettings();
}

void CSettings::Save(int iIgnoreCategory)
{
	char buff[0x7F];
    sprintf(buff, "%sSAMP/edgar.ini", "/storage/emulated/0/Android/data/blackrussia.online/files/");
	remove(buff);

	ini_table_s* config = ini_table_create();
	// create host and port with settings.ini
	ini_table_create_entry(config, "settings", "host", m_Settings.host); // host
	ini_table_create_entry_as_int(config, "settings", "port", m_Settings.port); // port

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

void CSettings::LoadSettings()
{
	LOG("Loading settings..");

	char buff[0x7F];
    sprintf(buff, "%sSAMP/edgar.ini", "/storage/emulated/0/Android/data/blackrussia.online/files/");

	ini_table_s* config = ini_table_create();
	LOG("Opening settings: %s", buff);
	if (!ini_table_read_from_file(config, buff))
	{
		LOG("Cannot load settings, exiting...");
		std::terminate();
		return;
	}

	// load host and port
	const char* host = ini_table_get_entry(config, "settings", "host"); // host
	if (host) strcpy(m_Settings.host, host);
	m_Settings.port = ini_table_get_entry_as_int(config, "settings", "port", -1); // port

	ini_table_destroy(config);
}
