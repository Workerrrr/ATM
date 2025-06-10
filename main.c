#include <windows.h>
#include <gtk/gtk.h>
#include "atm_head.h"
#include "init_window.h"
#include "button_clicked.h"

void open_new_window(GtkWidget *old_window, GtkWidget *new_window, GtkBuilder *builder);

// 消息框
gint show_message_box(GtkBuilder *builder, const gchar *message);
void message_box_response(GtkMessageDialog *dialog, gint response_id, gpointer data);



// 使用函数指针绑定窗口与其初始化函数
typedef void (*window_init_func)(GtkBuilder *builder);


// 使用全局窗口指针

// 登录界面
typedef struct {
    GtkWidget *account_check_window;
    GtkWidget *account_create_window;
    GtkWidget *password_check_window;
    GtkWidget *message_box;
} LoginWindow;

LoginWindow login_windows;


// 系统主界面
typedef struct {
    GtkWidget *menu_window;
    GtkWidget *deposit_window;
    GtkWidget *withdraw_window;
    GtkWidget *transfer_window;
    GtkWidget *transfer_balance_window;
    GtkWidget *transfer_password_window;
    GtkWidget *change_password_window;
    GtkWidget *message_box;
} MainWindow;

MainWindow main_windows;


// 用户数据
Account account;


// 初始化所有登录界面窗口
void init_login_window(GtkBuilder *builder) {
    // 读取Glade文件
    gtk_builder_add_from_file(builder, "C:\\Users\\Worker\\Desktop\\Glade Project\\login_window.glade", 0);

    // 初始化窗口指针
    login_windows.account_check_window = GTK_WIDGET(gtk_builder_get_object(builder, "account_check_window"));
    login_windows.account_create_window = GTK_WIDGET(gtk_builder_get_object(builder, "account_create_window"));
    login_windows.password_check_window = GTK_WIDGET(gtk_builder_get_object(builder, "password_check_window"));
    login_windows.message_box = GTK_WIDGET(gtk_builder_get_object(builder, "message_box"));


    // 绑定窗口与其初始化函数
    g_object_set_data(G_OBJECT(login_windows.account_check_window), "init_func", (gpointer)init_account_check_window);
    g_object_set_data(G_OBJECT(login_windows.account_create_window), "init_func", (gpointer)init_account_create_window);
    g_object_set_data(G_OBJECT(login_windows.password_check_window), "init_func", (gpointer)init_password_check_window);
}


// 绑定所有登录界面窗口信号
void init_login_signal(GtkBuilder *builder) {
    // 之前已经读取过Glade文件, 这里不需要再次读取

    // account_check_window 窗口信号
    GtkWidget *account_check_button = GTK_WIDGET(gtk_builder_get_object(builder, "account_check_button"));
    GtkWidget *account_create_button = GTK_WIDGET(gtk_builder_get_object(builder, "account_create_button"));

    g_signal_connect(login_windows.account_check_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(account_check_button, "clicked", G_CALLBACK(account_check_button_clicked), builder);
    g_signal_connect(account_create_button, "clicked", G_CALLBACK(account_create_button_clicked), builder);


    // account_create_window 窗口信号
    GtkWidget *account_create_accept_button = GTK_WIDGET(gtk_builder_get_object(builder, "account_create_accept_button"));
    GtkWidget *account_create_cancel_button = GTK_WIDGET(gtk_builder_get_object(builder, "account_create_cancel_button"));

    g_signal_connect(account_create_accept_button, "clicked", G_CALLBACK(account_create_accept_button_clicked), builder);
    g_signal_connect(account_create_cancel_button, "clicked", G_CALLBACK(account_create_cancel_button_clicked), builder);


    // password_check_window 窗口信号
    GtkWidget *password_check_button = GTK_WIDGET(gtk_builder_get_object(builder, "password_check_button"));

    g_signal_connect(password_check_button, "clicked", G_CALLBACK(password_check_button_clicked), builder);
}


// 初始化消息框
void init_message_box(GtkBuilder *builder) {
    GtkWidget *message_box = GTK_WIDGET(gtk_builder_get_object(builder, "message_box"));

    g_signal_connect(message_box, "response", G_CALLBACK(message_box_response), NULL);
}


// 显示消息框
gint show_message_box(GtkBuilder *builder, const gchar *message) {
    init_message_box(builder);
    GtkWidget *message_box = GTK_WIDGET(gtk_builder_get_object(builder, "message_box"));
    gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(message_box), message);

    // 运行消息框并接收返回值
    gint response = gtk_dialog_run(GTK_DIALOG(message_box));
    // gtk_widget_destroy(message_box);
    return response;
}


// 消息框响应
void message_box_response(GtkMessageDialog *dialog, gint response_id, gpointer data) {
    gtk_widget_hide(GTK_WIDGET(dialog));
}


// 初始化账户验证窗口
void init_account_check_window(GtkBuilder *builder) {
    GtkWidget *account_check_error_label = GTK_WIDGET(gtk_builder_get_object(builder, "account_check_error_label"));
    GtkWidget *entry = GTK_WIDGET(gtk_builder_get_object(builder, "entry"));

    gtk_entry_set_text(GTK_ENTRY(entry), "");
    gtk_label_set_text(GTK_LABEL(account_check_error_label), "");
}


