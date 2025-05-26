#include<winsock2.h>
#include<bits/stdc++.h>

using namespace std;

int main(){
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2,2),&wsadata);
    SOCKET clientsocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    sockaddr_in seraddr;
    seraddr.sin_family = AF_INET;
    seraddr.sin_addr.S_un.S_addr = inet_addr("");
    seraddr.sin_port = htons(1);

    connect(clientsocket,(sockaddr*)&seraddr,sizeof(seraddr));

    string str;
    while(1){
        cout<<"client: ";
        getline(cin,str);
        send(clientsocket,str.c_str(),str.size(),0);
        char buf[1024] = {0};
        int len = recv(clientsocket,buf,sizeof(buf),0);
        if(len == 0){
            cout<<"server closed"<<endl;
            break;
        }
        cout<<"server: "<<buf<<endl;
    }


    
}