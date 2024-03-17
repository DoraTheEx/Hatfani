#include <stdio.h>
#include <winsock2.h>

#define MAX_NODES 100
#define PORT 12345

typedef struct Node {
    char ip[16]; // IP address
    int parent_index; // Index of the parent node in the array
} Node;

Node nodes[MAX_NODES];
int num_nodes = 0;

void addNode(char* ip, int parent_index) {
    if (num_nodes < MAX_NODES) {
        strcpy(nodes[num_nodes].ip, ip);
        nodes[num_nodes].parent_index = parent_index;
        num_nodes++;
    } else {
        printf("Maximum number of nodes reached.\n");
    }
}

void printFamilyTree() {
    printf("Family Tree:\n");
    for (int i = 0; i < num_nodes; i++) {
        int indent = 0;
        int current_index = i;
        while (current_index != -1) {
            for (int j = 0; j < indent; j++) {
                printf("  "); // Two spaces for each level of indentation
            }
            printf("%s\n", nodes[current_index].ip);
            current_index = nodes[current_index].parent_index;
            indent++;
        }
        printf("\n");
    }
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    SOCKET server_socket;
    struct sockaddr_in server_addr;

    server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (server_socket == INVALID_SOCKET) {
        printf("Error creating socket.\n");
        WSACleanup();
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed.\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);
    char sender_ip[16];
    char parent_ip[16];

    while (1) {
        recvfrom(server_socket, sender_ip, sizeof(sender_ip), 0, (struct sockaddr *)&client_addr, &client_addr_len);
        recvfrom(server_socket, parent_ip, sizeof(parent_ip), 0, (struct sockaddr *)&client_addr, &client_addr_len);

        // Assume you have some function to determine the parent index based on the parent IP
        int parent_index = 0; // Placeholder for parent index
        addNode(sender_ip, parent_index);
        printFamilyTree();
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
