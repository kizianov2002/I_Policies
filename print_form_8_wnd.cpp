#include "print_form_8_wnd.h"
#include "ui_print_form_8_wnd.h"

print_form_8_wnd::print_form_8_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    db(db),
    data_app(data_app),
    settings(settings),
    QDialog(parent),
    ui(new Ui::print_form_8_wnd)
{
    ui->setupUi(this);

    f_init = true;

    on_bd_cur_date_clicked();
    on_bn_report_date_clicked();
    on_bn_edit_clicked(false);


    ui->ln_doc1_num->setText(data_app.form8_doc1_num);
    ui->date_doc1->setDate(data_app.form8_doc1_date);
    ui->ch_doc2->setChecked(data_app.form8_has_doc2);
    ui->ln_doc2_num->setText(data_app.form8_doc2_num);
    ui->date_doc2->setDate(data_app.form8_doc2_date);
    ui->ln_smo_name->setText(data_app.smo_name);
    ui->ln_smo_address->setText(data_app.smo_address);

    ui->ln_codename1->setText(data_app.form8_codename1);
    ui->ln_codename2->setText(data_app.form8_codename2);
    ui->ln_codename3->setText(data_app.form8_codename3);
    ui->ln_code1->setText(data_app.form8_code1);
    ui->ln_code2->setText(data_app.form8_code2);
    ui->ln_code3->setText(data_app.form8_code3);

    ui->ln_smo_director->setText(data_app.smo_director);
    ui->ln_tfoms_director_fio->setText(data_app.tfoms_director_fio);
    ui->ln_tfoms_worker->setText(data_app.tfoms_worker);
    ui->ln_tfoms_worker2_place->setText(data_app.tfoms_worker2_place);
    ui->ln_tfoms_worker2_fio->setText(data_app.tfoms_worker2_fio);
    ui->ln_tfoms_worker2_phone->setText(data_app.tfoms_worker2_phone);

    f_init = false;
}

print_form_8_wnd::~print_form_8_wnd() {
    delete ui;
}

void print_form_8_wnd::on_bn_cansel_clicked() {
    reject();
}

void print_form_8_wnd::on_bd_cur_date_clicked() {
    ui->date_cur->setDate(QDate::currentDate().addDays(-1));
}

void print_form_8_wnd::on_bn_report_date_clicked() {
    QDate date = QDate::currentDate().addDays(-1);
    int day=date.day(), month=date.month(), year=date.year();
    day = 1;
    switch (month) {
    case 1:
    case 2:
        month = 1;
        break;
    case 3:
    case 4:
    case 5:
        month = 4;
        break;
    case 6:
    case 7:
    case 8:
        month = 7;
        break;
    case 9:
    case 10:
    case 11:
        month = 10;
        break;
    case 12:
        month = 1;
        year = date.year() + 1;
        break;
    }
    ui->date_report->setDate(QDate(year,month,day));
}

