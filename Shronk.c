// Function to print tree recursively
void printTree(TreeNode* root, int depth) {
    for (int i = 0; i < depth; i++) {
        printf("  "); // Indent based on depth
    }
    printf("%s\n", root->ip);
    for (int i = 0; i < root->num_children; i++) {
        printTree(root->children[i], depth + 1);
    }
}
