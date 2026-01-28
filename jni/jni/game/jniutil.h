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

    void ShowReg(const std::string& jsonString, uint16_t rpcID);
	void ShowDonateDialog(const std::string& jsonString);

    CJavaWrapper(JNIEnv* env);
    ~CJavaWrapper();

	jclass edgar;
jclass guiManagerClass;
jclass donateGui;

    jobject activity;
};

extern CJavaWrapper* g_pJavaWrapper;