QString print_form_8_wnd::month_to_str(QDate date) {
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

void print_form_8_wnd::on_bn_edit_clicked(bool checked) {
    if (checked) {
        ui->bn_edit->setText("Сохранить");

        /*ui->lab_cur_date->setEnabled(true);
        ui->date_cur->setEnabled(true);
        ui->bd_cur_date->setEnabled(true);
        ui->lab_report_date->setEnabled(true);
        ui->_date->setEnabled(true);
        ui->bd_cur_date->setEnabled(true);*/

        ui->lab_doc1_num->setEnabled(checked);
        ui->ln_doc1_num->setEnabled(checked);
        ui->lab_doc1_date->setEnabled(checked);
        ui->date_doc1->setEnabled(checked);
        ui->ch_doc2->setEnabled(checked);
        ui->ln_doc2_num->setEnabled(checked && ui->ch_doc2->isChecked());
        ui->lab_doc2_date->setEnabled(checked && ui->ch_doc2->isChecked());
        ui->date_doc2->setEnabled(checked && ui->ch_doc2->isChecked());
        ui->lab_smo_name->setEnabled(checked);
        ui->ln_smo_name->setEnabled(checked);
        ui->lab_smo_address->setEnabled(checked);
        ui->ln_smo_address->setEnabled(checked);

        ui->lab_codes->setEnabled(checked);
        ui->lab_codenum1->setEnabled(checked);
        ui->lab_codenum2->setEnabled(checked);
        ui->lab_codenum3->setEnabled(checked);
        ui->lab_codenames->setEnabled(checked);
        ui->lab_codevals->setEnabled(checked);
        ui->ln_codename1->setEnabled(checked);
        ui->ln_codename2->setEnabled(checked);
        ui->ln_codename3->setEnabled(checked);
        ui->ln_code1->setEnabled(checked);
        ui->ln_code2->setEnabled(checked);
        ui->ln_code3->setEnabled(checked);

        ui->lab_signers->setEnabled(checked);
        ui->lab_smo_director->setEnabled(checked);
        ui->ln_smo_director->setEnabled(checked);
        ui->lab_tfoms_director_fio->setEnabled(checked);
        ui->ln_tfoms_director_fio->setEnabled(checked);
        ui->lab_tfoms_worker->setEnabled(checked);
        ui->ln_tfoms_worker->setEnabled(checked);
        ui->lab_tfoms_worker2->setEnabled(checked);
        ui->ln_tfoms_worker2_place->setEnabled(checked);
        ui->ln_tfoms_worker2_fio->setEnabled(checked);
        ui->ln_tfoms_worker2_phone->setEnabled(checked);
        ui->lab_tfoms_worker2_place->setEnabled(checked);
        ui->lab_tfoms_worker2_fio->setEnabled(checked);
        ui->lab_tfoms_worker2_phone->setEnabled(checked);

        ui->bn_print->setEnabled(false);

    } else {
        if (!f_init) {
            data_app.form8_doc1_num  = ui->ln_doc1_num->text();
            data_app.form8_doc1_date = ui->date_doc1->date();
            data_app.form8_has_doc2  = ui->ch_doc2->isChecked();
            data_app.form8_doc2_num  = ui->ln_doc2_num->text();
            data_app.form8_doc2_date = ui->date_doc2->date();
            data_app.smo_name        = ui->ln_smo_name->text();
            data_app.smo_address     = ui->ln_smo_address->text();

            data_app.form8_codename1 = ui->ln_codename1->text();
            data_app.form8_codename2 = ui->ln_codename2->text();
            data_app.form8_codename3 = ui->ln_codename3->text();
            data_app.form8_code1     = ui->ln_code1->text();
            data_app.form8_code2     = ui->ln_code2->text();
            data_app.form8_code3     = ui->ln_code3->text();

            data_app.smo_director    = ui->ln_smo_director->text();
            data_app.tfoms_director_fio  = ui->ln_tfoms_director_fio->text();
            data_app.tfoms_worker    = ui->ln_tfoms_worker->text();
            data_app.tfoms_worker2_place = ui->ln_tfoms_worker2_place->text();
            data_app.tfoms_worker2_fio   = ui->ln_tfoms_worker2_fio->text();
            data_app.tfoms_worker2_phone = ui->ln_tfoms_worker2_phone->text();

            if (QMessageBox::question(this,
                                      "Нужно подтверждение",
                                      "Сохранить сделанные изменения в настройках программы?\n\n"
                                      "В случае согласия новые данные будут сохранены и останутся доступны после перезагрузки программы.\n"
                                      "В случае отказа эти изменения будут доступны только до первого перезапуска программы.",
                                      QMessageBox::Yes|QMessageBox::No,
                                      QMessageBox::No)==QMessageBox::Yes)
            {
                // [req_ch_smo]
                settings.beginGroup("smo");
                settings.setValue("smo_director", data_app.smo_director.toUtf8());
                settings.setValue("smo_address", data_app.smo_address.toUtf8());
                settings.endGroup();

                // [form8]
                settings.beginGroup("form8");
                settings.setValue("doc1_num",  data_app.form8_doc1_num);
                settings.setValue("doc1_date", data_app.form8_doc1_date.toString("yyyy-MM-dd"));
                settings.setValue("has_doc2",  data_app.form8_has_doc2 ? "1" : "0");
                settings.setValue("doc2_num",  data_app.form8_doc2_num);
                settings.setValue("doc2_date", data_app.form8_doc2_date.toString("yyyy-MM-dd"));
                settings.setValue("codename1", data_app.form8_codename1);
                settings.setValue("codename2", data_app.form8_codename2);
                settings.setValue("codename3", data_app.form8_codename3);
                settings.setValue("code1",     data_app.form8_code1);
                settings.setValue("code2",     data_app.form8_code2);
                settings.setValue("code3",     data_app.form8_code3);
                settings.setValue("tfoms_worker",        data_app.tfoms_worker);
                settings.setValue("tfoms_worker2_place", data_app.tfoms_worker2_place);
                settings.setValue("tfoms_worker2_fio",   data_app.tfoms_worker2_fio);
                settings.setValue("tfoms_worker2_phone", data_app.tfoms_worker2_phone);
                settings.endGroup();
                settings.sync();
            }
        }
        ui->bn_edit->setText("Изменить");

        /*ui->lab_cur_date->setEnabled(false);
        ui->date_cur->setEnabled(false);
        ui->bd_cur_date->setEnabled(false);
        ui->lab_report_date->setEnabled(false);
        ui->_date->setEnabled(false);
        ui->bd_cur_date->setEnabled(false);*/

        ui->lab_doc1_num->setEnabled(checked);
        ui->ln_doc1_num->setEnabled(checked);
        ui->lab_doc1_date->setEnabled(checked);
        ui->date_doc1->setEnabled(checked);
        ui->ch_doc2->setEnabled(checked);
        ui->ln_doc2_num->setEnabled(checked && ui->ch_doc2->isChecked());
        ui->lab_doc2_date->setEnabled(checked && ui->ch_doc2->isChecked());
        ui->date_doc2->setEnabled(checked && ui->ch_doc2->isChecked());
        ui->lab_smo_name->setEnabled(checked);
        ui->ln_smo_name->setEnabled(checked);
        ui->lab_smo_address->setEnabled(checked);
        ui->ln_smo_address->setEnabled(checked);

        ui->lab_codes->setEnabled(checked);
        ui->lab_codenum1->setEnabled(checked);
        ui->lab_codenum2->setEnabled(checked);
        ui->lab_codenum3->setEnabled(checked);
        ui->lab_codenames->setEnabled(checked);
        ui->lab_codevals->setEnabled(checked);
        ui->ln_codename1->setEnabled(checked);
        ui->ln_codename2->setEnabled(checked);
        ui->ln_codename3->setEnabled(checked);
        ui->ln_code1->setEnabled(checked);
        ui->ln_code2->setEnabled(checked);
        ui->ln_code3->setEnabled(checked);

        ui->lab_signers->setEnabled(checked);
        ui->lab_smo_director->setEnabled(checked);
        ui->ln_smo_director->setEnabled(checked);
        ui->lab_tfoms_director_fio->setEnabled(checked);
        ui->ln_tfoms_director_fio->setEnabled(checked);
        ui->lab_tfoms_worker->setEnabled(checked);
        ui->ln_tfoms_worker->setEnabled(checked);
        ui->lab_tfoms_worker2->setEnabled(checked);
        ui->ln_tfoms_worker2_place->setEnabled(checked);
        ui->ln_tfoms_worker2_fio->setEnabled(checked);
        ui->ln_tfoms_worker2_phone->setEnabled(checked);
        ui->lab_tfoms_worker2_place->setEnabled(checked);
        ui->lab_tfoms_worker2_fio->setEnabled(checked);
        ui->lab_tfoms_worker2_phone->setEnabled(checked);

        ui->bn_print->setEnabled(true);
    }
}

#include "qt_windows.h"
#include "qwindowdefs_win.h"
#include <shellapi.h>

void print_form_8_wnd::on_bn_print_clicked() {
    ui->bn_print->setEnabled(false);
    this->setCursor(Qt::WaitCursor);

    // распаковка шаблона
    QString rep_folder = data_app.path_reports;
    // распаковка шаблона
    QString tmp_folder = data_app.path_out + "_NOTICES_/temp/";
    QString rep_template = rep_folder + "rep_form_8.odt";

    if (!QFile::exists(rep_template)) {
        QMessageBox::warning(this,
                             "Шаблон не найден",
                             "Не найден шаблон печатной формы \"Форма № 8\": \n" + rep_template +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    QDir tmp_dir(tmp_folder);
    if (tmp_dir.exists(tmp_folder) && !tmp_dir.removeRecursively()) {
        QMessageBox::warning(this,
                             "Ошибка при очистке папки",
                             "Не удалось очистить папку временных файлов: \n" + tmp_folder +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    tmp_dir.mkpath(tmp_folder);
    QProcess myProcess;
    QString com = "\"" + data_app.path_arch + "7z.exe\" e \"" + rep_template + "\" -o\"" + tmp_folder + "\" content.xml";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка распаковки шаблона",
                             "Не удалось распаковать файл контента шаблона\n" + rep_template + " -> content.xml"
                             "\nпечатной формы \"Форма № 8\" во временную папку\n" + tmp_folder +
                             "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    com = "\"" + data_app.path_arch + "7z.exe\" e \"" + rep_template + "\" -o\"" + tmp_folder + "\" styles.xml";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка распаковки шаблона",
                             "Не удалось распаковать файл стилей шаблона \n" + rep_template + " -> styles.xml"
                             "\nпечатной формы \"Форма № 8\" во временную папку\n" + tmp_folder +
                             "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }


    // ===========================
    // правка контента шаблона
    // ===========================
    // открытие контента шаблона
    QFile f_content(tmp_folder + "/content.xml");
    f_content.open(QIODevice::ReadOnly);
    QString s_content = f_content.readAll();
    f_content.close();
    if (s_content.isEmpty()) {
        QMessageBox::warning(this,
                             "Файл контента пустой",
                             "Файл контента пустой. Возможно шаблон был испорчен.\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    // правка полей контента шаблона

    QDate curDate = ui->date_cur->date();
    QDate repDate = ui->date_report->date();

    s_content = s_content.
            replace("#MONTH#", month_to_str(repDate)).
            replace("#YR#", QString::number(repDate.year()-2000)).
            replace("#day#", QString::number(curDate.day())).
            replace("#month#", month_to_str(curDate)).
            replace("#yr#", QString(QString("00") + QString::number(curDate.year()-2000)).right(2)).
            replace("#DATE1#", data_app.form8_doc1_date.toString("yyyy-MM-dd")).
            replace("#N1#", data_app.form8_doc1_num).
            replace("#DATE2#", data_app.form8_has_doc2 ? data_app.form8_doc2_date.toString("yyyy-MM-dd") : "").
            replace("#N2#", data_app.form8_has_doc2 ? data_app.form8_doc2_num : "").
            replace("#SMO_NAME#", data_app.smo_name_full).
            replace("#SMO_ADDRESS#", data_app.smo_name_full).
            replace("#CODE_NAME1#", data_app.form8_codename1).
            replace("#CODE_NAME2#", data_app.form8_codename2).
            replace("#CODE_NAME3#", data_app.form8_codename3).
            replace("#CODE_VAL1#", data_app.form8_code1).
            replace("#CODE_VAL2#", data_app.form8_code2).
            replace("#CODE_VAL3#", data_app.form8_code3).
            replace("#SMO_DIRECTOR#", data_app.smo_director).
            replace("#TFOMS_DIRECTOR#", data_app.tfoms_director_fio).
            replace("#TFOMS_WORKER#", data_app.tfoms_worker).
            replace("#RESP_DOLJN#", data_app.tfoms_worker2_place).
            replace("#RESP_FIO#", data_app.tfoms_worker2_fio).
            replace("#RESP_PHONE#", data_app.tfoms_worker2_phone);

    // ------------------------- //
    // собственно выборка данных //
    // ------------------------- //

    // пороговые даты
    QString ds = repDate.toString("yyyy-MM-dd");
    QDate  d05 = repDate.addYears( -5);
    QDate  d16 = repDate.addYears(-16);
    QDate  d18 = repDate.addYears(-18);
    QDate  d55 = repDate.addYears(-55);
    QDate  d60 = repDate.addYears(-60);

    // Обновление статусов застрахованных
    QString sql = "select * from update_pers_links('" + ds + "') ; ";
    QSqlQuery *query = new QSqlQuery(db);
    bool res;
    if (!mySQL.exec(this, sql, "Обновление статусов застрахованных", *query, true, db, data_app))
        QMessageBox::warning(this, "Операция отменена", "При обновлении статусов застрахованных произошла непредвиденная ошибка.\n\nОперация пропущена.");
    delete query;

    // переменные для полей таблицы
    int N=0,      NW=0,      NU=0,      Ne=0,      NeW=0,      NeU=0,
        Nm=0,     NWm=0,     NUm=0,     Nem=0,     NeWm=0,     NeUm=0,
        Nw=0,     NWw=0,     NUw=0,     New=0,     NeWw=0,     NeUw=0,
        N0_17=0,  NW0_17=0,  NU0_17=0,  Ne0_17=0,  NeW0_17=0,  NeU0_17=0,
        Nm0_17=0, NWm0_17=0, NUm0_17=0, Nem0_17=0, NeWm0_17=0, NeUm0_17=0,
        Nw0_17=0, NWw0_17=0, NUw0_17=0, New0_17=0, NeWw0_17=0, NeUw0_17=0,
        N0_4=0,   NW0_4=0,   NU0_4=0,   Ne0_4=0,   NeW0_4=0,   NeU0_4=0,
        Nm0_4=0,  NWm0_4=0,  NUm0_4=0,  Nem0_4=0,  NeWm0_4=0,  NeUm0_4=0,
        Nw0_4=0,  NWw0_4=0,  NUw0_4=0,  New0_4=0,  NeWw0_4=0,  NeUw0_4=0,
        N18_=0,   NW18_=0,   NU18_=0,   Ne18_=0,   NeW18_=0,   NeU18_=0,
        Nm18_=0,  NWm18_=0,  NUm18_=0,  Nem18_=0,  NeWm18_=0,  NeUm18_=0,
        Nw18_=0,  NWw18_=0,  NUw18_=0,  New18_=0,  NeWw18_=0,  NeUw18_=0,
        N0_15=0,  NW0_15=0,  NU0_15=0,  Ne0_15=0,  NeW0_15=0,  NeU0_15=0,
        Nm0_15=0, NWm0_15=0, NUm0_15=0, Nem0_15=0, NeWm0_15=0, NeUm0_15=0,
        Nw0_15=0, NWw0_15=0, NUw0_15=0, New0_15=0, NeWw0_15=0, NeUw0_15=0,
        N16_P=0,  NW16_P=0,  NU16_P=0,  Ne16_P=0,  NeW16_P=0,  NeU16_P=0,
        Nm16_P=0, NWm16_P=0, NUm16_P=0, Nem16_P=0, NeWm16_P=0, NeUm16_P=0,
        Nw16_P=0, NWw16_P=0, NUw16_P=0, New16_P=0, NeWw16_P=0, NeUw16_P=0,
        Nm18_P=0, NWm18_P=0, NUm18_P=0, Nem18_P=0, NeWm18_P=0, NeUm18_P=0,
        Nw18_P=0, NWw18_P=0, NUw18_P=0, New18_P=0, NeWw18_P=0, NeUw18_P=0,
        NP_=0,    NWP_=0,    NUP_=0,    NeP_=0,    NeWP_=0,    NeUP_=0,
        NmP_=0,   NWmP_=0,   NUmP_=0,   NemP_=0,   NeWmP_=0,   NeUmP_=0,
        NwP_=0,   NWwP_=0,   NUwP_=0,   NewP_=0,   NeWwP_=0,   NeUwP_=0,
        NI=0,     NIW=0,     NIU=0,     NIe=0,     NIeW=0,     NIeU=0,
        NII=0,    NIIW=0,    NIIU=0,    NIIe=0,    NIIeW=0,    NIIeU=0,
        NS=0,     NSW=0,     NSU=0,     NSe=0,     NSeW=0,     NSeU=0;


    // ************************************************************************************
    QString sql_f8 = //"select * from update_pers_links('" +ds+ "') ; "
                     "select e.id, e.category, e.sex, e.date_birth, p.pol_v "
                     " from persons e join polises p on(p.id=e.id_polis) "
                     " where (p.date_begin<='" +ds+ "') "
                     "   and ((p.date_stop is NULL) or (p.date_stop>='" +ds+ "')) "
                     //"   and e.status>0 ; ";
                     "   and (select * from st_person(e.id,'" +ds+ "')) in(1,2,3,4,11,12,13,14) ";
    QSqlQuery *query_f8 = new QSqlQuery(db);
    if (mySQL.exec(this, sql_f8, "Форма № 8", *query_f8, true, db, data_app)) {
        while (query->next()) {
            int category = query_f8->value(1).toInt();
            int sex = query_f8->value(2).toInt();
            QDate date_birth = query_f8->value(3).toDate();
            int pol_v = query_f8->value(4).toInt();

            // 01:  Застраховано по обязательному медицинскому страхованию, всего
            N++;
            if (category<=5) {
                NW++;
            } else {
                NU++;
            }
            if (pol_v==3 || pol_v==4) {
                Ne++;
                if (category<=5) {
                    NeW++;
                } else {
                    NeU++;
                }
            }

            // 02:  в том числе: мужчины
            if (sex==1) {
                Nm++;
                if (category<=5) {
                    NWm++;
                } else {
                    NUm++;
                }
                if (pol_v==3 || pol_v==4) {
                    Nem++;
                    if (category<=5) {
                        NeWm++;
                    } else {
                        NeUm++;
                    }
                }
            }

            // 03:  в том числе: женщины
            if (sex==2) {
                Nw++;
                if (category<=5) {
                    NWw++;
                } else {
                    NUw++;
                }
                if (pol_v==3 || pol_v==4) {
                    New++;
                    if (category<=5) {
                        NeWw++;
                    } else {
                        NeUw++;
                    }
                }
            }

            // 04:  дети в возрасте: от 0 до 17 лет, всего
            if (date_birth>d18) {
                N0_17++;
                if (category<=5) {
                    NW0_17++;
                } else {
                    NU0_17++;
                }
                if (pol_v==3 || pol_v==4) {
                    Ne0_17++;
                    if (category<=5) {
                        NeW0_17++;
                    } else {
                        NeU0_17++;
                    }
                }

                // 05:  в том числе: мужчины
                if (sex==1) {
                    Nm0_17++;
                    if (category<=5) {
                        NWm0_17++;
                    } else {
                        NUm0_17++;
                    }
                    if (pol_v==3 || pol_v==4) {
                        Nem0_17++;
                        if (category<=5) {
                            NeWm0_17++;
                        } else {
                            NeUm0_17++;
                        }
                    }
                }

                // 06:  в том числе: женщины
                if (sex==2) {
                    Nw0_17++;
                    if (category<=5) {
                        NWw0_17++;
                    } else {
                        NUw0_17++;
                    }
                    if (pol_v==3 || pol_v==4) {
                        New0_17++;
                        if (category<=5) {
                            NeWw0_17++;
                        } else {
                            NeUw0_17++;
                        }
                    }
                }
            }


            // 07:  дети в возрасте: от 0 до 4 лет, всего
            if (date_birth>d05) {
                N0_4++;
                if (category<=5) {
                    NW0_4++;
                } else {
                    NU0_4++;
                }
                if (pol_v==3 || pol_v==4) {
                    Ne0_4++;
                    if (category<=5) {
                        NeW0_4++;
                    } else {
                        NeU0_4++;
                    }
                }

                // 08:  в том числе: мужчины
                if (sex==1) {
                    Nm0_4++;
                    if (category<=5) {
                        NWm0_4++;
                    } else {
                        NUm0_4++;
                    }
                    if (pol_v==3 || pol_v==4) {
                        Nem0_4++;
                        if (category<=5) {
                            NeWm0_4++;
                        } else {
                            NeUm0_4++;
                        }
                    }
                }

                // 09:  в том числе: женщины
                if (sex==2) {
                    Nw0_4++;
                    if (category<=5) {
                        NWw0_4++;
                    } else {
                        NUw0_4++;
                    }
                    if (pol_v==3 || pol_v==4) {
                        New0_4++;
                        if (category<=5) {
                            NeWw0_4++;
                        } else {
                            NeUw0_4++;
                        }
                    }
                }
            }


            // 10:  18 лет и старше
            if (date_birth<=d18) {
                N18_++;
                if (category<=5) {
                    NW18_++;
                } else {
                    NU18_++;
                }
                if (pol_v==3 || pol_v==4) {
                    Ne18_++;
                    if (category<=5) {
                        NeW18_++;
                    } else {
                        NeU18_++;
                    }
                }

                // 11:  в том числе: мужчины
                if (sex==1) {
                    Nm18_++;
                    if (category<=5) {
                        NWm18_++;
                    } else {
                        NUm18_++;
                    }
                    if (pol_v==3 || pol_v==4) {
                        Nem18_++;
                        if (category<=5) {
                            NeWm18_++;
                        } else {
                            NeUm18_++;
                        }
                    }
                }

                // 12:  в том числе: женщины
                if (sex==2) {
                    Nw18_++;
                    if (category<=5) {
                        NWw18_++;
                    } else {
                        NUw18_++;
                    }
                    if (pol_v==3 || pol_v==4) {
                        New18_++;
                        if (category<=5) {
                            NeWw18_++;
                        } else {
                            NeUw18_++;
                        }
                    }
                }
            }


            // 13:  моложе трудоспособного возраста от 0 до 15 лет, всего
            if (date_birth>d16) {
                N0_15++;
                if (category<=5) {
                    NW0_15++;
                } else {
                    NU0_15++;
                }
                if (pol_v==3 || pol_v==4) {
                    Ne0_15++;
                    if (category<=5) {
                        NeW0_15++;
                    } else {
                        NeU0_15++;
                    }
                }

                // 14:  в том числе: мужчины
                if (sex==1) {
                    Nm0_15++;
                    if (category<=5) {
                        NWm0_15++;
                    } else {
                        NUm0_15++;
                    }
                    if (pol_v==3 || pol_v==4) {
                        Nem0_15++;
                        if (category<=5) {
                            NeWm0_15++;
                        } else {
                            NeUm0_15++;
                        }
                    }
                }

                // 15:  в том числе: женщины
                if (sex==2) {
                    Nw0_15++;
                    if (category<=5) {
                        NWw0_15++;
                    } else {
                        NUw0_15++;
                    }
                    if (pol_v==3 || pol_v==4) {
                        New0_15++;
                        if (category<=5) {
                            NeWw0_15++;
                        } else {
                            NeUw0_15++;
                        }
                    }
                }
            }


            // 16:  в трудоспособном возрасте, всего
            if ( (sex==1 && date_birth<=d16 && date_birth>d60) ||
                 (sex==2 && date_birth<=d16 && date_birth>d55) ) {
                N16_P++;
                if (category<=5) {
                    NW16_P++;
                } else {
                    NU16_P++;
                }
                if (pol_v==3 || pol_v==4) {
                    Ne16_P++;
                    if (category<=5) {
                        NeW16_P++;
                    } else {
                        NeU16_P++;
                    }
                }

                // 17:  в том числе: мужчины 16 — 59 лет
                if (sex==1 && date_birth<=d16 && date_birth>d60) {
                    Nm16_P++;
                    if (category<=5) {
                        NWm16_P++;
                    } else {
                        NUm16_P++;
                    }
                    if (pol_v==3 || pol_v==4) {
                        Nem16_P++;
                        if (category<=5) {
                            NeWm16_P++;
                        } else {
                            NeUm16_P++;
                        }
                    }
                }

                // 18:  в том числе: женщины 16 — 54 года
                if (sex==2 && date_birth<=d16 && date_birth>d55) {
                    Nw16_P++;
                    if (category<=5) {
                        NWw16_P++;
                    } else {
                        NUw16_P++;
                    }
                    if (pol_v==3 || pol_v==4) {
                        New16_P++;
                        if (category<=5) {
                            NeWw16_P++;
                        } else {
                            NeUw16_P++;
                        }
                    }
                }

                // 19:  в том числе: мужчины 18 — 59 лет
                if (sex==1 && date_birth<=d18 && date_birth>d60) {
                    Nm18_P++;
                    if (category<=5) {
                        NWm18_P++;
                    } else {
                        NUm18_P++;
                    }
                    if (pol_v==3 || pol_v==4) {
                        Nem18_P++;
                        if (category<=5) {
                            NeWm18_P++;
                        } else {
                            NeUm18_P++;
                        }
                    }
                }

                // 20:  в том числе: женщины 18 — 54 года
                if (sex==2 && date_birth<=d18 && date_birth>d55) {
                    Nw18_P++;
                    if (category<=5) {
                        NWw18_P++;
                    } else {
                        NUw18_P++;
                    }
                    if (pol_v==3 || pol_v==4) {
                        New18_P++;
                        if (category<=5) {
                            NeWw18_P++;
                        } else {
                            NeUw18_P++;
                        }
                    }
                }
            }


            // 21:  старше трудоспособного возраста, всего
            if ( (sex==1 && date_birth<=d60) ||
                 (sex==2 && date_birth<=d55) ) {
                NP_++;
                if (category<=5) {
                    NWP_++;
                } else {
                    NUP_++;
                }
                if (pol_v==3 || pol_v==4) {
                    NeP_++;
                    if (category<=5) {
                        NeWP_++;
                    } else {
                        NeUP_++;
                    }
                }

                // 22:  в том числе: мужчины 60 лет и старше
                if (sex==1 && date_birth<=d60) {
                    NmP_++;
                    if (category<=5) {
                        NWmP_++;
                    } else {
                        NUmP_++;
                    }
                    if (pol_v==3 || pol_v==4) {
                        NemP_++;
                        if (category<=5) {
                            NeWmP_++;
                        } else {
                            NeUmP_++;
                        }
                    }
                }

                // 23:  в том числе: женщины 55 лет и старше
                if (sex==2 && date_birth<=d55) {
                    NwP_++;
                    if (category<=5) {
                        NWwP_++;
                    } else {
                        NUwP_++;
                    }
                    if (pol_v==3 || pol_v==4) {
                        NewP_++;
                        if (category<=5) {
                            NeWwP_++;
                        } else {
                            NeUwP_++;
                        }
                    }
                }
            }


            // 25:  Иностранные граждане и лица без гражданства , всего
            if ( category==2 ||
                 category==3 ||
                 category==4 ||
                 category==7 ||
                 category==8 ||
                 category==9 ) {
                NI++;
                if (category<=5) {
                    NIW++;
                } else {
                    NIU++;
                }
                if (pol_v==3 || pol_v==4) {
                    NIe++;
                    if (category<=5) {
                        NIeW++;
                    } else {
                        NIeU++;
                    }
                }
            }


            // 26:  из них: временно проживающие на территории Российской Федераци
            if ( category==3 ||
                 category==8 ) {
                NII++;
                if (category<=5) {
                    NIIW++;
                } else {
                    NIIU++;
                }
                if (pol_v==3 || pol_v==4) {
                    NIIe++;
                    if (category<=5) {
                        NIIeW++;
                    } else {
                        NIIeU++;
                    }
                }
            }


            // 27:  Лица, имеющие право на медицинскую помощь в соответствии с Федеральным законом "О беженцах"
            if ( category==5 ||
                 category==10 ) {
                NS++;
                if (category<=5) {
                    NSW++;
                } else {
                    NSU++;
                }
                if (pol_v==3 || pol_v==4) {
                    NSe++;
                    if (category<=5) {
                        NSeW++;
                    } else {
                        NSeU++;
                    }
                }
            }
        }


        // запишем результаты в печатную форму
        s_content = s_content.
            replace("#N#", QString::number(N)).
            replace("#NW#", QString::number(NW)).
            replace("#NU#", QString::number(NU)).
            replace("#Ne#", QString::number(Ne)).
            replace("#NeW#", QString::number(NeW)).
            replace("#NeU#", QString::number(NeU)).

            replace("#Nm#", QString::number(Nm)).
            replace("#NWm#", QString::number(NWm)).
            replace("#NUm#", QString::number(NUm)).
            replace("#Nem#", QString::number(Nem)).
            replace("#NeWm#", QString::number(NeWm)).
            replace("#NeUm#", QString::number(NeUm)).

            replace("#Nw#", QString::number(Nw)).
            replace("#NWw#", QString::number(NWw)).
            replace("#NUw#", QString::number(NUw)).
            replace("#New#", QString::number(New)).
            replace("#NeWw#", QString::number(NeWw)).
            replace("#NeUw#", QString::number(NeUw)).

            replace("#N0_17#", QString::number(N0_17)).
            replace("#NW0_17#", QString::number(NW0_17)).
            replace("#NU0_17#", QString::number(NU0_17)).
            replace("#Ne0_17#", QString::number(Ne0_17)).
            replace("#NeW0_17#", QString::number(NeW0_17)).
            replace("#NeU0_17#", QString::number(NeU0_17)).

            replace("#Nm0_17#", QString::number(Nm0_17)).
            replace("#NWm0_17#", QString::number(NWm0_17)).
            replace("#NUm0_17#", QString::number(NUm0_17)).
            replace("#Nem0_17#", QString::number(Nem0_17)).
            replace("#NeWm0_17#", QString::number(NeWm0_17)).
            replace("#NeUm0_17#", QString::number(NeUm0_17)).

            replace("#Nw0_17#", QString::number(Nw0_17)).
            replace("#NWw0_17#", QString::number(NWw0_17)).
            replace("#NUw0_17#", QString::number(NUw0_17)).
            replace("#New0_17#", QString::number(New0_17)).
            replace("#NeWw0_17#", QString::number(NeWw0_17)).
            replace("#NeUw0_17#", QString::number(NeUw0_17)).

            replace("#N0_4#", QString::number(N0_4)).
            replace("#NW0_4#", QString::number(NW0_4)).
            replace("#NU0_4#", QString::number(NU0_4)).
            replace("#Ne0_4#", QString::number(Ne0_4)).
            replace("#NeW0_4#", QString::number(NeW0_4)).
            replace("#NeU0_4#", QString::number(NeU0_4)).

            replace("#Nm0_4#", QString::number(Nm0_4)).
            replace("#NWm0_4#", QString::number(NWm0_4)).
            replace("#NUm0_4#", QString::number(NUm0_4)).
            replace("#Nem0_4#", QString::number(Nem0_4)).
            replace("#NeWm0_4#", QString::number(NeWm0_4)).
            replace("#NeUm0_4#", QString::number(NeUm0_4)).

            replace("#Nw0_4#", QString::number(Nw0_4)).
            replace("#NWw0_4#", QString::number(NWw0_4)).
            replace("#NUw0_4#", QString::number(NUw0_4)).
            replace("#New0_4#", QString::number(New0_4)).
            replace("#NeWw0_4#", QString::number(NeWw0_4)).
            replace("#NeUw0_4#", QString::number(NeUw0_4)).

            replace("#N18_#", QString::number(N18_)).
            replace("#NW18_#", QString::number(NW18_)).
            replace("#NU18_#", QString::number(NU18_)).
            replace("#Ne18_#", QString::number(Ne18_)).
            replace("#NeW18_#", QString::number(NeW18_)).
            replace("#NeU18_#", QString::number(NeU18_)).

            replace("#Nm18_#", QString::number(Nm18_)).
            replace("#NWm18_#", QString::number(NWm18_)).
            replace("#NUm18_#", QString::number(NUm18_)).
            replace("#Nem18_#", QString::number(Nem18_)).
            replace("#NeWm18_#", QString::number(NeWm18_)).
            replace("#NeUm18_#", QString::number(NeUm18_)).

            replace("#Nw18_#", QString::number(Nw18_)).
            replace("#NWw18_#", QString::number(NWw18_)).
            replace("#NUw18_#", QString::number(NUw18_)).
            replace("#New18_#", QString::number(New18_)).
            replace("#NeWw18_#", QString::number(NeWw18_)).
            replace("#NeUw18_#", QString::number(NeUw18_)).

            replace("#N0_15#", QString::number(N0_15)).
            replace("#NW0_15#", QString::number(NW0_15)).
            replace("#NU0_15#", QString::number(NU0_15)).
            replace("#Ne0_15#", QString::number(Ne0_15)).
            replace("#NeW0_15#", QString::number(NeW0_15)).
            replace("#NeU0_15#", QString::number(NeU0_15)).

            replace("#Nm0_15#", QString::number(Nm0_15)).
            replace("#NWm0_15#", QString::number(NWm0_15)).
            replace("#NUm0_15#", QString::number(NUm0_15)).
            replace("#Nem0_15#", QString::number(Nem0_15)).
            replace("#NeWm0_15#", QString::number(NeWm0_15)).
            replace("#NeUm0_15#", QString::number(NeUm0_15)).

            replace("#Nw0_15#", QString::number(Nw0_15)).
            replace("#NWw0_15#", QString::number(NWw0_15)).
            replace("#NUw0_15#", QString::number(NUw0_15)).
            replace("#New0_15#", QString::number(New0_15)).
            replace("#NeWw0_15#", QString::number(NeWw0_15)).
            replace("#NeUw0_15#", QString::number(NeUw0_15)).

            replace("#N16_P#", QString::number(N16_P)).
            replace("#NW16_P#", QString::number(NW16_P)).
            replace("#NU16_P#", QString::number(NU16_P)).
            replace("#Ne16_P#", QString::number(Ne16_P)).
            replace("#NeW16_P#", QString::number(NeW16_P)).
            replace("#NeU16_P#", QString::number(NeU16_P)).

            replace("#Nm16_P#", QString::number(Nm16_P)).
            replace("#NWm16_P#", QString::number(NWm16_P)).
            replace("#NUm16_P#", QString::number(NUm16_P)).
            replace("#Nem16_P#", QString::number(Nem16_P)).
            replace("#NeWm16_P#", QString::number(NeWm16_P)).
            replace("#NeUm16_P#", QString::number(NeUm16_P)).

            replace("#Nw16_P#", QString::number(Nw16_P)).
            replace("#NWw16_P#", QString::number(NWw16_P)).
            replace("#NUw16_P#", QString::number(NUw16_P)).
            replace("#New16_P#", QString::number(New16_P)).
            replace("#NeWw16_P#", QString::number(NeWw16_P)).
            replace("#NeUw16_P#", QString::number(NeUw16_P)).

            replace("#Nm18_P#", QString::number(Nm18_P)).
            replace("#NWm18_P#", QString::number(NWm18_P)).
            replace("#NUm18_P#", QString::number(NUm18_P)).
            replace("#Nem18_P#", QString::number(Nem18_P)).
            replace("#NeWm18_P#", QString::number(NeWm18_P)).
            replace("#NeUm18_P#", QString::number(NeUm18_P)).

            replace("#Nw18_P#", QString::number(Nw18_P)).
            replace("#NWw18_P#", QString::number(NWw18_P)).
            replace("#NUw18_P#", QString::number(NUw18_P)).
            replace("#New18_P#", QString::number(New18_P)).
            replace("#NeWw18_P#", QString::number(NeWw18_P)).
            replace("#NeUw18_P#", QString::number(NeUw18_P)).

            replace("#NP_#", QString::number(NP_)).
            replace("#NWP_#", QString::number(NWP_)).
            replace("#NUP_#", QString::number(NUP_)).
            replace("#NeP_#", QString::number(NeP_)).
            replace("#NeWP_#", QString::number(NeWP_)).
            replace("#NeUP_#", QString::number(NeUP_)).

            replace("#NmP_#", QString::number(NmP_)).
            replace("#NWmP_#", QString::number(NWmP_)).
            replace("#NUmP_#", QString::number(NUmP_)).
            replace("#NemP_#", QString::number(NemP_)).
            replace("#NeWmP_#", QString::number(NeWmP_)).
            replace("#NeUmP_#", QString::number(NeUmP_)).

            replace("#NwP_#", QString::number(NwP_)).
            replace("#NWwP_#", QString::number(NWwP_)).
            replace("#NUwP_#", QString::number(NUwP_)).
            replace("#NewP_#", QString::number(NewP_)).
            replace("#NeWwP_#", QString::number(NeWwP_)).
            replace("#NeUwP_#", QString::number(NeUwP_)).

            replace("#NI#", QString::number(NI)).
            replace("#NIW#", QString::number(NIW)).
            replace("#NIU#", QString::number(NIU)).
            replace("#NIe#", QString::number(NIe)).
            replace("#NIeW#", QString::number(NIeW)).
            replace("#NIeU#", QString::number(NIeU)).

            replace("#NII#", QString::number(NII)).
            replace("#NIIW#", QString::number(NIIW)).
            replace("#NIIU#", QString::number(NIIU)).
            replace("#NIIe#", QString::number(NIIe)).
            replace("#NIIeW#", QString::number(NIIeW)).
            replace("#NIIeU#", QString::number(NIIeU)).

            replace("#NS#", QString::number(NS)).
            replace("#NSW#", QString::number(NSW)).
            replace("#NSU#", QString::number(NSU)).
            replace("#NSe#", QString::number(NSe)).
            replace("#NSeW#", QString::number(NSeW)).
            replace("#NSeU#", QString::number(NSeU));

        delete query_f8;
    } else {
        delete query_f8;
        QMessageBox::warning(this,
                             "Ошибка при выборе данных",
                             "При выборе данных для отчёта \"Форма № 8\" ничего не получено.\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }

    // сохранение контента шаблона
    QFile f_content_save(tmp_folder + "/content.xml");
    f_content_save.open(QIODevice::WriteOnly);
    f_content_save.write(s_content.toUtf8());
    f_content_save.close();


    // ===========================
    // архивация шаблона
    // ===========================
    QString fname_res = data_app.path_out + "_REPORTS_/" +
            QDate::currentDate().toString("yyyy-MM-dd") + "  -  " + data_app.smo_short + " - " + data_app.filial_name + "  -  form_8 - " + repDate.toString("yyyy-MM-dd") + (curDate!=repDate ? (" - " + curDate.toString("yyyy-MM-dd")) : "") + ".odt";

    QFile::remove(fname_res);
    while (QFile::exists(fname_res)) {
        if (QMessageBox::warning(this,
                                 "Ошибка сохранения печатной формы \"Форма № 8\"",
                                 "Не удалось сохранить шаблон печатной формы \"Форма № 8\": \n" + rep_template +
                                 "\n\nво временную папку\n" + fname_res +
                                 "\n\nПроверьте, не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                 QMessageBox::Retry|QMessageBox::Abort,
                                 QMessageBox::Retry)==QMessageBox::Abort) {
            this->setCursor(Qt::ArrowCursor);
            ui->bn_print->setEnabled(true);
            return;
        }
        QFile::remove(fname_res);
    }
    if (!QFile::copy(rep_template, fname_res) || !QFile::exists(fname_res)) {
        QMessageBox::warning(this, "Ошибка копирования шаблона", "Не удалось скопировать шаблон печатной формы \"Форма № 8\": \n" + rep_template + "\n\nво временную папку\n" + fname_res + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    com = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname_res + "\" \"" + tmp_folder + "/content.xml\"";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка обновления контента",
                             "При добавлении нового контента в шаблон печатной формы \"Форма № 8\" произошла непредвиденная ошибка\n\nОпреация прервана.");
    }


    // ===========================
    // собственно открытие шаблона
    // ===========================
    // открытие полученного ODT
    long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);
    this->setCursor(Qt::ArrowCursor);
    if (result<32) {
        QMessageBox::warning(this,
                             "Ошибка при открытии файла печатной формы \"Форма № 8\"",
                             "Файл печатной формы \"Форма № 8\" не найден:\n\n" + fname_res +
                             "\nКод ошибки: " + QString::number(result) +
                             "\n\nПопробуйте открыть этот файл вручную.\n\nЕсли файл не создан или ошибка будет повторяться - обратитесь к разработчику.");
    }
    this->setCursor(Qt::ArrowCursor);
    ui->bn_print->setEnabled(true);
    close();
}

void print_form_8_wnd::on_ln_smo_director_textChanged(const QString &arg1) {
    data_app.smo_director = ui->ln_smo_director->text();
}

void print_form_8_wnd::on_ch_doc2_clicked(bool checked) {
    ui->ln_doc2_num->setEnabled(checked);
    ui->date_doc2->setEnabled(checked);
}
