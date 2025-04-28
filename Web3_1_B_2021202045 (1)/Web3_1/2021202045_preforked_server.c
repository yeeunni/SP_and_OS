////////////////////////////////////////////////////////////////////////////////
// File Name  :2021202045_preforked_server.c                                  //
// Date       :2023/05/17                                                     //  
// Os : Ubuntu 16.04 LTS 64bits                                               // 
// Author : Kim Ye Eun                                                        // 
// Student ID : 2021202045                                                    //
// ---------------------------------------------------                        //
// Title : System Programming Assignment #3-1                                 //
// Description : clients - server system that clients want request and server //
//               show ls -l or -al result to client. Also, server can show    //
//               appropriate result according to type of entries. server can  //
//               classify appropriate ip address of client. parent process    //
//               make 5 child in advance.                                     //
////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <grp.h>
#include <time.h>
#include<fcntl.h>
#include <math.h>
#include <sys/wait.h>
#define _GNU_SOURCE
#include <pwd.h>
#include <signal.h>
#include <fnmatch.h>
#define URL_LEN 256
#define BUFSIZE 40000  //fixed bufsize: 4000
#define PORTNO 40000  //fixed value of port number : 40000
#define MAX_CLIENT 100 //max simultaneous client number 
typedef struct client_info{  //struct that stores information of client
    int cum_num; //cumulative number of accessing
    pid_t pid;   //pid of client
    time_t time; //access time
    char ip_addr[INET_ADDRSTRLEN]; //ip address of client
    int port; //port number of client
}c_info; //c_info 

c_info ci_array[MAX_CLIENT]={0,}; //client ifo array
static int client_num=0; //client number

static int start=1; //index of start

int maxNchildren;
static pid_t *pids; //store child process's pid
static char* buf;


