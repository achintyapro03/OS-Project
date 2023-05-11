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
#include <math.h>
#include "data.h"


void spaceWrite(int sd, char str[], int len2){
    int len1 = strlen(str);
    char new[len2 - len1];
    for(int i = 0; i < len2 - len1; i++){
        new[i] = ' ';
    } 
    char temp[len2];
    strcpy(temp, str);
    strcat(temp, new);
    write(sd, temp, len2);
    memset(new,0,strlen(new));
    memset(temp,0,strlen(temp));
}

void idGenerator(int cond, char id[]){
    if(!cond){
        int fd = open("DB/userTable", O_RDWR| O_CREAT, 0744);
        int count = 0;
        product temp;
        while(read(fd, &temp, sizeof(product))){
            count++;
        }

        printf("count : %d\n",count);
        count--;
        int x = count;
        int len = 0;
        while(x > 0){
            x = x / 10;
            len++;
        }

        for(int i = 0; i < 8 - len; i++)
            id[i] = '0';
        id[8 - len] = '\0';
        char r[len];
        sprintf(r, "%d", count);
        strcat(id, r);
    }
    else{
        int fd = open("DB/productTable", O_RDWR| O_CREAT, 0744);
        int count = 0;
        product temp;
        while(read(fd, &temp, sizeof(product))){
            count++;
        }
        int x = count;
        int len = 0;
        while(x > 0){
            x = x / 10;
            len++;
        }

        if(count == 0){
            strcpy(id, "00000000\0");
            return;
        }

        for(int i = 0; i < 8 - len; i++)
            id[i] = '0';
        id[8 - len] = '\0';
        char r[len];
        sprintf(r, "%d", count);
        strcat(id, r);
    }

    
}

int checkInDB(char key[], char DBname[]){
    int fd = open(DBname, O_RDWR| O_CREAT, 0744);

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
    close(fd);
    return 0;
}

int login(char username[], char password[], char res[])
{
    int fd = open("DB/userTable", O_RDWR| O_CREAT, 0744);
    lseek(fd, 0, SEEK_SET);
    user temp;
    while (read(fd, &temp, sizeof(user)))
    {
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
    strcpy(res, "-1");
    close(fd);
    return 0;
}

void charDetector(char c, char str[]){
    int i = 0;
    for(; i<strlen(str) + 1; i++){
        if(str[i] == c || (int)str[i] == 127){
            str[i] = '\0';
            return;
        }
    }
    str[i - 1] = '\0';

}

int reg(char username[], char password[], char res[])
{
    int fd = open("DB/userTable", O_RDWR| O_CREAT, 0744);
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
    idGenerator(0, id);
    strcpy(res, id);
    strcat(res, " 0");
    user u;
    u.isAdmin = 0;
    strcpy(u.password, password);
    strcpy(u.userName, username);
    strcpy(u.userId, id);

    memset(id,0,strlen(id));

    write(fd, &u, sizeof(user));
    return 1;
}

int viewAllProducts(int fd, char res[])
{
    strcpy(res, "Achi MART All Products!!\n\n");
    strcat(res, "\nProductId\tName\t\tQuantity\tPrice\n\n");

    
    product temp;
    while (read(fd, &temp, sizeof(product)))
    {
        if (temp.quantity == 0)
            continue;
        strcat(res, temp.prodId);
        strcat(res, "\t");
        strcat(res, temp.name);
        strcat(res, "\t\t");
        char qty[9];
        sprintf(qty, "%d", temp.quantity);
        strcat(res, qty);
        strcat(res, "\t\t");
        char price[8];
        gcvt(temp.price, 8, price);
        strcat(res, price);
        strcat(res, "\n");
    }
    return 1;
}

int viewOneProduct(int fd, char prodId[], char res[])
{
    strcpy(res, "Achi MART!!\n\n");
    strcat(res, "\nProductId\tName\tQuantity\tPrice\n\n");

    if(!checkInDB(prodId, "DB/productTable")){
        strcpy(res, "Opps!! Product not found!\n");
        return 0;
    }

    product temp;
    printf("offset: %d\n", atoi(prodId));
    lseek(fd, atoi(prodId) * sizeof(product), SEEK_SET);
    read(fd, &temp, sizeof(product));
    
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

void lockRecord(int fd, int cond, int recordNumber) {
    struct flock fl;
    fl.l_type = F_WRLCK; 
    fl.l_whence = SEEK_SET;
    fl.l_start = cond * recordNumber * sizeof(product);
    fl.l_len = cond * sizeof(product);
    fl.l_pid = getpid();

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
    fl.l_pid = getpid();

    if (fcntl(fd, F_SETLK, &fl) == -1) {
        perror("fcntl");
        exit(1);
    }
}

int addProduct(int fd, char prodName[], char qty[], char price[], char res[])
{
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
            return 1;
        }
    }
    char id[9];
    idGenerator(1, id);

    product p1;
    strcpy(p1.name, prodName);
    p1.price = atof(price);
    strcpy(p1.prodId, id);
    memset(id,0,strlen(id));
    p1.quantity = atoi(qty);
    strcpy(res, "Product Added");
    write(fd, &p1, sizeof(product));
    return 1;
}

