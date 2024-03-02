#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_IP_LENGTH 15  // Assuming IPv4 addresses
#define MAX_INPUTS 100

// Structure representing a node in the infection tree
typedef struct Node {
    char ip[MAX_IP_LENGTH + 1];
    struct Node* parent;
    struct Node* next;  // Next sibling
} Node;

// Structure to pass data to the thread function
typedef struct ThreadData {
    Node** root;             // Pointer to the root of the infection tree
    char ip[MAX_IP_LENGTH + 1];      // Current IP being processed
    char parentIp[MAX_IP_LENGTH + 1];  // Parent IP of the current IP
    pthread_mutex_t* mutex;  // Mutex for thread synchronization
} ThreadData;

// Function to create a new Node with the given IP address
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

// Function to insert a new Node with the given IP into the infection tree
void insertNode(Node** root, const char* ip, const char* parentIp) {
    Node* newNode = createNode(ip);
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    // If the tree is empty, make the new node the root
    if (*root == NULL) {
        *root = newNode;
    } else {
        // Traverse the tree to find the parent node based on the parent IP
        Node* parent = *root;
        while (parent != NULL) {
            // If the parent IP matches, insert the new node as a child
            if (strcmp(parent->ip, parentIp) == 0) {
                newNode->parent = parent;
                newNode->next = parent->next;
                parent->next = newNode;
                break;
            }
            parent = parent->next;
        }

        // If the parent IP is not found, print an error and free the allocated memory
        if (parent == NULL) {
            fprintf(stderr, "Parent IP not found: %s\n", parentIp);
            free(newNode);
        }
    }
}

// Function to process input in a thread-safe manner
void* processInput(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    pthread_mutex_lock(data->mutex);  // Lock the mutex to ensure thread safety
    insertNode(data->root, data->ip, data->parentIp);
    pthread_mutex_unlock(data->mutex);  // Unlock the mutex after processing
    return NULL;
}

// Function to print the infection tree in a hierarchical manner
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

    // Recursively print the next sibling and the children at the next level
    printTree(root->next, depth + 1);
}

// Main function
int main() {
    Node* root = NULL;               // Root of the infection tree
    char ip[MAX_IP_LENGTH + 1];      // Buffer for current IP input
    char parentIp[MAX_IP_LENGTH + 1];  // Buffer for parent IP input

    // Adding Patient Zero
    insertNode(&root, "1.1.1.1", "");  // No parent for patient zero

    printf("Enter up to 100 IPs and their infectors (Type QUIT to finish):\n");

    pthread_t threads[MAX_INPUTS];     // Array to store thread IDs
    ThreadData threadData[MAX_INPUTS];  // Array to store thread data
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // Mutex for thread synchronization

    int inputCount = 0;
    int done = 0;

    // Read inputs
    while (inputCount < MAX_INPUTS && !done) {

         // Read current IP
        if (scanf("%s", ip) != EOF) { 
            if (strcmp(ip, "QUIT") == 0) {

                // Exit loop if QUIT
                done = 1;
            } else {
                scanf("%s", parentIp);  // Read parent IP
                threadData[inputCount].root = &root;
                strncpy(threadData[inputCount].ip, ip, MAX_IP_LENGTH);
                threadData[inputCount].ip[MAX_IP_LENGTH] = '\0';
                strncpy(threadData[inputCount].parentIp, parentIp, MAX_IP_LENGTH);
                threadData[inputCount].parentIp[MAX_IP_LENGTH] = '\0';
                threadData[inputCount].mutex = &mutex;

                // Create a thread to process the input
                pthread_create(&threads[inputCount], NULL, processInput, &threadData[inputCount]);
                inputCount++;
            }
        } else {
            done = 1;  // Exit the loop on EOF
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < inputCount; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\nInfection Tree:\n");
    printTree(root, 0);  // Print the infection tree
    sleep(10);

    return 0;
}
