#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "product.h"    

void lockRecord(int fd, int cond, int recordNumber) {
    struct flock fl;
    fl.l_type = F_WRLCK; 
    fl.l_whence = SEEK_SET;
    fl.l_start = cond * recordNumber * sizeof(product);
    fl.l_len = cond * sizeof(product);

    if (fcntl(fd, F_SETLKW, &fl) == -1) {
        perror("fcntl");
        exit(1);
    }
}

void unlockRecord(int fd,int cond, int recordNumber) {
    struct flock fl;
    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = cond * recordNumber * sizeof(product);
    fl.l_len = cond * sizeof(product);

    if (fcntl(fd, F_SETLK, &fl) == -1) {
        perror("fcntl");
        exit(1);
    }
}

int main() {
    int fd = open("DB/productTable", O_RDWR | O_CREAT, 0666);
    char prodId[9] = "00000003\0";

    lockRecord(fd, 1, atoi(prodId));
    printf("hi\n");
    getchar();
    unlockRecord(fd, 1, atoi(prodId));

    close(fd);

    return 0;
}
