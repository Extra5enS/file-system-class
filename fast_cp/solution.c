#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <liburing.h>

#define N 4
#define COPY_SIZE (1L << 18)
#define min(x, y) ((x < y)?(x):(y))

typedef struct {
    struct iovec iov;
    char* buf;
    off_t offset;
    int read;
} data_set;

void data_set_init(data_set* ds, off_t buf_size, off_t fsize, int read) {
    ds -> buf = calloc(buf_size, 1);
    ds -> iov.iov_base = ds -> buf;
    ds -> iov.iov_len = buf_size;
    ds -> offset = 0;
    ds -> read = read;
}

void data_set_cpy(data_set* copy, data_set* origin) {
    memcpy(copy -> buf, origin -> buf, origin -> iov.iov_len);

    copy -> iov.iov_len = origin -> iov.iov_len;
    copy -> offset = origin -> offset;
}

void data_set_free(data_set* ds) {
    free(ds -> buf);
}

off_t file_size(int fd) {
    struct stat st;
    if(fstat(fd, &st) < 0) {
        return 0;
    }
    if(S_ISREG(st.st_mode)) {
        return st.st_size;
    } else if(S_ISBLK(st.st_mode)) { 
        // block has const size, so we can find it, using ioctl
        off_t size;
        ioctl(fd, BLKGETSIZE64, &size);
        return size;
    }
    return 0;
}

int main(int argc, char** argv) {
    if(argc != 3) {
        printf("Args error\n");
        return -1;
    }
    // argv[1] <- file that we are copying
    // argv[2] <- file to which we copy the first one

    int fd_out = 0;
    int fd_in = 0;
    if((fd_in = open(argv[1], O_RDONLY)) < 0) {
        perror("open first file error");
        return -1;
    }
    if((fd_out = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, 0644)) < 0) {
        perror("open second file error");
        return -1;
    }

    off_t fsize = 0;
    if((fsize = file_size(fd_in)) == 0) {
        perror("file_size error\n");
        return -1;
    }

    struct io_uring ring;
    if(io_uring_queue_init(2 * N, &ring, 0) < 0) {
        //printf("io_uring_queue_init error\n");
        perror("io_uring_queue_inti error");
        return -1;
    }
    data_set read_set[N];
    for(int i = 0; i < N; ++i) {
        data_set_init(&read_set[i], COPY_SIZE, fsize, 1); 
    }
    
    struct io_uring_cqe* cqe;
    struct io_uring_sqe* sqe;

    // TODO cpy (^_^)
    off_t offset = 0;
    off_t write_size = 0;

    // creat 4 read_messages
    for(int i = 0; i < N; ++i) {
        sqe = io_uring_get_sqe(&ring);
        io_uring_prep_readv(sqe, fd_in, &read_set[i].iov, 1, offset);
        read_set[i].offset = offset;
        offset += min(COPY_SIZE, fsize - offset);
        io_uring_sqe_set_data(sqe, &read_set[i]);
    } 
    
    // main part of copy
    while(write_size != fsize) {
        int ret = io_uring_wait_cqe(&ring, &cqe);
        if(ret < 0) {
            printf("io_uring_wait_cqe error\n");
        }
        data_set* winfo;
        winfo = io_uring_cqe_get_data(cqe);
        
        if(winfo->read) {
            data_set write_set;
            data_set_init(&write_set, COPY_SIZE, fsize, 0);
            data_set_cpy(&write_set, winfo);
            
            // new read
            sqe = io_uring_get_sqe(&ring);
            io_uring_prep_readv(sqe, fd_in, &winfo -> iov, 1, offset);
            winfo -> offset = offset;
            offset += min(COPY_SIZE, fsize - offset);
            io_uring_sqe_set_data(sqe, &winfo);
            
            // write
            io_uring_prep_writev(sqe, fd_out, &write_set.iov, 1, write_set.offset);
            io_uring_sqe_set_data(sqe, &write_set);

        } else {
            write_size += winfo -> offset;
            data_set_free(winfo);
            free(winfo);
        }
        io_uring_cqe_seen(&ring, cqe);
    }

    close(fd_out);
    close(fd_in);
    io_uring_queue_exit(&ring);
    return 0;
}
