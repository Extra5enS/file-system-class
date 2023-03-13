#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#include<unistd.h>
#include<dirent.h>
#include<getopt.h>
#include<fcntl.h>
#include<time.h>

#include<sys/types.h>
#include<sys/stat.h>
#include<sys/errno.h>
#include<sys/time.h>

#include<pwd.h>


#define MAX_MEM_SIZE 256
#define MAX_PIDSTR_SIZE 16 

typedef struct {
    int pid; // (1)
    char* comm; // (2)
    char stat; // (3)
    unsigned long long starttime; // (22)
    unsigned long vsize; // (23)
    unsigned long rss; // (24)
    // it's need to add %CUP, %MEM, TTY
} pid_stat_t;

size_t skip_fields_scanf(char* line, size_t num) {
    char buf[MAX_MEM_SIZE];
    size_t skiped = 0;
    for(size_t i = 0; i < num; ++i) {
        skiped += sscanf(line + skiped, "%[1-9,0] ", buf);
    }
    return skiped;
}

void pid_stat_read(int pid, struct timeval boot_time) {
    char pathname[MAX_MEM_SIZE];
    memset(pathname, '\0', MAX_MEM_SIZE);
    snprintf(pathname, sizeof(pathname), "/proc/%d/stat", pid);
    // Now we have pathname = "/proc/[pid]/stat";
    // So we can read "stat" to print information;
    char strstat[MAX_MEM_SIZE];
    memset(strstat, '\0', MAX_MEM_SIZE);
    int fd = 0; 
    if((fd = open(pathname, O_RDONLY)) == -1) {
        perror("Open error");
        exit(-1); 
    }
    struct stat mystat;
    if(fstat(fd, &mystat) == -1) { // for user!
		perror("Stat error");
        exit(-1);
    }
    if(read(fd, strstat, MAX_MEM_SIZE) == -1) {
        perror("Read error");
        exit(-1);
    }
    close(fd);

    // Now we will use sscanf to select information
    pid_stat_t pid_stat;
    struct passwd* user;
    char comm[MAX_MEM_SIZE];
    
    sscanf(strstat, "%d %[A-Z, a-z, ,(,.]) %c %*s%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s %llu %lu %lu", 
            &pid_stat.pid, comm, &pid_stat.stat, 
            &pid_stat.starttime, &pid_stat.vsize, &pid_stat.rss);
    pid_stat.comm = comm + 1; 

    boot_time.tv_sec += pid_stat.starttime / sysconf(_SC_CLK_TCK);
    char buftime[20];
    strftime(buftime, 20, "%H:%M", localtime(&boot_time.tv_sec));
    user = getpwuid(mystat.st_uid);
    printf("%8s %10d %4c %6lu %7lu %10s %s\n", 
            user -> pw_name,     
            pid_stat.pid, 
            pid_stat.stat,
            pid_stat.vsize / 1024,
            pid_stat.rss,
            buftime,
            pid_stat.comm);
}

int main() {
    DIR* dir;
    if((dir = opendir("/proc")) == NULL) {
        perror("Open dir error");
        exit(-1);
    }
    printf("%8s %10s %4s %10s %6s %7s %s\n", "USER", "PID", "STAT", "VSIZE", "RSS", "START", "COMM");
    struct dirent* dirbuf;

    struct timespec boot_timespec;
    clock_gettime(CLOCK_BOOTTIME, &boot_timespec);
    struct timeval time;
    gettimeofday(&time, NULL);

    struct timeval boot_time;
    boot_time.tv_sec = time.tv_sec - boot_timespec.tv_sec;

    for(;(dirbuf = readdir(dir)) != NULL;) {
        int pid = 0;
        sscanf(dirbuf -> d_name, "%d", &pid);
        if(pid == 0) {
            continue;
        }
        pid_stat_read(pid, boot_time);
    }
    closedir(dir);
    return 0;
}
