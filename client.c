#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<string.h>
#include<arpa/inet.h>

void keyWait(){
    printf("Enter any key to continue ->");
    getchar();
    getchar();
}

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

void charDetector(char c, char str[]){
    int i = 0;
    for(; i<strlen(str) + 1; i++){
        if(str[i] == c){
            str[i] = '\0';
            return;
        }
    }
    str[i - 1] = '\0';

}

void split(char str[], char params[][9])
{
    char temp[100];
    strcpy(temp, str);
    strcat(temp, " ");
    int count = 0;
    int idx = 0;
    for(int i = 0; i < strlen(str) && count < 2; i++){
        if(str[i] == ' '){
            count++;
            idx = 0;
        }
        else{
            params[count][idx] = str[i];
            idx++;
        }
    }
}

char * logReg(int sd, int cond){
    char userName[20];
    char password[20];
    printf("Enter username : ");
    scanf("%s",userName);
    printf("Enter password : ");
    scanf("%s",password);
    spaceWrite(sd, "-1", 8);
    (cond == 1) ? spaceWrite(sd, "1", 2) :  spaceWrite(sd, "2", 2);

    spaceWrite(sd, userName, 19);
    spaceWrite(sd, password, 19);

}

int landing(int time){
    // system("clear");
    printf("Hello !! Welcome to Achi MART !!\n");
    if(time > 0)
        printf("Invalid option\n" );    
    
    printf("Choose one of the options to proceed\n");
    printf("1. Login\n");
    printf("2. Register\n");
    printf("3. Exit\n");

    int choice;
    scanf("%d", &choice);

    if(choice == 1) 
        return 0;
    else if(choice == 2)
        return 1;
    else if(choice == 3){
        return -999;
    }
    else 
        landing(1);
}

int editCart(int sd, char userId[]){
    printf("a. Remove item\n");
    printf("b. Update Quantity\n");
    printf("c. Cancel\n");

    char choice, temp;

    scanf("%c", &choice);

    if(choice == 'c') return -1;
    
    spaceWrite(sd, userId, 8);
    spaceWrite(sd, "5 ", 2);
    char x[2] ={choice, ' '};
    spaceWrite(sd, x, 2);

    char prodId[20];
    printf("Enter Product Id : ");
    scanf("%s", prodId);

    spaceWrite(sd, prodId, 19);
    if(choice == 'b'){
        scanf("%c", &temp);
        printf("Enter the new quantity : ");
        char qty[10];
        scanf("%s", qty);
        spaceWrite(sd, qty, 9);
    }
}

