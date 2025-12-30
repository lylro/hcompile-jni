#pragma once

#include <vector>
#include <string>
#include <jni.h>

#define EXCEPTION_CHECK(env) \
	if ((env)->ExceptionCheck()) \
	{ \
		(env)->ExceptionDescribe(); \
		(env)->ExceptionClear(); \
		return; \
	}

class CJavaWrapper
{

    jmethodID s_showradar;
   
public:
    JNIEnv* GetEnv();

    void ShowMain(const std::string& jsonString);
	void ShowShop(const std::string& jsonString);
	void ShowInv(const std::string& jsonString);
	void ShowMap(const std::string& jsonString);
	void ShowChat(const std::string& jsonString);
	void ShowGear(const std::string& jsonString);
	void ShowBag(const std::string& jsonString);
	void ShowMail(const std::string& jsonString);
	void ShowRank(const std::string& jsonString);
	void ShowLog(const std::string& jsonString);
	void ShowInfo(const std::string& jsonString);
	void ShowHelp(const std::string& jsonString);
	void ShowMenu(const std::string& jsonString);
	void ShowJoin(const std::string& jsonString);
	void ShowPlay(const std::string& jsonString);
	void ShowLoad(const std::string& jsonString);
	void ShowSave(const std::string& jsonString);
	void ShowOpt(const std::string& jsonString);
	void ShowExit(const std::string& jsonString);
	void ShowUpg(const std::string& jsonString);
	void ShowDown(const std::string& jsonString);
	void ShowLeft(const std::string& jsonString);
	void ShowRight(const std::string& jsonString);
	void ShowTop(const std::string& jsonString);
	void ShowBot(const std::string& jsonString);
	void ShowHud(const std::string& jsonString);
	void ShowWin(const std::string& jsonString);
	void ShowLose(const std::string& jsonString);
	void ShowPvP(const std::string& jsonString);
	void ShowPvE(const std::string& jsonString);
	void ShowCoop(const std::string& jsonString);
	void ShowClan(const std::string& jsonString);
	void ShowAlly(const std::string& jsonString);
	void ShowFoe(const std::string& jsonString);
	void ShowBuff(const std::string& jsonString);
	void ShowDebuff(const std::string& jsonString);
	void ShowPet(const std::string& jsonString);
	void ShowMount(const std::string& jsonString);
	void ShowSkill(const std::string& jsonString);
	void ShowTree(const std::string& jsonString);
	void ShowQuest(const std::string& jsonString);
	void ShowNote(const std::string& jsonString);
	void ShowMark(const std::string& jsonString);
	void ShowCamp(const std::string& jsonString);
	void ShowZone(const std::string& jsonString);
	void ShowFort(const std::string& jsonString);
	void ShowTrap(const std::string& jsonString);
	void ShowSafe(const std::string& jsonString);
	void ShowWild(const std::string& jsonString);
	void ShowSky(const std::string& jsonString);
	void ShowDark(const std::string& jsonString);
	void ShowLite(const std::string& jsonString);
	void ShowRain(const std::string& jsonString);
	void ShowSnow(const std::string& jsonString);
	void ShowFire(const std::string& jsonString);
	void ShowIce(const std::string& jsonString);
	void ShowWind(const std::string& jsonString);
	void ShowDust(const std::string& jsonString);
	void ShowFog(const std::string& jsonString);
	void ShowBoss(const std::string& jsonString);
	void ShowMin(const std::string& jsonString);
	void ShowMax(const std::string& jsonString);
	void ShowLvl(const std::string& jsonString);
	void ShowExp(const std::string& jsonString);
	void ShowHP(const std::string& jsonString);
	void ShowMP(const std::string& jsonString);
	void ShowKey(const std::string& jsonString);
	void ShowBox(const std::string& jsonString);
	void ShowDoor(const std::string& jsonString);
	void ShowRoom(const std::string& jsonString);
	void ShowHall(const std::string& jsonString);
	void ShowScan(const std::string& jsonString);
	void ShowPing(const std::string& jsonString);
	void ShowNet(const std::string& jsonString);
	void ShowIP(const std::string& jsonString);
	void ShowMac(const std::string& jsonString);
	void ShowLoadout(const std::string& jsonString);
	void ShowDrop(const std::string& jsonString);
	void ShowPick(const std::string& jsonString);
	void ShowUse(const std::string& jsonString);
	void ShowCraft(const std::string& jsonString);
	void ShowBuild(const std::string& jsonString);

    CJavaWrapper(JNIEnv* env);
    ~CJavaWrapper();

	jclass edgar;
jclass guiManagerClass;

    jobject activity;
};

extern CJavaWrapper* g_pJavaWrapper;