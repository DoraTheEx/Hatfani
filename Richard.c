#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define MAX_CONNECTIONS 100

typedef struct Node {
    char ip[16];
    struct Node* next;
} Node;

typedef struct Tree {
    char ip[16];
    Node* infections;
    struct Tree* children[MAX_CONNECTIONS];
    int num_children;
} Tree;

void addInfection(Tree* root, const char* infector, const char* infected) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    strncpy(newNode->ip, infected, sizeof(newNode->ip));
    newNode->next = root->infections;
    root->infections = newNode;

    for (int i = 0; i < root->num_children; ++i) {
        if (strcmp(root->children[i]->ip, infector) == 0) {
            addInfection(root->children[i], infector, infected);
            break;
        }
    }
}

void printTree(Tree* root, int depth) {
    for (int i = 0; i < depth; ++i)
        printf("  ");

    printf("%s\n", root->ip);

    Node* infections = root->infections;
    while (infections != NULL) {
        for (int i = 0; i < depth + 1; ++i)
            printf("  ");
        printf("%s\n", infections->ip);
        infections = infections->next;
    }

    for (int i = 0; i < root->num_children; ++i)
        printTree(root->children[i], depth + 1);
}

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Error initializing Winsock");
        return EXIT_FAILURE;
    }

    // Initialize the root of the tree (Patient Zero)
    Tree root;
    strncpy(root.ip, "1.1.1.1", sizeof(root.ip));
    root.infections = NULL;
    root.num_children = 0;

    // Create a socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        perror("Error creating socket");
        WSACleanup();
        return EXIT_FAILURE;
    }

    // Bind the socket to a specific port
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8080);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        perror("Error binding socket");
        closesocket(serverSocket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    // Listen for incoming connections
    if (listen(serverSocket, MAX_CONNECTIONS) == SOCKET_ERROR) {
        perror("Error listening for connections");
        closesocket(serverSocket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    printf("Server is listening on port 8080...\n");

    // Accept connections and handle data
    while (1) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            perror("Error accepting connection");
            continue;
        }

        char buffer[256];
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            perror("Error receiving data");
            closesocket(clientSocket);
            continue;
        }

        buffer[bytesRead] = '\0';
        char* ip = strtok(buffer, "-");
        char* parentIP = strtok(NULL, "-");

        // Add infection to the tree
        addInfection(&root, parentIP, ip);

        // Print the tree
        printf("Tree after receiving data:\n");
        printTree(&root, 0);

        closesocket(clientSocket);
    }

    // Close the server socket and cleanup Winsock
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
