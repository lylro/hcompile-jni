#include <jni.h>
#include <android/log.h>
#include <ucontext.h>
#include <pthread.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
#include <unistd.h> // system api
#include <sys/mman.h> // sysm()
#include <assert.h> // assert()
#include <dlfcn.h> // dlopen

static bool testbuild = true; 

#include "main.h"
#include "util/patch.h"
#include "game/game.h"
#include "game/RW/RenderWare.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "chatwindow.h"
#include "playertags.h"
#include "dialog.h"
#include "keyboard.h"
#include "settings.h"
#include "util/CClientJava.h"
#include "scoreboard.h"
#include "CAudioStream.h"
#include "util/armhook.h"
#include "game/ccheckfilehash.h"
#include "str_obfuscator_no_template.hpp"

#include "voice/CVoiceChatClient.h"
#include "crashlytics.h"

#include "gtare/CHud.h"

#include "util/CClientJava.h"
#include "vendor/bass/bass.h"
#include "gui/CFontRenderer.h"
#include "util/CJavaWrapper.h"
#include "cryptors/INITSAMP_result.h"
#include "debug.h"

uintptr_t g_libGTASA = 0;
uintptr_t g_libBR = 0;
uintptr_t g_libSTUFF = 0;
const char* g_pszStorage = nullptr;
JNIEnv* env = nullptr;
#include "clientlogic.h"
#include "TurnLeftAndRight.h"
#include "br/CNetwork.h"
#include "br/CLocalisation.h"

#include "gtare/CSkyBox.h"
#include "gtare/CRQ_Commands.h"

const cryptor::string_encryptor encLib = cryptor::create("libtwilight.so", 11);
void CrashLog(const char* fmt, ...);
CGame *pGame = nullptr;
CNetGame *pNetGame = nullptr;
CChatWindow *pChatWindow = nullptr; 
CPlayerTags *pPlayerTags = nullptr;
CDialogWindow *pDialogWindow = nullptr;
CVoiceChatClient* pVoice = nullptr;
CSnapShotHelper* pSnapShotHelper = nullptr;
CScoreBoard* pScoreBoard = nullptr;
CAudioStream* pAudioStream = nullptr;
CGUI *pGUI = nullptr;
CKeyBoard *pKeyBoard = nullptr;
CSettings *pSettings = nullptr;
CClientLogic *pClientLogic = nullptr;
CTurnLeftAndRight *pTurnLeftAndRight = nullptr;
void InitHookStuff();
void InstallSpecialHooks();
void InitRenderWareFunctions();
void ApplyInGamePatches();
void ApplyPatches_level0();
void MainLoop();
char g_iLastBlock[512];
void PrintBuildInfo()
{
	Log(OBFUSCATE("Initializing zalupa: 0"));
}
#include "CLoader.h"
#ifdef GAME_EDITION_CR
extern uint16_t g_usLastProcessedModelIndexAutomobile;
extern int g_iLastProcessedModelIndexAutoEnt;
#endif

extern int g_iLastProcessedSkinCollision;
extern int g_iLastProcessedEntityCollision;
extern int g_iLastRenderedObject;
extern char g_bufRenderQueueCommand[200];
extern uintptr_t g_dwRenderQueueOffset;

