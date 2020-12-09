#include "print_blanks_act_wnd.h"
#include "ui_print_blanks_act_wnd.h"

print_blanks_act_wnd::print_blanks_act_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    db(db),
    data_app(data_app),
    settings(settings),
    QDialog(parent),
    ui(new Ui::print_blanks_act_wnd)
{
    ui->setupUi(this);

    ui->pan_settings->setEnabled(ui->bn_edit->isChecked());

    ui->ln_dog_num->setText(data_app.act_dog_num);
    ui->date_dog->setDate(data_app.act_dog_date);
    ui->ln_filial_chif_post->setText(data_app.act_filial_chif_post);
    ui->ln_filial_chif_fio->setText(data_app.act_filial_chif_fio);
    ui->ln_point_chif_post->setText(data_app.act_point_chif_post);
    ui->ln_point_chif_fio->setText(data_app.act_point_chif_fio);

    ui->ln_point_org_name->setText(data_app.act_point_org_name);
    ui->ln_point_address->setText(data_app.act_point_address);
    ui->ln_point_requisits_1->setText(data_app.act_point_requisits_1);
    ui->ln_point_requisits_2->setText(data_app.act_point_requisits_2);
    ui->ln_point_okpo->setText(data_app.act_point_okpo);

    ui->ln_filial_org_name->setText(data_app.act_filial_org_name);
    ui->ln_filial_address->setText(data_app.act_filial_address);
    ui->ln_filial_requisits_1->setText(data_app.act_filial_requisits_1);
    ui->ln_filial_requisits_2->setText(data_app.act_filial_requisits_2);
    ui->ln_filial_okpo->setText(data_app.act_filial_okpo);

    ui->ln_base_doc_name->setText(data_app.act_base_doc_name);
    ui->ln_base_doc_num->setText(data_app.act_base_doc_num);
    ui->date_base_doc->setDate(data_app.act_base_doc_date);
    ui->ln_act_num->setText(data_app.act_num);
    ui->date_act->setDate(data_app.act_date);
    ui->ln_tabel_num->setText(data_app.act_tabel_num);
    ui->ln_count->setText(data_app.act_count);
}

print_blanks_act_wnd::~print_blanks_act_wnd() {
    delete ui;
}

void print_blanks_act_wnd::on_bn_cansel_clicked() {
    reject();
}

QString print_blanks_act_wnd::month_to_str(QDate date) {
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

void print_blanks_act_wnd::on_bn_edit_clicked(bool checked) {
    if (checked) {
        ui->bn_edit->setText("Сохранить");
        ui->pan_settings->setEnabled(true);
        ui->bn_print->setEnabled(false);
    } else {

        data_app.act_dog_num = ui->ln_dog_num->text();
        data_app.act_dog_date = ui->date_dog->date();
        data_app.act_filial_chif_post = ui->ln_filial_chif_post->text();
        data_app.act_filial_chif_fio = ui->ln_filial_chif_fio->text();
        data_app.act_point_chif_post = ui->ln_point_chif_post->text();
        data_app.act_point_chif_fio = ui->ln_point_chif_fio->text();

        data_app.act_point_org_name = ui->ln_point_org_name->text();
        data_app.act_point_address = ui->ln_point_address->text();
        data_app.act_point_requisits_1 = ui->ln_point_requisits_1->text();
        data_app.act_point_requisits_2 = ui->ln_point_requisits_2->text();
        data_app.act_point_okpo = ui->ln_point_okpo->text();

        data_app.act_filial_org_name = ui->ln_filial_org_name->text();
        data_app.act_filial_address = ui->ln_filial_address->text();
        data_app.act_filial_requisits_1 = ui->ln_filial_requisits_1->text();
        data_app.act_filial_requisits_2 = ui->ln_filial_requisits_2->text();
        data_app.act_filial_okpo = ui->ln_filial_okpo->text();

        data_app.act_base_doc_name = ui->ln_base_doc_name->text();
        data_app.act_base_doc_num = ui->ln_base_doc_num->text();
        data_app.act_base_doc_date = ui->date_base_doc->date();
        data_app.act_num = ui->ln_act_num->text();
        data_app.act_date = ui->date_act->date();
        data_app.act_tabel_num = ui->ln_tabel_num->text();
        data_app.act_count = ui->ln_count->text();

        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Сохранить сделанные изменения в настройках программы?\n\n"
                                  "В случае согласия новые данные будут сохранены и останутся доступны после перезагрузки программы.\n"
                                  "В случае отказа эти изменения будут доступны только до первого перезапуска программы.",
                                  QMessageBox::Yes|QMessageBox::No,
                                  QMessageBox::No)==QMessageBox::Yes)
        {
            // [blank_acts]
            settings.beginGroup("blank_acts");
            settings.setValue("dog_num",            data_app.act_dog_num );
            settings.setValue("dog_date",           data_app.act_dog_date.toString("yyyy-MM-dd"));
            settings.setValue("filial_chif_post",   data_app.act_filial_chif_post );
            settings.setValue("filial_chif_fio",    data_app.act_filial_chif_fio );
            settings.setValue("point_chif_post",    data_app.act_point_chif_post );
            settings.setValue("point_chif_fio",     data_app.act_point_chif_fio );

            settings.setValue("point_org_name",     data_app.act_point_org_name );
            settings.setValue("point_address",      data_app.act_point_address );
            settings.setValue("point_requisits_1",  data_app.act_point_requisits_1 );
            settings.setValue("point_requisits_2",  data_app.act_point_requisits_2 );
            settings.setValue("point_okpo",         data_app.act_point_okpo );

            settings.setValue("filial_org_name",    data_app.act_filial_org_name );
            settings.setValue("filial_address",     data_app.act_filial_address );
            settings.setValue("filial_requisits_1", data_app.act_filial_requisits_1 );
            settings.setValue("filial_requisits_2", data_app.act_filial_requisits_2 );
            settings.setValue("filial_okpo",        data_app.act_filial_okpo );

            settings.setValue("base_doc_name",      data_app.act_base_doc_name );
            settings.setValue("base_doc_num",       data_app.act_base_doc_num );
            settings.setValue("base_doc_date",      data_app.act_base_doc_date.toString("yyyy-MM-dd"));
            settings.setValue("act_num",            data_app.act_num );
            settings.setValue("act_date",           data_app.act_date.toString("yyyy-MM-dd"));
            settings.setValue("act_count",          data_app.act_count );
            settings.setValue("act_tabel_num",      data_app.act_tabel_num );
            settings.endGroup();
            settings.sync();
        }

        ui->bn_edit->setText("Изменить");
        ui->pan_settings->setEnabled(false);
        ui->bn_print->setEnabled(true);
    }
}

#include "qt_windows.h"
#include "qwindowdefs_win.h"
#include <shellapi.h>


void print_blanks_act_wnd::on_bn_print_clicked() {
    accept();
}