// 账户验证按钮按下
void account_check_button_clicked(GtkButton *button, gpointer data) {
    GtkBuilder *builder = GTK_BUILDER (data);
    GtkWidget *entry = GTK_WIDGET(gtk_builder_get_object(builder, "entry"));
    GtkWidget *account_check_error_label = GTK_WIDGET(gtk_builder_get_object(builder, "account_check_error_label"));

    // 获取输入文本
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));

    // 进入账户验证, 并接收返回值
    const int checked = account_check(text);

    if (checked == 2) { // 账户验证成功
        gtk_label_set_text(GTK_LABEL(account_check_error_label), "");
        open_new_window(login_windows.account_check_window, login_windows.password_check_window, builder);
    } else {
        if (checked == 0) { // 文件打开失败
            gint response = show_message_box(builder, "系统错误！");

            // 点击“OK”
            if (response == GTK_RESPONSE_ACCEPT || response == GTK_RESPONSE_OK || response == GTK_RESPONSE_YES) {
                gtk_main_quit();
                exit(0);
            }
        }else if (checked == 1) {   // 空值
            gtk_label_set_text(GTK_LABEL(account_check_error_label), "请输入账户");
        }else if (checked == 3) {   // 账户验证失败
            gtk_label_set_text(GTK_LABEL(account_check_error_label), "账户不存在");
        }
        // 清空输入栏
        gtk_entry_set_text(GTK_ENTRY(entry), "");
    }
}


// 开户按钮按下
void account_create_button_clicked(GtkButton *button, gpointer data) {
    GtkBuilder *builder = GTK_BUILDER (data);
    open_new_window(login_windows.account_check_window, login_windows.account_create_window, builder);
}


// 初始化开户界面窗口
void init_account_create_window(GtkBuilder *builder) {
    GtkWidget *name_entry = GTK_WIDGET(gtk_builder_get_object(builder, "account_create_name_entry"));
    GtkWidget *password_entry = GTK_WIDGET(gtk_builder_get_object(builder, "account_create_password_entry"));
    GtkWidget *check_entry = GTK_WIDGET(gtk_builder_get_object(builder, "account_create_check_entry"));
    GtkWidget *account_create_check_error_label = GTK_WIDGET(gtk_builder_get_object(builder, "account_create_check_error_label"));

    // 将所有输入栏清空
    gtk_entry_set_text(GTK_ENTRY(name_entry), "");
    gtk_entry_set_text(GTK_ENTRY(password_entry), "");
    gtk_entry_set_text(GTK_ENTRY(check_entry), "");

    // 隐藏错误提示栏
    gtk_label_set_text(GTK_LABEL(account_create_check_error_label), "");
}


// 开户确认按钮按下
void account_create_accept_button_clicked(GtkButton *button, gpointer data) {
    GtkBuilder *builder = GTK_BUILDER (data);

    GtkWidget *account_create_name_entry = GTK_WIDGET(gtk_builder_get_object(builder, "account_create_name_entry"));
    GtkWidget *account_create_password_entry = GTK_WIDGET(gtk_builder_get_object(builder, "account_create_password_entry"));
    GtkWidget *account_create_check_entry = GTK_WIDGET(gtk_builder_get_object(builder, "account_create_check_entry"));
    GtkWidget *account_create_check_error_label = GTK_WIDGET(gtk_builder_get_object(builder, "account_create_check_error_label"));

    const gchar *name = gtk_entry_get_text(GTK_ENTRY(account_create_name_entry));
    const gchar *password = gtk_entry_get_text(GTK_ENTRY(account_create_password_entry));
    const gchar *password_check = gtk_entry_get_text(GTK_ENTRY(account_create_check_entry));

    const int checked = account_create_check(name, password, password_check);

    if (checked == 1) { // 账户创建成功
        gtk_label_set_text(GTK_LABEL(account_create_check_error_label), "");

        const gchar *message = "\n账户创建成功！\n您的账户是：";
        gchar *link_message = g_strdup_printf("%s%d", message, account.account);
        gint response = show_message_box(builder, link_message);

        // 释放内存
        g_free(link_message);

        // 点击“OK”
        if (response == GTK_RESPONSE_ACCEPT || response == GTK_RESPONSE_OK || response == GTK_RESPONSE_YES) {
            // 返回账户验证窗口
            open_new_window(login_windows.account_create_window, login_windows.account_check_window, builder);
        }
    }else {
        if (checked == 0) { // 文件打开失败
            gint response = show_message_box(builder, "系统错误！");

            // 点击“OK”
            if (response == GTK_RESPONSE_ACCEPT || response == GTK_RESPONSE_OK || response == GTK_RESPONSE_YES) {
                gtk_main_quit();
                exit(0);
            }
        }else if (checked == 2) {   // 用户已存在
            gtk_label_set_text(GTK_LABEL(account_create_check_error_label), "账户已存在");
            gtk_entry_set_text(GTK_ENTRY(account_create_name_entry), "");
        }else if (checked == 3) {   // 密码验证失败
            gtk_label_set_text(GTK_LABEL(account_create_check_error_label), "密码验证失败");
        }else if (checked == 4) {   // 非法字符
            gtk_label_set_text(GTK_LABEL(account_create_check_error_label), "非法字符");
            gtk_entry_set_text(GTK_ENTRY(account_create_name_entry), "");
        }else if (checked == 5) {   // 空值
            gtk_label_set_text(GTK_LABEL(account_create_check_error_label), "姓名、密码不能为空");
        }else if (checked == 6) {   // 密码长度不足
            gtk_label_set_text(GTK_LABEL(account_create_check_error_label), "密码长度不足");
        }
        gtk_entry_set_text(GTK_ENTRY(account_create_check_entry), "");
        gtk_entry_set_text(GTK_ENTRY(account_create_password_entry), "");
    }
}