void PrintBuildCrashInfo()
{
	CrashLog(OBFUSCATE("Версия клиента: %s"), _VERSION);

	CrashLog(OBFUSCATE("Последний использованный объект: %d"), g_iLastRenderedObject);

                  if(g_iLastProcessedEntityCollision == 228) 
	                  CrashLog(OBFUSCATE("Последнее использованное авто: 0"));
	else
                                  	CrashLog(OBFUSCATE("Последнее использованное авто: %d"), g_iLastProcessedEntityCollision);

                  if(g_iLastProcessedSkinCollision == 228) 
	                  CrashLog(OBFUSCATE("Последний использованный скин: 0"));
	else
                                    CrashLog(OBFUSCATE("Последний использованный скин: %d"), g_iLastProcessedSkinCollision);
}
void  (*RQ_Command_rqTextureMipMode)(int **a1);
void  RQ_Command_rqTextureMipMode_hook(int **a1)
{
    int v1; // r3
    int v2; // r6
    GLuint v3; // r4
   // LOGBR(__FUNCTION__ );
    v1 = **a1;
    *a1 += 2;
    v2 = g_libGTASA;
    v3 = *(uintptr_t *)(v1 + 40);
    if ( *(uintptr_t *)(g_libGTASA + 0x5D2468) != 5 )
    {
        glActiveTexture(0x84C5u);
        *(uintptr_t *)(v2 + 0x5D2468) = 5;
    }
    glBindTexture(0xDE1u, v3);
    *(uintptr_t *)(g_libGTASA + 0x5D245C) = v3;
    glTexParameteri(0xDE1u, 0x2801u, 9729);
    glTexParameteri(0xDE1u, 0x2800u, 9729);
}
jint (*JNI_OnLoadBR)(JavaVM *vm, void *reserved);
jint JNI_OnLoadBR_hook(JavaVM *vm, void *reserved){
	return 0;
}
void printPovtor();
void InitSAMP(JNIEnv* pEnv, jobject thiz/*, jstring storage*/);
extern "C"
{
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_initSAMPg(JNIEnv* pEnv, jobject thiz/*, jstring storage*/)
	{
		
	/*	env = pEnv;
		if (env != nullptr) {
		 jclass systemClass = env->FindClass("java/lang/System");

         jmethodID loadLibraryMethodID = env->GetStaticMethodID(systemClass, "loadLibrary", "(Ljava/lang/String;)V"); 
         env->CallStaticVoidMethod(systemClass, loadLibraryMethodID, env->NewStringUTF("samp"));
                
	     g_libBR = FindLibrary(OBFUSCATE("libsamp.so"));
		// NOP(g_libBR + 0x02B3E6, 46);
		// SetUpHook(g_libBR + 0x02B2BC, (uintptr_t)JNI_OnLoadBR_hook, (uintptr_t*)& JNI_OnLoadBR);
		 Log("Succefully hooked!");
		}*/
		InitSAMP(pEnv, thiz/*, storage*/);
	}
}

void InitBASSFuncs();
void InitSAMP(JNIEnv* pEnv, jobject thiz/*, jstring storage*/)
{
	PROTECT_CODE_INITSAMP;
	
	static FILE* flLogCrash = nullptr;

	PrintBuildInfo();

	InitBASSFuncs();
	BASS_Init(-1, 44100, BASS_DEVICE_3D, 0, NULL);      
                
	// g_pszStorage = (const char*)(SA_ADDR(0x63C4B8));
	// g_pszStorage = OBFUSCATE("/storage/emulated/0/KirillVovk/");
	

   
	
	char buffer2[100]; // Буфер для хранения пути к файлу

    if (flLogCrash == nullptr && g_pszStorage != nullptr) {
        sprintf(buffer2, "%sloader/blackrussia_crash.log", g_pszStorage);
        flLogCrash = fopen(buffer2, "w"); // Открываем файл для записи, этот режим "w" автоматически очищает содержимое файла
    }

    if (flLogCrash != nullptr) {
        memset(buffer2, 0, sizeof(buffer2)); // Обнуляем буфер

       // std::cout << "Файл " << buffer << " успешно очищен." << std::endl;

        fclose(flLogCrash); // Закрываем файл после использования
    } else {
     //   std::cout << "Не удалось открыть файл для очистки." << std::endl;
    }

	pSettings = new CSettings();

//                ARTEFACTS
	if (!*(uintptr_t *)(g_libGTASA + 0x61B298) && !((int (*)(const char *))(g_libGTASA + 0x179A20))("glAlphaFuncQCOM") )
  	{
   	                  CHook::NOP(g_libGTASA + 0x1A6164, 4);
    	                  CHook::WriteMemory(g_libGTASA + 0x1A6164, "\x70\x47", 2);
  	}

	CWeaponsOutFit::SetEnabled(0);
	CRadarRect::SetEnabled(0);
	CGame::SetEnabledPCMoney(pSettings->GetReadOnly().iPCMoney);
	CDebugInfo::SetDrawFPS(pSettings->GetReadOnly().iFPSCounter);
	CInfoBarText::SetEnabled(0);
	CSnow::SetCurrentSnow(pSettings->GetReadOnly().iSnow);

	g_pJavaWrapper = new CJavaWrapper(pEnv, thiz);
	g_pJavaWrapper->SetUseFullScreen(pSettings->GetReadOnly().iCutout);
	
	

	CLocalisation::Initialise(OBFUSCATE("ru.lc"));

	firebase::crashlytics::SetUserId(pSettings->GetReadOnly().szNickName);

	CWeaponsOutFit::ParseDatFile();

	if(!CCheckFileHash::IsFilesValid())
	{
		CClientInfo::bSAMPModified = false;
		return;
	}
	JMPCode(g_libGTASA + 0x1BDDAA, g_libGTASA + 0x1BDE4A);
	NOP(g_libGTASA + 0x1BDA64, 3);

    
   SetUpHook(g_libGTASA + 0x1A6994,(uintptr_t)RQ_Command_rqTextureMipMode_hook,(uintptr_t*)&RQ_Command_rqTextureMipMode);
}

