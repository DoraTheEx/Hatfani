#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 5585
#define MAX_CLIENTS 10
#define MAX_IP_LENGTH 16

typedef struct Node {
    char ip[MAX_IP_LENGTH];
    struct Node *parent;
    struct Node *next;
} Node;

Node *root = NULL;

Node* createNode(char *ip, Node *parent) {
    Node *newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    strcpy(newNode->ip, ip);
    newNode->parent = parent;
    newNode->next = NULL;
    return newNode;
}

void insertNode(char *ip, char *parentIP) {
    Node *parent = NULL;
    if (parentIP != NULL) {
        parent = root;
        while (parent != NULL && strcmp(parent->ip, parentIP) != 0) {
            parent = parent->next;
        }
        if (parent == NULL) {
            fprintf(stderr, "Parent IP not found\n");
            return;
        }
    }
    Node *newNode = createNode(ip, parent);
    if (root == NULL) {
        root = newNode;
    } else {
        Node *temp = root;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}

void printTree(Node *node, int level) {
    if (node == NULL) return;
    for (int i = 0; i < level; i++) {
        printf("|   ");
    }
    printf("|---%s\n", node->ip);
    printTree(node->next, level);
    printTree(node->parent, level + 1);
}

int main() {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int addrLen = sizeof(clientAddr);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed\n");
        return EXIT_FAILURE;
    }

    // Create socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        fprintf(stderr, "Socket creation failed\n");
        WSACleanup();
        return EXIT_FAILURE;
    }

    // Prepare the sockaddr_in structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        fprintf(stderr, "Bind failed\n");
        closesocket(serverSocket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    // Listen
    if (listen(serverSocket, MAX_CLIENTS) == SOCKET_ERROR) {
        fprintf(stderr, "Listen failed\n");
        closesocket(serverSocket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    printf("Server started. Waiting for connections...\n");

    // Initialize root node
    root = createNode("1.1.1.1", NULL);

    // Accept connections and handle data
    while (1) {
        if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrLen)) == INVALID_SOCKET) {
            fprintf(stderr, "Accept failed\n");
            closesocket(serverSocket);
            WSACleanup();
            return EXIT_FAILURE;
        }

        char ip[MAX_IP_LENGTH];
        char parentIP[MAX_IP_LENGTH];
        
        // Receive IP and parent IP from client
        recv(clientSocket, ip, MAX_IP_LENGTH, 0);
        recv(clientSocket, parentIP, MAX_IP_LENGTH, 0);

        // Insert into tree
        insertNode(ip, parentIP);

        // Print tree
        printf("Hierarchical Tree:\n");
        printTree(root, 0);

        closesocket(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
