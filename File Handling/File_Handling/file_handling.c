
#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *fp;
    char filename[] = "myfile.txt";
    char text[1000];

    fp = fopen(filename, "w");

    if (fp == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    printf("Enter text to write to the file:\n");

    fgets(text, sizeof(text), stdin);
    fprintf(fp, "%s", text);

    fclose(fp);

    printf("Text written to '%s' successfully.\n", filename);

    return 0;
}
