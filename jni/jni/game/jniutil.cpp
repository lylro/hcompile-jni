#include "jniutil.h"
#include "chat.h"


extern "C" JavaVM* javaVM;

JNIEnv* CJavaWrapper::GetEnv()
{
    JNIEnv* env = nullptr;
    int getEnvStat = javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);

    if (getEnvStat == JNI_EDETACHED)
    {
        if (javaVM->AttachCurrentThread(&env, NULL) != 0)
        {
            return nullptr;
        }
    }
    if (getEnvStat == JNI_EVERSION)
    {
        return nullptr;
    }

    if (getEnvStat == JNI_ERR)
    {
        return nullptr;
    }

    return env;
}
#include "xorstr.h"

void CJavaWrapper::ShowMain(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 1 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowShop(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 2 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowInv(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 3 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowMap(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 4 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowChat(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 5 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowGear(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 6 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowBag(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 7 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowMail(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 8 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowRank(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 9 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowLog(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 10 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowInfo(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 11 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowHelp(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 12 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowMenu(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 13 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowJoin(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 14 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowPlay(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 15 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowLoad(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 16 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowSave(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 17 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowOpt(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 18 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowExit(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 19 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowUpg(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 20 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowDown(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 21 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowLeft(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 22 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowRight(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 23 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowTop(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 24 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowBot(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 25 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowHud(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 26 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowWin(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 27 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowLose(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 28 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowPvP(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 29 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowPvE(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 30 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowCoop(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 31 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowClan(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 32 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowAlly(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 33 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowFoe(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 34 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowBuff(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 35 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowDebuff(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 36 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowPet(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 37 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowMount(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 38 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowSkill(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 39 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowTree(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 40 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowQuest(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 41 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowNote(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 42 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowMark(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 43 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowCamp(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 44 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowZone(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 45 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowFort(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 46 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowTrap(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 47 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowSafe(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 48 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowWild(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 49 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowSky(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 50 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowDark(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 51 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowLite(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 52 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowRain(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 53 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowSnow(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 54 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowFire(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 55 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowIce(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 56 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowWind(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 57 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowDust(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 58 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowFog(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 59 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowBoss(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 60 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowMin(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 61 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowMax(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 62 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowLvl(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 63 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowExp(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 64 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowHP(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 65 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowMP(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 66 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowKey(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 67 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowBox(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 68 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowDoor(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 69 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowRoom(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 70 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowHall(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 71 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowScan(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 72 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowPing(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 73 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowNet(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 74 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowIP(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 75 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowMac(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 76 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowLoadout(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 77 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowDrop(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 78 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowPick(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 79 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowUse(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 80 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowCraft(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 81 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}


void CJavaWrapper::ShowBuild(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип  

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");  
            if (showCurrentGUIMethod != NULL) {  
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");  
                if (jsonObjectClass != NULL) {  
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");  
                    if (constructor != NULL) {  
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);  

                        // Теперь вызываем метод Java  
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 82 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);  
                    }  
                }  
            }  

            env->DeleteLocalRef(jsonJString);  
            env->DeleteLocalRef(guiManagerInstance);  
        }  
    }  

    env->DeleteLocalRef(guiManagerClass);  

    EXCEPTION_CHECK(env);  
}
CJavaWrapper::CJavaWrapper(JNIEnv* env)
{
	guiManagerClass = env->FindClass("com/blackhub/bronline/game/GUIManager");
	
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