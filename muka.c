#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 5585
#define MAX_CLIENTS 100

struct Node {
    char ip[20];
    char parent_ip[20];
    struct Node* next;
};

struct Node* createNode(char* ip, char* parent_ip) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    strcpy(newNode->ip, ip);
    strcpy(newNode->parent_ip, parent_ip);
    newNode->next = NULL;
    return newNode;
}

void insertNode(struct Node** head, char* ip, char* parent_ip) {
    struct Node* newNode = createNode(ip, parent_ip);
    newNode->next = *head;
    *head = newNode;
}

void draw_tree(struct Node* root, char* current_ip, int level) {
    if (root == NULL)
        return;
    
    if (strcmp(root->parent_ip, current_ip) == 0) {
        printf("%*s%s\n", level * 4, "", root->ip);
        draw_tree(root->next, root->ip, level + 1);
    }

    draw_tree(root->next, current_ip, level);
}

int main() {
    WSADATA wsa;
    SOCKET s, new_socket;
    struct sockaddr_in server, client;
    int c, client_num = 0;
    struct Node* root = NULL;

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

        // Insert into the family tree
        insertNode(&root, ip, parent_ip);
        
        // Print the entire family tree
        printf("Family tree after client %d:\n", client_num + 1);
        draw_tree(root, "1.1.1.1", 0);

        // Send confirmation to client
        send(new_socket, "Received", 9, 0);

        client_num++;

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