int deleteProduct(int fd, char prodId[], char res[])
{
    if(!checkInDB(prodId, "DB/productTable")){
        strcpy(res, "Opps!! Product not found!\n");
        return 0;
    }
    product temp;

    lseek(fd, atoi(prodId) * sizeof(product), SEEK_SET);
    read(fd, &temp, sizeof(product));
    temp.quantity = 0;
    temp.price = 0;
    lseek(fd, -1 * sizeof(product), SEEK_CUR);
    write(fd, &temp, sizeof(product));
    strcpy(res, "Product Deleted!!\n");
    return 1;
}

int updateProduct(int fd, char prodId[], char qty[], char price[], char res[])
{
    if(!checkInDB(prodId, "DB/productTable")){
        strcpy(res, "Opps!! Product not found!\n");
        return 0;
    }
    printf("in updateProduct\n");
    product temp;
    lseek(fd, atoi(prodId) * sizeof(product), SEEK_SET);
    read(fd, &temp, sizeof(product));
    temp.quantity = atoi(qty);
    temp.price = atof(price);
    lseek(fd, -1 * sizeof(product), SEEK_CUR);
    write(fd, &temp, sizeof(product));
    strcpy(res, "Product Updated");
    return 1;
    
}

int prodCheckQty(char prodId[], int qty, char res[]){

    int fd = open("DB/productTable", O_RDWR| O_CREAT, 0744);

    lockRecord(fd, 1, atoi(prodId));
    product temp2;

    lseek(fd, atoi(prodId) * sizeof(product), SEEK_SET);
    read(fd, &temp2, sizeof(product));
    int q = temp2.quantity - qty;
    if(q < 0){
        strcpy(res, "Insufficient quantity!");
        close(fd);
        return 0;
    }
    else{
        temp2.quantity = q;
        lseek(fd, -1 * sizeof(product), SEEK_CUR);
        write(fd, &temp2, sizeof(product));
        close(fd);
        return 1;
    }

    unlockRecord(fd, 1, atoi(prodId));
    close(fd);    
}

