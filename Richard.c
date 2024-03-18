#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NODES 100

// Node structure to represent each IP address
typedef struct Node {
    char ip[16]; // Assuming IPv4 addresses
    struct Node* parent;
} Node;

Node* nodes[MAX_NODES]; // Array to hold all nodes
int numNodes = 0;

// Function to find a node by IP address
Node* findNode(const char* ip) {
    for (int i = 0; i < numNodes; i++) {
        if (strcmp(nodes[i]->ip, ip) == 0) {
            return nodes[i];
        }
    }
    return NULL;
}

// Function to create a new node
Node* createNode(const char* ip) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    strcpy(newNode->ip, ip);
    newNode->parent = NULL;
    return newNode;
}

// Function to add a new relationship (child to parent)
void addRelationship(const char* childIp, const char* parentIp) {
    Node* child = findNode(childIp);
    Node* parent = findNode(parentIp);

    if (child == NULL) {
        child = createNode(childIp);
        nodes[numNodes++] = child;
    }

    if (parent == NULL) {
        parent = createNode(parentIp);
        nodes[numNodes++] = parent;
    }

    child->parent = parent;
}

// Function to print the hierarchical tree
void printTree(Node* node, int depth) {
    if (node == NULL) return;

    for (int i = 0; i < depth; i++) {
        printf("\t");
    }
    printf("|-- %s\n", node->ip);

    printTree(node->parent, depth + 1);
}

int main() {
    // Example input (you can replace this with your input mechanism)
    addRelationship("192.168.1.1", "192.168.1.0");
    addRelationship("192.168.1.2", "192.168.1.1");
    addRelationship("192.168.1.3", "192.168.1.1");
    addRelationship("192.168.2.1", "192.168.2.0");
    addRelationship("192.168.2.2", "192.168.2.1");

    // Print the hierarchical tree
    for (int i = 0; i < numNodes; i++) {
        if (nodes[i]->parent == NULL) {
            printTree(nodes[i], 0);
        }
    }

    // Free allocated memory
    for (int i = 0; i < numNodes; i++) {
        free(nodes[i]);
    }

    return 0;
}