// function declaration
void option_l(char *arr, char *path, int check, char *prev_dir, char*response_message,char*url);
void sort_hidden(int dir_num, char *arr[], int max);
char *what_color(char *path, char *prev, int aflag, char *arr, char *color);
int total_blockSize(char *arr, char *path, int total, int check, char *prev_dir);
void alarm_handler(int sig);
int print_option(char* response_message,int aflag, int lflag, char*response_header, char*store_path,char*prev,int client_fd, char* url);
pid_t child_make(int i, int socketfd, int addrlen);
void child_main(int i, int socket_fd, int addrlen);
void child_handler(int sig);
void signalHandler_child(int sig);
int socket_fd;   //socket descripter
////////////////////////////////////////////////////////////////////
//signalHandler                                                   //
//=====================================================           //
// Input:  int sig-> get termination signal                       //
// Output: void                                                   //
// Purpose: exit child process then exit parent process           //                                  
////////////////////////////////////////////////////////////////////
void signalHandler(int sig){
    int stat; //store state of child process
     if(sig == SIGINT){
        for(int p=0;p<maxNchildren;p++){
            kill(pids[p],SIGTERM);         //send sigterm signal to child process
        }
        for(int p=0;p<maxNchildren;p++){
           
            wait(&stat);                  //wait child process
        }
        close(socket_fd); //close socket_fd
        time_t server_end_time = time(NULL);  //store server end time
        char *ct_2;     //store char of time_t
        ct_2 =ctime(&server_end_time); //time_t -> char
        ct_2[strlen(ct_2)-1]='\0';
        printf("[%s] Server is terminated.\n",ct_2); //print server is terminated
        exit(0); //exit parent process
    }
   
}
////////////////////////////////////////////////////////////////////
//signalHandler_child                                             //
//=====================================================           //
// Input:  int sig-> get termination signal                       //
// Output: void                                                   //
// Purpose: exit child according to terminaton key                //                                  
////////////////////////////////////////////////////////////////////
void signalHandler_child(int sig){
    if(sig == SIGTERM){
        time_t child_end_time = time(NULL);
        char *ct_1;     //store char of time_t
        ct_1 =ctime(&child_end_time); //time_t -> char
        ct_1[strlen(ct_1)-1]='\0';
        printf("[%s] %ld process is terminated.\n",ct_1,(long)getpid());
        exit(0);
    }
}
int main()
{   
    signal(SIGINT, signalHandler); //get signal to signalHandler function
    struct sockaddr_in server_addr, client_addr;  //server_addr , client_addr declaration
   
    int len, len_out;
    int opt = 1; 
    

    if ((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0)  //make socket
    {
        printf("Server : Can't open stream socket\n");      //error message
        return 0;
    }
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));   //setsockopt()
    
    memset(&server_addr, 0, sizeof(server_addr));   //initialize server_addr
    server_addr.sin_family = AF_INET;            //domain = AF_INET
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //initialize ip address and byte ordering
    server_addr.sin_port = htons(PORTNO);  //byte ordering port number

    if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) //bind socket_fd and server_addr
    {
        printf("Server : Can't bind\n");
        return 0;
    }
    time_t server_make_time = time(NULL); //store server make time
    char *ct_0;     //store char of time_t
    ct_0 =ctime(&server_make_time); //time_t -> char
    ct_0[strlen(ct_0)-1]='\0';
    printf("[%s] Server is started.\n",ct_0); //server is started
    listen(socket_fd, 5);  //can accept
    maxNchildren =5; //update maxNchildren
    int addrlen = sizeof(client_addr);
    pids = (pid_t *)malloc(maxNchildren * sizeof(pid_t)); //allocate memory
    signal(SIGINT,signalHandler);  //get signal
    signal(SIGALRM, alarm_handler); //get signal to alarm_handler function
    alarm(10); //alarm per 10 sec
    for(int p=0; p< maxNchildren; p++){
        pids[p] = child_make(p, socket_fd,addrlen); //call child_make()
    }
    for( ; ; )   //infinitive loop
        pause(); //loop until getting signal
   
    return 0;
}
////////////////////////////////////////////////////////////////////
//child_make                                                      //
//=====================================================           //
// Input:  int i -> which process                                 //
//         int socketfd-> server descripter                       //
//         int addrlen -> size of client_addr                     //
// Output: pid_t -> pid of child process                          //
// Purpose: store child process's pid and call child main function//                                  
////////////////////////////////////////////////////////////////////
pid_t child_make(int i, int socketfd, int addrlen){
    pid_t pid;
    if((pid = fork())>0){
        
        return(pid);
    }
    child_main(i,socketfd,addrlen);
}
////////////////////////////////////////////////////////////////////
//child_main                                                      //
//=====================================================           //
// Input:  int i -> which process                                 //
//         int socket_fd-> server descripter                      //
//         int addrlen -> size of client_addr                     //
// Output: void                                                   //
// Purpose: exectue child process                                 //                                  
////////////////////////////////////////////////////////////////////
void child_main(int i, int socket_fd, int addrlen){
    signal(SIGTERM,signalHandler_child); //when get signal from parent process
    signal(SIGUSR1,child_handler); //print information of history
    time_t child_make_time =time(NULL); //access time
    char *ct;     //store char of time_t
    ct =ctime(&child_make_time); //time_t -> char
    ct[strlen(ct)-1]='\0';
    printf("[%s] %ld process is forked.\n",ct,(long)getpid()); //print process is forked
   
   int status; //get child's termination condition
    while(1){
        struct sockaddr_in client_addr; 
         char path[1024]; //store current working directory
        char prev[1024];  //store prev directory
        getcwd(path,1024);  //initialize
        getcwd(prev,1024);  //initialize
        char link_path[1024]; // would go to link_path
        struct in_addr inet_client_address; 
        char buf[BUFSIZE] ={0, };    //store from client_fd
        char tmp[BUFSIZE] ={0, };      //temporary memory
        char response_header[BUFSIZE] ={0, };  //store response header
        char response_message[BUFSIZE] ={0, }; //store response message
        char url[BUFSIZE] ={0, };   //store url
        char method[20] ={0, };    //store method
        char * tok= NULL;         //using for strtok()
        char ip_buffer[256];  //get data of image
        int len = sizeof(client_addr); //initialize len
        
        int client_fd;
        client_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &len);  //accept -> make client_fd(socket)
        if(client_fd <0){ 
            printf("Server : accept failed\n"); //error message of accept failed
            return;
        }
        read(client_fd, buf, BUFSIZE);  //read from client_fd and store buf
        if( strlen(buf)== 0){    //ignore empty pk
            continue;
        }
        inet_client_address.s_addr = client_addr.sin_addr.s_addr;  //initialize
        time_t cur_time = time(NULL); //store time to cur_time
        client_num++;     //client_num increase
       
        strcpy(ci_array[client_num].ip_addr,inet_ntoa(inet_client_address)); //ip address -> change char type
        //store struct information
        ci_array[client_num].port = client_addr.sin_port; //update port number
        ci_array[client_num].time = cur_time;            //update time
        ci_array[client_num].cum_num = client_num;       //update cumulative number  
        FILE *ip_fp;                                    
        ip_fp = fopen("accessible.usr","r");           //file open
        if (ip_fp == NULL)                            
        {
            perror("Error opening file");                //file open error
            break;
        }
        int is_match=1;                                 //value if ip address is matched
        while(fgets(ip_buffer,256,ip_fp)!=NULL){    //automatically file read one line and store in ip_buffer[256]
            
            if(ip_buffer[ strlen(ip_buffer)-1]=='\n'){ //the last character is '\n'
                ip_buffer[ strlen(ip_buffer) -1] = '\0'; //change '\0'
            }
            is_match = 1;
           if(fnmatch(ip_buffer, inet_ntoa(inet_client_address), FNM_CASEFOLD) != 0){ //fnmatch with ip_buffer and client ip
             is_match = 0;  //update is_match
             continue;  
           }
        }
           if(is_match == 0){  //if not match
           //make error header and message
            sprintf(response_message, "<html><body><h1>Access denied!</h1><h2>Yout IP : %s </h2><h5>You have no permission to access this web server.<br>HTTP 403.6 - Forbidden: IP address reject</h5></body></html>", inet_ntoa(inet_client_address));
            sprintf(response_header, "HTTP/1.1 403.6 ERROR\r\nContent-Type: text/html\r\n\r\n");
            //send to client error header and message
            write(client_fd, response_header, strlen(response_header));
            write(client_fd, response_message, strlen(response_message));
            close(client_fd);
            continue;
           }
        fclose(ip_fp); //file close
       
        //signal(SIGINT,signalHandler);  //get signal
        
        puts("\n================New Client======================"); //print new client's information
        time_t client_make_time = time(NULL); //store server make time
        char *ct_3;     //store char of time_t
        ct_3 =ctime(&client_make_time); //time_t -> char
        ct_3[strlen(ct_3)-1]='\0';
        printf("[%s] \n",ct_3); //server is started
        printf("IP : %s\n",inet_ntoa(inet_client_address));         //client's ip
        printf("Port : %d\n",client_addr.sin_port);                 //client's port number   
        puts("==================================================");
        strcpy(tmp, buf); //copy buf to tmp 

        //puts(buf);   //print buf data to terminal 

        if(tmp == NULL){  //ignore if tmp is NULL
            continue;
        }
        tok = strtok(tmp," "); //cut method from tmp
        strcpy(method, tok);  //copy tok to method
        if(strcmp(method, "GET")==0)  //if method is GET
        {
                tok = strtok(NULL," "); //cut
                strcpy(url, tok); //initilaize url
        }
        int aflag=0;  //aflag declare
        int lflag=0;  //lflag declare


        if(strcmp(url,"/")==0){  //if url is "/" -> root directory
            //printf("i am home\n");
            lflag=1;   //update lflag
            aflag=0;
         
            strcpy(link_path,path);   //initialize link_path
            sprintf(response_header, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"); //update response_header, type : text/html
            sprintf(response_message, "<html><body><h1>Welcome to System Programming Http</h1><br>"); //update response_message
             if(print_option(response_message,aflag,lflag, response_header, link_path,prev,client_fd,url)==1){  //call print_option function
                break; //if error in print_option()
            }
        }
        else if(fnmatch("*.jpg", url, FNM_CASEFOLD) == 0 || fnmatch("*.png", url, FNM_CASEFOLD) == 0 || fnmatch("*.jpeg", url, FNM_CASEFOLD) == 0){  //if url is related with image file
            
            char *p = url;  //*p is url

            if (*p == '/') {  // if url's starting point is '/'
                p++;          //skip '/'
            }
            
            char buffer[8192];  //get data of image
            char *response = "HTTP/1.1 200 OK\r\nContent-Type: image/*\r\n\r\n"; //update response_header type: image/*
            send(client_fd, response, strlen(response), 0); //send it to client_fd
            FILE *fp = fopen(p, "rb");  //image file open
            if (fp != NULL) {
                while (1) {
                    size_t bytes_read = fread(buffer, 1, 8192, fp);  //reand and store
                    if (bytes_read > 0) {
                        send(client_fd, buffer, bytes_read, 0); //send it to client_fd
                    } else {  //error to read image file

                        break;
                    }
                }
                fclose(fp); //file close
            }
        }
        else{                                                   //if not root directory and if not image file
            
            aflag=1; //update aflag
            lflag=1; //update lflag
            
            strcpy(link_path,path); //copy path to link_path
            strcpy(prev,link_path); //copy path to prev
            strcat(link_path,url); //link_path + url
         

            sprintf(response_header, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");  //update response_header, type : text/html
            sprintf(response_message, "<html><body><h1>System Programming Http</h1><br>"); //update response_message
            if(print_option(response_message,aflag,lflag, response_header, link_path,prev,client_fd,url)==1){ //call print_option function
                break; //if error in print_option()
            }

         }
        printf("\n");
        puts("================Disconnected Client======================"); //disconnected message
        time_t client_end_time = time(NULL); //store server make time
        char *ct_4;     //store char of time_t
        ct_4 =ctime(&client_end_time); //time_t -> char
        ct_4[strlen(ct_4)-1]='\0';
        printf("[%s] \n",ct_4); //server is started
        printf("IP : %s\n",inet_ntoa(inet_client_address));    //client's ip
        printf("Port : %d\n",client_addr.sin_port);              //client's port number
        puts("=========================================================");
        close(client_fd);   //close client_fd  

    }
}
////////////////////////////////////////////////////////////////////
//alarm_handler                                                   //
//=====================================================           //
// Input:  int sig-> get alarm signal                             //
// Output: void                                                   //
// Purpose: print connection history and struct ci_array sorting  //
//          by bubble sorting and print struct information        //                                  
////////////////////////////////////////////////////////////////////
void alarm_handler(int sig){
    printf("=========Connection History==================================================\n"); //print format of connection history
    //printf("Number of request(s) : %d\n",client_num);                                 //print number of request   
    printf("NO.         IP            PID        PORT      TIME\n");                  //print format of information of struct
      for(int p=0; p< maxNchildren; p++){
        kill(pids[p],SIGUSR1);
      }
    alarm(10);
}
////////////////////////////////////////////////////////////////////
//child_handler                                                   //
//=====================================================           //
// Input:  int sig-> get alarm signal                             //
// Output: void                                                   //
// Purpose: print connection history and struct ci_array sorting  //
//          by bubble sorting and print struct information        //                                  
////////////////////////////////////////////////////////////////////
void child_handler(int sig){
   time_t tmp;       //temporary sotrage
     if(client_num > 10){ //client_num is over 10(store start at 1)
        start++;   //move start index
    }
    for (int i = start; i < client_num; i++)    //bubble sorting according to access time
    {
        for (int j = i + 1; j <= client_num; j++)
        {
            if (ci_array[j].time < ci_array[i].time)  //if array[i]>array[j]
            {                        
                tmp= ci_array[i].time;       //switch order 
                ci_array[i].time= ci_array[j].time;
                ci_array[j].time= tmp;

            }
        }
    }
    for(int i = start; i<=client_num; i++){ //start~client_num
        char *ct;     //store char of time_t
        ct =ctime(&(ci_array[i].time)); //time_t -> char
        printf("%d         %s      %d       %d      %s",ci_array[i].cum_num,ci_array[i].ip_addr,getpid(),ci_array[i].port,ct); //print information
    }
}
////////////////////////////////////////////////////////////////////
//print_option                                                    //
//=====================================================           //
// Input: char*response_message->update response_message          //
//        int aflag -> if aflag is 1                              //
//        int lflag -> if lflag is 1                              //
//        char*response_header->update response_header            //
//        char* store_path -> absolute path                       //
//        char*prev->back to prev path                            //
//        int client_fd->write to client_fd                       //
//        char*url-> to make hyper link                           //
// Output: int-> 1-:error                                         //
//         0: print information of arr(file or directory)         //
// Purpose: search file(or directory) and update response_header  //
//          and response_message                                  //
////////////////////////////////////////////////////////////////////
int print_option(char* response_message,int aflag, int lflag, char*response_header, char*store_path,char*prev,int client_fd, char* url)
{   
     DIR *dirp = NULL;      // get directory's pointer
        struct dirent *dir;    // use struct dirent
        int dir_num = 0;       // number of file or direcotry
        int max =0;         //max size of entry's name
        char response_tmp[BUFSIZE] ={0, }; //temporary memory

        int total = 0; // variable of counting block
        dir_num = 0; //initialzie
        int local_check = 1; // initialize
        if (aflag == 0)
        {
            local_check = 1; // option is -l
        }
        else if (aflag == 1)
        { // option is -al
            local_check = 0;
        }
        if((dirp = opendir(store_path))==NULL){  //if not directory
            if (access(store_path, F_OK) < 0){   //if not exist file
            sprintf(response_message, "<html><body><h1>Not Found</h1><h4>The request URL %s was not found on this server<br>HTTP 404 - Not Page Found</h4></body></html>", url); //error message
            sprintf(response_header, "HTTP/1.1 404 ERROR\r\nContent-Length: %ld\r\nContent-Type: text/html\r\n\r\n", strlen(response_message));  //404 error header
            write(client_fd, response_header, strlen(response_header)); //write to client_fd
            write(client_fd, response_message, strlen(response_message)); //write to client_fd
            return 1;  //return 1
            }
            else{
                char *p = url;  //*p = url

                if (*p == '/') {   // if url's starting point is '/'
                    p++;          // skip '/'
                } 

                 int fd = open(p, O_RDONLY); //open file
                     if (fd == -1) {  //error to open file
                    perror("open");
                    return 1; //return 1
                }
                 sprintf(response_header, "HTTP/1.1 200 OK\r\nContent-Type:text/plain \r\n\r\n"); //update header type: text/plain
                int nread; //store byte from fd
                while ((nread = read(fd, response_message, BUFSIZE)) > 0) { //read fd to response_message
                    response_message[nread] = '\0'; // add '\0'
                    sprintf(response_header, "%s%s", response_header, response_message); // add response_header to response_message
                }

                 write(client_fd, response_header, strlen(response_header)); //write it to client_fd
    
                close(fd); //close file

                 return 0;
            }
        }    
        
        sprintf(response_tmp, "<h3>Directory path: %s\n</h3>", store_path); //store direcotry path to response_tmp
        strcat(response_message, response_tmp);  //response_message + response_tmp
        while ((dir = readdir(dirp)) != NULL) 
        {                 // read dirp's directory until the end's file(or directory)
            dir_num += 1; // increase numbers of dir_num
            if (strlen(dir->d_name) > max)
            {
                max = strlen(dir->d_name); // modify max using strlen
            }
        }
        max = max + 1;                          // make free space
        char **arr;                             // array of 2D
        arr = malloc(sizeof(char *) * dir_num); // col is dir_num
        for (int j = 0; j < dir_num; j++)
        {
            arr[j] = malloc(sizeof(char) * max); // row is max
        }

        rewinddir(dirp);  // for reusing dirp
        int dir_num1 = 0; // for iterator
        while ((dir = readdir(dirp)) != NULL)
        {
            strcpy(arr[dir_num1], dir->d_name); // put dir->d_name in 2D array
            dir_num1++;                         // dir_num1 increases
        }
        dir_num1 -= 1;
        total = 0;
        sort_hidden(dir_num, arr, max); // sorting file include hidden file
        
        for (int j = 0; j < dir_num; j++)
        {
            total = total_blockSize(arr[j], store_path, total, local_check, prev); // save total blocksize
        }
        
           sprintf(response_tmp, "<h3>total  %d\n</h3>", total / 2);  //store total data to response_tmp
           strcat(response_message, response_tmp); //response_message + response_tmp

        sprintf(response_tmp, "<table border=\"1\">\n"); //store total data to response_tmp
        strcat(response_message, response_tmp);      //response_message + response_tmp
        sprintf(response_tmp, "<tr><th>Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th></tr>\n"); // set table head name
        strcat(response_message, response_tmp);  //response_message + response_tmp
        
            for (int i = 0; i < dir_num; i++)
            {
                option_l(arr[i], store_path, local_check, prev, response_message,url); // call option_l function
            }
        
        sprintf(response_tmp, "</table></body></html>"); // end table
        strcat(response_message, response_tmp); //response_message + response_tmp
        write(client_fd, response_header, strlen(response_header)); //write to client_fd header
        write(client_fd, response_message, strlen(response_message)); //write to client_fd message
        return 0;
        for (int k = 0; k < dir_num; k++)
        { // deallocate arr
            free(arr[k]);
        }
        free(arr);
        closedir(dirp); // closedir dirp
        }

//////////////////////////////////////////////////////////////////
// sort_hidden                                                   //
//=====================================================         //
// Input: int dir_num -> directory numvber for iterator          //
//        char* arr[]-> want to sort char*arr[]                  //
//        int max -> to allocate appropriate tmp for bubble sort //
// Output:   sorted file or directory                            //
// Purpose: sort file or directory using bubble sorting include  //
//          hidden file(start with '.')                          //
//////////////////////////////////////////////////////////////////
void sort_hidden(int dir_num, char *arr[], int max)
{

    char *tmp = malloc(sizeof(char) * max); // for using to change order of name
    for (int i = 0; i < dir_num - 1; i++)
    { // like bubble-sorting
        for (int j = i + 1; j < dir_num; j++)
        {
            char *big_arr1 = strdup(arr[i]); // copy arr[i]
            char *big_arr2 = strdup(arr[j]); // copy arr[j]
            for (int k = 0; k < strlen(big_arr1); k++)
            {
                big_arr1[k] = toupper(big_arr1[k]); // change letters to big letters
            }
            for (int k = 0; k < strlen(big_arr2); k++)
            {
                big_arr2[k] = toupper(big_arr2[k]); // change letters to captial
            }
            if (arr[i][0] == '.' && arr[j][0] == '.')
            {
                if (strcmp(big_arr1 + 1, big_arr2 + 1) > 0)
                {                        // strcasecmp(a+1,b+1): if result >0 -> b+1 is before a+1 in dicitionary
                    strcpy(tmp, arr[i]); // change order of arr[i]and arr[j] using tmp
                    strcpy(arr[i], arr[j]);
                    strcpy(arr[j], tmp);
                }
            }
            else if (arr[i][0] == '.' && arr[j][0] != '.')
            {
                if (strcmp(big_arr1 + 1, big_arr2) > 0)
                {                        // strcasecmp(a+1,b): if result >0 -> b is before a+1 in dicitionary
                    strcpy(tmp, arr[i]); // change order of arr[i]and arr[j] using tmp
                    strcpy(arr[i], arr[j]);
                    strcpy(arr[j], tmp);
                }
            }
            else if (arr[i][0] != '.' && arr[j][0] == '.')
            {
                if (strcmp(big_arr1, big_arr2 + 1) > 0)
                {                        // strcasecmp(a,b+1): if result >0 -> b+1 is before a in dicitionary
                    strcpy(tmp, arr[i]); // change order of arr[i]and arr[j] using tmp
                    strcpy(arr[i], arr[j]);
                    strcpy(arr[j], tmp);
                }
            }
            else
            {
                if (strcmp(big_arr1, big_arr2) > 0)
                {                        // strcasecmp(a,b): if result >0 -> b is before a in dicitionary
                    strcpy(tmp, arr[i]); // change order of arr[i]and arr[j] using tmp
                    strcpy(arr[i], arr[j]);
                    strcpy(arr[j], tmp);
                }
            }
            free(big_arr1); // deallocate big_arr1
            free(big_arr2); // deallocate big_arr2
        }
    }
    free(tmp); // deallocate tmp
}
////////////////////////////////////////////////////////////////////
// option_l                                                       //
//=====================================================           //
// Input: char* arr -> file or direcotry to find information      //
//        char* path -> absolute path                             //
//        int check -> whether hidden file care or don't care     //
//        char*prev_dir->back to prev path                        //
//        char*response_message->update response_message          //
//        char*url-> to make hyper link                           //
// Output: none-> error                                           //
//         print information of arr(file or directory)            //
// Purpose: search file(or directory)'s information using stat    //
////////////////////////////////////////////////////////////////////
void option_l(char *arr, char *path, int check, char *prev_dir, char*response_message, char*url)
{
    char perm[11] = "----------\0"; // initialize permission array
    struct stat buf;                // declare struct stat
    struct passwd *pwd;             // for getuid
    struct group *grp;              // for getgid
    char t_buf[80];                 // buffer of strftime()
    char response_tmp[BUFSIZE] ={0, };
    chdir(path); // change directory to path

    if (lstat(arr, &buf) < 0) // perror message
    {
        perror("stat");
        printf("%s: \n", arr);
        printf("errorofoption: %s\n", strerror(errno));
        chdir(prev_dir); // back to prev directory
        return;
    }

    if (S_ISDIR(buf.st_mode))
    {
        perm[0] = 'd';
    } // directory
    else if (S_ISLNK(buf.st_mode))
    {
        perm[0] = 'l';
    } // symbolic link

    // if file(or folder) has this permission -> update char perm[11]
    if (S_IRUSR & buf.st_mode) // if user can read
    {
        perm[1] = 'r';
    }
    if (S_IWUSR & buf.st_mode) // if user can write
    {
        perm[2] = 'w';
    }
    if (S_IXUSR & buf.st_mode) // if user can exe
    {
        perm[3] = 'x';
    }
    if (S_IRGRP & buf.st_mode) // if group can read
    {
        perm[4] = 'r';
    }
    if (S_IWGRP & buf.st_mode) // if group can write
    {
        perm[5] = 'w';
    }
    if (S_IXGRP & buf.st_mode) // if group can exe
    {
        perm[6] = 'x';
    }
    if (S_IROTH & buf.st_mode) // if other can read
    {
        perm[7] = 'r';
    }
    if (S_IWOTH & buf.st_mode) // if other can write
    {
        perm[8] = 'w';
    }
    if (S_IXOTH & buf.st_mode) // if other can exe
    {
        perm[9] = 'x';
    }
    if (check == 1)
    { // check == 1 -> no -a option so don't care hidden
        if (arr[0] == '.')
        {
            chdir(prev_dir); // back to prev directory
            return;
        }
    }

    pwd = getpwuid(buf.st_uid); // get st_uid to char
    if (pwd == NULL)
    { // error message
        perror("getpwuid");
        printf("Error: %s\n", strerror(errno));
        chdir(prev_dir); // back to prev directory
        return;
    }
    grp = getgrgid(buf.st_gid); // get group information using GID
    if (grp == NULL)
    { // error message
        perror("getgrgid");
        printf("Error: %s\n", strerror(errno));
        chdir(prev_dir); // back to prev directory
        return;
    }
    char size_str[20];
    char color_p[1024];
    strcpy(color_p, what_color(path, prev_dir, check, arr, color_p));
    strftime(t_buf, sizeof(t_buf), "%b %d %H:%M", localtime(&buf.st_mtime)); // format time using t_buf
    
        //printf("%s\t%lu\t%s\t%s\t%ld\t", perm, buf.st_nlink, pwd->pw_name, grp->gr_name, buf.st_size); // print permisiion, number of link, username, groupname, size using '\t'
        if (strcmp(arr, "..") == 0)  //if arr is ..
        {          
            char *q = url;  //*q is url
            if (q[strlen(q) - 1] == '/') {  //if last point is '/'
                q[strlen(q) - 1] = '\0';    //'\0' instead of '/' 
            }               
            char hyper[1024]={0, };                                                           // make path of hyper link to file 
            if(strcmp(url,"/")!=0){  //if url is not '/'

            strcpy(hyper,url);   //copy url to hyper
            strcat(hyper,"/");   //url+'/'
            }
            else{
            strcpy(hyper,url); //copy to url
            }
            strcat(hyper,arr); //hyper + arr
            sprintf(response_tmp, "<tr><td style =\"color:%s\"><a href =\"%s\">%s</a></td>", color_p, hyper, arr); // store in response_tmp
            strcat(response_message, response_tmp);   //response_message+ response_tmp     
        }
        else
        {
            char hyper[1024]={0, };                                  // make path of hyper link to file                                           
                 char *q = url;
            if (q[strlen(q) - 1] == '/') { //if last point is '/'
                q[strlen(q) - 1] = '\0'; //'\0' instead of '/' 
            }
            if(strcmp(url,"/")!=0){  //if url is not '/'
            strcpy(hyper,url);   //copy url to hyper
            strcat(hyper,"/");    //url+'/'
            }
            else{
            strcpy(hyper,url); //copy to url
            }
            strcat(hyper,arr);  //hyper + arr
            sprintf(response_tmp, "<tr><td style =\"color:%s\"><a href =\"%s\">%s</a></td>", color_p, hyper, arr); // store in response_tmp
            strcat(response_message, response_tmp); //response_message+ response_tmp
        }
        sprintf(response_tmp, "<td style =\"color:%s\">%s</td><td style =\"color:%s\">%lu</td><td style =\"color:%s\">%s</td>", color_p, perm, color_p, buf.st_nlink, color_p, pwd->pw_name); // store permisiion, number of link, username, groupname, size using '\t'
        strcat(response_message, response_tmp); //response_message+ response_tmp
        sprintf(response_tmp, "<td style =\"color:%s\">%s</td><td style =\"color:%s\">%ld</td>", color_p, grp->gr_name, color_p, buf.st_size);                                                // store permisiion, number of link, username, groupname, size using '\t'
        strcat(response_message, response_tmp); //response_message+ response_tmp
        sprintf(response_tmp, "<td style =\"color:%s\">%s</td></tr>", color_p, t_buf);
        strcat(response_message, response_tmp); //response_message+ response_tmp

    chdir(prev_dir);                // back to prev directory
}
//////////////////////////////////////////////////////////////////
// total_blockSize                                               //
//=====================================================         //
// Input: char* arr -> file or direcotry to find block size      //
//        char* path -> absolute path                            //
//        int check -> whether hidden file care or don't care    //
// Output: 0 -> error                                            //
//         total -> update or not changed total variable         //
// Purpose: counting block size                                  //
//////////////////////////////////////////////////////////////////
int total_blockSize(char *arr, char *path, int total, int check, char *prev_dir)
{

    chdir(path); // change directory
    struct stat buf;

    if (lstat(arr, &buf) < 0)
    { // perror message
        perror("stat");
        printf("%s: \n", arr);
        printf("error_total: %s\n", strerror(errno));
        return 0; // end
    }

    if (check == 1)
    { // check ==1 -> no -a option
        if (arr[0] == '.')
        {
            chdir(prev_dir); // back to prev directory
            return total;    // not add of hidden file's block size
        }
    }
    total += buf.st_blocks; // update total
    chdir(prev_dir);        // back to prev directory
    return total;           // return updated total
}
//////////////////////////////////////////////////////////////////
// what_color                                                    //
//=====================================================         //
// Input: int aflag -> checking if aflag is 1                    //
//        char* arr-> want to know arr's information             //
//        char *path-> change to path                            //
//        char *prev_dir-> back to prev_dir                      //
//        char *color-> return appropriate color information     //
// Output:   return -> color according to type                   //
// Purpose: to know what type arr is and what color the arr have //
//////////////////////////////////////////////////////////////////
char *what_color(char *path, char *prev, int aflag, char *arr, char *color)
{
    chdir(path); // change directory
    struct stat buf;

    if (lstat(arr, &buf) < 0) // lstat of arr
    {                         // perror message
        perror("stat");
        printf("%s: \n", arr);
        printf("error_color: %s\n", strerror(errno));
        chdir(prev); // change prev directory
        return color;
    }

    if (S_ISLNK(buf.st_mode))
    { // if link file
        color = "green";
    }
    else if (S_ISDIR(buf.st_mode))
    { // if directory
        color = "blue";
    }
    else
    { // the others
        color = "red";
    }

    chdir(prev);  // change prev directory
    return color; // return color
}