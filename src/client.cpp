#include<winsock2.h>
#include<bits/stdc++.h>
#include<thread>

SOCKET clientsocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
std::string str;

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void Mysend(){
    while(1){
        std::cout<<"client: ";
        std::getline(std::cin,str);

        //消息的第一个字符用于区分用户
        str.insert(0,1,'A'); //用户A，不同的用户可以使用不同的字符
        if(send(clientsocket,str.c_str(),str.size(),0) == SOCKET_ERROR){
            error_handling("send() error");
        }
        if(str == "exit"){
            std::cout<<"client closed connection"<<std::endl;
            break;
        }
    }
}

void Myrecv(){
    while(1){
        char buf[1024] = {0};
        int len = recv(clientsocket,buf,sizeof(buf)-1,0);
        if(len == SOCKET_ERROR){
            error_handling("recv() error");
        } else if(len == 0){
            std::cout<<"server closed connection"<<std::endl;
            break;
        }

        std::cout<<"server: "<<buf<<std::endl;
    }
}

int main(){
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2,2),&wsadata);
    sockaddr_in seraddr;
    memset(&seraddr,0,sizeof(seraddr));
    seraddr.sin_family = AF_INET;
    seraddr.sin_addr.S_un.S_addr = inet_addr("118.178.142.29");
    seraddr.sin_port = htons(atoi("16555"));

    if(connect(clientsocket,(sockaddr*)&seraddr,sizeof(seraddr)) == SOCKET_ERROR){
        error_handling("connect() error");
    }

    std::thread send_thread(Mysend);
    std::thread recv_thread(Myrecv);

    //独立recv，send线程结束后，recv也结束
    recv_thread.detach(); 
    send_thread.join();

    closesocket(clientsocket);
    WSACleanup();
    return 0;
}