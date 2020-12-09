#include "act_blanknums_wnd.h"
#include "ui_act_blanknums_wnd.h"
#include <QMessageBox>

act_blanknums_wnd::act_blanknums_wnd(QStringList &blanknums, QWidget *parent) :
    blanknums(blanknums),
    QDialog(parent),
    ui(new Ui::act_blanknums_wnd)
{
    ui->setupUi(this);

    on_rb_list_clicked(ui->rb_list->isChecked());
}

act_blanknums_wnd::~act_blanknums_wnd() {
    delete ui;
}

void act_blanknums_wnd::on_bn_cansel_clicked() {
    if (QMessageBox::question(this, "Просто закрыть окно?", "Вы действительно хотите закрыть окно без изменения данных?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes)
        reject();
}

void act_blanknums_wnd::on_rb_nums_clicked(bool checked) {
    ui->spin_min->setEnabled(checked);
    ui->spin_max->setEnabled(checked);
    ui->te_blanknums->setEnabled(!checked);
}

void act_blanknums_wnd::on_rb_list_clicked(bool checked) {
    ui->spin_min->setEnabled(!checked);
    ui->spin_max->setEnabled(!checked);
    ui->te_blanknums->setEnabled(checked);
}

void act_blanknums_wnd::on_spin_min_editingFinished() {
    if (ui->spin_max->value()<ui->spin_min->value())
        ui->spin_max->setValue(ui->spin_min->value());
    int dif = ui->spin_max->value() - ui->spin_min->value() + 1;
    ui->lab_dif->setText(QString::number(dif));
}

void act_blanknums_wnd::on_spin_max_editingFinished() {
    if (ui->spin_max->value()<ui->spin_min->value())
        ui->spin_min->setValue(ui->spin_max->value());
    int dif = ui->spin_max->value() - ui->spin_min->value() + 1;
    ui->lab_dif->setText(QString::number(dif));
}


void act_blanknums_wnd::on_bn_ok_clicked()  {
    if (ui->rb_nums->isChecked()) {
        int dif = ui->spin_max->value() - ui->spin_min->value() + 1;
        if (dif==0 || dif>1000) {
            QMessageBox::warning(this, "Ошибка в данных?", "Надо задать диапазон номеров бланков больше 0 и меньше 1000 штук!");
            return;
        }
        ui->te_blanknums->clear();
        for (int i=ui->spin_min->value(); i<=ui->spin_max->value(); i++)
            ui->te_blanknums->append(QString::number(i));
    }
    if (ui->te_blanknums->toPlainText().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Нет данных", "Сначала задайте список номеров бланков полисов");
        return;
    }

    QString str_blanknums = ui->te_blanknums->toPlainText().trimmed();
    str_blanknums = str_blanknums.replace(" ","").replace(",","\n").replace(";","\n").replace(".","\n").replace("\t","\n");
    str_blanknums = str_blanknums.simplified();

    blanknums.clear();
    blanknums.append(str_blanknums.split(" "));

    accept();
}
