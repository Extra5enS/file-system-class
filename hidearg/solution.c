#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define BUF_SIZE 100

void printcmdline() {
    int fd = open("/proc/self/cmdline", O_RDONLY); 
    
    char buf[BUF_SIZE];
    memset(buf, '\0', BUF_SIZE);
    
    int len = 0;
    int locklen = BUF_SIZE;
    while(locklen != 0) {
        locklen = read(fd, buf, BUF_SIZE);
        len += locklen;
        write(1, buf, locklen);
    }
    putchar('\n');
    close(fd);
}

int main(int argc, char** argv) {
    printcmdline();
    
    if(prctl(PR_SET_MM, PR_SET_MM_ARG_START, (unsigned long) argv[1], NULL, NULL) < 0) {
        if(errno == EPERM) {
            perror("Use must use sudo here!");
        } else {
            perror(strerror(errno));
        }
    }
    
    printcmdline();
    
    return 0;
}