extern CSnapShotHelper* pSnapShotHelper;
void ProcessCheckForKeyboard();
void GTASAInit();
void InitInMenu()
{
                  GTASAInit();

	pGame = new CGame();
	pGame->InitInMenu();

	pGUI = new CGUI();
	pKeyBoard = new CKeyBoard();
	pChatWindow = new CChatWindow();
	pPlayerTags = new CPlayerTags();
	pDialogWindow = new CDialogWindow();
	pScoreBoard = new CScoreBoard();
	pSnapShotHelper = new CSnapShotHelper();
	pAudioStream = new CAudioStream();

	ProcessCheckForKeyboard();

	CFontRenderer::Initialise();
	
	pChatWindow->AddDebugMessageNonFormatted(CLocalisation::GetMessage(E_MSG::CONNECTING));
	//CHud::Initialise();
}

bool unique_library_handler(const char* library)
{
	Dl_info info;
	if (dladdr(__builtin_return_address(0), &info) != 0)
	{
		void* current_library_addr = dlopen(info.dli_fname, RTLD_NOW);
		if (!current_library_addr)
			return false;

		if (dlopen(library, RTLD_NOW) != current_library_addr)
			return false;
	}
	return true;
}

void ProcessCheckForKeyboard()
{
	if (pSettings->GetReadOnly().iAndroidKeyboard)
	{
		pKeyBoard->EnableNewKeyboard();
	}
	else
	{
		pKeyBoard->EnableOldKeyboard();
	}
}

void ObfuscatedForceExit3()
{
       /* todo */	
}

