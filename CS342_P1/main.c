#include <stdio.h>

int main(int argc, char *argv) {

    //argv[0] is the name of the program  3 in1.txt in2.txt in3.txt out.txt

    printf("%s\n", argv[1]);

    /*printf("%s\n", argv[2]);
    printf("%s\n", argv[3]);
    printf("%s\n", argv[4]);


    const int numberOfInputs = atoi(argv[2]);

    for (int i = 3; i < numberOfInputs + 3; i++) {
        printf(argv[i]);
    }

    const char *const outputFileName = argv[numberOfInputs + 4];


    printf("Hello, World!\n");*/
    return 0;
}

/*void readFile(char *inputFileName) {

    //file reading part is taken from the C language library guides. The following is possibly the best way to do it since you don't have to allocate a pre-determined size for the input buffer
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(inputFileName, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {

        if( line[read-1] == '\n' ) //replace line endings with string terminators. Well, maybe this results in a memory leak because "someString\n\0" becomes "someString\0\0". Not sure about that though.
            line[read-1] = '\0';

        printf("%s\n", line);
    }

    //fp = fopen(outputFileName, "w+");

    fclose(fp);

}*/