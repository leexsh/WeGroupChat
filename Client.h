#pragma once
#include <string>
#include "Common.h"
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/select.h>
using namespace std;

class Client
{

public:
    //constructor
    Client();
    //connect the server
    void Connect();
    //close the client
    void Close();
    //start the client
    void Start();
    void Start(FILE *fp);
private:
    //the server socket
    int sock;
    

    
    // select set
    fd_set client_set;
    int maxfd;
    int stdineof;



    bool isClientWork;
    //chat message buffer
    char message[BUF_SZIE];
    //server ip + port
    sockaddr_in serverAddr;
};