void NewObjectAdd(int iModel, float fPosX, float fPosY, float fPosZ, float vecRotX, float vecRotY, float vecRotZ, float fDrawDistance){
	VECTOR vecRot = {vecRotX, vecRotY, vecRotZ};
	pGame->NewObject( iModel,  fPosX,  fPosY,  fPosZ,  vecRot,  fDrawDistance);
}
void loadAllObjects(){
	
}
int g_iServer = 0;
#include "net/CUDPSocket.h"
void InitInGame()
{
	static bool bGameInited = false;
	static bool bNetworkInited = false;
	
	if (!bGameInited)
	{
		pGame->InitInGame();
		pGame->SetMaxStats();
                                    pGame->SetWorldWeather(0); // сетаем погоду
		pGame->DisplayWidgets(false); // показываем кнопки
		pGame->ToggleRadar(false); // скрываем радар

                                    if(testbuild)
                                    {
			//g_pJavaWrapper->AddTestServer("тест сервер ZBT");
			//g_pJavaWrapper->AddTestServer("тест сервер ретиз паблик");
			//g_pJavaWrapper->AddTestServer("тест сервер вызов с jni");

              //                                        Log("!!!!!!!!!!!!!!!!!! Add test server !!!!!!!!!!!!!!!!!!!!");
                //                                      Log("!!!!!!!!!!!!!!!!!! Add test server !!!!!!!!!!!!!!!!!!!!");
                //                                      Log("!!!!!!!!!!!!!!!!!! Add test server !!!!!!!!!!!!!!!!!!!!");
                                    }

		g_pJavaWrapper->UpdateSplash(111); // обновляем прогресс загрузки

		CPlayerPed *pPlayer = pGame->FindPlayerPed();
	                  CCamera *pCamera = pGame->GetCamera();
                                    pCamera->SetPosition(426.4, 241.377, 12.97, 0.0f, 0.0f, 0.0f);
			pCamera->LookAtPoint(391.5, 281.755, 18.0, 2);

		//pClientLogic = new CClientLogic();
		bGameInited = true;
		printPovtor();
		return;
	}
	

	if (!bNetworkInited)
	{
		bNetworkInited = true;
		return;
	}
	
	
}

void (*CTimer__StartUserPause)();
void CTimer__StartUserPause_hook()
{
	// process pause event
	if (g_pJavaWrapper)
	{
		if (pKeyBoard)
		{
			if (pKeyBoard->IsNewKeyboard())
			{
				pKeyBoard->Close();
			}
		}
		g_pJavaWrapper->SetPauseState(true);
	}

	*(uint8_t*)(g_libGTASA + 0x008C9BA3) = 1;
}

void (*CTimer__EndUserPause)();
void CTimer__EndUserPause_hook()
{
	// process resume event
	if (g_pJavaWrapper)
	{
		g_pJavaWrapper->SetPauseState(false);
	}

	*(uint8_t*)(g_libGTASA + 0x008C9BA3) = 0;
}

#include "debug.h"
void MainLoop()
{
	InitInGame();

	if(pNetGame) pNetGame->Process();

	if (pNetGame)
	{
		if (pNetGame->GetPlayerPool())
		{
			if (pNetGame->GetPlayerPool()->GetLocalPlayer())
			{
				CVehicle* pVeh = pNetGame->GetVehiclePool()->GetAt(pNetGame->GetPlayerPool()->GetLocalPlayer()->m_CurrentVehicle);
				if (pVeh)
				{
					VECTOR vec;
					pVeh->GetMoveSpeedVector(&vec);
					CDebugInfo::ProcessSpeedMode(&vec);
				}
			}
		}
	}

	if(pClientLogic) pClientLogic->ProcessLogic();
}
extern int g_iLastRenderedObject;

void PrintSymbols(void* pc, void* lr)
{
	Dl_info info_pc, info_lr;
	if (dladdr(pc, &info_pc) != 0)
	{
		CrashLog(OBFUSCATE("PC: %s"), info_pc.dli_sname);
	}
	if (dladdr(lr, &info_lr) != 0)
	{
		CrashLog(OBFUSCATE("LR: %s"), info_lr.dli_sname);
	}
}


