#include <linux/module.h>
#include <linux/highmem.h>
#include <linux/kallsyms.h>
#include <linux/syscalls.h>
#include <asm/syscall_wrapper.h>
#include <linux/syscalls.h>

#ifndef __NR_os_ftrace
#define __NR_os_ftrace 336 // allocate 336
#endif

void **syscall_table; 
void *real_os_ftrace; //store original system call(os_ftrace) address

__SYSCALL_DEFINEx(1, my_ftrace, pid_t,pid) {
    printk(KERN_INFO "os_ftrace() hooked! os_ftrace -> my_ftrace");
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
	if (!syscall_table) {
		printk(KERN_ERR "NO SYSTEM CALL TABLE\n");
		
	}

	syscall_table = (void**) kallsyms_lookup_name("sys_call_table"); //find system call table's address
	make_rw(syscall_table); //give authority 
	real_os_ftrace = syscall_table[__NR_os_ftrace]; //store original address
	syscall_table[__NR_os_ftrace] =(void *)__x64_sysmy_ftrace; //hooking

	return 0;
}

static void __exit os_ftracehooking_exit(void){
	syscall_table[__NR_os_ftrace] = real_os_ftrace; //reconnect to original os_ftrace's address
	make_ro(syscall_table); //steal authority
}

module_init(os_ftracehooking_init); //when module loads
module_exit(os_ftracehooking_exit); //when module unloads
MODULE_LICENSE("GPL");

