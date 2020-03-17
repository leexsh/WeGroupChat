#include "Server.h"
Server::Server()
{
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(SERVER_PORT);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    listen_fd = 0;
    epoll_fd = 0;
}
void Server::init()
{
    cout<<"Server is initing..."<<endl;
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("listener error");
        exit(-1);
    }
    if (bind(listen_fd, (sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        perror("bind error");
        exit(-1);
    }

    int ret = listen(listen_fd, 5);
    if (ret < 0) {
        perror("listen error");
        exit(-1);
    }

    cout << "start to listen: " << SERVER_IP << endl;

    epoll_fd = epoll_create(EPOLL_SIZE);
    if (epoll_fd < 0) {
        perror("epoll create error");
        exit(-1);
    }
    addfd(epoll_fd, listen_fd, true);
}
void Server::close()
{
    ::close(epoll_fd);
    ::close(listen_fd);
}

int Server::sendMessageToAll(int cli_fd)
{
    char buf[BUF_SZIE], message[BUF_SZIE];
    bzero(buf, sizeof(buf));
    bzero(message, sizeof(BUF_SZIE));
    cout << "read form client : clientId--" << cli_fd << endl;
    int len = recv(cli_fd, buf, BUF_SZIE, 0);
      //if the client close the connection
    if (len == 0) {
        ::close(cli_fd);
        //remove the clientfd from clinets_list
        clients_fd.remove(cli_fd);

        cout << "ClientId :" << cli_fd << " closed , there are " << clients_fd.size() << "int the chatroom" << endl;
    }
     else {
        //if there are  clients in the chatroom
        if (clients_fd.size() == 1) {
            send(cli_fd, CAUTION, strlen(CAUTION), 0);
            return len;
        }
        //format send the message
        sprintf(message, SERVER_MESSAGE, cli_fd, buf);

        list<int>::iterator it;
        for (it = clients_fd.begin(); it != clients_fd.end(); ++it) {
            if (*it != cli_fd) {
                if (send(*it, message, BUF_SZIE, 0) < 0) {
                    return -1;
                }
            }
        }
    }
    return len;
}


void Server::start()
{
     //epoll queue
    static epoll_event events[EPOLL_SIZE];
    //init the server
    init();
    while(1)
    {
        int epoll_count = epoll_wait(epoll_fd, events,EPOLL_SIZE, -1);
        if (epoll_count < 0) {
            perror("epoll error");
            break;
        }
        cout << "epoll_events_count =\n"
        << epoll_count << endl;
        for(int i = 0; i < epoll_count; i++){
            int sockfd = events[i].data.fd;
            if(sockfd == listen_fd)
            {
                sockaddr_in client_address;
                socklen_t client_addrLength = sizeof(sockaddr_in);
                int clientfd = accept(listen_fd, (sockaddr*)&client_address, &client_addrLength);
                cout << "client connection from: "
                     << inet_ntoa(client_address.sin_addr) << ":"
                     << ntohs(client_address.sin_port) << ", cientfd = "
                     << clientfd << endl;
                addfd(epoll_fd, clientfd, true);

                 //clients_list save the client
                clients_fd.push_back(clientfd);
                cout << "add new clientfd = " << clientfd << "to epoll " << endl;
                cout << "Now there are " << clients_fd.size() << "clients in the chatroom" << endl;
                //server send the message
                cout << "welcome message" << endl;
                char message[BUF_SZIE];
                memset(&message, 0, BUF_SZIE);
                //clientg -> SERVER_WELCOME -> message,return strlen(message)
                sprintf(message, SERVER_WELCOME, clientfd);
                int ret = send(clientfd, message, BUF_SZIE, 0);
                if (ret < 0) {
                    perror("send error");
                    close();
                    exit(-1);
                }
            }
            else
            {
               int ret = sendMessageToAll(sockfd);
                if (ret < 0) {
                    perror("");
                    close();
                    exit(-1);
                } 
            }
        }
    }
}