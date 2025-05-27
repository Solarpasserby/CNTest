#include<stdio.h>
#include<stdlib.h>
#include<winsock2.h>

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[])
{
    WSADATA wsaData;
    SOCKET hSocket;
    SOCKADDR_IN serverAddr;

    char message[30];
    int strlen;
    if(argc)
    {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        error_handling("WSAStartup() error");

    hSocket = socket(PF_INET, SOCK_STREAM, 0);
    if(hSocket == INVALID_SOCKET)
        error_handling("socket() error");

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
    serverAddr.sin_port = htons(atoi(argv[2]));

    if(connect(hSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        error_handling("connect() error");

    strlen = recv(hSocket, message, sizeof(message) - 1, 0);
    if(strlen == -1)
        error_handling("recv() error");
    printf("Message from server: %s\n", message);

    closesocket(hSocket);
    WSACleanup();
    return 0;
}