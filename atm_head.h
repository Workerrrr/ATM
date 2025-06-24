#ifndef ATM_HEAD_H
#define ATM_HEAD_H

#define PASSWORD_LEN 6
#define NAME_LEN 20

#define LOGIN_GLADE "C:\\Users\\Worker\\Desktop\\Glade Project\\login_window.glade"
#define MAIN_GLADE "C:\\Users\\Worker\\Desktop\\Glade Project\\main_window.glade"
#define ACCOUNT_PATH "G:\\ATM_Project\\account.txt"
#define TEMP_PATH "G:\\ATM_Project\\temp.txt"


// Global
typedef struct {
    int account;
    char password[PASSWORD_LEN + 1];
    char name[NAME_LEN + 1];
    double balance;
} Account;

extern Account account;

// check.c
int account_check(const gchar *text);
int account_create_check(const gchar *name, const gchar *password, const gchar *password_check);
int login_password_check(const gchar *password);
int change_password_check(const gchar *old_password, const gchar *new_password, const gchar *new_password_check);
int transfer_password_check(const gchar *password);

// balance.c
int withdraw(double withdraw_balance, int deposit);
Account* get_transfer_name(int transfer_account);
int transfer_func(int transfer_account, double transfer_balance);

#endif //ATM_HEAD_H

