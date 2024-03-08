#include <stdio.h>
#include <stdlib.h>

char* readFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        return NULL;
    }

    // Initialize variables
    int capacity = 10; // Initial capacity of the buffer
    int size = 0;      // Current size of the buffer
    char* buffer = (char*)malloc(capacity * sizeof(char)); // Allocate initial buffer
    if (buffer == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        return NULL;
    }

    // Read characters from the file
    int c;
    while ((c = fgetc(file)) != EOF) {
        buffer[size++] = (char)c;

        // Resize buffer if needed
        if (size >= capacity) {
            capacity *= 2; // Double the capacity
            char* newBuffer = (char*)realloc(buffer, capacity * sizeof(char));
            if (newBuffer == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                fclose(file);
                free(buffer);
                return NULL;
            }
            buffer = newBuffer;
        }
    }

    // Null-terminate the string
    buffer[size] = '\0';

    // Close the file
    fclose(file);

    return buffer;
}

int main() {
    const char* filename = "something.txt";
    char* content = readFile(filename);
    if (content != NULL) {
        printf("File content:\n%s\n", content);
        free(content);
    }
    return 0;
}
