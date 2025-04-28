#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILE_NUM 100        // Requirement - MAX_file_number: 100
#define NUM_BLOCKS 1024         // Requirement - MAX_num_block: 1024
#define BLOCK_SIZE 32           // Requirement - block_size(byte): 32
#define MAX_FILE_NAME 100       // Requirement - MAX_file_name: 100
#define FS_STAT "fs_state.dat"  // Disk File
#define MAX_BLOCK_NUM 1024
// File Entry
typedef struct {
    char filename[MAX_FILE_NAME];
    int start_block;
    int size;
} FileEnt;

// File system structure
typedef struct {
    int fat_table[NUM_BLOCKS];                  // FAT Table
    FileEnt directory[MAX_FILE_NUM];           // Directory
    char data_area[NUM_BLOCKS * BLOCK_SIZE];   // 1024 blocks * 32 bytes
} FileSystem;

FileSystem myfat;  // File System Instance

/* ===========================================
                  CONTROL API
   =========================================== */
int create_file(const char *filename);          // Create & allocate block
int write_file(const char *filename, const char *data);  // Write data to file
int read_file(const char *filename);            // Read data from file
int delete_file(const char *name);              // Delete the file
void list_files(void);                          // Display file list
void save_file_system(void);                    // Save the File System
void load_file_system(void);                    // Load the File System

// Save the File System to Disk
void save_file_system(void) {
    FILE *f = fopen(FS_STAT, "wb");
    if (f == NULL) {
        printf("Error: Could not save file system state.\n");
        return;
    }
    fwrite(&myfat, sizeof(FileSystem), 1, f);
    fclose(f);
}

// Restore the File System from Disk
void load_file_system(void) {
    FILE *f = fopen(FS_STAT, "rb");
    if (f == NULL) {
        printf("Warning: No saved state found. Starting fresh.\n");
        return;
    }
    fread(&myfat, sizeof(FileSystem), 1, f);
    fclose(f);
}

// File Creation
int create_file(const char *filename) {
    // Check file name
    for (int i = 0; i < MAX_FILE_NUM; i++) {
        if (strcmp(myfat.directory[i].filename, filename) == 0) {
            printf("File '%s' already exists.\n", filename);
            return -1;
        }
    }

    // allocate new file
    for (int i = 0; i < MAX_FILE_NUM; i++) {
        if (myfat.directory[i].filename[0] == '\0') {
            for (int j = 0; j < NUM_BLOCKS; j++) {
                if (myfat.fat_table[j] == 0) {
                    myfat.fat_table[j] = 0xFFFF;

                    strcpy(myfat.directory[i].filename, filename);
                    myfat.directory[i].start_block = j;
                    myfat.directory[i].size = 0;
                    printf("File '%s' created.\n", filename);
                    return 0;
                }
            }
        }
    }
    printf("File system full. Cannot create more files.\n");
    return -1;
}

int write_file(const char *filename, const char *data) {
    for (int i = 0; i < MAX_FILE_NUM; i++) {
        if (strcmp(myfat.directory[i].filename, filename) == 0) {
            int start_block = myfat.directory[i].start_block;
            int block = start_block;
            int remaining_data = strlen(data);
            int data_offset = 0;

            //if not initialzie
            if (start_block == -1) {
                printf("File '%s' not initialized. Use create command first.\n", filename);
                return -1;
            }

            // write to original block
            while (remaining_data > 0) {
                // compute remain
                int block_end = BLOCK_SIZE;
                while (block_end > 0 && myfat.data_area[block * BLOCK_SIZE + block_end - 1] == 0) {
                    block_end--;
                }

                int space_in_block = BLOCK_SIZE - block_end;
                int bytes_to_write = (remaining_data < space_in_block) ? remaining_data : space_in_block;

                //write to current block
                strncpy(&myfat.data_area[block * BLOCK_SIZE + block_end], &data[data_offset], bytes_to_write);
                data_offset += bytes_to_write;
                remaining_data -= bytes_to_write;
                myfat.directory[i].size += bytes_to_write;

                // allocate new block if exceed size
                if (remaining_data > 0) {
                    int next_block = -1;

                    // find empty one
                    for (int j = 0; j < MAX_BLOCK_NUM; j++) {
                        if (myfat.fat_table[j] == 0) { // not used
                            next_block = j;
                            break;
                        }
                    }

                    if (next_block == -1) {
                        printf("No more space available for file '%s'.\n", filename);
                        return -1; // fail to allocate
                    }

                    // update current block
                    myfat.fat_table[block] = next_block;

                    // initialize
                    myfat.fat_table[next_block] = -1; //mark the new end of new block
                    block = next_block;
                }
            }

            printf("Data written to '%s'.\n", filename);
            return 0;
        }
    }

    printf("File '%s' not found.\n", filename);
    return -1;
}


