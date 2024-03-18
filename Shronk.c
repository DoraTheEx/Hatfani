#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NODES 100

// Node structure to represent each IP address
typedef struct Node {
    char ip[16]; // Assuming IPv4 addresses
    struct Node* parent;
    struct Node* children[MAX_NODES];
    int numChildren;
    int printed;
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
    newNode->numChildren = 0;
    newNode->printed = 0;
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
    parent->children[parent->numChildren++] = child;
}

// Function to print the hierarchical tree
void printTree(Node* node, int depth) {
    if (node == NULL || node->printed) return;

    for (int i = 0; i < depth; i++) {
        printf("\t");
    }
    printf("|-- %s\n", node->ip);
    node->printed = 1;

    for (int i = 0; i < node->numChildren; i++) {
        printTree(node->children[i], depth + 1);
    }
}

int main() {
    // Adding root node
    Node* root = createNode("1.1.1.1");
    nodes[numNodes++] = root;

    // Example input (you can replace this with your input mechanism)
    addRelationship("192.168.1.1", "192.168.1.0");
    addRelationship("192.168.1.2", "192.168.1.1");
    addRelationship("192.168.1.3", "192.168.1.1");
    addRelationship("192.168.2.1", "192.168.2.0");
    addRelationship("192.168.2.2", "192.168.2.1");

    // Print the hierarchical tree
    printTree(root, 0);

    // Free allocated memory
    for (int i = 0; i < numNodes; i++) {
        free(nodes[i]);
    }

    return 0;
}
