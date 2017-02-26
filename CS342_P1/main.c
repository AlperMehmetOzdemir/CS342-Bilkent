#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void processFile(char *inputFileName, char *outputFileName, const char *keyword) {

    //file reading part is taken from the C language library guides. The following is possibly the best way to do it since you don't have to allocate a pre-determined size for the input buffer
    FILE *fr, *fw;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    fr = fopen(inputFileName, "r");
    fw = fopen(outputFileName, "w+");

    if (fr == NULL)
        exit(EXIT_FAILURE);

    int lineNumber = 1;

    while ((read = getline(&line, &len, fr)) != -1) {

        if( line[read-1] == '\n' ) //replace line endings with string terminators. Well, maybe this results in a memory leak because "someString\n\0" becomes "someString\0\0". Not sure about that though.
            line[read-1] = '\0';

        char *line_copy;
        line_copy = (char*)malloc(sizeof(char) * strlen(line));
        strcpy(line_copy, line);

        char *tok = line, *end = line;
        while (tok != NULL) {
            strsep(&end, " ");

            if (strcmp(keyword, tok) == 0) {
                fprintf(fw, "%s, %d: %s\n", inputFileName, lineNumber, line_copy);
                break;
            }

            tok = end;
        }

        lineNumber++;
    }

    fclose(fw);

}

int main(int argc, char **argv) {

    //argv[0] is the name of the program hello 3 in1.txt in2.txt in3.txt out.txt

    const char *const keyword = argv[1];

    printf("\nKeyword to search for: %s", keyword);

    const int numberOfInputs = atoi(argv[2]);

    //printf("\nNumber of Inputs: %d", numberOfInputs);


    for (int i = 3; i < numberOfInputs + 3; i++) {

        pid_t childProcessId = fork();

        if (childProcessId < 0) {
            printf("\nCannot create child process. Exiting...");
            exit(1);
        } else if (childProcessId == 0) {
            printf("\nChild (%d): %d\n", i + 1, getpid());
            printf("\nInput file name: %s", argv[i]);

            char str[10];
            sprintf(str, "%d.txt", i);

            processFile(argv[i], str, keyword);

            exit(0);
        }

    }

    /*if (child == 0)
        readFile(argv[3]);
    else
        readFile(argv[4]);*/


    /*const char *const outputFileName = argv[numberOfInputs + 3];

    printf("\nOutput file name:  %s", outputFileName);*/

    return 0;
}