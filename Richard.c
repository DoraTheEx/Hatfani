#include <stdio.h>
#include <winsock2.h>

#define MAX_NODES 100
#define PORT 12345

typedef struct Node {
    char ip[16]; // IP address
    struct Node* parent; // Pointer to parent node
} Node;

Node nodes[MAX_NODES];
int num_nodes = 0;

void addNode(char* ip, Node* parent) {
    if (num_nodes < MAX_NODES) {
        strcpy(nodes[num_nodes].ip, ip);
        nodes[num_nodes].parent = parent;
        num_nodes++;
    } else {
        printf("Maximum number of nodes reached.\n");
    }
}

void printFamilyTree() {
    printf("Family Tree:\n");
    for (int i = 0; i < num_nodes; i++) {
        Node* current = &nodes[i];
        int indent = 0;
        while (current != NULL) {
            for (int j = 0; j < indent; j++) {
                printf("  "); // Two spaces for each level of indentation
            }
            printf("%s\n", current->ip);
            current = current->parent;
            indent++;
        }
        printf("\n");
    }
}

Node* findParent(char* parent_ip) {
    for (int i = 0; i < num_nodes; i++) {
        if (strcmp(nodes[i].ip, parent_ip) == 0) {
            return &nodes[i];
        }
    }
    return NULL;
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

        Node* parent = findParent(parent_ip);
        addNode(sender_ip, parent);
        printFamilyTree();
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
