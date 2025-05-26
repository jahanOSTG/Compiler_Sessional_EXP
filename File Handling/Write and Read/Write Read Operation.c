#include <stdio.h>

int main() {
    FILE *fptr;
    int num;

    // Create and write to the file
    fptr = fopen("my.txt", "w");
    if (fptr == NULL) {
        printf(" Error creating file.\n");
        return 1;
    }

    printf("Enter a number: ");
    scanf("%d", &num);

    fprintf(fptr, "%d", num);  // Write number to file
    fclose(fptr);  // Close after writing
    printf("Number saved to file successfully.\n");

    // read operation
    fptr = fopen("my.txt", "r");
    if (fptr == NULL) {
        printf(" Error reading file.\n");
        return 1;
    }

    int readNum;
    if (fscanf(fptr, "%d", &readNum) == 1) {
        printf(" Number read from file: %d\n", readNum);
    } else {
        printf(" Could not read number from file.\n");
    }

    fclose(fptr);  // Final close
    return 0;
}
