#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

int main() {
    pid_t pid = getpid();  
    int result;

    result = syscall(336, pid);  
    if (result == 0) {
        printf("sys_os_ftrace success! PID: %d\n", pid);
    } else {
        perror("sys_os_ftrace failed");
    }

    return 0;
}
