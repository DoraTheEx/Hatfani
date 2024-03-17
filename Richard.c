#include <stdio.h>
#include <winsock2.h>

#define MAX_INPUTS 100
#define MAX_IP_LENGTH 16

// Structure to represent a node in the tree
typedef struct TreeNode {
    char ip[MAX_IP_LENGTH];
    char parent_ip[MAX_IP_LENGTH];
    struct TreeNode* children[MAX_INPUTS];
    int num_children;
} TreeNode;

// Function to print tree recursively
void printTree(TreeNode* root, int depth) {
    for (int i = 0; i < depth; i++) {
        printf("  "); // Indent based on depth
    }
    printf("%s\n", root->ip);
    for (int i = 0; i < root->num_children; i++) {
        printTree(root->children[i], depth + 1);
    }
}

int main() {
    WSADATA wsaData;
    SOCKET serverSocket;
    struct sockaddr_in serverAddr;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    // Create server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("Socket creation failed.\n");
        return 1;
    }

    // Setup server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(5585);

    // Bind server socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Socket bind failed.\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 10) == SOCKET_ERROR) {
        printf("Listen failed.\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Accept incoming connections
    SOCKET clientSocket;
    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (clientSocket == INVALID_SOCKET) {
        printf("Accept failed.\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Handle incoming data and build the tree
    TreeNode* nodes[MAX_INPUTS];
    int num_nodes = 0;

    while (1) {
        char ip[MAX_IP_LENGTH];
        char parent_ip[MAX_IP_LENGTH];

        // Receive input from client
        recv(clientSocket, ip, sizeof(ip), 0);
        recv(clientSocket, parent_ip, sizeof(parent_ip), 0);

        // Add node to tree
        TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
        strcpy(node->ip, ip);
        strcpy(node->parent_ip, parent_ip);
        node->num_children = 0;
        nodes[num_nodes++] = node;

        // Check for end of input
        if (strcmp(ip, "END") == 0) {
            break;
        }
    }

    // Build tree structure
    for (int i = 0; i < num_nodes; i++) {
        for (int j = 0; j < num_nodes; j++) {
            if (strcmp(nodes[i]->parent_ip, nodes[j]->ip) == 0) {
                nodes[j]->children[nodes[j]->num_children++] = nodes[i];
                break;
            }
        }
    }

    // Find root node
    TreeNode* root = NULL;
    for (int i = 0; i < num_nodes; i++) {
        if (strcmp(nodes[i]->parent_ip, "") == 0) {
            root = nodes[i];
            break;
        }
    }

    // Print tree
    printTree(root, 0);

    // Clean up
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
