#include <linux/module.h>
#include <linux/highmem.h>
#include <linux/kallsyms.h>
#include <linux/syscalls.h>
#include <asm/syscall_wrapper.h>
#include <linux/sched.h>

#ifndef __NR_os_ftrace
#define __NR_os_ftrace 336 
#endif



void **syscall_table; 
void *real_os_ftrace; // Store original system call (os_ftrace) address


asmlinkage pid_t process_tracer(pid_t trace_task) { //function to trace and parameter is from my_trace
    //printk(KERN_INFO "trace_task = %d",trace_task);
    struct task_struct *findtask = &init_task; //first task is 0
    int isExist = 0;
    int sibling_count=0;
    int child_count=0;
    struct list_head *head;
    
    do {
        if (findtask->pid == trace_task) { 
            isExist = 1; //found
            //start to trace
            printk(KERN_INFO "##### TASK INFORMATION of ''[%d] %s'' #####\n", findtask->pid, findtask->comm);
            
            // about find task state
            switch (findtask->state) {
                case TASK_RUNNING:
                    printk(KERN_INFO "- task state : Running or ready\n");
                    break;
                case TASK_INTERRUPTIBLE:
                    printk(KERN_INFO "- task state : Wait\n");
                    break;
                case TASK_UNINTERRUPTIBLE:
                    printk(KERN_INFO "- task state : Wait with ignoring all signals\n");
                    break;
                case TASK_STOPPED:
                    printk(KERN_INFO "- task state : Stopped\n");
                    break;
                case EXIT_ZOMBIE:
                    printk(KERN_INFO "- task state : Zombie process\n");
                    break;
                case EXIT_DEAD:
                    printk(KERN_INFO "- task state : Dead\n");
                    break;
                default:
                    printk(KERN_INFO "- task state: etc.\n");
            }
            //information about group
            printk(KERN_INFO "- Process Group Leader : [%d] %s\n", findtask->group_leader->pid, findtask->group_leader->comm);
            //total context switches
            printk(KERN_INFO "- Number of context switches : %lu\n", findtask->nvcsw + findtask->nivcsw); 
            //total fork() count
            printk(KERN_INFO "- Number of calling fork() : %d\n", findtask->fork_count);
            
            // parent process
            if (findtask->parent)
            {
                printk(KERN_INFO "- its parent process : [%d] %s\n", findtask->parent->pid, findtask->parent->comm);
            }

            // Sibling processes
            printk(KERN_INFO "- its sibling process(es) :\n");
            list_for_each(head, &findtask->sibling) //loop(circulate) by head's next until the end and point to first sibling of findtask
            {
                struct task_struct *sibling; //store sibling process information
                
                sibling = list_entry(head, struct task_struct, sibling); //access task_struct by head pointer
                
                if (sibling->pid != findtask->parent->pid) //except parent
                { 
                    printk(KERN_INFO "  > [%d] %s\n", sibling->pid, sibling->comm);
                    sibling_count++;
                }
            }
	    if (sibling_count > 0)
	   {
	    printk(KERN_INFO "  > This process has %d sibling process(es)\n", sibling_count);
	   }
	   else{
             printk(KERN_INFO "  > It has no sibling.\n");
           }
           //child process
           printk(KERN_INFO "- its child process(es) :\n");
           list_for_each(head, &findtask->children)
           {
                struct task_struct *child;
                child = list_entry(head, struct task_struct, sibling);
                printk(KERN_INFO "  > [%d] %s\n", child->pid, child->comm);
                child_count++;
           }
           
           if (child_count > 0)
	   {
	   printk(KERN_INFO "  > This process has %d child process(es)\n", child_count);
	   }
	   else{
             printk(KERN_INFO "  > It has no child.\n");
           }
           break;
        }
        findtask = next_task(findtask); 
    } while (findtask != &init_task);

    if (!isExist) { //if no process
        printk(KERN_INFO "##### NO PROCESS TRACE #####\n");
        return -1;
    }

    printk(KERN_INFO "##### END OF INFORMATION #####\n");
    return trace_task; //return input pid
}

__SYSCALL_DEFINEx(1, my_ftrace, pid_t, pid) {
    return process_tracer(pid);
}

// Function to change memory protection to read-write
void make_rw(void *addr) {
    unsigned int level;
    pte_t *pte = lookup_address((u64)addr, &level);
    if (pte->pte & ~_PAGE_RW) {
        pte->pte |= _PAGE_RW;
    }
}

// Function to revert memory protection to read-only
void make_ro(void *addr) {
    unsigned int level;
    pte_t *pte = lookup_address((u64)addr, &level);
    pte->pte = pte->pte & ~_PAGE_RW;
}

static int __init os_ftracehooking_init(void) { // Called when module loads
    syscall_table = (void**) kallsyms_lookup_name("sys_call_table"); // Find system call table's address
    
    if (!syscall_table) {
        printk(KERN_ERR "NO SYSTEM CALL TABLE\n");
        return -1; // Return error if syscall_table is not found
    } else {
        printk(KERN_INFO "sys_call_table found at address: %px\n", syscall_table);
    }

    make_rw(syscall_table); // Grant write permission to syscall table
    real_os_ftrace = syscall_table[__NR_os_ftrace]; // Store original address
    syscall_table[__NR_os_ftrace] = (void *)__x64_sysmy_ftrace; // Hook the syscall

    return 0;
}

static void __exit os_ftracehooking_exit(void) { // Called when module unloads
    syscall_table[__NR_os_ftrace] = real_os_ftrace; // Restore original os_ftrace's address
    make_ro(syscall_table); // Revert permission
}

module_init(os_ftracehooking_init); // Module loading function
module_exit(os_ftracehooking_exit); // Module unloading function
MODULE_LICENSE("GPL");

