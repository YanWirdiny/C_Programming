#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Node structure definition
typedef struct NODE {
    int id;
    char* name;
    struct NODE* first;
    struct NODE* nextToFirst;
} Node;

// Function to create a new node
Node* createNode(int id, const char* name) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    newNode->id = id;
    newNode->name = strdup(name);
    newNode->first = NULL;
    newNode->nextToFirst = NULL;
    return newNode;
}

// Function to add a child to a parent node
void addChild(Node* parent, Node* child) {
    if (!parent || !child) return;

    // Check for cycles
    Node* temp = parent;
    while (temp) {
        if (temp == child) {
            fprintf(stderr, "ADD FAILED: Cycle detected! Cannot add node %d (%s) as child of %d (%s)\n",
                    child->id, child->name, parent->id, parent->name);
            return;
        }
        temp = temp->nextToFirst;
    }

    if (!parent->first) {
        parent->first = child; // Set as first child
    } else {
        temp = parent->first;
        while (temp->nextToFirst) {
            temp = temp->nextToFirst;
        }
        temp->nextToFirst = child; // Add as last sibling
    }

    fprintf(stderr, "ADD: Added node %d (%s) as child of %d (%s)\n",
            child->id, child->name, parent->id, parent->name);
}


// Function to remove a node
void removeNode(Node* root, int id, int keepSubtree) {
    if (!root || id == 0) return;

    Node* parent = NULL;
    Node* temp = root->first;

    while (temp) {
        if (temp->id == id) {
            fprintf(stderr, "REMOVE: Removing node %d (%s)\n", id, temp->name);

            if (parent) {
                parent->nextToFirst = temp->nextToFirst; // Detach node from siblings
            } else {
                root->first = temp->nextToFirst; // Detach node from parent's first child
            }

            if (!keepSubtree) {
                // Promote children of the removed node (if keepSubtree is false)
                Node* child = temp->first;
                while (child) {
                    Node* nextChild = child->nextToFirst;
                    child->nextToFirst = NULL; // Clear sibling link
                    fprintf(stderr, "REMOVE: Promoting child %d (%s) to root\n", child->id, child->name);
                    addChild(root, child);
                    child = nextChild;
                }
            }

            // Free the removed node
            free(temp->name);
            free(temp);
            return;
        }
        parent = temp;
        temp = temp->nextToFirst;
    }

    // Recursively check children
    temp = root->first;
    while (temp) {
        removeNode(temp, id, keepSubtree);
        temp = temp->nextToFirst;
    }
}


// Function to find a node by ID
Node* findNode(Node* root, int id) {
    if (!root) return NULL;
    if (root->id == id) return root;

    Node* result = findNode(root->first, id); // Search in children
    if (result) return result;

    return findNode(root->nextToFirst, id); // Search in siblings
}


// Function to move a node to a new parent
// Function to move a node along with all its children to a new parent
void moveNode(Node* root, int parentId, int childId) {
    if (!root || parentId == childId) {
        fprintf(stderr, "MOVE failed: Invalid IDs or cycle detected (parentId: %d, childId: %d)\n", parentId, childId);
        return;
    }

    Node* parent = findNode(root, parentId); // Find the new parent node
    Node* child = findNode(root, childId);   // Find the child node to move

    if (!parent) {
        fprintf(stderr, "MOVE failed: Parent ID %d not found\n", parentId);
        return;
    }
    if (!child) {
        fprintf(stderr, "MOVE failed: Child ID %d not found\n", childId);
        return;
    }

    fprintf(stderr, "MOVE: Moving node %d (%s) and its children to parent %d (%s)\n", 
            childId, child->name, parentId, parent->name);

    // Detach the child (and its subtree) from its current location
    Node* currentParent = NULL;
    Node* temp = root->first;

    while (temp) {
        if (temp == child) {
            if (currentParent) {
                currentParent->nextToFirst = temp->nextToFirst; // Detach child from siblings
            } else {
                root->first = temp->nextToFirst; // Detach child as the first child of root
            }
            temp->nextToFirst = NULL; // Clear child's sibling link
            break;
        }
        currentParent = temp;
        temp = temp->nextToFirst;
    }

    // Add the child (along with its subtree) to the new parent
    addChild(parent, child);
}



// Function to sort children by ID
void sortChildrenById(Node* parent) {
    if (!parent || !parent->first) return;

    for (Node* i = parent->first; i; i = i->nextToFirst) {
        for (Node* j = i->nextToFirst; j; j = j->nextToFirst) {
            if (i->id > j->id) {
                int tempId = i->id;
                char* tempName = i->name;

                i->id = j->id;
                i->name = j->name;

                j->id = tempId;
                j->name = tempName;
            }
        }
    }
}

// Function to sort children by name
void sortChildrenByName(Node* parent) {
    if (!parent || !parent->first) return;

    for (Node* i = parent->first; i; i = i->nextToFirst) {
        for (Node* j = i->nextToFirst; j; j = j->nextToFirst) {
            if (strcmp(i->name, j->name) > 0) {
                int tempId = i->id;
                char* tempName = i->name;

                i->id = j->id;
                i->name = j->name;

                j->id = tempId;
                j->name = tempName;
            }
        }
    }
}

// Function to print children of a node
void printTree(Node* parent) {
    if (!parent || !parent->first) return;

    Node* child = parent->first;
    while (child) {
        printf("%s", child->name);
        if (child->nextToFirst) printf(",");
        child = child->nextToFirst;
    }
    printf("\n");
}

// Main function
int main() {
    int N;
    scanf("%d", &N);

    if (N <= 0 || N > 1000000) { // Validate N
        fprintf(stderr, "Invalid number of nodes: %d\n", N);
        return 1;
    }

    Node* nodes[N];
    for (int i = 0; i < N; i++) {
        int id;
        char name[1000];
        scanf("%d %s", &id, name);

        if (id < 0 || id >= N) { // Validate ID
            fprintf(stderr, "Invalid ID: %d\n", id);
            return 1;
        }

        nodes[id] = createNode(id, name);
    }

    Node* root = nodes[0];
    char command[20];

    while (scanf("%s", command) != EOF) {
        if (strcmp(command, "ADD") == 0) {
            int parentId, childId;
            scanf("%d %d", &parentId, &childId);

            if (parentId < N && childId < N) {
                addChild(nodes[parentId], nodes[childId]);
            }
        } else if (strcmp(command, "REMOVE") == 0) {
            int id;
            scanf("%d", &id);
            if (id < N && id != 0) {
                removeNode(root, id,0);
            }
        } else if (strcmp(command, "MOVE") == 0) {
            int parentId, childId;
            scanf("%d %d", &parentId, &childId);
            if (parentId < N && childId < N) {
                moveNode(root, parentId, childId);
            }
        } else if (strcmp(command, "SORT_ID") == 0) {
            int id;
            scanf("%d", &id);
            if (id < N) {
                sortChildrenById(nodes[id]);
            }
        } else if (strcmp(command, "SORT_NAME") == 0) {
            int id;
            scanf("%d", &id);
            if (id < N) {
                sortChildrenByName(nodes[id]);
            }
        } else if (strcmp(command, "PRINT") == 0) {
            int id;
            scanf("%d", &id);
            if (id < N) {
                printTree(nodes[id]);
            }
        }
    }

    // Free allocated memory
    for (int i = 0; i < N; i++) {
        free(nodes[i]->name);
        free(nodes[i]);
    }

    return 0;
}
