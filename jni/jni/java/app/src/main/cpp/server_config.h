#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include <string>

namespace server_config {
    extern std::string SERVER_HOST;
    extern int SERVER_PORT;

    void set_server_config(const std::string& host, int port);
    std::string get_server_host();
    int get_server_port();
    std::string get_server_url();
}

#endif