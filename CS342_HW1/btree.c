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

    printf("%s", "Comparing value and the tree data: ");
    printf("%d", strcmp(val, (*tree)->data));

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

int main(int argc, char *argv[]) {

    // argv[0] is the name of the program
    const char *const fileName = argv[1]; //this is a constant pointer to a constant char. confusing stuff

    node *root;
    root = NULL; //root is initialized to some random address if not specified explicitly like this

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("input.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        printf("Retrieved line of length %zu :\n", read);
        printf("%s", line);

        if( line[read-1] == '\n' )
            line[read-1] = '\0';

        insert(&root, line);
    }

    /* Printing nodes of tree */
    printf("In Order Display\n");
    inorder(root);

    return 0;
}