#include <stdio.h>
#include <stdlib.h>

#define MAX 1000

//function used in Optimal
int calculate_replacement_optimal(int current,int page_frame_count, int *page_frame_arr, int *rf_string, int *when_recall, int rf_count) {
    int point_to_replace=-1;
    
   for(int i=0;i<page_frame_count;i++){  //check which one(of page number in page frame) is the furthese in the future
   
    for(int j=current;j<rf_count;j++){
    	if(page_frame_arr[i] == rf_string[j]){
    		when_recall[i] = j;  //array that stores when this number would be recalled again
    		break;
    	}
    	else{
    		when_recall[i]=1001; //it means this number is never recalled in the future
    	}
    }
    }
 
    int max_value = -1;  
    int max_index = -1;
     for(int i=0;i<page_frame_count;i++){
     
      
       if (when_recall[i] > max_value) {  //caculate the furthest component
            max_value = when_recall[i];  
            max_index = i; 
        }
     }
     point_to_replace=max_index; //this would be replaced

    return point_to_replace;
}
int calculate_replacement_clock(int page_frame_count, int *page_frame_arr, int *rf_string, int *use_bit, int rf_count, int *clock_hand) {
    while (1) {
        if (use_bit[*clock_hand] == 0) {
            // need to change
            return *clock_hand;
        } else {
            //use this page -> turn it to 0 -> move to next hand
            use_bit[*clock_hand] = 0;
            *clock_hand = (*clock_hand + 1) % page_frame_count;
        }
    }
}

int calculate_replacement_lru(int current,int page_frame_count, int *page_frame_arr, int *rf_string, int *when_recall, int rf_count) {
    int point_to_replace=-1;
    
   for(int i=0;i<page_frame_count;i++){
   
    for(int j=current;j>-1;j--){ //from current to the past, which one is the least recently used
        when_recall[i] = -1;
    	if(page_frame_arr[i] == rf_string[j]){
    		when_recall[i] = j;
    		break;
    	}
    }
    }
    int min_value = 1001;  
    int min_index = 1001;
     for(int i=0;i<page_frame_count;i++){
      
       if (when_recall[i] < min_value) {
            min_value = when_recall[i];   //caculate the furthest component(past) from current 
            min_index = i; 
        }
     }
     point_to_replace=min_index; //this would be replaced
    
    return point_to_replace;
}

void clock(int page_frame_count, int *page_frame_arr, int *rf_string, int rf_count) {
    int fault = 0;
    int hit = 0;
    int use_bit[MAX] = {0}; // use bit
    int clock_hand = 0; // clock hand to the page that would be checked
    int point_to_replace;
    
    // initialize
    for (int i = 0; i < page_frame_count; i++) {
        page_frame_arr[i] = -1;
    }
    
    for (int j = 0; j < rf_count; j++) {
        int isHit = 0;
        
        // check if it is hit or not
        for (int i = 0; i < page_frame_count; i++) {
            if (rf_string[j] == page_frame_arr[i]) {
                hit++;
                isHit = 1;
                use_bit[i] = 1; // reference -> set use bit to 1
                break;
            }
        }
        
        if (!isHit) {
            //page fault
            fault++;
            point_to_replace = calculate_replacement_clock(page_frame_count, page_frame_arr, rf_string, use_bit, rf_count, &clock_hand);
            page_frame_arr[point_to_replace] = rf_string[j];
            use_bit[point_to_replace] = 1; //reference -> use bit to 1
        }
    }
    
    //print output
    printf("Clock Algorithm:\nNumber of Page Faults: %d \n", fault);
    printf("Page Fault Rate: %.2f%%\n", ((double)fault / rf_count) * 100);
}
void optimal(int page_frame_count, int *page_frame_arr, int *rf_string, int rf_count){ //optimal policy algo
    int fault=0;
    int hit=0;
    int when_recall[MAX]; //store when this index's number refereces again
    int point_to_replace;
    //initialize
     for(int i=0;i<page_frame_count;i++){
    	page_frame_arr[i]=-1;
    } 
    if(page_frame_count<MAX){
    page_frame_arr[page_frame_count+1]=-1;
    }
    for(int j=0;j<rf_count;j++){
    	int isHit=0;
    	for(int i=0;i<page_frame_count;i++){

    		//hit
    		if(rf_string[j] == page_frame_arr[i]){
    			hit++;
    			isHit=1;
    			break;
    		}
    	}
    	//fault
    	if(!isHit){
    		fault++;
    		point_to_replace = calculate_replacement_optimal(j,page_frame_count, page_frame_arr, rf_string, when_recall, rf_count); 
    		 
    		page_frame_arr[point_to_replace] = rf_string[j]; //replace
    	}

    }
    
    printf("Optimal Algorithm:\nNumber of Page Faults: %d \n", fault);
	    
    printf("Page Falut Rate: %.2f%\n", ((double)fault/rf_count)*100);
    
}

