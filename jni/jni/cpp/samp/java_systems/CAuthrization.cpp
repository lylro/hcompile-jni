//
// Created by Error on 21.05.2025.
//

#include "CAuthrization.h"


#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"
#include "CSettings.h"
jobject CAuthrization::thiz = nullptr;
jclass CAuthrization::clazz = nullptr;

bool CAuthrization::bIsShow = false;
void CAuthrization::hide() {
    if(CAuthrization::thiz != nullptr)
    {
        JNIEnv* env = g_pJavaWrapper->GetEnv();

        jmethodID method = env->GetMethodID(CAuthrization::clazz, "destroy", "()V");
        env->CallVoidMethod(CAuthrization::thiz, method);

        env->DeleteGlobalRef(CAuthrization::thiz);
        CAuthrization::thiz = nullptr;
        CAuthrization::bIsShow = false;
    }
}

void CAuthrization::show(bool isEmail, bool isAutoAuth) {
    JNIEnv *env = g_pJavaWrapper->GetEnv();

    if(CAuthrization::thiz == nullptr) {

        jmethodID constructor = env->GetMethodID(CAuthrization::clazz, "<init>",
                                                 "(ZZ)V");
        CAuthrization::thiz = env->NewObject(CAuthrization::clazz, constructor,
                                             isEmail, isAutoAuth);
        CAuthrization::thiz = env->NewGlobalRef(CAuthrization::thiz);
        CAuthrization::bIsShow = true;
    }
}


extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_auth_Autorization_ToggleAutoLogin(JNIEnv *env, jobject thiz,
                                                               jboolean toggle) {
    // TODO: implement ToggleAutoLogin()
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)PACKET_AUTH);
    bsSend.Write((uint8_t)6);
    bsSend.Write((uint8_t) toggle);

    pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
    CSettings::m_Settings.szAutoLogin = toggle;
    CSettings::save();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_auth_Autorization_onLoginClick(JNIEnv *env, jobject thiz,
                                                            jstring pass) {
    // TODO: implement onLoginClick()
    const char *inputPassword = env->GetStringUTFChars(pass, nullptr);
    if (pNetGame) {
        pNetGame->SendLoginPacket(inputPassword);
    }
    env->ReleaseStringUTFChars(pass, inputPassword);
}
