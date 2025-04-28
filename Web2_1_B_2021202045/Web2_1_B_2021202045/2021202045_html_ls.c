////////////////////////////////////////////////////////////////////////////////
// File Name  :2021202045_html_ls.c                                           //
// Date       :2023/04/19                                                     //  
// Os : Ubuntu 16.04 LTS 64bits                                               // 
// Author : Kim Ye Eun                                                        // 
// Student ID : 2021202045                                                    //
// ----------------------------                                               //
// Title : System Programming Assignment #2-1                                 //
// Description : a program to create a command called                         //  
// 'html_ls' that performs the same function as the optioned 'ls'             //
// command in a Linux system. Also, add wild card code using fnmatch function //
// Furthermore, display the result on html page named 'html_ls.html'          //
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
#include <math.h>
#include <pwd.h>
#include <fnmatch.h>
//function declaration
void sort_hidden(int dir_num, char*arr[],int max);   
void option_l(char *arr,char*path,int check,char*prev_dir,int hflag,FILE *file);
int total_blockSize(char *arr,char*path,int total,int check,char*prev_dir);
char** option_s(int dir_num, char*arr[],int max,char *path, char*prev_dir);
void wild(char*prev_dir, char*path,char*pattern,int check,FILE *file);
int ls_final(int argc, char **argv, FILE* file);
char* what_color(char*path, char*prev, int aflag, char*arr,char* color);

int main(int argc, char** argv)
{
    FILE* file = fopen("html_ls.html", "w"); //write to "html_ls.html" file -> if no file, make


    if(file == 0)
    {
        printf("error\n");
        return -1;
    }

    char current_path[1024]; //current path store using getcwd()
	getcwd(current_path, 1024);  
    fprintf(file, "<html>\n"); //open html
    fprintf(file,"<head>");
    fprintf(file, "<title>%s</title>\n", current_path); //title is current_path
    fprintf(file,"</head>");
    fprintf(file, "<body>\n"); //open body
    fprintf(file,"<h1>");
    for (int i = 0; i < argc; i++) {
     fprintf(file, " %s", argv[i]); //print instruction of shell to html
    }
    fprintf(file,"</h1>");
    fprintf(file, "<br>"); //'\n'
    ls_final(argc, argv, file); //call ls_final
    fprintf(file, "</body>\n"); //close body
    fprintf(file,"</html>"); //close html
    fclose(file);    //close file stream
    return 0; 
    
}

