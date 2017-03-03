#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef struct node {
    char *data;
    struct node *right;
} node;

void insert(node **tree, char *val) { //this is the btree from HW1 but only extends to the right. It is indeed a linked list.

    node *temp = NULL;
    if (!(*tree)) {
        temp = (node *) malloc(sizeof(node));
        temp->right = NULL;

        char *val_copy = strdup(val);
        temp->data = val_copy;
        *tree = temp;
        return;
    }


    insert(&(*tree)->right, val);
}

void inorder(node *tree, FILE* fp) {
    if (tree) {
        fprintf(fp, "%s\n", tree->data);
        inorder(tree->right, fp);
    }

}

typedef struct {
    //Or whatever information that you need
    char *inputFileName;
    const char *keyword;
    node **list;
} processFileArgs;

void *processFile(void *args) {

    processFileArgs *actual_args = args;

    //file reading part is taken from the C language library guides. The following is possibly the best way to do it since you don't have to allocate a pre-determined size for the input buffer
    FILE *fr;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    fr = fopen(actual_args->inputFileName, "r");

    if (fr == NULL) {
        printf("\n\nInput files don't exist. Exiting...\n\n");
        exit(EXIT_FAILURE);
    }


    int lineNumber = 1;

    while ((read = getline(&line, &len, fr)) != -1) {

        if (line[read - 1] ==
            '\n') //replace line endings with string terminators. Well, maybe this results in a memory leak because "someString\n\0" becomes "someString\0\0". Not sure about that though.
            line[read - 1] = '\0';

        char *line_copy;
        line_copy = (char *) malloc(sizeof(char) * (strlen(line)));
        strcpy(line_copy, line);

        char *tok = line, *end = line;
        while (tok != NULL) {
            strsep(&end, " ");

            if (strcmp(actual_args->keyword, tok) == 0) {

                char tmp[len];
                sprintf(tmp, "%s, %d: %s", actual_args->inputFileName, lineNumber, line_copy);
                insert((actual_args->list), tmp);

                break;
            }

            tok = end;
        }

        lineNumber++;
    }

    return 0;
}

int main(int argc, char **argv) {

    const char *const keyword = argv[1];

    const int numberOfInputs = atoi(argv[2]);

    node *root[numberOfInputs];
    processFileArgs *pfa[numberOfInputs];

    for (int i = 0; i < numberOfInputs; i++) {
        root[i] = NULL; //root is initialized to some random address if not specified explicitly like this
        pfa[i] = malloc(sizeof *pfa[0]);
    }

    int err;
    pthread_t tid[numberOfInputs];

    for (int i = 0; i < numberOfInputs; i++)
    {

        pfa[i]->inputFileName = argv[i + 3];
        pfa[i]->keyword = keyword;
        pfa[i]->list = &root[i];

        err = pthread_create(&(tid[i]), NULL, &processFile, pfa[i]);

        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));


        pthread_join(tid[i], NULL);


    }


    //now start merging the intermediary .txt files
    FILE *fw;

    fw = fopen(argv[numberOfInputs + 3], "w+");

    for (int i = 0; i < numberOfInputs; i++) {
        inorder(root[i], fw);
    }

    fclose(fw);

    for (int i = 0; i < numberOfInputs; i++) {
        free(root[i]);
        free(pfa[i]);
    }
}