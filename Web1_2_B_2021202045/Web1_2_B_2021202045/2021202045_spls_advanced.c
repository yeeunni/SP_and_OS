////////////////////////////////////////////////////////////////////////////////
// File Name  :2021202045_spls_advanced.c                                     //
// Date       :2023/04/05                                                     //  
// Os : Ubuntu 16.04 LTS 64bits                                               // 
// Author : Kim Ye EUn                                                        // 
// Student ID : 2021202045                                                    //
// ----------------------------                                               //
// Title : System Programming Assignment #1-2                                 //
// Description : a program to create a command called                         //  
// 'spls_advanced' that performs the same function as the optioned 'ls'       //
// command in a Linux system.(optioned = 'a,l,al')                            //
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <grp.h>
#include <time.h>
#include<fcntl.h>
#include <pwd.h>

void sort_hidden(int dir_num, char*arr[],int max);
void option_l(char *arr,char*path,int check,char*prev_dir);
int total_blockSize(char *arr,char*path,int total,int check,char*prev_dir);

int main(int argc, char **argv){       //get parameter from terminal
	DIR *dirp = NULL;                  //get directory's pointer
	struct dirent *dir;                //use struct dirent
	int dir_num=0; // number of file or direcotry
	int max =0;    // max size of name of file or directory
    int aflag=0;   //if option -a is on
    int lflag=0;   //if option -l is on
    int c;            //get getopt() is ok
    char pathname[1024]; //get pathname
    int file_num=0;     //number of file in parameter
    int directory_num =0; //number of directory in parameter
    int file_max=0;  //parameter of file's max size
    int dir_max=0; //parameter of directory's max size
    int check=0;  //difference of option a and l
    while((c = getopt(argc, argv,"al"))!=-1){   //get option and turn on each flag
        switch(c){
            case 'a':  
                aflag =1;
                break;
            case 'l':
                lflag =1;
                break;
        }
    } 
   
	if(argc == 1){   //no parameter
            dirp = opendir("."); //automatically search currnet directory
           
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
            
            rewinddir(dirp);   //for reusing dirp
            int dir_num1=0;    //for iterator
            while((dir=readdir(dirp)) != NULL){
                strcpy(arr[dir_num1], dir->d_name); //put dir->d_name in 2D array
                dir_num1++;  //dir_num1 increases
            }
            dir_num1 -=1;

            sort_hidden(dir_num,arr,max);  //sorting name

            for(int i=0; i<dir_num; i++){
             if(arr[i][0]=='.')continue;  //not print file name starting '.'
             printf("%s\n", arr[i]);      //print sorting file
            }

            for(int k=0; k< dir_num; k++){   //deallocate arr
                 free(arr[k]);
            }
	        free(arr);
            closedir(dirp);   //closedir dirp
            return 0;
	}
    else if(argc == 2 && aflag ==1 &&lflag==0){ //if only -a option and no parameter(file or directory)
        
           dirp = opendir("."); //automatically search currnet directory
           
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
            
            rewinddir(dirp);   //for reusing dirp
            int dir_num1=0;    //for iterator
            while((dir=readdir(dirp)) != NULL){
                strcpy(arr[dir_num1], dir->d_name); //put dir->d_name in 2D array
                dir_num1++;  //dir_num1 increases
            }
            dir_num1 -=1;

            sort_hidden(dir_num,arr,max);

            for(int i=0; i<dir_num; i++){
             printf("%s\n", arr[i]);      //print sorting file
            }

            for(int k=0; k< dir_num; k++){   //deallocate arr
                 free(arr[k]);
            }
	        free(arr);
            closedir(dirp);   //closedir dirp
        return 0;
    }
    else if(argc ==2 && lflag == 1){ //if only -l or -al option and no parameter(file or directory)
            int total =0;       // variable of counting block
            dir_num=0;         
             int local_check = 1;  //initialize
            if(aflag==0){
                local_check = 1;   //option is -l
            }
            else if(aflag ==1){    //option is -al
                local_check =0;
            }
            dirp = opendir("."); //automatically search currnet directory
            getcwd(pathname,1024);  //sotre current working directory in pathname
            printf("Directory path: %s\n",pathname); 
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
            
            rewinddir(dirp);   //for reusing dirp
            int dir_num1=0;    //for iterator
            while((dir=readdir(dirp)) != NULL){
                strcpy(arr[dir_num1], dir->d_name); //put dir->d_name in 2D array
                dir_num1++;  //dir_num1 increases
            }
            dir_num1 -=1;
            total =0;
            sort_hidden(dir_num,arr,max); //sorting file include hidden file
            for(int j=0; j<dir_num; j++){
                total = total_blockSize(arr[j],pathname,total,local_check,pathname);  //save total blocksize
           }
           printf("total : %d\n",total/2);
            for(int i=0; i<dir_num; i++){
                option_l(arr[i],pathname,local_check,pathname); //call option_l function 
            }

            for(int k=0; k< dir_num; k++){   //deallocate arr
                 free(arr[k]);
            }
	        free(arr);
            closedir(dirp);   //closedir dirp
        return 0;
    }
	else{                                               //option and parameter both exit
        for(int it=optind;it<argc;it++){    
	        if((dirp=opendir(argv[it])) == NULL){   //if argv[it] is no directory(don't exit or not directory) 
               
                 if(argv[it][0] =='-'){
                   continue;
                }
                file_num++;    
                if(strlen(argv[it])>file_max){       
                file_max = strlen(argv[it]);   //modify max of file using strlen
                }
	     }
        else{
            directory_num++;
            if(strlen(argv[it])>dir_max){       
			    dir_max = strlen(argv[it]);   //modify max of directory using strlen
		    }
            closedir(dirp);
        }
     }
	}
    
    char **arr_file;                       //array of 2D
	arr_file = malloc(sizeof(char*)*file_num); //col is file_num
	for(int j=0; j<file_num; j++){                 
		arr_file[j] = malloc(sizeof(char)*file_max);   //row is file_max
	}
	char **arr_dir;                       //array of 2D
	arr_dir = malloc(sizeof(char*)*directory_num); //col is directory_num
	for(int j=0; j<directory_num; j++){                 
		arr_dir[j] = malloc(sizeof(char)*dir_max);   //row is dir_max
	}
    int it_dir =0;
    int it_file =0;
  for(int it=optind;it<argc;it++){    
	   if((dirp=opendir(argv[it])) == NULL){   //if argv[it] is no directory(don't exit or not directory) 
          
            strcpy(arr_file[it_file], argv[it]); //store file parameter in arr_file
            it_file++;
	    }
        else{
            strcpy(arr_dir[it_dir], argv[it]); //store directory parameter in arr_dir
            it_dir++;
            closedir(dirp);
        }
    }

    sort_hidden(file_num,arr_file,file_max);  //sorting file include hidden 
    sort_hidden(directory_num,arr_dir,dir_max); //soritng directory include hidden

    if(aflag == 1 && lflag ==0){  //if option -a
        for(int i=0; i<file_num; i++){
		 if(access(arr_file[i],F_OK)<0){   //check if file is exit
            printf("cannot access \'%s\' : No such directory\n", arr_file[i]);
            continue;
        }
		printf("%s\n", arr_file[i]);      //print sorted file
	    }
        
        for(int i=0;i<directory_num;i++){  
            if(access(arr_dir[i],F_OK)<0){  //check if directory is exit
           printf("cannot access \'%s\' : No such directory\n", arr_dir[i]);
            continue;
            }
            dir_num=0; //initialize dir_num
            printf("%s:\n",arr_dir[i]);
            dirp = opendir(arr_dir[i]); //open directory
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
            
            rewinddir(dirp);   //for reusing dirp
            int dir_num1=0;    //for iterator
            while((dir=readdir(dirp)) != NULL){
                strcpy(arr[dir_num1], dir->d_name); //put dir->d_name in 2D array
                dir_num1++;  //dir_num1 increases
            }
            dir_num1 -=1;

            sort_hidden(dir_num,arr,max);

            for(int ip=0; ip<dir_num; ip++){
             printf("%s\n", arr[ip]);      //print sorting file
            }

            for(int k=0; k< dir_num; k++){   //deallocate arr
                 free(arr[k]);
            }
	        free(arr);
            closedir(dirp);   //closedir dirp
        }
    }
    else if(aflag == 1 && lflag ==1){ //option is -al
        struct stat buf;     //for counting total block size
        for(int i=0; i<file_num; i++){
        getcwd(pathname,1024); //get current working directory in pathname
        if(access(arr_file[i],F_OK)<0){
             printf("cannot access \'%s\' : No such directory\n", arr_file[i]);
            continue;
        }
        printf("Directory path: %s\n",pathname);
		option_l(arr_file[i],pathname,0,pathname); //call option_l function
	    }
                
        for(int i=0;i<directory_num;i++){
            
           if(access(arr_dir[i],F_OK)<0){    //check if directory exit
            printf("cannot access \'%s\' : No such directory\n", arr_dir[i]);
            continue;
            }
            char* p_path = (char*)malloc(1024);   //sotre absolute path
            char* path = (char*)malloc(1024);
            char prev_dir[1024]; //sotre prev_dir
            strcpy(p_path, "/");          //make absolute path
            strcat(p_path, arr_dir[i]);
            getcwd(path, 1024);
            strcat(path, p_path);
            getcwd(prev_dir,1024);
            if((arr_dir[i][0])== '/'){
               strcpy(path,arr_dir[i]);
            }
            printf("Directory path: %s\n",path);
            dir_num =0; //initialize
            int total =0; //initialzie
            dirp = opendir(arr_dir[i]); //open directory
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
            
            rewinddir(dirp);   //for reusing dirp
            int dir_num1=0;    //for iterator
            while((dir=readdir(dirp)) != NULL){
                strcpy(arr[dir_num1], dir->d_name); //put dir->d_name in 2D array
                dir_num1++;  //dir_num1 increases
            }

            sort_hidden(dir_num,arr,max); //sorting file include hidden file
            for(int j=0; j<dir_num; j++){
                total = total_blockSize(arr[j],path,total,0,prev_dir);  //update total calling total_blockSize function
           }
           printf("total : %d\n",total/2);
            for(int j=0; j<dir_num; j++){
                option_l(arr[j],path,0,prev_dir); //call option_l function 
            }

            for(int k=0; k< dir_num; k++){   //deallocate arr
                 free(arr[k]);
            }
	        free(arr);
            closedir(dirp);   //closedir dirp
        }
    }
    else if(aflag == 0 && lflag ==1){

        int total =0;
        
        for(int i=0; i<file_num; i++){
        getcwd(pathname,1024);
        if(access(arr_file[i],F_OK)<0){ //check existing file
            printf("cannot access \'%s\' : No such directory\n", arr_file[i]);
            continue;
        }
        printf("Directory path: %s\n",pathname);
		option_l(arr_file[i],pathname,1,pathname);
	    }
                
        for(int i=0;i<directory_num;i++){
       
            if(access(arr_dir[i],F_OK)<0){
            printf("cannot access \'%s\' : No such directory\n", arr_dir[i]); //check existing directory
            continue;
            }
            char* p_path = (char*)malloc(1024); //make absolute path
            char* path = (char*)malloc(1024);
            char prev_dir[1024]; //store prev_dir
            strcpy(p_path, "/");
            strcat(p_path, arr_dir[i]);
            getcwd(path, 1024);
            strcat(path, p_path);
            getcwd(prev_dir,1024);
            if((arr_dir[i][0])== '/'){
               strcpy(path,arr_dir[i]);
            }
            printf("Directory path: %s\n",path);
            dirp = opendir(arr_dir[i]);
            dir_num =0;
            total =0;
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
            
            rewinddir(dirp);   //for reusing dirp
            int dir_num1=0;    //for iterator
            while((dir=readdir(dirp)) != NULL){
                strcpy(arr[dir_num1], dir->d_name); //put dir->d_name in 2D array
                dir_num1++;  //dir_num1 increases
            }

            sort_hidden(dir_num,arr,max); //sorting file or folder include hidden

            for(int j=0; j<dir_num; j++){
                total = total_blockSize(arr[j],path,total,1,prev_dir); //update total
           }
            printf("total : %d\n",total/2);
            for(int j=0; j<dir_num; j++){
                option_l(arr[j],path,1,prev_dir); //call option_l function
            }

            for(int k=0; k< dir_num; k++){   //deallocate arr
                 free(arr[k]);
            }
	        free(arr);
            closedir(dirp);   //closedir dirp
        }
    }
    else{
        for(int i=0; i<file_num; i++){
        
		if(arr_file[i][0]=='.')continue;  //not print file name starting '.'
        if(access(arr_file[i],F_OK)<0){
            printf("cannot access \'%s\' : No such directory\n", arr_file[i]);
            continue;
            }
        printf("%s\n", arr_file[i]);      //print sorting file
	    }

        for(int i=0;i<directory_num;i++){
            if(access(arr_dir[i],F_OK)<0){
            printf("cannot access \'%s\' : No such directory\n", arr_dir[i]);
            continue;
            }
            printf("%s:\n",arr_dir[i]);
            dirp = opendir(arr_dir[i]);
            dir_num =0;
            int total =0;
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
            
            rewinddir(dirp);   //for reusing dirp
            int dir_num1=0;    //for iterator
            while((dir=readdir(dirp)) != NULL){
                strcpy(arr[dir_num1], dir->d_name); //put dir->d_name in 2D array
                dir_num1++;  //dir_num1 increases
            }
            dir_num1 -=1;

            sort_hidden(dir_num,arr,max);
           
            for(int ip=0; ip<dir_num; ip++){
             if(arr[ip][0]=='.')continue;  //not print file name starting '.'
             printf("%s\n", arr[ip]);      //print sorting file
            }

            for(int k=0; k< dir_num; k++){   //deallocate arr
                 free(arr[k]);
            }
	        free(arr);
            closedir(dirp);   //closedir dirp
        }
    }

	for(int k=0; k< file_num; k++){   //deallocate arr_file
     free(arr_file[k]);
	}
	free(arr_file);

    for(int k=0; k< directory_num; k++){   //deallocate arr_dir
     free(arr_dir[k]);
	}
	free(arr_dir);

	return 0;
} 