int addToCart(char userId[], char prodId[], char qty[], char res[])
{
    if(!checkInDB(prodId, "DB/productTable")){
        strcpy(res, "Invalid Product ID");
        return 0;
    }

    int fd1 = open("DB/cartTable", O_RDWR| O_CREAT, 0744);
    lseek(fd1, 0, SEEK_SET);

    cart temp1;

    while (read(fd1, &temp1, sizeof(cart)))
    {
        if (strcmp(temp1.prodId, prodId) == 0)
        {
            if (strcmp(temp1.userId, userId) == 0)
            {
                temp1.quantity = temp1.quantity + atoi(qty);

                if(!prodCheckQty(prodId, atoi(qty), res)) {
                    close(fd1);
                    return 0;
                }

                lseek(fd1, -1 * sizeof(cart), SEEK_CUR);
                write(fd1, &temp1, sizeof(cart));
                strcpy(res, "Product added to cart");
                close(fd1);
                return 1;
            }
        }
    }

    cart c;
    strcpy(c.userId, userId);
    strcpy(c.prodId, prodId);

    if(!prodCheckQty(prodId, atoi(qty), res)) return 0;
    
    c.quantity = atoi(qty);
    strcpy(res, "Product added to cart");
    write(fd1, &c, sizeof(cart));
    close(fd1);

    return 1;
}

int viewCart(char userID[], char res[])
{

    int fd1 = open("DB/cartTable", O_RDWR| O_CREAT, 0744);
    lseek(fd1, 0, SEEK_SET);
    strcpy(res, "\nProducs in cart\n");
    strcat(res, "Prod Id\t\tProd Name\tQuantity\n");
    cart temp1;

    while (read(fd1, &temp1, sizeof(cart)))
    {
        if (strcmp(temp1.userId, userID) == 0 && temp1.quantity > 0)
        {
            strcat(res, temp1.prodId);
            strcat(res, "\t");
            int fd2 = open("DB/productTable", O_RDWR| O_CREAT, 0744);

            lockRecord(fd2, 1, atoi(temp1.prodId));

            product temp2;
            lseek(fd2, atoi(temp1.prodId) * sizeof(product), SEEK_SET);
            read(fd2, &temp2, sizeof(product));
            strcat(res, temp2.name);
            strcat(res, "\t\t");

            unlockRecord(fd2, 1, atoi(temp2.prodId));

            close(fd2);

            char qty[8];
            sprintf(qty, "%d", temp1.quantity);
            strcat(res, qty);
            strcat(res, "\n");
        }
    }
    close(fd1);
    return 1;
}

int editCart(char userId[], char condition, char prodId[], char qty[], char res[])
{
    if(!checkInDB(prodId, "DB/productTable")){
        strcpy(res, "Invalid Product ID");
        return 0;
    }

    int fd1 = open("DB/cartTable", O_RDWR| O_CREAT, 0744);
    lseek(fd1, 0, SEEK_SET);

    cart temp1;

    while (read(fd1, &temp1, sizeof(cart)))
    {
        if (strcmp(temp1.prodId, prodId) == 0)
        {
            if (strcmp(temp1.userId, userId) == 0)
            {
                int q, change;
                if(condition == 'a')
                    q = 0;
                else if(condition == 'b')
                    q = atoi(qty);
                else{
                    q = 0;
                    change = 0;
                }
                
                change = q - temp1.quantity;
                temp1.quantity = q;
                
                if(condition != 'c'){
                    if(!prodCheckQty(prodId, change, res)) {
                    close(fd1); 
                    return 0;
                    }
                }

                lseek(fd1, -1 * sizeof(cart), SEEK_CUR);
                write(fd1, &temp1, sizeof(cart));

                if(condition == 'a')
                    strcpy(res, "Porduct removed from cart");
                else if(condition == 'b')
                    strcpy(res, "Product quantity updated in cart");
                
                close(fd1);
                return 1;
            }
        }
    }
    close(fd1);

    strcpy(res, "You dont have this product in cart!!");

    return 1;
}