// 开户取消按钮按下
void account_create_cancel_button_clicked(GtkButton *button, gpointer data) {
    GtkBuilder *builder = GTK_BUILDER (data);

    // 返回账户验证窗口
    open_new_window(login_windows.account_create_window, login_windows.account_check_window, builder);
}


// 初始化密码验证窗口
void init_password_check_window(GtkBuilder *builder) {
    GtkWidget *password_check_entry = GTK_WIDGET(gtk_builder_get_object(builder, "password_check_entry"));
    GtkWidget *password_check_timer_label = GTK_WIDGET(gtk_builder_get_object(builder, "password_check_timer_label"));
    GtkWidget *password_check_error_label = GTK_WIDGET(gtk_builder_get_object(builder, "password_check_error_label"));

    gtk_entry_set_text(GTK_ENTRY(password_check_entry), "");
    gtk_label_set_text(GTK_LABEL(password_check_timer_label), "");  // 计时器Label
    gtk_label_set_text(GTK_LABEL(password_check_error_label), "");

    // 设置计时器
    
}


// 密码验证按钮按下
void password_check_button_clicked(GtkButton *button, gpointer data) {
    GtkBuilder *builder = GTK_BUILDER (data);

    GtkWidget *password_check_entry = GTK_WIDGET(gtk_builder_get_object(builder, "password_check_entry"));
    GtkWidget *password_check_error_label = GTK_WIDGET(gtk_builder_get_object(builder, "password_check_error_label"));

    const gchar *password = gtk_entry_get_text(GTK_ENTRY(password_check_entry));
    int checked = login_password_check(password);

    if (checked == 0) { // 登录成功

        show_message_box(builder, "登录成功");

        init_main_window(builder);
        init_main_signal(builder);

        // 释放登录界面窗口
        gtk_widget_destroy(login_windows.account_create_window);
        gtk_widget_destroy(login_windows.account_check_window);
        gtk_widget_destroy(login_windows.password_check_window);

        // 打开主界面窗口
        open_new_window(NULL, main_windows.menu_window, builder);
    }else if (checked == -1) {  // 空值
        gtk_label_set_text(GTK_LABEL(password_check_error_label), "请输入密码");
    }else
        {
        if (checked == 3) { // 三次验证失败
            gint response = show_message_box(builder, "密码错误！\n登录次数已用尽！");
            if (response == GTK_RESPONSE_ACCEPT || response == GTK_RESPONSE_OK || response == GTK_RESPONSE_YES) {
                // 关闭程序
                gtk_main_quit();
                exit(0);
            }
        }

        const gchar *error_message = "密码错误！剩余登录次数：";
        gchar *link_message = g_strdup_printf("%s%d", error_message, 3 - checked);
        gtk_label_set_text(GTK_LABEL(password_check_error_label), link_message);
        gtk_entry_set_text(GTK_ENTRY(password_check_entry), "");

        g_free(link_message);
    }
}







// 初始化主界面窗口
void init_main_window(GtkBuilder *builder) {
    gtk_builder_add_from_file(builder, "C:\\Users\\Worker\\Desktop\\Glade Project\\main_window.glade", 0);

    // 初始化窗口指针
    main_windows.menu_window = GTK_WIDGET(gtk_builder_get_object(builder, "menu_window"));
    main_windows.deposit_window = GTK_WIDGET(gtk_builder_get_object(builder, "deposit_window"));
    main_windows.withdraw_window = GTK_WIDGET(gtk_builder_get_object(builder, "withdraw_window"));
    main_windows.transfer_window = GTK_WIDGET(gtk_builder_get_object(builder, "transfer_window"));
    main_windows.transfer_balance_window = GTK_WIDGET(gtk_builder_get_object(builder, "transfer_balance_window"));
    main_windows.transfer_password_window = GTK_WIDGET(gtk_builder_get_object(builder, "transfer_password_window"));
    main_windows.change_password_window = GTK_WIDGET(gtk_builder_get_object(builder, "change_password_window"));
    main_windows.message_box = GTK_WIDGET(gtk_builder_get_object(builder, "message_box"));

    // 绑定窗口及其初始化函数
    g_object_set_data(G_OBJECT(main_windows.menu_window), "init_func", (gpointer)init_menu_window);
    g_object_set_data(G_OBJECT(main_windows.deposit_window), "init_func", (gpointer)init_deposit_window);
    g_object_set_data(G_OBJECT(main_windows.withdraw_window), "init_func", (gpointer)init_withdraw_window);
    g_object_set_data(G_OBJECT(main_windows.transfer_window), "init_func", (gpointer)init_transfer_window);
    g_object_set_data(G_OBJECT(main_windows.transfer_balance_window), "init_func", (gpointer)init_transfer_balance_window);
    g_object_set_data(G_OBJECT(main_windows.transfer_password_window), "init_func", (gpointer)init_transfer_password_window);
    g_object_set_data(G_OBJECT(main_windows.change_password_window), "init_func", (gpointer)init_change_password_window);
}


