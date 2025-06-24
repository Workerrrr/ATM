#include<gtk/gtk.h>
#include<stdlib.h>
#include<string.h>
#include"atm_head.h"

int withdraw(double withdraw_balance, int deposit) { // withdraw完成取款和存款两个功能
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
            if (deposit == 0)
                temp.balance -= withdraw_balance;
            else
                temp.balance += withdraw_balance;
            account.balance = temp.balance;
        }

        fprintf(tmp, "%-5d\t%6s\t%s\t%-.2lf\n", temp.account, temp.password, temp.name, temp.balance);
    }

    fclose(fp);
    fclose(tmp);

    remove(ACCOUNT_PATH);
    rename(TEMP_PATH, ACCOUNT_PATH);

    return 1;   // 操作成功
}


int transfer_func(int transfer_account, double transfer_balance) {
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
        if (temp.account == transfer_account) {
            temp.balance += transfer_balance;
        }

        if (temp.account == account.account) {
            account.balance -= transfer_balance;
        }

        fprintf(tmp, "%-5d\t%6s\t%s\t%-.2lf\n", temp.account, temp.password, temp.name, temp.balance);
    }

    fclose(fp);
    fclose(tmp);

    remove(ACCOUNT_PATH);
    rename(TEMP_PATH, ACCOUNT_PATH);

    return 1;   // 操作成功
}