#ifndef BR_CLOADER_H
#define BR_CLOADER_H
#include <jni.h>

static class CLoader {
public:
    static void LoadClass(JavaVM* vw);
};


#endif //BR_CLOADER_H
