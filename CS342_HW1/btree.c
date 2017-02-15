#include<stdlib.h>
#include<stdio.h>
#include <string.h>

typedef struct bin_node {
    char *data;
    struct bin_node *right, *left;
} node;

void insert(node **tree, char *val) {
    node *temp = NULL;
    if (!(*tree)) {
        temp = (node *) malloc(sizeof(node));
        temp->left = temp->right = NULL;
        temp->data = val;
        *tree = temp;
        return;
    }

    //do not insert the new string if it exists in the tree -> do not handle the case for strcmp == 0
    if (strcmp(val, (*tree)->data) < 0) {
        insert(&(*tree)->left, val);
    } else if (strcmp(val, (*tree)->data) > 0) {
        insert(&(*tree)->right, val);
    }

}



void inorder(node *tree) {
    if (tree) {
        inorder(tree->left);
        printf("%s\n", tree->data);
        inorder(tree->right);
    }
}

void deltree(node *tree) {
    if (tree) {
        deltree(tree->left);
        deltree(tree->right);
        free(tree);
    }
}

void main() {
    node *root;
    root = NULL; //root is initialized to some random address if not specified explicitly like this

    /* Inserting nodes into tree */
    insert(&root, "zzz");
    insert(&root, "asd");
    insert(&root, "ddd");

    /* Printing nodes of tree */
    printf("In Order Display\n");
    inorder(root);

    /* Deleting all nodes of tree */
    deltree(root);
}