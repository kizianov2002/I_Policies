#include "info_not_sended_wnd.h"
#include "ui_info_not_sended_wnd.h"

info_not_sended_wnd::info_not_sended_wnd(QString msg, QWidget *parent) :
    msg(msg), QDialog(parent), ui(new Ui::info_not_sended_wnd)
{
    ui->setupUi(this);

    ui->te_msg->setText(this->msg);
}

info_not_sended_wnd::~info_not_sended_wnd() {
    delete ui;
}
void info_not_sended_wnd::on_bn_close_clicked() {
    close();
}