struct sigaction act_old;
struct sigaction act1_old;
struct sigaction act2_old;
struct sigaction act3_old;
void handler3(int signum, siginfo_t* info, void* contextPtr)
{
	ucontext* context = (ucontext_t*)contextPtr;

	if (act3_old.sa_sigaction)
	{
		act3_old.sa_sigaction(signum, info, contextPtr);
	}

	if (info->si_signo == SIGBUS)
	{
		int crashId = (int)rand() % 20000;
		CrashLog(" ");
		PrintBuildCrashInfo();
		CrashLog("ID: %d", crashId);

		CrashLog("ADDR:");
		CrashLog("1: libGTASA.so + 0x%X", context->uc_mcontext.arm_pc - g_libGTASA);
		CrashLog("2: libGTASA.so + 0x%X", context->uc_mcontext.arm_lr - g_libGTASA);
		CrashLog("1: libtwilight.so + 0x%X", context->uc_mcontext.arm_pc - FindLibrary("libtwilight.so"));
		CrashLog("2: libtwilight.so + 0x%X", context->uc_mcontext.arm_lr - FindLibrary("libtwilight.so"));
		exit(0);
	}

	return;
}
void handler(int signum, siginfo_t *info, void* contextPtr)
{
	ucontext* context = (ucontext_t*)contextPtr;

	if (act_old.sa_sigaction)
	{
		act_old.sa_sigaction(signum, info, contextPtr);
	}

	if(info->si_signo == SIGSEGV)
	{

		int crashId = (int)rand() % 20000;
		CrashLog(" ");
		PrintBuildCrashInfo();
		CrashLog("ID(2): %d", crashId);

		CrashLog("ADDR:");
		CrashLog("1: libGTASA.so + 0x%X", context->uc_mcontext.arm_pc - g_libGTASA);
		CrashLog("2: libGTASA.so + 0x%X", context->uc_mcontext.arm_lr - g_libGTASA);

		CrashLog("1: libtwilight.so + 0x%X", context->uc_mcontext.arm_pc - FindLibrary("libtwilight.so"));
		CrashLog("2: libtwilight.so + 0x%X", context->uc_mcontext.arm_lr - FindLibrary("libtwilight.so"));
		exit(0);
	}

	return;
}

void handler2(int signum, siginfo_t* info, void* contextPtr)
{
	ucontext* context = (ucontext_t*)contextPtr;

	if (act2_old.sa_sigaction)
	{
		act2_old.sa_sigaction(signum, info, contextPtr);
	}

	if (info->si_signo == SIGFPE)
	{

		int crashId = (int)rand() % 20000;
		CrashLog(" ");
		PrintBuildCrashInfo();
		CrashLog("ID(3): %d", crashId);

		CrashLog("ADDR:");
		CrashLog("1: libGTASA.so + 0x%X", context->uc_mcontext.arm_pc - g_libGTASA);
		CrashLog("2: libGTASA.so + 0x%X", context->uc_mcontext.arm_lr - g_libGTASA);

		CrashLog("1: libtwilight.so + 0x%X", context->uc_mcontext.arm_pc - FindLibrary("libtwilight.so"));
		CrashLog("2: libtwilight.so + 0x%X", context->uc_mcontext.arm_lr - FindLibrary("libtwilight.so"));

		exit(0);
	}

	return;
}

void handler1(int signum, siginfo_t* info, void* contextPtr)
{
	ucontext* context = (ucontext_t*)contextPtr;

	if (act1_old.sa_sigaction)
	{
		act1_old.sa_sigaction(signum, info, contextPtr);
	}

	if (info->si_signo == SIGABRT)
	{

		int crashId = (int)rand() % 20000;
		CrashLog(" ");
		PrintBuildCrashInfo();
		CrashLog("ID(5): %d", crashId);

		CrashLog("ADDR:");
		CrashLog("1: libGTASA.so + 0x%X", context->uc_mcontext.arm_pc - g_libGTASA);
		CrashLog("2: libGTASA.so + 0x%X", context->uc_mcontext.arm_lr - g_libGTASA);

		CrashLog("1: libtwilight.so + 0x%X", context->uc_mcontext.arm_pc - FindLibrary("libtwilight.so"));
		CrashLog("2: libtwilight.so + 0x%X", context->uc_mcontext.arm_lr - FindLibrary("libtwilight.so"));

		exit(0);
	}

	return;
}

extern "C"
{
	JavaVM* javaVM = NULL;
	JavaVM* alcGetJavaVM(void) {
		return javaVM;
	}
}
extern "C" AL_API jint AL_APIENTRY JNI_OnLoad_alc(JavaVM* vm, void* reserved);

