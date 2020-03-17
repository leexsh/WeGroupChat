#pragma once
#include <string>
#include <stdlib.h>
#include <netinet/in.h>
#include <list>

#include "Common.h"

using namespace std;

class Server{
public:
    Server();
    void init();
    void close();
    void start();
private:
    int sendMessageToAll(int cli_fd);
    sockaddr_in serveraddr;
    int listen_fd;
    int epoll_fd;
    list<int> clients_fd;
};