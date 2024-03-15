#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define PORT 6969
#define MAX_INPUTS 100

struct Node {
    char ip[16];
    struct Node *parent;
    struct Node *next;
};

void print_tree(struct Node *root, int indent);
void insert_node(struct Node **root, char *sender_ip, char *parent_ip);

int main() {
    WSADATA wsaData;
    SOCKET sockfd;
    struct sockaddr_in serverAddr;
    int addrLen = sizeof(struct sockaddr_in);
    char sender_ip[16];
    char parent_ip[16];
    int num_inputs = 0;
    struct Node *root = NULL;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed.\n");
        return 1;
    }

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        fprintf(stderr, "Socket creation failed.\n");
        WSACleanup();
        return 1;
    }

    // Fill in server address struct
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&serverAddr, addrLen) != 0) {
        fprintf(stderr, "Connection failed.\n");
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    // Receive inputs and build tree
    while (num_inputs < MAX_INPUTS) {
        if (recv(sockfd, sender_ip, sizeof(sender_ip), 0) == SOCKET_ERROR) {
            fprintf(stderr, "Failed to receive sender IP.\n");
            break;
        }
        if (recv(sockfd, parent_ip, sizeof(parent_ip), 0) == SOCKET_ERROR) {
            fprintf(stderr, "Failed to receive parent IP.\n");
            break;
        }
        insert_node(&root, sender_ip, parent_ip);
        num_inputs++;
    }

    // Print tree
    printf("Hierarchical family tree:\n");
    print_tree(root, 0);

    // Close socket and cleanup
    closesocket(sockfd);
    WSACleanup();
    return 0;
}

void print_tree(struct Node *root, int indent) {
    if (root == NULL) return;
    
    printf("%*s%s - %s\n", indent * 2, "", root->ip, (root->parent != NULL) ? root->parent->ip : "Root");
    print_tree(root->next, indent);
    print_tree(root->parent, indent + 1);
}

void insert_node(struct Node **root, char *sender_ip, char *parent_ip) {
    struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return;
    }
    strcpy(new_node->ip, sender_ip);
    new_node->next = NULL;

    if (*root == NULL) {
        *root = new_node;
    } else {
        struct Node *parent = *root;
        while (parent->next != NULL) {
            parent = parent->next;
        }
        parent->next = new_node;
    }

    // Find parent node
    struct Node *parent_node = *root;
    while (parent_node != NULL && strcmp(parent_node->ip, parent_ip) != 0) {
        parent_node = parent_node->next;
    }
    if (parent_node != NULL) {
        new_node->parent = parent_node;
    } else {
        fprintf(stderr, "Parent node not found for IP: %s\n", sender_ip);
        free(new_node);
    }
}
