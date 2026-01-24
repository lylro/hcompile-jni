#include "main.h"
#include "gui/gui.h"
#include "chatwindow.h"
#include "keyboard.h"
#include "settings.h"
#include "game/game.h"
#include "net/netgame.h"
#include "vendor/bass/bass.h"
#include <dlfcn.h>
#include "scrollbar.h"
#include "voice/CVoiceChatClient.h"

extern CGUI *pGUI;
extern CKeyBoard *pKeyBoard;
extern CSettings *pSettings;
extern CNetGame *pNetGame;
extern CAMERA_AIM * pcaInternalAim;
extern CGame * pGame;
extern CVoiceChatClient* pVoice;
extern CChatWindow* pChatWindow;

bool g_bShowVoiceList = false;

#define NUM_OF_MESSAGES	100

template<typename T> T GetBASSFunc(void* handle, const char* szFunc)
{
	return (T)dlsym(handle, szFunc);
}

BOOL(*BASS_Init_func)(int device, DWORD freq, DWORD flags, void* win, void* dsguid);
HSTREAM(*BASS_StreamCreateURL_func)(const char* url, DWORD offset, DWORD flags, DOWNLOADPROC* proc, void* user);
BOOL(*BASS_ChannelPlay_func)(DWORD handlee, BOOL restart);
int (*BASS_ErrorGetCode_func)();
BOOL(*BASS_ChannelSet3DPosition_func)(DWORD handlee, const BASS_3DVECTOR* pos, const BASS_3DVECTOR* orient, const BASS_3DVECTOR* vel);
BOOL(*BASS_StreamFree_func)(HSTREAM handlee);
void (*BASS_Apply3D_func)();
BOOL(*BASS_Set3DPosition_func)(const BASS_3DVECTOR* pos, const BASS_3DVECTOR* vel, const BASS_3DVECTOR* front, const BASS_3DVECTOR* top);
BOOL(*BASS_ChannelSet3DAttributes_func)(DWORD handlee, int mode, float min, float max, int iangle, int oangle, float outvol);
BOOL(*BASS_ChannelSetAttribute_func)(DWORD handle, DWORD attrib, float value);
DWORD (*BASS_ChannelFlags_func)(DWORD handle, DWORD flags, DWORD mask);
BOOL (*BASS_ChannelPause_func)(DWORD handle);

BOOL BASS_Init(int device, DWORD freq, DWORD flags, void* win, void* dsguid)
{
	return BASS_Init_func(device, freq, flags, win, dsguid);
}

HSTREAM BASS_StreamCreateURL(const char* url, DWORD offset, DWORD flags, DOWNLOADPROC* proc, void* user)
{
	return BASS_StreamCreateURL_func(url, offset, flags, proc, user);
}

DWORD BASS_ChannelFlags(DWORD handle, DWORD flags, DWORD mask)
{
	return BASS_ChannelFlags_func(handle, flags, mask);
}

BOOL BASS_ChannelPlay(DWORD handlee, BOOL restart)
{
	return BASS_ChannelPlay_func(handlee, restart);
}

int BASS_ErrorGetCode()
{
	return BASS_ErrorGetCode_func();
}

BOOL BASS_ChannelSet3DPosition(DWORD handlee, const BASS_3DVECTOR* pos, const BASS_3DVECTOR* orient, const BASS_3DVECTOR* vel)
{
	return BASS_ChannelSet3DPosition_func(handlee, pos, orient, vel);
}

BOOL BASS_StreamFree(HSTREAM handlee)
{
	return BASS_StreamFree_func(handlee);
}

void BASS_Apply3D()
{
	return BASS_Apply3D_func();
}

BOOL BASS_Set3DPosition(const BASS_3DVECTOR* pos, const BASS_3DVECTOR* vel, const BASS_3DVECTOR* front, const BASS_3DVECTOR* top)
{
	return BASS_Set3DPosition_func(pos, vel, front, top);
}

BOOL BASS_ChannelSet3DAttributes(DWORD handlee, int mode, float min, float max, int iangle, int oangle, float outvol)
{
	return BASS_ChannelSet3DAttributes_func(handlee, mode, min, max, iangle, oangle, outvol);
}

BOOL BASS_ChannelSetAttribute(DWORD handle, DWORD attrib, float value)
{
	return BASS_ChannelSetAttribute_func(handle, attrib, value);
}

BOOL BASS_ChannelPause(DWORD handle)
{
	return BASS_ChannelPause_func(handle);
}

void InitBASSFuncs()
{
	void* handle;

#ifdef GAME_EDITION_CR
	handle = dlopen("/data/data/com.byreytiz.game/lib/libbass.so", 3);
#else
	handle = dlopen("/data/data/com.byreytiz.game/lib/libbass.so", 3);
#endif
	if (!handle)
	{
		Log("Cannot load libbass.so");
		return;
	}

	BASS_Init_func = GetBASSFunc<BOOL(*)(int device, DWORD freq, DWORD flags, void* win, void*)>(handle, "BASS_Init");

	BASS_StreamCreateURL_func = GetBASSFunc<HSTREAM(*)(const char* url, DWORD offset, DWORD flags, DOWNLOADPROC * proc, void* user)>(handle, "BASS_StreamCreateURL");

	BASS_ChannelPlay_func = GetBASSFunc<BOOL(*)(DWORD handlee, BOOL restart)>(handle, "BASS_ChannelPlay");

	BASS_ErrorGetCode_func = GetBASSFunc<int (*)() >(handle, "BASS_ErrorGetCode");

	BASS_ChannelSet3DPosition_func = GetBASSFunc< BOOL(*)(DWORD handlee, const BASS_3DVECTOR * pos, const BASS_3DVECTOR * orient, const BASS_3DVECTOR * vel)>(handle, "BASS_ChannelSet3DPosition");

	BASS_StreamFree_func = GetBASSFunc<BOOL(*)(HSTREAM handlee)>(handle, "BASS_StreamFree");

	BASS_Apply3D_func = GetBASSFunc<void (*)()>(handle, "BASS_Apply3D");

	BASS_Set3DPosition_func = GetBASSFunc<BOOL(*)(const BASS_3DVECTOR * pos, const BASS_3DVECTOR * vel, const BASS_3DVECTOR * front, const BASS_3DVECTOR * top)>(handle, "BASS_Set3DPosition");

	BASS_ChannelSet3DAttributes_func = GetBASSFunc<BOOL(*)(DWORD handlee, int mode, float min, float max, int iangle, int oangle, float outvol)>(handle, "BASS_ChannelSet3DAttributes");

	BASS_ChannelSetAttribute_func = GetBASSFunc<BOOL(*)(DWORD handle, DWORD attrib, float value)>(handle, "BASS_ChannelSetAttribute");

	BASS_ChannelFlags_func = GetBASSFunc< DWORD(*)(DWORD handle, DWORD flags, DWORD mask)>(handle, "BASS_ChannelFlags");

	BASS_ChannelPause_func = GetBASSFunc<BOOL(*)(DWORD handle)>(handle, "BASS_ChannelPause");
}

BOOL returnedValue;
#include "util/CJavaWrapper.h"

//Повороты головой как на ПК с синхранизацией
#ifdef GAME_EDITION_CR
uint32_t g_uiHeadMoveEnabled = 0;
#else
uint32_t g_uiHeadMoveEnabled = 1;
#endif

