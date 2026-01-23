#include "CJavaWrapper.h"
#include "../main.h"
extern "C" JavaVM* javaVM;

#include "..//net/CUDPSocket.h"
#include "..//br/CNetwork.h"
extern int g_iServer;
#include "../gtare/CHud.h"
#include "../Radar.h"
#include "../keyboard.h"
#include "../chatwindow.h"
#include "..//settings.h"
#include "..//net/netgame.h"
#include "../game/game.h"
#include "../game/snapshothelper.h"
#include "../str_obfuscator_no_template.hpp"
#include "../game/vehicle.h"
#include "../nlohmann/json.hpp"
#include "voice/CVoiceChatClient.h"
extern CKeyBoard* pKeyBoard;
extern CVoiceChatClient* pVoice;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;
extern CGame* pGame;
extern CVehicle* pVehicle;
extern CSnapShotHelper* pSnapShotHelper;
JNIEnv* CJavaWrapper::GetEnv()
{
	JNIEnv* env = nullptr;
	int getEnvStat = javaVM->GetEnv((void**)& env, JNI_VERSION_1_4);

	if (getEnvStat == JNI_EDETACHED)
	{
		Log("GetEnv: not attached");
		if (javaVM->AttachCurrentThread(&env, NULL) != 0)
		{
			Log("Failed to attach");
			return nullptr;
		}
	}
	if (getEnvStat == JNI_EVERSION)
	{
		Log("GetEnv: version not supported");
		return nullptr;
	}

	if (getEnvStat == JNI_ERR)
	{
		Log("GetEnv: JNI_ERR");
		return nullptr;
	}

	return env;
}

std::string CJavaWrapper::GetClipboardString()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return std::string("");
	}

	jbyteArray retn = (jbyteArray)env->CallObjectMethod(activity, s_GetClipboardText);

	if ((env)->ExceptionCheck())
	{
		(env)->ExceptionDescribe();
		(env)->ExceptionClear();
		return std::string("");
	}

	if (!retn)
	{
		return std::string("");
	}

	jboolean isCopy = true;

	jbyte* pText = env->GetByteArrayElements(retn, &isCopy);
	jsize length = env->GetArrayLength(retn);

	std::string str((char*)pText, length);

	env->ReleaseByteArrayElements(retn, pText, JNI_ABORT);
	
	return str;
}