int buy(char userId[], char res[])
{
    int fd1 = open("DB/cartTable", O_RDWR| O_CREAT, 0744);
    int count = 0;
    cart temp1;

    strcat(res, "\n$$$$$$$$$$$$$$$$$$$$$$$ BILL $$$$$$$$$$$$$$$$$$$$$$$\n\n");
    strcat(res, "Name\t\tqty\tPrice/Item\tPrice\n");
    float totalPrice = 0.0;

    while(read(fd1, &temp1, sizeof(cart))){
        if(strcmp(temp1.userId, userId) == 0 && temp1.quantity > 0){
            count++;
            int fd2 = open("DB/productTable", O_RDWR| O_CREAT, 0744);

            lockRecord(fd2, 1, atoi(temp1.prodId));
            product temp2;

            lseek(fd2, atoi(temp1.prodId) * sizeof(product), SEEK_SET);
            read(fd2, &temp2, sizeof(product));

            unlockRecord(fd2, 1, atoi(temp1.prodId));


            float pricePerType = temp2.price * temp1.quantity;
            totalPrice += pricePerType;
            strcat(res, temp2.name);
            strcat(res, "\t\t");
            char qty[9];
            sprintf(qty, "%d", temp1.quantity);
            strcat(res, qty);
            strcat(res, "\t");
            char price1[8];
            gcvt(temp2.price, 8, price1);
            char price2[12];
            gcvt(pricePerType, 12, price2);
            strcat(res, price1);
            strcat(res, "\t\t");
            strcat(res, price2);
            strcat(res, "\n");
                
            char r[300];
            // int x = editCart(userId, 'c', temp1.prodId, 0, r);
            close(fd2);
        }
    }
    totalPrice = round(totalPrice * 100) / 100;
    if(count == 0) strcpy(res, "You have no items in cart! Please add some items first\n");
    else{
        char price[10];
        gcvt(totalPrice, 10, price);
        strcat(res, "\n\t\t\t\tTotal : ");
        strcat(res, price);
        strcat(res, "\n");
    }
}

int isAdmin(char userId[])
{

    int fd = open("DB/userTable", O_RDWR| O_CREAT, 0744);
    user temp;
    while (read(fd, &temp, sizeof(user)))
    {
        printf("my user id len = %ld\n", strlen(userId));
        printf("user id len = %ld\n", strlen(temp.userId));

        if (strcmp(temp.userId, userId) == 0)
        {
            int x = temp.isAdmin;
            close(fd);
            return x;
        }
    }
}

void testFunction(){
    for(int i = 0; i < 10; i++){
        printf("%i\n", (i + 1) * (i + 1));
    }
}

