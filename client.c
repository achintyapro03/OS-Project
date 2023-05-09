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

// typedef struct request {
//     char uid[8];
//     int opCode;
//     char 
// }

void keyWait(){
    printf("Enter any key to continue ->");
    getchar();
    getchar();
}

void split(char str[], char params[][20])
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
}

int contactServer(int sd, char req[], char res[]){
    write(sd,req,100);
    read(sd,res,300);
}


// char* itoa(int val, int base){
	
// 	static char buf[32] = {0};
	
// 	int i = 30;
	
// 	for(; val && i ; --i, val /= base)
	
// 		buf[i] = "0123456789abcdef"[val % base];
	
// 	return &buf[i+1];
	
// }

char * logReg(int cond, char req []){
    char name[50];
    char password[50];
    printf("Enter username : ");
    scanf("%s",name);
    printf("Enter password : ");
    scanf("%s",password);

    (cond == 1) ? strcpy(req,"-1 1 ") :  strcpy(req,"-1 2 ");
    strcat(req, name);
    strcat(req, " ");
    strcat(req, password);
    strcat(req, " ");
    strcat(req, "0");
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

// int dispProd(){

// }

// int buyProduct(){

// }

// int viewCart(){

// }

int editCart(char * req){
    printf("a. Remove item\n");
    printf("b. Update Quantity\n");
    printf("c. Cancel\n");

    char choice;

    scanf("%c", &choice);

    char prodId[50];
    printf("Enter Product Id : ");
    scanf("%s", prodId);
    // printf("%c", choice);
    // keyWait();
    if(choice == 'a'){
        strcat(req, " ");
        strcat(req, prodId);
        strcat(req, " 0 0");    
    }
    else if(choice == 'b'){
        printf("Enter the new quantity : ");
        char qty[10];
        scanf("%s", qty);
        strcat(req, " ");
        strcat(req, prodId);
        strcat(req, " ");
        strcat(req, qty); 
        strcat(req, " 0");  
    }
    else{
        return -1;
    }
}


// Request Format
// UserId RequestType 


int home(int time, char req[], char userId[], int isAdmin){
    // system("clear");
    
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

                
        int choice;
        char temp;
        strcpy(req, userId);
        // printf("%s\n", userId);
        // strcpy(temp, sid);
        // printf("%s\n", req);
        scanf("%d", &choice);
        if(choice == 1) 
            strcat(req, " 1 0 0 0");
            // dispProd();
        else if(choice == 2){
            char x[30];
            scanf("%c", &temp);
            printf("Enter product id: ");
            scanf("%[^\n]", x);
            strcat(req, " 2 ");
            strcat(req, x);
            strcat(req, " 0 0");
        }
            // buyProduct();
        else if(choice == 3){
            char name[30];
            char qty[10];
            char price[10];
            scanf("%c", &temp);
            printf("Enter product name: ");
            scanf("%[^\n]", name);    
            printf("Enter product quantity: ");
            scanf("%s", qty);
            printf("Enter product price: ");
            scanf("%s", price);
            strcat(req, " 3 ");
            strcat(req, name);
            strcat(req, " ");
            strcat(req, qty);
            strcat(req, " ");  
            strcat(req, price);  

        }
            // viewCart();
        else if(choice == 4){
            char name[30];
            scanf("%c", &temp);
            printf("Enter product name: ");
            scanf("%[^\n]", name);    
            strcat(req, " 4 ");
            strcat(req, name);
            strcat(req, " 0");
            strcat(req, " 0");  
        }
        else if(choice == 5){
            char id[30];
            char qty[10];
            char price[10];
            scanf("%c", &temp);
            printf("Enter product id: ");
            scanf("%[^\n]", id);    
            printf("Enter new product quantity: ");
            scanf("%s", qty);
            printf("Enter new product price: ");
            scanf("%s", price);
            strcat(req, " 5 ");
            strcat(req, id);
            strcat(req, " ");
            strcat(req, qty);
            strcat(req, " ");
            strcat(req, price);  
        }
        else if(choice == 6){
            system("clear");
            return -1;
        }
            // editCart();
        else if(choice == 7){
            return -999;
        }
        else 
            home(1, req, userId, isAdmin);
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
        
        int choice;
        char temp;
        strcpy(req, userId);
        // printf("%s\n", userId);
        // strcpy(temp, sid);
        // printf("%s\n", req);
        scanf("%d", &choice);
        if(choice == 1) 
            strcat(req, " 1 0 0");
            // dispProd();
        else if(choice == 2){
            char x[30];
            scanf("%c", &temp);
            printf("Enter product name: ");
            scanf("%[^\n]", x);
            strcat(req, " 2 ");
            strcat(req, x);
            strcat(req, " 0");
            strcat(req, " 0");  
        }
            // buyProduct();
        else if(choice == 3){
            char x[30];
            char qty[10];
            scanf("%c", &temp);
            printf("Enter product id: ");
            scanf("%[^\n]", x);    
            printf("Enter product quantity: ");
            scanf("%s", qty);
            strcat(req, " 3 ");
            strcat(req, x);
            strcat(req, " ");
            strcat(req, qty);
            strcat(req, " 0");  
        }
            // viewCart();
        else if(choice == 4){
            strcat(req, " 4 0 0");
            strcat(req, " 0");  
        }
        else if(choice == 5){
            scanf("%c", &temp);
            strcat(req, " 5 ");
            if(editCart(req) == -1) return -1;
        }

        else if(choice == 6){
            char confirm[10];
            printf("Enter \"yes\" to proceed: ");
            scanf("%s", confirm);

            if(strcmp(confirm, "yes"))
                strcat(req, " 6 0 0 0");
            else
                return -1;
        }
            // editCart();
        else if(choice == 7){
            system("clear");
            return -1;
        }
        else if(choice == 8){
            return -999;
        }
        else 
            home(1, req, userId, isAdmin);
    }
    // keyWait();

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
        char req[100];
        char res[300];
        // system("clear");
        if(!loggedIn){
            int out = landing(0);
            if(out == -999)
                break;
            ((!out) ? logReg(1, req) : logReg(2,req));
            contactServer(sd, req, res);
            printf("%s\n", req);
            printf("%s\n", res);
            if(res[0] != '-'){
                printf("You have successfully logged in\n");
                loggedIn = 1;
                char params[2][20];
                split(res, params);
                printf("params: %s\n", params[0]);
                printf("params: %s\n", params[1]);

                strcpy(userId, params[0]);
                isAdmin = (int) params[1][0] - 48;
                printf("user : %s\n", userId);
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
            printf("%s\n", userId);    
            int out = home(0, req, userId, isAdmin);
            printf("%s\n", userId);    
            printf("%s\n", req);
            if(out == -1) continue;
            else if(out == -999) {
                loggedIn = 0;
                isAdmin = -1;
                memset(userId,0,strlen(userId));
            }
            else {
                contactServer(sd, req, res);
                // printf("ola");
                printf("%s\n", res);
                keyWait();
            }
        }
        memset(req,0,strlen(req));
        memset(res,0,strlen(res));
    }
    close(sd);
}




