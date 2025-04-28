#include "ftracehooking.h"
#ifndef __NR_os_ftrace
#define __NR_os_ftrace 336 // allocate 336
#endif

void **syscall_table; 
void *real_os_ftrace; //store original system call(os_ftrace) address

pid_t cur_pid; // pid variable
char pc_name[20]; // Process name
long read_total_byte = 0;
long write_total_byte = 0;

int open_num = 0;
int read_num = 0;
int write_num = 0;
int lseek_num = 0;
int close_num = 0;
char file_name[256];

//value export
EXPORT_SYMBOL(read_total_byte);
EXPORT_SYMBOL(write_total_byte);
EXPORT_SYMBOL(cur_pid);
EXPORT_SYMBOL(open_num);
EXPORT_SYMBOL(read_num);
EXPORT_SYMBOL(write_num);
EXPORT_SYMBOL(lseek_num);
EXPORT_SYMBOL(close_num);
EXPORT_SYMBOL(file_name);


static asmlinkage int my_ftrace(struct pt_regs *regs) {
    struct task_struct *task;
    pid_t pid = regs->di; // extract pid from pt_regs

    if (pid == 0) {
        task = current; // get task_struct of current proces 
        if (task) {
            strncpy(pc_name, task->comm, sizeof(pc_name) - 1);
            pc_name[sizeof(pc_name) - 1] = '\0'; // null 종료
        }
        printk(KERN_INFO "[2021202045] %s file[%s] stats [x] read - %ld / written - %ld\n", pc_name, file_name, read_total_byte, write_total_byte);
        printk(KERN_INFO "open[%d] close[%d] read[%d] write[%d] lseek[%d]\n", open_num, close_num, read_num, write_num, lseek_num);
        printk(KERN_INFO "OS Assignment2 ftrace [%d] End\n", cur_pid);
    } else {
        task = pid_task(find_vpid(pid), PIDTYPE_PID); // get task_struct from pid
        if (!task) {
            printk(KERN_ERR "Failed to find task for pid: %d\n", pid);
            return -1; // task error  
        }
        cur_pid = task->pid;
        printk(KERN_INFO "OS Assignment2 ftrace [%d] Start\n", cur_pid);
    }
    return 0;
}


void make_rw(void *addr) { //function that can give authority of reading and writing to the page which includes parameter
	unsigned int level;
	pte_t *pte = lookup_address((u64)addr, &level);
	 if (pte->pte &~ _PAGE_RW) {
        pte->pte |= _PAGE_RW;
    }
}

void make_ro(void *addr) { //roll back function from giving authority
	unsigned int level;
	pte_t *pte = lookup_address((u64)addr, &level);

	pte->pte = pte->pte &~ _PAGE_RW;
}

static int __init os_ftracehooking_init(void){ //call when moudle loads
	syscall_table = (void**) kallsyms_lookup_name("sys_call_table"); //find system call table's address
	if (!syscall_table) {
		printk(KERN_ERR "NO SYSTEM CALL TABLE\n");
		
	}

	make_rw(syscall_table); //give authority 
	real_os_ftrace = syscall_table[__NR_os_ftrace]; //store original address
	syscall_table[__NR_os_ftrace] = my_ftrace; //hooking

	return 0;
}

static void __exit os_ftracehooking_exit(void){
	syscall_table[__NR_os_ftrace] = real_os_ftrace; //reconnect to original os_ftrace's address
	make_ro(syscall_table); //steal authority
	//printk(KERN_INFO "ftracehooking module unloaded successfully\n");
}

module_init(os_ftracehooking_init); //when module loads
module_exit(os_ftracehooking_exit); //when module unloads
MODULE_LICENSE("GPL");

