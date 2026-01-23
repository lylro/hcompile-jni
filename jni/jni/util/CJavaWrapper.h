#pragma once

#include <jni.h>
#include "../game/CRenderTarget.h"
#include <string>

#define EXCEPTION_CHECK(env) \
	if ((env)->ExceptionCheck()) \ 
	{ \
		(env)->ExceptionDescribe(); \
		(env)->ExceptionClear(); \
		return; \
	}

class CJavaWrapper
{
	jobject activity;
	
	
	
	//jclass nvEventClass;
	
jmethodID s_CallLauncherActivity;
	jmethodID s_GetClipboardText;
	jmethodID s_setStat;
	
	jmethodID addChatMessage;
	jmethodID closekeY;
	jmethodID s_showInv;
	jmethodID s_showschool;
	jmethodID s_showendtest;
	jmethodID s_setanswers;

                  jmethodID s_showTabb;
                  jmethodID s_sendConnect;
    jmethodID s_updateAuto;
    jmethodID s_updInv;
    
    jmethodID s_showKs;
    jmethodID s_hideKs;
    
    jmethodID s_showAuto;
    jmethodID s_hideAuto;
	jmethodID s_ShowInputLayout;
	jmethodID s_HideInputLayout;
	jmethodID s_addChatMessage;

	jmethodID s_ShowClientSettings;
	jmethodID s_SetUseFullScreen;
	jmethodID s_MakeDialog;

	jmethodID s_showFuelStation;
	jmethodID s_showSpawnSelector;


	jmethodID s_showradar;
                  jmethodID s_hideradar;

	jmethodID s_showGps;
                  jmethodID s_hideGps;

	jmethodID s_showZona;
	jmethodID s_hideZona;
                  jmethodID s_showx2;
	jmethodID s_hidex2;

	jmethodID s_showSpeed;
                  jmethodID s_hideSpeed;
	jmethodID s_updateSpeedInfo;

	jmethodID s_showNotification;
	jmethodID s_showMenu;

	jmethodID s_setPauseState;
	jmethodID s_sendDonateNew;
	jmethodID s_shownewq;

	jmethodID s_showSplash;
	jmethodID s_updateSplash;
	jmethodID s_showRadial;
	jmethodID s_sendMusicData;
	
	jmethodID s_showHudFeatures;
	jmethodID s_showTuning;
	jmethodID s_hideHudFeatures;

	jmethodID s_showDonate;
	jmethodID s_updateDonate;
	jmethodID s_show_sc;
	jmethodID s_showMusic;
	
	jmethodID s_showHudAndLogo;
	jmethodID s_hideHudAndLogo;
public:
	JNIEnv* GetEnv();
	
	

	std::string GetClipboardString();
	void CallLauncherActivity(int type);

	void ShowInputLayout();
	CRenderTarget* m_pRenderTarget;
	void HideInputLayout();
	void ShowSchool(int a, int b, int c, int d, int e, int f, int g, int h, int l, int j);
	void AddChatMessage(const char msg[], uint32_t textcolor);

	void ShowRadar();
	void ShowEndTest(int i, int y);
	void SetAnswers(int i, int i2);
	
	void showKs();
	void  hideKs();
	void showMusic();
	void  showInv();
	
	RwTexture *CreateTextureSnapShot();
	
	void HideRadar();
	void setStat(int idd, char* namee, int score, int ping);
	void ShowTabb();
	void sendDonateNew(int access1, int access2);
	void SetRadar();
	void HideAuto();
	void NextQuestSchool();
	void sendMusicJson(int page1, int num, int page2, int but, int isPlay, int currentMusic);
	void ShowAuto();
	void ShowTuning(int type, char* text, int money);
	void UpdateAuto(char* text0, char* text1, char* text2, char* text3);
	void updInv(int ction, int id, int modelId, int dr, int col);
	void closekey();

	void MenuHud();
	void ShowClientSettings();
	void SetUseFullScreen(int b);
	void SendConnect(int text);
	void ShowSpawnSelector();

	void ShowFuelStation(int type, int price1, int price2, int price3, int price4, int price5, int maxCount);

//	void UpdateHudInfo(int health, int armour, int hunger, int weaponidweik, int ammo, int ammoinclip, int money, int wanted);
	void ShowHud();
                  void HideHud();

                 


	void MakeDialog(int dialogId, int dialogTypeId, char* caption, char* content, char* leftBtnText, char* rightBtnText); // Диалоги
	void ShowNotification(int type, char* text, int duration, char* actionforBtn, char* textBtn);
	void ShowMenu();

	void SetPauseState(bool a1);

	void ShowSplash();
	void UpdateSplash(int progress);	

	void ShowRadial(bool park, bool key, bool doors, bool lights, bool suspension, bool launch_control, bool engine, bool turbo);

	void ShowDonate(int money, int bc);
	void UpdateDonate(int money, int bc);
	void show_sc(int money, int bc);

	CJavaWrapper(JNIEnv* env, jobject activity);
	~CJavaWrapper();
};

extern CJavaWrapper* g_pJavaWrapper;
