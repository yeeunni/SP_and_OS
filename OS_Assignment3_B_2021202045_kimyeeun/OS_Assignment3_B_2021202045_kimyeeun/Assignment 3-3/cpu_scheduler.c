#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#define MAX 1000
typedef struct {
    int pid;         
    int arrival; 
    int burst; 
    int remaining;
    int start;
    int total_wait;
    int end;
    int final_end;
    
} process; //store information

int pc_queue[MAX]={0};
int queue_index=0;
int burstArr[MAX]={0};
int turnaround_time=0;
int response_time=0;
int wait_time=0;
int busy_time=0;
int switch_num=1;
// function to put ready queue
void enqueue_ready_queue(process pc[], int num_pc, int ready[], int *ready_end, int current_time) {
    for (int i = 0; i < num_pc; i++) {
        if (pc[i].arrival <= current_time && pc[i].remaining > 0) {
            int in_queue = 0;  //for removing duplicated pc in ready queue
            for (int j = 0; j < *ready_end; j++) {
                if (ready[j] == i) {
                    in_queue = 1;
                    break;
                }
            }
            if (!in_queue) {
                ready[(*ready_end)++] = i;  //add to ready queue
                
            }
        }
    }
}

int* RR(process pc[], int num_pc, int slice) {
    int current_time = 0;
    int completed = 0;
    int ready[MAX] = {0};  // ready queue
    int ready_start = 0;   // points to the start of the ready queue
    int ready_end = 0;     // points to the end of the ready queue

    // initialize remaining time for each process
    for (int i = 0; i < num_pc; i++) {
        pc[i].remaining = pc[i].burst;
    }

    while (completed < num_pc) {
        // enqueue processes that have arrived and are not yet completed
        enqueue_ready_queue(pc, num_pc, ready, &ready_end, current_time);

        // if the ready queue has processes
        if (ready_start < ready_end) {
            int idx = ready[ready_start];  // Get the first process in the ready queue

            // execute the process for slice or remaining time, whichever is shorter
            int exec_time = (pc[idx].remaining > slice) ? slice : pc[idx].remaining;
            if (pc[idx].remaining == pc[idx].burst) {
                    pc[idx].start = current_time;
             }
            pc[idx].remaining -= exec_time;
            pc[idx].end = current_time + exec_time;
            current_time += exec_time;
	    
            for (int time = 0; time < exec_time; time++) {
                pc_queue[queue_index++] = idx;
                
            }
            enqueue_ready_queue(pc, num_pc, ready, &ready_end, current_time);
            // if process finished, update completed
            if (pc[idx].remaining == 0) {
                completed++;
                pc[idx].final_end = current_time;
                pc[idx].total_wait = pc[idx].final_end - pc[idx].arrival - pc[idx].burst;
                turnaround_time += pc[idx].final_end - pc[idx].arrival;
                wait_time += pc[idx].total_wait;
                response_time += (pc[idx].start - pc[idx].arrival);
                
                busy_time += pc[idx].burst;

                // move start of ready queue forward
                ready_start++;
            } else {
                // if process not finished, move it to the end of the ready queue
                ready[ready_end++] = idx;
                ready_start++;
            }
        } else {
            // no process is ready
            current_time++;
        }
    }

    return pc_queue;
}
int* SRTF(process pc[], int num_pc) {
    int current_time = 0;
    int completed = 0;
    
    
    //initalize all remain time
    for (int i = 0; i < num_pc; i++) {
        pc[i].remaining = pc[i].burst;
    }

    while (completed < num_pc) { 
        int min_index = -1;
        
        // search process which has the least remaining time
        for (int i = 0; i < num_pc; i++) {
            if (pc[i].arrival <= current_time && pc[i].remaining > 0) {
                if (min_index == -1 || pc[i].remaining < pc[min_index].remaining) {
                    min_index = i;
                }
            }
        }
        
        if (min_index != -1) { //if we have process to execute
            pc_queue[queue_index++] = min_index; 
            if(pc[min_index].remaining == pc[min_index].burst){
            pc[min_index].start = current_time;
            }
            pc[min_index].remaining--; //execute process
            
            
            current_time++;
            
            // check if process is done or not
            if (pc[min_index].remaining == 0) {
                completed++;
                pc[min_index].final_end = current_time;
                pc[min_index].total_wait = pc[min_index].final_end - pc[min_index].arrival - pc[min_index].burst;
                turnaround_time += pc[min_index].final_end - pc[min_index].arrival;
                wait_time += pc[min_index].total_wait;
                response_time += (pc[min_index].start - pc[min_index].arrival);
                //printf("pid: %d's response_time: %d\n",min_index,pc[min_index].start - pc[min_index].arrival);
                busy_time += pc[min_index].burst;
            }
        } else {
            current_time++; 
        }
    }
    
    return pc_queue;
}


