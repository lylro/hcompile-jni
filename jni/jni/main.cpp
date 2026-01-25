#include <jni.h>
#include "util/patch.h"

uintptr_t g_libSTUFF = 0;
uintptr_t g_libGTASA = 0;

extern "C"
{	
	jboolean Java_com_nvidia_devtech_NvEventQueueActivity_customMultiTouchEvent(JNIEnv *env, jobject thiz, jint action, jint pointer, jint x1, jint y1, jint x2, jint y2, jint x3, jint y3)
	{



		return CPatch::CallFunction<jboolean>(g_libSTUFF + 0x2A810 + 1, env, thiz, action, pointer, x1, y1, x2, y2, x3, y3);
	}

	void Java_com_nvidia_devtech_NvEventQueueActivity_onEventBackPressed(JNIEnv *pEnv, jobject thiz)
	{
		return;
	}
}


int (*NVEventGetNextEvent)(uintptr_t event, int a1);
int NVEventGetNextEvent_hook(uintptr_t event, int a1)
{	
	return CPatch::CallFunction<int>(g_libSTUFF + 0x348E8 + 1, event, a1);
}

int RET_ORIGNVEvent(uintptr_t event, int a1)
{
	return NVEventGetNextEvent(event, a1);
}

int pointsArray[4000], pointersArray[4000];
void TouchStuff()
{	
	g_libSTUFF = FindLibrary("libStuff.so"); 
	*(uintptr_t*)(g_libSTUFF + 0x1D5FB4) = g_libGTASA;
	CPatch::UnFuck(g_libGTASA + 0x5D1E8C);
	CPatch::UnFuck(g_libGTASA + 0x5CEA8C);
  	*(uintptr_t*)(g_libGTASA + 0x5D1E8C) = (uintptr_t)pointsArray;
  	*(uintptr_t*)(g_libGTASA + 0x5CEA8C) = (uintptr_t)pointersArray;
  	CPatch::WriteMemory(g_libGTASA + 0x238232, "\x03\x20", 2);

	CPatch::InlineHook(g_libGTASA, 0x23ACC4, &NVEventGetNextEvent_hook, &NVEventGetNextEvent);
	*(uintptr_t*)(g_libSTUFF + 0x1F9EE8) = (uintptr_t)NVEventGetNextEvent;
}

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{   
    g_libGTASA = FindLibrary("libGTASA.so");
	TouchStuff();
    return JNI_VERSION_1_4;
}