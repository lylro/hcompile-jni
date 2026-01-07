//
// Created by Error on 21.05.2025.
//

#ifndef CRIMINAL_MOSCOW_CAUTHRIZATION_H
#define CRIMINAL_MOSCOW_CAUTHRIZATION_H

#include <jni.h>
class CAuthrization {
    static jobject thiz;

public:
    static bool bIsShow;
    static jclass clazz;


    static void hide();

    static void show(bool isEmail, bool isAutoAuth);
};


#endif //CRIMINAL_MOSCOW_CAUTHRIZATION_H
