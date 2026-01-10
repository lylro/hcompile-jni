#include "CServerManager.h"
#include "dialog.h"

#include "main.h"

extern CDialogWindow *pDialogWindow;

#ifdef FLIN
	const char* g_szServerNames[MAX_SERVERS] = {
		OBFUSCATE("{ff9729}[1]{ffffff} MOSKOW | Server: 01"),
		OBFUSCATE("{ff9729}[1]{ffffff} TEST | Server: 02")
	};

	const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
		CServerInstance::create(OBFUSCATE("188.127.241.74"), 1, 20, 2793, false), // 1
		CServerInstance::create(OBFUSCATE("188.127.241.74"), 1, 20, 4507, false), // 2

	};
#endif

void CServerInstance::ShowDialog()
{
/*	pDialogWindow->Clear();

	pDialogWindow->m_wDialogID = 0xFFF7;
	pDialogWindow->m_byteDialogStyle = DIALOG_STYLE_LIST;

	cp1251_to_utf8(pDialogWindow->m_utf8Title, "Выбор сервера");
	cp1251_to_utf8(pDialogWindow->m_utf8Button1, "Выбрать");
	cp1251_to_utf8(pDialogWindow->m_utf8Button2, "");

	char szBuff[400];
	memset(szBuff, 0, sizeof(szBuff));
	for (auto & g_szServerName : g_szServerNames)
	{
		strcat(&szBuff[0], g_szServerName);
		strcat(&szBuff[0], OBFUSCATE("\n"));
	}

	pDialogWindow->SetInfo(szBuff, strlen(szBuff));
	pDialogWindow->Show(true);*/
}