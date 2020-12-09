#include "change_error_wnd.h"
#include "ui_change_error_wnd.h"

change_error_wnd::change_error_wnd(s_data_error &data_error, QWidget *parent) :
    data_error(data_error), QDialog(parent), ui(new Ui::change_error_wnd)
{
    ui->setupUi(this);

    ui->combo_event->clear();
    ui->combo_event->addItem(" - не определено - ", "");
    ui->combo_event->addItem("(E) ошибка - запись отвергнута", "E");
    ui->combo_event->addItem("(W) предупреждение - запись принята", "W");
    ui->combo_event->addItem("(У) принято при соблюдении условий", "У");

    if (data_error.id>=0) {
        ui->spin_code->setValue(data_error.code);
        ui->combo_event->setCurrentIndex(ui->combo_event->findData(data_error.event));
        ui->error_text->setText(data_error.text);
        ui->error_comment->setText(data_error.comment);
    }
}

change_error_wnd::~change_error_wnd() {
    delete ui;
}

void change_error_wnd::on_bn_cansel_clicked() {
    reject();
}

void change_error_wnd::on_bn_save_clicked() {
    data_error.code = ui->spin_code->value();
    data_error.event = ui->combo_event->currentData().toString();
    data_error.text = ui->error_text->toPlainText();
    data_error.comment = ui->error_comment->toPlainText();
    accept();
}
