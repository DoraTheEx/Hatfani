#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define MAX_INPUT_SIZE 100

typedef struct Node {
    char ip[16];
    struct Node* parent;
} Node;

Node* nodes[MAX_INPUT_SIZE];
int numNodes = 0;

void addNode(char* ip, char* parentIp) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    strcpy(newNode->ip, ip);
    newNode->parent = NULL;

    // Find parent node
    for (int i = 0; i < numNodes; ++i) {
        if (strcmp(nodes[i]->ip, parentIp) == 0) {
            newNode->parent = nodes[i];
            break;
        }
    }

    nodes[numNodes++] = newNode;
}

void printFamilyTree(Node* node, int level) {
    for (int i = 0; i < level; ++i) {
        printf("  ");
    }
    printf("%s\n", node->ip);
    
    for (int i = 0; i < numNodes; ++i) {
        if (nodes[i]->parent == node) {
            printFamilyTree(nodes[i], level + 1);
        }
    }
}

int main() {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in server;
    struct sockaddr_in client;
    int c;
    char senderIp[16];
    char parentIp[16];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed.\n");
        return 1;
    }

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(12345);

    // Bind
    if (bind(sock, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed.\n");
        return 1;
    }

    // Listen
    listen(sock, 3);

    // Accept and incoming connection
    printf("Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);
    while ((c = accept(sock, (struct sockaddr*)&client, &c)) != INVALID_SOCKET) {
        // Receive sender IP
        recv(c, senderIp, 16, 0);
        // Receive parent IP
        recv(c, parentIp, 16, 0);

        addNode(senderIp, parentIp);

        printf("Added node: %s (Parent: %s)\n", senderIp, parentIp);
    }

    if (c == INVALID_SOCKET) {
        printf("Accept failed.\n");
        return 1;
    }

    // Print family tree
    printf("Family Tree:\n");
    for (int i = 0; i < numNodes; ++i) {
        if (nodes[i]->parent == NULL) {
            printFamilyTree(nodes[i], 0);
        }
    }

    // Cleanup
    closesocket(sock);
    WSACleanup();

    return 0;
}
