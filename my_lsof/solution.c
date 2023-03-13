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
#include <sys/sysmacros.h>

#include<pwd.h>

#define MAX_MEM_SIZE 256
#define MAX_PIDSTR_SIZE 16 

typedef struct {
    int pid; // (1)
    char* comm; // (2)
} pid_stat_t;


void pid_stat_read(int pid) {
    char pathname[MAX_MEM_SIZE];
    memset(pathname, '\0', MAX_MEM_SIZE);
    snprintf(pathname, sizeof(pathname), "/proc/%d/stat", pid);


    // Now we have stat = "/proc/[pid]/stat";
    // So we can read "stat" to print information;
    char strstat[MAX_MEM_SIZE];
    struct stat mystat;
    memset(strstat, '\0', MAX_MEM_SIZE);
    int fd = 0; 
    if((fd = open(pathname, O_RDONLY)) == -1) {
        perror("Open error");
        exit(-1); 
    }
    if(fstat(fd, &mystat) == -1) {
			perror("Stat error");
			exit(-1);
    }
    if(read(fd, strstat, MAX_MEM_SIZE) == -1) {
        perror("Read error");
        exit(-1);
    }
    close(fd);
    pid_stat_t pid_stat;
    
    char comm[MAX_MEM_SIZE];
    sscanf(strstat, "%d %[A-Z, a-z, ,(,.])", &pid_stat.pid, comm);
    pid_stat.comm = comm + 1;
    
    // Now we will use sscanf to select information
    struct passwd* user = getpwuid(mystat.st_uid);
    
    
    //Now we must read /proc/[pid]/fd to find fd
    memset(pathname, '\0', MAX_MEM_SIZE);
    snprintf(pathname, sizeof(pathname), "/proc/%d/fd/", pid);
    
    DIR* dir;
    if((dir = opendir(pathname)) == NULL) {
		perror("Open dir error");
		exit(-1);
	}

    struct dirent* dirbuf;
    for(;(dirbuf = readdir(dir)) != NULL;) {
        if(strcmp(dirbuf -> d_name, ".") == 0 || strcmp(dirbuf -> d_name, "..") == 0) {
            continue;
        }
        
        char fd_info[MAX_MEM_SIZE];
        memset(fd_info, '\0', MAX_MEM_SIZE);
        
        strcat(fd_info, pathname);
        strcat(fd_info, dirbuf -> d_name);
   
        struct stat linkstat;
        lstat(fd_info, &linkstat);
        
        char* readlinkbuf = calloc(linkstat.st_size, 1);
        readlink(fd_info, readlinkbuf, linkstat.st_size + 1);

        printf("%25s\t%4d %10s\t\t%s\n", 
                pid_stat.comm, pid_stat.pid, user -> pw_name, readlinkbuf);
        
        free(readlinkbuf);
    }
    closedir(dir);
}

int main() {
    DIR* dir;
    if((dir = opendir("/proc")) == NULL) {
		perror("Open dir error");
		exit(-1);
	}
    printf("%25s\t%4s %10s %s %30s %s %10s %10s %s\n",
           "COMMAND", "PID", "USER", //have
           "FD", "TYPE", "DEVICE", 
           "SIZE", "NODE", "NAME");
    struct dirent* dirbuf;

    for(;(dirbuf = readdir(dir)) != NULL;) {
       int pid = 0;
       sscanf(dirbuf -> d_name, "%d", &pid);
       if(pid == 0) {
           continue;
       }
       pid_stat_read(pid);
    }
    closedir(dir);
    return 0;
}
