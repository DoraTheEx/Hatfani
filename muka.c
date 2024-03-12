#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define MAX_INPUT_LEN 20
#define MAX_NODES 100

struct Node {
    char ip[MAX_INPUT_LEN];
    char parent_ip[MAX_INPUT_LEN];
    struct Node* children[MAX_NODES];
    int num_children;
};

void print_tree(struct Node* root, int level) {
    if (root == NULL) return;
    
    for (int i = 0; i < level; i++) {
        printf("    "); // Adjust indentation based on level
    }
    printf("|--- %s\n", root->ip);

    for (int i = 0; i < root->num_children; i++) {
        print_tree(root->children[i], level + 1);
    }
}

int main() {
    WSADATA wsaData;
    SOCKET listenSocket;
    struct sockaddr_in serverAddr;
    struct Node nodes[MAX_NODES];
    int num_nodes = 0;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        printf("Error creating socket.\n");
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(5585);

    if (bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind failed.\n");
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen failed.\n");
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    printf("Waiting for connections...\n");

    while (1) {
        SOCKET clientSocket;
        struct sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);

        clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == INVALID_SOCKET) {
            printf("Accept failed.\n");
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }

        char ip[MAX_INPUT_LEN];
        char parent_ip[MAX_INPUT_LEN];
        recv(clientSocket, ip, MAX_INPUT_LEN, 0);
        recv(clientSocket, parent_ip, MAX_INPUT_LEN, 0);

        if (num_nodes < MAX_NODES) {
            strcpy(nodes[num_nodes].ip, ip);
            strcpy(nodes[num_nodes].parent_ip, parent_ip);
            nodes[num_nodes].num_children = 0;

            for (int i = 0; i < num_nodes; i++) {
                if (strcmp(nodes[i].ip, parent_ip) == 0) {
                    nodes[i].children[nodes[i].num_children++] = &nodes[num_nodes];
                    break;
                }
            }

            num_nodes++;
        }

        closesocket(clientSocket);

        if (num_nodes >= MAX_NODES) {
            printf("Maximum number of nodes reached.\n");
            break;
        }
    }

    printf("Printing family tree...\n");
    printf("Root: 1.1.1.1\n");

    for (int i = 0; i < num_nodes; i++) {
        if (strcmp(nodes[i].parent_ip, "1.1.1.1") == 0) {
            print_tree(&nodes[i], 1);
        }
    }

    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
