#include "user.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(){
    user u;


    int fd = open("database", O_RDONLY);

    read(fd, &u, sizeof(user));

    printf("%s\n", u.userName);
    printf("%s\n", u.userId);
    printf("%s\n", u.password);
    printf("%i\n", u.isAdmin);
    printf("%i\n", u.noOfOrders);

    close(fd);

}