void CJavaWrapper::CallLauncherActivity(int type)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_CallLauncherActivity, type);

	EXCEPTION_CHECK(env);
}
void CJavaWrapper::AddChatMessage(const char msg[], uint32_t textcolor)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    char msg_utf[1024];
    cp1251_to_utf8(msg_utf, msg);
    //CChatWindow::FilterInvalidChars(msg_utf);
    //
    jstring jmsg = env->NewStringUTF(msg_utf);

    env->CallVoidMethod(activity, addChatMessage, jmsg, textcolor);
    env->DeleteLocalRef(jmsg);
}
void CJavaWrapper::ShowInputLayout()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_ShowInputLayout);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowRadar()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_showradar);

	EXCEPTION_CHECK(env);
}
void CJavaWrapper::HideRadar()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_hideradar);

	EXCEPTION_CHECK(env);
}
#include <string>
#include <codecvt>
#include <locale>
void CJavaWrapper::HideInputLayout()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_HideInputLayout);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowClientSettings()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_ShowClientSettings);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::MakeDialog(int dialogId, int dialogTypeId, char* caption, char* content, char* leftBtnText, char* rightBtnText)
{
    JNIEnv* env = GetEnv();
    if (!env)
    {
	Log("No env");
	return;
    }
    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jstring encoding = env->NewStringUTF("UTF-8");
    jbyteArray bytes = env->NewByteArray(strlen(caption));
    env->SetByteArrayRegion(bytes, 0, strlen(caption), (jbyte*)caption);
    jstring str1 = (jstring)env->NewObject(strClass, ctorID, bytes, encoding);
    //
    jclass strClass1 = env->FindClass("java/lang/String");
    jmethodID ctorID1 = env->GetMethodID(strClass1, "<init>", "([BLjava/lang/String;)V");
    jstring encoding1 = env->NewStringUTF("UTF-8");
    jbyteArray bytes1 = env->NewByteArray(strlen(content));
    env->SetByteArrayRegion(bytes1, 0, strlen(content), (jbyte*)content);
    jstring str2 = (jstring)env->NewObject(strClass1, ctorID1, bytes1, encoding1);
    //
    jclass strClass2 = env->FindClass("java/lang/String");
    jmethodID ctorID2 = env->GetMethodID(strClass2, "<init>", "([BLjava/lang/String;)V");
    jstring encoding2 = env->NewStringUTF("UTF-8");
    jbyteArray bytes2 = env->NewByteArray(strlen(leftBtnText));
    env->SetByteArrayRegion(bytes2, 0, strlen(leftBtnText), (jbyte*)leftBtnText);
    jstring str3 = (jstring)env->NewObject(strClass2, ctorID2, bytes2, encoding2);
    //
    jclass strClass3 = env->FindClass("java/lang/String");
    jmethodID ctorID3 = env->GetMethodID(strClass3, "<init>", "([BLjava/lang/String;)V");
    jstring encoding3 = env->NewStringUTF("UTF-8");
    jbyteArray bytes3 = env->NewByteArray(strlen(rightBtnText));
    env->SetByteArrayRegion(bytes3, 0, strlen(rightBtnText), (jbyte*)rightBtnText);
    jstring str4 = (jstring)env->NewObject(strClass3, ctorID3, bytes3, encoding3);

    env->CallVoidMethod(activity, s_MakeDialog, dialogId, dialogTypeId, str1, str2, str3, str4);

    EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetUseFullScreen(int b)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_SetUseFullScreen, b);

	EXCEPTION_CHECK(env);
}
int currentQuest =  1;
void CJavaWrapper::NextQuestSchool(){
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	/* jclass mainClass = env->FindClass("com/aleshagames/game/gui/drivingschool/UIQuestions");
    jmethodID constructorId = env->GetMethodID(mainClass, "<init>", "()V");
    jobject mainObject = env->NewObject(mainClass, constructorId);
    jmethodID startTest = env->GetMethodID(mainClass, "showNewQuest", "(I)V");
    env->CallVoidMethod(mainObject, startTest, currentQuest + 1);
	*/
	env->CallVoidMethod(activity, s_shownewq, currentQuest + 1);

	EXCEPTION_CHECK(env);
   
}
void CJavaWrapper::ShowEndTest(int i, int y){
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	/* jclass mainClass = env->FindClass("com/aleshagames/game/gui/drivingschool/UIQuestions");
    jmethodID constructorId = env->GetMethodID(mainClass, "<init>", "()V");
    jobject mainObject = env->NewObject(mainClass, constructorId);
    jmethodID startTest = env->GetMethodID(mainClass, "showNewQuest", "(I)V");
    env->CallVoidMethod(mainObject, startTest, currentQuest + 1);
	*/
	//env->CallVoidMethod(activity, s_shownewq, i, y);
	
	/*jclass main = env->FindClass("com/nvidia/devtech/NvEventQueueActivity"); // alternatively: env->GetObjectClass(arg);
	//Log("start test seek....");
	jmethodID startTest = env->GetMethodID(main, "showEndTest", "(II)V"); // int getInt(java.lang.String key)
	//Log("start test call....");
	env->CallVoidMethod(main, startTest, i, y);
	*/
	env->CallVoidMethod(activity, s_showendtest, i, y);

	EXCEPTION_CHECK(env);
   
}
void CJavaWrapper::ShowSchool(int a, int b, int c, int d, int e, int f, int g, int h, int l, int j){
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	jclass jsonArrayClass = env->FindClass("org/json/JSONArray");
    jmethodID jsonArrayConstructor = env->GetMethodID(jsonArrayClass, "<init>", "()V");
    jobject jsonArray = env->NewObject(jsonArrayClass, jsonArrayConstructor);

    jmethodID jsonArrayPutInt = env->GetMethodID(jsonArrayClass, "put", "(I)Lorg/json/JSONArray;");
    env->CallObjectMethod(jsonArray, jsonArrayPutInt, a);
    env->CallObjectMethod(jsonArray, jsonArrayPutInt, b);
    env->CallObjectMethod(jsonArray, jsonArrayPutInt, c);
    env->CallObjectMethod(jsonArray, jsonArrayPutInt, d);
    env->CallObjectMethod(jsonArray, jsonArrayPutInt, e);

    jobject jsonArray2 = env->NewObject(jsonArrayClass, jsonArrayConstructor);
    env->CallObjectMethod(jsonArray2, jsonArrayPutInt, f);
    env->CallObjectMethod(jsonArray2, jsonArrayPutInt, g);
    env->CallObjectMethod(jsonArray2, jsonArrayPutInt, h);
    env->CallObjectMethod(jsonArray2, jsonArrayPutInt, l);
    env->CallObjectMethod(jsonArray2, jsonArrayPutInt, j);

    jclass jsonObjectClass = env->FindClass("org/json/JSONObject");
    jmethodID jsonObjectConstructor = env->GetMethodID(jsonObjectClass, "<init>", "()V");
    jobject jsonObject = env->NewObject(jsonObjectClass, jsonObjectConstructor);

    jmethodID jsonObjectPut = env->GetMethodID(jsonObjectClass, "put", "(Ljava/lang/String;Ljava/lang/Object;)Lorg/json/JSONObject;");
    env->CallObjectMethod(jsonObject, jsonObjectPut, env->NewStringUTF("a"), jsonArray);
    env->CallObjectMethod(jsonObject, jsonObjectPut, env->NewStringUTF("p"), jsonArray2);
	
	//Log("start test create....");
	//jclass main = env->FindClass("com/nvidia/devtech/NvEventQueueActivity"); // alternatively: env->GetObjectClass(arg);
	//Log("start test seek....");
//	jmethodID startTest = env->GetMethodID(main, "showSchool", "(Ljava/lang/Object;)V"); // int getInt(java.lang.String key)
	//Log("start test call....");
	//env->CallVoidMethod(main, startTest, jsonObject);
	
	env->CallVoidMethod(activity, s_showschool, jsonObject);

	EXCEPTION_CHECK(env);
   
}
RwTexture *CJavaWrapper::CreateTextureSnapShot() {
    CRGBA color_white;
    color_white.A = 255;
    color_white.B = 255;
    color_white.G = 255;
    color_white.R = 255;
	VECTOR vec;
		vec.X = 0.0;
		vec.Y = 0.0;
		vec.Z = 0.0;
    RwTexture* texture = (RwTexture*) pSnapShotHelper->CreateVehicleSnapShot(
				466,
				-1,
				&vec,
				1.0f,
				1,
				1
			);
    CSprite2d* m_pSprite2d = new CSprite2d();
    m_pSprite2d->m_pRwTexture = texture;

    m_pRenderTarget->Begin();

    m_pSprite2d->Draw(0,0,texture->raster->height, texture->raster->width, color_white);


    return m_pRenderTarget->End();
}
#include "CSnapShots.h"
#include "CSnapShotWrapper.h"
#include <queue>
extern CSnapShots* pSnapShots;
extern int g_iStatusDriftChanged;
#include "..//debug.h"
extern "C"
{
		JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onInputEnd(JNIEnv* pEnv, jobject thiz, jbyteArray str)
		{
			if (pKeyBoard)
			{
				pKeyBoard->OnNewKeyboardInput(pEnv, thiz, str);
			}
		}
		
		
			JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendTuning(JNIEnv* pEnv, jobject thiz, jint id, jint r, jint g, jint b, jint a)
			{
				if((int)id == 20091){
					pGame->DisplayWidgets(true);
					pChatWindow->showingchat=true;
				}
			/*	if((int)id == 0){
					CVehicle *pVehicle = nullptr;
					CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
					CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
					VEHICLEID id = pVehiclePool->FindIDFromGtaPtr(pPlayerPed->GetGtaVehicle());
					if(id != 0){
						pVehicle = pVehiclePool->GetAt(id);
					pVehicle->mainColor = {static_cast<RwUInt8>(r), static_cast<RwUInt8>(g), static_cast<RwUInt8>(b), 255};}
					return;
				}*/
				pNetGame->SendTuning(id, r, g, b, a); 
			}
			
			
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_buycar(JNIEnv* pEnv, jobject thiz, jint id, jint cost, jint action)
	{
		pNetGame->SendDonateCar(id, cost, action);
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendAuto(JNIEnv* pEnv, jobject thiz, jint autoo)
	{

	    switch(autoo)
			{
				case 0:
				{
					pNetGame->SendChatCommand("/organizationn");
					break;
				}
				case 1:
				{
					pNetGame->SendChatCommand("/vokzalll");
					break;
				}
				case 2:
				{
					pNetGame->SendChatCommand("/mestovihoda");
					break;
				}
				case 3:
				{
					pNetGame->SendChatCommand("/domm");
					break;
				}
				default:
				{
					pNetGame->SendChatCommand("/pon");
					break;
				}

			}
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onInvClick(JNIEnv *env, jobject thiz, jint idslot, jint model)
	{
	        pNetGame->SendInvData(idslot, model);
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onFuelStationClick(JNIEnv *env, jobject thiz, jint fueltype, jint fuelliters)
	{
	        pNetGame->SendFuelData(fuelliters, fueltype);
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_showKey(JNIEnv *env, jobject thiz)
	{
	     //   pChatWindow->Opencc();
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendKs(JNIEnv *env, jobject thiz, jint ction, jint pl)
	{
	        pNetGame->sendKss(ction, pl);
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_hideKey(JNIEnv *env, jobject thiz)
	{
	        pKeyBoard->Close();
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendSpawnRequest(JNIEnv* pEnv, jobject thiz, jint request)
		{
	        switch(request)
			{
				case 0:
				{
					pNetGame->SendChatCommand("/organizationn");
					break;
				}
				case 1:
				{
					pNetGame->SendChatCommand("/vokzalll");
					break;
				}
				case 2:
				{
					pNetGame->SendChatCommand("/mestovihoda");
					break;
				}
				case 3:
				{
					pNetGame->SendChatCommand("/domm");
					break;
				}
			}
		}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendRPC(JNIEnv* pEnv, jobject thiz, jint type, jbyteArray str, jint action)
	{
		switch(type) {
			case 1:
				switch(action) {
					case 398:
						pNetGame->SendChatCommand("/gps");
						break;
					case 1:
						pNetGame->SendChatCommand("/taxi");
						break;
					case 2:
						pNetGame->SendChatCommand("/mn");
						break;
					case 4:
						pNetGame->SendChatCommand("/inv");
						break;
					case 5: {
						pNetGame->SendChatCommand("/anim");
						pNetGame->SendChatCommand("/anim");
						break;
					              }
					case 6:
						pNetGame->SendChatCommand("/donate");
						break;
					case 7:
						pNetGame->SendChatCommand("/car");
						break;
					case 8:
						pNetGame->SendChatCommand("/");
						break;
					}
				case 2:
					switch(action) {
						case 0:
                            pNetGame = new CNetGame(
                                    "188.127.241.74",
                                    1889,
                                    pSettings->GetReadOnly().szNickName,
                                    pSettings->GetReadOnly().szPassword);
                            pSettings->GetWrite().last_server = 0;                                                pChatWindow->playnet = true;
							break;
                        case 1:
                            pNetGame = new CNetGame(
                                    "188.127.241.74",
                                    1889,
                                    pSettings->GetReadOnly().szNickName,
                                    pSettings->GetReadOnly().szPassword);
                            pSettings->GetWrite().last_server = 1;                                                pChatWindow->playnet = true;
                            break;
					}
			}
		}
		 /* env->DeleteLocalRef(keyString);
		 env->DeleteLocalRef(keyInt);
		  env->DeleteLocalRef(valueString);
		  jstring keyInt = env->NewStringUTF("lesha2");
		  jint valueInt = 123;
		  env->CallObjectMethod(jsonObject, putMethod, keyInt, valueInt);*/
		
		
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendJsonData(JNIEnv* env, jobject thiz, jint i, jobject arg) {
		jclass cls = env->FindClass("org/json/JSONObject"); // alternatively: env->GetObjectClass(arg);
		jmethodID method = env->GetMethodID(cls, "getInt", "(Ljava/lang/String;)I"); // int getInt(java.lang.String key)
				
		switch(i){
			case 1: {
				jint val = env->CallIntMethod(arg, method, env->NewStringUTF("t"));
				
				if((int)val == 0){ 
					pNetGame->SendChatCommand("/no");
				}
			}
			case 9: {
				jint val = env->CallIntMethod(arg, method, env->NewStringUTF("t"));
				
				if((int)val == 2){
					jint currentMusic = env->CallIntMethod(arg, method, env->NewStringUTF("b"));
					pNetGame->sendMusic((int)currentMusic);
				}
			}
		
			case 37: {
				jint val = env->CallIntMethod(arg, method, env->NewStringUTF("t"));
				
				if((int)val == 12){
					//Log("start test create....");
					jclass main = env->FindClass("com/nvidia/devtech/NvEventQueueActivity"); // alternatively: env->GetObjectClass(arg);
					//Log("start test seek....");
					jmethodID startTest = env->GetMethodID(main, "startTest", "()V"); // int getInt(java.lang.String key)
					//Log("start test call....");
					env->CallVoidMethod(main, startTest);
				}
				if((int)val == 1){
					currentQuest = (int)env->CallIntMethod(arg, method, env->NewStringUTF("s"));
				}
				if((int)val == 3){
					int answer = (int)env->CallIntMethod(arg, method, env->NewStringUTF("cq"));
					pNetGame->SendAutoSchoolAnswer(currentQuest, answer);
				}
				if((int)val == 52){
					//int answer = (int)env->CallIntMethod(arg, method, env->NewStringUTF("cq"));
					pNetGame->SendAutoSchoolAnswer(52, 52);
				}
				if((int)val > 499){
					//int answer = (int)env->CallIntMethod(arg, method, env->NewStringUTF("cq"));
					pNetGame->SendAutoSchoolAnswer((int)val, (int)val);
				}
			}
		}
	}
		
	// TODO: implement nativeEntitySnapAddToQueue()
		//CSnapShotWrapper::startProcess(image_view, type, modelid, color1, color2, zoom, off_set_x, off_set_y, off_set_z);
		/*static struct QueueItem{
        static uint8_t type;
        static int id;
        static jobject ImageView;
        static VECTOR vecOffset;
        static float zoom;
        static int color1;
        static int color2;
    };
	
	static QueueItem queueItem;
		CSnapShotWrapper::queueMutex.lock();
		Log("StartProcessng");
		CSnapShotWrapper::QueueItem::type = type;
		CSnapShotWrapper::QueueItem::id = modelid;
		CSnapShotWrapper::QueueItem::ImageView = env->NewGlobalRef(image_view);
		CSnapShotWrapper::QueueItem::color1 = color1;
		CSnapShotWrapper::QueueItem::color2 = color2;
		CSnapShotWrapper::QueueItem::zoom = zoom;
		CSnapShotWrapper::QueueItem::vecOffset.X = off_set_x;
		CSnapShotWrapper::QueueItem::vecOffset.Y = off_set_y;
		CSnapShotWrapper::QueueItem::vecOffset.Z = off_set_z;
		CSnapShotWrapper::isProcessing = true;


		CSnapShotWrapper::queueMutex.unlock();*/
	extern "C"
	JNIEXPORT void JNICALL
		Java_com_nvidia_devtech_NvEventQueueActivity_nativeRequestRender(JNIEnv *env, jobject thiz,
																			jobject image_view,
																			jint type, jint modelid,
																			jint color1, jint color2,
																			jfloat zoom,
																			jfloat off_set_x,
																			jfloat off_set_y,
																			jfloat off_set_z) {
																				
		std::unique_lock<std::mutex> lock(CSnapShotWrapper::queueMutex);
		
		CSnapShotWrapper::QueueItem newItem; 

		newItem.type = type;
		newItem.id = modelid;
		newItem.ImageView = env->NewGlobalRef(image_view);
		newItem.color1 = color1;
		newItem.color2 = color2;
		newItem.zoom = zoom;
		newItem.vecOffset.X = off_set_x;
		newItem.vecOffset.Y = off_set_y;
		newItem.vecOffset.Z = off_set_z;

		CSnapShotWrapper::itemQueue.push(newItem); 
		
		env->DeleteLocalRef(image_view);
	}

    JNIEXPORT void JNICALL Java_android_support_v4_os_OnBackPressed_HideKK(JNIEnv *env, jobject thiz)
{
        pKeyBoard->Close();
}
	JNIEXPORT void JNICALL Java_com_blackhub_bronline_game_gui_menuPauseSettingAndMap_mapUI_showMapNative(JNIEnv *env, jobject thiz)
{
       // ((void(*)(/*void* thiz*/))(g_libGTASA + 0x259E50 + 1))();

}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_showHud(JNIEnv* env, jobject thiz)
	{
		pChatWindow->showinghud = true;
		*(uint8_t*)(g_libGTASA+0x8EF36B) = 0;
		if (true)
		{
			CRadar::Enable();
		}
		else
		{
			CRadar::Disable();
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_hideHud(JNIEnv* env, jobject thiz)
	{
		pChatWindow->showinghud = false;
		*(uint8_t*)(g_libGTASA+0x8EF36B) = 1;
		if (false)
		{
			CRadar::Enable();
		}
		else
		{
			CRadar::Disable();
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_SetRadarEnabled(JNIEnv* env, jobject thiz, jboolean tf)
	{
		if (tf)
		{
			CRadar::Enable();
		}
		else
		{
			CRadar::Disable();
		}
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendMessage(JNIEnv *env, jobject thiz, jbyteArray msg)
	{
	    jbyte* pMsg = env->GetByteArrayElements(msg, nullptr);
	    jsize length = env->GetArrayLength(msg);

	    std::string szStr((char*)pMsg, length);

	    pNetGame->SendChatMessage(szStr.c_str());

	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendDialogResponse(JNIEnv* pEnv, jobject thiz, jint i3, jint i, jint i2, jbyteArray str)
	{
		jboolean isCopy = true;

		jbyte* pMsg = pEnv->GetByteArrayElements(str, &isCopy);
		jsize length = pEnv->GetArrayLength(str);

		std::string szStr((char*)pMsg, length);

		if(pNetGame) {
			pNetGame->SendDialogResponse(i, i3, i2, (char*)szStr.c_str());
		}

		pEnv->ReleaseByteArrayElements(str, pMsg, JNI_ABORT);
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendCommand(JNIEnv* pEnv, jobject thiz, jbyteArray str)
	{
		jboolean isCopy = true;

		jbyte* pMsg = pEnv->GetByteArrayElements(str, &isCopy);
		jsize length = pEnv->GetArrayLength(str);

		std::string szStr((char*)pMsg, length);

		if(pNetGame) {
			pNetGame->SendChatCommand((char*)szStr.c_str());
		}

		pEnv->ReleaseByteArrayElements(str, pMsg, JNI_ABORT);
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_togglePlayer(JNIEnv* pEnv, jobject thiz, jint toggle) {
		if(pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()){
			if(toggle)
				pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->TogglePlayerControllable(false);
			else
				pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->TogglePlayerControllable(true);
		}
		
	}
	JNIEXPORT jint JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getLastServer(JNIEnv* pEnv, jobject thiz)
	{
		return (jint)pSettings->GetReadOnly().last_server;
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onEventBackPressed(JNIEnv* pEnv, jobject thiz)
	{
		if (pKeyBoard)
		{
			if (pKeyBoard->IsOpen())
			{
				Log("Closing keyboard");
				pKeyBoard->Close();
			}
		}
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onNativeHeightChanged(JNIEnv* pEnv, jobject thiz, jint orientation, jint height)
	{
		if (pChatWindow)
		{
			pChatWindow->SetLowerBound(height);
		}
	}

	
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_showMenuu(JNIEnv* pEnv, jobject thiz)
	{
		if (pGame->FindPlayerPed()->IsInVehicle() && !pGame->FindPlayerPed()->IsAPassenger())
            pNetGame->SendChatCommand("/ponradial");
        else
            g_pJavaWrapper->ShowMenu();
	}
	
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendRadialClick(JNIEnv* pEnv, jobject thiz, jint id)
	{
		if (!pGame->FindPlayerPed()->IsInVehicle() && !pGame->FindPlayerPed()->IsAPassenger())
            return;
        switch (id) {
        	case 0: pNetGame->SendChatCommand("/key"); break;
            case 1: pNetGame->SendChatCommand("/park"); break;
            case 2: pNetGame->SendChatCommand("/turbo"); break;
            case 3: pNetGame->SendChatCommand("/e"); break;
            case 4: pNetGame->SendChatCommand("/lk"); break;
            case 5: pNetGame->SendChatCommand("/l"); break;
            case 6: pNetGame->SendChatCommand("/panhbihjbih"); break;
            case 7: pNetGame->SendChatCommand("/launch_control"); break;
            case 8: pNetGame->SendChatCommand("/dalekooo"); break;
            case 9: pNetGame->SendChatCommand("/neonnn"); break;
            case 10: pNetGame->SendChatCommand("/strobbb"); break;
            case 11: pNetGame->SendChatCommand("/pnevmo222"); break;
            case 12: pNetGame->SendChatCommand("/pnevmo333"); break;
            case 13: pNetGame->SendChatCommand("/pnevmo111"); break;
            case 14: pNetGame->SendChatCommand("/hydraaa"); break;
            case 15: pNetGame->SendChatCommand("/drifttt"); break;
            case 16: pNetGame->SendChatCommand("/comforttt"); break;
            case 17: pNetGame->SendChatCommand("/plusss"); break;
            case 18: pNetGame->SendChatCommand("/sporttt"); break;
        }
	}
	

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setSky(JNIEnv* pEnv, jobject thiz, jint type, jint r, jint g, jint b)
	{
	    *(uint8_t*)(g_libGTASA+0x8C9588) = r;
		        *(uint8_t*)(g_libGTASA+0x8C94D0) = g;
		        *(uint8_t*)(g_libGTASA+0x8C9418) = b;
		switch(type){
		    case 0:
		        *(uint8_t*)(g_libGTASA+0x8C9360) = r;
		        *(uint8_t*)(g_libGTASA+0x8C92A8) = g;
		        *(uint8_t*)(g_libGTASA+0x8C91F0) = b;
		        break;
		    case 1:
		        *(uint8_t*)(g_libGTASA+0x8C9588) = r;
		        *(uint8_t*)(g_libGTASA+0x8C94D0) = g;
		        *(uint8_t*)(g_libGTASA+0x8C9418) = b;
		        break;}
        (( void (*)())(g_libGTASA+0x3BEFC0+1))();
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendPonClick(JNIEnv* pEnv, jobject thiz, jint id)
	{
		pNetGame->SendPonClic(id);

	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setSettings(JNIEnv* pEnv, jobject thiz, jint id, jint parameter)
	{
		
        switch (id) {
			case 0: *(int32_t *)(g_libGTASA + 0x63E384) = parameter; break;
			case 1: *(int32_t *)(g_libGTASA + 0x63E3C4) = parameter; break;
			case 2: *(int32_t *)(g_libGTASA + 0x63E3A4) = parameter; break;
			case 3: *(int32_t *)(g_libGTASA + 0x63E424) = parameter; break;
			case 4: *(int32_t *)(g_libGTASA + 0x63E464) = parameter; break;
			case 5: *(uint8_t*)(g_libGTASA + 0x008C9BA3) = parameter; break;
		}
	}
	JNIEXPORT jint JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getSettings(JNIEnv* pEnv, jobject thiz, jint id)
	{
		
        switch (id) {
			case 0: return *(int32_t *)(g_libGTASA + 0x63E384);
			case 1: return *(int32_t *)(g_libGTASA + 0x63E3C4); 
			case 2: return *(int32_t *)(g_libGTASA + 0x63E3A4); 
			case 3: return *(int32_t *)(g_libGTASA + 0x63E424);
			case 4: return *(int32_t *)(g_libGTASA + 0x63E464);
			case 5: return *(uint8_t*)(g_libGTASA + 0x008C9BA3);
		}
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendDonateClick(JNIEnv* pEnv, jobject thiz, jint id)
	{
        switch (id) {
        	case 0: pNetGame->SendChatCommand("/openbc"); break;
            	case 1: pNetGame->SendChatCommand("/openrub"); break;
		case 2: pNetGame->SendChatCommand("/keys"); break;
		case 3: pNetGame->SendChatCommand("/bp"); break;
		case 4: pNetGame->SendChatCommand("/nabori"); break;
		case 5: pNetGame->SendChatCommand("/akss"); break;
		case 6: pNetGame->SendChatCommand("/changename"); break;
		case 7: pNetGame->SendChatCommand("/unnwarn"); break;
		case 8: pNetGame->SendChatCommand("/alllic"); break;
		case 9: pNetGame->SendChatCommand("/powerbuy"); break;
		case 10: pNetGame->SendChatCommand("/bomboxbuy"); break;
		case 11: pNetGame->SendChatCommand("/buyslot"); break;
		case 12: pNetGame->SendChatCommand("/buynomber"); break;
		case 13: pNetGame->SendChatCommand("/buyphonenum"); break;
		case 14: pNetGame->SendChatCommand("/vipsilv"); break;
		case 15: pNetGame->SendChatCommand("/vipgld"); break;
		case 16: pNetGame->SendChatCommand("/vippltnum"); break;
	}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSettingsWindowSave(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			pSettings->Save();
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSettingsWindowDefaults(JNIEnv* pEnv, jobject thiz, jint category)
	{
		if (pSettings)
		{
			pSettings->ToDefaults(category);
			if (pChatWindow)
			{
				pChatWindow->m_bPendingReInit = true;
			}
		}
	}
}

void CJavaWrapper::ShowHud()
{
    pChatWindow->showinghud = true;pChatWindow->showingchat = true;
		*(uint8_t*)(g_libGTASA+0x8EF36B) = 0;
		if (true)
		{
			CRadar::Enable();
		}
		else
		{
			CRadar::Disable();
		}
}
void CJavaWrapper::showKs()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    //g_pJavaWrapper->ShowNotification(4, "Худ успешно подключен", 5, "", ">>");
    env->CallVoidMethod(this->activity, this->s_showKs);
}
void CJavaWrapper::showMusic(){
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	
	env->CallVoidMethod(this->activity, this->s_showMusic);
}	
	/*radiosList.put("t", 0); //page 0 - radio, 1 - library
	radiosList.put("nu", 4); //num musics (only < 4)
	radiosList.put("p", 1); //page
	radiosList.put("b", 1); //butt status 1
	radiosList.put("a", 1); //isPlaying
	radiosList.put("e", 0); //0 ? isActiveEXP
	radiosList.put("n", "Record "); //current playing
	radiosList.put("r0", "Record"); // 1 music
	radiosList.put("r1", "Enegry"); // 2 music
	radiosList.put("r2", "Авторадио"); // 3 music
	radiosList.put("r3", "Eвропа+"); // 4 music*/
void CJavaWrapper::sendMusicJson(int page1, int num, int page2, int but, int isPlay, int currentMusic){
    JNIEnv* env = GetEnv();

    if (!env)
    {
        Log("No env");
        return;
    }
	
	char pon[5];

    jclass jsonObjectClass = env->FindClass("org/json/JSONObject");
    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "()V");
    jobject jsonObject = env->NewObject(jsonObjectClass, constructor);
    jmethodID putMethod = env->GetMethodID(jsonObjectClass, "put", "(Ljava/lang/String;Ljava/lang/Object;)Lorg/json/JSONObject;");
	jclass integerClass = env->FindClass("java/lang/Integer");
	jmethodID integerCtor = env->GetMethodID(integerClass, "<init>", "(I)V");

    jobject localRef = env->NewGlobalRef(jsonObject);

    env->CallObjectMethod(jsonObject, putMethod, env->NewStringUTF("t"), env->NewObject(integerClass, integerCtor, page1));
	env->CallObjectMethod(jsonObject, putMethod, env->NewStringUTF("nu"), env->NewObject(integerClass, integerCtor, num));
	env->CallObjectMethod(jsonObject, putMethod, env->NewStringUTF("p"), env->NewObject(integerClass, integerCtor, page2));
	env->CallObjectMethod(jsonObject, putMethod, env->NewStringUTF("b"), env->NewObject(integerClass, integerCtor, but));
	env->CallObjectMethod(jsonObject, putMethod, env->NewStringUTF("a"), env->NewObject(integerClass, integerCtor, isPlay));
	env->CallObjectMethod(jsonObject, putMethod, env->NewStringUTF("e"), env->NewObject(integerClass, integerCtor, 0));
    env->CallObjectMethod(localRef, putMethod, env->NewStringUTF("r0"), env->NewStringUTF("Record"));
    env->CallObjectMethod(localRef, putMethod, env->NewStringUTF("r1"), env->NewStringUTF("Energy"));
    env->CallObjectMethod(localRef, putMethod, env->NewStringUTF("r2"), env->NewStringUTF("Авторадио"));
    env->CallObjectMethod(localRef, putMethod, env->NewStringUTF("r3"), env->NewStringUTF("Европа+"));

    jmethodID method2 = env->GetMethodID(jsonObjectClass, "getString", "(Ljava/lang/String;)Ljava/lang/String;");
	if(currentMusic != 999){
		sprintf(pon, "r%d", currentMusic);
		jstring strVal = (jstring) env->CallObjectMethod(jsonObject, method2, env->NewStringUTF(pon));
		env->CallObjectMethod(localRef, putMethod, env->NewStringUTF("n"), strVal);
	}else{
		env->CallObjectMethod(localRef, putMethod, env->NewStringUTF("n"), env->NewStringUTF(""));
	}
	
	
    env->CallVoidMethod(this->activity, this->s_sendMusicData, localRef);

    env->DeleteGlobalRef(localRef);
}

void CJavaWrapper::hideKs()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    //g_pJavaWrapper->ShowNotification(4, "Худ успешно подключен", 5, "", ">>");
    env->CallVoidMethod(this->activity, this->s_hideKs);
}
void CJavaWrapper::HideHud()
{
    pChatWindow->showinghud = false;pChatWindow->showingchat = false;
		*(uint8_t*)(g_libGTASA+0x8EF36B) = 1;
		if (false)
		{
			CRadar::Enable();
		}
		else
		{
			CRadar::Disable();
		}
}



void CJavaWrapper::SetPauseState(bool a1)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_setPauseState, a1);
}

void CJavaWrapper::ShowSplash() {

	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(this->activity, this->s_showSplash);
}

void CJavaWrapper::UpdateSplash(int percent) {
	
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(this->activity, this->s_updateSplash, percent);
}
void CJavaWrapper::sendDonateNew(int access1, int access2) {
	
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(this->activity, this->s_sendDonateNew, access1, access2);
}
void CJavaWrapper::ShowMenu() 
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
                  //g_pJavaWrapper->ShowNotification(4, "Test game version", 5, "", ">>"); 
	env->CallVoidMethod(this->activity, this->s_showMenu);
}


void CJavaWrapper::ShowRadial(bool park, bool key, bool doors, bool lights, bool suspension, bool launch_control, bool engine, bool turbo)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(this->activity, this->s_showRadial,  park,  key,  doors,  lights,  suspension, launch_control,  engine,  turbo);
}
void CJavaWrapper::SendConnect(int text)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(this->activity, this->s_sendConnect, text);
}
void CJavaWrapper::ShowNotification(int type, char* text, int duration, char* actionforBtn, char* textBtn) 
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	jclass strClass = env->FindClass("java/lang/String");
                  jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
                  jstring encoding = env->NewStringUTF("UTF-8");

                  jbyteArray bytes = env->NewByteArray(strlen(text));
                  env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);
                  jstring jtext = (jstring) env->NewObject(strClass, ctorID, bytes, encoding);

	bytes = env->NewByteArray(strlen(actionforBtn));
                  env->SetByteArrayRegion(bytes, 0, strlen(actionforBtn), (jbyte*)actionforBtn);
                  jstring jactionforBtn = (jstring) env->NewObject(strClass, ctorID, bytes, encoding);

	bytes = env->NewByteArray(strlen(textBtn));
                  env->SetByteArrayRegion(bytes, 0, strlen(textBtn), (jbyte*)textBtn);
                  jstring jtextBtn = (jstring) env->NewObject(strClass, ctorID, bytes, encoding);

	env->CallVoidMethod(this->activity, this->s_showNotification, type, jtext, duration, jactionforBtn, jtextBtn);
}

void CJavaWrapper::UpdateAuto(char* text0, char* text1, char* text2, char* text3) 
{
  JNIEnv* env = GetEnv();

  if (!env)
  {
    Log("No env");
    return;
  }

	jclass strClass = env->FindClass("java/lang/String");
	jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
	jstring encoding = env->NewStringUTF("UTF-8");

	jbyteArray bytes = env->NewByteArray(strlen(text0));
	env->SetByteArrayRegion(bytes, 0, strlen(text0), (jbyte*)text0);
	jstring jtext0 = (jstring) env->NewObject(strClass, ctorID, bytes, encoding);

	bytes = env->NewByteArray(strlen(text1));
	env->SetByteArrayRegion(bytes, 0, strlen(text1), (jbyte*)text1);
	jstring jtext1 = (jstring) env->NewObject(strClass, ctorID, bytes, encoding);
	
	bytes = env->NewByteArray(strlen(text2));
	env->SetByteArrayRegion(bytes, 0, strlen(text2), (jbyte*)text2);
	jstring jtext2 = (jstring) env->NewObject(strClass, ctorID, bytes, encoding);
	
	bytes = env->NewByteArray(strlen(text3));
	env->SetByteArrayRegion(bytes, 0, strlen(text3), (jbyte*)text3);
	jstring jtext3 = (jstring) env->NewObject(strClass, ctorID, bytes, encoding);

  env->CallVoidMethod(this->activity, this->s_updateAuto, jtext0, jtext1, jtext2, jtext3);
}
void CJavaWrapper::setStat(int id, char* name, int score, int ping)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jstring encoding = env->NewStringUTF("UTF-8");

    jbyteArray bytes = env->NewByteArray(strlen(name));
    env->SetByteArrayRegion(bytes, 0, strlen(name), (jbyte*)name);
    jstring jname = (jstring) env->NewObject(strClass, ctorID, bytes, encoding);

	env->CallVoidMethod(this->activity, this->s_setStat, id, jname, score, ping);
}

void CJavaWrapper::closekey()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->closekeY);
}
void CJavaWrapper::MenuHud(){
	if (pGame->FindPlayerPed()->IsInVehicle() && !pGame->FindPlayerPed()->IsAPassenger())
            pNetGame->SendChatCommand("/ponradial");
	else
		g_pJavaWrapper->ShowMenu();
}
void CJavaWrapper::showInv()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_showInv);
}
void CJavaWrapper::ShowTuning(int type, char* text, int money)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	jclass strClass = env->FindClass("java/lang/String");
	jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
	jstring encoding = env->NewStringUTF("UTF-8");

	jbyteArray bytes = env->NewByteArray(strlen(text));
	env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);
	jstring jtext = (jstring) env->NewObject(strClass, ctorID, bytes, encoding);
	env->CallVoidMethod(this->activity, this->s_showTuning, type, jtext, money);

	pGame->DisplayWidgets(false);
	pChatWindow->showingchat=false;
}
void CJavaWrapper::ShowDonate(int money, int bc)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_showDonate, money, bc);
}
void CJavaWrapper::UpdateDonate(int money, int bc)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_updateDonate, money, bc);
}
void CJavaWrapper::ShowFuelStation(int type, int price1, int price2, int price3, int price4, int price5, int maxCount)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(this->activity, this->s_showFuelStation, type, price1, price2, price3, price4, price5, maxCount);
}
void CJavaWrapper::ShowSpawnSelector()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

    env->CallVoidMethod(this->activity, this->s_showSpawnSelector);
}
void CJavaWrapper::ShowTabb()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

    env->CallVoidMethod(this->activity, this->s_showTabb);
}
void CJavaWrapper::SetAnswers(int i, int i2)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

    env->CallVoidMethod(this->activity, this->s_setanswers, i, i2);
}
void CJavaWrapper::ShowAuto()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

    env->CallVoidMethod(this->activity, this->s_showAuto);
}
void CJavaWrapper::HideAuto()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

    env->CallVoidMethod(this->activity, this->s_hideAuto);
}

