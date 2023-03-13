#define FUSE_USE_VERSION 31

#include <fuse.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

struct {
    char* name;
    char* content;
} hello_file_info;

void* init_fs(struct fuse_conn_info* fconn) {
    //fconf -> kernel_cache = 1;
    return NULL;
}

int get_attr(const char* name, struct stat* st) {
    memset(st, 0, sizeof(struct stat));
    // we will have "/" and "/hello" calls only
    int error_ret = 0;
    if(strcmp(name + 1, hello_file_info.name) == 0) {
        st -> st_size = strlen(hello_file_info.content); 
        st -> st_mode = S_IFREG | 0444;
        st -> st_nlink = 1;
    } else if(strcmp(name, "/") == 0) {
        st -> st_mode = S_IFDIR | 0755;
        st -> st_nlink = 2; 
    } else { // error case
        error_ret = -1;
    }
    return error_ret;
}

int open_file(const char* name, struct fuse_file_info* ffinfo) {
    if(strcmp(name + 1, hello_file_info.name) != 0) {
        return -1; 
    }
    return 0;
}

int read_file(const char* name, char* buf, size_t size, off_t offset, 
        struct fuse_file_info* ffinfo) {
    if(strcmp(name + 1, hello_file_info.name) != 0) {
        return -1; 
    }
    size_t len = strlen(hello_file_info.content);
    if(offset < len) {
        if(offset + size > len) {
            size = len - offset;
        }
        memcpy(buf, hello_file_info.content + offset, size);
    } else {
        size = 0;
    }
    return size;
}

int read_dir(const char* name, void* buf, fuse_fill_dir_t filler, off_t offset, 
        struct fuse_file_info* ffinfo) {
    if(strcmp(name, "/") != 0) {
        return -1;
    }

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, hello_file_info.name, NULL, 0);

    return 0;
}



int main(int argc, char** argv) {
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
    
    char* name = calloc(strlen("hello") + 1, 1);
    strcat(name, "hello");
    hello_file_info.name = name;
    
    char* content = calloc(strlen("hello world! And I'm so sorry for this ('-_-)\n") + 1, 1);
    strcat(content, "hello world! And I'm so sorry for this ('-_-)\n");
    hello_file_info.content = content;

    if(fuse_opt_parse(&args, &hello_file_info, NULL, NULL) == -1) {
        perror("fuse_opt_parse error!");
        exit(-1);
    }

    const struct fuse_operations hello_fs_oper = {
        .init = init_fs,
        .getattr = get_attr,
        .readdir = read_dir,
        .open = open_file, 
        .read = read_file,
    };
    
    int res = fuse_main(args.argc, args.argv, &hello_fs_oper, NULL);
    fuse_opt_free_args(&args);
    free(name);
    free(content);
    return res;
}