uint32_t g_uiBorderedText = 1;
#include "debug.h"
#include "clientlogic/ChatMessenger.h"
#include "scoreboard.h"
#include "game/CCustomPlateManager.h"
#include <fcntl.h>

extern CScoreBoard* pScoreBoard;
bool ProcessVoiceCommands(const char* str)
{
       	if (strstr(str, "/tab"))
	{
		//by kalcor x weikton
		pScoreBoard->Toggle();
		return true;
	}

	if (strstr(str, "/hud"))
	{
		g_pJavaWrapper->ShowClientSettings();
		return true;
	}
	
	if (strstr(str, "/client"))
	{
		//g_pJavaWrapper->ShowSettings();
		return true;
	}

	if (strstr(str, "/fpsinfo"))
	{
		CDebugInfo::ToggleDebugDraw();
		return true;
	}

	if (strstr(str, "/pvoice"))
	{
		g_bShowVoiceList ^= 1;
		if (g_bShowVoiceList)
		{
			pChatWindow->AddDebugMessage("                              ");
		}
		else
		{
			pChatWindow->AddDebugMessage("                               ");
		}
		return true;
	}


	if (strstr(str, "/vdisconnect"))
	{
		delete (char*)(g_libGTASA + 0x10);
		if (pVoice) pVoice->FullDisconnect();
		return true;
	}
	if (strstr(str, "/vconnect"))
	{
		if (pVoice) pVoice->SetNetworkState(VOICECHAT_WAIT_CONNECT);
		return true;
	}
	if (strstr(str, "/vmute"))
	{
		while (*str)
		{
			if (*str == ' ')
			{
				str++;
				break;
			}
			str++;
		}
		int id = 0;
		if (sscanf(str, "%d", &id) == -1)
		{
			pChatWindow->AddDebugMessage("         : /vmute [playerid]");
			return true;
		}
		if (id < 0 || id > 1000)
		{
			pChatWindow->AddDebugMessage("         : ID                           0    1000");
			return true;
		}
		pVoice->MutePlayer(id);
		pChatWindow->AddDebugMessage("                        : %d", id);
		return true;
	}

	if (strstr(str, "/vunmute"))
	{
		while (*str)
		{
			if (*str == ' ')
			{
				str++;
				break;
			}
			str++;
		}
		int id = 0;
		if (sscanf(str, "%d", &id) == -1)
		{
			pChatWindow->AddDebugMessage("         : /vunmute [playerid]");
			return true;
		}
		if (id < 0 || id > 1000)
		{
			pChatWindow->AddDebugMessage("         : ID                           0    1000");
			return true;
		}
		pVoice->UnMutePlayer(id);
		pChatWindow->AddDebugMessage("                         : %d", id);
		return true;
	}

	if (strstr(str, "/voicevolume"))
	{
		while (*str)
		{
			if (*str == ' ')
			{
				str++;
				break;
			}
			str++;
		}
		int volume = 0;
		if (sscanf(str, "%d", &volume) == -1)
		{
			pChatWindow->AddDebugMessage("         : /voicevolume [value]");
			return true;
		}
		pChatWindow->AddDebugMessage("           :                       %d", volume);
		pVoice->SetVolume(volume);
		return true;
	}

	if (strstr(str, "/vplayer"))
	{
		while (*str)
		{
			if (*str == ' ')
			{
				str++;
				break;
			}
			str++;
		}
		int id = 0;
		int volume = 0;
		if (sscanf(str, "%d %d", &id, &volume) == -1)
		{
			pChatWindow->AddDebugMessage("         : /vplayer [playerid] [volume]");
			return true;
		}
		if (id < 0 || id > 1000)
		{
			pChatWindow->AddDebugMessage("         : ID                           0    1000");
			return true;
		}
		pVoice->SetVolumePlayer(id, volume);
		pChatWindow->AddDebugMessage("                      %d            %d", volume, id);
		return true;
	}
	return false;
}

void ChatWindowInputHandler(const char* str)
{
	if(!str || *str == '\0') return;
	if(!pNetGame) return;

	if (*str == '/')
	{
		if (ProcessVoiceCommands(str))
		{
			return;
		}
	}

	auto pCmd = pChatWindow->bufferedChat.WriteLock();

	if (*str == '/')
	{
		pCmd->type = 0;
	}
	else
	{
		pCmd->type = 1;
	}

	strcpy(pCmd->buff, str);

	pChatWindow->bufferedChat.WriteUnlock();

	return;
}


CChatWindow::CChatWindow()
{
	m_uiMaxTimeChatHide = 0;
	m_uiTimeChatHideBegin = 0;
	pScrollbar = nullptr;

	m_uiLastTimePushedMessage = GetTickCount();
	m_bNewMessage = false;
	m_uiTimePushed = GetTickCount();
	m_bPendingReInit = false;
	ReInit();
}

CChatWindow::~CChatWindow()
{
	if (pScrollbar)
	{
		delete pScrollbar;
		pScrollbar = nullptr;
	}
}

#include <mutex>
static std::mutex lDebugMutex;

#include "dialog.h"
extern CDialogWindow* pDialogWindow;

bool CChatWindow::OnTouchEvent(int type, bool multi, int x, int y)
{
	
	static bool bWannaOpenChat = false;

	switch (type)
	{
	case TOUCH_PUSH:
		if (x >= m_fChatPosX && x <= m_fChatPosX + m_fChatSizeX &&
			y >= m_fChatPosY && y <= m_fChatPosY + m_fChatSizeY)
		{
			bWannaOpenChat = true;
		}
		break;

	case TOUCH_POP:
		if (bWannaOpenChat &&
			x >= m_fChatPosX && x <= m_fChatPosX + m_fChatSizeX &&
			y >= m_fChatPosY && y <= m_fChatPosY + m_fChatSizeY)
		{
			if (pDialogWindow)
			{
				if (pDialogWindow->m_bRendered) return true;
			}
			if (pKeyBoard)
			{
				pKeyBoard->Open(&ChatWindowInputHandler);
			}
			m_uiLastTimePushedMessage = GetTickCount();
		}
		bWannaOpenChat = false;
		break;

	case TOUCH_MOVE:
		break;
	}

	return true;
}