void CJavaWrapper::updInv(int ction, int id, int modelId, int dr, int col)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}


    env->CallVoidMethod(this->activity, this->s_updInv,  ction,  id,  modelId,  dr,  col);
}
void CJavaWrapper::show_sc(int money, int bc)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_show_sc, money, bc);
}
CJavaWrapper::CJavaWrapper(JNIEnv* env, jobject activity)
{
	this->activity = env->NewGlobalRef(activity);

	jclass nvEventClass = env->GetObjectClass(activity);
	if (!nvEventClass)
	{
		Log("nvEventClass null");
		return;
	}

	//s_CallLauncherActivity = env->GetMethodID(nvEventClass, "callLauncherActivity", "(I)V");
    
	
	
	s_GetClipboardText = env->GetMethodID(nvEventClass, "getClipboardText", "()[B");
	s_ShowInputLayout = env->GetMethodID(nvEventClass, "showInputLayout", "()V");
	s_HideInputLayout = env->GetMethodID(nvEventClass, "hideInputLayout", "()V");

	s_ShowClientSettings = env->GetMethodID(nvEventClass, "showClientSettings", "()V");
	s_SetUseFullScreen = env->GetMethodID(nvEventClass, "setUseFullscreen", "(I)V");
    addChatMessage = env->GetMethodID(nvEventClass, "addChatMessage", "(Ljava/lang/String;I)V");
    closekeY = env->GetMethodID(nvEventClass, "closekey", "()V");
	s_MakeDialog = env->GetMethodID(nvEventClass, "showDialog", "(IILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");


                

                  s_showradar = env->GetMethodID(nvEventClass, "showradar", "()V");
                  s_hideradar = env->GetMethodID(nvEventClass, "hideradar", "()V");

                  s_showZona = env->GetMethodID(nvEventClass, "showZona", "()V");
                  s_hideZona = env->GetMethodID(nvEventClass, "hideZona", "()V");

                  s_showx2 = env->GetMethodID(nvEventClass, "showx2", "()V");
                  s_hidex2 = env->GetMethodID(nvEventClass, "hidex2", "()V");

	s_showNotification = env->GetMethodID(nvEventClass, "showNotification","(ILjava/lang/String;ILjava/lang/String;Ljava/lang/String;)V");
	s_setStat = env->GetMethodID(nvEventClass, "setStat","(ILjava/lang/String;II)V");
	s_sendConnect = env->GetMethodID(nvEventClass, "sendConnect","(I)V");
	s_showMenu = env->GetMethodID(nvEventClass, "showMenu", "()V");

	s_setPauseState = env->GetMethodID(nvEventClass, "setPauseState", "(Z)V");

	s_updateSplash = env->GetMethodID(nvEventClass, "updateSplash", "(I)V");
	s_setanswers = env->GetMethodID(nvEventClass, "mdoo", "(II)V");
	s_showSplash = env->GetMethodID(nvEventClass, "showSplash", "()V");
	
	
	s_showKs = env->GetMethodID(nvEventClass, "showKs", "()V");
	s_hideKs = env->GetMethodID(nvEventClass, "hideKs", "()V");
	
	s_showDonate = env->GetMethodID(nvEventClass, "showDonate", "(II)V");
	s_showTuning = env->GetMethodID(nvEventClass, "showTuning", "(ILjava/lang/String;I)V");
	s_showInv = env->GetMethodID(nvEventClass, "showInv", "()V");
	
	s_updateDonate = env->GetMethodID(nvEventClass, "updateDonate", "(II)V");
	s_show_sc = env->GetMethodID(nvEventClass, "show_sc", "(II)V");
	s_showFuelStation = env->GetMethodID(nvEventClass, "showFuelStation", "(IIIIIII)V");
	s_showSpawnSelector = env->GetMethodID(nvEventClass, "showSpawnSelector", "()V");
	s_showTabb = env->GetMethodID(nvEventClass, "showTabb", "()V");
	s_showAuto = env->GetMethodID(nvEventClass, "showAuto", "()V");
	s_hideAuto = env->GetMethodID(nvEventClass, "hideAuto", "()V");
	s_shownewq = env->GetMethodID(nvEventClass, "showNewQuest", "(I)V");
	s_updateAuto = env->GetMethodID(nvEventClass, "updateAuto", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	s_updInv = env->GetMethodID(nvEventClass, "updInv", "(IIIII)V");
	s_sendDonateNew = env->GetMethodID(nvEventClass, "sendDonateNew", "(II)V");
	s_showRadial = env->GetMethodID(nvEventClass, "showRadial", "(ZZZZZZZZ)V");
	s_sendMusicData = env->GetMethodID(nvEventClass, "sendMusicData", "(Lorg/json/JSONObject;)V");
	s_showMusic = env->GetMethodID(nvEventClass, "showMusic", "()V");
	s_showendtest = env->GetMethodID(nvEventClass, "startEndTest", "(II)V");
	s_showschool = env->GetMethodID(nvEventClass, "showSchool", "(Lorg/json/JSONObject;)V");

	env->DeleteLocalRef(nvEventClass);
}

CJavaWrapper::~CJavaWrapper()
{
	JNIEnv* pEnv = GetEnv();
	if (pEnv)
	{
		pEnv->DeleteGlobalRef(this->activity);
	}
}

CJavaWrapper* g_pJavaWrapper = nullptr;
