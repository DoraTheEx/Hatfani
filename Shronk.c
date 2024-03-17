#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define MAX_NODES 100
#define PORT 12345

// Structure to hold information about each node
typedef struct {
    char ip[16]; // Assuming IPv4 addresses
    char parent_ip[16]; // Parent node's IP
} Node;

Node nodes[MAX_NODES]; // Array to store nodes
int num_nodes = 0; // Number of nodes

// Function to print family tree with proper indentation
void printFamilyTree(int index, int level) {
    for (int i = 0; i < level; i++) {
        printf("  "); // Two spaces for each level of indentation
    }
    printf("%s\n", nodes[index].ip);
    for (int i = 0; i < num_nodes; i++) {
        if (strcmp(nodes[i].parent_ip, nodes[index].ip) == 0) {
            printFamilyTree(i, level + 1); // Recursive call for child nodes
        }
    }
}

int main() {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in server_addr;
    int addr_len = sizeof(struct sockaddr_in);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed\n");
        return 1;
    }

    // Create socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        fprintf(stderr, "Socket creation failed\n");
        return 1;
    }

    // Bind to port
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        fprintf(stderr, "Bind failed\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    printf("Listening for connections...\n");

    // Receive and process data
    while (1) {
        char buffer[32];
        struct sockaddr_in client_addr;

        if (recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &addr_len) == SOCKET_ERROR) {
            fprintf(stderr, "Error in recvfrom\n");
            closesocket(sock);
            WSACleanup();
            return 1;
        }

        // Extract sender IP and parent IP
        char sender_ip[16];
        char parent_ip[16];
        sscanf(buffer, "%s %s", sender_ip, parent_ip);

        // Store the node information
        strcpy(nodes[num_nodes].ip, sender_ip);
        strcpy(nodes[num_nodes].parent_ip, parent_ip);
        num_nodes++;

        // Print family tree
        printf("Family Tree:\n");
        for (int i = 0; i < num_nodes; i++) {
            if (strcmp(nodes[i].parent_ip, "") == 0) { // Root node
                printFamilyTree(i, 0);
            }
        }
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
