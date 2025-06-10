#ifndef BUTTON_CLICKED_H
#define BUTTON_CLICKED_H

// 按钮响应
void account_check_button_clicked(GtkButton *button, gpointer data);
void account_create_button_clicked(GtkButton *button, gpointer data);
void account_create_accept_button_clicked(GtkButton *button, gpointer data);
void account_create_cancel_button_clicked(GtkButton *button, gpointer data);
void password_check_button_clicked(GtkButton *button, gpointer data);

void change_password_button_clicked(GtkButton *button, gpointer data);
void change_password_accept_button_clicked(GtkButton *button, gpointer data);
void change_password_cancel_button_clicked(GtkButton *button, gpointer data);

void deposit_button_clicked(GtkButton *button, gpointer data);
void withdraw_button_clicked(GtkButton *button, gpointer data);
void transfer_button_clicked(GtkButton *button, gpointer data);
void quit_button_clicked(GtkButton *button, gpointer data);

void withdraw_accept_button_clicked(GtkButton *button, gpointer data);
void withdraw_cancel_button_clicked(GtkButton *button, gpointer data);

void deposit_accept_button_clicked(GtkButton *button, gpointer data);
void deposit_cancel_button_clicked(GtkButton *button, gpointer data);

void transfer_accept_button_clicked(GtkButton *button, gpointer data);
void transfer_cancel_button_clicked(GtkButton *button, gpointer data);
void transfer_balance_accept_button_clicked(GtkButton *button, gpointer data);
void transfer_balance_cancel_button_clicked(GtkButton *button, gpointer data);
void transfer_balance_return_button_clicked(GtkButton *button, gpointer data);
void transfer_password_accept_button_clicked(GtkButton *button, gpointer data);
void transfer_password_cancel_button_clicked(GtkButton *button, gpointer data);
void transfer_password_return_button_clicked(GtkButton *button, gpointer data);

#endif //BUTTON_CLICKED_H
