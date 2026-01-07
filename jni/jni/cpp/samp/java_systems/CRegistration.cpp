//
// Created by Error on 21.05.2025.
//

#include "CRegistration.h"

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"
#include "CSettings.h"
jobject CRegistration::thiz = nullptr;
jclass CRegistration::clazz = nullptr;

int CRegistration::RegisterSkinValue = 0;
int CRegistration::RegisterSexMale = 0;
int CRegistration::RegisterSkinId = 0;
bool CRegistration::bIsShow = false;

const uint32_t cRegisterSkin[2][10] = {
        {9,  195, 231, 232, 1,   1,   1,   1,   1, 1}, // female
        {16, 79,  134, 135, 200, 234, 235, 236, 239} // male
};


extern CGame* pGame;
void CRegistration::hide() {
    if(CRegistration::thiz != nullptr)
    {
        JNIEnv* env = g_pJavaWrapper->GetEnv();

        jmethodID method = env->GetMethodID(CRegistration::clazz, "destroy", "()V");
        env->CallVoidMethod(CRegistration::thiz, method);

        env->DeleteGlobalRef(CRegistration::thiz);
        CRegistration::thiz = nullptr;
        CRegistration::bIsShow = false;
    }
}

void CRegistration::show() {
    JNIEnv *env = g_pJavaWrapper->GetEnv();

    if(CRegistration::thiz == nullptr) {

        jmethodID constructor = env->GetMethodID(CRegistration::clazz, "<init>",
                                                 "()V");
        CRegistration::thiz = env->NewObject(CRegistration::clazz, constructor);
        CRegistration::thiz = env->NewGlobalRef(CRegistration::thiz);
        CRegistration::bIsShow = true;

    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_auth_Registration_onRegisterClick(JNIEnv *pEnv, jobject thiz,
                                                               jstring pass, jstring mail,
                                                               jint sex, jint age) {
    // TODO: implement onRegisterClick()

    // TODO: implement onRegisterClick()
    const char *inputPassword = pEnv->GetStringUTFChars(pass, nullptr);
    const char *inputMail = pEnv->GetStringUTFChars(mail, nullptr);

    if (pNetGame) {
        pNetGame->SendRegisterPacket((char *) inputPassword, (char *) inputMail, sex, age, CRegistration::ChangeRegisterSkin(CRegistration::RegisterSkinValue));
    }

    pGame->ToggleHUDElement(0, false);

    // ..  Log("onRegisterPlayClick: inputPassword - %s, inputMail - %s, ChooseSex - %d", inputPassword, inputMail, choosesex);

    pEnv->ReleaseStringUTFChars(pass, inputPassword);
    pEnv->ReleaseStringUTFChars(mail, inputMail);
}

uint32_t CRegistration::ChangeRegisterSkin(int skin) {
    uint32_t uiSkin = 16;
    bool bIsMan = CRegistration::RegisterSexMale == 1 ? true : false;
    uint32_t uiMaxSkins = bIsMan ? 9 : 4;

    if (!(0 < skin <= uiMaxSkins)) {
        CRegistration::RegisterSkinId = uiSkin;
        return uiSkin;
    }

    uiSkin = cRegisterSkin[(int) bIsMan][skin - 1];
    CRegistration::RegisterSkinId = uiSkin;

    return uiSkin;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_auth_Registration_onRegisterSkinBackClick(JNIEnv *env, jobject thiz) {
    // TODO: implement onRegisterSkinBackClick()
    CRegistration::RegisterSkinValue--;
    if (CRegistration::RegisterSexMale == 1) // man
    {
        if (CRegistration::RegisterSkinValue < 1) {
            CRegistration::RegisterSkinValue = 9;
        }
    } else if (CRegistration::RegisterSexMale == 2) // woman
    {
        if (CRegistration::RegisterSkinValue < 1) {
            CRegistration::RegisterSkinValue = 4;
        }
    }
    pNetGame->SendRegisterSkinPacket(
            CRegistration::ChangeRegisterSkin(CRegistration::RegisterSkinValue));
}
extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_auth_Registration_onRegisterSkinNextClick(JNIEnv *env, jobject thiz) {
    // TODO: implement onRegisterSkinNextClick()
    CRegistration::RegisterSkinValue++;
    if (CRegistration::RegisterSexMale == 1) // man
    {
        if (CRegistration::RegisterSkinValue > 9) {
            CRegistration::RegisterSkinValue = 1;
        }
    } else if (CRegistration::RegisterSexMale == 2) // woman
    {
        if (CRegistration::RegisterSkinValue > 4) {
            CRegistration::RegisterSkinValue = 1;
        }
    }
    pNetGame->SendRegisterSkinPacket(
            CRegistration::ChangeRegisterSkin(CRegistration::RegisterSkinValue));
}
extern "C"
JNIEXPORT void JNICALL
Java_com_criminal_moscow_gui_auth_Registration_onRegisterMaleSend(JNIEnv *env, jobject thiz,
                                                                  jint sex) {
    // TODO: implement onRegisterMaleSend()
    CRegistration::RegisterSexMale = sex;

    CPlayerPed *pPlayer = pGame->FindPlayerPed();
    CCamera *pCamera = pGame->GetCamera();
    pNetGame->SendRegisterSkinPacket(
            CRegistration::ChangeRegisterSkin(CRegistration::RegisterSkinValue));

    if (pPlayer->IsInVehicle())
        pPlayer->RemoveFromVehicleAndPutAt(-82.9753, 966.7605, 1597.9788);
    else
        pPlayer->TeleportTo(-82.9753, 966.7605, 1597.9788);

    pPlayer->ForceTargetRotation(90.0f);

    if (pPlayer && pCamera) {
        pCamera->SetPosition(-85.068267, 966.699584, 1598.421997, 0.0f, 0.0f, 0.0f);
        pCamera->LookAtPoint(-80.124114, 967.120971, 1597.807373, 2);
        pPlayer->SetInterior(2, true);
    }
}