void lru(int page_frame_count, int *page_frame_arr, int *rf_string, int rf_count){ //lru policy algo
    int fault=0;
    int hit=0;
    int when_recall[MAX]; //store when this index's number was referenced
    int point_to_replace;
    //initialize
     for(int i=0;i<page_frame_count;i++){
    	page_frame_arr[i]=-1;
    } 
    if(page_frame_count<MAX){
    page_frame_arr[page_frame_count+1]=-1;
    }
    
    for(int j=0;j<rf_count;j++){
    	int isHit=0;
    	for(int i=0;i<page_frame_count;i++){
       
    		//hit
    		if(rf_string[j] == page_frame_arr[i]){
    			hit++;
    			isHit=1;
    			break;
    		}
    	}
    	//fault
    	if(!isHit){
    		fault++;
    		int current=j;
    		point_to_replace=calculate_replacement_lru(current,page_frame_count, page_frame_arr, rf_string, when_recall, rf_count);
    		
    		page_frame_arr[point_to_replace] = rf_string[j]; //replace
    	}

    }
    
    printf("LRU Algorithm:\nNumber of Page Faults: %d \n", fault);
	    
    printf("Page Falut Rate: %.2f%\n", ((double)fault/rf_count)*100);
    
}

void fifo(int page_frame_count, int *page_frame_arr, int *rf_string, int rf_count){ //fifo policy algo
    int fault=0;
    int hit=0;
    int point_to_replace;
    int oldest=0; //store index of page frame which one is the oldest so this would be out
    
     for(int i=0;i<page_frame_count;i++){
    	page_frame_arr[i]=-1;
    } 
    if(page_frame_count<MAX){
    page_frame_arr[page_frame_count+1]=-1;
    }
    
    for(int j=0;j<rf_count;j++){  // find whether it is hit or not
    	int isHit=0;
    	for(int i=0;i<page_frame_count;i++){
    		
    		if(rf_string[j] == page_frame_arr[i]){
    			hit++;
    			isHit=1;
    			break;
    		}
    	}
    	if(!isHit){
    		fault++;
    		point_to_replace = oldest; 
    		
    		if(oldest == page_frame_count-1){  
    		   oldest = 0;
    		}
    		else{oldest++;} //update oldest
    		page_frame_arr[point_to_replace] = rf_string[j]; //replace
    		
    	}

    }
    
    printf("FIFO Algorithm:\nNumber of Page Faults: %d \n", fault);
	    
    printf("Page Falut Rate: %.2f%\n", ((double)fault/rf_count)*100);


}

int main(int argc, char *argv[]) {
    if (argc != 2) { 
        printf("need more parameter\n");
        return 1;
    }

    FILE *file = fopen(argv[1], "r"); //open file stream to read
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    int page_frame_count;
    int page_frame_arr[MAX];
    
   
    if (fscanf(file, "%d", &page_frame_count) != 1) { //page_frame_count
        printf("Can't open file\n");
        fclose(file);
        return 1;
    }
   

    int rf_string[MAX];
    int rf_count = 0;

    while (fscanf(file, "%d", &rf_string[rf_count]) == 1 && rf_count < MAX) { //reference string
        rf_count++;
    }
        
    fclose(file); //close file stream
    
    optimal(page_frame_count, page_frame_arr, rf_string, rf_count); //optimal algo
    printf("\n");
    fifo(page_frame_count, page_frame_arr, rf_string, rf_count); //fifo algo
    printf("\n");
    lru(page_frame_count, page_frame_arr, rf_string, rf_count); //lru algo
    printf("\n");
    clock(page_frame_count, page_frame_arr, rf_string, rf_count); //clock algo

    return 0;
}

