#include <stdio.h>
#include <winsock2.h>
#include <string.h>

#define PORT 5585
#define MAX_INPUTS 100

struct Node {
    char ip[16];
    struct Node *parent;
    struct Node *children;
    struct Node *next; // For siblings
};

struct Node* create_node(char *ip, struct Node *parent) {
    struct Node *node = (struct Node*)malloc(sizeof(struct Node));
    strcpy(node->ip, ip);
    node->parent = parent;
    node->children = NULL;
    return node;
}

void add_child(struct Node *parent, struct Node *child) {
    if (parent == NULL) {
        return;
    }
    if (parent->children == NULL) {
        parent->children = child;
    } else {
        struct Node *current = parent->children;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = child;
    }
}

void print_tree(struct Node *node, int level) {
    if (node == NULL) {
        return;
    }
    for (int i = 0; i < level; i++) {
        printf("  ");
    }
    printf("%s\n", node->ip);
    print_tree(node->children, level + 1);
}

int main() {
    WSADATA wsaData;
    SOCKET serverSocket;
    SOCKADDR_IN serverAddr, clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    char buffer[100];
    int bytesReceived;
    struct Node *root = create_node("1.1.1.1", NULL);
    int inputCount = 0;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed!\n");
        return 1;
    }

    // Create a socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("Socket creation failed!\n");
        WSACleanup();
        return 1;
    }

    // Bind the socket to the port
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind failed!\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        printf("Listen failed!\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    printf("Server listening on port %d...\n", PORT);

    while (inputCount < MAX_INPUTS) {
        SOCKET clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddr, &clientAddrSize);

        if (clientSocket == INVALID_SOCKET) {
            printf("Accept failed!\n");
            continue;
        }

        printf("Client connected!\n");

        // Receive IP and parent IP
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            char *ip = strtok(buffer, " ");
            char *parentIp = strtok(NULL, " ");

            struct Node *node = create_node(ip, NULL);
            struct Node *parentNode = create_node(parentIp, NULL);

            // Find parent node in the tree
            struct Node *current = root;
            while (current) {
                if (strcmp(current->ip, parentIp) == 0) {
                    parentNode = current;
                    break;
                }
                current = current->children;
            }

            // Add node to the tree
            add_child(parentNode, node);

            inputCount++;
            printf("%d inputs received\n", inputCount);
        }

        closesocket(clientSocket);
    }

    printf("\nPrinting hierarchical tree:\n");
    print_tree(root, 0);

    // Close socket and cleanup
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
