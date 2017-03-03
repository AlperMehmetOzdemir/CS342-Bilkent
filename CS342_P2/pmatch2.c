#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LENGTH 256

void processFile(char *inputFileName, const char *keyword, FILE *fw) {

    //file reading part is taken from the C language library guides. The following is possibly the best way to do it since you don't have to allocate a pre-determined size for the input buffer
    FILE *fr;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    fr = fopen(inputFileName, "r");
    if (fr == NULL) {
        printf("\n\nInput files don't exist. Exiting...\n\n");
        exit(EXIT_FAILURE);
    }

    int lineNumber = 1;

    while ((read = getline(&line, &len, fr)) != -1) {

        if (line[read - 1] ==
            '\n') //replace line endings with string terminators. Well, maybe this results in a memory leak because "someString\n\0" becomes "someString\0\0". Not sure about that though.
            line[read - 1] = '\0';

        char *line_copy = strdup(line);

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
}

int main(int argc, char **argv) {

    //argv[0] is the name of the program hello 3 in1.txt in2.txt in3.txt out.txt

    const char *const keyword = argv[1];
    const int numberOfInputs = atoi(argv[2]);
    const char *const outputFileName = argv[numberOfInputs + 3];

    FILE *fw;
    fw = fopen(outputFileName, "w+");

    int fd[2 * numberOfInputs];

    FILE *fpin[2 * numberOfInputs];
    FILE *fpout[2 * numberOfInputs];

    for (int i = 0; i < numberOfInputs; i++) {
        pipe(&fd[2*i]);
    }

    for (int i = 0; i < numberOfInputs; i++) {

        pid_t childProcessId = fork();

        if (childProcessId < 0) {
            printf("\nCannot create child process. Exiting...");
            exit(1);
        } else if (childProcessId == 0) {
            close(fd[2 * i]); /* Close unused end*/

            fpout[2 * i] = fdopen(fd[2 * i + 1], "w");
            processFile(argv[i + 3], keyword, fpout[2 * i]);
            fclose(fpout[2 * i]);

            _exit(0); //_exit and exit has a difference...
        }

        else {
            close(fd[2 * i + 1]); /* Close unused end*/

            fpin[2 * i] = fdopen(fd[2 * i], "r");

            char line[MAX_LENGTH];
            while (fgets(line, MAX_LENGTH, fpin[2 * i]) != NULL)
                fprintf(fw, "%s", line);

            fclose(fpin[2 * i]);

            close(fd[2 * i]); /* Close used end */

            wait(0); //wait for all children process to exit
        }
    }



    return 0;
}
