#include "user.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void randomString(char str[]){
    srand(time(0));
    char option[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for(int i = 0; i < 8; i++)
        str[i] = option[rand() % 62];
}

int main(){
    user u;

    u.isAdmin = 1;
    u.noOfOrders = 0;
    strcpy(u.userName, "admin");
    strcpy(u.password, "password");
    char userId[8];
    randomString(userId);
    strcpy(u.userId, userId);

    int fd = open("DB/userTable", O_WRONLY );
    if(fd==-1) perror(" ");
    write(fd, &u, sizeof(user));
    close(fd);
}