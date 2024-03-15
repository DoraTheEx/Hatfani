#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define MAX_INPUTS 100
#define BUFFER_SIZE 1024

typedef struct Node {
    char ip[16];
    struct Node* parent;
    struct Node* next;
} Node;

Node* root = NULL;

void addNode(char* ip, char* parentIp) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    strcpy(newNode->ip, ip);
    newNode->next = NULL;

    if (root == NULL) {
        root = newNode;
        newNode->parent = NULL;
        return;
    }

    Node* parent = root;
    while (parent != NULL) {
        if (strcmp(parent->ip, parentIp) == 0) {
            newNode->parent = parent;
            if (parent->next == NULL)
                parent->next = newNode;
            else {
                Node* sibling = parent->next;
                while (sibling->next != NULL)
                    sibling = sibling->next;
                sibling->next = newNode;
            }
            return;
        }
        parent = parent->next;
    }

    printf("Parent IP not found: %s\n", parentIp);
    free(newNode);
}

void printTree(Node* node, int depth) {
    if (node == NULL)
        return;

    for (int i = 0; i < depth; i++)
        printf("  ");
    printf("%s\n", node->ip);

    printTree(node->next, depth + 1);
}

int main() {
    WSADATA wsaData;
    SOCKET listenSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    char senderIp[16];
    char parentIp[16];
    int inputs = 0;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    // Create a socket for listening
    if ((listenSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Bind the socket
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(1234);
    if (bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind failed: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(listenSocket, 5) == SOCKET_ERROR) {
        printf("Listen failed: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    printf("Waiting for connections...\n");

    while (inputs < MAX_INPUTS) {
        // Accept a connection
        clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == INVALID_SOCKET) {
            printf("Accept failed: %d\n", WSAGetLastError());
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }

        // Receive data from the client
        recv(clientSocket, senderIp, sizeof(senderIp), 0);
        recv(clientSocket, parentIp, sizeof(parentIp), 0);

        // Add node to the tree
        addNode(senderIp, parentIp);
        inputs++;

        printf("Received: Sender IP - %s, Parent IP - %s\n", senderIp, parentIp);

        // Close the client socket
        closesocket(clientSocket);
    }

    // Close the listening socket
    closesocket(listenSocket);
    WSACleanup();

    // Print the hierarchical family tree
    printf("\nHierarchical family tree:\n");
    printTree(root, 0);

    return 0;
}
