#pragma once

#define MAX_SETTINGS_STRING	0x7F

struct stSettings
{
	// univ / @wow_edgar / client and port to connect
	char host[20]; // ip
	int port; // port

};

class CSettings
{
public:
	CSettings();
	~CSettings();

	void ToDefaults(int iCategory);

	void Save(int iIgnoreCategory = 0);

	const stSettings& GetReadOnly();
	stSettings& GetWrite();
	void LoadSettings();
private:
	stSettings m_Settings;
};