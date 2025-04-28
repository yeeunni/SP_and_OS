#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/time.h> 
#include <time.h>

#define MAX_PROCESSES 8

int fork_sum(int *nums, int start, int end){
  //if elements are 2 ->divide to 1, 1
  if (end - start == 1) {
  return nums[start];} 
  //base condition
  else if (end - start == 0){
  return 0;
  }

  int mid = (start + end) / 2; //divide criteria
  pid_t pid = fork();

    if (pid == 0) { //child
        int left_sum = fork_sum(nums, start, mid); //divide and conquer
        if (left_sum > 256) {
            left_sum %= 256; 
        }
        exit(left_sum);
    } 
    else { //parent
        int child_sum;
        
        int right_sum = fork_sum(nums, mid, end); //divide and conquer
        if (right_sum > 256) {
            right_sum %= 256;  
        }
        wait(&child_sum);
        int left_sum = (child_sum>>8); //shift 8bits
     int total_sum = left_sum + right_sum;
     FILE *f_write = fopen("temp.txt", "a"); //wrtie result
 
     fprintf(f_write, "%d\n",total_sum);
    
    fclose(f_write);   //close stream
     return left_sum + right_sum; //merge
     }
}





int main() {
    struct timespec start, end;
    int temp_num[MAX_PROCESSES * 2];
    FILE *f_read = fopen("temp.txt", "r"); //read from "temp.txt"
   
    if (f_read == NULL) {
        perror("Error opening file");
        return 1;
    }
    int line = 0;
    while (fscanf(f_read, "%d", &temp_num[line]) != EOF) { //store to temp_num array
        //printf("[%d] ", nums[line]);
        line++;
    }
    fclose(f_read);
    int total_sum=0;
    // store start time
    clock_gettime(CLOCK_MONOTONIC, &start);

    total_sum = fork_sum(temp_num,0,line); //execute

    // store end time
    clock_gettime(CLOCK_MONOTONIC, &end);

    // calculate
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("value of fork : %d", total_sum);
    printf("\n%.6f\n", elapsed);

    return 0;
}

