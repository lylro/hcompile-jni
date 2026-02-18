#include "entry.h"
#include "xorstr.h"
#include "settings.h"
#include "game/jniutil.h"

CSettings *pSettings = nullptr;
CJavaWrapper *pJavaWrapper = nullptr;
extern "C"
{
	JavaVM* javaVM = NULL;
	JavaVM* alcGetJavaVM(void) {
		return javaVM;
	}
}

extern "C"
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    // Делаем все, что обычно делается в JNI_Onload
	CApp::Initialise(eAppInit::APP_INIT_OFFSETS);
	
	javaVM = vm;
	
	pthread_t ptid;
    pthread_create(&ptid, NULL, hack_thread, NULL);
	
	
    return JNI_VERSION_1_6;
}

void (*orig_JNILib_step)(JNIEnv* env, jclass cls);
void hook_JNILib_step(JNIEnv* env, jclass cls)
{
	pSettings = new CSettings();
	pJavaWrapper = new CJavaWrapper(env);
	pJavaWrapper->death = cls;
	orig_JNILib_step(env, cls);
}

void* hack_thread(void* args)
{
    while(!CGameAPI::GetBase()) {}
    while(*(int *)(CGameAPI::GetBase(xorstr("RwInitialised"))) == 0) {}

    // Хук на RegisterAsRemoteProcedureCall — ДО инициализации App
    DobbyHook(
        (void*)(CGameAPI::GetBase(xorstr("RakClient::RegisterAsRemoteProcedureCall"))),
        (void*)hook_RakClient__RegisterAsRemoteProcedureCall,
        (void**)&orig_RakClient__RegisterAsRemoteProcedureCall
    );

    CApp::Initialise(eAppInit::APP_INIT_RW);

    // Ждём pRakClient
    uintptr_t ng_pRakClient = *(uintptr_t*)(CGameAPI::GetBase(xorstr("CNetGame::m_pRakClient")));
    while(!ng_pRakClient)
    {
        ng_pRakClient = *(uintptr_t*)(CGameAPI::GetBase(xorstr("CNetGame::m_pRakClient")));
    }

    // Регистрируем свои RPC
    RegisterRPCs((RakClientInterface*)ng_pRakClient);

    // Хук ProcessNetwork
    DobbyHook(
        (void*)(CGameAPI::GetBase(xorstr("CNetGame::ProcessNetwork"))),
        (void*)hook_CNetGame__ProcessNetwork,
        (void**)&orig_CNetGame__ProcessNetwork
    );

    // Хук Connect
    DobbyHook(
        (void*)( *(uintptr_t *)(*(uintptr_t*)ng_pRakClient + 8) ),
        (void*)hook_RakClient__Connect,
        (void**)&orig_RakClient__Connect
    );

    // Хук Send
    DobbyHook(
        (void*)( *(uintptr_t *)(*(uintptr_t*)ng_pRakClient + 32) ),
        (void*)hook_RakClient__Send,
        (void**)&orig_RakClient__Send
    );

    // Хук RPC
    DobbyHook(
        (void*)( *(uintptr_t *)(*(uintptr_t*)ng_pRakClient + 108) ),
        (void*)hook_RakClient__RPC,
        (void**)&orig_RakClient__RPC
    );

    pthread_exit(nullptr);
    return nullptr;
}
