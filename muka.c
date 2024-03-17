#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 12345
#define MAX_INPUTS 100
#define MAX_NODES 100

struct Node {
    char ip[16];
    char parent_ip[16];
};

struct Node nodes[MAX_NODES];
int num_nodes = 0;

void addNode(char *ip, char *parent_ip) {
    strcpy(nodes[num_nodes].ip, ip);
    strcpy(nodes[num_nodes].parent_ip, parent_ip);
    num_nodes++;
}

void printTree(int node_index, int level) {
    for (int i = 0; i < level; i++)
        printf("    ");

    printf("%s\n", nodes[node_index].ip);

    for (int i = 0; i < num_nodes; i++) {
        if (strcmp(nodes[i].parent_ip, nodes[node_index].ip) == 0) {
            printTree(i, level + 1);
        }
    }
}

int main() {
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server, client;
    char sender_ip[16];
    char parent_ip[16];
    int c, recv_size;

    printf("Initializing Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return 1;
    }

    printf("Winsock initialized.\n");

    // Create socket
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket : %d", WSAGetLastError());
    }

    printf("Socket created.\n");

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Bind
    if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed with error code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    printf("Bind done.\n");

    // Main loop
    while (1) {
        c = sizeof(struct sockaddr_in);

        // Receive sender IP
        if ((recv_size = recvfrom(s, sender_ip, 16, 0, (struct sockaddr *)&client, &c)) == SOCKET_ERROR) {
            printf("recvfrom() failed with error code : %d", WSAGetLastError());
            exit(EXIT_FAILURE);
        }
        sender_ip[recv_size] = '\0';

        // Receive parent IP
        if ((recv_size = recvfrom(s, parent_ip, 16, 0, (struct sockaddr *)&client, &c)) == SOCKET_ERROR) {
            printf("recvfrom() failed with error code : %d", WSAGetLastError());
            exit(EXIT_FAILURE);
        }
        parent_ip[recv_size] = '\0';

        // Add node to array
        addNode(sender_ip, parent_ip);

        // Print family tree
        printf("Family Tree after adding node:\n");
        printTree(num_nodes - 1, 0);
    }

    closesocket(s);
    WSACleanup();

    return 0;
}
