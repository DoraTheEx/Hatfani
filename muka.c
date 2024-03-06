#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define MAX_CONNECTIONS 100

struct Node {
    char ip[16];
    struct Node* parent;
    struct Node* children[MAX_CONNECTIONS];
    int childCount;
};

// Function to print the tree recursively with proper indentation
void printTree(struct Node* node, int level) {
    if (node == NULL)
        return;

    for (int i = 0; i < level; i++)
        printf("\t");

    printf("%s (Parent: %s)\n", node->ip, (node->parent != NULL) ? node->parent->ip : "None");

    for (int i = 0; i < node->childCount; i++)
        printTree(node->children[i], level + 1);
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("WSAStartup failed");
        return EXIT_FAILURE;
    }

    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int addrLen = sizeof(struct sockaddr_in);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(12345);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        perror("Bind failed");
        closesocket(serverSocket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    if (listen(serverSocket, MAX_CONNECTIONS) == SOCKET_ERROR) {
        perror("Listen failed");
        closesocket(serverSocket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    printf("Server listening on port 12345...\n");

    struct Node root;
    strcpy(root.ip, "1.1.1.1");
    root.parent = NULL;
    root.childCount = 0;

    struct Node* allNodes[MAX_CONNECTIONS];
    allNodes[0] = &root;

    while (1) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
        if (clientSocket == INVALID_SOCKET) {
            perror("Accept failed");
            closesocket(serverSocket);
            WSACleanup();
            return EXIT_FAILURE;
        }

        char senderIP[16], parentIP[16];
        recv(clientSocket, senderIP, sizeof(senderIP), 0);
        recv(clientSocket, parentIP, sizeof(parentIP), 0);

        struct Node* child = (struct Node*)malloc(sizeof(struct Node));
        strcpy(child->ip, senderIP);
        child->parent = NULL;
        child->childCount = 0;

        for (int i = 0; i < MAX_CONNECTIONS; i++) {
            if (allNodes[i] != NULL && strcmp(allNodes[i]->ip, parentIP) == 0) {
                child->parent = allNodes[i];
                allNodes[i]->children[allNodes[i]->childCount++] = child;
                break;
            }
        }

        if (child->parent == NULL) {
            child->parent = &root;
            root.children[root.childCount++] = child;
        }

        for (int i = 0; i < MAX_CONNECTIONS; i++) {
            if (allNodes[i] == NULL) {
                allNodes[i] = child;
                break;
            }
        }

        // Print the tree
        printf("Current Tree Structure:\n");
        printTree(&root, 0);

        closesocket(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
