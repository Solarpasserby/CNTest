#include<winsock2.h>
#include<bits/stdc++.h>

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(){
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2,2),&wsadata);
    SOCKET clientsocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    sockaddr_in seraddr;
    memset(&seraddr,0,sizeof(seraddr));
    seraddr.sin_family = AF_INET;
    seraddr.sin_addr.S_un.S_addr = inet_addr("118.178.142.29");
    seraddr.sin_port = htons(atoi("16555"));

    if(connect(clientsocket,(sockaddr*)&seraddr,sizeof(seraddr)) == SOCKET_ERROR){
        error_handling("connect() error");
    }

    std::string str;
    while(1){
        cout<<"client: ";
        getline(cin,str);

        //消息的第一个字符用于区分用户
        str.insert(0,1,'A'); //用户A，不同的用户可以使用不同的字符

        if(send(clientsocket,str.c_str(),str.size(),0) == SOCKET_ERROR){
            error_handling("send() error");
        }

        char buf[1024] = {0};
        int len = recv(clientsocket,buf,sizeof(buf)-1,0);
        if(len == SOCKET_ERROR){
            error_handling("recv() error");
        } else if(len == 0){
            cout<<"server closed connection"<<endl;
            break;
        }

        cout<<"server: "<<buf<<endl;
    }

    closesocket(clientsocket);
    WSACleanup();
    return 0;
}