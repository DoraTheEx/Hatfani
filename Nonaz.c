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

void printTree(struct Node* node, int level) {
    if (node == NULL)
        return;

    for (int i = 0; i < level; i++)
        printf("\t");

    printf("%s\n", node->ip);

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

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }

    // Setup server address structure
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(12345);

    // Bind socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        perror("Bind failed");
        closesocket(serverSocket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    // Listen for incoming connections
    if (listen(serverSocket, MAX_CONNECTIONS) == SOCKET_ERROR) {
        perror("Listen failed");
        closesocket(serverSocket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    printf("Server listening on port 12345...\n");

    struct Node root;
    strcpy(root.ip, "Root");
    root.parent = NULL;
    root.childCount = 0;

    while (1) {
        // Accept connection
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
        if (clientSocket == INVALID_SOCKET) {
            perror("Accept failed");
            closesocket(serverSocket);
            WSACleanup();
            return EXIT_FAILURE;
        }

        // Handle connection (replace with your logic)
        char senderIP[16], parentIP[16];
        recv(clientSocket, senderIP, sizeof(senderIP), 0);
        recv(clientSocket, parentIP, sizeof(parentIP), 0);

        // Create a new child node
        struct Node* child = (struct Node*)malloc(sizeof(struct Node));
        strcpy(child->ip, senderIP);
        child->parent = NULL;
        child->childCount = 0;

        // Find the parent node and add the child
        for (int i = 0; i < root.childCount; i++) {
            if (strcmp(root.children[i]->ip, parentIP) == 0) {
                child->parent = root.children[i];
                root.children[i]->children[root.children[i]->childCount++] = child;
            }
        }

        // If parent not found, add to root (assumes the parent connects first)
        if (child->parent == NULL) {
            child->parent = &root;
            root.children[root.childCount++] = child;
        }

        // Print the tree
        printTree(&root, 0);

        // Close the client socket
        closesocket(clientSocket);
    }

    // Close the server socket
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
