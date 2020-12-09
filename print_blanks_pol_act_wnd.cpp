#include "print_blanks_pol_act_wnd.h"
#include "ui_print_blanks_pol_act_wnd.h"

print_blanks_pol_act_wnd::print_blanks_pol_act_wnd(QSqlDatabase &db, s_data_blanks_pol_act &data_act, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    db(db),
    data_act(data_act),
    data_app(data_app),
    settings(settings),
    QDialog(parent),
    ui(new Ui::print_blanks_pol_act_wnd)
{
    ui->setupUi(this);

    ui->ln_filial_city->setText(data_app.filial_city);
    ui->ln_smo_name->setText(data_app.smo_name);
    ui->ln_filial_name->setText(data_app.filial_name);
    ui->ln_filial_name_rp->setText(data_app.filial_name_rp);

    ui->ln_act_num->setText(data_act.act_num);
    ui->date_act->setDate(data_act.act_date);
    ui->ln_sender->setText(data_act.sender);
    ui->ln_sender_post->setText(data_act.sender_post);
    ui->ln_sender_fio->setText(data_act.sender_fio);
    ui->ln_obtainer->setText(data_act.obtainer);
    ui->ln_obtainer_post->setText(data_act.obtainer_post);
    ui->ln_obtainer_fio->setText(data_act.obtainer_fio);
}

print_blanks_pol_act_wnd::~print_blanks_pol_act_wnd() {
    delete ui;
}

void print_blanks_pol_act_wnd::on_bn_cansel_clicked() {
    reject();
}

QString print_blanks_pol_act_wnd::month_to_str(QDate date) {
    QString res;
    switch (date.month()) {
    case 1:
        res += "января ";
        break;
    case 2:
        res += "февраля ";
        break;
    case 3:
        res += "марта ";
        break;
    case 4:
        res += "апреля ";
        break;
    case 5:
        res += "мая ";
        break;
    case 6:
        res += "июня ";
        break;
    case 7:
        res += "июля ";
        break;
    case 8:
        res += "августа ";
        break;
    case 9:
        res += "сентября ";
        break;
    case 10:
        res += "октября ";
        break;
    case 11:
        res += "ноября ";
        break;
    case 12:
        res += "декабря ";
        break;
    default:
        break;
    }
    return res;
}

void print_blanks_pol_act_wnd::on_bn_edit_clicked(bool checked) {
    if (checked) {
        ui->bn_edit->setText("Сохранить");

        ui->lab_filial_city->setEnabled(true);
        ui->ln_filial_city->setEnabled(true);
        ui->lab_smo_name->setEnabled(true);
        ui->ln_smo_name->setEnabled(true);
        ui->lab_filial_name->setEnabled(true);
        ui->ln_filial_name->setEnabled(true);
        ui->lab_filial_name_rp->setEnabled(true);
        ui->ln_filial_name_rp->setEnabled(true);

        ui->lab_act_num->setEnabled(true);
        ui->lab_act_date->setEnabled(true);
        ui->ln_act_num->setEnabled(true);
        ui->date_act->setEnabled(true);
        ui->lab_sender->setEnabled(true);
        ui->lab_sender_fio->setEnabled(true);
        ui->lab_sender_post->setEnabled(true);
        ui->ln_sender->setEnabled(true);
        ui->ln_sender_fio->setEnabled(true);
        ui->ln_sender_post->setEnabled(true);
        ui->lab_obtainer->setEnabled(true);
        ui->lab_obtainer_fio->setEnabled(true);
        ui->lab_obtainer_post->setEnabled(true);
        ui->ln_obtainer->setEnabled(true);
        ui->ln_obtainer_fio->setEnabled(true);
        ui->ln_obtainer_post->setEnabled(true);

        ui->bn_print->setEnabled(false);
    } else {
        data_app.filial_city = ui->ln_filial_city->text();
        data_app.smo_name = ui->ln_smo_name->text();
        data_app.filial_name = ui->ln_filial_name->text();
        data_app.filial_name_rp = ui->ln_filial_name_rp->text();

        data_act.act_num = ui->ln_act_num->text();
        data_act.act_num = data_act.act_num.replace("/", " / ").trimmed().simplified();
        data_act.act_date = ui->date_act->date();
        data_act.sender = ui->ln_sender->text();
        data_act.sender_post = ui->ln_sender_post->text();
        data_act.sender_fio = ui->ln_sender_fio->text();
        data_act.obtainer = ui->ln_obtainer->text();
        data_act.obtainer_post = ui->ln_obtainer_post->text();
        data_act.obtainer_fio = ui->ln_obtainer_fio->text();

        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Сохранить сделанные изменения в настройках программы?\n\n"
                                  "В случае согласия новые данные будут сохранены и останутся доступны после перезагрузки программы.\n"
                                  "В случае отказа эти изменения будут доступны только до первого перезапуска программы.",
                                  QMessageBox::Yes|QMessageBox::No,
                                  QMessageBox::No)==QMessageBox::Yes)
        {
            // [filial]
            settings.beginGroup("filial");
            settings.setValue("filial_city", data_app.filial_city.toUtf8());
            settings.setValue("smo_name", data_app.smo_name.toUtf8());
            settings.setValue("filial_name", data_app.filial_name.toUtf8());
            settings.setValue("filial_name_rp", data_app.filial_name_rp.toUtf8());
            settings.sync();
        }
        ui->bn_edit->setText("Изменить");

        ui->lab_filial_city->setEnabled(false);
        ui->ln_filial_city->setEnabled(false);
        ui->lab_smo_name->setEnabled(false);
        ui->ln_smo_name->setEnabled(false);
        ui->lab_filial_name->setEnabled(false);
        ui->ln_filial_name->setEnabled(false);
        ui->lab_filial_name_rp->setEnabled(false);
        ui->ln_filial_name_rp->setEnabled(false);

        ui->lab_act_num->setEnabled(false);
        ui->lab_act_date->setEnabled(false);
        ui->ln_act_num->setEnabled(false);
        ui->date_act->setEnabled(false);
        ui->lab_sender->setEnabled(false);
        ui->lab_sender_fio->setEnabled(false);
        ui->lab_sender_post->setEnabled(false);
        ui->ln_sender->setEnabled(false);
        ui->ln_sender_fio->setEnabled(false);
        ui->ln_sender_post->setEnabled(false);
        ui->lab_obtainer->setEnabled(false);
        ui->lab_obtainer_fio->setEnabled(false);
        ui->lab_obtainer_post->setEnabled(false);
        ui->ln_obtainer->setEnabled(false);
        ui->ln_obtainer_fio->setEnabled(false);
        ui->ln_obtainer_post->setEnabled(false);

        ui->bn_print->setEnabled(true);
    }
}

#include "qt_windows.h"
#include "qwindowdefs_win.h"
#include <shellapi.h>


void print_blanks_pol_act_wnd::on_bn_print_clicked() {
    if ( data_act.act_num==" ??? "
         || data_act.act_num=="???"
         || data_act.act_num.trimmed().isEmpty() ) {
        QMessageBox::warning(this, "Задайте номер акта", "Номер акта не может быть пустым");
        return;
    }
    accept();
}
