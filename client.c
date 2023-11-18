#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define IP "127.0.0.1"
#define PORT_No 12345
#define BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char command[BUFFER_SIZE];

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        error("Error creating socket");
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT_No);
    inet_pton(AF_INET, IP, &serverAddr.sin_addr);

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        error("Error connecting to the server");
    }

    printf("Enter a Linux command: ");
    if (fgets(command, sizeof(command), stdin) == NULL) {
        error("Error reading user input");
    }

    if (send(clientSocket, command, strlen(command), 0) == -1) {
        error("Error sending command to server");
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead == -1) {
        error("Error receiving results from server");
    } else if (bytesRead == 0) {
        printf("Server closed the connection\n");
    } else {
        buffer[bytesRead] = '\0'; 
        printf("Results from the server:\n%s\n", buffer);
    }

    close(clientSocket);

    return 0;
}
