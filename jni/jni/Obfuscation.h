#ifndef OBFUSCATION_H
#define OBFUSCATION_H

#include <jni.h>
#include <string>

class Obfuscation {
public:
    Obfuscation();
    ~Obfuscation();

    std::string obfuscate(const std::string &input);
    std::string deobfuscate(const std::string &input);
};

#endif // OBFUSCATION_H
