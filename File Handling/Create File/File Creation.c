#include<stdio.h>
int main(){
    FILE *fptr;

fptr = fopen("myfile.txt","w");

fclose(fptr);
}
