#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define PORT 1234
#define MAX_INPUTS 100
#define MAX_IP_LENGTH 16

typedef struct Node {
    char ip[MAX_IP_LENGTH];
    struct Node *sons[MAX_INPUTS];
    int num_sons;
} Node;

Node *createNode(char *ip) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    strcpy(newNode->ip, ip);
    newNode->num_sons = 0;
    return newNode;
}

void addChild(Node *parent, Node *child) {
    if (parent->num_sons < MAX_INPUTS) {
        parent->sons[parent->num_sons++] = child;
    } else {
        printf("Cannot add more sons to %s, limit reached\n", parent->ip);
    }
}

void printTree(Node *root, int depth) {
    int i;
    for (i = 0; i < depth; i++) {
        printf("\t");
    }
    printf("|-- %s\n", root->ip);
    for (i = 0; i < root->num_sons; i++) {
        printTree(root->sons[i], depth + 1);
    }
}

int main() {
    WSADATA wsaData;
    SOCKET sockfd;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    char buffer[MAX_IP_LENGTH * 2];
    int numInputs = 0;
    Node *root = createNode("1.1.1.1");

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        return 1;
    }

    // Bind socket
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Bind to any available interface
    serverAddr.sin_port = htons(PORT);
    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) != 0) {
        printf("Socket binding failed\n");
        return 1;
    }

    printf("Listening on port %d\n", PORT);

    while (numInputs < MAX_INPUTS) {
        int bytesReceived = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (bytesReceived == SOCKET_ERROR) {
            printf("Error in receiving data\n");
            break;
        }
        buffer[bytesReceived] = '\0';
        char senderIP[MAX_IP_LENGTH], parentIP[MAX_IP_LENGTH];
        sscanf(buffer, "%s %s", senderIP, parentIP);

        Node *senderNode = createNode(senderIP);
        Node *parentNode = NULL;

        // Find parent node
        if (strcmp(parentIP, "1.1.1.1") != 0) {
            // Assume parent is already in the tree
            parentNode = root;
            // Search for the parent in the tree
            // You can implement a more efficient search if needed
            // This is a simple linear search
            int i;
            for (i = 0; i < numInputs; i++) {
                if (strcmp(root->sons[i]->ip, parentIP) == 0) {
                    parentNode = root->sons[i];
                    break;
                }
            }
        }

        addChild(parentNode, senderNode);
        printf("Family tree after input %d:\n", ++numInputs);
        printTree(root, 0);
    }

    closesocket(sockfd);
    WSACleanup();
    return 0;
}
