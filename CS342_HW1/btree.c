#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct bin_node {
    char *data;
    struct bin_node *right, *left;
} node; //typedef bin_node -> node. Whenever I write node somewhere, compiler automatically replaces it with struct bin_node

void insert(node **tree, char *val) {
    node *temp = NULL;
    if (!(*tree)) {
        temp = (node *) malloc(sizeof(node));
        temp->left = temp->right = NULL;

        char * val_copy;
        val_copy = malloc(sizeof(char) * strlen(val));
        strcpy(val_copy, val);

        temp->data = val_copy;
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

void inorder(node *tree, FILE* fp) {
    if (tree) {
        inorder(tree->left, fp);
        //printf("%s\n", tree->data); // this part is going to be replaced by -write output to file-

        fprintf(fp, "%s\n", tree->data);

        inorder(tree->right, fp);
    }

}

int main(int argc, char *argv[]) {

    // argv[0] is the name of the program
    const char *const fileName = argv[1]; //this is a constant pointer to a constant char. confusing stuff

    node *root;
    root = NULL; //root is initialized to some random address if not specified explicitly like this

    //file reading part is taken from the C language library guides. The following is possibly the best way to do it since you don't have to allocate a pre-determined size for the input buffer
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("input.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {

        if( line[read-1] == '\n' ) //replace line endings with string terminators. Well, maybe this results in a memory leak because "someString\n\0" becomes "someString\0\0". Not sure about that though.
            line[read-1] = '\0';

        /* pointer errors sometimes figure out days to figure out. By not doing a deep copy in the insert function, I was overwriting the current root's *data.
         * always do a deep copy while working with strings and trees... */
        insert(&root, line);
    }

    fp = fopen("output.txt", "w+");
    inorder(root, fp);

    fclose(fp);

    return 0;
}