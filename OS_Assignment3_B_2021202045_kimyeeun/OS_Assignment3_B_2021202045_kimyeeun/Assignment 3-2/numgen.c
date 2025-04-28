#include <stdio.h>
#define MAX_PROCESSES 8

int main() {
    FILE *f_write = fopen("temp.txt", "w"); //write to temp.txt
    for (int i = 0; i < MAX_PROCESSES * 2; i++) {
        fprintf(f_write, "%d\n", i + 1);
    }
    fclose(f_write);  //close stram
    return 0;
}

