#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 1234
#define MAX_USERS 100
#define MAX_IP_LENGTH 16
#define BUFLEN 512


// Structure to represent a node in the family tree
struct Node {
    char ip[MAX_IP_LENGTH];
    struct Node *parent;
    struct Node *children[MAX_USERS];
    int num_children;
};

// Function to create a new node
struct Node* createNode(char ip[]) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    strcpy(newNode->ip, ip);
    newNode->parent = NULL;
    newNode->num_children = 0;
    return newNode;
}

// Function to add a child to a parent node
void addChild(struct Node* parent, struct Node* child) {
    parent->children[parent->num_children++] = child;
    child->parent = parent;
}

// Function to print the family tree
void printTree(struct Node* root, int level) {
    if (root == NULL)
        return;

    for (int i = 0; i < level; i++)
        printf("\t");

    printf("%s\n", root->ip);

    for (int i = 0; i < root->num_children; i++)
        printTree(root->children[i], level + 1);
}

int main() {
    WSADATA wsaData;
    SOCKET s;
    struct sockaddr_in server, si_other;
    int slen, recv_len;
    char buf[BUFLEN];
    struct Node *root = createNode("1.1.1.1");

    // Initialize winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed. Error Code : %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    // Create socket
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
        printf("socket() failed with error code : %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    // Bind socket to port
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("bind() failed with error code : %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    slen = sizeof(si_other);

    // Receive input from users
    for (int i = 0; i < MAX_USERS; i++) {
        printf("Waiting for input...\n");
        fflush(stdout);

        // Receive message from client
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*)&si_other, &slen)) == SOCKET_ERROR) {
            printf("recvfrom() failed with error code : %d\n", WSAGetLastError());
            exit(EXIT_FAILURE);
        }

        buf[recv_len] = '\0';
        char sender_ip[MAX_IP_LENGTH], parent_ip[MAX_IP_LENGTH];

        // Parse sender and parent IPs from received message
        sscanf(buf, "%s %s", sender_ip, parent_ip);

        // Create nodes for sender and parent if not already exist
        struct Node *sender_node = createNode(sender_ip);
        struct Node *parent_node = createNode(parent_ip);

        // Find parent node in the tree
        struct Node *cur = root;
        while (strcmp(cur->ip, parent_ip) != 0 && cur->parent != NULL)
            cur = cur->parent;

        // Add sender as child of parent
        addChild(cur, sender_node);

        // Print the updated family tree
        printf("Family tree after input %d:\n", i + 1);
        printTree(root, 0);
    }

    // Cleanup winsock
    closesocket(s);
    WSACleanup();

    return 0;
}
