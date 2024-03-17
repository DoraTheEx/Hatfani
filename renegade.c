#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#define MAX_NODES 100
#define PORT 12345

struct Node {
    char ip[16];
    char parent_ip[16];
};

struct Node nodes[MAX_NODES];
int num_nodes = 0;

void addNode(char *ip, char *parent_ip) {
    if (num_nodes < MAX_NODES) {
        strcpy(nodes[num_nodes].ip, ip);
        strcpy(nodes[num_nodes].parent_ip, parent_ip);
        num_nodes++;
    }
}

void printFamilyTree() {
    printf("Family Tree:\n");
    for (int i = 0; i < num_nodes; i++) {
        printf("%s\n", nodes[i].ip);
        for (int j = 0; j < num_nodes; j++) {
            if (strcmp(nodes[j].parent_ip, nodes[i].ip) == 0) {
                printf("  %s\n", nodes[j].ip);
            }
        }
    }
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed.\n");
        return 1;
    }

    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    int client_len = sizeof(client_addr);
    char sender_ip[16], parent_ip[16];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        fprintf(stderr, "Socket creation failed.\n");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "Binding failed.\n");
        return 1;
    }

    while (1) {
        printf("Waiting for sender IP...\n");
        recvfrom(sockfd, sender_ip, sizeof(sender_ip), 0, (struct sockaddr *)&client_addr, &client_len);
        printf("Sender IP received: %s\n", sender_ip);

        printf("Waiting for parent IP...\n");
        recvfrom(sockfd, parent_ip, sizeof(parent_ip), 0, (struct sockaddr *)&client_addr, &client_len);
        printf("Parent IP received: %s\n", parent_ip);

        addNode(sender_ip, parent_ip);
        printFamilyTree();
    }

    closesocket(sockfd);
    WSACleanup();

    return 0;
}
