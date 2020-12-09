#include "show_text_wnd.h"
#include "ui_show_text_wnd.h"

show_text_wnd::show_text_wnd(QString header, QString text, bool f_readonly, QWidget *parent) :
    header(header),
    text(text),
    f_readonly(f_readonly),
    QDialog(parent),
    ui(new Ui::show_text_wnd)
{
    ui->setupUi(this);

    this->setWindowTitle(header);
    ui->textEdit->setReadOnly(f_readonly);
    ui->textEdit->setText(text);
}

show_text_wnd::~show_text_wnd() {
    delete ui;
}

void show_text_wnd::on_bn_close_clicked() {
    close();
}