// 绑定主界面窗口信号
void init_main_signal(GtkBuilder *builder) {
    // 重新连接destroy信号至主界面窗口
    g_signal_handlers_disconnect_by_func(login_windows.account_check_window, gtk_main_quit, NULL);
    g_signal_connect(main_windows.menu_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // menu_window 窗口信号
    GtkWidget *change_password_button = GTK_WIDGET(gtk_builder_get_object(builder, "change_password_button"));
    GtkWidget *deposit_button = GTK_WIDGET(gtk_builder_get_object(builder, "deposit_button"));
    GtkWidget *withdraw_button = GTK_WIDGET(gtk_builder_get_object(builder, "withdraw_button"));
    GtkWidget *transfer_button = GTK_WIDGET(gtk_builder_get_object(builder, "transfer_button"));
    GtkWidget *quit_button = GTK_WIDGET(gtk_builder_get_object(builder, "quit_button"));

    g_signal_connect(change_password_button, "clicked", G_CALLBACK(change_password_button_clicked), builder);
    g_signal_connect(deposit_button, "clicked", G_CALLBACK(deposit_button_clicked), builder);
    g_signal_connect(withdraw_button, "clicked", G_CALLBACK(withdraw_button_clicked), builder);
    g_signal_connect(transfer_button, "clicked", G_CALLBACK(transfer_button_clicked), builder);
    g_signal_connect(quit_button, "clicked", G_CALLBACK(quit_button_clicked), NULL);

    // withdraw_window 窗口信号
    GtkWidget *withdraw_accept_button = GTK_WIDGET(gtk_builder_get_object(builder, "withdraw_accept_button"));
    GtkWidget *withdraw_cancel_button = GTK_WIDGET(gtk_builder_get_object(builder, "withdraw_cancel_button"));

    g_signal_connect(withdraw_accept_button, "clicked", G_CALLBACK(withdraw_accept_button_clicked), builder);
    g_signal_connect(withdraw_cancel_button, "clicked", G_CALLBACK(withdraw_cancel_button_clicked), builder);

    // deposit_window 窗口信号
    GtkWidget *deposit_accept_button = GTK_WIDGET(gtk_builder_get_object(builder, "deposit_accept_button"));
    GtkWidget *deposit_cancel_button = GTK_WIDGET(gtk_builder_get_object(builder, "deposit_cancel_button"));

    g_signal_connect(deposit_accept_button, "clicked", G_CALLBACK(deposit_accept_button_clicked), builder);
    g_signal_connect(deposit_cancel_button, "clicked", G_CALLBACK(deposit_cancel_button_clicked), builder);

    // change_password_window 窗口信号
    GtkWidget *change_password_accept_button = GTK_WIDGET(gtk_builder_get_object(builder, "change_password_accept_button"));
    GtkWidget *change_password_cancel_button = GTK_WIDGET(gtk_builder_get_object(builder, "change_password_cancel_button"));

    g_signal_connect(change_password_accept_button, "clicked", G_CALLBACK(change_password_accept_button_clicked), builder);
    g_signal_connect(change_password_cancel_button, "clicked", G_CALLBACK(change_password_cancel_button_clicked), builder);

    // transfer_window 窗口信号
    GtkWidget *transfer_accept_button = GTK_WIDGET(gtk_builder_get_object(builder, "transfer_accept_button"));
    GtkWidget *transfer_cancel_button = GTK_WIDGET(gtk_builder_get_object(builder, "transfer_cancel_button"));

    g_signal_connect(transfer_accept_button, "clicked", G_CALLBACK(transfer_accept_button_clicked), builder);
    g_signal_connect(transfer_cancel_button, "clicked", G_CALLBACK(transfer_cancel_button_clicked), builder);

    // transfer_balance_window 窗口信号
    GtkWidget *transfer_balance_accept_button = GTK_WIDGET(gtk_builder_get_object(builder, "transfer_balance_accept_button"));
    GtkWidget *transfer_balance_cancel_button = GTK_WIDGET(gtk_builder_get_object(builder, "transfer_balance_cancel_button"));
    GtkWidget *transfer_balance_return_button = GTK_WIDGET(gtk_builder_get_object(builder, "transfer_balance_return_button"));

    g_signal_connect(transfer_balance_accept_button, "clicked", G_CALLBACK(transfer_balance_accept_button_clicked), builder);
    g_signal_connect(transfer_balance_cancel_button, "clicked", G_CALLBACK(transfer_balance_cancel_button_clicked), builder);
    g_signal_connect(transfer_balance_return_button, "clicked", G_CALLBACK(transfer_balance_return_button_clicked), builder);

    // transfer_password_window 窗口信号
    GtkWidget *transfer_password_accept_button = GTK_WIDGET(gtk_builder_get_object(builder, "transfer_password_accept_button"));
    GtkWidget *transfer_password_cancel_button = GTK_WIDGET(gtk_builder_get_object(builder, "transfer_password_cancel_button"));
    GtkWidget *transfer_password_return_button = GTK_WIDGET(gtk_builder_get_object(builder, "transfer_password_return_button"));

    g_signal_connect(transfer_password_accept_button, "clicked", G_CALLBACK(transfer_password_accept_button_clicked), builder);
    g_signal_connect(transfer_password_cancel_button, "clicked", G_CALLBACK(transfer_password_cancel_button_clicked), builder);
    g_signal_connect(transfer_password_return_button, "clicked", G_CALLBACK(transfer_password_return_button_clicked), builder);
}

// 初始化主菜单窗口
void init_menu_window(GtkBuilder *builder) {
    GtkWidget *login_user_message_label = GTK_WIDGET(gtk_builder_get_object(builder, "login_user_message_label"));

    const gchar *user_name = "姓名：";
    const gchar *user_balance = "余额：";
    gchar *link_message = g_strdup_printf("%s%s\n%s%.2lf", user_name, account.name, user_balance, account.balance);
    gtk_label_set_text(GTK_LABEL(login_user_message_label), link_message);

    g_free(link_message);
}


// 退出按钮按下
void quit_button_clicked(GtkButton *button, gpointer data) {
    gtk_main_quit();
}







// 初始化存款窗口
void init_deposit_window(GtkBuilder *builder) {
    GtkWidget *deposit_entry = GTK_WIDGET(gtk_builder_get_object(builder, "deposit_entry"));
    GtkWidget *deposit_error_label = GTK_WIDGET(gtk_builder_get_object(builder, "deposit_error_label"));

    gtk_entry_set_text(GTK_ENTRY(deposit_entry), "");
    gtk_label_set_text(GTK_LABEL(deposit_error_label), "");
}

// 存款按钮按下
void deposit_button_clicked(GtkButton *button, gpointer data) {
    GtkBuilder *builder = GTK_BUILDER (data);

    open_new_window(main_windows.menu_window, main_windows.deposit_window, builder);
}

// 存款确认按钮按下
void deposit_accept_button_clicked(GtkButton *button, gpointer data) {
    GtkBuilder *builder = GTK_BUILDER (data);
    GtkWidget *deposit_entry = GTK_WIDGET(gtk_builder_get_object(builder, "deposit_entry"));
    GtkWidget *deposit_error_label = GTK_WIDGET(gtk_builder_get_object(builder, "deposit_error_label"));

    const gchar *deposit_balance_str = gtk_entry_get_text(GTK_ENTRY(deposit_entry));
    gchar *ptr;
    double deposit_balance = strtod(deposit_balance_str, &ptr);

    if (deposit_balance <= 0) {   // 无效金额
        gtk_label_set_text(GTK_LABEL(deposit_error_label), "无效金额");
    }else {
        gchar *link_message = g_strdup_printf("确定存款:%.2lf", deposit_balance);
        gint response = show_message_box(builder, link_message);

        g_free(link_message);

        if (response == GTK_RESPONSE_ACCEPT || response == GTK_RESPONSE_OK || response == GTK_RESPONSE_YES) {   // 按下确认按钮
            int checked = withdraw(deposit_balance, 1);

            if (checked != 1) {
                show_message_box(builder, "系统错误\n存款失败");
                gtk_main_quit();
                exit(0);
            }

            show_message_box(builder, "存款成功");

            // 回到主菜单
            open_new_window(main_windows.deposit_window, main_windows.menu_window, builder);

        }else if (response == GTK_RESPONSE_REJECT || response == GTK_RESPONSE_NO) {
            gtk_label_set_text(GTK_LABEL(deposit_error_label), "");
        }
    }

    gtk_entry_set_text(GTK_ENTRY(deposit_entry), "");
}

// 存款取消按钮按下
void deposit_cancel_button_clicked(GtkButton *button, gpointer data) {
    GtkBuilder *builder = GTK_BUILDER (data);

    open_new_window(main_windows.deposit_window, main_windows.menu_window, builder);
}







// 初始化取款窗口
void init_withdraw_window(GtkBuilder *builder) {
    GtkWidget *withdraw_entry = GTK_WIDGET(gtk_builder_get_object(builder, "withdraw_entry"));
    GtkWidget *withdraw_error_label = GTK_WIDGET(gtk_builder_get_object(builder, "withdraw_error_label"));

    gtk_entry_set_text(GTK_ENTRY(withdraw_entry), "");
    gtk_label_set_text(GTK_LABEL(withdraw_error_label), "");
}

// 取款按钮按下
void withdraw_button_clicked(GtkButton *button, gpointer data) {
    GtkBuilder *builder = GTK_BUILDER (data);

    open_new_window(main_windows.menu_window, main_windows.withdraw_window, builder);
}

// 取款确认按钮按下
void withdraw_accept_button_clicked(GtkButton *button, gpointer data) {
    GtkBuilder *builder = GTK_BUILDER (data);
    GtkWidget *withdraw_entry = GTK_WIDGET(gtk_builder_get_object(builder, "withdraw_entry"));
    GtkWidget *withdraw_error_label = GTK_WIDGET(gtk_builder_get_object(builder, "withdraw_error_label"));

    const gchar *withdraw_balance_str = gtk_entry_get_text(GTK_ENTRY(withdraw_entry));
    gchar *ptr;
    double with_draw_balance = strtod(withdraw_balance_str, &ptr);

    if (with_draw_balance <= 0) {   // 无效金额
        gtk_label_set_text(GTK_LABEL(withdraw_error_label), "无效金额");
    }else if (with_draw_balance > account.balance) {    // 余额不足
        show_message_box(builder, "余额不足");

        gtk_label_set_text(GTK_LABEL(withdraw_error_label), "");
    }else {
        gchar *link_message = g_strdup_printf("确定取款:%.2lf", with_draw_balance);
        gint response = show_message_box(builder, link_message);

        g_free(link_message);

        if (response == GTK_RESPONSE_ACCEPT || response == GTK_RESPONSE_OK || response == GTK_RESPONSE_YES) {   // 按下确认按钮
            int checked = withdraw(with_draw_balance, 0);

            if (checked != 1) {
                show_message_box(builder, "系统错误\n取款失败");
                gtk_main_quit();
                exit(0);
            }

            show_message_box(builder, "取款成功");

            // 回到主菜单
            open_new_window(main_windows.withdraw_window, main_windows.menu_window, builder);

        }else if (response == GTK_RESPONSE_REJECT || response == GTK_RESPONSE_NO) { // 按下取消按钮
            gtk_label_set_text(GTK_LABEL(withdraw_error_label), "");
        }
    }

    gtk_entry_set_text(GTK_ENTRY(withdraw_entry), "");
}

// 取款取消按钮按下
void withdraw_cancel_button_clicked(GtkButton *button, gpointer data) {
    GtkBuilder *builder = GTK_BUILDER (data);

    open_new_window(main_windows.withdraw_window, main_windows.menu_window, builder);
}






// 初始化转账窗口
void init_transfer_window(GtkBuilder *builder) {
    GtkWidget *transfer_entry = GTK_WIDGET(gtk_builder_get_object(builder, "transfer_entry"));
    GtkWidget *transfer_error_label = GTK_WIDGET(gtk_builder_get_object(builder, "transfer_error_label"));

    gtk_entry_set_text(GTK_ENTRY(transfer_entry), "");
    gtk_label_set_text(GTK_LABEL(transfer_error_label), "");
}

// 转账按钮按下
void transfer_button_clicked(GtkButton *button, gpointer data) {
    GtkBuilder *builder = GTK_BUILDER (data);

    open_new_window(main_windows.menu_window, main_windows.transfer_window, builder);
}

// 转账账户确认按钮按下
void transfer_accept_button_clicked(GtkButton *button, gpointer data) {
    GtkBuilder *builder = GTK_BUILDER (data);
    GtkWidget *transfer_error_label = GTK_WIDGET(gtk_builder_get_object(builder, "transfer_error_label"));
    GtkWidget *transfer_entry = GTK_WIDGET(gtk_builder_get_object(builder, "transfer_entry"));

    const gchar *transfer_account_str = gtk_entry_get_text(GTK_ENTRY(transfer_entry));
    gchar *ptr = NULL;
    int transfer_account = strtol(transfer_account_str, &ptr, 10);

    if (transfer_account <= 0) {
        gtk_label_set_text(GTK_LABEL(transfer_error_label), "请输入正确的账号");
    }else if (account.account == transfer_account) {
        gtk_label_set_text(GTK_LABEL(transfer_error_label), "不能向自己转账");
    }else {
        Account *transfer = get_transfer_name(transfer_account);

        if (transfer == NULL) {
            gtk_label_set_text(GTK_LABEL(transfer_error_label), "账户不存在");
        }else {
            gchar *link_message = g_strdup_printf("确定向\"%d %s\"转账", transfer->account, transfer->name);
            gint response = show_message_box(builder, link_message);

            g_free(link_message);

            if (response == GTK_RESPONSE_ACCEPT || response == GTK_RESPONSE_OK || response == GTK_RESPONSE_YES) {   // 按下确认按钮
                g_object_set_data(G_OBJECT(main_windows.transfer_window), "transfer", transfer);
                open_new_window(main_windows.transfer_window, main_windows.transfer_balance_window, builder);
            }else if (response == GTK_RESPONSE_REJECT || response == GTK_RESPONSE_NO) { // 按下取消按钮
                g_free(transfer);
            }
        }
    }
    gtk_entry_set_text(GTK_ENTRY(transfer_entry), "");
}

// 转账账户取消按钮按下
void transfer_cancel_button_clicked(GtkButton *button, gpointer data) {
    GtkBuilder *builder = GTK_BUILDER (data);

    open_new_window(main_windows.transfer_window, main_windows.menu_window, builder);
}

// 初始化转账金额窗口
void init_transfer_balance_window(GtkBuilder *builder) {
    GtkWidget *transfer_balance_entry = GTK_WIDGET(gtk_builder_get_object(builder, "transfer_balance_entry"));
    GtkWidget *transfer_balance_error_label = GTK_WIDGET(gtk_builder_get_object(builder, "transfer_balance_error_label"));

    gtk_entry_set_text(GTK_ENTRY(transfer_balance_entry), "");
    gtk_label_set_text(GTK_LABEL(transfer_balance_error_label), "");
}

// 转账金额确认按钮按下
void transfer_balance_accept_button_clicked(GtkButton *button, gpointer data) {
    GtkBuilder *builder = GTK_BUILDER (data);
    GtkWidget *transfer_balance_error_label = GTK_WIDGET(gtk_builder_get_object(builder, "transfer_balance_error_label"));
    GtkWidget *transfer_balance_entry = GTK_WIDGET(gtk_builder_get_object(builder, "transfer_balance_entry"));

    const gchar *transfer_balance_str = gtk_entry_get_text(GTK_ENTRY(transfer_balance_entry));
    gchar *ptr = NULL;
    double transfer_balance = strtod(transfer_balance_str, &ptr);
    Account *transfer = g_object_get_data(G_OBJECT(main_windows.transfer_window), "transfer");

    if (transfer_balance <= 0) {
        gtk_label_set_text(GTK_LABEL(transfer_balance_error_label), "无效金额");
    }else {
        gchar *link_message = g_strdup_printf("确定转账 %.2lf 元", transfer_balance);
        gint response = show_message_box(builder, link_message);

        g_free(link_message);

        if (response == GTK_RESPONSE_ACCEPT || response == GTK_RESPONSE_OK || response == GTK_RESPONSE_YES) {   // 按下确认按钮
            transfer->balance = transfer_balance;
            g_object_set_data(G_OBJECT(main_windows.transfer_window), "transfer", transfer);
            open_new_window(main_windows.transfer_balance_window, main_windows.transfer_password_window, builder);
        }else if (response == GTK_RESPONSE_REJECT || response == GTK_RESPONSE_NO) { // 按下取消按钮
            g_free(transfer);
        }
    }

    gtk_entry_set_text(GTK_ENTRY(transfer_balance_entry), "");
}

// 转账金额返回按钮按下
void transfer_balance_return_button_clicked(GtkButton *button, gpointer data) {
    GtkBuilder *builder = GTK_BUILDER (data);

    open_new_window(main_windows.transfer_balance_window, main_windows.menu_window, builder);
}

// 转账金额取消按钮按下
void transfer_balance_cancel_button_clicked(GtkButton *button, gpointer data) {
    GtkBuilder *builder = GTK_BUILDER (data);

    open_new_window(main_windows.transfer_balance_window, main_windows.transfer_window, builder);
}

// 初始化转账密码验证窗口
void init_transfer_password_window(GtkBuilder *builder) {
    GtkWidget *transfer_password_entry = GTK_WIDGET(gtk_builder_get_object(builder, "transfer_password_entry"));
    GtkWidget *transfer_password_error_label = GTK_WIDGET(gtk_builder_get_object(builder, "transfer_password_error_label"));

    gtk_entry_set_text(GTK_ENTRY(transfer_password_entry), "");
    gtk_label_set_text(GTK_LABEL(transfer_password_error_label), "");
}

// 转账密码验证确认按钮按下
void transfer_password_accept_button_clicked(GtkButton *button, gpointer data) {
    GtkBuilder *builder = GTK_BUILDER (data);
    GtkWidget *transfer_password_error_label = GTK_WIDGET(gtk_builder_get_object(builder, "transfer_password_error_label"));
    GtkWidget *transfer_password_entry = GTK_WIDGET(gtk_builder_get_object(builder, "transfer_password_entry"));

    const gchar *password = gtk_entry_get_text(GTK_ENTRY(transfer_password_entry));
    int checked = transfer_password_check(password);

    if (checked == 1) {  // 密码正确
        Account *transfer = g_object_get_data(G_OBJECT(main_windows.transfer_window), "transfer");

        gchar *link_message = g_strdup_printf("确定向\"%d %s\"转账\"%.2lf\"元", transfer->account, transfer->name, transfer->balance);
        gint response = show_message_box(builder, link_message);

        g_free(link_message);

        if (response == GTK_RESPONSE_ACCEPT || response == GTK_RESPONSE_OK || response == GTK_RESPONSE_YES) {   // 按下确认按钮
            int ret = transfer_func(transfer->account, transfer->balance);

            g_free(transfer);

            if (ret == 1) {
                show_message_box(builder, "转账成功");
            }else {
                show_message_box(builder, "转账失败");
            }

            open_new_window(main_windows.transfer_password_window, main_windows.menu_window, builder);
        }else if (response == GTK_RESPONSE_REJECT || response == GTK_RESPONSE_NO) { // 按下取消按钮
            g_free(transfer);
            open_new_window(main_windows.transfer_password_window, main_windows.menu_window, builder);
        }

    }else {
        if (checked == 0) { // 空值
            gtk_label_set_text(GTK_LABEL(transfer_password_error_label), "请输入密码");
        }else
            gtk_label_set_text(GTK_LABEL(transfer_password_error_label), "密码错误");
    }

    gtk_entry_set_text(GTK_ENTRY(transfer_password_entry), "");
}

// 转账密码验证取消按钮按下
void transfer_password_cancel_button_clicked(GtkButton *button, gpointer data) {
    GtkBuilder *builder = GTK_BUILDER (data);

    open_new_window(main_windows.transfer_password_window, main_windows.transfer_balance_window, builder);
}

// 转账密码验证返回按钮按下
void transfer_password_return_button_clicked(GtkButton *button, gpointer data) {
    GtkBuilder *builder = GTK_BUILDER (data);

    open_new_window(main_windows.transfer_password_window, main_windows.menu_window, builder);
}






// 初始化更改密码窗口
void init_change_password_window(GtkBuilder *builder) {
    GtkWidget *old_password_entry = GTK_WIDGET(gtk_builder_get_object(builder, "old_password_entry"));
    GtkWidget *new_password_entry = GTK_WIDGET(gtk_builder_get_object(builder, "new_password_entry"));
    GtkWidget *new_password_check_entry = GTK_WIDGET(gtk_builder_get_object(builder, "new_password_check_entry"));
    GtkWidget *change_password_error_label = GTK_WIDGET(gtk_builder_get_object(builder, "change_password_error_label"));

    gtk_entry_set_text(GTK_ENTRY(old_password_entry), "");
    gtk_entry_set_text(GTK_ENTRY(new_password_entry), "");
    gtk_entry_set_text(GTK_ENTRY(new_password_check_entry), "");
    gtk_label_set_text(GTK_LABEL(change_password_error_label), "");
}



// 更改密码按钮按下
void change_password_button_clicked(GtkButton *button, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);

    open_new_window(main_windows.menu_window, main_windows.change_password_window, builder);
}