int* SJF(process pc[], int num_pc) { 
    int current_time = 0;
    int completed = 0;
    int min_index;
    int min_burst;
    
    
    while (completed < num_pc) { //until all process's done
        min_index = -1;
        min_burst = 1001; 
        
        for (int i = 0; i < num_pc; i++) {    //search shortest job
            if (pc[i].arrival <= current_time && pc[i].burst > 0) {
                if (pc[i].burst < min_burst) {
                    min_burst = pc[i].burst;
                    min_index = i;
                }
            }
        }
        
        if (min_index != -1) {  //if we have process to execute
            pc[min_index].start = current_time; //store start time
            pc[min_index].total_wait = current_time - pc[min_index].arrival; //store waiting time of each pc
            current_time += pc[min_index].burst; //update current_time
            pc[min_index].final_end = current_time;
            int b = pc[min_index].burst;
	    while(b>0){  //insert to pc queue for gantt chart
	     pc_queue[queue_index] = min_index;
	     queue_index++;
	     b--;
	    }
            busy_time += pc[min_index].burst; //busy time
            pc[min_index].burst = 0; 
            completed++; //update completed
            wait_time += pc[min_index].total_wait;
            turnaround_time += pc[min_index].final_end - pc[min_index].arrival;
            response_time += pc[min_index].start - pc[min_index].arrival;
        } else {
            current_time++; //if we don't have any process to execute
        }
    }
    
    return pc_queue;
}

int* FCFS(process pc[], int num_pc){
 int current_time = 0;

 for (int i = 0; i < num_pc; i++) {

        if (pc[i].arrival > current_time) {
            current_time = pc[i].arrival;
        }
        pc[i].start=current_time;
        pc[i].total_wait = pc[i].start-pc[i].arrival;
        pc[i].final_end = current_time + pc[i].burst;
        int b = pc[i].burst;
        while(b>0){   //insert to pc_queue for gantt chart
        pc_queue[queue_index] = i;
        queue_index++;
        b--;
        }
        burstArr[i]=pc[i].burst;
        busy_time += pc[i].burst;
        current_time = pc[i].final_end;
        wait_time += pc[i].total_wait;
        
        turnaround_time += pc[i].final_end- pc[i].arrival;
        response_time += pc[i].start - pc[i].arrival;
        
    }
    return pc_queue; 
}

int main(int argc, char *argv[]) {
    
    if (argc < 3) {
        printf("more parameters\n");
    }
    char file_name[256];
    strcpy(file_name,argv[1]);
    
    
    FILE *file = fopen(file_name, "r");
    if (file == NULL) {
        perror("Can't open file");
        return 1;
    }

    process pc[MAX]; //make process
    int i = 0;
    while (fscanf(file, "%d %d %d", &pc[i].pid, &pc[i].arrival, &pc[i].burst) != EOF) { //read from file
       pc[i].start = 0;
       pc[i].total_wait = 0;
       pc[i].end = 0;
       pc[i].final_end = 0;
       i++;
    }
    int num_pc = i;
    fclose(file);
    
   
    /*for (int j = 0; j < num_pc; j++) {
        printf("%d\t%d\t\t%d\n", pc[j].pid, pc[j].arrival, pc[j].burst);
    }*/
    

    if (strcmp(argv[2], "FCFS") == 0) {
       FCFS(pc,num_pc);
    } 
    else if (strcmp(argv[2], "SJF") == 0) {
        SJF(pc,num_pc);
    }
    else if (strcmp(argv[2], "RR") == 0) {
        int slice = 0;
        if (argc > 3) {
        slice = atoi(argv[3]);
        } 
        else {
        printf("no slice parameter\n");
    }
        RR(pc,num_pc,slice);
    } 
    else if (strcmp(argv[2], "SRTF") == 0) {
        SRTF(pc,num_pc);
    } 
    else {
        printf("UNKNOWN FUNCTION: %s\n", argv[2]);
        return 1;
    }
   
    printf("Gantt Chart:\n"); //print Gantt chart
    printf("| ");
    for (int j = 0; j < queue_index; j++) {
        printf("P%d", pc_queue[j]+1);
        if(j < queue_index - 1 && (pc_queue[j] != pc_queue[j+1])){ //calculate context switch number
          switch_num++;
        }
        if (j < queue_index - 1) {
            printf(" | ");  
        }
    }
    printf("\n");
    printf("Average Waiting Time= %.2f\n",(double)wait_time/num_pc);
    printf("Average Turnaround Time=%.2f\n",(double)turnaround_time/num_pc);
    printf("Average Response Time=%.2f\n",(double)response_time/num_pc);
    printf("CPU Utilization=%.2f%\n",(busy_time/(busy_time+0.1*switch_num)*100));
    //printf("switch_num:%d",switch_num);
  

    return 0;
}