void CChatWindow::Render()
{
	if (m_bPendingReInit)
	{
		ReInit();
		m_bPendingReInit = false;
	}

	if (!pGame->IsToggledHUDElement(HUD_ELEMENT_CHAT)) return;
	if (pScrollbar)
	{
		pScrollbar->m_bDrawState = false;
		if (pKeyBoard && pDialogWindow)
		{
			if (pKeyBoard->IsOpen() && !pDialogWindow->m_bRendered)
			{
				pScrollbar->m_bDrawState = true;
			}
		}
		pScrollbar->Draw();
	}
	if (false)
	{
		ImGui::GetOverlayDrawList()->AddRect(
			ImVec2(m_fChatPosX, m_fChatPosY),
			ImVec2(m_fChatPosX + m_fChatSizeX, m_fChatPosY + m_fChatSizeY),
			IM_COL32_BLACK);
	}

	//ImVec2 pos = ImVec2(m_fChatPosX, m_fChatPosY + m_fChatSizeY);
	if (!pScrollbar)
	{
		if (pScrollbar->GetValue() < 0 || pScrollbar->GetValue() > NUM_OF_MESSAGES + m_iMaxMessages)
		{
			return;
		}
		return;
	}

	if (pKeyBoard)
	{
		if (pKeyBoard->IsOpen())
		{
			m_uiLastTimePushedMessage = GetTickCount();
		}
	}

	uint32_t currentOffset = 1;

	bool firstMessageAppear = false;

	for (int i = 100 - pScrollbar->GetValue(); i < 100 - pScrollbar->GetValue() + m_iMaxMessages + 1; i++)
	{
		if (i >= m_vChatWindowEntries.size())
		{
			break;
		}
		if (!m_vChatWindowEntries[i])
		{
			break;
		}

		ImVec2 pos = ImVec2(m_fChatPosX, m_fChatPosY + m_fChatSizeY);
		float fProgressedAlpha = 1.0f;
		float fY_ = pGUI->GetFontSize() * (float)currentOffset;
		if (m_bNewMessage)
		{
			uint32_t timeSpent = GetTickCount() - m_uiTimePushed;
			if (timeSpent >= NEW_MESSAGE_PUSH_TIME)
			{
				m_bNewMessage = false;
				m_uiTimePushed = 0;
			}
			else
			{
				float fProgress = (float)timeSpent / (float)NEW_MESSAGE_PUSH_TIME;
				fProgressedAlpha = fProgress;
				fY_ = ((pGUI->GetFontSize() * fProgress) + fY_ - pGUI->GetFontSize());
			}
		}
		pos.y -= fY_;
		currentOffset++;
		if (firstMessageAppear)
		{
			fProgressedAlpha = 1.0f;
		}
		if (!firstMessageAppear)
		{
			firstMessageAppear = true;
		}

		auto entry = m_vChatWindowEntries[i];
		switch (entry->eType)
		{
		case CHAT_TYPE_CHAT:
			if (entry->szNick[0] != 0)
			{
				RenderText(entry->szNick, pos.x, pos.y, entry->dwNickColor, fProgressedAlpha);
				pos.x += ImGui::CalcTextSize(entry->szNick).x + ImGui::CalcTextSize(" ").x; //+ pGUI->GetFontSize() * 0.4;
			}
			RenderText(entry->utf8Message, pos.x, pos.y, entry->dwTextColor, fProgressedAlpha);
			break;

		case CHAT_TYPE_INFO:
		case CHAT_TYPE_DEBUG:
			RenderText(entry->utf8Message, pos.x, pos.y, entry->dwTextColor, fProgressedAlpha);
			break;
		}

		pos.x = m_fChatPosX;
		pos.y -= pGUI->GetFontSize();
	}

	
}

bool ProcessInlineHexColor(const char* start, const char* end, ImVec4& color)
{
	const int hexCount = (int)(end - start);
	if (hexCount == 6 || hexCount == 8)
	{
		char hex[9];
		strncpy(hex, start, hexCount);
		hex[hexCount] = 0;

		unsigned int hexColor = 0;
		if (sscanf(hex, "%x", &hexColor) > 0)
		{
			color.x = static_cast<float>((hexColor & 0x00FF0000) >> 16) / 255.0f;
			color.y = static_cast<float>((hexColor & 0x0000FF00) >> 8) / 255.0f;
			color.z = static_cast<float>((hexColor & 0x000000FF)) / 255.0f;
			color.w = 1.0f;

			if (hexCount == 8)
				color.w = static_cast<float>((hexColor & 0xFF000000) >> 24) / 255.0f;

			return true;
		}
	}

	return false;
}

void CChatWindow::ReInit()
{
	m_fChatPosX = pGUI->ScaleX(pSettings->GetReadOnly().fChatPosX);
	m_fChatPosY = pGUI->ScaleY(pSettings->GetReadOnly().fChatPosY);
	m_fChatSizeX = pGUI->ScaleX(pSettings->GetReadOnly().fChatSizeX);
	m_fChatSizeY = pGUI->ScaleY(pSettings->GetReadOnly().fChatSizeY);
	m_iMaxMessages = pSettings->GetReadOnly().iChatMaxMessages;

	m_fChatSizeY = m_iMaxMessages * pGUI->GetFontSize();

	m_dwTextColor = 0xFFFFFFFF;
	m_dwInfoColor = 0x00C8C8FF;
	m_dwDebugColor = 0xBEBEBEFF;

	if (!pScrollbar)
	{
		pScrollbar = new CScrollbar();
	}
	pScrollbar->m_fX = m_fChatPosX - pGUI->ScaleX(50.0f);
	pScrollbar->m_fY = m_fChatPosY - pGUI->ScaleY(10.0f);

	pScrollbar->m_fWidthBox = pGUI->ScaleX(30.0f);
	pScrollbar->m_fHeightBox = pGUI->ScaleY(80.0f);

	pScrollbar->m_fUpY = m_fChatPosY - pGUI->ScaleY(10.0f);
	pScrollbar->m_fDownY = m_fChatSizeY + m_fChatPosY + pGUI->ScaleY(20.0f);

	pScrollbar->m_iMaxValue = NUM_OF_MESSAGES;
	pScrollbar->SetOnPos(NUM_OF_MESSAGES);

	m_uiLastTimePushedMessage = GetTickCount();

	m_fPosBeforeBoundChat = m_fChatPosY;
	m_fOffsetBefore = 0.0f;
}

void CChatWindow::RenderText(const char* u8Str, float posX, float posY, uint32_t dwColor, float alphaNewMessage)
{
	const char* textStart = u8Str;
	const char* textCur = u8Str;
	const char* textEnd = u8Str + strlen(u8Str);

	uint8_t bAlpha = GetAlphaFromLastTimePushedMessage();

	ImVec2 posCur = ImVec2(posX, posY);
	ImColor colorCur = ImColor(dwColor);
	if (bAlpha != 255)
	{
		colorCur.Value.w = (float)bAlpha / 255.0f;
	}
	ImVec4 col;

	if (alphaNewMessage != 1.0f)
	{
		colorCur.Value.w = alphaNewMessage;
	}

	while (*textCur)
	{
		// {BBCCDD}
		// '{' e '}' niioaaonoao?o ASCII eiae?iaea
		if (textCur[0] == '{' && ((&textCur[7] < textEnd) && textCur[7] == '}'))
		{
			// Auaiaei oaeno ai oeao?iie neiaee
			if (textCur != textStart)
			{
				// Auaiaei ai oaeouaai neiaiea
				pGUI->RenderTextForChatWindow(posCur, colorCur, true, textStart, textCur);

				// Aun?eouaaai iiaia niauaiea
				posCur.x += ImGui::CalcTextSize(textStart, textCur).x;
			}

			// Iieo?aai oaao
			if (ProcessInlineHexColor(textCur + 1, textCur + 7, col))
			{
				colorCur = col;
				if (bAlpha != 255)
				{
					colorCur.Value.w = (float)bAlpha / 255.0f;
				}
				if (alphaNewMessage != 1.0f)
				{
					colorCur.Value.w = alphaNewMessage;
				}
			}

			// Aaeaaai niauaiea
			textCur += 7;
			textStart = textCur + 1;
		}

		textCur++;
	}

	if (textCur != textStart)
		pGUI->RenderTextForChatWindow(posCur, colorCur, true, textStart, textCur);

	return;
}

void CChatWindow::SetChatDissappearTimeout(uint32_t uiTimeoutStart, uint32_t uiTimeoutEnd)
{
	m_uiMaxTimeChatHide = uiTimeoutEnd;
	m_uiTimeChatHideBegin = uiTimeoutStart;
}

