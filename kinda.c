#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define MAX_INPUTS 100

// Define a structure to represent each node in the family tree
typedef struct Node {
    char ip[16];  // IP address of the node
    struct Node* children[MAX_INPUTS];  // Array to store children of the node
    int child_count;  // Number of children of the node
} Node;

// Function to initialize a node with given IP address
Node* createNode(char ip[]) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    strcpy(newNode->ip, ip);
    newNode->child_count = 0;
    return newNode;
}

// Function to print the family tree recursively with correct indentation
void printTree(Node* root, int level) {
    if (root == NULL)
        return;

    // Print indentation based on the level of the node
    for (int i = 0; i < level; i++)
        printf("  ");

    // Print the IP address of the current node
    printf("%s\n", root->ip);

    // Recursively print children of the current node
    for (int i = 0; i < root->child_count; i++)
        printTree(root->children[i], level + 1);
}

// Function to find the node with the given IP address in the tree
Node* findNode(Node* root, char ip[]) {
    if (root == NULL)
        return NULL;

    // Check if the current node's IP matches the target IP
    if (strcmp(root->ip, ip) == 0)
        return root;

    // Recursively search children of the current node
    for (int i = 0; i < root->child_count; i++) {
        Node* foundNode = findNode(root->children[i], ip);
        if (foundNode != NULL)
            return foundNode;
    }

    return NULL;
}

// Function to add a child node to a parent node
void addChild(Node* parent, Node* child) {
    parent->children[parent->child_count++] = child;
}

int main() {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in server;
    char senderIP[16], parentIP[16];
    Node* root = createNode("1.1.1.1"); // Root node initialized with IP 1.1.1.1

    // Initialize winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Failed to initialize winsock.\n");
        return 1;
    }

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Failed to create socket.\n");
        return 1;
    }

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(1234);

    // Bind
    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed.\n");
        closesocket(sock);
        return 1;
    }

    printf("Listening on port 1234...\n");
    listen(sock, 3);

    // Accept incoming connections and handle data
    for (int i = 0; i < MAX_INPUTS; i++) {
        SOCKET client;
        struct sockaddr_in client_addr;
        int client_len = sizeof(client_addr);

        // Accept connection
        client = accept(sock, (struct sockaddr *)&client_addr, &client_len);
        if (client == INVALID_SOCKET) {
            printf("Accept failed.\n");
            closesocket(sock);
            return 1;
        }

        // Receive sender IP
        recv(client, senderIP, sizeof(senderIP), 0);
        printf("Received sender IP: %s\n", senderIP);

        // Receive parent IP
        recv(client, parentIP, sizeof(parentIP), 0);
        printf("Received parent IP: %s\n", parentIP);

        // Find or create nodes for sender and parent IPs
        Node* senderNode = findNode(root, senderIP);
        if (senderNode == NULL) {
            senderNode = createNode(senderIP);
            addChild(root, senderNode);
        }
        
        Node* parentNode = findNode(root, parentIP);
        if (parentNode == NULL) {
            parentNode = createNode(parentIP);
            addChild(root, parentNode);
        }

        // Link sender node to parent node
        addChild(parentNode, senderNode);

        printf("Family Tree after input:\n");
        printTree(root, 0);

        closesocket(client);
    }

    closesocket(sock);
    WSACleanup();

    return 0;
}
