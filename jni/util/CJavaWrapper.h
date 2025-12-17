#pragma once

#include <jni.h>

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

	jmethodID s_GetClipboardText;
	jmethodID s_CallLauncherActivity;

                  jmethodID s_RadarBR;

	jmethodID s_ShowInputLayout;
	jmethodID s_HideInputLayout;
	jmethodID s_showShop;
	jmethodID s_hideShop;
	jmethodID s_ShowClientSettings;
	jmethodID s_SetUseFullScreen;
	jmethodID s_MakeDialog;

	jmethodID s_showFuelStation;
	jmethodID s_showTaxi;
        jmethodID s_showTabWindow;
	jmethodID s_setTabStat;

	jmethodID s_showHud;
                  jmethodID s_hideHud;


	jmethodID s_updateHudInfo;

	jmethodID s_showradar;
                  jmethodID s_hideradar;

	jmethodID s_showGps;
                  jmethodID s_hideGps;
	

	jmethodID s_showTuningstage;
		jmethodID s_showTuningmenu;

	jmethodID s_showZona;
	jmethodID s_hideZona;

	jmethodID s_showHelper;
        jmethodID s_hideHelper;
	
	jmethodID s_showfraction;
	jmethodID s_showfractiontext;
	jmethodID s_showfractionquest;

                  jmethodID s_showx2;
	jmethodID s_hidex2;
	   jmethodID s_showBrDiner;
    	jmethodID s_hideBrDiner;

	jmethodID s_showtest;
	jmethodID s_showtuning;

	jmethodID s_showcatcoin;

	jmethodID s_showSpeed;
                  jmethodID s_hideSpeed;
	jmethodID s_updateSpeedInfo;

	jmethodID s_showHudButtonG;
	jmethodID s_hideHudButtonG;

	jmethodID s_showNotification;
	jmethodID s_showMenu;


	jmethodID s_showFamily;
	jmethodID s_showSettingf;
	jmethodID s_showuluc;

	jmethodID s_setPauseState;

	jmethodID s_showSplash;
	jmethodID s_updateSplash;
	jmethodID s_showRadial;
       
        jmethodID s_showRecon;
        jmethodID s_hideRecon;

        jmethodID s_showdriving;

        jmethodID s_showbp;



        jmethodID s_showPerson;
	jmethodID s_hidePerson;

        jmethodID s_updateAuto;
	jmethodID s_showAuto;
	jmethodID s_hideAuto;

        jmethodID s_showCalendar;

        jmethodID s_showSpawn;

        jmethodID s_showInventory;
	jmethodID s_closeInventory;
	jmethodID s_addItemToInv;

	jmethodID s_showCaptcha;
	jmethodID s_hideCaptcha;
	jmethodID s_showsmi;

	jmethodID s_showHudFeatures;
	jmethodID s_hideHudFeatures;
	jmethodID s_showJbl;

	jmethodID s_showDonate;
	jmethodID s_updateDonate;
	jmethodID s_show_sc;
	
	jmethodID s_showHudAndLogo;
	jmethodID s_hideHudAndLogo;
public:
	JNIEnv* GetEnv();

	std::string GetClipboardString();
	void CallLauncherActivity(int type);

	void ShowInputLayout();
	void HideInputLayout();

	void ShowRadar();
	void HideRadar();
	void ShowShop(int price);
	void HideShop();
	void SetRadar();
	void ShowFamily(char* name, int num, int num2);
	void ShowSettingf(char* name, char* statys, int repa, int colivo, int lvl1, int lvl2, int lvl3, int cash, int mater, int masks, int fuels, int ammos, int armors);
	void showuluc(int lvl1, int lvl2, int lvl3, int cash, int maxcash, int mats, int mask, int fuels, int ammos, int puples);
	void showtest();
	void showtuning();

	void ShowFuelStation(int type, int price1, int price2, int price3, int price4, int price5, int maxCount);

	void ShowFraction(int id);
	void ShowQuest(int id);
	void FixText(int id);

	void showcatcoin();

   	void ShowG();
   	void HideG();

	void ShowClientSettings();

        void ShowTabWindow();
	void SetTabStat(int id, char* name, int score, int ping);

	void SetUseFullScreen(int b);
	void HideBrDiner();
    	void ShowBrDiner();

	void UpdateHudInfo(int health, int armour, int hunger, int weaponidweik, int ammo, int ammoinclip, int money, int wanted);

	void ShowTuningstage(int money);
		void ShowTuningmenu(int money);

	void ShowHud();
                  void HideHud();

	void ShowGPS();
                  void HideGPS();

	void ShowZona();
                  void HideZona();

	void ShowX2();
                  void HideX2();
                  
   void ShowHudFeatures();
   void HideHudFeatures();
   
   void ShowHudAndLogo();
   void HideHudAndLogo();

	void UpdateSpeedInfo(int speed, int fuel, int hp, int mileage, int engine, int light, int belt, int lock);
	void ShowSpeed();
                  void HideSpeed();

	void MakeDialog(int dialogId, int dialogTypeId, char* caption, char* content, char* leftBtnText, char* rightBtnText); // Диалоги
	void ShowNotification(int type, char* text, int duration, char* actionforBtn, char* textBtn);
	void ShowMenu();
	void ShowSmi(char* smi_edit, char* smi_player, char* smi_textplayer);

	void SetPauseState(bool a1);

	void ShowSplash();
	void UpdateSplash(int progress);	
	

	void ShowRadial(bool park, bool key, bool doors, bool lights, bool suspension, bool launch_control, bool engine, bool turbo);

        void HideRecon();
        void ShowRecon(char* nick, int id);

        void showInventory();
        void closeInventory();
        void addItemToInv(int id, int arg, int slot);



        void ShowCalendar(int calendar);

        void ShowBp(int money, int bc);

        void ShowDriving(int id);

        void ShowAuto();
        void UpdateAuto(char* name, int price, int count, float maxspeed, float acceleration, int gear);
        void HideAuto();

        void ShowPerson();
	void HidePerson();

	void showHelper(int type);
        void hideHelper();
	void ShowTaxi(int id, int num, int cash, int price, int rast);
	void ShowCaptcha();
	void HideCaptcha();

        void ShowSpawn();
	void ShowJbl();

	void ShowDonate(int money, int bc);
	void UpdateDonate(int money, int bc);
	void show_sc(int money, int bc);
	
    void setTurnState();

	CJavaWrapper(JNIEnv* env, jobject activity);
	~CJavaWrapper();
};

extern CJavaWrapper* g_pJavaWrapper;