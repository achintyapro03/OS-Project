typedef struct product{
    char name[20];
    char prodId[9];
    int quantity;
    float price;
}product;

typedef struct user {
    char userName[20];
    char password[20];
    char userId[9];
    int isAdmin;
}user;


typedef struct cart{
    char userId[9];
    char prodId[9];
    int quantity;
}cart;