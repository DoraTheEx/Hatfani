#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_IP_LENGTH 15  // Assuming IPv4 addresses
#define MAX_INPUTS 100

typedef struct Node {
    char ip[MAX_IP_LENGTH + 1];
    struct Node* parent;
    struct Node* next;
} Node;

typedef struct ThreadData {
    Node** root;
    char ip[MAX_IP_LENGTH + 1];
    char parentIp[MAX_IP_LENGTH + 1];
} ThreadData;

Node* createNode(const char* ip) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode != NULL) {
        strncpy(newNode->ip, ip, MAX_IP_LENGTH);
        newNode->ip[MAX_IP_LENGTH] = '\0';  // Ensure null-termination
        newNode->parent = NULL;
        newNode->next = NULL;
    }
    return newNode;
}

void insertNode(Node** root, const char* ip, const char* parentIp) {
    Node* newNode = createNode(ip);
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    if (*root == NULL) {
        *root = newNode;
    } else {
        Node* parent = *root;
        while (parent != NULL) {
            if (strcmp(parent->ip, parentIp) == 0) {
                newNode->parent = parent;
                newNode->next = parent->next;
                parent->next = newNode;
                break;
            }
            parent = parent->next;
        }
        if (parent == NULL) {
            fprintf(stderr, "Parent IP not found: %s\n", parentIp);
            free(newNode);
        }
    }
}

void* processInput(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    insertNode(data->root, data->ip, data->parentIp);
    return NULL;
}

void printTree(Node* root, int depth) {
    if (root == NULL) return;

    for (int i = 0; i < depth; i++) {
        if (i % 2 == 0) {
            printf("│");
        } else {
            printf(" ");
        }
        printf("  ");
    }

    if (depth > 0) {
        printf("├─");
    }

    printf("%s\n", root->ip);

    printTree(root->next, depth + 1);
}

int main() {
    Node* root = NULL;
    char ip[MAX_IP_LENGTH + 1];
    char parentIp[MAX_IP_LENGTH + 1];

    // Adding Patient Zero
    insertNode(&root, "1.1.1.1", "");  // No parent for patient zero

    printf("Enter up to 100 IPs and their infectors (Ctrl+D to finish):\n");

    pthread_t threads[MAX_INPUTS];
    ThreadData threadData[MAX_INPUTS];

    int inputCount = 0;

    // Read inputs
    while (inputCount < MAX_INPUTS && scanf("%s %s", ip, parentIp) != EOF) {
        threadData[inputCount].root = &root;
        strncpy(threadData[inputCount].ip, ip, MAX_IP_LENGTH);
        threadData[inputCount].ip[MAX_IP_LENGTH] = '\0';
        strncpy(threadData[inputCount].parentIp, parentIp, MAX_IP_LENGTH);
        threadData[inputCount].parentIp[MAX_IP_LENGTH] = '\0';

        pthread_create(&threads[inputCount], NULL, processInput, &threadData[inputCount]);
        inputCount++;
    }

    // Wait for all threads to finish
    for (int i = 0; i < inputCount; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\nInfection Tree:\n");
    printTree(root, 0);

    return 0;
}