int read_file(const char *filename) {
    for (int i = 0; i < MAX_FILE_NUM; i++) {
        if (strcmp(myfat.directory[i].filename, filename) == 0) {
            int block = myfat.directory[i].start_block;
            int total_size = myfat.directory[i].size;
            int byte_read = 0;

            if (block == -1) {
                printf("File '%s' not initialized. Use create command first.\n", filename);
                return -1;
            }

            //printf("Content of '%s':\n", filename);
            //printf("Initial total_size: %d\n", total_size);  // total_size check

           //read data using loop
            while (block != -1 && total_size > 0) {
                int read_size = (total_size < BLOCK_SIZE) ? total_size : BLOCK_SIZE;

                // debugging
                //printf("Read size: %d, Block: %d\n", read_size, block);
               // printf("Data: %.40s\n", &myfat.data_area[block * BLOCK_SIZE]);

                 //print of reading
                for (int j = 0; j < read_size; j++) {
                    putchar(myfat.data_area[block * BLOCK_SIZE + j]);
                }


                total_size -= read_size;
                block = myfat.fat_table[block]; // move to next block
            }

            printf("\n");
            return 0;
        }
    }

    printf("File '%s' not found.\n", filename);
    return -1;
}



//File Delegation
int delete_file(const char*filename) {
	for(int i=0;i<MAX_FILE_NUM;i++){
		if(strcmp(myfat.directory[i].filename, filename)==0){
		   int start_block = myfat.directory[i].start_block;
		   int block = start_block;
		   
		  while (block != -1 && block != 0xFFFF){
		   	int next_block = myfat.fat_table[block];
		   	myfat.fat_table[block]=0;
		   	//printf("Deallocating block %d\n", block); 

		   	block = next_block;
		   }
		   myfat.directory[i].filename[0] = '\0';
		   myfat.directory[i].start_block = -1; //set new start_block
            	   myfat.directory[i].size = 0; // set size of file
		   printf("File '%s' deleted.\n",filename);
		   
		   return 0;
		}
	
	}
	printf("File '%s' not found.\n",filename);
	return -1;

}


// Display a list of all files in the system
void list_files() {
    printf("Files in the file system:\n");
    int file_count = 0;

    for (int i = 0; i < MAX_FILE_NUM; i++) {
        if (myfat.directory[i].filename[0] != '\0') {
            printf("File: %s, Size: %d bytes\n", myfat.directory[i].filename, myfat.directory[i].size);
            file_count++;
        }
    }

    if (file_count == 0) {
        printf("No files found in the system.\n");
    }
}
//function for input
void execute_cmd(char *cmd, char *filename, char*data, int num){
	if(strcmp(cmd, "create")==0){
		if(num != 3){
		printf("Usage: create <filename>\n");
		}
		else{create_file(filename);}
	}
	else if(strcmp(cmd, "write")==0){
		if(num != 4){
		printf("Usage: write <filename> <data>\n");
		}
		else{write_file(filename,data);}
	}
	else if(strcmp(cmd, "read")==0){
		if(num != 3){
		printf("Usage: read <filename>\n");
		}
		else{read_file(filename);}
	}
	else if(strcmp(cmd, "delete")==0){
		if(num != 3){
		printf("Usage: delete <filename>\n");
		}
		else{delete_file(filename);}
	}
	else if(strcmp(cmd, "list")==0){
		list_files();
	}
	else{
	 	printf("Invalid command.\n");
	}
}


int main(int argc, char*argv[])
{
	if(argc <=1){
		printf("USAGE: ./fat <COOMAND> [ARGS]...\n");
		exit(1);
	}
	load_file_system(); //load
	execute_cmd(argv[1], argv[2], argv[3], argc);
	save_file_system(); //save
	exit(0);
}














































