#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define MAX_NODES 100
#define PORT 1234

struct Node {
    char ip[16]; // Assuming IPv4 addresses
    char parent_ip[16];
};

struct Node infection_tree[MAX_NODES];
int node_count = 0;

void printTree(int index) {
    if (index >= node_count)
        return;

    printf("%s infected by %s\n", infection_tree[index].ip, infection_tree[index].parent_ip);

    for (int i = 0; i < node_count; i++) {
        if (strcmp(infection_tree[i].parent_ip, infection_tree[index].ip) == 0) {
            printTree(i);
        }
    }
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "Failed to initialize Winsock\n");
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        fprintf(stderr, "Failed to create socket\n");
        WSACleanup();
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        fprintf(stderr, "Bind failed\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        fprintf(stderr, "Listen failed\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    printf("Server listening on port %d\n", PORT);

    SOCKET clientSocket;
    struct sockaddr_in clientAddr;
    int addrLen = sizeof(clientAddr);

    while (1) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
        if (clientSocket == INVALID_SOCKET) {
            fprintf(stderr, "Accept failed\n");
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        char sender_ip[16];
        char parent_ip[16];

        recv(clientSocket, sender_ip, sizeof(sender_ip), 0);
        recv(clientSocket, parent_ip, sizeof(parent_ip), 0);

        if (node_count < MAX_NODES) {
            strcpy(infection_tree[node_count].ip, sender_ip);
            strcpy(infection_tree[node_count].parent_ip, parent_ip);
            node_count++;

            // Print the infection tree
            printf("\nInfection Tree:\n");
            printTree(0);
            printf("\n");
        } else {
            printf("Maximum number of nodes reached.\n");
            break;
        }

        closesocket(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