void CChatWindow::ProcessPushedCommands()
{
	BUFFERED_COMMAND_CHAT* pCmd = nullptr;
	while (pCmd = bufferedChat.ReadLock())
	{
		if (pCmd->buff[0] == '/')
		{
			pNetGame->SendChatCommand(pCmd->buff);
		}
		else
		{
			pNetGame->SendChatMessage(pCmd->buff);
		}

		bufferedChat.ReadUnlock();
	}
}

void CChatWindow::AddChatMessage(char* szNick, uint32_t dwNickColor, char* szMessage)
{
	FilterInvalidChars(szMessage);
	AddToChatWindowBuffer(CHAT_TYPE_CHAT, szMessage, szNick, m_dwTextColor, dwNickColor);
}

void CChatWindow::AddInfoMessage(char* szFormat, ...)
{
	char tmp_buf[512];
	memset(tmp_buf, 0, sizeof(tmp_buf));

	va_list args;
	va_start(args, szFormat);
	vsprintf(tmp_buf, szFormat, args);
	va_end(args);

	FilterInvalidChars(tmp_buf);
	AddToChatWindowBuffer(CHAT_TYPE_INFO, tmp_buf, nullptr, m_dwInfoColor, 0);
}

void CChatWindow::AddDebugMessage(char* szFormat, ...)
{
	char tmp_buf[512];
	memset(tmp_buf, 0, sizeof(tmp_buf));

	va_list args;
	va_start(args, szFormat);
	vsprintf(tmp_buf, szFormat, args);
	va_end(args);

	FilterInvalidChars(tmp_buf);
	AddToChatWindowBuffer(CHAT_TYPE_DEBUG, tmp_buf, nullptr, m_dwDebugColor, 0);
}

void CChatWindow::AddDebugMessageNonFormatted(char* szStr)
{
	FilterInvalidChars(szStr);
	AddToChatWindowBuffer(CHAT_TYPE_DEBUG, szStr, nullptr, m_dwDebugColor, 0);
}

void CChatWindow::AddClientMessage(uint32_t dwColor, char* szStr)
{
	FilterInvalidChars(szStr);
	AddToChatWindowBuffer(CHAT_TYPE_INFO, szStr, nullptr, dwColor, 0);
}

void CChatWindow::SetLowerBound(int bound)
{
	ImGuiIO& io = ImGui::GetIO();

	m_fChatPosY = m_fPosBeforeBoundChat;
	pScrollbar->m_fY += m_fOffsetBefore;
	pScrollbar->m_fDownY += m_fOffsetBefore;

	float y = io.DisplaySize.y - (float)bound;

	m_fOffsetBefore = 0.0f;

	while (m_fChatPosY + m_fChatSizeY + pGUI->GetFontSize() * 2.0f >= y)
	{
		m_fChatPosY -= pGUI->GetFontSize();
		m_fOffsetBefore += pGUI->GetFontSize();
	}

	pScrollbar->m_fY -= m_fOffsetBefore;
	pScrollbar->m_fDownY -= m_fOffsetBefore;
}

void CChatWindow::PushBack(CHAT_WINDOW_ENTRY& entry)
{
	if (m_vChatWindowEntries.size() >= NUM_OF_MESSAGES + m_iMaxMessages)
	{
		auto it = m_vChatWindowEntries.back();
		delete it;
		m_vChatWindowEntries.pop_back();
	}

	CHAT_WINDOW_ENTRY* pEntry = new CHAT_WINDOW_ENTRY;
	memcpy(pEntry, &entry, sizeof(CHAT_WINDOW_ENTRY));

	m_bNewMessage = true;
	m_uiTimePushed = GetTickCount();

	m_vChatWindowEntries.insert(std::begin(m_vChatWindowEntries), pEntry);
	return;
}

uint8_t CChatWindow::GetAlphaFromLastTimePushedMessage()
{
	if (!m_uiMaxTimeChatHide || !m_uiTimeChatHideBegin)
	{
		return 0xFF;
	}

	if ((GetTickCount() - m_uiLastTimePushedMessage) >= TIME_CHAT_HIDE_BEGIN)
	{
		uint32_t time =  GetTickCount() - m_uiLastTimePushedMessage - TIME_CHAT_HIDE_BEGIN;
		uint32_t distance = MAX_TIME_CHAT_HIDE - TIME_CHAT_HIDE_BEGIN;
		if (!distance)
		{
			return 0xFF;
		}
		float res = (float)time / (float)distance;
		if (res >= 1.0f)
		{
			return 0;
		}
		return (255 - (uint8_t)(res * 255.0f));
	}
	return 0xFF;
}

void CChatWindow::AddToChatWindowBuffer(eChatMessageType type, char* szString, char* szNick, 
	uint32_t dwTextColor, uint32_t dwNickColor)
{
	m_uiLastTimePushedMessage = GetTickCount();

	int iBestLineLength = 0;
	CHAT_WINDOW_ENTRY entry;
	entry.eType = type;
	entry.dwNickColor = __builtin_bswap32(dwNickColor | 0x000000FF);
	entry.dwTextColor = __builtin_bswap32(dwTextColor | 0x000000FF);

	if(szNick)
	{
		strcpy(entry.szNick, szNick);
		strcat(entry.szNick, ":");
	}
	else 
		entry.szNick[0] = '\0';

	if(type == CHAT_TYPE_CHAT && strlen(szString) > MAX_LINE_LENGTH)
	{
		iBestLineLength = MAX_LINE_LENGTH;
		//                              
		while(szString[iBestLineLength] != ' ' && iBestLineLength)
			iBestLineLength--;

		//                             12         
		if((MAX_LINE_LENGTH - iBestLineLength) > 12)
		{
			//            MAX_MESSAGE_LENGTH/2
			cp1251_to_utf8(entry.utf8Message, szString, MAX_LINE_LENGTH);
			PushBack(entry);

			//               MAX_MESSAGE_LENGTH/2
			entry.szNick[0] = '\0';
			cp1251_to_utf8(entry.utf8Message, szString+MAX_LINE_LENGTH);
			PushBack(entry);
		}
		else
		{
			//                   
			cp1251_to_utf8(entry.utf8Message, szString, iBestLineLength);
			PushBack(entry);

			//                      
			entry.szNick[0] = '\0';
			cp1251_to_utf8(entry.utf8Message, szString+(iBestLineLength+1));
			PushBack(entry);
		}
	}
	else
	{
		cp1251_to_utf8(entry.utf8Message, szString, MAX_MESSAGE_LENGTH);
		PushBack(entry);
	}

	return;
}

void CChatWindow::FilterInvalidChars(char *szString)
{
	while(*szString)
	{
		if(*szString > 0 && *szString < ' ')
			*szString = ' ';

		szString++;
	}
}

//==================================================================
uintptr_t* dword_609F1C;

//RwRaster* g_WaterDuDvTex;
RwRaster* dword_748C98;
RwRaster* dword_748C94;
RwRaster* dword_748C9C;
//RwRaster* dword_748C90;
//RwRaster* dword_748C8C;
//RwRaster* dword_748C88;
//by t.me/rkkdev
RwRaster* dword_748F80;
RwRaster* dword_748F84;
RwRaster* dword_748F88;
RwRaster* dword_748F8C;
RwRaster* dword_748F90;
RwRaster* dword_748F94;

