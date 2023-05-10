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
        int fd = open("DB/userTable", O_RDONLY);
        int count = 0;
        user temp;
        while(read(fd, &temp, sizeof(user))){
            count++;
        }
        int x = count;
        int len = 0;
        while(x > 0){
            x = x / 10;
            len++;
        }

        if(count == 0) len = 1;

        for(int i = 0; i < 8 - len; i++)
            id[i] = '0';
        char r[len];
        sprintf(r, "%d", count);
        strcat(id, r);
    }
    else{
        int fd = open("DB/productTable", O_RDONLY);
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

        if(count == 0) len = 1;

        for(int i = 0; i < 8 - len; i++)
            id[i] = '0';
        id[9 - len] = '\0';
        char r[len];
        sprintf(r, "%d", count);
        strcat(id, r);
    }

    
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
    close(fd);
    return 0;
}

int login(char username[], char password[], char res[])
{
    int fd = open("DB/userTable", O_RDONLY);
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
    for(; i<strlen(str); i++){
        if(str[i] == c){
            str[i] = '\0';
            return;
        }
    }
    str[i - 1] = '\0';

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
    idGenerator(0, id);
    strcpy(res, id);
    strcat(res, " 0");
    user u;
    u.isAdmin = 0;
    u.noOfOrders = 0;
    strcpy(u.password, password);
    strcpy(u.userName, username);
    strcpy(u.userId, id);

    memset(id,0,strlen(id));

    write(fd, &u, sizeof(user));
    return 1;
}

int viewAllProducts(char res[])
{
    strcpy(res, "Achi MART All Products!!\n\n");
    strcat(res, "\nProductId\tName\t\tQuantity\tPrice\n\n");

    int fd = open("DB/productTable", O_RDONLY);
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
    close(fd);
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
    idGenerator(1, id);

    product p1;
    strcpy(p1.name, prodName);
    p1.price = atof(price);
    strcpy(p1.prodId, id);
    memset(id,0,strlen(id));
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

int prodCheckQty(char prodId[], int qty, char res[]){

    int fd = open("DB/productTable", O_RDWR);
    product temp2;
    while(read(fd, &temp2, sizeof(product))){
        if(strcmp(temp2.prodId, prodId) == 0){
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
        }
    }
    
}

int addToCart(char userId[], char prodId[], char qty[], char res[])
{
    if(!checkInDB(prodId, "DB/productTable")){
        strcpy(res, "Invalid Product ID");
        return 0;
    }

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

    int fd1 = open("DB/cartTable", O_RDWR);
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
    close(fd1);
    return 1;
}

int editCart(char userId[], char condition, char prodId[], char qty[], char res[])
{
    if(!checkInDB(prodId, "DB/productTable")){
        strcpy(res, "Invalid Product ID");
        return 0;
    }

    int fd1 = open("DB/cartTable", O_RDWR);
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
    int fd1 = open("DB/cartTable", O_RDWR);
    int count = 0;
    cart temp1;

    strcat(res, "\n$$$$$$$$$$$$$$$$$$$$$$$ BILL $$$$$$$$$$$$$$$$$$$$$$$\n\n");
    strcat(res, "Name\t\tqty\tPrice/Item\tPrice\n");
    float totalPrice = 0.0;

    while(read(fd1, &temp1, sizeof(cart))){
        if(strcmp(temp1.userId, userId) == 0 && temp1.quantity > 0){
            count++;
            product temp2;
            int fd2 = open("DB/productTable", O_RDONLY);

            float pricePerType = 0.0;
            while(read(fd2, &temp2, sizeof(product))){
                if(strcmp(temp1.prodId, temp2.prodId) == 0){
                    pricePerType = temp2.price * temp1.quantity;
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
                }
            }
            char r[300];
            int x = editCart(userId, 'c', temp1.prodId, 0, r);
            close(fd2);
        }
    }
    if(count == 0) strcpy(res, "You have no items in cart! Please add some items first\n");
    else{
        char price[12];
        gcvt(totalPrice, 12, price);
        strcat(res, "\n\t\t\t\tTotal : ");
        strcat(res, price);
        strcat(res, "\n");
    }
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

void processRequest(int *nsd, char res[])
{
    char userId[9], buff[3];
    char opCode;

    read(*nsd, &userId, 8);
    read(*nsd, &buff, 2);

    charDetector(' ', userId);
    charDetector(' ', buff);

    opCode = buff[0];


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
            if (opCode == '1')
                viewAllProducts(res);
            else if (opCode == '2')
            {
                char prodId[20];
                read(*nsd, prodId, 19);
                charDetector(' ', prodId);
                viewOneProduct(prodId, res);
                memset(prodId,0,strlen(prodId));
            }
            else if (opCode == '3')
            {
                char name[20], qty[10], price[10];
                read(*nsd, name, 19);
                read(*nsd, qty, 9);
                read(*nsd, price, 9);
                charDetector(' ', name);
                charDetector(' ', qty);       
                charDetector(' ', price);
                addProduct(name, qty, price, res);
                memset(name,0,strlen(name));
                memset(qty,0,strlen(qty));
                memset(price,0,strlen(price));

            }
            else if (opCode == '4')
            {
                char prodId[20];
                read(*nsd, prodId, 19);
                charDetector(' ', prodId);
                deleteProduct(prodId, res);
                memset(prodId,0,strlen(prodId));

            }
            else if (opCode == '5')
            {
                char prodId[20], qty[10], price[10];   
                read(*nsd, prodId, 19);
                read(*nsd, qty, 9);
                read(*nsd, price, 9);
                charDetector(' ', prodId);
                charDetector(' ', qty);       
                charDetector(' ', price);
                updateProduct(prodId, qty, price, res);
                memset(prodId,0,strlen(prodId));
                memset(qty,0,strlen(qty));
                memset(price,0,strlen(price));
            }
        }
    
        else
        {
            if (opCode == '1')
                viewAllProducts(res);
            else if (opCode == '2')
            {
                char prodId[20];
                read(*nsd, prodId, 19);
                charDetector(' ', prodId);
                viewOneProduct(prodId, res);
                memset(prodId,0,strlen(prodId));
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
        char res[300];
        processRequest(nsd, res);
        strcat(res, "#");
        spaceWrite(*nsd, res, 299);
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