#include "info_pers_doubles_wnd.h"
#include "ui_info_pers_doubles_wnd.h"

info_pers_doubles_wnd::info_pers_doubles_wnd(QString msg, int &res, bool can_stop, bool can_arch, QWidget *parent) :
    msg(msg), res(res), can_stop(can_stop), can_arch(can_arch), QDialog(parent), ui(new Ui::info_pers_doubles_wnd)
{
    ui->setupUi(this);

    ui->bn_ok->setVisible( this->can_stop );
    ui->bn_cansel->setVisible( this->can_stop );
    ui->bn_archive->setVisible( this->can_stop && this->can_arch );

    ui->te_msg->setText(this->msg);
}

info_pers_doubles_wnd::~info_pers_doubles_wnd() {
    delete ui;
}
void info_pers_doubles_wnd::on_bn_ok_clicked() {
    res = 1;
    accept();
}
void info_pers_doubles_wnd::on_bn_close_clicked() {
    res = 0;
    close();
}
void info_pers_doubles_wnd::on_bn_cansel_clicked() {
    res = 0;
    reject();
}
void info_pers_doubles_wnd::on_bn_archive_clicked() {
    res = -1;
    reject();
}