// Замена для uintptr_t - это просто uintptr_t
uintptr_t dword_7BEE74;
uintptr_t dword_7BEE7C;
uintptr_t dword_7BEE78;
uintptr_t dword_7BEE80;
uintptr_t dword_7BEE84;
uintptr_t dword_7BEE88;
uintptr_t dword_7BEE8C;
uintptr_t dword_7BEE90;
uintptr_t dword_7BEE94;
uintptr_t dword_7BEE98;
uintptr_t dword_7BEE9C;
uintptr_t dword_7BEEA0;
uintptr_t dword_7BEEA4;
uintptr_t dword_7BEEA8;
uintptr_t dword_7BEEAC;
uintptr_t dword_7BEEB0;
uintptr_t dword_7BEEB4;
uintptr_t dword_7BEEB8;
uintptr_t dword_7BEEBC;
uintptr_t dword_7BEEC0;
uintptr_t dword_7BEEC4;
uintptr_t dword_7BEEC8;
uintptr_t dword_7BEECC;
uintptr_t dword_7BEED0;
uintptr_t dword_7BEED4;
uintptr_t dword_7BEED8;
uintptr_t dword_7BEEDC;
uintptr_t dword_7BEEE0;
uintptr_t dword_7BEEE4;
uintptr_t dword_7BEEE8;
uintptr_t dword_7BEEEC;
uintptr_t dword_7BEEF0;
uintptr_t dword_7BEEF8;
uintptr_t dword_7BEEF4;
uintptr_t dword_7BEEFC;
uintptr_t dword_7BEF00;
uintptr_t dword_7BEF04;
uintptr_t dword_7BEF08;
uintptr_t dword_7BEF0C;
uintptr_t dword_7BEF10;
uintptr_t dword_7BEF14;
uintptr_t dword_7BEF18;
uintptr_t dword_7BEF20;
uintptr_t dword_7BEF1C;
uintptr_t dword_7BEF24;
uintptr_t dword_7BEF28;
uintptr_t dword_7BEF2C;
uintptr_t dword_7BEF34;
uintptr_t dword_7BEF30;
uintptr_t dword_7BEF3C;
uintptr_t dword_7BEF40;
uintptr_t dword_7BEF48;
uintptr_t dword_7BEF44;
uintptr_t dword_7BEF54;
uintptr_t dword_7BEF50;
uintptr_t dword_7BEF58;
uintptr_t dword_7BEF4C;
uintptr_t dword_7BEF5C;
uintptr_t dword_7BEF60;

constexpr uintptr_t unk_1AD6B9 = 0x1AD6B9;
constexpr uintptr_t unk_1B0649 = 0x1B0649;
constexpr uintptr_t unk_1B023D = 0x1B023D;
constexpr uintptr_t unk_1AF339 = 0x1AF339;
constexpr uintptr_t unk_1AF74D = 0x1AF74D;
constexpr uintptr_t unk_1AFBB1 = 0x1AFBB1;
constexpr uintptr_t unk_1B1B4D = 0x1B1B4D;
constexpr uintptr_t unk_1B2559 = 0x1B2559;
constexpr uintptr_t unk_1E0EA1 = 0x1E0EA1;
constexpr uintptr_t unk_1AEE2D = 0x1AEE2D;
constexpr uintptr_t unk_1E284D = 0x1E284D;
constexpr uintptr_t locret_19A448 = 0x19A448;
constexpr uintptr_t unk_1B92D5 = 0x1B92D5;
constexpr uintptr_t unk_1B29E5 = 0x1B29E5;
constexpr uintptr_t unk_1D6CED = 0x1D6CED;

uint8_t jj = 0;

extern char* PLAYERS_REALLOC;

void sub_57F64(int a1, uintptr_t a2)
{
	// TODO: реализовать sub_57F64
}

void sub_55814(uintptr_t a1, uintptr_t a2, int a3)
{
	// TODO: реализовать sub_55814
}

bool sub_57DF0(uintptr_t a1)
{
	// TODO: реализовать sub_57DF0
	return (bool)((int (*)(void))(g_libGTASA + 0x41DD7D))();
}

extern void sub_55718(int* param1, uint8_t a2, uint8_t a3, uint16_t a4, int a5)
{
	param1[1] = a3;
	*param1 = a2;
	param1[2] = a4;
	param1[3] = a5;
}
extern void sub_5578E(int* a1) {
	a1[2] | (a1[1] << 8) | (*a1 << 0x10) | (a1[3] << 0x18);
}

int(*LoadAtomicFile)(int a1, int a2);
int LoadAtomicFile_hook(int a1, int a2)
{
	int v3; // r4

	v3 = LoadAtomicFile(a1, a2);
	if (!v3)
	{
		Log("Invalid atomic %d", a2);
		// if ( *(uintptr_t *)(g_libGTASA + 0x88F7CC) )
		 //  ((void (*)(void))(g_libGTASA + 0x337A89))();
		return LoadAtomicFile(a1, 18631);
	}
	return v3;
}

uintptr_t sub_57DE0() {
	return (uintptr_t)PLAYERS_REALLOC;
}

constexpr uintptr_t off_1F95D4 = 0x1F95D4;
constexpr uintptr_t unk_1D4EC2 = 0x1D4EC2;
constexpr uintptr_t unk_1D50BA = 0x1D50BA;

static uint32_t* dword_207F24 = nullptr;

uintptr_t* get_CMirrors_reflBuffer()
{
	return (uintptr_t*)dword_748C98;
}

void dword_7BEF38(int a1, int a2)
{
	return ((void (*)(int, int))((char*)0x1B8071 + g_libGTASA))(a1, a2);
}

uintptr_t* sub_5212C(uintptr_t* a1)
{
	*reinterpret_cast<uintptr_t*>(a1 + 8) = 1;
	*reinterpret_cast<uintptr_t*>(a1) = off_1F95D4;
	*reinterpret_cast<uintptr_t*>(a1 + 0xC) = sub_57DE0();
	*reinterpret_cast<uintptr_t*>(a1 + 4) = sub_57DE0();

	uintptr_t v2 = *reinterpret_cast<uintptr_t*>(a1 + 0xC);
	*reinterpret_cast<char*>(a1 + 0x10) = 0;

	sub_57F64(0, v2);
	sub_55814(unk_1D4EC2, *reinterpret_cast<uintptr_t*>(a1 + 8), 0);

	if (*reinterpret_cast<uintptr_t*>(a1 + 0xC) && sub_57DF0(*reinterpret_cast<uintptr_t*>(a1 + 8)))
		sub_55814(unk_1D50BA, *reinterpret_cast<uintptr_t*>(a1 + 8), 1);

	*reinterpret_cast<uintptr_t*>(a1 + 0x11) = 0;
	*reinterpret_cast<uint16_t*>(a1 + 0x1D) = 1;
	*reinterpret_cast<uintptr_t*>(a1 + 0x15) = 0;
	*reinterpret_cast<uintptr_t*>(a1 + 0x19) = 0;
	*reinterpret_cast<uintptr_t*>(a1 + 0x1F) = 0x3C03126F;

	memset(reinterpret_cast<void*>(a1 + 0x50), 0, 0x23Au);

	reinterpret_cast<void(*)(uintptr_t)>(g_libGTASA + 0x43E201)(*reinterpret_cast<uintptr_t*>(a1 + 0xC));
	reinterpret_cast<void(*)(uintptr_t)>(g_libGTASA + 0x43E229)(*reinterpret_cast<uintptr_t*>(a1 + 0xC));

	return a1;
}

