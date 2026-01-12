#include "main.h"
#include "jniutil.h"
#include <stdio.h>

uintptr_t g_libGTASA = 0;
uintptr_t g_libSAMP = 0;
const char* g_pAPKPackage;

jobject appContext;
JavaVM *mVm;
JNIEnv *mEnv;


#define BR

#ifdef VERSION_2
RakClientInterface* myInterface;

RakClientInterface* (*old_GetRakClientInterface)();
RakClientInterface* GetRakClientInterface_hook()
{
    myInterface = RakNetworkFactory::GetRakClientInterface();
    myInterface->Connect(OBFUSCATE("185.189.255.97"), 3005, 0, 0, 5);
    return myInterface;
}

#endif

#ifdef BR
void (*CNetGame__CNetGame)(char*, const char*, int, const char*, int);
void CNetGame__CNetGame_hook(char* a1, const char* a2, int a3, const char* a4, int a5)
{
    const char* newAddress = OBFUSCATE("185.189.255.97");
    int newPort = 3005;
    CNetGame__CNetGame(a1, newAddress, newPort, a4, a5);
}

void (*a)(uintptr_t*);
void a_hook(uintptr_t* param)
{
    return;
}

void (*b)(uintptr_t*);
void b_hook(uintptr_t* param)
{
    return;
}
#endif

void initSamp() 
{
#ifdef VERSION_2
    CHook::InlineHook(g_libSAMP, 0x075B10, (uintptr_t)GetRakClientInterface_hook, (uintptr_t*)&old_GetRakClientInterface);
#endif
#ifdef BR
    // ÈÑÏÐÀÂËÅÍÎ: äîáàâëåí offset (çàìåíè 0x123456 íà íóæíûé)
    CHook::InlineHook(g_libSAMP, 0x123456, (uintptr_t)CNetGame__CNetGame_hook, (uintptr_t*)&CNetGame__CNetGame);
    CHook::InlineHook(g_libSAMP, 0x07DE34, (uintptr_t)a_hook, (uintptr_t*)&a);	
    CHook::InlineHook(g_libSAMP, 0x0883D0, (uintptr_t)b_hook, (uintptr_t*)&b);	
#endif
}

void Main() 
{
     // code for GTASA
}

void *InitialiseThread(void *p)
{
	Main();
	pthread_exit(0);
    return 0;
}          

JNIEnv *getEnv() 
{
	JNIEnv* env = 0;
    JavaVM* javaVM = mVm;
	int getEnvStat = javaVM->GetEnv((void**)&env, JNI_VERSION_1_6);

	if (getEnvStat == JNI_EDETACHED)
		if (javaVM->AttachCurrentThread(&env, 0) != 0)
		  return 0;

	if (getEnvStat == JNI_EVERSION)
	    return 0;

	if (getEnvStat == JNI_ERR)
	   return 0;

	return env;
}

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    mVm = vm;
    mEnv = getEnv();

	appContext = GetGlobalActivity(mEnv);
    
	g_libGTASA = ARMHook::getLibraryAddress(OBFUSCATE("libGTASA.so"));
	if(g_libGTASA)
	{
		srand(time(0));

		uintptr_t memlib_start = (g_libGTASA + 0x174D4);
		uintptr_t size = 0x1234A;
		
		ARMHook::InitialiseTrampolines(memlib_start, size);

		pthread_t thread;
		pthread_create(&thread, 0, InitialiseThread, 0);
	}

    g_libSAMP = FindLibrary(OBFUSCATE("libsamp.so"));
    if(g_libSAMP) initSamp();

	return JNI_VERSION_1_6;
}

uint32_t GetTickCount()
{
	struct timeval tv;
	gettimeofday(&tv, 0);

	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}