//////////////////////////////////////////////////////////////////
//sort_hidden                                                   //
//=====================================================         //
//Input: int dir_num -> directory numvber for iterator          //
//       char* arr[]-> want to sort char*arr[]                  //
//       int max -> to allocate appropriate tmp for bubble sort //
//Output:   sorted file or directory                            //
//Purpose: sort file or directory using bubble sorting include  //        
//         hidden file(start with '.')                          //
//////////////////////////////////////////////////////////////////
void sort_hidden(int dir_num, char*arr[],int max){

    char * tmp = malloc(sizeof(char)*max); //for using to change order of name
    for (int i =0; i<dir_num-1; i++){        //like bubble-sorting
		for(int j= i+1; j<dir_num; j++){
            if(arr[i][0]=='.' && arr[j][0] =='.'){
			if(strcasecmp(arr[i]+1,arr[j]+1)>0){   //strcasecmp(a+1,b+1): if result >0 -> b+1 is before a+1 in dicitionary
				strcpy(tmp, arr[i]);           //change order of arr[i]and arr[j] using tmp
				strcpy(arr[i],arr[j]);
				strcpy(arr[j],tmp);
			}
            }
            else  if(arr[i][0]=='.' && arr[j][0] !='.'){
			if(strcasecmp(arr[i]+1,arr[j])>0){   //strcasecmp(a+1,b): if result >0 -> b is before a+1 in dicitionary
				strcpy(tmp, arr[i]);           //change order of arr[i]and arr[j] using tmp
				strcpy(arr[i],arr[j]);
				strcpy(arr[j],tmp);
			}
            }
            else  if(arr[i][0]!='.' && arr[j][0] =='.'){
			if(strcasecmp(arr[i],arr[j]+1)>0){   //strcasecmp(a,b+1): if result >0 -> b+1 is before a in dicitionary
				strcpy(tmp, arr[i]);           //change order of arr[i]and arr[j] using tmp
				strcpy(arr[i],arr[j]);
				strcpy(arr[j],tmp);
			}
            }
            else{
			if(strcasecmp(arr[i],arr[j])>0){    //strcasecmp(a,b): if result >0 -> b is before a in dicitionary
				strcpy(tmp, arr[i]);           //change order of arr[i]and arr[j] using tmp
				strcpy(arr[i],arr[j]);
				strcpy(arr[j],tmp);
			}
            }
            }
		}
        free(tmp); //deallocate tmp
}
//////////////////////////////////////////////////////////////////
//option_l                                                      //
//=====================================================         //
//Input: char* arr -> file or direcotry to find information     //
//       char* path -> absolute path                            //
//       int check -> whether hidden file care or don't care    //
//Output: none-> error                                          //
//        print information of arr(file or directory)           //
//Purpose: search file(or directory)'s information using stat   //
//////////////////////////////////////////////////////////////////
void option_l(char *arr,char*path,int check,char*prev_dir){
    char perm[11]= "----------\0"; //initialize permission array
    struct stat buf;    //declare struct stat
    struct passwd *pwd; //for getuid  
    struct group *grp; //for getgid
    char t_buf[80]; // buffer of strftime()

    chdir(path); //change directory to path
    if(lstat(arr, &buf) <0) //perror message
    { 	
        perror("stat");
        printf("%s: \n", arr);
        printf("errorofoption: %s\n", strerror(errno)); 
    	return;
    }
    
    if(S_ISDIR(buf.st_mode)){perm[0] = 'd';}         // directory
    else if(S_ISLNK(buf.st_mode)){perm[0] = 'l';}         // symbolic link
   
    //if file(or folder) has this permission -> update char perm[11]
    if(S_IRUSR & buf.st_mode) //if user can read
    {
        perm[1] ='r';
    }
    if(S_IWUSR & buf.st_mode) //if user can write
    {
        perm[2] = 'w';
    }
    if(S_IXUSR & buf.st_mode)  //if user can exe
    {
        perm[3] = 'x';
    }
    if(S_IRGRP & buf.st_mode) //if group can read
    {
        perm[4] = 'r';
    }
    if(S_IWGRP & buf.st_mode) //if group can write
    {
        perm[5] = 'w';
    }
    if(S_IXGRP & buf.st_mode)  //if group can exe
    {
       perm[6] = 'x';
    }
    if(S_IROTH & buf.st_mode)  //if other can read
    {
        perm[7] = 'r';
    }
    if(S_IWOTH & buf.st_mode)  //if other can write
    {
        perm[8] = 'w';
    }
    if(S_IXOTH & buf.st_mode)  //if other can exe
    {
        perm[9] = 'x';
    }
    if(check ==1){  //check == 1 -> no -a option so don't care hidden
        if(arr[0]=='.')return;
    }
    pwd = getpwuid(buf.st_uid); //get st_uid to char
    if(pwd == NULL){ //error message
        perror("getpwuid");
        printf("Error: %s\n", strerror(errno));
        return;
    }
    grp = getgrgid(buf.st_gid); // get group information using GID
    if(grp == NULL){ //error message
        perror("getgrgid");
        printf("Error: %s\n", strerror(errno));
        return;
    }
    strftime(t_buf, sizeof(t_buf), "%b %d %H:%M", localtime(&buf.st_mtime)); // format time using t_buf
    printf("%s\t%u\t%s\t%s\t%ld\t",perm,buf.st_nlink, pwd->pw_name,grp->gr_name,buf.st_size); //print permisiion, number of link, username, groupname, size using '\t'
    printf("%s\t%s\n", t_buf, arr); // print directory or file name
    chdir(prev_dir); //back to prev directory

}
//////////////////////////////////////////////////////////////////
//total_blockSize                                               //
//=====================================================         //
//Input: char* arr -> file or direcotry to find block size      //
//       char* path -> absolute path                            //
//       int check -> whether hidden file care or don't care    //
//Output: 0 -> error                                            //
//        total -> update or not changed total variable         //
//Purpose: counting block size                                  //
//////////////////////////////////////////////////////////////////
int total_blockSize(char *arr,char*path,int total,int check,char*prev_dir){

    chdir(path); //change directory
    struct stat buf;
   
    if(lstat(arr, &buf) <0 )
    { 	//perror message
        perror("stat");
        printf("%s: \n", arr);
        printf("error_total: %s\n", strerror(errno)); 
    	return 0;
    }
    if(check == 1){ //check ==1 -> no -a option
        if(arr[0]=='.')
            return total;  //not add of hidden file's block size
    }
    total += buf.st_blocks; //update total
    chdir(prev_dir); //back to prev directory
    return total; //return updated total
    
}