#include "..//game/WaterShader.h"
uintptr_t* g_WaterShaderClass = nullptr;
void (*emu_glEndInternal)();
void emu_glEndInternal_hook()
{
	if (((*(uintptr_t*)(g_libGTASA + 0x61572C)) & 0x80000) != 0)
	{
		if (g_WaterShaderClass == nullptr)
		{
			//getClassWaterShader(&WaterShaderClass);
			//g_WaterShaderClass = new uintptr_t{};
			g_WaterShaderClass = (uintptr_t*)malloc(64);
			memset(g_WaterShaderClass, 1, 64);
			//for (int i = 0; i < 64; ++i) {
			//    *(uintptr_t *)(g_WaterShaderClass + i) = 0;
			//}
			WaterShader::BuildShadersSource1(g_WaterShaderClass);
			//	WaterShader::BuildShadersSource2(g_WaterShaderClass);
		}
		else {
			//LOGG(OBFUSCATEV2("Cant Build Shaders"));
		}
		if (g_WaterShaderClass) {
			WaterShader::EmuShader__Select3(g_WaterShaderClass);
			//WaterShader::GetTexture_waterDUDV();
			//LOGG(OBFUSCATEV2("Selecting Shaders"));
		}
		else {
			//LOGG(OBFUSCATEV2("Shaders class is null"));
		}

	}

	emu_glEndInternal();
	//(uintptr_t *)(*(uintptr_t *)(g_libGTASA + 0x61572C) << 12);
	//if ( (((*(uintptr_t *)(g_libGTASA + 0x61572C)) & 0x80000) != 0)  && g_WaterShaderClass )
	//{
	//    LOGG(OBFUSCATEV2("Setting waterDUDV for Shaders"));
	//    WaterShader::GetTexture_waterDUDV();
	//}



}

extern void sub_3F614(uintptr_t* a1, int a2)
{
	uint32_t* v4; // r5
	int v5; // r6
	uint32_t* v6; // r8
	int v7; // r6
	uint32_t* v8; // r1
	uint32_t* v9; // r0
	bool v10; // zf
	int v11; // r2
	bool v12; // zf
	int v13; // r4
	int v14; // r5
	int v15; // r0
	char v16; // r0
	uint32_t* v17; // r0
	int v18; // r0
	int v19; // r6
	float v20; // s0
	int v21; // r0
	float v22; // s4
	void(__fastcall * **v23)(uint32_t, uint32_t); // r6
	void(__fastcall * *v24)(uint32_t, uint32_t); // r0
	void(__fastcall * **v25)(uint32_t, uint32_t); // r6
	void(__fastcall * *v26)(uint32_t, uint32_t); // r0
	int* v27; // r1
	int v28; // r1
	int v29; // r0
	int v30; // r0
	uint32_t* v31; // r0
	int v33; // [sp+10h] [bp-48h] BYREF
	int v34[3]; // [sp+14h] [bp-44h] BYREF
	int64_t v35; // [sp+20h] [bp-38h] BYREF
	int v36; // [sp+28h] [bp-30h]
	float v37; // [sp+2Ch] [bp-2Ch] BYREF
	float v38; // [sp+30h] [bp-28h]
	float v39; // [sp+34h] [bp-24h]
	int v40; // [sp+38h] [bp-20h]

	// нету


	v4 = dword_207F24;
	if (dword_207F24)
	{
		if (!*dword_207F24)
		{
			//  v5 = operator new(0x28Au);
			  // нету
			sub_5212C(a1);
			*v4 = v5;
		}
		//	Log("testcam 3");
		dword_7BEF38(1, 0);
		dword_7BEF38(6, 0);
		dword_7BEF38(8, 0);
		dword_7BEF38(0xC, 0);
		dword_7BEF38(0xA, 5);
		dword_7BEF38(0xB, 6);
		dword_7BEF38(0xE, 0);
		dword_7BEF38(0x14, 1);
		//Log("testcam 4");
		v6 = (uint32_t*)(0x1AD8A1);
		//	Log("testcam 44");
		v7 = *(uint32_t*)((char*)g_libGTASA + 0x95B064);
		//	Log("testcam 5");
		v33 = 0xFFFFFFFF;
		if (a2)
			v7 = a2;
		// Log("testcam 6");
		((void(__fastcall*)(int, int*, int))(0x1AD8A1 + g_libGTASA))(v7, &v33, 2);
		((void(__fastcall*)(int, int*, int))(0x1AD8A1 + g_libGTASA))(v7, &v33, 3);
		//Log("testcam 7");
	 ///   v8 = (uint32_t*)(a1 + 0x40);
	  ///  v9 = (uint32_t*)(a1 + 4 * *v8);
		v10 = v9 == 0;
		//Log("testcam 8");
		if (v9)
		{
			/// v8 = (uint32_t*)(a1 + 4 * *v8);
		   ///  v6 = (uint32_t*)(v8[4]);
			v10 = v6 == 0;
		}
		//	Log("testcam 9");
		if (!v10)
		{
			v11 = v8[8];
			v12 = v11 == 0;
			if (v11)
				v12 = v8[0xC] == 0;
			if (!v12)
			{
				//	  Log("testcam 10");
				if (!a2)
					v6 = v9;
				v13 = g_libGTASA;
				v14 = *(uint32_t*)(0x61572C + g_libGTASA);
				v15 = g_libGTASA;
				if ((v14 & 0x800000) != 0)
				{

					*(uint32_t*)(0x61572C + g_libGTASA) = v14 & 0xF77FFFFF;
					//  v16 = byte_609F24;
					//  __dmb(0xBu);
				//	Log("testcam 11");
					if ((v16 & 1) == 0 /*&& j___cxa_guard_acquire((__guard*)&byte_609F24)*/)
					{
						v17 = (uint32_t*)(::operator new(4u));
						*v17 = 0;
						//  dword_609F20 = (int)(v17);
						//  j___cxa_guard_release((__guard*)&byte_609F24);
					}
					// *(uint32_t*)dword_609F20 = *(uint32_t*)(a1 + 4 * *(uint32_t*)(a1 + 0x40) + 0x30);
				//     v18 = nullsub_16(&v37);
				//     sub_55754(v18, 0xFFFFFFFF);
				//     sub_57158(dword_609F20, 0, 0, 0x44000000, 0x44000000, &v37);
					*(uint32_t*)((char*)0x61572C + v13) |= v14 & 0x8000000 | 0x800000;
					v15 = g_libGTASA;
					//  Log("testcam 12");
				}
				//  v19 = /*v6[1]*/(int)CSkyBox::GetSkyObject();
			  //	Log("testcam 12 0");
				//  v38 = (float)((float)((float)*(unsigned int*)(0x63E3C4 + v15) + 150.0) / 150.0) * 0.8;// scale sky
			  //	Log("testcam 12 1");
				v37 = v38;
				//	Log("testcam 12 2 %f", v37);
				 ///   v39 = v38;
				 ///   ((void ( *)(int, float*, uint32_t))(0x1AED7D + v15/* + 0x18*/))(v19, &v37, 0);
					//Log("testcam 12 3");
				  ///  v36 = 0x3F800000;
				  ///  v35 = 0LL;
				//	Log("testcam 13");
				//    ((void (__fastcall *)(int, int64_t*, uint32_t, int))(0x1AED7D + g_libGTASA + 0x48))( //rotate sky
				 //     v19,
				 //     &v35,
				 //     *(float*)&dword_6295CC * 360.0,
				  //    1);
				 //   v34[0] = *(uint32_t*)((char*)g_libGTASA + 0x30);
				//    v34[1] = *(uint32_t*)((char*)g_libGTASA + 0x34);
				 //   v34[2] = *(uint32_t*)((char*)g_libGTASA + 0x38);
					//((void (__fastcall *)(int, int*, int))(0x1AED7D + g_libGTASA))(v19, v34, 2); //rw translate
				  /*  ((void (*)(void))((char*)g_libGTASA + 0x559EF9))();
					((void (*)(void))((char*)g_libGTASA + 0x559FC9))();
					v20 = 0.0;
				//	Log("testcam 14");
				 //   v21 = *(uint32_t*)(a1 + 0x40);
					v22 = 0.0;
					if ( v21 == 3 )
					  v22 = 4.0;
					if ( !v21 )
					{
					  v20 = 5.0;
					  v22 = 11.0;
					}
					if ( v21 == 1 )
					  v20 = 12.0;
					if ( v21 == 1 )
					  v22 = 18.0;
					if ( v21 == 2 )
					{
					  v20 = 19.0;
					  v22 = 24.0;
					}
				//	Log("testcam 15");
			   //     *(float*)(a1 + 0x44) = (float)((float)*(unsigned char*)(g_libGTASA + 0x8B18A5) - v20) / (float)(v22 - v20);
					if ( !dword_609F28 )
					{
				   //   v23 = reinterpret_cast<void (__fastcall ***)(uint32_t, uint32_t)>(::operator new(0xCu));
				   //   sub_6FD48();
				   //   v24 = *v23;
				  //    dword_609F28 = reinterpret_cast<int>(v23);
				  //    (*v24)(v23, 0);
					 // sub_6F580((uintptr_t*)dword_748F98, dword_609F28);
					//  v25 = reinterpret_cast<void (__fastcall ***)(uint32_t, uint32_t)>(::operator new(0xCu));
				   //   sub_6FE14();
					//  v26 = *v25;
					//  dword_609F2C = reinterpret_cast<int>(v25);
				 //     (*v26)(v25, 0);
					//  sub_6F580((uintptr_t*)dword_748F98, dword_609F2C);
				//	  Log("testcam 16");
					}
					if ( (byte_61572E[v13] & 0x80) != 0 )
					  v27 = &dword_609F2C;
					else
					  v27 = &dword_609F28;
					//sub_6F6A0((uintptr_t*)dword_748F98, *v27);
				  //  v28 = *(uint32_t*)(a1 + 0x40);
					v29 = v28 + 1;
					//Log("testcam 17");
					if ( v28 > 2 )
					  v29 = 0;
				 //   v30 = a1 + 4 * v29;
				 //   if ( a2 )
				//      v31 = (uint32_t*)(v30 + 0x30);
				 //   else
				 //     v31 = (uint32_t*)(v30 + 0x20);
				 // Log("testcam 133");
				//    sub_6EC24(*v31, 1);
				///	Log("testcam 441");
				  //  sub_58518(v6);
				//	Log("testcam 18");
				  //  sub_6F706((char*)dword_748F98);*/
			}
		}
	}
}