int home(int sd, int time, char userId[], int isAdmin){
    if(isAdmin){
        if(time > 0)
            printf("Invalid option\n" );    
        printf("\nWhat would you like to do?\n");
        printf("1. View All Products\n");
        printf("2. View 1 products\n");
        printf("3. Add Product\n");
        printf("4. Delete Product\n");
        printf("5. Update Product quantity\n");
        printf("6. Clear Screen\n");
        printf("7. Logout\n");

                
        char choice;
        char temp;
        scanf("%c", &choice);
        if(choice == '1' || choice == '2' || choice == '3' || choice == '4' || choice == '5'){
            spaceWrite(sd, userId, 8);
            char x[2] ={choice, ' '};
            spaceWrite(sd, x, 2);
        }
        if(choice == '1') return 1;
        else if(choice == '2'){
            char id[20];
            scanf("%c", &temp);
            printf("Enter product id: ");
            scanf("%[^\n]", id);
            spaceWrite(sd, id, 19);
            return 1;
        }
    //         // buyProduct();
        else if(choice == '3'){
            char name[20], qty[10], price[10];
            scanf("%c", &temp);
            printf("Enter product name: ");
            scanf("%[^\n]", name);    
            spaceWrite(sd, name, 19);
            printf("Enter product quantity: ");
            scanf("%s", qty);
            spaceWrite(sd, qty, 9);
            printf("Enter product price: ");
            scanf("%s", price);
            spaceWrite(sd, price, 9);
            return 1;
        }
        else if(choice == '4'){
            char id[20];
            scanf("%c", &temp);
            printf("Enter product id: ");
            scanf("%[^\n]", id);
            spaceWrite(sd, id, 19);
            return 1;
        }
        else if(choice == '5'){
            char id[20], qty[10], price[10];
            scanf("%c", &temp);
            printf("Enter product id: ");
            scanf("%[^\n]", id);    
            spaceWrite(sd, id, 19);
            printf("Enter new product quantity: ");
            scanf("%s", qty);
            spaceWrite(sd, qty, 9); 
            printf("Enter new product price: ");
            scanf("%s", price);
            spaceWrite(sd, price, 9); 
            return 1;
        }
        if(choice == '6'){
            system("clear");
            scanf("%c", &temp);
            return -1;
        }
        else if(choice == '7'){
            return -999;
        }
        else 
            home(sd, 1, userId, isAdmin);
    }
    else{
        if(time > 0)
            printf("Invalid option\n" );    
        printf("What would you like to do?\n");
        printf("1. View All Products\n");
        printf("2. View 1 products\n");
        printf("3. Add to cart\n");
        printf("4. View Cart\n");
        printf("5. Edit Cart\n");
        printf("6. Buy\n");
        printf("7. Clear Screen\n");
        printf("8. Logout\n");
        
        char choice;
        char temp;
        scanf("%c", &choice);
        if(choice == '1' || choice == '2' || choice == '3' || choice == '4'){
            spaceWrite(sd, userId, 8);
            char x[2] ={choice, ' '};
            spaceWrite(sd, x, 2);
        }
        if(choice == '1' || choice == '4') return 1;

        else if(choice == '2'){
            char id[20];
            scanf("%c", &temp);
            printf("Enter product id: ");
            scanf("%[^\n]", id);
            spaceWrite(sd, id, 19);
            return 1;
        }
        else if(choice == '3'){
            char id[20], qty[10];
            scanf("%c", &temp);
            printf("Enter product id: ");
            scanf("%[^\n]", id);    
            spaceWrite(sd, id, 19);

            printf("Enter product quantity: ");
            scanf("%s", qty);
            spaceWrite(sd, qty, 9);
            return 1;
        }

        else if(choice == '5'){
            scanf("%c", &temp);
            if(editCart(sd, userId) == -1) return -1;
            return 1;
        }

        else if(choice == '6'){
            char confirm[10];
            printf("Enter \"yes\" to proceed: ");
            scanf("%s", confirm);

            if(strcmp(confirm, "yes") == 0){
                spaceWrite(sd, userId, 8);
                spaceWrite(sd, "6 ", 2);
            }
            else
                return -1;
            return 1;
        }
        else if(choice == '7'){
            system("clear");
            scanf("%c", &temp);
            return -1;
        }
        else if(choice == '8'){
            return -999;
        }
        else 
            home(sd, 1, userId, isAdmin);
    }
}

int main(){

    int sd; 
    struct sockaddr_in server ;
     
    sd = socket(PF_UNIX,SOCK_STREAM,0);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");  
    server.sin_family = AF_UNIX;
    server.sin_port = htonl(5000);

    if(connect(sd,(struct sockaddr*)(&server),sizeof(server))==-1){
        perror("Failed to connect\nExiting");
        exit(EXIT_FAILURE); 
    }

    int loggedIn = 0;
    char userId[20];
    int isAdmin = -1;


    while(1){
        char res[300];
        if(!loggedIn){
            int out = landing(0);
            if(out == -999)
                break;
            ((!out) ? logReg(sd, 1) : logReg(sd, 2));
            read(sd, res, 299);
            charDetector('#', res);
            printf("%s\n", res);
            if(res[0] != '-'){
                printf("You have successfully logged in\n");
                loggedIn = 1;
                char params[2][9];
                split(res, params);
                params[0][8] = '\0';
                strcpy(userId, params[0]);
                isAdmin = (int) params[1][0] - 48;
                memset(params[0],0,strlen(params[0]));
                memset(params[1],0,strlen(params[1]));
            }
            else{
                if(!out)
                    printf("Failed to login!! Username or password invalid!!\n");
                else
                    printf("Resgistration unsuccessful!! Username already exists!!\n");
            }
            keyWait();
        }
        else{
            int out = home(sd, 0, userId, isAdmin);

            if(out == -1) continue;
            else if(out == -999) {
                loggedIn = 0;
                isAdmin = -1;
                memset(userId,0,strlen(userId));
            }
            else {
                read(sd, res, 299);
                charDetector('#', res);
                printf("%s\n", res);
                keyWait();
            }
        }
        memset(res,0,strlen(res));
    }
    close(sd);
}




