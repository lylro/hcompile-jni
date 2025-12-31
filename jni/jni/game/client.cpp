#include "client.h"
#include "../core/logger.h"
#include <dlfcn.h>
#include <unistd.h>

namespace Client {

void WaitForClient() {
    Logger::Log("Waiting for game client...");

    while (!dlopen("libblackrussia-client.so", RTLD_NOW)) {
        usleep(500000);
    }

    Logger::Log("Game client loaded");
}

}