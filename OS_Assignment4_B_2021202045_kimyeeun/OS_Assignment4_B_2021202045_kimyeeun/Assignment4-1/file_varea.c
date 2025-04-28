#include <linux/module.h>
#include <linux/highmem.h>
#include <linux/kallsyms.h>
#include <linux/syscalls.h>
#include <asm/syscall_wrapper.h>
#include <linux/mm.h>
#include <linux/sched.h>

#ifndef __NR_os_ftrace
#define __NR_os_ftrace 336 
#endif

void **syscall_table; 
void *real_os_ftrace; // Store original system call (os_ftrace) address

asmlinkage pid_t file_varea_call(pid_t trace_task) { // Function to trace, parameter is from my_trace
    //printk(KERN_INFO "trace_task pid = %d\n", trace_task);
    struct task_struct *findtask = &init_task; // first task is 0
    int isExist = 0;
    struct vm_area_struct *vma;
    struct mm_struct *find_mm;
    char *path;
    char p_buf[256];
    do {
        if (findtask->pid == trace_task) { 
            isExist = 1; // found
            // start to trace
            printk(KERN_INFO "######## Loaded files of a process '%s(%d)' in VM ########\n", findtask->comm, findtask->pid);
            find_mm = findtask->mm; //access to vm throgh this
            vma = find_mm->mmap;
            while (vma) {
                if (vma->vm_file) { //for avoid printing annoymous mapping when printing f_path
                    printk(KERN_INFO "mem(%lx~%lx)", vma->vm_start, vma->vm_end); //vm_area's start and end
                    printk(KERN_CONT " code(%lx~%lx)", find_mm->start_code, find_mm->end_code); //code area
                    printk(KERN_CONT " data(%lx~%lx)", find_mm->start_data, find_mm->end_data); //data area
                    printk(KERN_CONT " heap(%lx~%lx)", find_mm->start_brk, find_mm->brk); //heap area

                    path = d_path(&vma->vm_file->f_path, p_buf, sizeof(p_buf));
                    if (!IS_ERR(path)) {
                        printk(KERN_CONT " %s\n", path);
                    } else {
                        printk(KERN_CONT "----anonymous mapping----\n");
                        return -1;
                    }
                }
                vma = vma->vm_next; // move to next vm, if not exist next vm, it means this is the last node
            }

            break;
        }
        findtask = next_task(findtask); // circulate
    } while (findtask != &init_task);

    if (!isExist) { // if no process found
        printk(KERN_INFO "##### NO PROCESS TRACE #####\n");
        return -1;
    }

    printk(KERN_INFO "###########################################################\n");
    return trace_task; // return input pid
}

__SYSCALL_DEFINEx(1, file_varea, pid_t, pid) { //the name of hooking function is 'file_varea'
    return file_varea_call(pid); // move to file_vare_call
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
    } 
    //else {
        //printk(KERN_INFO "sys_call_table found at address: %px\n", syscall_table);
    //}

    make_rw(syscall_table); // Grant write permission to syscall table
    real_os_ftrace = syscall_table[__NR_os_ftrace]; // Store original address
    syscall_table[__NR_os_ftrace] = (void *)__x64_sysfile_varea; // Hook the syscall

    return 0;
}

static void __exit os_ftracehooking_exit(void) { // Called when module unloads
    syscall_table[__NR_os_ftrace] = real_os_ftrace; // Restore original os_ftrace's address
    make_ro(syscall_table); // Revert permission
}

module_init(os_ftracehooking_init); // Module loading function
module_exit(os_ftracehooking_exit); // Module unloading function
MODULE_LICENSE("GPL");

