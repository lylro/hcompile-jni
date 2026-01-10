#include "CLoader.h"
#include "main.h"
#include "util/CJavaWrapper.h"
#include "clientlogic/CFamily.h"

void CLoader::LoadClass(JavaVM* vm){
    JNIEnv* env = nullptr;
    vm->GetEnv((void**)& env, JNI_VERSION_1_6);

    CFamily::clazz = env->FindClass("com/eliterussia/game/gui/family/BrFamilyManager");
    CFamily::clazz = (jclass) env->NewGlobalRef( CFamily::clazz );
}
