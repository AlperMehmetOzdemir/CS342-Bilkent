#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

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

    fclose(fw);

}

int main(int argc, char **argv) {

    //clock_t start_time = clock();

    //argv[0] is the name of the program hello 3 in1.txt in2.txt in3.txt out.txt

    const char *const keyword = argv[1];
    const int numberOfInputs = atoi(argv[2]);

    for (int i = 3; i < numberOfInputs + 3; i++) {

        pid_t childProcessId = fork();

        if (childProcessId < 0) {
            printf("\nCannot create child process. Exiting...");
            exit(1);
        } else if (childProcessId == 0) {
            char str[10];
            sprintf(str, "%d.txt", i - 3);
            processFile(argv[i], str, keyword);
            _exit(0);
        } else
            wait(0); //wait for all children process to exit

    }

    //now start merging the intermediary .txt files
    FILE *fr, *fw;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    fw = fopen(argv[numberOfInputs + 3], "w+");

    for (int i = 0; i < numberOfInputs; i++) {
        char str[10];
        sprintf(str, "%d.txt", i);

        fr = fopen(str, "r");

        if (fr == NULL)
            exit(EXIT_FAILURE);

        while ((read = getline(&line, &len, fr)) != -1) {

            fprintf(fw, "%s", line);

        }

        remove(str);
    }

    fclose(fw);
    fclose(fr);

    //clock_t end_time = clock();

    //printf("The program took: %zd milliseconds.", end_time - start_time);

    return 0;
}
