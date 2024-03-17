#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define MAX_NODES 100

struct Node {
    char ip[16]; // assuming IPv4 addresses
    char parent_ip[16];
};

struct Node nodes[MAX_NODES];
int num_nodes = 0;

void printTree(char* parent_ip, int level) {
    for (int i = 0; i < num_nodes; i++) {
        if (strcmp(nodes[i].parent_ip, parent_ip) == 0) {
            for (int j = 0; j < level * 4; j++) {
                printf(" ");
            }
            printf("|-- %s\n", nodes[i].ip);
            printTree(nodes[i].ip, level + 1);
        }
    }
}

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    SOCKET s;
    struct sockaddr_in server;
    int slen = sizeof(server);
    char ip[16], parent_ip[16];

    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == INVALID_SOCKET) {
        printf("Could not create socket.\n");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(5585);

    if (bind(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed.\n");
        return 1;
    }

    while (1) {
        printf("Enter IP address: ");
        scanf("%s", ip);
        printf("Enter Parent IP address: ");
        scanf("%s", parent_ip);

        strcpy(nodes[num_nodes].ip, ip);
        strcpy(nodes[num_nodes].parent_ip, parent_ip);
        num_nodes++;

        if (num_nodes >= MAX_NODES) {
            printf("Maximum number of nodes reached.\n");
            break;
        }

        printf("Do you want to enter another node? (y/n): ");
        char choice;
        scanf(" %c", &choice);
        if (choice != 'y' && choice != 'Y') {
            break;
        }
    }

    printf("\nFamily Tree:\n");
    printTree("root", 0);

    closesocket(s);
    WSACleanup();
    return 0;
}