void sub_6F160() {
	int* v2;
	RwRaster* result;
	dword_748F80 = RwRasterCreate(256, 256, 32, 1285);
	v2 = (int*)operator new(0x48u);
	//sub_3F028(v2);
	dword_609F1C = (uintptr_t*)v2;
	dword_748F84 = RwRasterCreate(256, 256, 0, 1);
	dword_748F88 = RwRasterCreate(256, 256, 32, 1285);
	dword_748F8C = RwRasterCreate(256, 100, 0, 1);
	dword_748F90 = RwRasterCreate(256, 256, 32, 1285);
	//	dword_748C88 = *(RwRaster**)LoadTextureFromDB("mobile", "BR_Nebo_Day");
	  //  dword_748C90 = *(RwRaster**)LoadTextureFromDB("mobile", "waterDUDV");
	dword_748C98 = RwRasterCreate(256, 256, 32, 1285);
	dword_748C94 = RwRasterCreate(256, 256, 0, 1);
	dword_748C9C = RwRasterCreate(256, 256, 0, 1);
	//dword_748C8C = /*RwRasterCreate(256, 256, 0, 1);*/*(RwRaster**)LoadTextureFromDB("mobile", "BR_Nebo_Day");
	result = RwRasterCreate(100, 100, 0, 1);
	dword_748F94 = result;
	// dword_7BEE74 = g_libGTASA + 0x95B068;
	dword_207F24 = (uint32_t*)(g_libGTASA + 0x8B1104);
}

