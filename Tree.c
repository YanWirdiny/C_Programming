#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Tree node structure
typedef struct TREE {
    int id;
    char* name;
    struct TREE* first;       // Pointer to the first child
    struct TREE* nextSibling; // Pointer to the next sibling
} Tree;

// Function to create a new tree node
Tree* createTree(int id, const char* name) {
    Tree* newNode = (Tree*)malloc(sizeof(Tree));
    if (!newNode) {
        perror("Memory allocation failed");
        exit(1);
    }

    newNode->id = id;
    newNode->name = strdup(name); // Allocate and copy name
    newNode->first = NULL;
    newNode->nextSibling = NULL;
    return newNode;
}

// Function to add a child node to a parent
void addChild(Tree* parent, int id, const char* name) {
    if (!parent) return;

    Tree* child = createTree(id, name);
    if (parent->first == NULL) {
        parent->first = child;
    } else {
        Tree* temp = parent->first;
        while (temp->nextSibling != NULL) {
            temp = temp->nextSibling;
        }
        temp->nextSibling = child;
    }
}

// Helper function to recursively free a subtree
void freeSubtree(Tree* node) {
    if (node == NULL) return;

    Tree* child = node->first;
    while (child != NULL) {
        Tree* next = child->nextSibling;
        freeSubtree(child);
        child = next;
    }

    free(node->name);
    free(node);
}

// Function to remove a specific child and its subtree
void removeChild(Tree* parent, Tree* child) {
    if (parent == NULL || parent->first == NULL || child == NULL) return;

    Tree* temp = parent->first;
    Tree* prev = NULL;

    // Locate the child node
    while (temp != NULL && temp->id != child->id) {
        prev = temp;
        temp = temp->nextSibling;
    }

    if (temp == NULL) return; // Child not found

    // Unlink the child
    if (prev == NULL) {
        parent->first = temp->nextSibling; // Child is the first node
    } else {
        prev->nextSibling = temp->nextSibling;
    }

    // Free the child and its subtree
    freeSubtree(temp);
}

// Function to move a child and its subtree from one parent to another
void moveChild(Tree* sourceParent, Tree* destinationParent, Tree* child) {
    if (!sourceParent || !destinationParent || !child) return;

    // Unlink child from source parent
    Tree* temp = sourceParent->first;
    Tree* prev = NULL;

    while (temp != NULL && temp->id != child->id) {
        prev = temp;
        temp = temp->nextSibling;
    }

    if (temp == NULL) return; // Child not found

    // Remove from source parent
    if (prev == NULL) {
        sourceParent->first = temp->nextSibling; // Child was the first child
    } else {
        prev->nextSibling = temp->nextSibling;
    }

    temp->nextSibling = NULL; // Detach the node

    // Add the subtree to the destination parent
    if (destinationParent->first == NULL) {
        destinationParent->first = temp;
    } else {
        Tree* current = destinationParent->first;
        while (current->nextSibling != NULL) {
            current = current->nextSibling;
        }
        current->nextSibling = temp;
    }
}

// Function to print the tree in a structured format
void printTree(Tree* root, int level) {
    if (root == NULL) return;

    // Indentation based on the level
    for (int i = 0; i < level; i++) {
        printf("  ");
    }
    printf("ID: %d, Name: %s\n", root->id, root->name);

    // Recursively print the children
    Tree* child = root->first;
    while (child != NULL) {
        printTree(child, level + 1);
        child = child->nextSibling;
    }
}

// Test the implementation
int main() {
    // Create root of the tree
    Tree* root = createTree(1, "Root");

    // Add children to root
    addChild(root, 2, "Child1");
    addChild(root, 3, "Child2");
    addChild(root, 4, "Child3");

    // Add sub-children
    addChild(root->first, 5, "SubChild1");
    addChild(root->first, 6, "SubChild2");

    printf("Original Tree:\n");
    printTree(root, 0);

    // Move SubChild1 to Child2
    printf("\nMoving SubChild1 to Child2...\n");
    moveChild(root->first, root->first->nextSibling, root->first->first);

    printf("\nTree After Moving:\n");
    printTree(root, 0);

    // Remove Child3
    printf("\nRemoving Child3...\n");
    removeChild(root, root->first->nextSibling->nextSibling);

    printf("\nTree After Removing Child3:\n");
    printTree(root, 0);

    // Free the entire tree
    freeSubtree(root);

    return 0;
}