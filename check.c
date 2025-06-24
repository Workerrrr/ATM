#include<ctype.h>
#include<gtk/gtk.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<windows.h>
#include"atm_head.h"


// 登录计数器
int login_counter = 0;


int account_check(const gchar *text) {
    FILE *fp = fopen(ACCOUNT_PATH, "r");
    if (fp == NULL) {
        fclose(fp);
        return 0;   // 文件打开失败
    }


    //------------ 没有输入内容 -----------
    if (*text == '\0') {
        return 1;   // 空值
    }


    //-------------------- 账户验证 ---------------------
    gchar *ptr = NULL;
    const long int account_check = strtol(text, &ptr, 10);

    while(fscanf(fp, "%5d %6s %20s %lf", &account.account, account.password, account.name, &account.balance) == 4) {
        if (account.account == (int)account_check) {
            g_print("账户验证成功！\n");
            fclose(fp);
            return 2;   // 账户验证成功
        }
    }

    fclose(fp);
    return 3;   // 账户验证失败
}


int account_create_check(const gchar *name, const gchar *password, const gchar *password_check) {


    if (g_strcmp0(name, "") == 0 || g_strcmp0(password, "") == 0 || g_strcmp0(password_check, "") == 0) {
        return 5;   // 空值
    }


    for(int i = 0; name[i] != '\0'; i++) {
        if(!isprint(name[i]) || (name[i] >= 32 && name[i] <= 47)
            || (name[i] >= 58 && name[i] <= 64)
            || (name[i] >= 91 && name[i] <= 96)
            || (name[i] >= 123 && name[i] <= 127)) {
            return 4;   // 非法字符
        }
    }

    for(int i = 0; password[i] != '\0'; i++) {
        if (password[i] == 9 || password[i] == 10 || password[i] == 32)
            return 4;   // 非法字符
    }


    FILE *fp = fopen(ACCOUNT_PATH, "r");
    if (fp == NULL) {
        g_print("Error open file\n");
        fclose(fp);
        return 0;   // 无法打开文件
    }

    while(fscanf(fp, "%5d %6s %20s %lf", &account.account, account.password, account.name, &account.balance) == 4) {
        if (strcmp(account.name, name) == 0) {
            fclose(fp);
            return 2;   // 账户已存在
        }
    }

    fclose(fp);


    if (strlen(password) != PASSWORD_LEN) {
        return 6;   // 密码长度不足
    }
    if (strcmp(password_check, password) != 0) {
        return 3;   // 密码验证失败
    }


    fp = fopen(ACCOUNT_PATH, "a");  // 以追加方式打开文件
    if (fp == NULL) {
        g_print("Error open file\n");
        fclose(fp);
        return 0;   // 无法打开文件
    }

    const int new_account = account.account + 1;
    fprintf(fp, "%5d\t%6s\t%s\t%.2lf\n", new_account, password, name, 0.00); // 追加记录
    fclose(fp);

    account.account = new_account;
    *account.password = *password;
    *account.name = *name;
    account.balance = 0;

    return 1;   // 成功创建账户
}


int login_password_check(const gchar *password) {
    if (g_strcmp0(password, "") == 0) {
        return -1;   // 空值
    }

    if (strcmp(password, account.password) == 0) {
        return 0;
        // 验证成功
    }
    login_counter++;
    return login_counter;
}


int change_password_check(const gchar *old_password, const gchar *new_password, const gchar *new_password_check) {

    if (g_strcmp0(old_password, "") == 0 || g_strcmp0(new_password, "") == 0 || g_strcmp0(new_password_check, "") == 0) {
        return 2;   // 空值
    }

    if (strcmp(old_password, account.password) != 0) {
        return 3;   // 原密码错误
    }

    for(int i = 0; new_password[i] != '\0'; i++) {
        if (new_password[i] == 9 || new_password[i] == 10 || new_password[i] == 32)
            return 4;   // 非法字符
    }

    if (strcmp(new_password, new_password_check) != 0) {
        return 5;   // 密码验证错误
    }

    if (strcmp(old_password, new_password) == 0) {
        return 6;   // 新密码与旧密码相同
    }

    if (strlen(new_password) != PASSWORD_LEN) {
        return 7;   // 密码长度不足
    }

    FILE *fp = fopen(ACCOUNT_PATH, "r");
    FILE *tmp = fopen(TEMP_PATH, "w");

    if (fp == NULL || tmp == NULL) {
        g_print("Error open file\n");
        fclose(fp);
        fclose(tmp);
        return 0;   // 文件打开失败
    }

    Account temp;

    while(fscanf(fp, "%5d %6s %20s %lf", &temp.account, temp.password, temp.name, &temp.balance) == 4) {

        if(temp.account == account.account) {
            strncpy(temp.password, new_password, PASSWORD_LEN);
            strncpy(account.password, new_password, PASSWORD_LEN);
        }

        fprintf(tmp, "%-5d\t%6s\t%s\t%-.2lf\n", temp.account, temp.password, temp.name, temp.balance);
    }

    fclose(fp);
    fclose(tmp);

    remove(ACCOUNT_PATH);
    rename(TEMP_PATH, ACCOUNT_PATH);

    return 1;   // 密码更改成功
}


Account* get_transfer_name(int transfer_account) {
    FILE *fp = fopen(ACCOUNT_PATH, "r");
    if (fp == NULL) {
        fclose(fp);
        return NULL;   // 文件打开失败
    }

    Account* temp = malloc(sizeof(Account));

    if (temp == NULL) {
        fclose(fp);
        return NULL;
    }

    while(fscanf(fp, "%5d %6s %20s %lf", &temp->account, temp->password, temp->name, &temp->balance) == 4) {
        g_print("%5d %6s %20s %lf\n", temp->account, temp->password, temp->name, temp->balance);
        if (temp->account == transfer_account) {
            temp->balance = 0;
            g_print("账户验证成功！\n");
            fclose(fp);
            return temp;   // 账户验证成功
        }
    }

    free(temp);
    fclose(fp);

    return NULL;    // 账户不存在
}


int transfer_password_check(const gchar *password) {
    if (g_strcmp0(password, "") == 0) {
        return 0;   // 空值
    }

    if (strcmp(password, account.password) == 0) {
        return 1;
        // 验证成功
    }

    return 2;
}