#include <stdio.h>
#include <winsock2.h>

#define MAX_INPUTS 100

void printTree(char (*tree)[32], int size) {
    printf("Family Tree:\n");
    for (int i = 0; i < size; i++) {
        printf("%s\n", tree[i]);
    }
}

int main() {
    WSADATA wsa;
    SOCKET s, new_socket;
    struct sockaddr_in server, client;
    int c, recv_size;
    char senderIP[16];
    char parentIP[16];
    char tree[MAX_INPUTS][32]; // Maximum length of each entry is 32
    int tree_size = 0;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        printf("WSAStartup failed. Error Code : %d\n", WSAGetLastError());
        return 1;
    }

    // Create a socket
    if((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket : %d\n", WSAGetLastError());
        return 1;
    }

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(1234);

    // Bind
    if(bind(s,(struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed with error code : %d\n", WSAGetLastError());
        return 1;
    }

    // Listen to incoming connections
    listen(s, 3);

    // Accept and incoming connection
    printf("Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);
    new_socket = accept(s, (struct sockaddr *)&client, &c);
    if (new_socket == INVALID_SOCKET) {
        printf("Accept failed with error code : %d\n", WSAGetLastError());
        return 1;
    }
    printf("Connection accepted\n");

    strcpy(tree[tree_size++], "1.1.1.1");

    // Receive data from client
    while ((recv_size = recv(new_socket, senderIP, 16, 0)) > 0 && tree_size < MAX_INPUTS) {
        senderIP[recv_size] = '\0';
        recv_size = recv(new_socket, parentIP, 16, 0);
        parentIP[recv_size] = '\0';

        // Add the new entry to the tree
        sprintf(tree[tree_size], "%s -> %s", parentIP, senderIP);
        tree_size++;

        // Print the family tree
        printTree(tree, tree_size);

        // Send the family tree to the client
        for (int i = 0; i < tree_size; i++) {
            send(new_socket, tree[i], strlen(tree[i]), 0);
            send(new_socket, "\n", 1, 0);
        }
    }

    if (recv_size == SOCKET_ERROR) {
        printf("recv failed with error code : %d", WSAGetLastError());
    }

    closesocket(s);
    WSACleanup();

    return 0;
}