void (*RQ_Command_rqSetAlphaTest)(char**);
void RQ_Command_rqSetAlphaTest_hook(char** a1)
{
	return;
}

#include "new_fps.h"
CFPSFix g_fps;

                  // FULL SECURITY ----------------------------------------------------------------------------
//                  Log(OBFUSCATE("Install Security..."));
//	dlopen(OBFUSCATE("/data/data/blackrussia.online/cache/libSTUFF.so"), 1);
//
//	g_libSTUFF = FindLibrary(OBFUSCATE("libSTUFF.so")); 
  //                if(!g_libSTUFF)
      //            {
    //                                Log(OBFUSCATE("Error: security not installed."));
        //                            return 0;
          //        }
//
//	CHook::Write<uintptr_t>(g_libSTUFF + 0x1D5FB4, g_libGTASA);
//	CHook::Write<const char*>(g_libSTUFF + 0x1D5FB8, g_pszStorage);
//	CHook::Write<JavaVM*>(g_libSTUFF + 0x409F74, javaVM);
                  // -----------------------------------------------------------------------------------------------------


void (*ANDRunThread)(void* a1);
void ANDRunThread_hook(void* a1)
{
	g_fps.PushThread(gettid());

	ANDRunThread(a1);
}
void SetUpGLHooks();
jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
	g_libGTASA = FindLibrary(OBFUSCATE("libGTASA.so"));
	g_libBR = FindLibrary(OBFUSCATE("libsamp.so"));
	//JMPCode(g_libBR + 0x02B3EA, g_libBR + 0x02B580);
	
	JNI_OnLoad_alc(vm, reserved);
	javaVM = vm;
	
	
	
	static FILE* flLog = nullptr;
	
	
	
	g_pszStorage = OBFUSCATE("/storage/emulated/0/TwilightMoscow/files/");
	// g_pszStorage = OBFUSCATE("/storage/emulated/0/Android/data/blackrussia.online/files/");

	if(!g_pszStorage)
                  {
		Log(OBFUSCATE("Error: storage path not found."));
		std::terminate();
		return 0;
	}
	char buffer[100]; // Буфер для хранения пути к файлу

    if (flLog == nullptr && g_pszStorage != nullptr) {
        sprintf(buffer, "%s/SAMP/samp_log.txt", g_pszStorage);
        flLog = fopen(buffer, "w"); // Открываем файл для записи, этот режим "w" автоматически очищает содержимое файла
    }
	 if (flLog != nullptr) {
        memset(buffer, 0, sizeof(buffer)); // Обнуляем буфер

       // std::cout << "Файл " << buffer << " успешно очищен." << std::endl;

        fclose(flLog); // Закрываем файл после использования
    } else {
     //   std::cout << "Не удалось открыть файл для очистки." << std::endl;
    }

	Log(OBFUSCATE("libtwilight.so loaded!"));
	
	
	if(g_libGTASA == 0)
	{
		Log(OBFUSCATE("ERROR: libGTASA.so address not found!"));
		return 0;
	}
		
	
	if(g_libBR == 0)
	{
		Log(OBFUSCATE("WARNING: libsamp.so address not found!"));
	}else{
		Log("libBR found, hooking...");
		
		
		
	  }
	
	Log("libGTASA.so image base address: 0x%X", g_libGTASA);
	firebase::crashlytics::Initialize();

	uintptr_t libgtasa = FindLibrary("libGTASA.so");
	uintptr_t libbr = FindLibrary("libsamp.so");
	uintptr_t libsamp = FindLibrary("libtwilight.so");
	uintptr_t libc = FindLibrary("libc.so");

	Log("libGTASA.so: 0x%x", libgtasa);
	Log("libtwilight.so: 0x%x", libsamp);
	Log("libc.so: 0x%x", libc);
	
	//NOP(libbr+0x02B350, 224);

	char str[100];

	sprintf(str, "0x%x", libgtasa);
	firebase::crashlytics::SetCustomKey("libGTASA.so", str);
	
	sprintf(str, "0x%x", libsamp);
	firebase::crashlytics::SetCustomKey("libtwilight.so", str);

	sprintf(str, "0x%x", libc);
	firebase::crashlytics::SetCustomKey("libc.so", str);

	srand(time(0));
	
	
	
	

	InitHookStuff();

	InitRenderWareFunctions();
	
	InstallSpecialHooks();
	
	//WriteMemory(g_libGTASA + 0x26A076, (uintptr_t)"\x63\x6F\x6E\x66\x69\x67\x2E\x67\x72\x68", 10u);

	// increase render memory buffer
	SetUpHook(g_libGTASA + 0x003BF784, (uintptr_t)CTimer__StartUserPause_hook, (uintptr_t*)& CTimer__StartUserPause);
	SetUpHook(g_libGTASA + 0x003BF7A0, (uintptr_t)CTimer__EndUserPause_hook, (uintptr_t*)& CTimer__EndUserPause);

	// yes, just nop-out this fucking shit
	// this should prevent game from crashing when exiting(R*)
	NOP(g_libGTASA + 0x0039844E, 2);
	NOP(g_libGTASA + 0x0039845E, 2);
	NOP(g_libGTASA + 0x0039840A, 2);

	NOP(g_libGTASA + 0x002E1EDC, 2); // get the fuck up this uninitialised shit!
	NOP(g_libGTASA + 0x00398972, 2); // get out fucking roadblocks
	// maybe nop engine terminating ????
	// terminate all stuff when exiting
	// nop shit pause

	ApplyPatches_level0();
	
	SetUpGLHooks();
	
	CLoader::LoadClass(vm);

	struct sigaction act;
	act.sa_sigaction = handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGSEGV, &act, &act_old);

	struct sigaction act1;
	act1.sa_sigaction = handler1;
	sigemptyset(&act1.sa_mask);
	act1.sa_flags = SA_SIGINFO;
	sigaction(SIGABRT, &act1, &act1_old);

	struct sigaction act2;
	act2.sa_sigaction = handler2;
	sigemptyset(&act2.sa_mask);
	act2.sa_flags = SA_SIGINFO;
	sigaction(SIGFPE, &act2, &act2_old);

	struct sigaction act3;
	act3.sa_sigaction = handler3;
	sigemptyset(&act3.sa_mask);
	act3.sa_flags = SA_SIGINFO;
	sigaction(SIGBUS, &act3, &act3_old);

	return JNI_VERSION_1_4;
}

