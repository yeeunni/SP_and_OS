///////////////////////////////////////////////////////////////
// File Name  :2021202045_simple_ls.c                        //
// Date       :2023/03/26                                    //  
// Os : Ubuntu 16.04 LTS 64bits                              // 
// Author : Kim Ye EUn                                       // 
//   Student ID : 2021202045                                 //
// ----------------------------                              //
// Title : System Programming Assignment #1-1 (proxy server) //
// Description : a program to create a command called        //  
// 'simpe_ls' that performs the same function as the 'ls'    //
// command in a Linux system.                                //
///////////////////////////////////////////////////////////////

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
int main(int argc, char **argv){       //get parameter from terminal
	DIR *dirp = NULL;                  //get directory's pointer
	struct dirent *dir;                //use struct dirent
	int dir_num=0; // number of file or direcotry
	int max =0;    // max size of name of file or directory

	if(argc>2){    //parameter's number is over 1
		 
		printf("simple_ls: only one directory path can be processed\n"); //print error message 
		return 0; //exit program
	}
	if(argc==1){   //no parameter
		
		dirp = opendir("."); //automatically search currnet directory
	}
	else{          //if argc==2
	if((dirp=opendir(argv[1])) == NULL){   //if argv[1] is no directory(don't exit or not directory) 
		printf("simple_ls: cannot access \'%s\' : No such directory\n", argv[1]); //print error message
		return 0; //eixt program
	}
	}
	
	while((dir=readdir(dirp)) != NULL){  //read dirp's directory until the end's file(or directory)
		dir_num += 1;                   //increase numbers of dir_num    
		if(strlen(dir->d_name)>max){       
			max = strlen(dir->d_name);   //modify max using strlen
		}
	}
	max = max+1;                      //make free space
	char **arr;                       //array of 2D
	arr = malloc(sizeof(char*)*dir_num); //col is dir_num
	for(int j=0; j<dir_num; j++){                 
		arr[j] = malloc(sizeof(char)*max);   //row is max
	}
	char * tmp = malloc(sizeof(char)*max); //for using to change order of name
	rewinddir(dirp);   //for reusing dirp
	int dir_num1=0;    //for iterator
	while((dir=readdir(dirp)) != NULL){
		strcpy(arr[dir_num1], dir->d_name); //put dir->d_name in 2D array
		dir_num1++;  //dir_num1 increases
	}
	dir_num1 -=1;
	for (int i =0; i<dir_num-1; i++){        //like bubble-sorting
		for(int j= i+1; j<dir_num; j++){
			if(strcasecmp(arr[i],arr[j])>0){   //strcasecmp(a,b): if result >0 -> b is before a in dicitionary
				strcpy(tmp, arr[i]);           //change order of arr[i]and arr[j] using tmp
				strcpy(arr[i],arr[j]);
				strcpy(arr[j],tmp);
			}
		}
	}
	for(int i=0; i<dir_num; i++){
		if(arr[i][0]=='.')continue;  //not print file name starting '.'
		printf("%s\n", arr[i]);      //print sorting file
	}

	for(int k=0; k< dir_num; k++){   //deallocate
     free(arr[k]);
	}
	free(arr);
	free(tmp);
	
	closedir(dirp);   //closedir dirp
	return 0;
} 