extern CNetGame* pNetGame;
extern CGame* pGame;
void sub_6ED20(int a1, uintptr_t* a2, uintptr_t* a3, int a4, int a5, int a6)
{
	int v8 = g_libGTASA;
	int v9;
	float* v10;
	float v11;
	int v12;
	int v13;
	int* v144;
	bool v14;
	int v16;
	int v17;
	int v18;
	int v19;
	float v166;
	float v155;
	VECTOR v77;
	int v222; // r0
	int v233; // [sp+8h] [bp+0h]
	float v288; // [sp+24h] [bp+1Ch]

	int v20;
	float v21;
	int v22[9];
	//Log("1");
	if (a2)
	{
		v9 = *(int*)(g_libGTASA + 0x95B064);
		v17 = *(int*)(v9 + 0x60);
		v16 = *(int*)(v9 + 0x64);
		*(int*)(g_libGTASA + 0x9B4CB8) = 3;
		*(uintptr_t*)(v9 + 0x64) = *a3;
		*(uintptr_t*)(v9 + 0x60) = *a2;
		*(int*)((char*)0x61572C + v8) |= a1;
		v22[0] = 0xFFFFFFFF;
		((void(__fastcall*)(int, int*, int))((char*)0x1AD8A1 + g_libGTASA))(v9, v22, 3);
		//Log("11");
		   // if (!*(int *)(dword_207F50 + 0x12C))
		  //  {
		   //   nullsub_16(&v19);
		sub_55718(
			&v19,
			*(uint8_t*)(g_libGTASA + 0x8C9AB4),
			*(uint8_t*)(g_libGTASA + 0x8C9AB6),
			*(uint16_t*)(g_libGTASA + 0x8C9AB8),
			0xFF);
		sub_5578E(&v19);

		((void (*)(int, int*, int))((char*)0x1AD8A1 + g_libGTASA))(v9, (int*)&v19, 3);
		//  }
	  //Log("2");
		if (((int(__fastcall*)(int))(g_libGTASA + 0x463DF1))(v9) == 1)
		{
			*(int*)(g_libGTASA + 0x95B064 + 0x84) = *(int*)(g_libGTASA + 0x9B4C2C);
			*(float*)(g_libGTASA + 0x95B064 + 0x88) = *(float*)(g_libGTASA + 0x9B4C2C) * 100.0; //in gtasa

			*(uint8_t*)(v8 + 0x9B4CB2) = 1;
			v21 = 0.0;
			v19 = 0;
			v20 = 0;
			//  Log("22");

			 /* if (sub_57DE0())
			  {
				  Log("sub_57DE0 succes,  = %d", (int)sub_57DE0());
				v10 = *(float **)(/*sub_57DE0() + 0x14*//*g_libGTASA + 0x8a1144);
				if (v10)
				{
					Log("v10 succes,  = %d", (int)v10);
					Log("666");
				  v11 = (float)*((int *)v10 + 0xC);
				  Log("5e55");
				  v12 = *((int *)v10 + 0xD);
				  Log("66666");
				  v21 = v10[0xE];
				  Log("42343");
				  v19 = v11;
				  Log("34234");
				  v20 = v12;
				}
			  }*/

			v77.Z = 0.0f;
			if (pChatWindow->isConnect) {
				if (g_libGTASA + 0x8A1144)
				{
					/*	  if (g_libGTASA + 0x8B081C)
							v144 = (int *)(g_libGTASA + 0x8B081C + 0x30);
						  else
							v144 = (int *)(g_libGTASA + 0x8B080C);
						  v155 = *((float *)v144 + 1);
						  v166 = *((float *)v144 + 2);
						  v77.X = *v144;
						  v77.Y = v155;
						  v288 = v166;*/
						  // CPlayerPed* pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
						   //pPed->GetBonePosition(4, &v77);
					v77 = *(VECTOR*)(0x8C1638 + g_libGTASA);
				}
			}
			//Log("3");
			if ((a1 & 0x8000000) == 0)
				v21 = v21 + 2.0;
			//v77.Z += 0.6f;
			float cameraPosition[3] = { v77.X, v77.Y, v77.Z };
			((void (*)(float[3]))(0x197E0C + g_libGTASA + 1))(cameraPosition);
			// Log("v77 = %f", v77.X);


			if (*(int*)(g_libGTASA + 0x8B7D28))
			{
				v18 = 0;
				((void(__fastcall*)(int, int*, int))((char*)0x1AD8A1 + g_libGTASA))(v9, &v18, 2);
				((void(__fastcall*)(int, int*, int))((char*)0x1AD8A1 + g_libGTASA))(v9, &v18, 3);
				a4 = 1;
				v13 = a6;
			}
			else
			{
				v13 = a6;
				//    if (dword_609F1C && !a6 /*&& *(int *)(dword_207F50 + 0x12C*/)
					//  ((void (*)(void))sub_3F614)();

			}
			((void (*)(void))(g_libGTASA + 0x559009))();
			v14 = (a4 & (v13 ^ 1)) == 0;
			if ((a4 & (v13 ^ 1)) == 0)
				v14 = a5 == 0;

			if (v14)

			{
				if (/**(int *)(dword_207F50 + 0x12C) && */dword_609F1C)
					sub_3F614(dword_609F1C, 0);
				((void (*)(void))(g_libGTASA + 0x3B3B41))();
				((void (*)(void))(g_libGTASA + 0x3B1609))();
				((void (*)(void))(g_libGTASA + 0x3B1779))();
			}

			((void (*)(int))((char*)0x1AD6B9 + g_libGTASA))(v9);

			*(uint8_t*)(v8 + 0x9B4CB2) = 0;
		}
		*(int*)((char*)0x61572C + v8) &= ~a1;
		*(int*)(v9 + 0x64) = v16;
		*(int*)(v9 + 0x60) = v17;
		*(int*)(g_libGTASA + 0x9B4CB8) = 0;
	}

}

int sub_6F04C()
{
	unsigned int v1;
	int v2;
	int v3;
	int result;
	int settings_WaterReflLvl = 3;

	//  if ( *(uintptr_t *)(g_libGTASA + 0x63E464) ) 
	  //{
		  //Log("1");
	((void (*)())(g_libGTASA + 0x54DE1D))();// CMirrors::CreateBuffer(CMirrors *this)
// }
	v1 = *(uintptr_t*)(g_libGTASA + 0x63E464);
	if (v1 >= 1)
		//	Log("2");
		sub_6ED20(0x800000, *(uintptr_t**)(g_libGTASA + 0x9B4CD0), *(uintptr_t**)(g_libGTASA + 0x9B4CD4), v1 == 2, 0, v1 == 3);
	v3 = settings_WaterReflLvl;
	if (v3 >= 1)                                // water 
	{
		if (v3 != 1)
			v3 = 0;
		//	Log("3");
		sub_6ED20(0x8800000, *(uintptr_t**)dword_748F90, *(uintptr_t**)dword_748F94, v3, 0, 0);
		if (settings_WaterReflLvl >= 3)
		{
			//	Log("4");
			sub_6ED20(0x8000000, *(uintptr_t**)dword_748F88, *(uintptr_t**)dword_748F8C, 0, 0, 0);
		}
	}
	result = 1;
	if (result) {
		//Log("5");
		sub_6ED20(0, *(uintptr_t**)dword_748F80, *(uintptr_t**)dword_748F84, 0, 1, 0);
		return 0;
	}

	return result;
}

void updateWater();
void (*CMirrors__RenderReflections)(uintptr_t* thiz);
void CMirrors__RenderReflections_hook(uintptr_t* thiz) {
	//
	if (jj != 0) {
		//  CMirrors__RenderReflections(thiz);
		sub_6F04C();
		return;
	}


	sub_6F160();
	jj = 1;
	/*updateWater();
	if (UniformLocationWaveness != -1) {
				waterwave += 0.03;

			glUniform1f(glGetUniformLocation(*(uintptr_t *)(UniformLocationWaveness + 1000), "Waveness"), waterwave);
			Log("waterwave %f", waterwave);
		}*/
		// Или любое другое значение, которое вы хотите установить


	   //g_WaterShaderClass[4] += 0.03f;


	//   CMirrors__RenderReflections(thiz);

	sub_6F04C();

	return;
}

int (*car_customenvmap)(int result, int a2);
int car_customenvmap_hook(int result, int a2)
{
	int* v2; // r4
	unsigned int v3; // r12

	v2 = *(int**)(0x669E48 + g_libGTASA);
	v3 = *(int*)(a2 + result) - *v2;
	if (v3 / 0xC < v2[2])
	{
		*(uintptr_t*)(v2[1] + v3 / 0xC) |= 0x80u;
		*(int*)(result + a2) = 0;
	}
	return result;
}

void gooo() {
	//SetUpHook(SA_ADDR(0x1A7A58), (uintptr_t)sub_29C0C_hook, (uintptr_t*)&sub_29C0C);
	//sub_1B4A8(0x1A8E20 + g_libGTASA + 0x3FC, sub_29D0C, &unk_207F64);
	//SetUpHook(SA_ADDR(0x1A8E20 + 0x3FC), (uintptr_t)sub_29D0C_hook, (uintptr_t*)&sub_29D0C);
 // sub_1B4A8((char *)sub_1A1F40 + g_libGTASA + 0x274, sub_29E10, &unk_207F68);
	//SetUpHook(SA_ADDR(0x1A8E20), (uintptr_t)sub_29E40_hook, (uintptr_t*)&sub_29E40);
}

int (*dword_67E048)();
int DoSunAndMoon()
{
	float v0; // s16
	int result; // r0

	v0 = *(float*)(g_libGTASA + 0x985264);
	if ( /**(int *)(dword_207F50 + 0x134)*/ pChatWindow->isConnect)
		*(float*)(g_libGTASA + 0x985264) = v0 * 4.5;
	dword_67E048();
	result = g_libGTASA + 0x985264;
	*(float*)(g_libGTASA + 0x985264) = v0;
	return result;
}
//==========================================================

void __fillArray()
{
}

void InitCTX(AES_ctx&, unsigned char const*)
{

}

/* ====================================================== */