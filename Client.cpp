#include "Client.h"
#include <iostream>
using namespace std;

Client::Client()
{
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET,"101.37.118.181",&serverAddr.sin_addr.s_addr);

    //init the socket
    sock = 0;
 
    //the client stat
    isClientWork = true;

    FD_ZERO(&client_set);

    maxfd = 0;
    stdineof = 0;
}
//connect
void Client::Connect()
{
    cout << "Connect server: " << SERVER_IP << " : " << SERVER_PORT << endl;
    //create the socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("sock error");
        exit(-1);
    }
    //connect the server
    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("connect error");
        exit(-1);
    }
 
}
void Client::Close()
{
   
    //close the socket
    close(sock);
   
}



void Client::Start(FILE *fp)
{
    //epoll events queue
    static epoll_event events[2];
    //conect the server
    Connect();
    while(1)
    {
        if(stdineof == 0){
            FD_SET(fileno(fp),&client_set);
        }
        FD_SET(sock, &client_set);
        maxfd = max(fileno(fp), sock) + 1;
        int ready = select(maxfd, &client_set, NULL,NULL,NULL);
        if(FD_ISSET(fileno(fp), &client_set)){
            if((read(fileno(fp),message, sizeof(message)) == 0)){
                stdineof = 1;
                shutdown(sock, SHUT_WR);
                FD_CLR(fileno(fp), &client_set);
                continue;
            }
            write(sock,message, strlen(message));
            bzero(message, sizeof(message));
        }

        if(FD_ISSET(sock,&client_set)){
            if((read(sock, message, sizeof(message))) == 0){
                if(stdineof == 1){
                    return;
                }
                else
                {
                    perror("error");
                }
            }  
            cout<<message<<endl;
            bzero(message, sizeof(message));
        }
    }
}