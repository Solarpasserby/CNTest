#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<queue>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<signal.h>

std::queue<char*> messageA;
std::queue<char*> messageB;

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void handle_client(int clnt_sock)
{
    char message[1024];
    int str_len = read(clnt_sock, message, sizeof(message) - 1);
    if(str_len == -1)
        error_handling("read() error");
    message[str_len] = '\0';

    if(message[0] == 'A') {
        messageA.push(strdup(message+1));
        printf("Received from A: %s\n", message);

        if(!messageB.empty()) {
            char* msgB = messageB.front();
            write(clnt_sock, msgB, strlen(msgB));
            free(msgB);
            messageB.pop();
        } else {
            const char* response = "";
            write(clnt_sock, response, strlen(response));
        }
    } else if (message[0] == 'B') {
        messageB.push(strdup(message+1));
        printf("Received from B: %s\n", message);

        if(!messageA.empty()) {
            char* msgA = messageA.front();
            write(clnt_sock, msgA, strlen(msgA));
            free(msgA);
            messageA.pop();
        } else {
            const char* response = "";
            write(clnt_sock, response, strlen(response));
        }
    } else {
        printf("Unknown user type: %c\n", message[0]);
    }
    close(clnt_sock);
}

void sigchld_handler(int signo) {
    // 回收僵尸进程
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char *argv[])
{
    int serv_sock;
    int clnt_sock;

    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;

    signal(SIGCHLD, sigchld_handler); // 防止僵尸进程

    if(argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");

    if(listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    while(1) {
        clnt_addr_size = sizeof(clnt_addr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        if(clnt_sock == -1)
            continue; // accept失败继续

        pid_t pid = fork();
        if(pid == -1) {
            close(clnt_sock);
            continue;
        }
        if(pid == 0) { // 子进程
            close(serv_sock); // 子进程不用监听socket
            handle_client(clnt_sock);
            exit(0);
        } else { // 父进程
            close(clnt_sock); // 父进程不用客户端socket
        }
    }

    // 清理队列
    while(!messageA.empty()) {
        char* msgA = messageA.front();
        free(msgA);
        messageA.pop();
    }
    while(!messageB.empty()) {
        char* msgB = messageB.front();
        free(msgB);
        messageB.pop();
    }

    close(serv_sock);
    return 0;
}