#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "data.h"

int main(){
    user u;

    u.isAdmin = 1;
    strcpy(u.userName, "admin");
    strcpy(u.password, "password");
    strcpy(u.userId, "00000000");    

    int fd = open("DB/userTable", O_WRONLY );
    if(fd==-1) perror(" ");
    write(fd, &u, sizeof(user));
    close(fd);
}