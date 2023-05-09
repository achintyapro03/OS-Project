#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ctype.h>
#include "user.h"
#include "product.h"
#include "cart.h"

void spilt(char str[], char params[5][20])
{
    char temp[100];
    strcpy(temp, str);
    strcat(temp, " ");
    int count = 0;
    int idx = 0;
    for(int i = 0; i < strlen(str); i++){
        if(str[i] == ' '){
            count++;
            idx = 0;
        }
        else{
            params[count][idx] = str[i];
            idx++;
        }
    }

    // for(int i =0; i<5; i++){
    //     printf("%s rob\n", params[i]);
    // }
}

char *itoa(int val, int base, char str[])
{

    int i = 30;

    for (; val && i; --i, val /= base)

        str[i] = "0123456789abcdef"[val % base];
}

void strLower(char str[])
{
    for (int i = 0; i < strlen(str); i++)
        str[i] = tolower(str[i]);
}

void randomString(char str[])
{
    srand(time(0));
    char option[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (int i = 0; i < 8; i++)
        str[i] = option[rand() % 62];
    str[8] = '\0';
}

int checkInDB(char key[], char DBname[]){
    int fd = open(DBname, O_RDONLY);

    if(strcmp(DBname, "DB/userTable")==0){
        user temp;
        while (read(fd, &temp, sizeof(user)))
            if (strcmp(temp.userName, key) == 0){
                close(fd);
                return 1;
            }
    }
    else if(strcmp(DBname, "DB/productTable")==0){
        product temp;
        while (read(fd, &temp, sizeof(product)))
            if (strcmp(temp.prodId, key) == 0){
                close(fd);
                return 1;
            }
    }
    // else if(strcmp(DBname, "cartDB")==0){
    //     cart temp;
    //     while (read(fd, &temp, sizeof(cart)))
    //         if (strcmp(temp.prodId, key) == 0)
    //             return 1;
    // }
    close(fd);
    return 0;
}

int login(char username[], char password[], char res[])
{
    printf("in login\n");
    int fd = open("DB/userTable", O_RDONLY);
    lseek(fd, 0, SEEK_SET);
    user temp;
    // int flag1 = 1;
    while (read(fd, &temp, sizeof(user)))
    {
        printf("in while\n");
        if (strcmp(temp.userName, username) == 0)
        {
            if (strcmp(temp.password, password) == 0)
            {
                strcpy(res, temp.userId);
                (temp.isAdmin) ? strcat(res, " 1") : strcat(res, " 0");
                close(fd);
                return 1;
            }
        }
    }
    printf("WASTED\n");
    strcpy(res, "-1");
    close(fd);
    return 0;
}

int reg(char username[], char password[], char res[])
{
    int fd = open("DB/userTable", O_RDWR);
    lseek(fd, 0, SEEK_SET);
    user temp;
    // int flag1 = 1;
    while (read(fd, &temp, sizeof(user)))
    {
        if (strcmp(temp.userName, username) == 0)
        {
            strcpy(res, "-1");
            return 0;
        }
    }
    char id[9];
    randomString(id);
    strcpy(res, id);
    strcat(res, " 0");
    user u;
    u.isAdmin = 0;
    u.noOfOrders = 0;
    strcpy(u.password, password);
    strcpy(u.userName, username);
    strcpy(u.userId, id);

    write(fd, &u, sizeof(user));

    return 1;
}

int viewAllProducts(char res[])
{
    strcpy(res, "Achi MART All Products!!\n\n");
    strcat(res, "\nProductId\tName\t\tQuantity\tPrice\n\n");

    int fd = open("DB/productTable", O_RDONLY);
    product temp;
    // printf("no smash 1\n");
    while (read(fd, &temp, sizeof(product)))
    {
        if (temp.quantity == 0)
            continue;
        // printf("no smash 2\n");
        strcat(res, temp.prodId);
        strcat(res, "\t");
        strcat(res, temp.name);
        strcat(res, "\t\t");
        // printf("no smash 3\n");
        char qty[9];
        sprintf(qty, "%d", temp.quantity);
        strcat(res, qty);
        strcat(res, "\t\t");
        char price[8];
        gcvt(temp.price, 8, price);
        // printf("no smash 4\n");
        strcat(res, price);
        strcat(res, "\n");
    }
    // printf("no smash 5\n");
    close(fd);
    // printf("%s\n", res);
    return 1;
}

int viewOneProduct(char prodId[], char res[])
{
    strcpy(res, "Achi MART!!\n\n");
    strcat(res, "\nProductId\tName\tQuantity\tPrice\n\n");

    int fd = open("DB/productTable", O_RDONLY);
    product temp;
    while (read(fd, &temp, sizeof(product)))
    {
        if (strcmp(temp.prodId, prodId) == 0)
        {
            strcat(res, temp.prodId);
            strcat(res, "\t");
            strcat(res, temp.name);
            strcat(res, "\t");
            char qty[8];
            sprintf(qty, "%d", temp.quantity);
            strcat(res, qty);
            strcat(res, "\t\t");
            char price[8];
            gcvt(temp.price, 8, price);
            strcat(res, price);
            strcat(res, "\n");
            return 1;
        }
    }
    close(fd);
    strcpy(res, "Opps!! Product not found!\n");
}

int addProduct(char prodName[], char qty[], char price[], char res[])
{
    int fd = open("DB/productTable", O_RDWR);
    lseek(fd, 0, SEEK_SET);
    product temp;
    // int flag1 = 1;
    while (read(fd, &temp, sizeof(product)))
    {
        if (strcmp(temp.name, prodName) == 0)
        {
            temp.quantity = atoi(qty) + temp.quantity;
            temp.price = atof(price);
            lseek(fd, -1 * sizeof(product), SEEK_CUR);
            write(fd, &temp, sizeof(product));
            strcpy(res, "Product Added");
            close(fd);
            return 1;
        }
    }
    char id[9];
    randomString(id);

    product p1;
    strcpy(p1.name, prodName);
    p1.price = atof(price);
    strcpy(p1.prodId, id);
    p1.quantity = atoi(qty);
    strcpy(res, "Product Added");
    write(fd, &p1, sizeof(product));
    close(fd);
    return 1;
}

int deleteProduct(char prodId[], char res[])
{
    int fd = open("DB/productTable", O_RDWR);
    lseek(fd, 0, SEEK_SET);
    product temp;

    while (read(fd, &temp, sizeof(product)))
    {
        if (strcmp(temp.prodId, prodId) == 0)
        {
            temp.quantity = 0;
            temp.price = 0;
            lseek(fd, -1 * sizeof(product), SEEK_CUR);
            write(fd, &temp, sizeof(product));
            strcpy(res, "Product Deleted");
            close(fd);
            return 1;
        }
    }

    strcpy(res, "Product Not found");
    close(fd);

    return 0;
}

int updateProduct(char prodId[], char qty[], char price[], char res[])
{
    int fd = open("DB/productTable", O_RDWR);
    lseek(fd, 0, SEEK_SET);
    product temp;

    while (read(fd, &temp, sizeof(product)))
    {
        if (strcmp(temp.prodId, prodId) == 0)
        {
            temp.quantity = atoi(qty);
            printf("%i\n", temp.quantity);
            temp.price = atof(price);
            lseek(fd, -1 * sizeof(product), SEEK_CUR);
            write(fd, &temp, sizeof(product));
            strcpy(res, "Product Updated");
            close(fd);
            return 1;
        }
    }

    strcpy(res, "Product Not found");
    close(fd);

    return 0;
}

int addToCart(char userId[], char prodId[], char qty[], char res[])
{
    // printf("1%s\n", userId);
    // printf("2%s\n", prodId);
    // printf("3%s\n", qty);
    if(!checkInDB(prodId, "DB/productTable")){
        strcpy(res, "Invalid Product ID");
        return 0;
    }
    // printf("%s\n", userId);
    // printf("%s\n", prodId);
    // printf("%s\n", qty);

    int fd1 = open("DB/cartTable", O_RDWR);
    lseek(fd1, 0, SEEK_SET);

    cart temp1;

    while (read(fd1, &temp1, sizeof(cart)))
    {
        if (strcmp(temp1.prodId, prodId) == 0)
        {
            if (strcmp(temp1.userId, userId) == 0)
            {
                temp1.quantity = temp1.quantity + atoi(qty);

                int fd2 = open("DB/productTable", O_RDWR);
                product temp2;
                while(read(fd2, &temp2, sizeof(product))){
                    if(strcmp(temp2.prodId, prodId) == 0){
                        int q = temp2.quantity - atoi(qty);
                        if(q < 0){
                            strcpy(res, "Insufficient quantity!");
                            close(fd1);
                            close(fd2);
                            return 0;
                        }
                        else{
                            temp2.quantity = atoi(q);
                            lseek(fd2, -1 * sizeof(product), SEEK_CUR);
                            write(fd2, &temp2, sizeof(product));
                        }
                    }
                }
                close(fd2);

                lseek(fd1, -1 * sizeof(cart), SEEK_CUR);
                write(fd1, &temp1, sizeof(cart));
                strcpy(res, "Product added to cart");
                close(fd1);
                return 1;
            }
        }
    }

    // printf("4%s\n", userId);
    // printf("5%s\n", prodId);
    // printf("6%s\n", qty);
    cart c;
    strcpy(c.userId, userId);
    strcpy(c.prodId, prodId);
    // cart c = {userId, prodId,atoi(qty)};
    c.quantity = atoi(qty);
    strcpy(res, "Product added to cart");
    write(fd1, &c, sizeof(cart));
    close(fd1);

    // printf("%s\n", c.userId);
    // printf("%s\n", c.prodId);
    // printf("%i\n", c.quantity);

    return 1;
}

int viewCart(char userID[], char res[])
{

    int fd1 = open("DB/cartTable", O_RDWR);
    lseek(fd1, 0, SEEK_SET);
    strcpy(res, "\nProducs in cart\n");
    strcat(res, "Prod Id\t\tProd Name\tQuantity\n");
    cart temp1;

    while (read(fd1, &temp1, sizeof(cart)))
    {
        printf("found\n");
        printf("a%s\n", temp1.userId);
        // printf("a%s\n", userID);
        printf("b%s\n", temp1.prodId);
        printf("c%d\n", temp1.quantity);
        if (strcmp(temp1.userId, userID) == 0)
        {
            printf("located\n");
            // strcpy(res, "Product added to cart");
            strcat(res, temp1.prodId);
            strcat(res, "\t");
            int fd2 = open("DB/productTable", O_RDWR);
            product temp2;
            while(read(fd2, &temp2, sizeof(product))){
                if(strcmp(temp1.prodId, temp2.prodId) == 0){
                    strcat(res, temp2.name);
                    strcat(res, "\t\t");
                    close(fd2);
                    break;
                }
            }
            char qty[8];
            sprintf(qty, "%d", temp1.quantity);
            strcat(res, qty);
            strcat(res, "\n");
        }
    }
    printf("%s\n", res);
    close(fd1);
    return 1;
}

int editCart(char res[])
{
    
}

int buy(char res[])
{
}

int isAdmin(char userId[])
{
    int fd = open("DB/userTable", O_RDONLY);
    user temp;
    while (read(fd, &temp, sizeof(user)))
    {
        if (strcmp(temp.userId, userId) == 0)
        {
            int x = temp.isAdmin;
            close(fd);
            return x;
        }
    }
}

void processRequest(char req[], char res[])
{
    char params[5][20];
    // for(int i = 0; i < 5; i++){
    //     printf("%s xxxxxxx\n", params[i]);
    // }
    spilt(req, params);
    // for(int i = 0; i < 5; i++){
    //     printf("%s yyyyyyy\n", params[i]);
    // }
    if (strcmp(params[0], "-1") == 0)
    {
        // login
        int out;
        if (params[1][0] == '1')
        {
            out = login(params[2], params[3], res);
        }
        // register
        else if (params[1][0] == '2')
        {
            out = reg(params[2], params[3], res);
        }
    }
    else
    {
        if (isAdmin(params[0]))
        {
            // printf("is admin\n");
            if (params[1][0] == '1')
            {
                viewAllProducts(res);
                // printf("no smash 1000\n");
            }
            else if (params[1][0] == '2')
            {
                viewOneProduct(params[2], res);
            }
            else if (params[1][0] == '3')
            {
                // printf("add\n");
                addProduct(params[2], params[3], params[4], res);
            }
            else if (params[1][0] == '4')
            {
                deleteProduct(params[2], res);
            }
            else if (params[1][0] == '5')
            {
                updateProduct(params[2], params[3], params[4], res);
            }
        }
        else
        {
            if (params[1][0] == '1')
            {
                viewAllProducts(res);
            }
            else if (params[1][0] == '2')
            {
                viewOneProduct(params[2], res);
            }
            else if (params[1][0] == '3')
            {
                addToCart(params[0], params[2], params[3], res);
            }
            else if (params[1][0] == '4')
            {
                viewCart(params[0], res);
            }
            else if (params[1][0] == '5')
            {
            }
            else if (params[1][0] == '6')
            {
            }
        }
    }
    for(int i=0; i<5; i++){
        memset(params[i],0,strlen(params[i]));
    }
}

int linker(int *nsd)
{
    // printf("hi");
    // write(1, "hi", 2);
    while (1)
    {
        char req[100];
        char res[300];
        read(*nsd, req, sizeof(req));
        processRequest(req, res);
        write(*nsd, res, sizeof(res));
        memset(req,0,strlen(req));
        memset(res,0,strlen(res));
    }
}

// int main(){
//     int sd;
//     struct sockaddr_in server,client;
//     pthread_t thread;

//     sd=socket(AF_UNIX,SOCK_STREAM,0);

//     server.sin_family = AF_UNIX;
//     server.sin_addr.s_addr = inet_addr("127.0.0.1");
//     server.sin_port = htonl(5000);

//     if(bind(sd,(struct sockaddr *)&server,sizeof(server)) <0){
//         perror("BIND ERROR");
//         exit(EXIT_FAILURE);
//     }
//     if(listen(sd, 100) < 0){
//         perror("listen error");
//         exit(EXIT_FAILURE);
//     }

//     int sz = sizeof(client);
//     int nsd ;

//     while(1){
//         printf("ooo\n");
//         nsd = accept(sd,(struct sockaddr *)(&client),&sz);
//         // if(nsd <0){
//         //     perror("accept error" );
//         //     exit(EXIT_FAILURE);
//         // }

// 		if(pthread_create(&thread, NULL ,(void *)linker ,&nsd)<0){
// 			perror("could not create thread");
// 			return 1;
// 		}
//     }
//     close(sd);

// }

int main()
{
    struct sockaddr_in server, client;
    unsigned int sd, sz, nsd;

    sd = socket(AF_UNIX, SOCK_STREAM, 0);

    server.sin_family = AF_UNIX;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htonl(5000);
    bind(sd, (struct sockaddr *)&server, sizeof(server));

    listen(sd, 5);

    while (1)
    {
        sz = sizeof(client);
        if ((nsd = accept(sd, (struct sockaddr *)(&client), &sz)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        if (!fork())
        {
            close(sd);
            linker(&nsd);
            exit(0);
        }
        else
        {
            close(nsd);
        }
    }
}