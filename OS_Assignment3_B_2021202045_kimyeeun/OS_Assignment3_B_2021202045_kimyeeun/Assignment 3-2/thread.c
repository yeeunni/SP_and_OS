#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>

#define MAX_THREADS 8

typedef struct {
    int *nums;
    int start;
    int end;
    int result;
} ThreadData;        //store thread's data

void *thread_sum(void *arg) {
    //initalize values
    ThreadData *data = (ThreadData *)arg;
    int start = data->start;
    int end = data->end;
    int *nums = data->nums;

    // Base condition
    if (end - start == 1) {
        data->result = nums[start];
        return NULL;
    } else if (end - start == 0) {
        data->result = 0;
        return NULL;
    }

    int mid = (start + end) / 2;
    
    ThreadData left_data = {nums, start, mid, 0};
    ThreadData right_data = {nums, mid, end, 0};

    pthread_t left_thread, right_thread;

    // Create threads for left and right 
    pthread_create(&left_thread, NULL, thread_sum, &left_data);
    pthread_create(&right_thread, NULL, thread_sum, &right_data);

    // Wait for both threads to finish
    pthread_join(left_thread, NULL);
    pthread_join(right_thread, NULL);

    // Combine results
    data->result = (left_data.result + right_data.result);
    FILE *f_write = fopen("temp.txt", "a");  //write to file
 
     fprintf(f_write, "%d\n",data->result);
    
    fclose(f_write);
    return NULL;
}

int main() {
    struct timespec start, end;
    int temp_num[MAX_THREADS * 2];
    FILE *f_read = fopen("temp.txt", "r"); //read file
    
    if (f_read == NULL) {
        perror("Error opening file");
        return 1;
    }
    int line = 0;
    while (fscanf(f_read, "%d", &temp_num[line]) != EOF) { //store to temp_num array
        line++;
    }
    fclose(f_read);
    
    int total_sum = 0;
    ThreadData data = {temp_num, 0, line, 0};

    // Store start time
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Start thread summation
    thread_sum(&data);
    total_sum = data.result;

    // Store end time
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calculate elapsed time
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("value of thread : %d\n", total_sum);
    printf("%.6f\n", elapsed);

    return 0;
}