void processRequest(int *nsd, char res[])
{
    char userId[9], buff[3];
    char opCode;

    read(*nsd, &userId, 8);
    read(*nsd, &buff, 2);
    printf("userId: %s\n", userId);
    charDetector(' ', userId);
    charDetector(' ', buff);
    printf("userId: %s\n", userId);
    printf("%s\n", buff);

    opCode = buff[0];

    printf("khalibhali\n");
    if (strcmp(userId, "-1") == 0)
    {
        // login
        int out;
        char userName[20], password[20];

        read(*nsd, &userName, 19);
        read(*nsd, &password, 19);
        charDetector(' ', userName);
        charDetector(' ', password);

        if (opCode == '1')
            out = login(userName, password, res);
        else if (opCode == '2')
            out = reg(userName, password, res);
    }
    else
    {
        if (isAdmin(userId))
        {
            printf("yes Admin!!\n");
            if (opCode == '1'){
                int fd = open("DB/productTable", O_RDWR| O_CREAT, 0744);
                lockRecord(fd, 0, 0);
                viewAllProducts(fd, res);
                unlockRecord(fd, 0, 0);
                close(fd);
            }
            else if (opCode == '2')
            {
                char prodId[20];
                read(*nsd, prodId, 19);
                charDetector(' ', prodId);

                int fd = open("DB/productTable", O_RDWR| O_CREAT, 0744);

                lockRecord(fd, 1, atoi(prodId));
                viewOneProduct(fd, prodId, res);
                unlockRecord(fd, 1, atoi(prodId));

                memset(prodId,0,strlen(prodId));
                close(fd);
            }
            else if (opCode == '3')
            {
                int fd = open("DB/productTable", O_RDWR| O_CREAT, 0744);

                lockRecord(fd, 0, 0);

                char name[20], qty[10], price[10];
                read(*nsd, name, 19);
                read(*nsd, qty, 9);
                read(*nsd, price, 9);
                charDetector(' ', name);
                charDetector(' ', qty);       
                charDetector(' ', price);
                addProduct(fd, name, qty, price, res);

                unlockRecord(fd, 0, 0);

                memset(name,0,strlen(name));
                memset(qty,0,strlen(qty));
                memset(price,0,strlen(price));
                close(fd);

            }
            else if (opCode == '4')
            {
                int fd = open("DB/productTable", O_RDWR| O_CREAT, 0744);

                lockRecord(fd, 0, 0);

                char prodId[20];
                read(*nsd, prodId, 19);
                charDetector(' ', prodId);
                deleteProduct(fd, prodId, res);

                unlockRecord(fd, 0, 0);

                memset(prodId,0,strlen(prodId));
                close(fd);

            }
            else if (opCode == '5')
            {
                int fd = open("DB/productTable", O_RDWR | O_CREAT, 0744);

                char prodId[20], qty[10], price[10];
                read(*nsd, prodId, 19);
                charDetector(' ', prodId);
                lockRecord(fd, 1, atoi(prodId));
                read(*nsd, qty, 9);
                read(*nsd, price, 9);

                charDetector(' ', qty);
                charDetector(' ', price);
                updateProduct(fd, prodId, qty, price, res);
                unlockRecord(fd, 1, atoi(prodId));

                memset(prodId,0,strlen(prodId));
                memset(qty,0,strlen(qty));
                memset(price,0,strlen(price));

                close(fd);
            }
        }
    
        else
        {
            if (opCode == '1'){
                int fd = open("DB/productTable", O_RDWR| O_CREAT, 0744);

                lockRecord(fd, 0, 0);
                viewAllProducts(fd, res);
                unlockRecord(fd, 0, 0);

                close(fd);
            }
            else if (opCode == '2')
            {
                char prodId[20];
                read(*nsd, prodId, 19);
                charDetector(' ', prodId);
                printf("int prodid : %d\n", atoi(prodId));

                int fd = open("DB/productTable", O_RDWR| O_CREAT, 0744);

                lockRecord(fd, 1, atoi(prodId));
                viewOneProduct(fd, prodId, res);
                unlockRecord(fd, 1, atoi(prodId));
                
                memset(prodId,0,strlen(prodId));
                close(fd);
            }
            else if (opCode == '3')
            {
                char prodId[20], qty[10];   
                read(*nsd, prodId, 19);
                read(*nsd, qty, 9);
                charDetector(' ', prodId);
                charDetector(' ', qty);       
                addToCart(userId, prodId, qty, res);
                memset(prodId,0,strlen(prodId));
                memset(qty,0,strlen(qty));
            }
            else if (opCode == '4')
            {
                viewCart(userId, res);
            }
            else if (opCode == '5')
            {
                char buff2[3], opCode2, prodId[20], qty[10];
                read(*nsd, &buff2, 2);
                read(*nsd, prodId, 19);
                charDetector(' ', buff2);
                charDetector(' ', prodId);

                opCode2 = buff2[0];

                if(opCode2 == 'b'){
                    read(*nsd, &qty, 9);
                    charDetector(' ', qty);
                }
                editCart(userId, opCode2, prodId, qty, res);
                memset(prodId,0,strlen(prodId));
                memset(qty,0,strlen(qty));
                memset(buff2,0,strlen(buff2));
            }
            else if (opCode == '6')
            {
                buy(userId, res);
            }
        }
    }
}

int linker(int *nsd)
{

    while (1)
    {
        char res[350];
        processRequest(nsd, res);
        strcat(res, "#");
        printf("%s", res);
        spaceWrite(*nsd, res, 349);
        memset(res,0,strlen(res));
    }
}

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
            close(nsd);
    }
}