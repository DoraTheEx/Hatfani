#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define MAX_NODES 100
#define PORT 12345

// Node structure to represent each IP address
typedef struct Node {
    char ip[16]; // Assuming IPv4 addresses
    struct Node* parent;
    struct Node* children[MAX_NODES];
    int numChildren;
    int printed;
} Node;

Node* nodes[MAX_NODES]; // Array to hold all nodes
int numNodes = 0;

// Function to find a node by IP address
Node* findNode(const char* ip) {
    for (int i = 0; i < numNodes; i++) {
        if (strcmp(nodes[i]->ip, ip) == 0) {
            return nodes[i];
        }
    }
    return NULL;
}

// Function to create a new node
Node* createNode(const char* ip) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    strcpy(newNode->ip, ip);
    newNode->parent = NULL;
    newNode->numChildren = 0;
    newNode->printed = 0;
    return newNode;
}

// Function to add a new relationship (child to parent)
void addRelationship(const char* childIp, const char* parentIp) {
    Node* child = findNode(childIp);
    Node* parent = findNode(parentIp);

    if (child == NULL) {
        child = createNode(childIp);
        nodes[numNodes++] = child;
    }

    if (parent == NULL) {
        parent = createNode(parentIp);
        nodes[numNodes++] = parent;
    }

    child->parent = parent;
    parent->children[parent->numChildren++] = child;
}

// Function to print the hierarchical tree
void printTree(Node* node, int depth) {
    if (node == NULL || node->printed) return;

    for (int i = 0; i < depth; i++) {
        printf("\t");
    }
    printf("|-- %s\n", node->ip);
    node->printed = 1;

    for (int i = 0; i < node->numChildren; i++) {
        printTree(node->children[i], depth + 1);
    }
}

int main() {
    WSADATA wsa;
    SOCKET server_fd, new_socket;
    struct sockaddr_in server, client;
    int opt = 1;
    int addrlen = sizeof(server);
    char buffer[32];
    
    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup failed\n");
        exit(EXIT_FAILURE);
    }
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        exit(EXIT_FAILURE);
    }
    
    // Forcefully attaching socket to the port 12345
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (char *)&opt, sizeof(opt)) == SOCKET_ERROR) {
        printf("setsockopt failed\n");
        exit(EXIT_FAILURE);
    }
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
    
    // Bind the socket to localhost port 12345
    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("bind failed\n");
        exit(EXIT_FAILURE);
    }
    
    // Listen for incoming connections
    if (listen(server_fd, 3) == SOCKET_ERROR) {
        printf("listen failed\n");
        exit(EXIT_FAILURE);
    }
    
    printf("Server listening on port %d\n", PORT);
    
    while (1) {
        // Accept a new connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&client, (int*)&addrlen)) == INVALID_SOCKET) {
            printf("accept failed\n");
            exit(EXIT_FAILURE);
        }
        
        memset(buffer, 0, sizeof(buffer));
        
        // Receive data from the client
        if (recv(new_socket, buffer, sizeof(buffer), 0) == SOCKET_ERROR) {
            printf("recv failed\n");
            exit(EXIT_FAILURE);
        }
        
        // Add relationship based on received data
        char* childIp = strtok(buffer, " ");
        char* parentIp = strtok(NULL, " ");
        addRelationship(childIp, parentIp);
        
        // Print the hierarchical tree
        printTree(nodes[0], 0);
        
        // Send acknowledgment to the client
        const char* ack = "Data received successfully\n";
        send(new_socket, ack, strlen(ack), 0);
        
        closesocket(new_socket);
    }
    
    closesocket(server_fd);
    WSACleanup();
    
    return 0;
}
