#ifndef INIT_WINDOW_H
#define INIT_WINDOW_H

// 初始化窗口
void init_login_window(GtkBuilder *builder);
void init_login_signal(GtkBuilder *builder);

void init_main_window(GtkBuilder *builder);
void init_main_signal(GtkBuilder *builder);

void init_account_check_window(GtkBuilder *builder);
void init_account_create_window(GtkBuilder *builder);
void init_password_check_window(GtkBuilder *builder);
void init_message_box(GtkBuilder *builder);

void init_menu_window(GtkBuilder *builder);
void init_deposit_window(GtkBuilder *builder);
void init_withdraw_window(GtkBuilder *builder);
void init_transfer_window(GtkBuilder *builder);
void init_transfer_balance_window(GtkBuilder *builder);
void init_transfer_password_window(GtkBuilder *builder);
void init_change_password_window(GtkBuilder *builder);

#endif //INIT_WINDOW_H