int ls_final(int argc, char **argv,FILE *file){       //get parameter from terminal
   DIR *dirp = NULL;                  //get directory's pointer
   struct dirent *dir;                //use struct dirent
   int dir_num=0; // number of file or direcotry
   int max =0;    // max size of name of file or directory
    int aflag=0;   //if option -a is on
    int lflag=0;   //if option -l is on
    int rflag=0;   //if option -r is on
    int sflag=0;    //if option -S is on
    int hflag=0;     //if option -h is on
    int c;            //get getopt() is ok
    char pathname[1024]; //get pathname
    int file_num=0;     //number of file in parameter
    int directory_num =0; //number of directory in parameter
    int file_max=0;  //parameter of file's max size
    int dir_max=0; //parameter of directory's max size
    int pat_max=0;    //number of pattern in parameter
    int pat_num=0;   //parameter of pattern's max size
    int check=0;  //difference of option a and l
    
    while((c = getopt(argc, argv,"alrSh"))!=-1){   //get option and turn on each flag
        switch(c){      //turn on the flag using 'c'
            case 'a':  
                aflag =1;  //turn on aflag
                break;
            case 'l':
                lflag =1;  //turn on lflag
                break;
            case 'r':
                rflag =1; //turn on rflag
                break;
            case 'S':
                sflag =1; //turn on s flag
                break;
            case 'h':
                hflag =1;  //turn on hflag
                break;
        }
    } 
 
   if(argc == 1 || (argc==2 && lflag==0 && aflag==0)){   //no parameter or argc==2 but no lflag and aflag
            DIR* dirp2 = NULL;         //declare dirp2 -> to open each entry
            char path[1024];               //store path
            char prev_dir[1024]; //sotre prev_dir
            int exist=0;    //if pattern(wild card) is exist
            int patt_start=0;  //index of starting wild card part
            int it=0;  //iteratior declare 
            char pattern[1024];  //store pattern
            if(argc != 1){   //if not no parameter
                int num=0;    //to know end of array
                for(int p=0; p<strlen(argv[1]);p++){  
                    if(argv[1][p] == '*' ||argv[1][p] == '[' ||argv[1][p] == '?' ){  //if wild card is exist
                        exist =1;      //turn on exist 1
                        patt_start = p;  //store index of starting wild card
                        if(argv[1][0] != '/'){   //wild card doesn't start with '/'
                            dirp = opendir(".");  //opendir of current directory
                            getcwd(path,1024);   //store path
                            strcpy(prev_dir,path);  //sotre prev_dir path
                            strcpy(pattern,argv[1]);  //sotre pattern
                            exist=2;    //exist is 2 -> diffrent from '/' wild card
                            break;
                        }
                        else{    //if wild card starts with '/'
                            for(int pi=0; pi<patt_start-1;pi++){   
                                path[pi] = argv[1][pi];    //sotre path until pattern appears
                                num++;  //increase num
                            }
                            for(int ip=patt_start; ip<strlen(argv[1]);ip++){
                                
                                pattern[it]=argv[1][ip];  //store pattern
                                it++;
                            }

                            pattern[it]='\0';    // no trash value
                            
                            path[num] = '\0';  //no trash value
                            strcpy(prev_dir,path); //store prev_dir
                            
                            dirp = opendir(path);  //open directory of path
                            break;
                        }
                    }
                   
                }
                if(exist == 0){  //if no wild card
                    if((dirp=opendir(argv[1]))==NULL){
                        dirp = opendir(".");  //open current directory
                    }
                    getcwd(path,1024);
                    if(argv[1][0]!='/'){ //if parameter is not absolute path
                    strcat(path,"/"); 
                    strcat(path,argv[1]); //make absolute path
                    fprintf(file,"<h3>Directory path: %s\n</h3>",path);
                    }
                    else{ //if parameter is absolute path
                    dirp = opendir(argv[1]);  //open parameter
                    strcpy(path,argv[1]);
                    fprintf(file,"<h3>Directory path: %s\n</h3>",argv[1]); 
                    }
                    getcwd(prev_dir,1024); //prev_dir is current working dir
                }
            }
            else{  //if no parameter
            dirp = opendir("."); //automatically search currnet directory
            getcwd(path,1024);
            getcwd(prev_dir,1024);
            printf("Directory path: %s\n",path);
            fprintf(file,"<h3>Directory path: %s\n</h3>",path);
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
            
            rewinddir(dirp);   //for reusing dirp
            int dir_num1=0;    //for iterator
            while((dir=readdir(dirp)) != NULL){
                strcpy(arr[dir_num1], dir->d_name); //put dir->d_name in 2D array
                dir_num1++;  //dir_num1 increases
            }
            dir_num1 -=1;

            sort_hidden(dir_num,arr,max);  //sorting name
            if(sflag == 1 && exist ==0){
                option_s(dir_num,arr,max,path,prev_dir);
            }
            if(rflag == 1 && exist==0){
             fprintf(file, "<table border=\"1\">\n");
	         fprintf(file, "<tr><th>Name</th></tr>\n");
             for(int i=dir_num-1; i>=0; i--){
                char color_p[1024];
                 if(arr[i][0]=='.')continue;
                 if(strcmp(arr[i], "html_ls.html") == 0)continue;
                strcpy(color_p,what_color(path,prev_dir,aflag,arr[i],color_p));  
                printf("%s\n", arr[i]);      //print sorting file
                fprintf(file,"<tr><td style =\"color:%s\"><a href =\"%s\">%s</a></td></tr>", color_p,path,arr[i]);      //print sorting file
             }
            }
            else if(rflag==0 && exist==0){
             fprintf(file, "<table border=\"1\">\n");
	         fprintf(file, "<tr><th>Name</th></tr>\n");   
            for(int i=0; i<dir_num; i++){
             char color_p[1024];
             if(arr[i][0]=='.')continue;  //not print file name starting '.'
             if(strcmp(arr[i], "html_ls.html") == 0)continue; 
             strcpy(color_p,what_color(path,prev_dir,aflag,arr[i],color_p));  
             printf("%s\n", arr[i]);      //print sorting file
             char hyper[1024]; // make path of hyper link to file
             strcpy(hyper,path);  //copy path
             strcat(hyper,"/");   //add '/'
             strcat(hyper,arr[i]);  //add file name
             printf("hyper: %s\n",hyper);
             fprintf(file,"<tr><td style =\"color:%s\"><a href =\"%s\">%s</a></td></tr>", color_p,hyper,arr[i]); 
            }
            }
            else if(exist==1){  // if exist == 1
                for(int i=0; i<dir_num; i++){
                if(!fnmatch(pattern, arr[i],0)){      //if mathcing with pattern and entry 
                strcpy(path,prev_dir);    //strcpy path
                if(arr[i][0]=='.')continue;  //not print file name starting '.'
                strcat(path,"/");   //make path of directory
                strcat(path, arr[i]);
                if((dirp2=opendir(path))==NULL){  //if no directory
                printf("Directory path: %s\n",path);    //if not dir -> print
                fprintf(file,"<h3>Directory path: %s\n</h3>",path);
                wild(prev_dir,path,pattern,1,file);  // wild function
                }
                }
            }
                for(int i=0; i<dir_num; i++){
                if(!fnmatch(pattern, arr[i],0)){    //if matching with pattern and entry
                strcpy(path,prev_dir);
                if(arr[i][0]=='.')continue;  //not print file name starting '.'
                if(strcmp(arr[i], "html_ls.html") == 0)continue; 
                strcat(path,"/");
                strcat(path, arr[i]);
                if((dirp2=opendir(path))!=NULL){ //if dir -> print
                    printf("Directory path: %s\n",path);
                    fprintf(file,"<h3>Directory path: %s\n</h3>",path);
                    wild(prev_dir,path,pattern,1,file);  //wild function
                    closedir(dirp2); //close directory
                    }
                }
            }
            }
            else if(exist==2){ //if exist == 2
                for(int i=0; i<dir_num; i++){
                if(!fnmatch(pattern, arr[i],0)){    //if matching with pattern and entry
                strcpy(path,prev_dir);
                if(arr[i][0]=='.')continue;  //not print file name starting '.'
                if(strcmp(arr[i], "html_ls.html") == 0)continue; //no print html_ls.html file
                strcat(path,"/");
                strcat(path, arr[i]);
                if((dirp2=opendir(path))==NULL){ //if not dir -> print
                printf("%s\n",arr[i]);    //if not dir -> print
                fprintf(file,"<h3>Directory path: %s\n</h3>",path);
                wild(prev_dir,path,pattern,2,file);
                }
                }
            }
                for(int i=0; i<dir_num; i++){
                if(!fnmatch(pattern, arr[i],0)){    //if matching with pattern and entry
                strcpy(path,prev_dir);
                if(arr[i][0]=='.')continue;  //not print file name starting '.'
                if(strcmp(arr[i], "html_ls.html") == 0)continue;  //no print html_ls.html file
                strcat(path,"/");                //make new path
                strcat(path, arr[i]);
                if((dirp2=opendir(path))!=NULL){ //if dir -> print
                    printf("Directory path: %s\n",path);
                    fprintf(file,"<h3>Directory path: %s\n</h3>",path);
                    wild(prev_dir,path,pattern,2,file);
                    closedir(dirp2);
                    }
                }
            }
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
           char path[1024];
           char prev_dir[1024];
           getcwd(path,1024);
           getcwd(prev_dir,1024);
           printf("Directory path: %s\n",path);
           fprintf(file,"<h3>Directory path: %s\n</h3>",path);
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

            sort_hidden(dir_num,arr,max);  //sorting arr
            fprintf(file, "<table border=\"1\">\n"); //setting table border
	        fprintf(file, "<tr><th>Name</th></tr>\n");  //set table head name
            if(rflag == 1){  
             for(int i=dir_num-1; i>=0; i--){
             char color_p[1024]; // store color information
             if(strcmp(arr[i], "html_ls.html") == 0)continue; //if html file -> not print
             strcpy(color_p,what_color(path,prev_dir,aflag,arr[i],color_p));  //strcpy color
             printf("%s\n", arr[i]);      //print sorting file
             if(strcmp(arr[i],"..")==0){
             char pre_path[1024];
             strcpy(pre_path,path);
             strcat(pre_path,"/..");   
             fprintf(file,"<tr><td style =\"color:%s\"><a href =\"%s\">%s</a></td></tr>", color_p,pre_path,arr[i]);  //print with color and hyperlink
             }
             else{
             char hyper[1024]; // make path of hyper link to file
             strcpy(hyper,path);  //copy path
             strcat(hyper,"/");   //add '/'
             strcat(hyper,arr[i]);  //add file name
             //printf("hyper: %s\n",hyper);
             fprintf(file,"<tr><td style =\"color:%s\"><a href =\"%s\">%s</a></td></tr>", color_p,hyper,arr[i]);  //print with color and hyperlink
             }
             }
            }
            else{
            for(int i=0; i<dir_num; i++){
             char color_p[1024]; // store color information
             if(strcmp(arr[i], "html_ls.html") == 0)continue; //if html file -> not print
             strcpy(color_p,what_color(path,prev_dir,aflag,arr[i],color_p));  //strcpy color
             printf("%s\n", arr[i]);      //print sorting file
             if(strcmp(arr[i],"..")==0){
             char pre_path[1024];
             strcpy(pre_path,path);
             strcat(pre_path,"/..");   
             fprintf(file,"<tr><td style =\"color:%s\"><a href =\"%s\">%s</a></td></tr>", color_p,pre_path,arr[i]);  //print with color and hyperlink
             }
             else{
             char hyper[1024]; // make path of hyper link to file
             strcpy(hyper,path);  //copy path
             strcat(hyper,"/");   //add '/'
             strcat(hyper,arr[i]);  //add file name
             //printf("hyper: %s\n",hyper);
             fprintf(file,"<tr><td style =\"color:%s\"><a href =\"%s\">%s</a></td></tr>", color_p,hyper,arr[i]);  //print with color and hyperlink
             }
             } 
            }
            fprintf(file,"</table>"); //end table
            for(int k=0; k< dir_num; k++){   //deallocate arr
                 free(arr[k]);
            }
           free(arr);
            closedir(dirp);   //closedir dirp
        return 0;
    }
    else if(argc ==2 && lflag == 1){ //if only -l or -al option and no parameter(file or directory)
            int total =0;       // variable of counting block
            char prev_dir[1024];
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
            getcwd(prev_dir,1024);
            printf("Directory path: %s\n",pathname); 
            fprintf(file,"<h3>Directory path: %s\n</h3>",pathname); 
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
           if(hflag ==0){  //if hflag is 0
            printf("total : %d\n",total/2);
            fprintf(file,"<h3>total : %d\n</h3>",total/2);
            } //print total
            
            if(hflag == 1){ //if hflag is 1
            char size_str[20];  //sotre readable size
            total = total/2; //total update
            printf("total :");
            fprintf(file,"<h3>total ");

            if (total < 1024) {  
               sprintf(size_str, "%.0fK\n", ((double)total)); //print with K
            } else if (total < 1024 * 1024) {
               sprintf(size_str, "%.0fM\n", ceil(total / (1024.0))); //print with M
            } else {
                sprintf(size_str, "%.0fG\n", ceil(total / (1024.0 * 1024.0))); //print with G
            }
            printf("%s",size_str); //print size_str
            fprintf(file,"%s</h3>",size_str); //print size_str
        }
           if(sflag ==1){  //if sflag is 1
                arr = option_s(dir_num,arr,max,pathname,pathname); //go to option_s function
           }
           fprintf(file, "<table border=\"1\">\n"); //setting table border
	       fprintf(file, "<tr><th>Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th></tr>\n");  //set table head name
           if(rflag == 1){ //if rflag is 1
             for(int i=dir_num-1; i>=0; i--){
                if(strcmp(arr[i], "html_ls.html") == 0)continue; //no print html_ls.html file
               option_l(arr[i],pathname,local_check,pathname,hflag,file); //go to option_l function
             }
            }
            else{
            for(int i=0; i<dir_num; i++){
                if(strcmp(arr[i], "html_ls.html") == 0)continue; //no print html_ls.html file
                option_l(arr[i],pathname,local_check,pathname,hflag,file); //call option_l function 
             }
            }
            fprintf(file, "</table>"); //end table
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
                
                 if(argv[it][0] =='-'){ //if option -> skip
                   continue;
                }
                if((strstr(argv[it], "*") != NULL) || (strstr(argv[it], "?") != NULL) || (strstr(argv[it], "[") != NULL) || (strstr(argv[it], "]") != NULL) ){ //if pattern (wild card) is in argv[it]
                    pat_num += 1; //update pattern number
                    if(strlen(argv[it])>pat_max){       
                        pat_max = strlen(argv[it]);   //modify max of pattern using strlen
                    }
                    continue;
                }
                file_num++;    //update file number
                if(strlen(argv[it])>file_max){       
                file_max = strlen(argv[it]);   //modify max of file using strlen
                }
           }
            else{
                
                directory_num++;     //update directory number
                if(strlen(argv[it])>dir_max){       
                    dir_max = strlen(argv[it]);   //modify max of directory using strlen
                }
                closedir(dirp);  //close directory
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
    char **arr_pat;                       //array of 2D
   arr_pat = malloc(sizeof(char*)* pat_num); //col is pat_num
   for(int j=0; j<pat_num; j++){                 
      arr_pat[j] = malloc(sizeof(char)*pat_max);   //row is pat_max
   }
    int it_dir =0;                  //iterator declar and initialize
    int it_file =0;
    int it_pat =0;
  for(int it=optind;it<argc;it++){    
      if((dirp=opendir(argv[it])) == NULL){   //if argv[it] is no directory(don't exit or not directory) 
            if((strstr(argv[it], "*") != NULL) || (strstr(argv[it], "?") != NULL) || (strstr(argv[it], "[") != NULL) || (strstr(argv[it], "]") != NULL) ){
                    strcpy(arr_pat[it_pat], argv[it]); //store directory parameter in arr_dir
                    it_pat++;
                    continue;
                }
          
            strcpy(arr_file[it_file], argv[it]); //store file parameter in arr_file
            it_file++;
       }
        else{
            strcpy(arr_dir[it_dir], argv[it]); //store directory parameter in arr_dir
            it_dir++;
            closedir(dirp);
        }
    }

    for(int i = 0; i < directory_num; i++)
    {
        if((strstr(arr_dir[i], "*") != NULL) || (strstr(arr_dir[i], "?") != NULL) || (strstr(arr_dir[i], "[") != NULL) || (strstr(arr_dir[i], "]") != NULL) ){
            strcpy(arr_pat[it_pat], arr_dir[i]); //store pattern parameter in arr_dir
            it_pat++;
        }
    }


    sort_hidden(file_num,arr_file,file_max);  //sorting file include hidden 
    sort_hidden(directory_num,arr_dir,dir_max); //soritng directory include hidden
    sort_hidden(pat_num,arr_pat,pat_max); //soritng pattern include hidden

    
    if(aflag == 1 && lflag ==0){  //if option -a
        
        if(rflag == 1){
            char path[1024];
            char prev_dir[1024];
            getcwd(prev_dir,1024);
            getcwd(path,1024);
            for(int i=file_num-1; i>=0; i--){ 
            if(access(arr_file[i],F_OK)<0){   //check if file is exit
            printf("cannot access \'%s\' : No such directory\n", arr_file[i]);
            continue;
            }
            fprintf(file,"<h3>Directory path: %s\n</h3>",path); 
            fprintf(file, "<table border=\"1\">\n"); //setting table border
	        fprintf(file, "<tr><th>Name</th></tr>\n");  //set table head name
            char color_p[1024];
            if(strcmp(arr_file[i], "html_ls.html") == 0)continue; //no print html_ls.html file
            strcpy(color_p,what_color(path,prev_dir,aflag,arr_file[i],color_p));   //update color
            printf("%s\n", arr_file[i]);      //print sorting file
            char hyper[1024]; // make path of hyper link to file
             strcpy(hyper,path);  //copy path
             strcat(hyper,"/");   //add '/'
             strcat(hyper,arr_file[i]);  //add file name
             //printf("hyper: %s\n",hyper);
            fprintf(file,"<tr><td style =\"color:%s\"><a href =\"%s\">%s</a></td></tr>", color_p,hyper,arr_file[i]);  //print with color and hyperlink
            fprintf(file,"</table>");
            }
        }
        else{
           char path[1024];
            char prev_dir[1024];
            getcwd(prev_dir,1024);
            getcwd(path,1024);
         for(int i=0; i<file_num; i++){
             if(access(arr_file[i],F_OK)<0){   //check if file is exit
            printf("cannot access \'%s\' : No such directory\n", arr_file[i]);
            continue;
            }   
            fprintf(file,"<h3>Directory path: %s\n</h3>",path); 
            fprintf(file, "<table border=\"1\">\n"); //setting table border
	        fprintf(file, "<tr><th>Name</th></tr>\n");  //set table head name
            char color_p[1024];
            if(strcmp(arr_file[i], "html_ls.html") == 0)continue; //no print html_ls.html file
            strcpy(color_p,what_color(path,prev_dir,aflag,arr_file[i],color_p));  //update color
            printf("%s\n", arr_file[i]);      //print sorting file
            char hyper[1024]; // make path of hyper link to file
             strcpy(hyper,path);  //copy path
             strcat(hyper,"/");   //add '/'
             strcat(hyper,arr_file[i]);  //add file name
             //printf("hyper: %s\n",hyper);
            fprintf(file,"<tr><td style =\"color:%s\"><a href =\"%s\">%s</a></td></tr>", color_p,hyper,arr_file[i]);  //print with color and hyperlink
            fprintf(file,"</table>");
        }
     }
        if(rflag==0){
        for(int i=0;i<directory_num;i++){   //print about directory
            if(access(arr_dir[i],F_OK)<0){  //check if directory is exit
           printf("cannot access \'%s\' : No such directory\n", arr_dir[i]);
            continue;
            }
            dir_num=0; //initialize dir_num
            char path[1024];
            char prev_dir[1024];
            getcwd(prev_dir,1024);
            getcwd(path,1024);
            strcat(path,"/"); //make absolute path
            strcat(path,arr_dir[i]);
            printf("%s:\n",arr_dir[i]);
            fprintf(file,"<h3>Directory path: %s\n</h3>",path); 
            fprintf(file, "<table border=\"1\">\n"); //setting table border
	        fprintf(file, "<tr><th>Name</th></tr>\n");  //set table head name
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

            sort_hidden(dir_num,arr,max);  //sorting arr
                
            
            for(int ip=0; ip<dir_num; ip++){
                printf("%s\n", arr[ip]);      //print sorting file
                char color_p[1024];
                if(strcmp(arr[ip], "html_ls.html") == 0)continue; //no print html_ls.html file
                strcpy(color_p,what_color(path,prev_dir,aflag,arr[ip],color_p));
                if(strcmp(arr[ip],"..")==0){ 
                fprintf(file,"<tr><td style =\"color:%s\"><a href =\"%s\">%s</a></td></tr>", color_p,prev_dir,arr[ip]);  //print with color and hyperlink
                }
                else{
                char hyper[1024]; // make path of hyper link to file
                strcpy(hyper,path);  //copy path
                strcat(hyper,"/");   //add '/'
                strcat(hyper,arr[ip]);  //add file name
                fprintf(file,"<tr><td style =\"color:%s\"><a href =\"%s\">%s</a></td></tr>", color_p,hyper,arr[ip]);  //print with color and hyperlink
                }  
                
            }
              fprintf(file,"</table>");
            
            for(int k=0; k< dir_num; k++){   //deallocate arr
                 free(arr[k]);
            }
           free(arr);
            closedir(dirp);   //closedir dirp
        }
        }
        else if(rflag==1){     //if rflag is 1
            for(int i=directory_num-1;i>=0;i--){      //print in converse
            if(access(arr_dir[i],F_OK)<0){  //check if directory is exit
           printf("cannot access \'%s\' : No such directory\n", arr_dir[i]);
            continue;
            }
            dir_num=0; //initialize dir_num
            char path[1024];
            char prev_dir[1024];
            getcwd(prev_dir,1024);
            getcwd(path,1024); //make absolute path
            strcat(path,"/");
            strcat(path,arr_dir[i]);
            printf("%s:\n",arr_dir[i]);
            fprintf(file,"<h3>Directory path: %s\n</h3>",path); 
            fprintf(file, "<table border=\"1\">\n"); //setting table border
	        fprintf(file, "<tr><th>Name</th></tr>\n");  //set table head name
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
                
             for(int ip=dir_num-1; ip>=0; ip--){
                printf("%s\n", arr[ip]);      //print sorting file
                char color_p[1024];
                if(strcmp(arr[ip], "html_ls.html") == 0)continue;
                strcpy(color_p,what_color(path,prev_dir,aflag,arr[ip],color_p));
                 if(strcmp(arr[ip],"..")==0){ 
                fprintf(file,"<tr><td style =\"color:%s\"><a href =\"%s\">%s</a></td></tr>", color_p,prev_dir,arr[ip]);  //print with color and hyperlink
                }
                else{
                char hyper[1024]; // make path of hyper link to file
                strcpy(hyper,path);  //copy path
                strcat(hyper,"/");   //add '/'
                strcat(hyper,arr[ip]);  //add file name
                fprintf(file,"<tr><td style =\"color:%s\"><a href =\"%s\">%s</a></td></tr>", color_p,hyper,arr[ip]);  //print with color and hyperlink
                }  

             }
             fprintf(file,"</table>"); //close table
            
            for(int k=0; k< dir_num; k++){   //deallocate arr
                 free(arr[k]);
            }
           free(arr);
            closedir(dirp);   //closedir dirp
        }
        }
    }
    else if(lflag ==1 && rflag==0){ //option l and no option r               
        struct stat buf;     //for counting total block size
        int local_check=0;
        if(aflag ==0){
            local_check =1; //if 1 -> no print hidden file
        }
        else{
            local_check =0;  //if 0-> print include hidden file
        }
        for(int i=0; i<file_num; i++){
        int total =0;
        getcwd(pathname,1024); //get current working directory in pathname
        if(access(arr_file[i],F_OK)<0){
             printf("cannot access \'%s\' : No such directory\n", arr_file[i]);
            continue;
        }
        printf("Directory path: %s\n",pathname);
        fprintf(file,"<h3>Directory path: %s\n</h3>",pathname);
        if(hflag ==0){    //if hflag is 0
           printf("total : %d\n",total/2);
           fprintf(file,"<h3>total : %d\n</h3>",total/2);
           }
            if(hflag == 1){  //if hflag is 1
            total = total/2;
            char size_str[20];
            printf("total ");
            fprintf(file,"<h3>total ");
             if (total < 1024) {
               sprintf(size_str, "%.0fK\n", ((double)total));  //print readble size of K
            } else if (total < 1024 * 1024) {
               sprintf(size_str, "%.0fM\n", ceil(total / (1024.0)));  //print readble size of M
            } else {
                sprintf(size_str, "%.0fG\n", ceil(total / (1024.0 * 1024.0)));  //print readble size of G
            }
            printf("%s",size_str);
            fprintf(file,"%s</h3>",size_str);
        }
         fprintf(file, "<table border=\"1\">\n"); //setting table border
	    fprintf(file, "<tr><th>Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th></tr>\n");  //set table head name
        option_l(arr_file[i],pathname,local_check,pathname,hflag,file); //call option_l function
        fprintf(file,"</table>");
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
            getcwd(path, 1024);        //sotre path
            strcat(path, p_path);   
            getcwd(prev_dir,1024);      //store prev_dir
            if((arr_dir[i][0])== '/'){   //if '/' exist
               strcpy(path,arr_dir[i]);
            }
            printf("Directory path: %s\n",path);
            fprintf(file,"<h3>Directory path: %s\n</h3>",path);
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
            total =0;
            sort_hidden(dir_num,arr,max); //sorting file include hidden file
            for(int j=0; j<dir_num; j++){
                
                total = total_blockSize(arr[j],path,total,local_check,prev_dir);  //update total calling total_blockSize function
           }
            if(hflag ==0){    //if hflag is 0
           printf("total : %d\n",total/2);
           fprintf(file,"<h3>total : %d\n</h3>",total/2);
           }
            if(hflag == 1){  //if hflag is 1
            total = total/2;
            char size_str[20];
            printf("total ");
            fprintf(file,"<h3>total :");
             if (total < 1024) {
               sprintf(size_str, "%.0fK\n", ((double)total));  //print readble size of K
            } else if (total < 1024 * 1024) {
               sprintf(size_str, "%.0fM\n", ceil(total / (1024.0)));  //print readble size of M
            } else {
                sprintf(size_str, "%.0fG\n", ceil(total / (1024.0 * 1024.0)));  //print readble size of G
            }
            printf("%s",size_str);
            fprintf(file,"%s</h3>",size_str);
        }
            if(sflag ==1){  //if sflag is 1
                arr = option_s(dir_num,arr,max,path,prev_dir);
           }
           fprintf(file, "<table border=\"1\">\n"); //setting table border
	       fprintf(file, "<tr><th>Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th></tr>\n");  //set table head name
            for(int j=0; j<dir_num; j++){
                if(strcmp(arr[j], "html_ls.html") == 0)continue; //no print html_ls.html file
                option_l(arr[j],path,local_check,prev_dir,hflag,file); //call option_l function 
            }
           fprintf(file,"</table>");
            for(int k=0; k< dir_num; k++){   //deallocate arr
                 free(arr[k]);
            }
           free(arr);
            closedir(dirp);   //closedir dirp
        }
    }
    
      else if(lflag ==1 && rflag==1){ //option l
        struct stat buf;     //for counting total block size
        int local_check=0;
        if(aflag ==0){
            local_check =1; //if 1 -> no print hidden file
        }
        else{
            local_check =0; //if 0 ->  print include hidden file
        }
        for(int i=file_num-1; i>=0; i--){
        int total =0;
        getcwd(pathname,1024); //get current working directory in pathname
        if(access(arr_file[i],F_OK)<0){
             printf("cannot access \'%s\' : No such directory\n", arr_file[i]);
            continue;
        }
        printf("Directory path: %s\n",pathname);
        fprintf(file,"<h3>Directory path: %s\n</h3>",pathname);
        if(hflag ==0){    //if hflag is 0
           printf("total : %d\n",total/2);
           fprintf(file,"<h3>total : %d\n</h3>",total/2);
        }
        if(hflag == 1){  //if hflag is 1
        total = total/2;
        char size_str[20];
        printf("total ");
        fprintf(file,"<h3>total ");
            if (total < 1024) {
            sprintf(size_str, "%.0fK\n", ((double)total));  //print readble size of K
        } else if (total < 1024 * 1024) {
            sprintf(size_str, "%.0fM\n", ceil(total / (1024.0)));  //print readble size of M
        } else {
            sprintf(size_str, "%.0fG\n", ceil(total / (1024.0 * 1024.0)));  //print readble size of G
        }
        printf("%s",size_str);
        fprintf(file,"%s</h3>",size_str);
        }
        fprintf(file, "<table border=\"1\">\n"); //setting table border
	    fprintf(file, "<tr><th>Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th></tr>\n");  //set table head name
        option_l(arr_file[i],pathname,local_check,pathname,0,file); //call option_l function
        fprintf(file,"</table>");
       }
            
        for(int i=directory_num-1;i>=0;i--){
            
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
            if((arr_dir[i][0])== '/'){  //if absolute path
               strcpy(path,arr_dir[i]);
            }
            printf("Directory path: %s\n",path); //print format
            fprintf(file,"<h3>Directory path: %s\n</h3>",path); //print format
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
                total = total_blockSize(arr[j],path,total,local_check,prev_dir);  //update total calling total_blockSize function
           }
             if(hflag ==0){
           printf("total : %d\n",total/2);
           fprintf(file,"<h3>total : %d\n</h3>",total/2);
           }
            if(hflag == 1){
            char size_str[20]; //readable size arr
            printf("total ");
            fprintf(file,"<h3>total :");
            total = total /2;
             if (total < 1024) {
               sprintf(size_str, "%.0fK\n", ((double)total)); //print readble size of K
            } else if (total < 1024 * 1024) {
               sprintf(size_str, "%.0fM\n", ceil(total / (1024.0))); //print readble size of M
            } else {
                sprintf(size_str, "%.0fG\n", ceil(total / (1024.0 * 1024.0))); //print readble size of G
            }
            printf("%s",size_str);
            fprintf(file,"%s</h3>",size_str);
        }
            if(sflag ==1){
                arr = option_s(dir_num,arr,max,path,prev_dir); //call option_s function
           }

           fprintf(file, "<table border=\"1\">\n"); //setting table border
	       fprintf(file, "<tr><th>Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th></tr>\n");  //set table head name
            
            for(int j=dir_num-1; j>=0; j--){ //print in converse
               if(strcmp(arr[j], "html_ls.html") == 0)continue; //no print html_ls.html file
                option_l(arr[j],path,local_check,prev_dir,hflag,file); //call option_l function 
            }
           fprintf(file,"</table>");
            for(int k=0; k< dir_num; k++){   //deallocate arr
                 free(arr[k]);
            }
           free(arr);
            closedir(dirp);   //closedir dirp
        }
    }
    else if(lflag==0 && rflag ==0 && aflag ==0 && hflag==0 && sflag==0){ //no option   
    DIR* dirp2 = NULL; //value of opening directory
    char path[1024];  //pathname
    char prev_dir[1024]; //sotre prev_dir
    int exist=0; //if pattern exists
    int it=0; //iterator
    char pattern[1024]; //store pattern
    char origin[1024];
    getcwd(origin,1024); //get current working directory
        for(int i=0;i <pat_num; i++)
        {
            
            if(arr_pat[i][0] == '/')  //if absolute path exist
            {   
                exist =1;   //exist is 1
                strcpy(pattern, arr_pat[i]); //update path to absolute path
                strcpy(path, pattern);
            }
            if(arr_pat[i][0] != '/'){  //no absolute wild card
                dirp = opendir(".");  //open current directory
                getcwd(path,1024);
                strcpy(prev_dir,path);
                strcpy(pattern,arr_pat[i]);
                exist=2;  //exist is 2
                
            }
            else{
                strcpy(path, pattern);
                int num = 0; //initialzie
                int ii = 0; //store index
                for(num=0; num<strlen(path);num++){
                    if(path[num] == '/') //if meet '/'
                    {
                        ii = num;
                        continue;
                    }
                }
                path[ii] = '\0'; //no trash value

                strcpy(pattern, pattern + ii + 1);
  
                strcpy(prev_dir,path);
                dirp = opendir(path); //open path directory
                
            }
            int f_num = 0;

            while((dir=readdir(dirp)) != NULL){  //read dirp's directory until the end's file(or directory)
            f_num += 1;                   //increase numbers of dir_num    
            if(strlen(dir->d_name)>max){       
                max = strlen(dir->d_name);   //modify max using strlen
                }
            }
            max = max+1;                      //make free space
            char **arr;                       //array of 2D
            arr = malloc(sizeof(char*)*f_num); //col is dir_num
            for(int j=0; j<f_num; j++){                 
                arr[j] = malloc(sizeof(char)*max);   //row is max
            }
            
            rewinddir(dirp);   //for reusing dirp
            int f_num1=0;    //for iterator
            while((dir=readdir(dirp)) != NULL){
                strcpy(arr[f_num1], dir->d_name); //put dir->d_name in 2D array
                f_num1++;  //dir_num1 increases
            }
            f_num1 -=1;
            sort_hidden(f_num,arr,max);  //sorting name

            if(exist==1){
                for(int i=0; i<f_num; i++){
                if(!fnmatch(pattern, arr[i],0)){   //if matching with pattern   
                strcpy(path,prev_dir);
                if(arr[i][0]=='.')continue;  //not print file name starting '.'
                strcat(path,"/");
                strcat(path, arr[i]);
                if((dirp2=opendir(path))==NULL){ //if no dir -> print
                printf("Directory path: %s\n",path);    //if not dir -> print
                fprintf(file,"<h3>Directory path: %s\n</h3>",path);    //if not dir -> print
                wild(prev_dir,path,pattern,1,file); //go to wild function
                }
                }
            }
                for(int i=0; i<f_num; i++){
                if(!fnmatch(pattern, arr[i],0)){      //if matching with pattern   
                strcpy(path,prev_dir);
                if(arr[i][0]=='.')continue;  //not print file name starting '.'
                strcat(path,"/");
                strcat(path, arr[i]);
                if((dirp2=opendir(path))!=NULL){ //if dir -> print
                    printf("Directory path: %s\n",path);
                    fprintf(file,"<h3>Directory path: %s\n</h3>",path);
                    wild(prev_dir,path,pattern,1,file); //go to wild function
                    closedir(dirp2); //close dirp2
                    }
                }
            }
            }
            else if(exist==2){
                for(int i=0; i<f_num; i++){
                if(!fnmatch(pattern, arr[i],0)){     //if matching with pattern   
                strcpy(path,prev_dir);
                if(arr[i][0]=='.')continue;  //not print file name starting '.'
                strcat(path,"/");
                strcat(path, arr[i]);
                if((dirp2=opendir(path))==NULL){
                printf("%s\n",arr[i]);    //if not dir -> print
                fprintf(file,"<h3>Directory path: %s\n</h3>",path);
                wild(prev_dir,path,pattern,2,file); //go to wild function
                }
                }
            }
                for(int i=0; i<f_num; i++){
                if(!fnmatch(pattern, arr[i],0)){     //if matching with pattern   
                strcpy(path,prev_dir);
                if(arr[i][0]=='.')continue;  //not print file name starting '.'
                strcat(path,"/");
                strcat(path, arr[i]);
                if((dirp2=opendir(path))!=NULL){ //if  dir -> print
                    printf("Directory path: %s\n",path);
                    fprintf(file,"<h3>Directory path: %s\n</h3>",path);
                    wild(prev_dir,path,pattern,2,file); //go to wild function
                    closedir(dirp2);  //close dirp2
                    }
                }
            }
            }

            chdir(origin);

            for(int k=0; k< f_num; k++){   //deallocate arr
                 free(arr[k]);
            }
            free(arr);
            closedir(dirp);   //closedir dirp


        }
        chdir(origin);                                                             
        for(int i=0; i<file_num; i++){   //print file
              if(arr_file[i][0]=='.')continue;  //not print file name starting '.'
                if(access(arr_file[i],F_OK)<0){
                    printf("cannot access \'%s\' : No such directory\n", arr_file[i]);
                    continue;
                 }
                 
                char path[1024]; //store path
                char prev_dir[1024]; //store prev directory
                getcwd(path,1024);
                if(arr_file[i][0]!='/'){  
                fprintf(file,"<h3>Directory path: %s\n</h3>",path);
                }
                else{
                strcpy(path,arr_file[i]);
                fprintf(file,"<h3>Directory path: %s\n</h3>",path);
                }
                getcwd(prev_dir,1024);
                printf("%s\n", arr_file[i]);      //print sorting file
                char color_p[1024]; //value of storing color
                if(strcmp(arr_file[i], "html_ls.html") == 0)continue;  //no print html_ls.html file
                fprintf(file, "<table border=\"1\">\n"); //setting table border
	            fprintf(file, "<tr><th>Name</th></tr>\n");  //set table head name
                strcpy(color_p,what_color(path,prev_dir,aflag,arr_file[i],color_p));  //return what color the arr_file[i] is
                char hyper[1024]; // make path of hyper link to file
                strcpy(hyper,path);  //copy path
                strcat(hyper,"/");   //add '/'
                strcat(hyper,arr_file[i]);  //add file name
                //printf("hyper: %s\n",hyper); 
                fprintf(file,"<tr><td style =\"color:%s\"><a href =\"%s\">%s</a></td></tr>", color_p,hyper,arr_file[i]);  //print with color and hyperlink 
                fprintf(file,"</table>");
       }
       
        
        for(int i=0;i<directory_num;i++){ //print directory

            if(access(arr_dir[i],F_OK)<0){  //access error message
            printf("cannot access \'%s\' : No such directory\n", arr_dir[i]); //if there isn't directory
            continue;
            }
            char path[1024];  //store path
            char prev_dir[1024]; //store prev directory
            getcwd(path,1024);  //path = current directory path
            getcwd(prev_dir,1024);
            if(arr_dir[i][0]=='/'){  //if parameter is absolute path
                strcpy(path,arr_dir[i]);  //path is parameter
            }
            else{   //if not absolute path
            strcat(path,"/");
            strcat(path,arr_dir[i]);   //make absolute path
            }
            printf("%s:\n",arr_dir[i]);  //print directory
            fprintf(file,"<h3>Directory path: %s\n</h3>",path); //print to html
            dirp = opendir(arr_dir[i]); //open directory
            dir_num =0;
            int total =0;  //initialize total value
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

            sort_hidden(dir_num,arr,max); //sorting arr
            fprintf(file, "<table border=\"1\">\n"); //setting table border
	        fprintf(file, "<tr><th>Name</th></tr>\n");  //set table head name
            for(int ip=0; ip<dir_num; ip++){
             char color_p[1024]; //parameter of color
             if(arr[ip][0]=='.')continue;  //not print file name starting '.'
             if(strcmp(arr[ip], "html_ls.html") == 0)continue;
             printf("%s\n", arr[ip]);      //print sorting file
             strcpy(color_p,what_color(path,prev_dir,aflag,arr[ip],color_p));
             char hyper[1024]; // make path of hyper link to file
             strcpy(hyper,path);  //copy path
             strcat(hyper,"/");   //add '/'
             strcat(hyper,arr[ip]);  //add file name
             fprintf(file,"<tr><td style =\"color:%s\"><a href =\"%s\">%s</a></td></tr>", color_p,hyper,arr[ip]);  //print with color and hyperlink
            }
            fprintf(file,"</table>");

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
            char *big_arr1 = strdup(arr[i]);   //copy arr[i]
            char *big_arr2 = strdup(arr[j]);   //copy arr[j]
            for (int k = 0; k < strlen(big_arr1); k++) {
                big_arr1[k] = toupper(big_arr1[k]);   //change letters to big letters
            }
            for (int k = 0; k < strlen(big_arr2); k++) {
                big_arr2[k] = toupper(big_arr2[k]);         //change letters to captial
            }
            if(arr[i][0]=='.' && arr[j][0] =='.'){
                if(strcmp(big_arr1+1,big_arr2+1)>0){   //strcasecmp(a+1,b+1): if result >0 -> b+1 is before a+1 in dicitionary
                    strcpy(tmp, arr[i]);           //change order of arr[i]and arr[j] using tmp
                    strcpy(arr[i],arr[j]);
                    strcpy(arr[j],tmp);
                }
            }
            else  if(arr[i][0]=='.' && arr[j][0] !='.'){
                if(strcmp(big_arr1+1,big_arr2)>0){   //strcasecmp(a+1,b): if result >0 -> b is before a+1 in dicitionary
                    strcpy(tmp, arr[i]);           //change order of arr[i]and arr[j] using tmp
                    strcpy(arr[i],arr[j]);
                    strcpy(arr[j],tmp);
                }
            }
            else  if(arr[i][0]!='.' && arr[j][0] =='.'){
                if(strcmp(big_arr1,big_arr2+1)>0){   //strcasecmp(a,b+1): if result >0 -> b+1 is before a in dicitionary
                    strcpy(tmp, arr[i]);           //change order of arr[i]and arr[j] using tmp
                    strcpy(arr[i],arr[j]);
                    strcpy(arr[j],tmp);
                }
            }
            else{
                if(strcmp(big_arr1,big_arr2)>0){    //strcasecmp(a,b): if result >0 -> b is before a in dicitionary
                    strcpy(tmp, arr[i]);           //change order of arr[i]and arr[j] using tmp
                    strcpy(arr[i],arr[j]);
                    strcpy(arr[j],tmp);
                }
            }
            free(big_arr1); //deallocate big_arr1
            free(big_arr2); //deallocate big_arr2
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
//       char*prev_dir->back to prev path                      //
//       int hflag -> hflag is 1?                              //
//       FILE* file -> pointer of file stream                  //
//Output: none-> error                                          //
//        print information of arr(file or directory)           //
//Purpose: search file(or directory)'s information using stat   //
//////////////////////////////////////////////////////////////////
void option_l(char *arr,char*path,int check,char*prev_dir,int hflag, FILE* file){
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
        chdir(prev_dir); //back to prev directory
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
        if(arr[0]=='.'){
            chdir(prev_dir); //back to prev directory
            return;
        }
    }
  
    pwd = getpwuid(buf.st_uid); //get st_uid to char
    if(pwd == NULL){ //error message
        perror("getpwuid");
        printf("Error: %s\n", strerror(errno));
        chdir(prev_dir); //back to prev directory
        return;
    }
    grp = getgrgid(buf.st_gid); // get group information using GID
    if(grp == NULL){ //error message
        perror("getgrgid");
        printf("Error: %s\n", strerror(errno));
        chdir(prev_dir); //back to prev directory
        return;
    }
    char size_str[20];
    if(hflag == 1){  //if hflag is 1
        if (buf.st_size < 1024) {   
            sprintf(size_str, "%ld", buf.st_size); //print size
        } else if (buf.st_size < 1024 * 1024) {
            sprintf(size_str, "%.0fK", ceil(buf.st_size / 1024.0)); //print size with K
        } else if (buf.st_size < 1024 * 1024 * 1024) {
            sprintf(size_str, "%.0fM", ceil(buf.st_size / (1024.0 * 1024))); //print size with M
        } else {
            sprintf(size_str, "%.0fG", ceil(buf.st_size / (1024.0 * 1024 * 1024))); //print size with G
        }
    }
    char color_p[1024];
    strcpy(color_p, what_color(path,prev_dir,check,arr,color_p));
    strftime(t_buf, sizeof(t_buf), "%b %d %H:%M", localtime(&buf.st_mtime)); // format time using t_buf
    
    if(hflag==1){ //hflag ->1, readable size printing
    printf("%s\t%lu\t%s\t%s\t%s\t",perm,buf.st_nlink, pwd->pw_name,grp->gr_name,size_str); //print permisiion, number of link, username, groupname, size using '\t'
    if(strcmp(arr,"..")==0){  //if ..
        char pre_path[1024]; //store parent dir path
        strcpy(pre_path,path);
        strcat(pre_path,"/.."); //hyper link is parent directory
        fprintf(file,"<tr><td style =\"color:%s\"><a href =\"%s\">%s</a></td>", color_p, pre_path,arr); // print directory or file name
    }
    else{
    char hyper[1024]; // make path of hyper link to file
        strcpy(hyper,path);  //copy path
        strcat(hyper,"/");   //add '/'
        strcat(hyper,arr);  //add file name
    fprintf(file,"<tr><td style =\"color:%s\"><a href =\"%s\">%s</a></td>", color_p, hyper,arr); // print directory or file name
    }
    fprintf(file,"<td style =\"color:%s\">%s</td><td style =\"color:%s\">%lu</td><td style =\"color:%s\">%s</td>",color_p,perm,color_p,buf.st_nlink,color_p,pwd->pw_name); //print permisiion, number of link, username, groupname, size using '\t'
    fprintf(file,"<td style =\"color:%s\">%s</td><td style =\"color:%s\">%s</td>",color_p,grp->gr_name,color_p,size_str); //print permisiion, number of link, username, groupname, size using '\t'
    fprintf(file,"<td style =\"color:%s\">%s</td></tr>",color_p,t_buf);

    }
    
    else if(hflag==0){  //if no hflag -> no readable
    printf("%s\t%lu\t%s\t%s\t%ld\t",perm,buf.st_nlink, pwd->pw_name,grp->gr_name,buf.st_size); //print permisiion, number of link, username, groupname, size using '\t'
    if(strcmp(arr,"..")==0){
        char pre_path[1024]; //store parent dir path
        strcpy(pre_path,path);
        strcat(pre_path,"/.."); //hyper link is parent directory
        fprintf(file,"<tr><td style =\"color:%s\"><a href =\"%s\">%s</a></td>", color_p, pre_path,arr); // print directory or file name
    }
    else{   
    char hyper[1024]; // make path of hyper link to file
    strcpy(hyper,path);  //copy path
    strcat(hyper,"/");   //add '/'
    strcat(hyper,arr);  //add file name
    fprintf(file,"<tr><td style =\"color:%s\"><a href =\"%s\">%s</a></td>", color_p, hyper,arr); // print directory or file name
    }
    fprintf(file,"<td style =\"color:%s\">%s</td><td style =\"color:%s\">%lu</td><td style =\"color:%s\">%s</td>",color_p,perm,color_p,buf.st_nlink,color_p,pwd->pw_name); //print permisiion, number of link, username, groupname, size using '\t'
    fprintf(file,"<td style =\"color:%s\">%s</td><td style =\"color:%s\">%ld</td>",color_p,grp->gr_name,color_p,buf.st_size); //print permisiion, number of link, username, groupname, size using '\t'
    fprintf(file,"<td style =\"color:%s\">%s</td></tr>",color_p,t_buf);
    }
    
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
    {    //perror message
        perror("stat");
        printf("%s: \n", arr);
        printf("error_total: %s\n", strerror(errno)); 
       return 0; //end
    }
    
    if(check == 1){ //check ==1 -> no -a option
        if(arr[0]=='.'){
            chdir(prev_dir); //back to prev directory
            return total;  //not add of hidden file's block size
        }
    }
    total += buf.st_blocks; //update total
    chdir(prev_dir); //back to prev directory
    return total; //return updated total
    
}
//////////////////////////////////////////////////////////////////
//option_s                                                      //
//=====================================================         //
//Input: int dir_num -> directory numvber for iterator          //
//       char* arr[]-> want to sort char*arr[]                  //
//       int max -> to allocate appropriate temp                //
//       char *path-> change to path                            //
//       char *prev_dir-> back to prev_dir                      //
//Output:   sorted 2d array                                     //
//Purpose: sort file or dir of size using bubble sorting include //        
//         hidden file(start with '.')                          //
//////////////////////////////////////////////////////////////////
char** option_s(int dir_num, char*arr[],int max,char *path, char*prev_dir){
    chdir(path);
    
    
    for (int i =0; i<dir_num-1; i++){        //like bubble-sorting
      for(int j= i+1; j<dir_num; j++){
            struct stat buf1;
            struct stat buf2;
           if(lstat(arr[i], &buf1) <0 || lstat(arr[j],&buf2)<0)
            {    //perror message
            perror("stat");
            printf("error_total: %s\n", strerror(errno));
            chdir(prev_dir); //back to prev directory 
            return arr;  //end
            }
            if (buf1.st_size < buf2.st_size) {  //compare size 
                char temp[max];   //sotre temporary value
                strncpy(temp, arr[i], max);
                strncpy(arr[i], arr[j], max);
                strncpy(arr[j], temp, max);
            }
            if(buf1.st_size == buf2.st_size){  //if size is same -> sorting according name
            char temp[max]; //temporary sotre value
            char *big_arr1 = strdup(arr[i]); //update arr[i] to big_arr1
            char *big_arr2 = strdup(arr[j]);  //update arr[j] to big_arr2
            for (int k = 0; k < strlen(big_arr1); k++) {
                big_arr1[k] = toupper(big_arr1[k]);  //all letter -> capital
            }
            for (int k = 0; k < strlen(big_arr2); k++) {
                big_arr2[k] = toupper(big_arr2[k]);  //all letter -> capital
            }
            if(arr[i][0]=='.' && arr[j][0] =='.'){
                if(strcmp(big_arr1+1,big_arr2+1)>0){   //strcasecmp(a+1,b+1): if result >0 -> b+1 is before a+1 in dicitionary
                    strcpy(temp, arr[i]);           //change order of arr[i]and arr[j] using tmp
                    strcpy(arr[i],arr[j]);
                    strcpy(arr[j],temp);
                }
            }
            else  if(arr[i][0]=='.' && arr[j][0] !='.'){
                if(strcmp(big_arr1+1,big_arr2)>0){   //strcasecmp(a+1,b): if result >0 -> b is before a+1 in dicitionary
                    strcpy(temp, arr[i]);           //change order of arr[i]and arr[j] using tmp
                    strcpy(arr[i],arr[j]);
                    strcpy(arr[j],temp);
                }
            }
            else  if(arr[i][0]!='.' && arr[j][0] =='.'){
                if(strcmp(big_arr1,big_arr2+1)>0){   //strcasecmp(a,b+1): if result >0 -> b+1 is before a in dicitionary
                    strcpy(temp, arr[i]);           //change order of arr[i]and arr[j] using tmp
                    strcpy(arr[i],arr[j]);
                    strcpy(arr[j],temp);
                }
            }
            else{
                if(strcmp(big_arr1,big_arr2)>0){    //strcasecmp(a,b): if result >0 -> b is before a in dicitionary
                    strcpy(temp, arr[i]);           //change order of arr[i]and arr[j] using tmp
                    strcpy(arr[i],arr[j]);
                    strcpy(arr[j],temp);
                }
            }
            free(big_arr1);   //deallocate big_arr1
            free(big_arr2);    //deallocate big_arr2
            }

         }
        }
    chdir(prev_dir); //back to prev_dir
    return arr;
}
//////////////////////////////////////////////////////////////////
//wild                                                          //
//=====================================================         //
//Input: int check -> checking if exist is 1 or 2               //
//       char* pattern -> checking pattern(wild card)           //
//       char *path-> change to path                            //
//       char *prev_dir-> back to prev_dir                      //
//       FILE* file -> pointer of file stream                   //
//Output:   void -> just print entries                          //
//Purpose: print entries of matching directory of matching with //        
//         wild cards -> differernt printing format of exist 1,2 //
///////////////////////////////////////////////////////////////////
void wild(char*prev_dir, char*path,char*pattern,int check,FILE *file){
    
    DIR *dirp = NULL;                  //get directory's pointer
   struct dirent *dir;                //use struct dirent
    DIR *dirp2 = NULL;
   int dir_num=0; // number of file or direcotry
   int max =0;    // max size of name of file or directory
    //chdir(path);
          
            dirp = opendir(path); //open directory
            if(dirp==NULL){  //if no directory
                return; //return
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
            if(arr == NULL){
                //chdir(prev_dir);
                return;
            }
            for(int j=0; j<dir_num; j++){                 
                arr[j] = malloc(sizeof(char)*max);   //row is max
                if(arr[j] == NULL){
                    return;
                }
            }
            
            rewinddir(dirp);   //for reusing dirp
            int dir_num1=0;    //for iterator
            while((dir=readdir(dirp)) != NULL){
                strcpy(arr[dir_num1], dir->d_name); //put dir->d_name in 2D array
                dir_num1++;  //dir_num1 increases
            }
            dir_num1 -=1;
            
            sort_hidden(dir_num,arr,max); //soting arr
            if(dir_num>0){
            fprintf(file, "<table border=\"1\">\n");
	        fprintf(file, "<tr><th>Name</th></tr>\n");
            }
            if(check ==1){   //if check ==1
            
                for(int ip=0; ip<dir_num; ip++){
                char color_p[1024];
                if((dirp2 = opendir(arr[ip]))==NULL){ //if no dir -> print
                if(arr[ip][0]=='.')continue;  //not print file name starting '.'
                if(strcmp(arr[ip], "html_ls.html") == 0)continue; //if html -> not print
                printf("%s\n", arr[ip]);      //print sorting file
                strcpy(color_p,what_color(path,prev_dir,0,arr[ip],color_p)); 
                char hyper[1024]; // make path of hyper link to file
                strcpy(hyper,path);  //copy path
                strcat(hyper,"/");   //add '/'
                strcat(hyper,arr[ip]);  //add file name
                fprintf(file,"<tr><td style =\"color:%s\"><a href =\"%s\">%s</a></td></tr>", color_p,hyper,arr[ip]);  //print with color and hyperlink
               }}
           
                for(int ip=0; ip<dir_num; ip++){
                char color_p[1024];
                if((dirp2 = opendir(arr[ip]))!=NULL){ //if  dir -> print
                if(arr[ip][0]=='.')continue;  //not print file name starting '.'
                if(strcmp(arr[ip], "html_ls.html") == 0)continue; //if html -> not print
                strcpy(color_p,what_color(path,prev_dir,0,arr[ip],color_p)); 
                printf("%s\n", arr[ip]);      //print sorting file
                char hyper[1024]; // make path of hyper link to file
                strcpy(hyper,path);  //copy path
                strcat(hyper,"/");   //add '/'
                strcat(hyper,arr[ip]);  //add file name
               fprintf(file,"<tr><td style =\"color:%s\"><a href =\"%s\">%s</a></td></tr>", color_p,hyper,arr[ip]);  //print with color and hyperlink 
                
                closedir(dirp2);
               }
               }

               fprintf(file, "</table>"); //close table
            
            }

            else if(check == 2){
                 
                for(int ip=0; ip<dir_num; ip++){
                char color_p[1024];
                if((dirp2 = opendir(arr[ip]))==NULL){ //if no dir -> print
                if(arr[ip][0]=='.')continue;  //not print file name starting '.'
                if(strcmp(arr[ip], "html_ls.html") == 0)continue; //if html -> not print
                printf("%s\n", arr[ip]);      //print sorting file
                strcpy(color_p,what_color(path,prev_dir,0,arr[ip],color_p)); 
                char hyper[1024]; // make path of hyper link to file
                strcpy(hyper,path);  //copy path
                strcat(hyper,"/");   //add '/'
                strcat(hyper,arr[ip]);  //add file name
                fprintf(file,"<tr><td style =\"color:%s\"><a href =\"%s\">%s</a></td></tr>", color_p,hyper,arr[ip]);  //print with color and hyperlink
               }}
           
                for(int ip=0; ip<dir_num; ip++){
                char color_p[1024];
                if((dirp2 = opendir(arr[ip]))!=NULL){ //if  dir -> print
                if(arr[ip][0]=='.')continue;  //not print file name starting '.'
                if(strcmp(arr[ip], "html_ls.html") == 0)continue; //if html -> not print
                strcpy(color_p,what_color(path,prev_dir,0,arr[ip],color_p)); 
                printf("%s\n", arr[ip]);      //print sorting file
                char hyper[1024]; // make path of hyper link to file
                strcpy(hyper,path);  //copy path
                strcat(hyper,"/");   //add '/'
                strcat(hyper,arr[ip]);  //add file name
                fprintf(file,"<tr><td style =\"color:%s\"><a href =\"%s\">%s</a></td></tr>", color_p,hyper,arr[ip]);  //print with color and hyperlink
                closedir(dirp2);
               }
               }
                fprintf(file, "</table>"); //close table
               
            }
           
            for(int k=0; k< dir_num; k++){   //deallocate arr
                 free(arr[k]);
            }
           free(arr);
            closedir(dirp);   //closedir dirp

}
//////////////////////////////////////////////////////////////////
//what_color                                                    //
//=====================================================         //
//Input: int aflag -> checking if aflag is 1                    //
//       char* arr-> want to know arr's information             //
//       char *path-> change to path                            //
//       char *prev_dir-> back to prev_dir                      //
//       char *color-> return appropriate color information     //
//Output:   return -> color according to type                   //
//Purpose: to know what type arr is and what color the arr have //       
//////////////////////////////////////////////////////////////////
char* what_color(char*path, char*prev, int aflag, char*arr,char* color){
    chdir(path); //change directory
    struct stat buf;
   
    if(lstat(arr, &buf) <0 )  //lstat of arr
    {    //perror message
        perror("stat");
        printf("%s: \n", arr);
        printf("error_color: %s\n", strerror(errno)); 
        chdir(prev); //change prev directory
        return color;
    }

    if(S_ISLNK(buf.st_mode)){ //if link file
        color = "green";   
        
    }
    else if(S_ISDIR(buf.st_mode)){ //if directory
        color = "blue";
       
    }
    else{                        //the others
        color = "red";
        
    }

    chdir(prev); //change prev directory
    return color; //return color
}