// 更改密码确认按钮按下
void change_password_accept_button_clicked(GtkButton *button, gpointer data) {
    GtkBuilder *builder = GTK_BUILDER(data);
    GtkWidget *old_password_entry = GTK_WIDGET(gtk_builder_get_object(builder, "old_password_entry"));
    GtkWidget *new_password_entry = GTK_WIDGET(gtk_builder_get_object(builder, "new_password_entry"));
    GtkWidget *new_password_check_entry = GTK_WIDGET(gtk_builder_get_object(builder, "new_password_check_entry"));
    GtkWidget *change_password_error_label = GTK_WIDGET(gtk_builder_get_object(builder, "change_password_error_label"));

    const gchar *old_password = gtk_entry_get_text(GTK_ENTRY(old_password_entry));
    const gchar *new_password = gtk_entry_get_text(GTK_ENTRY(new_password_entry));
    const gchar *new_password_check = gtk_entry_get_text(GTK_ENTRY(new_password_check_entry));

    int checked = change_password_check(old_password, new_password, new_password_check);

    if (checked == 1) { // 密码更改成功
        gtk_label_set_text(GTK_LABEL(change_password_error_label), "");

        gint response = show_message_box(builder, "密码更改成功");

        // 点击“OK”
        if (response == GTK_RESPONSE_ACCEPT || response == GTK_RESPONSE_OK || response == GTK_RESPONSE_YES) {
            // 返回主窗口
            open_new_window(main_windows.change_password_window, main_windows.menu_window, builder);
        }
    }else {
        if (checked == 0) { // 文件打开失败
            gint response = show_message_box(builder, "系统错误！");

            // 点击“OK”
            if (response == GTK_RESPONSE_ACCEPT || response == GTK_RESPONSE_OK || response == GTK_RESPONSE_YES) {
                gtk_main_quit();
                exit(0);
            }
        }else if (checked == 2) {   // 空值
            gtk_label_set_text(GTK_LABEL(change_password_error_label), "请输入内容");
        }else if (checked == 3) {   // 原密码错误
            gtk_label_set_text(GTK_LABEL(change_password_error_label), "原密码错误");
            gtk_entry_set_text(GTK_ENTRY(old_password_entry), "");
        }else if (checked == 4) {   // 非法字符
            gtk_label_set_text(GTK_LABEL(change_password_error_label), "非法字符");
        }else if (checked == 5) {   // 密码验证错误
            gtk_label_set_text(GTK_LABEL(change_password_error_label), "密码验证错误");
        }else if (checked == 6) {   // 新密码与原密码相同
            gtk_label_set_text(GTK_LABEL(change_password_error_label), "新密码与原密码相同");
        }else if (checked == 7) {
            gtk_label_set_text(GTK_LABEL(change_password_error_label), "密码长度不足");
        }
        gtk_entry_set_text(GTK_ENTRY(new_password_entry), "");
        gtk_entry_set_text(GTK_ENTRY(new_password_check_entry), "");
    }
}


