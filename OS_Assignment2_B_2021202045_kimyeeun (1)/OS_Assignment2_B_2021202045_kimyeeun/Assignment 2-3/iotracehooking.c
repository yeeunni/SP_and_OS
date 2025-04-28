#include "ftracehooking.h"

void **syscall_table; 

extern pid_t cur_pid;
extern long read_total_byte;
extern long write_total_byte;
extern int open_num;
extern int write_num;
extern int lseek_num;
extern int read_num;
extern int close_num;

extern char file_name[256];

//point to original function
asmlinkage long (*original_openat)(const struct pt_regs *);
asmlinkage ssize_t (*original_read)(unsigned int, char __user *, size_t);
asmlinkage off_t (*original_lseek)(unsigned int, off_t, unsigned int);
asmlinkage long (*original_close)(unsigned int);
asmlinkage ssize_t (*original_write)(unsigned int, const char __user *, size_t);

//hooking openat function
asmlinkage long ftrace_openat(const struct pt_regs *regs) {
    long fd = original_openat(regs);  // call original openat
    
    if (current->pid == cur_pid) {  // check pid which is traced
	open_num++;  //increase open number
	const char __user *user_filename;  //original filename
	user_filename = (const char __user *)regs->si; 
        if (user_filename) {
            if (copy_from_user(file_name, user_filename, sizeof(file_name)-1)==0) {  //copy filename
               file_name[sizeof(file_name) - 1] = '\0';  // null termiantion
            } 
        }
    }

    return fd;  //return original openat function's return value
}
    
 //hooking read function   
asmlinkage ssize_t ftrace_read(unsigned int fd, char __user *buf, size_t count) {
    ssize_t read_count = original_read(fd, buf, count);
    if (current->pid == cur_pid) {
        read_num++;
        if (read_count > 0) {
        	read_total_byte = read_total_byte + read_count; //read count
        }
    }
    return read_count; //return original read function's return value
}

//hooking write function
asmlinkage ssize_t ftrace_write(unsigned int fd, const char __user *buf, size_t count) {
    ssize_t write_count = original_write(fd, buf, count);
    if (current->pid == cur_pid) {
    
    /*printk(KERN_INFO "ftrace_write called: fd=%d, count=%zu, write_num=%d\n", fd, write_count, write_num);*/

        write_num++;    //increase write_num
        if (write_count > 0) {
        	write_total_byte = write_total_byte + write_count; //written count
        }
    }
    return write_count; //return original write function's return value
}

//hooking lseek function
asmlinkage off_t ftrace_lseek(unsigned int fd, off_t offset, unsigned int whence){
	off_t new_offset = original_lseek(fd,offset,whence); //call original lseek function
	if(current->pid == cur_pid){
	lseek_num++;
	}
    return new_offset; //return original offset function's return value
}

//hooking close function
asmlinkage long ftrace_close(unsigned int fd){
     long result =  original_close(fd);  //call original close function
     if(current->pid == cur_pid){
       if(result==0){  //complete close function
       close_num++;  
       }
     }
    return result; //return original close function's return value
}

static int __init iotracehooking_init(void) {
    syscall_table = (void**)kallsyms_lookup_name("sys_call_table");
    if (!syscall_table) {
        printk(KERN_ERR "COULD NOT FIND SYS_CALL_TABLE ADDRESS\n");
        return -1; // error handling
    }
    
    //wrapping original function -> ftrace_openat/close/read/write/lseek
    original_openat = syscall_table[__NR_openat];
    syscall_table[__NR_openat] = ftrace_openat;
    original_read = syscall_table[__NR_read];
    syscall_table[__NR_read] = ftrace_read;
    original_write = syscall_table[__NR_write];
    syscall_table[__NR_write] = ftrace_write;
    original_lseek = syscall_table[__NR_lseek];
    syscall_table[__NR_lseek] = ftrace_lseek;
    original_close = syscall_table[__NR_close];
    syscall_table[__NR_close] = ftrace_close;
    return 0;
}

static void __exit iotracehooking_exit(void) {
    //return syscall table to original function
    syscall_table[__NR_openat] = original_openat;
    syscall_table[__NR_read] = original_read;
    syscall_table[__NR_write] = original_write;
    syscall_table[__NR_lseek] = original_lseek;
    syscall_table[__NR_close] = original_close;
    //printk(KERN_INFO "ftrace unhooked successfully\n");
    
}

module_init(iotracehooking_init);//load module
module_exit(iotracehooking_exit); //unload module
MODULE_LICENSE("GPL");

