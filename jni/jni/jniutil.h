#pragma once

inline jstring GetPackageName(JNIEnv *env, jobject jActivity)
{
    jmethodID method = env->GetMethodID(env->GetObjectClass(jActivity), "getPackageName", "()Ljava/lang/String;");
    return (jstring)env->CallObjectMethod(jActivity, method);
}

inline jobject GetGlobalActivity(JNIEnv *env)
{
    jclass activityThread = env->FindClass("android/app/ActivityThread");
    jmethodID currentActivityThread = env->GetStaticMethodID(activityThread, "currentActivityThread", "()Landroid/app/ActivityThread;");
    jobject at = env->CallStaticObjectMethod(activityThread, currentActivityThread);
    jmethodID getApplication = env->GetMethodID(activityThread, "getApplication", "()Landroid/app/Application;");
    jobject context = env->CallObjectMethod(at, getApplication);
    return context;
}