// 更改密码取消按钮按下
void change_password_cancel_button_clicked(GtkButton *button, gpointer data) {
    GtkBuilder *builder = GTK_BUILDER(data);

    open_new_window(main_windows.change_password_window, main_windows.menu_window, builder);
}






// 打开新窗口
void open_new_window(GtkWidget *old_window, GtkWidget *new_window, GtkBuilder *builder) {
    // 获取绑定的初始化函数
    window_init_func init_func = g_object_get_data(G_OBJECT(new_window), "init_func");

    // 如果有初始化函数, 则进行初始化
    if (init_func != NULL) {
        init_func(builder);
    }

    // 切换窗口
    if (old_window != NULL)
        gtk_widget_hide(old_window);
    gtk_window_set_position(GTK_WINDOW(new_window), GTK_WIN_POS_CENTER);
    gtk_widget_show_all(new_window);
}



// 主函数
int main (int argc, char *argv[]) {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    // 初始化gtk
    gtk_init(&argc, &argv);

    // 定义并初始化builder对象
    GtkBuilder *builder = gtk_builder_new();

    // 初始化窗口
    init_login_window(builder);
    init_login_signal(builder);

    // 打开第一个窗口
    open_new_window(NULL, login_windows.account_check_window, builder);

    // 进入gtk主循环
    gtk_main();

    // 释放builder对象
    g_object_unref(builder);

    return 0;
}