#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 5585
#define MAX_CLIENTS 100

void draw_tree(char* ip, char* parent_ip, int level) {
    printf("%*s%s\n", level * 4, "", ip);
    if (strcmp(parent_ip, "1.1.1.1") == 0) {
        draw_tree("1.1.1.2", "1.1.1.1", level + 1);
        draw_tree("1.1.1.3", "1.1.1.1", level + 1);
    }
}

int main() {
    WSADATA wsa;
    SOCKET s, new_socket;
    struct sockaddr_in server, client;
    int c, client_num = 0;
    char client_ips[MAX_CLIENTS][20];
    char parent_ips[MAX_CLIENTS][20];

    printf("Initializing Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return 1;
    }
    printf("Initialized.\n");

    // Create socket
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
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
    puts("Bind done");

    // Listen to incoming connections
    listen(s, 3);

    // Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    
    while ((new_socket = accept(s, (struct sockaddr *)&client, &c)) != INVALID_SOCKET) {
        printf("Connection accepted\n");

        // Receive data from client
        char ip[20], parent_ip[20];
        recv(new_socket, ip, sizeof(ip), 0);
        recv(new_socket, parent_ip, sizeof(parent_ip), 0);

        // Store client IPs and parent IPs
        strcpy(client_ips[client_num], ip);
        strcpy(parent_ips[client_num], parent_ip);
        client_num++;

        // Draw tree
        printf("Tree for client %d:\n", client_num);
        draw_tree(ip, parent_ip, 0);

        // Send confirmation to client
        send(new_socket, "Received", 9, 0);

        if (client_num >= MAX_CLIENTS) {
            printf("Maximum number of clients reached. Exiting...\n");
            break;
        }
    }

    if (new_socket == INVALID_SOCKET) {
        printf("Accept failed with error code : %d", WSAGetLastError());
        return 1;
    }

    closesocket(s);
    WSACleanup();

    return 0;
}
