#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define MAX_USERS 100

struct Node {
    char ip[16];
    struct Node* sons[MAX_USERS];
    int numSons;
};

struct Node* createNode(const char* ip) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    strcpy(newNode->ip, ip);
    newNode->numSons = 0;
    return newNode;
}

void insertNode(struct Node* parent, struct Node* son) {
    if (parent->numSons < MAX_USERS) {
        parent->sons[parent->numSons++] = son;
    }
}

void printTree(struct Node* root, int level) {
    int i;
    for (i = 0; i < level; i++) {
        printf("  ");
    }
    printf("%s\n", root->ip);

    for (i = 0; i < root->numSons; i++) {
        printTree(root->sons[i], level + 1);
    }
}

struct Node* findNode(struct Node* root, const char* ip) {
    if (strcmp(root->ip, ip) == 0) {
        return root;
    }

    for (int i = 0; i < root->numSons; i++) {
        struct Node* foundNode = findNode(root->sons[i], ip);
        if (foundNode != NULL) {
            return foundNode;
        }
    }

    return NULL;
}

void freeTree(struct Node* root) {
    if (root == NULL) {
        return;
    }

    for (int i = 0; i < root->numSons; i++) {
        freeTree(root->sons[i]);
    }

    free(root);
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("Error creating socket: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(1234);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind failed with error: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        printf("Listen failed with error: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    printf("Server listening on port 1234...\n");

    struct Node* root = createNode("1.1.1.1");

    while (1) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            printf("Accept failed with error: %d\n", WSAGetLastError());
            closesocket(serverSocket);
            WSACleanup();
            freeTree(root);
            return 1;
        }

        char senderIP[16];
        char parentIP[16];

        recv(clientSocket, senderIP, sizeof(senderIP), 0);
        recv(clientSocket, parentIP, sizeof(parentIP), 0);

        struct Node* senderNode = findNode(root, senderIP);
        struct Node* parentNode = findNode(root, parentIP);

        if (senderNode == NULL) {
            senderNode = createNode(senderIP);
            insertNode(parentNode, senderNode);
        }

        printf("Family tree after input:\n");
        printTree(root, 0);

        closesocket(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();
    freeTree(root);
    return 0;
}