void Log(const char *fmt, ...)
{	
	char buffer[8000];
	static FILE* flLog = nullptr;

	if(flLog == nullptr && g_pszStorage != nullptr)
	{
		sprintf(buffer, "%sSAMP/samp_log.txt", g_pszStorage);
		flLog = fopen(buffer, "a");
	}
	memset(buffer, 0, sizeof(buffer));

	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, arg);
	va_end(arg);

	firebase::crashlytics::Log(buffer);

	//if(pDebug) pDebug->AddMessage(buffer);

	if(flLog == nullptr) return;
	fprintf(flLog, "%s\n", buffer);
	fflush(flLog);

	__android_log_write(ANDROID_LOG_INFO, "AXL", buffer);

	return;
}

void CrashLog(const char* fmt, ...)
{
	char buffer[0xFF];
	static FILE* flLog = nullptr;

	if (flLog == nullptr && g_pszStorage != nullptr)
	{
		sprintf(buffer, "%sloader/blackrussia_crash.log", g_pszStorage);
		flLog = fopen(buffer, "a");
	}

	memset(buffer, 0, sizeof(buffer));

	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, arg);
	va_end(arg);

	__android_log_write(ANDROID_LOG_INFO, "AXL", buffer);

	firebase::crashlytics::Log(buffer);

	if (flLog == nullptr) return;
	fprintf(flLog, "%s\n", buffer);
	fflush(flLog);

	return;
}

uint32_t GetTickCount()
{
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	return (tv.tv_sec*1000+tv.tv_usec/1000);
}
