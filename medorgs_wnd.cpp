#include "medorgs_wnd.h"
#include <QSqlError>
#include <QMessageBox>
#include <QInputDialog>
#include "ui_medorgs_wnd.h"
#include "sessions_wnd.h"

medorgs_wnd::medorgs_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, s_data_assig &data_assig, QWidget *parent)
 : db(db), data_app(data_app), settings(settings), data_assig(data_assig), QDialog(parent), ui(new Ui::medorgs_wnd)
{
    ui->setupUi(this);

    add_medsite_ter_w = NULL;

    ui->splitter_main->setStretchFactor(0,4);
    ui->splitter_main->setStretchFactor(1,3);
    ui->splitter->setStretchFactor(0,1);
    ui->splitter->setStretchFactor(1,1);
    ui->split_MO->setStretchFactor(0,4);
    ui->split_MO->setStretchFactor(1,3);
    ui->split_MS->setStretchFactor(0,1);
    ui->split_MS->setStretchFactor(1,1);

    this->setCursor(Qt::WaitCursor);
    QApplication::processEvents();
    refresh_medorgs_tab();
    refresh_medpart_type();
    refresh_medsites_tab();
    refresh_medsite_mo();
    refresh_medsite_mp();
    refresh_medsite_type();
    refresh_medters_tab();
    refresh_medter_mo();
    refresh_medter_spec();

    this->setCursor(Qt::ArrowCursor);
    QApplication::processEvents();
}

medorgs_wnd::~medorgs_wnd() {
    delete ui;
}


void medorgs_wnd::refresh_medorgs_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    medorgs_sql  = "select id, ocato, code_mo, name_mo, name_mo_full, address, is_head, code_head, phone, fax, email, www, comment, oid_mo, "
                   "       case mo_assig when 1 then 1 else NULL end as mo_assig, "
                   "       mo_chif_famio, mo_chif_famio_dp, mo_chif_state, mo_chif_state_dp, mo_chif_greetings, mo_contract_num, mo_contract_date, "
                   "       mo_1st_aid, mo_assig_type, mo_assig_age "
                   "  from spr_medorgs "
                   " where is_head=1 ";
    QString filter = ui->ln_filter->text();
    if (!filter.trimmed().isEmpty()) {
        medorgs_sql += " and upper(name_mo) like(upper('%" + filter + "%')) ";
    }
    medorgs_sql += " order by ocato, code_mo, is_head ; ";

    model_medorgs.setQuery(medorgs_sql,db);
    QString err2 = model_medorgs.lastError().driverText();

    // подключаем модель из БД
    ui->tab_medorgs->setModel(&model_medorgs);

    // обновляем таблицу
    ui->tab_medorgs->reset();

    // задаём ширину колонок
    ui->tab_medorgs->setColumnWidth( 0,  1);
    ui->tab_medorgs->setColumnWidth( 1, 50);     // ocato,
    ui->tab_medorgs->setColumnWidth( 2, 45);     // code_mo,
    ui->tab_medorgs->setColumnWidth( 3,200);     // name_mo,
    if (ui->ch_show_mo->isChecked()) {
        ui->tab_medorgs->setColumnWidth( 4,  1);     // name_mo_full,
        ui->tab_medorgs->setColumnWidth( 5,150);     // address,
        ui->tab_medorgs->setColumnWidth( 6, 30);     // is_head,
        ui->tab_medorgs->setColumnWidth( 7, 75);     // code_head,
        ui->tab_medorgs->setColumnWidth( 8, 99);     // phone,
        ui->tab_medorgs->setColumnWidth( 9, 99);     // fax,
        ui->tab_medorgs->setColumnWidth(10,130);     // email,
        ui->tab_medorgs->setColumnWidth(11, 40);     // www,
        ui->tab_medorgs->setColumnWidth(12, 40);     // comment
        ui->tab_medorgs->setColumnWidth(13,140);     // oid_mo
    } else {
        ui->tab_medorgs->setColumnWidth( 4,  1);     // name_mo_full,
        ui->tab_medorgs->setColumnWidth( 5,  2);     // address,
        ui->tab_medorgs->setColumnWidth( 6,  2);     // is_head,
        ui->tab_medorgs->setColumnWidth( 7,  2);     // code_head,
        ui->tab_medorgs->setColumnWidth( 8,  2);     // phone,
        ui->tab_medorgs->setColumnWidth( 9,  2);     // fax,
        ui->tab_medorgs->setColumnWidth(10,  2);     // email,
        ui->tab_medorgs->setColumnWidth(11,  2);     // www,
        ui->tab_medorgs->setColumnWidth(12,  2);     // comment
        ui->tab_medorgs->setColumnWidth(13,  2);     // oid_mo
    }
    if (ui->ch_show_letters->isChecked()) {
        ui->tab_medorgs->setColumnWidth(14, 30);     // mo_assig
        ui->tab_medorgs->setColumnWidth(15,100);     // mo_chif_famio,
        ui->tab_medorgs->setColumnWidth(16,100);     // mo_chif_famio_dp,
        ui->tab_medorgs->setColumnWidth(17,100);     // mo_chif_state,
        ui->tab_medorgs->setColumnWidth(18,100);     // mo_chif_state_dp,
        ui->tab_medorgs->setColumnWidth(19,150);     // mo_chif_greetings,
        ui->tab_medorgs->setColumnWidth(20, 60);     // mo_contract_num,
        ui->tab_medorgs->setColumnWidth(21, 75);     // mo_contract_date
    } else {
        ui->tab_medorgs->setColumnWidth(14,  2);     // mo_assig
        ui->tab_medorgs->setColumnWidth(15,  2);     // mo_chif_famio,
        ui->tab_medorgs->setColumnWidth(16,  2);     // mo_chif_famio_dp,
        ui->tab_medorgs->setColumnWidth(17,  2);     // mo_chif_state,
        ui->tab_medorgs->setColumnWidth(18,  2);     // mo_chif_state_dp,
        ui->tab_medorgs->setColumnWidth(19,  2);     // mo_chif_greetings,
        ui->tab_medorgs->setColumnWidth(20,  2);     // mo_contract_num,
        ui->tab_medorgs->setColumnWidth(21,  2);     // mo_contract_date
    }
    ui->tab_medorgs->setColumnWidth(22, 50);     // mo_1st_aid
    ui->tab_medorgs->setColumnWidth(23, 50);     // mo_assig_type,
    ui->tab_medorgs->setColumnWidth(24, 50);     // mo_assig_age

    // правим заголовки
    model_medorgs.setHeaderData( 1, Qt::Horizontal, ("ОКАТО"));
    model_medorgs.setHeaderData( 2, Qt::Horizontal, ("код\nЛПУ"));
    model_medorgs.setHeaderData( 3, Qt::Horizontal, ("название"));
    model_medorgs.setHeaderData( 4, Qt::Horizontal, ("полное название"));
    model_medorgs.setHeaderData( 5, Qt::Horizontal, ("адрес"));
    model_medorgs.setHeaderData( 6, Qt::Horizontal, ("Гл?"));
    model_medorgs.setHeaderData( 7, Qt::Horizontal, ("код глав."));
    model_medorgs.setHeaderData( 8, Qt::Horizontal, ("телефон"));
    model_medorgs.setHeaderData( 9, Qt::Horizontal, ("факс"));
    model_medorgs.setHeaderData(10, Qt::Horizontal, ("e-mail"));
    model_medorgs.setHeaderData(12, Qt::Horizontal, ("примечания"));
    model_medorgs.setHeaderData(13, Qt::Horizontal, ("ОИД ЛПУ"));
    model_medorgs.setHeaderData(14, Qt::Horizontal, ("ФЛ\nАГ"));
    model_medorgs.setHeaderData(15, Qt::Horizontal, ("руководитель"));
    model_medorgs.setHeaderData(16, Qt::Horizontal, ("руководитель\nд.п."));
    model_medorgs.setHeaderData(17, Qt::Horizontal, ("должность"));
    model_medorgs.setHeaderData(18, Qt::Horizontal, ("должность\nд.п."));
    model_medorgs.setHeaderData(19, Qt::Horizontal, ("приветствие"));
    model_medorgs.setHeaderData(20, Qt::Horizontal, ("№\nдоговор"));
    model_medorgs.setHeaderData(21, Qt::Horizontal, ("дата\nдоговора"));
    model_medorgs.setHeaderData(22, Qt::Horizontal, ("скорая\nпомощь"));
    model_medorgs.setHeaderData(23, Qt::Horizontal, ("способ\nприкр."));
    model_medorgs.setHeaderData(24, Qt::Horizontal, ("возрст.\nкатег."));
    ui->tab_medorgs->repaint();

    //data_assig.id = -1;
    /*data_assig.code_mo = "";
    //data_assig.name_mo = "";
    data_assig.code_mp = "";
    data_assig.code_ms = "";
    data_assig.code_mt = "";*/

    refresh_head_mdorgs();

    ui->ln_ocato->setText("");
    ui->ln_code_mo->setText("");
    ui->ln_oid_mo->setText("");
    ui->ln_name_mo->setText("");
    ui->ln_name_mo_full->setText("");
    ui->ln_address->setText("");
    ui->ch_is_head->setChecked(true);
    ui->combo_head_mo->setCurrentIndex(-1);
    ui->ln_phone->setText("");
    ui->ln_fax->setText("");
    ui->ln_email->setText("");
    ui->ln_www->setText("");
    ui->ln_comment->setText("");

    ui->ch_assig_settings->setChecked(false);
    ui->ch_assig_adult->setChecked(false);
    ui->ch_assig_child->setChecked(false);
    ui->ch_assig_terr->setChecked(false);
    ui->ch_assig_state->setChecked(false);
    ui->ch_assig_1st_aid->setChecked(false);
    ui->pan_assig_settings->setVisible(false);

    ui->ch_mo_assig_data->setChecked(false);
    ui->pan_mo_assig_data->setVisible(false);
    ui->ln_chif_fio->setText("");
    ui->ln_chif_fio_dp->setText("");
    ui->ln_chif_state->setText("");
    ui->ln_chif_state_dp->setText("");
    ui->ln_chif_greetings->setText("");
    ui->ln_contract_num->setText("");
    ui->date_contract->setDate(QDate::currentDate());

    ui->ch_mo_assig_data->setChecked(false);
    ui->pan_mo_assig_data->setVisible(false);

    //ui->bn_ok->setEnabled(true);

    this->setCursor(Qt::ArrowCursor);
}

void medorgs_wnd::refresh_head_mdorgs() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка медорганизаций
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code_mo, '(' || code_mo || ') ' || name_mo "
                  " from public.spr_medorgs s "
                  " where is_head=1 "
                  " order by code_mo ; ";
    mySQL.exec(this, sql, QString("Справочник медорганизаций"), *query, true, db, data_app);
    ui->combo_head_mo->clear();
    while (query->next()) {
        ui->combo_head_mo->addItem(query->value(1).toString(), query->value(0).toString());
    }
    ui->combo_head_mo->setCurrentIndex(-1);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void medorgs_wnd::on_bn_refresh_clicked() {
    refresh_medorgs_tab();
}

void medorgs_wnd::on_bn_add_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_medorgs || !ui->tab_medorgs->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица медорганизаций не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите ЛПУ из списка.");
        return;
    }

    // проверка, введен ли код МО
    if (ui->ln_code_mo->text().trimmed().isEmpty() ||
            ui->ln_code_mo->text().trimmed().length()!=6) {
        QMessageBox::warning(this, "Недостаточно данных",
                             "Реестровый номер медорганизации должен состоять из 6 цифр без разделителей и пробелов не может быть пустым.\n\n"
                             "Сохранение отменено.");
        return;
    }

    /*// проверка, введен ли ОИД МО
    if (!ui->ln_oid_mo->text().trimmed().isEmpty()) {
        if (ui->ln_oid_mo->text().trimmed().length()<10) {
            QMessageBox::warning(this, "Недостаточно данных",
                                 "ОИД медорганизации не может быть короче 10 символов.\n\n"
                                 "Сохранение отменено.");
            return;
        }
    }*/

    // проверка, введено ли название МО
    if (ui->ln_name_mo->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных",
                             "Название медицинской организации не может быть пустым.\n\n"
                             "Сохранение отменено.");
        return;
    }

    if (ui->ch_mo_assig_data->isChecked()) {
        if (ui->ln_chif_fio->text().trimmed().simplified().isEmpty()) {
            QMessageBox::warning(this, "Недостаточно данных",
                                 "Поле \"Фамилия И.О. руководителя\" не может быть пустым.\n\n"
                                 "Сохранение отменено.");
            return;
        }
        if (ui->ln_chif_fio_dp->text().trimmed().simplified().isEmpty()) {
            QMessageBox::warning(this, "Недостаточно данных",
                                 "Поле \"Фамилия И.О. руководителя, дательный падеж\" не может быть пустым.\n\n"
                                 "Сохранение отменено.");
            return;
        }
        if (ui->ln_chif_state->text().trimmed().simplified().isEmpty()) {
            QMessageBox::warning(this, "Недостаточно данных",
                                 "Поле \"Должность руководителя\" не может быть пустым.\n\n"
                                 "Сохранение отменено.");
            return;
        }
        if (ui->ln_chif_state_dp->text().trimmed().simplified().isEmpty()) {
            QMessageBox::warning(this, "Недостаточно данных",
                                 "Поле \"Должность руководителя, дательный падеж\" не может быть пустым.\n\n"
                                 "Сохранение отменено.");
            return;
        }
        if (ui->ln_chif_greetings->text().trimmed().simplified().isEmpty()) {
            QMessageBox::warning(this, "Недостаточно данных",
                                 "Поле \"Форма приветствия\" не может быть пустым.\n\n"
                                 "Сохранение отменено.");
            return;
        }
        if (ui->ln_contract_num->text().trimmed().simplified().isEmpty()) {
            QMessageBox::warning(this, "Недостаточно данных",
                                 "Поле \"Номер договора ЛПУ с СМО\" не может быть пустым.\n\n"
                                 "Сохранение отменено.");
            return;
        }
        if ( ui->date_contract->date()<=QDate(2000,1,1)
             || ui->date_contract->date()>=QDate::currentDate() ) {
            QMessageBox::warning(this, "Недостаточно данных",
                                 "Поле \"Дата договора ЛПУ с СМО\" должен быть позже 01.01.2000\n"
                                 "и раньше текущей даты.\n\n"
                                 "Сохранение отменено.");
            return;
        }
    }

    // проверим кода на уникальность
    QString sql = "select count(*) from spr_medorgs "
                  " where code_mo='" + ui->ln_code_mo->text().trimmed() + "' ; ";

    QSqlQuery *query = new QSqlQuery(db);
    if (!mySQL.exec(this, sql, "Проверка кода МО на уникальность", *query, true, db, data_app) || !query->next()) {
        QMessageBox::warning(this, "Непредвиденная ошибка",
                             "При попытке проверить код МО на уникальность произошла неожиданная ошибка.\n\n"
                             "Сохранение отменено.");
        delete query;
        return;
    }
    if (query->value(0).toInt()>0) {
        QMessageBox::warning(this, "Такой код медицинской организации уже есть",
                             "В базе данных уже есть медицинская организация с таким же реестровым номером. Реестровый номер МО должен быть уникальным.\n\n"
                             "Сохранение отменено.");
        delete query;
        return;
    }

    if (!ui->ln_oid_mo->text().trimmed().isEmpty()) {
        // проверим ОИД на уникальность
        QString sql_oid = "select count(*) from spr_medorgs "
                      " where oid_mo='" + ui->ln_oid_mo->text().trimmed() + "' ; ";

        QSqlQuery *query_oid = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_oid, "Проверка ОИД МО на уникальность", *query_oid, true, db, data_app) || !query_oid->next()) {
            QMessageBox::warning(this, "Непредвиденная ошибка",
                                 "При попытке проверить ОИД МО на уникальность произошла неожиданная ошибка.\n\n"
                                 "Сохранение отменено.");
            delete query_oid;
            return;
        }
        if (query_oid->value(0).toInt()>0) {
            QMessageBox::warning(this, "Такой ОИД медицинской организации уже есть",
                                 "В базе данных уже есть медицинская организация с таким же ОИД.\n"
                                 "ОИД МО должен быть уникальным.\n\n"
                                 "Сохранение отменено.");
            delete query;
            return;
        }
    }


    if (QMessageBox::question(this, "Нужно подтверждение",
                              "Добавить новую медицинскую организацию?\n",
                              QMessageBox::Yes|QMessageBox::Cancel,
                              QMessageBox::Yes)==QMessageBox::Yes) {
        db.transaction();
        // добавим МО
        QString sql;
        sql = "insert into spr_medorgs "
              " (code_mo, oid_mo, name_mo, name_mo_full, address, is_head, code_head, phone, email, fax, www, comment, mo_assig, "
              "  mo_chif_famio, mo_chif_famio_dp, mo_chif_state, mo_chif_state_dp, mo_chif_greetings, mo_contract_num, mo_contract_date, "
              "  mo_1st_aid, mo_assig_type, mo_assig_age) "

              "values ("
              "   '" + ui->ln_code_mo->text().trimmed().simplified()       + "' "
              " , '" + ui->ln_oid_mo->text().trimmed().simplified()        + "' "
              " , '" + ui->ln_name_mo->text().trimmed().simplified()       + "' "
              " , '" + ui->ln_name_mo_full->text().trimmed().simplified()  + "' "
              " , '" + ui->ln_address->text().trimmed().simplified()       + "' "
              " , '" + QString(ui->ch_is_head->isChecked() ? "1" : "0")    + "' "
              " , '" + QString(ui->ch_is_head->isChecked() ?
                          ( ui->ln_code_mo->text().trimmed().simplified()) :
                          ( ui->combo_head_mo->currentData().toString() )) + "' "
              " , '" + ui->ln_phone->text().trimmed().simplified()         + "' "
              " , '" + ui->ln_email->text().trimmed().simplified()         + "' "
              " , '" + ui->ln_fax->text().trimmed().simplified()           + "' "
              " , '" + ui->ln_www->text().trimmed().simplified()           + "' "
              " , '" + ui->ln_comment->toPlainText().trimmed().simplified()+ "' "
              " , '" + QString(ui->ch_mo_assig_data->isChecked()?"1":"0")  + "' ";

        if (ui->ch_mo_assig_data->isChecked()) {
            sql+= " , '" + ui->ln_chif_fio->text().trimmed().simplified()       + "' "
                  " , '" + ui->ln_chif_fio_dp->text().trimmed().simplified()    + "' "
                  " , '" + ui->ln_chif_state->text().trimmed().simplified()     + "' "
                  " , '" + ui->ln_chif_state_dp->text().trimmed().simplified()  + "' "
                  " , '" + ui->ln_chif_greetings->text().trimmed().simplified() + "' "
                  " , '" + ui->ln_contract_num->text().trimmed().simplified()   + "' "
                  " , '" + ui->date_contract->date().toString("yyyy-MM-dd")     + "' ";
        } else {
            sql+= " , NULL "
                  " , NULL "
                  " , NULL "
                  " , NULL "
                  " , NULL "
                  " , NULL "
                  " , NULL ";
        }

        if (ui->ch_assig_settings->isChecked()) {
            sql+= " , " + QString(ui->ch_assig_1st_aid->isChecked() ? "1" : "0") + " "
                  " , " + QString(ui->ch_assig_terr->isChecked()  ? (ui->ch_assig_state->isChecked() ? "0" : "1") : (ui->ch_assig_state->isChecked() ? "2" : "0")) + " "
                  " , " + QString(ui->ch_assig_child->isChecked() ? (ui->ch_assig_adult->isChecked() ? "0" : "1") : (ui->ch_assig_adult->isChecked() ? "2" : "0")) + " ";
        } else {
            sql+= " , 0 "
                  " , 0 "
                  " , 0 ";
        }
        sql+= ")  returning id ; ";

        QSqlQuery *query = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Добавление новой медорганизации", *query, true, db, data_app) || !query->next()) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка",
                                  "При попытке добавить новую медорганизацию произошла неожиданная ошибка.\n\n"
                                  "Сохранение отменено.");
            delete query;
            return;
        }
        //data_assig.id = query->value(0).toInt();
        //data_assig.code_mo = ui->ln_code_mo->text();
        //data_assig.name_mo = ui->ln_name_mo->text();
        delete query;
        db.commit();

        refresh_medorgs_tab();
    }
}

void medorgs_wnd::on_tab_medorgs_clicked(const QModelIndex &index) {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_medorgs || !ui->tab_medorgs->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица медорганизаций не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите ЛПУ из списка.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_medorgs->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        QModelIndex index = indexes.front();

        //data_assig.id = model_medorgs.data(model_medorgs.index(index.row(), 0), Qt::EditRole).toInt();
        //data_assig.code_mo = model_medorgs.data(model_medorgs.index(index.row(), 2), Qt::EditRole).toString();
        //data_assig.name_mo = model_medorgs.data(model_medorgs.index(index.row(), 3), Qt::EditRole).toString();

        ui->ln_ocato->setText(model_medorgs.data(model_medorgs.index(index.row(), 1), Qt::EditRole).toString());
        ui->ln_code_mo->setText(model_medorgs.data(model_medorgs.index(index.row(), 2), Qt::EditRole).toString());
        ui->ln_oid_mo->setText(model_medorgs.data(model_medorgs.index(index.row(), 13), Qt::EditRole).toString());
        ui->ln_name_mo->setText(model_medorgs.data(model_medorgs.index(index.row(), 3), Qt::EditRole).toString());
        ui->ln_name_mo_full->setText(model_medorgs.data(model_medorgs.index(index.row(), 4), Qt::EditRole).toString());
        ui->ln_address->setText(model_medorgs.data(model_medorgs.index(index.row(), 5), Qt::EditRole).toString());
        ui->ln_phone->setText(model_medorgs.data(model_medorgs.index(index.row(), 8), Qt::EditRole).toString());
        ui->ln_fax->setText(model_medorgs.data(model_medorgs.index(index.row(), 9), Qt::EditRole).toString());
        ui->ln_email->setText(model_medorgs.data(model_medorgs.index(index.row(),10), Qt::EditRole).toString());
        ui->ln_www->setText(model_medorgs.data(model_medorgs.index(index.row(),11), Qt::EditRole).toString());
        ui->ln_comment->setText(model_medorgs.data(model_medorgs.index(index.row(),12), Qt::EditRole).toString());

        bool is_head_mo = model_medorgs.data(model_medorgs.index(index.row(), 6), Qt::EditRole).toInt()==1;
        ui->ch_is_head->setChecked(is_head_mo);
        ui->combo_head_mo->setCurrentIndex(ui->combo_head_mo->findData(model_medorgs.data(model_medorgs.index(index.row(), 7), Qt::EditRole).toString()));
        ui->combo_head_mo->setEnabled(!is_head_mo);
        ui->lab_is_head->setEnabled(is_head_mo);

        ui->combo_medsite_mo->setCurrentIndex(ui->combo_medsite_mo->findData(ui->ln_code_mo->text()));
        refresh_medsite_mp();
        ui->combo_medter_mo->setCurrentIndex(ui->combo_medsite_mo->findData(ui->ln_code_mo->text()));

        // параметря прикрепления к МО
        ui->ch_assig_1st_aid->setChecked ( model_medorgs.data(model_medorgs.index(index.row(),22), Qt::EditRole).toInt()> 0 );
        ui->ch_assig_child->setChecked   ( model_medorgs.data(model_medorgs.index(index.row(),24), Qt::EditRole).toInt()==0
                                        || model_medorgs.data(model_medorgs.index(index.row(),24), Qt::EditRole).toInt()==1 );
        ui->ch_assig_adult->setChecked   ( model_medorgs.data(model_medorgs.index(index.row(),24), Qt::EditRole).toInt()==0
                                        || model_medorgs.data(model_medorgs.index(index.row(),24), Qt::EditRole).toInt()==2 );
        ui->ch_assig_terr->setChecked    ( model_medorgs.data(model_medorgs.index(index.row(),23), Qt::EditRole).toInt()==0
                                        || model_medorgs.data(model_medorgs.index(index.row(),23), Qt::EditRole).toInt()==1 );
        ui->ch_assig_state->setChecked   ( model_medorgs.data(model_medorgs.index(index.row(),23), Qt::EditRole).toInt()==0
                                        || model_medorgs.data(model_medorgs.index(index.row(),23), Qt::EditRole).toInt()==2 );
        ui->ch_assig_settings->setChecked( model_medorgs.data(model_medorgs.index(index.row(),22), Qt::EditRole).toInt()> 0
                                        || model_medorgs.data(model_medorgs.index(index.row(),23), Qt::EditRole).toInt()> 0
                                        || model_medorgs.data(model_medorgs.index(index.row(),24), Qt::EditRole).toInt()> 0 );
        ui->pan_assig_settings->setVisible(ui->ch_assig_settings->isChecked());

        // формирование писем
        ui->ch_mo_assig_data->setChecked(model_medorgs.data(model_medorgs.index(index.row(),14), Qt::EditRole).toInt()>0);
        ui->pan_mo_assig_data->setVisible(model_medorgs.data(model_medorgs.index(index.row(),14), Qt::EditRole).toInt()>0);
        f_assig = (model_medorgs.data(model_medorgs.index(index.row(),14), Qt::EditRole).toInt()>0);
        ui->ln_chif_fio->setText(model_medorgs.data(model_medorgs.index(index.row(),15), Qt::EditRole).toString());
        ui->ln_chif_fio_dp->setText(model_medorgs.data(model_medorgs.index(index.row(),16), Qt::EditRole).toString());
        ui->ln_chif_state->setText(model_medorgs.data(model_medorgs.index(index.row(),17), Qt::EditRole).toString());
        ui->ln_chif_state_dp->setText(model_medorgs.data(model_medorgs.index(index.row(),18), Qt::EditRole).toString());
        ui->ln_chif_greetings->setText(model_medorgs.data(model_medorgs.index(index.row(),19), Qt::EditRole).toString());
        ui->ln_contract_num->setText(model_medorgs.data(model_medorgs.index(index.row(),20), Qt::EditRole).toString());
        ui->date_contract->setDate(model_medorgs.data(model_medorgs.index(index.row(),21), Qt::EditRole).toDate());

        //refresh_medter_mp();

    } else {
        //data_assig.id = -1;
        //data_assig.code_mo = "";
        //data_assig.name_mo = "";

        ui->ln_code_mo->setText("");
        ui->ln_name_mo->setText("");
        ui->ln_oid_mo->setText("");
        ui->ln_name_mo_full->setText("");
        ui->ln_address->setText("");
        ui->ch_is_head->setChecked(true);
        ui->combo_head_mo->setCurrentIndex(-1);
        ui->ln_phone->setText("");
        ui->ln_fax->setText("");
        ui->ln_email->setText("");
        ui->ln_www->setText("");
        ui->ln_comment->setText("");

        ui->ch_assig_child->setChecked    (false);
        ui->ch_assig_adult->setChecked    (false);
        ui->ch_assig_terr->setChecked     (false);
        ui->ch_assig_state->setChecked    (false);
        ui->ch_assig_1st_aid->setChecked  (false);
        ui->ch_assig_settings->setChecked (false);
        ui->pan_assig_settings->setVisible(false);

        ui->ch_mo_assig_data->setChecked(false);
        ui->pan_mo_assig_data->setVisible(false);
        ui->ln_chif_fio->setText("");
        ui->ln_chif_fio_dp->setText("");
        ui->ln_chif_state->setText("");
        ui->ln_chif_state_dp->setText("");
        ui->ln_chif_greetings->setText("");
        ui->ln_contract_num->setText("");
        ui->date_contract->setDate(QDate::currentDate());
    }

    refresh_medparts_tab();
    refresh_medsites_tab();
    refresh_medters_tab();

    //refresh_medsites_tab();
    //refresh_medsite_mo();

    //refresh_medters_tab();
    //refresh_medter_mo();

    if (ui->ch_medsites_mo->isChecked()) {
        refresh_medsites_tab();
    }
    if (ui->ch_medters_mo->isChecked()) {
        refresh_medters_tab();
    }

    refresh_medsite_ters_tab();
}

void medorgs_wnd::on_bn_edit_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_medorgs || !ui->tab_medorgs->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица медорганизаций не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите ЛПУ из списка.");
        return;
    }

    // проверка, введен ли код организации
    if (ui->ln_code_mo->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Реестровый номер организации не может быть пустым.\n\nИзменение отменено.");
        return;
    }

    // проверка, введено ли название организации
    if (ui->ln_name_mo->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Название организации не может быть пустым.\n\nИзменение отменено.");
        return;
    }

    if (ui->ch_mo_assig_data->isChecked()) {
        if (ui->ln_chif_fio->text().trimmed().simplified().isEmpty()) {
            QMessageBox::warning(this, "Недостаточно данных",
                                 "Поле \"Фамилия И.О. руководителя\" не может быть пустым.\n\n"
                                 "Сохранение отменено.");
            return;
        }
        if (ui->ln_chif_fio_dp->text().trimmed().simplified().isEmpty()) {
            QMessageBox::warning(this, "Недостаточно данных",
                                 "Поле \"Фамилия И.О. руководителя, дательный падеж\" не может быть пустым.\n\n"
                                 "Сохранение отменено.");
            return;
        }
        if (ui->ln_chif_state->text().trimmed().simplified().isEmpty()) {
            QMessageBox::warning(this, "Недостаточно данных",
                                 "Поле \"Должность руководителя\" не может быть пустым.\n\n"
                                 "Сохранение отменено.");
            return;
        }
        if (ui->ln_chif_state_dp->text().trimmed().simplified().isEmpty()) {
            QMessageBox::warning(this, "Недостаточно данных",
                                 "Поле \"Должность руководителя, дательный падеж\" не может быть пустым.\n\n"
                                 "Сохранение отменено.");
            return;
        }
        if (ui->ln_chif_greetings->text().trimmed().simplified().isEmpty()) {
            QMessageBox::warning(this, "Недостаточно данных",
                                 "Поле \"Форма приветствия\" не может быть пустым.\n\n"
                                 "Сохранение отменено.");
            return;
        }
        if (ui->ln_contract_num->text().trimmed().simplified().isEmpty()) {
            QMessageBox::warning(this, "Недостаточно данных",
                                 "Поле \"Номер договора ЛПУ с СМО\" не может быть пустым.\n\n"
                                 "Сохранение отменено.");
            return;
        }
        if ( ui->date_contract->date()<=QDate(2000,1,1)
             || ui->date_contract->date()>=QDate::currentDate() ) {
            QMessageBox::warning(this, "Недостаточно данных",
                                 "Поле \"Дата договора ЛПУ с СМО\" должен быть позже 01.01.2000\n"
                                 "и раньше текущей даты.\n\n"
                                 "Сохранение отменено.");
            return;
        }
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_medorgs->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Изменить данные выбранной медицинской организации?",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        QModelIndex index = indexes.front();
        QString ID = QString::number(model_medorgs.data(model_medorgs.index(index.row(), 0), Qt::EditRole).toInt());


        // проверим кода на уникальность
        QString sql = "select count(*) from spr_medorgs "
                      " where "
                      "    id<>" + ID + " and is_head=1 and "
                      "    code_mo='" + ui->ln_code_mo->text().trimmed() + "' ; ";

        QSqlQuery *query = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Проверка кода МО на уникальность", *query, true, db, data_app) || !query->next()) {
            QMessageBox::warning(this,
                                 "Непредвиденная ошибка",
                                 "При попытке проверить код МО на уникальность произошла неожиданная ошибка.\n\n"
                                 "Изменение отменено.");
            delete query;
            return;
        }
        if (query->value(0).toInt()>0) {
            QMessageBox::warning(this,
                                  "Такой код медицинской организации уже есть",
                                  "В базе данных уже есть медицинская организация с таким же реестровым номером.\n"
                                  "Реестровый номер МО должен быть уникальным.\n\n"
                                  "Изменение отменено.");
            delete query;
            return;
        }

        if (!ui->ln_oid_mo->text().trimmed().isEmpty()) {
            // проверим ОИД на уникальность
            QString sql_oid = "select count(*) from spr_medorgs "
                          " where "
                          "    id<>" + ID + " and "
                          "    oid_mo='" + ui->ln_oid_mo->text().trimmed() + "' ; ";

            QSqlQuery *query_oid = new QSqlQuery(db);
            if (!mySQL.exec(this, sql_oid, "Проверка ОИД МО на уникальность", *query_oid, true, db, data_app) || !query_oid->next()) {
                QMessageBox::warning(this,
                                     "Непредвиденная ошибка",
                                     "При попытке проверить ОИД МО на уникальность произошла неожиданная ошибка.\n\n"
                                     "Изменение отменено.");
                delete query_oid;
                return;
            }
            if (query_oid->value(0).toInt()>0) {
                QMessageBox::warning(this,
                                      "Такой ОИД медицинской организации уже есть",
                                      "В базе данных уже есть медицинская организация с таким же ОИД.\n"
                                      "ОИД МО должен быть уникальным.\n\n"
                                      "Изменение отменено.");
                delete query_oid;
                return;
            }
        }


        // проверим названия на уникальность
        QString sql2 ="select count(*) from spr_medorgs "
                      " where "
                      "    id<>" + ID + " and "
                      "    name_mo='" + ui->ln_name_mo->text().trimmed() + "' ; ";

        QSqlQuery *query2 = new QSqlQuery(db);
        if (!mySQL.exec(this, sql2, "Проверка названия МО на уникальность", *query2, true, db, data_app) || !query2->next()) {
            QMessageBox::warning(this,
                                 "Непредвиденная ошибка",
                                 "При попытке проверить названия МО на уникальность произошла неожиданная ошибка.\n\n"
                                 "Изменение отменено.");
            delete query2;
            return;
        }
        if (query2->value(0).toInt()>0) {
            QMessageBox::warning(this,
                                  "Такой код медицинской организации уже есть",
                                  "В базе данных уже есть медицинская организация с таким же названием.\n"
                                  "Название МО должен быть уникальным.\n\n"
                                  "Изменение отменено.");
            delete query2;
            return;
        }


        if ( f_assig
             && !ui->ch_mo_assig_data->isChecked() ) {
            if (QMessageBox::warning(this, "Нужно подтверждение!",
                                     "Вы действительно хотите удалить все данные для генерации официальных писем по прикреплению к этой медицинской организации?\n"
                                     "Это - необратимая операция!\n"
                                     "Продлолжить?\n", QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel)==QMessageBox::Cancel)
                return;
        }


        db.transaction();
        // добавим
        QString sql_upd;
        sql_upd = "update spr_medorgs "
                  "   set "
                  "   ocato='" + ui->ln_ocato->text().trimmed().simplified()               + "' "
                  " , code_mo='" + ui->ln_code_mo->text().trimmed().simplified()           + "' "
                  " , oid_mo='" + ui->ln_oid_mo->text().trimmed().simplified()             + "' "
                  " , name_mo='" + ui->ln_name_mo->text().trimmed().simplified()           + "' "
                  " , name_mo_full='" + ui->ln_name_mo_full->text().trimmed().simplified() + "' "
                  " , address='" + ui->ln_address->text().trimmed().simplified()           + "' "
                  " , is_head='" + QString(ui->ch_is_head->isChecked() ? "1" : "0")        + "' "
                  " , code_head='" + QString( ui->ch_is_head->isChecked()
                                      ? ui->ln_code_mo->text().trimmed().simplified()
                                      : ui->combo_head_mo->currentData().toString() )      + "' "
                  " , phone='" + ui->ln_phone->text().trimmed().simplified()               + "' "
                  " , fax='" + ui->ln_fax->text().trimmed().simplified()                   + "' "
                  " , email='" + ui->ln_email->text().trimmed().simplified()               + "' "
                  " , www='" + ui->ln_www->text().trimmed().simplified()                   + "' "
                  " , comment='" + ui->ln_comment->toPlainText().trimmed().simplified()    + "' "
                  " , mo_assig=" + QString(ui->ch_mo_assig_data->isChecked() ? "1" : "0");

        if (ui->ch_assig_settings->isChecked()) {
            sql_upd+= " , mo_1st_aid    =" + QString(ui->ch_assig_1st_aid->isChecked() ? "1" : "0") + " "
                      " , mo_assig_type =" + QString(ui->ch_assig_terr->isChecked()  ? (ui->ch_assig_state->isChecked() ? "0" : "1") : (ui->ch_assig_state->isChecked() ? "2" : "0")) + " "
                      " , mo_assig_age  =" + QString(ui->ch_assig_child->isChecked() ? (ui->ch_assig_adult->isChecked() ? "0" : "1") : (ui->ch_assig_adult->isChecked() ? "2" : "0")) + " ";
        } else {
            sql_upd+= " , mo_1st_aid    = 0 "
                      " , mo_assig_type = 0 "
                      " , mo_assig_age  = 0 ";
        }

        if (ui->ch_mo_assig_data->isChecked()) {
            sql_upd+= " , mo_chif_famio='" + ui->ln_chif_fio->text().trimmed().simplified()           + "' "
                      " , mo_chif_famio_dp='" + ui->ln_chif_fio_dp->text().trimmed().simplified()     + "' "
                      " , mo_chif_state='" + ui->ln_chif_state->text().trimmed().simplified()         + "' "
                      " , mo_chif_state_dp='" + ui->ln_chif_state_dp->text().trimmed().simplified()   + "' "
                      " , mo_chif_greetings='" + ui->ln_chif_greetings->text().trimmed().simplified() + "' "
                      " , mo_contract_num='" + ui->ln_contract_num->text().trimmed().simplified()     + "' "
                      " , mo_contract_date='" + ui->date_contract->date().toString("yyyy-MM-dd")      + "' ";
        } else {
            sql_upd+= " , mo_chif_famio=NULL "
                      " , mo_chif_famio_dp=NULL "
                      " , mo_chif_state=NULL "
                      " , mo_chif_state_dp=NULL "
                      " , mo_chif_greetings=NULL "
                      " , mo_contract_num=NULL "
                      " , mo_contract_date=NULL ";
        }
        sql_upd+= " where id=" + ID + " ; ";

        QSqlQuery *query_upd = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_upd, "Обновление медорганизации", *query_upd, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке изменить данные медицинской организации произошла неожиданная ошибка.\n\nИзменение отменено.");
            delete query_upd;
            return;
        }
        //data_assig.code_mo = ui->ln_code_mo->text();
        //data_assig.name_mo = ui->ln_name_mo->text();
        delete query_upd;
        db.commit();

        refresh_medorgs_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void medorgs_wnd::on_bn_delete_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_medorgs || !ui->tab_medorgs->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица медорганизаций не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите ЛПУ из списка.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_medorgs->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Удалить выбранную запись из базы?\n(Все лица, приписанные к удаляемой фирме будут от неё откреплены)\n",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        QModelIndex index = indexes.front();
        QString ID = QString::number(model_medorgs.data(model_medorgs.index(index.row(), 0), Qt::EditRole).toInt());

        QString sql = "delete from spr_medorgs "
                      " where id=" + ID + " ; ";
        QSqlQuery *query2 = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Удаление медорганизации", *query2, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке удалить медицинскую организацию произошла неожиданная ошибка.\n\nУдаление отменено.");
            delete query2;
            return;
        }
        //data_assig.id = -1;
        //data_assig.code_mo = "";
        //data_assig.name_mo = "";
        delete query2;

        db.commit();

        refresh_medorgs_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void medorgs_wnd::on_bn_ocato_clicked() {
    //ui->ln_ocato->setText(data_app.ocato);
}
/*
void medorgs_wnd::on_ln_ocato_editingFinished() {
    ui->bn_ok->setEnabled(false);   }
void medorgs_wnd::on_ln_code_mo_editingFinished() {
    ui->bn_ok->setEnabled(false);   }
void medorgs_wnd::on_ln_name_mo_editingFinished() {
    ui->bn_ok->setEnabled(false);   }
void medorgs_wnd::on_ln_name_mo_full_editingFinished() {
    ui->bn_ok->setEnabled(false);   }
void medorgs_wnd::on_combo_head_mo_activated(int index) {
    ui->bn_ok->setEnabled(false);   }
void medorgs_wnd::on_ln_phone_editingFinished() {
    ui->bn_ok->setEnabled(false);   }
void medorgs_wnd::on_ln_fax_editingFinished() {
    ui->bn_ok->setEnabled(false);   }
void medorgs_wnd::on_ln_email_editingFinished() {
    ui->bn_ok->setEnabled(false);   }
void medorgs_wnd::on_ln_www_editingFinished() {
    ui->bn_ok->setEnabled(false);   }
void medorgs_wnd::on_ln_comment_editingFinished() {
    ui->bn_ok->setEnabled(false);   }
*/
void medorgs_wnd::on_ch_is_head_clicked(bool checked) {
    ui->combo_head_mo->setEnabled(!checked);
    ui->lab_is_head->setEnabled(checked);
}


void medorgs_wnd::refresh_medparts_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    QString medparts_sql;
    medparts_sql  = "select s.id, s.code_mo, s.code_mp, s.name_mp, s.type_mp, '('||s.type_mp||') '||t.text as type_mp_text ";
    medparts_sql += " from spr_medparts s left join spr_medparts_types t on (s.type_mp=t.code) ";

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_medorgs->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        QModelIndex index = indexes.front();
        QString code_mo = model_medorgs.data(model_medorgs.index(index.row(), 2), Qt::EditRole).toString();
        medparts_sql += " where code_mo='" + code_mo + "' ";
    }
    medparts_sql += " order by s.code_mo, s.code_mp ; ";
    //QInputDialog::getText(this, "", "", QLineEdit::Normal, medparts_sql);

    model_medparts.setQuery(medparts_sql,db);
    QString err2 = model_medparts.lastError().driverText();

    // подключаем модель из БД
    ui->tab_medparts->setModel(&model_medparts);

    // обновляем таблицу
    ui->tab_medparts->reset();

    // задаём ширину колонок
    ui->tab_medparts->setColumnWidth( 0,  1);   // id,
    ui->tab_medparts->setColumnWidth( 1, 50);   // code_mo,
    ui->tab_medparts->setColumnWidth( 2, 50);   // code_mp,
    ui->tab_medparts->setColumnWidth( 3,200);   // name_mp,
    ui->tab_medparts->setColumnWidth( 4,  1);   // type_mp,
    ui->tab_medparts->setColumnWidth( 5,200);   // type_mp_text
    // правим заголовки
    model_medparts.setHeaderData( 1, Qt::Horizontal, ("ЛПУ"));
    model_medparts.setHeaderData( 2, Qt::Horizontal, ("код"));
    model_medparts.setHeaderData( 4, Qt::Horizontal, ("подразделение"));
    model_medparts.setHeaderData( 5, Qt::Horizontal, ("тип"));
    model_medparts.setHeaderData( 6, Qt::Horizontal, ("тип подразд."));
    ui->tab_medparts->repaint();

    /*data_medpart.id = -1;
    data_medpart.code_mo = "";
    data_medpart.name_mo = "";*/

    refresh_medpart_type();

    ui->ln_medpart_code->setText("");
    ui->ln_medpart_name->setText("");
    //ui->combo_medpart_type->setCurrentIndex(-1);

    //ui->bn_ok->setEnabled(true);

    this->setCursor(Qt::ArrowCursor);
}

void medorgs_wnd::refresh_medpart_type() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка подразделений
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, '(' || code || ') ' || text "
                  " from public.spr_medparts_types t "
                  " order by code ; ";
    //QInputDialog::getText(this, "", "", QLineEdit::Normal, sql);

    mySQL.exec(this, sql, QString("Справочник типов участков для таблицы мед.участков"), *query, true, db, data_app);
    ui->combo_medpart_type->clear();
    while (query->next()) {
        ui->combo_medpart_type->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    //ui->combo_medpart_type->setCurrentIndex(-1);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}


void medorgs_wnd::refresh_medsites_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    QString medsites_sql;
    medsites_sql  = "select ms.id, ms.code_mo, ms.code_mp, ms.code_ms, ms.name_ms, ms.type_ms, '('||ms.type_ms||') '||t.text as type_ms_text, mo.name_mo, mp.name_mp ";
    medsites_sql += " from spr_medsites ms "
                    "      left join spr_medsites_types t on (ms.type_ms=t.code) "
                    "      left join spr_medorgs  mo on(mo.code_mo=ms.code_mo and mo.is_head=1)"
                    "      left join spr_medparts mp on(mp.code_mo=ms.code_mo and mp.code_mp=ms.code_mp)";

    //QInputDialog::getText(this, "", "", QLineEdit::Normal, medsites_sql);

    if (ui->ch_medsites_mo->isChecked()) {
        // проверим выделенную строку
        QModelIndexList indexes = ui->tab_medorgs->selectionModel()->selection().indexes();

        if (indexes.size()>0) {
            QModelIndex index = indexes.front();
            QString code_mo = model_medorgs.data(model_medorgs.index(index.row(), 2), Qt::EditRole).toString();
            medsites_sql += " where mo.code_mo='" + code_mo + "' ";
        }
    }
    medsites_sql += " order by ms.code_mo, ms.code_mp, ms.code_ms ";

    model_medsites.setQuery(medsites_sql,db);
    QString err2 = model_medsites.lastError().driverText();

    // подключаем модель из БД
    ui->tab_medsites->setModel(&model_medsites);

    // обновляем таблицу
    ui->tab_medsites->reset();

    // задаём ширину колонок
    ui->tab_medsites->setColumnWidth( 0,  1);   // id,
    ui->tab_medsites->setColumnWidth( 1, 50);   // code_mo,
    ui->tab_medsites->setColumnWidth( 2, 80);   // code_mp,
    ui->tab_medsites->setColumnWidth( 3, 80);   // code_ms,
    ui->tab_medsites->setColumnWidth( 4,170);   // name_ms,
    ui->tab_medsites->setColumnWidth( 5,  1);   // type_ms,
    ui->tab_medsites->setColumnWidth( 6,160);   // type_ms_text
    ui->tab_medsites->setColumnWidth( 7,  1);   // name_mo,
    ui->tab_medsites->setColumnWidth( 8,  1);   // name_mp,
    // правим заголовки
    model_medsites.setHeaderData( 1, Qt::Horizontal, ("ЛПУ"));
    model_medsites.setHeaderData( 2, Qt::Horizontal, ("подразд."));
    model_medsites.setHeaderData( 3, Qt::Horizontal, ("участок"));
    model_medsites.setHeaderData( 4, Qt::Horizontal, ("название участка"));
    model_medsites.setHeaderData( 5, Qt::Horizontal, ("тип"));
    model_medsites.setHeaderData( 6, Qt::Horizontal, ("тип участка"));
    ui->tab_medsites->repaint();

    /*data_medsite.id = -1;
    data_medsite.code_mo = "";
    data_medsite.name_mo = "";*/

    refresh_medsite_type();

    ui->ln_medsite_code->setText("");
    ui->ln_medsite_name->setText("");
    //ui->combo_medsite_type->setCurrentIndex(-1);

    //ui->bn_ok->setEnabled(true);

    this->setCursor(Qt::ArrowCursor);
}

void medorgs_wnd::refresh_medsite_type() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка подразделений
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, '(' || code || ') ' || text "
                  " from public.spr_medsites_types t "
                  " order by code ; ";
    //QInputDialog::getText(this, "", "", QLineEdit::Normal, sql);

    mySQL.exec(this, sql, QString("Справочник типов участков для таблицы мед.участков"), *query, true, db, data_app);
    ui->combo_medsite_type->clear();
    while (query->next()) {
        ui->combo_medsite_type->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    //ui->combo_medsite_type->setCurrentIndex(-1);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void medorgs_wnd::on_bn_refresh_medsites_clicked() {
    refresh_medsites_tab();
}

void medorgs_wnd::on_tab_medsites_clicked(const QModelIndex &index) {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_medsites || !ui->tab_medsites->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Поиск] и выберите человека из списка.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_medsites->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        QModelIndex index = indexes.front();

        //data_assig.code_ms = model_medorgs.data(model_medorgs.index(index.row(), 3), Qt::EditRole).toString();
        ui->combo_medsite_mo->setCurrentIndex(ui->combo_medsite_mo->findData(model_medsites.data(model_medsites.index(index.row(), 1), Qt::EditRole).toString()));
        //refresh_medsite_mp();
        ui->combo_medsite_mp->setCurrentIndex(ui->combo_medsite_mp->findData(model_medsites.data(model_medsites.index(index.row(), 2), Qt::EditRole).toString()));
        ui->ln_medsite_code->setText(model_medsites.data(model_medsites.index(index.row(), 3), Qt::EditRole).toString());
        ui->ln_medsite_name->setText(model_medsites.data(model_medsites.index(index.row(), 4), Qt::EditRole).toString());
        ui->combo_medsite_type->setCurrentIndex(ui->combo_medsite_type->findData(model_medsites.data(model_medsites.index(index.row(), 5), Qt::EditRole).toString()));
    } else {
        ui->ln_medsite_code->setText("");
        ui->ln_medsite_name->setText("");
        //ui->combo_medsite_type->setCurrentIndex(-1);
    }

    //ui->combo_medter_mp->setCurrentIndex(ui->combo_medter_mp->findData(model_medsites.data(model_medsites.index(index.row(), 2), Qt::EditRole).toString()));
    //ui->combo_medter_ms->setCurrentIndex(ui->combo_medter_ms->findData(model_medsites.data(model_medsites.index(index.row(), 3), Qt::EditRole).toString()));

    /*if (ui->ch_medters_ms->isChecked()) {
        refresh_medters_tab();
    }*/
    refresh_medsite_ters_tab();
}

void medorgs_wnd::on_bn_add_medsite_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_medorgs || !ui->tab_medorgs->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица медорганизаций не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите ЛПУ из списка.");
        return;
    }

    if (ui->combo_medsite_type->currentIndex()<0) {
        QMessageBox::warning(this, "Недостаточно данных", "Выберите тип нового участка.\n\nСохранение отменено.");
        return;
    }
    if (ui->ln_medsite_code->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Введите код участка.\n\nСохранение отменено.");
        return;
    }
    if (ui->ln_medsite_name->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Введите название участка.\n\nСохранение отменено.");
        return;
    }

    // проверим код на уникальность
    QString sql = "select count(*) from spr_medsites "
                  " where code_mo='" + ui->combo_medsite_mo->currentData().toString() + "' and "
                  "       code_mp='" + ui->combo_medsite_mp->currentData().toString() + "' and "
                  "       code_ms='" + ui->ln_medsite_code->text().trimmed() + "' ; ";

    QSqlQuery *query = new QSqlQuery(db);
    if (!mySQL.exec(this, sql, "Проверка кода участка на уникальность", *query, true, db, data_app) || !query->next()) {
        QMessageBox::warning(this, "Непредвиденная ошибка", "При попытке проверить код участка на уникальность произошла неожиданная ошибка.\n\nСохранение отменено.");
        delete query;
        return;
    }
    if (query->value(0).toInt()>0) {
        QMessageBox::warning(this,
                              "Такой код участка МО уже есть",
                              "В базе данных уже есть участок данного подразделения данной МО с таким же реестровым номером.\nКод участка в подразделении должен быть уникальным.\n\nСохранение отменено.");
        delete query;
        return;
    }


    if (QMessageBox::question(this,
                              "Нужно подтверждение",
                              "Добавить новый участок?\n",
                              QMessageBox::Yes|QMessageBox::Cancel,
                              QMessageBox::Yes)==QMessageBox::Yes) {
        db.transaction();
        // добавим МО
        QString sql = "insert into spr_medsites (code_mo, code_mp, code_ms, name_ms, type_ms) "
                      " values ("
                      "   '" + ui->combo_medsite_mo->currentData().toString() + "', "
                      "   '" + ui->combo_medsite_mp->currentData().toString() + "', "
                      "   '" + ui->ln_medsite_code->text().trimmed().simplified() + "', "
                      "   '" + ui->ln_medsite_name->text().trimmed().simplified() + "', "
                      "   '" + ui->combo_medsite_type->currentData().toString() + "') "
                      " returning id ; ";

        QSqlQuery *query = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Добавление нового участка", *query, true, db, data_app) || !query->next()) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке добавить новый участок произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query;
            return;
        }
        //data_assig.code_ms = "";
        delete query;
        db.commit();

        refresh_medsites_tab();
    }
}

void medorgs_wnd::on_bn_edit_medsite_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_medorgs || !ui->tab_medorgs->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица медорганизаций не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите ЛПУ из списка.");
        return;
    }


    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_medsites->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Изменить данные выбранного участка?",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        QModelIndex index = indexes.front();
        QString ID = QString::number(model_medsites.data(model_medsites.index(index.row(), 0), Qt::EditRole).toInt());


        // проверка
        if (ui->combo_medsite_mo->currentIndex()<0) {
            QMessageBox::warning(this, "Недостаточно данных", "Выберите медорганизацию, к которой относится новый участок.\n\nСохранение отменено.");
            return;
        }
        /*if (ui->combo_medsite_mp->currentIndex()<0) {
            QMessageBox::warning(this, "Недостаточно данных", "Выберите подразделение, к которому относится новый участок.\n\nСохранение отменено.");
            return;
        }*/
        if (ui->combo_medsite_type->currentIndex()<0) {
            QMessageBox::warning(this, "Недостаточно данных", "Выберите тип нового участка.\n\nСохранение отменено.");
            return;
        }
        if (ui->ln_medsite_code->text().trimmed().simplified().isEmpty()) {
            QMessageBox::warning(this, "Недостаточно данных", "Введите код участка.\n\nСохранение отменено.");
            return;
        }
        if (ui->ln_medsite_name->text().trimmed().simplified().isEmpty()) {
            QMessageBox::warning(this, "Недостаточно данных", "Введите название участка.\n\nСохранение отменено.");
            return;
        }

        // проверим код на уникальность
        QString sql = "select count(*) from spr_medsites "
                      " where code_mo='" + ui->combo_medsite_mo->currentData().toString() + "' and "
                      "       code_ms='" + ui->ln_medsite_code->text().trimmed() + "' and "
                      "       id<>" + ID + " ; ";

        QSqlQuery *query = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Проверка кода участка на уникальность", *query, true, db, data_app) || !query->next()) {
            QMessageBox::warning(this, "Непредвиденная ошибка", "При попытке проверить код участка на уникальность произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query;
            return;
        }
        if (query->value(0).toInt()>0) {
            QMessageBox::warning(this,
                                  "Такой код участка МО уже есть",
                                  "В базе данных уже есть участок того же подразделения той же МО, код которого совпадает с введённым.\nКод участка должен быть уникальным в пределаж МО / подразделения МО.\n\nСохранение отменено.");
            delete query;
            return;
        }


        db.transaction();
        // добавим фирму
        QString sql_upd = "update spr_medsites "
                      " set "
                      "   code_mo='" + ui->combo_medsite_mo->currentData().toString() + "', "
                      "   code_mp='" + ui->combo_medsite_mp->currentData().toString() + "', "
                      "   code_ms='" + ui->ln_medsite_code->text().trimmed().simplified() + "', "
                      "   name_ms='" + ui->ln_medsite_name->text().trimmed().simplified() + "', "
                      "   type_ms='" + ui->combo_medsite_type->currentData().toString() + "' "
                      " where id=" + ID + " ; ";

        QSqlQuery *query_upd = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_upd, "Обновление медорганизации", *query_upd, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке изменить данные медицинской организации произошла неожиданная ошибка.\n\nИзменение отменено.");
            delete query_upd;
            return;
        }
        //data_assig.code_ms = ui->ln_medsite_code->text();
        /*data_medsite.name_mo = ui->ln_name_mo->text();*/
        delete query;
        db.commit();

        refresh_medsites_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void medorgs_wnd::on_ch_medsites_mo_clicked() {
    refresh_medsites_tab();
}

void medorgs_wnd::on_bn_delete_medsite_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_medsites || !ui->tab_medsites->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Поиск] и выберите человека из списка.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_medsites->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Удалить выбранный участок из базы?\n",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }
        db.transaction();

        QModelIndex index = indexes.front();
        QString ID = QString::number(model_medsites.data(model_medsites.index(index.row(), 0), Qt::EditRole).toInt());


        QString sql = "delete from spr_medsites "
                      " where id=" + ID + " ; ";
        QSqlQuery *query2 = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Удаление участка", *query2, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке удалить участок произошла неожиданная ошибка.\n\nУдаление отменено.");
            delete query2;
            return;
        }
        delete query2;

        db.commit();

        refresh_medsites_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void medorgs_wnd::on_bn_refresh_medters_clicked() {
    refresh_medters_tab();
}


void medorgs_wnd::refresh_medters_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    QString medters_sql;
    medters_sql  = "select mt.id, mt.code_mo, mt.code_mt, mt.fio_mt, mt.snils_mt, mt.spec_mt, '('||mt.spec_mt||') '||spec.text as spec_mt_text ";
    medters_sql += " from spr_medters mt "
                   "      left join spr_medters_spec spec on(mt.spec_mt=spec.code) ";

    if (ui->ch_medters_mo->isChecked()) {
        // проверим выделенную строку
        QModelIndexList indexes = ui->tab_medorgs->selectionModel()->selection().indexes();

        if (indexes.size()>0) {
            QModelIndex index = indexes.front();
            QString code_mo = model_medorgs.data(model_medorgs.index(index.row(), 2), Qt::EditRole).toString();
            medters_sql += " where code_mo='" + code_mo + "' ";
        }
    }
    medters_sql += " order by mt.fio_mt ";

    model_medters.setQuery(medters_sql,db);
    QString err2 = model_medsites.lastError().driverText();

    // подключаем модель из БД
    ui->tab_medters->setModel(&model_medters);

    // обновляем таблицу
    ui->tab_medters->reset();

    // задаём ширину колонок
    ui->tab_medters->setColumnWidth( 0,  1);   // id,
    ui->tab_medters->setColumnWidth( 1, 50);   // code_mo,
    //ui->tab_medters->setColumnWidth( 2, 50);   // code_mp,
    //ui->tab_medters->setColumnWidth( 3, 50);   // code_ms,
    ui->tab_medters->setColumnWidth( 2,  1);   // code_mt,
    ui->tab_medters->setColumnWidth( 3,170);   // fio_mt,
    ui->tab_medters->setColumnWidth( 4, 80);   // snils_mt,
    ui->tab_medters->setColumnWidth( 5,  1);   // spec_mt,
    ui->tab_medters->setColumnWidth( 6,240);   // spec_mt_text
    //ui->tab_medters->setColumnWidth( 9,  1);   // prof_mt_text
    //ui->tab_medters->setColumnWidth(10, 50);   // prof_mt_text
    // правим заголовки
    model_medters.setHeaderData( 1, Qt::Horizontal, ("ЛПУ"));
    //model_medters.setHeaderData( 2, Qt::Horizontal, ("подр."));
    //model_medters.setHeaderData( 3, Qt::Horizontal, ("уч."));
    model_medters.setHeaderData( 2, Qt::Horizontal, ("код"));
    model_medters.setHeaderData( 3, Qt::Horizontal, ("ФИО"));
    model_medters.setHeaderData( 4, Qt::Horizontal, ("СНИЛС"));
    model_medters.setHeaderData( 5, Qt::Horizontal, ("сп."));
    model_medters.setHeaderData( 6, Qt::Horizontal, ("специализация"));
    //model_medters.setHeaderData(10, Qt::Horizontal, ("проф."));
    ui->tab_medters->repaint();

    /*data_medsite.id = -1;
    data_medsite.code_mo = "";
    data_medsite.name_mo = "";*/

    refresh_medter_spec();

    //ui->combo_medter_mo->setCurrentIndex(-1);
    //ui->combo_medter_mp->setCurrentIndex(-1);
    //ui->combo_medter_ms->setCurrentIndex(-1);
    ui->ln_medter_code->setText("");
    ui->ln_medter_fio->setText("");
    ui->ln_medter_snils->setText("");
    ui->combo_medter_spec->setCurrentIndex(-1);
    //ui->combo_medter_prof->setCurrentIndex(-1);

    //ui->bn_ok->setEnabled(true);

    this->setCursor(Qt::ArrowCursor);
}

void medorgs_wnd::refresh_medter_mo() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка медорганизаций
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code_mo, '(' || code_mo || ') ' || name_mo "
                  " from public.spr_medorgs s "
                  " where is_head=1 "
                  " order by code_mo ; ";
    mySQL.exec(this, sql, QString("Справочник медорганизаций для таблицы врачей"), *query, true, db, data_app);
    ui->combo_medter_mo->clear();
    while (query->next()) {
        ui->combo_medter_mo->addItem(query->value(1).toString(), query->value(0).toString());
    }
    //ui->combo_medter_mo->setCurrentIndex(-1);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

/*void medorgs_wnd::refresh_medter_mp() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка подразделений
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code_mp, '(' || code_mp || ') ' || name_mp "
                  " from public.spr_medparts u "
                  " where u.code_mo='" + ui->combo_medter_mo->currentData().toString() + "' "
                  " order by code_mp ; ";
    mySQL.exec(this, sql, QString("Справочник подразделений для таблицы врачей"), *query, true);
    //ui->combo_medter_mp->clear();
    while (query->next()) {
        ui->combo_medter_mp->addItem(query->value(1).toString(), query->value(0).toString());
    }
    //ui->combo_medter_mp->setCurrentIndex(-1);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}*/

/*void medorgs_wnd::refresh_medter_ms() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка подразделений
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code_ms, '(' || code_ms || ') ' || name_ms "
                  " from public.spr_medsites u "
                  " where u.code_mo='" + ui->combo_medter_mo->currentData().toString() + "' "
                  " order by code_ms ; ";
    mySQL.exec(this, sql, QString("Справочник участков для таблицы врачей"), *query, true);
    ui->combo_medter_ms->clear();
    while (query->next()) {
        ui->combo_medter_ms->addItem(query->value(1).toString(), query->value(0).toString());
    }
    //ui->combo_medter_ms->setCurrentIndex(-1);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}*/

void medorgs_wnd::refresh_medter_spec() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка подразделений
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, '(' || code || ') ' || text "
                  " from public.spr_medters_spec t "
                  " order by code ; ";
    mySQL.exec(this, sql, QString("Справочник спечиальностей для таблицы врачей"), *query, true, db, data_app);
    ui->combo_medter_spec->clear();
    while (query->next()) {
        ui->combo_medter_spec->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    //ui->combo_medter_spec->setCurrentIndex(-1);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}


void medorgs_wnd::on_tab_medters_clicked(const QModelIndex &index) {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_medsites || !ui->tab_medsites->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Поиск] и выберите человека из списка.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_medters->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        QModelIndex index = indexes.front();

        //data_assig.code_mt = model_medorgs.data(model_medorgs.index(index.row(), 4), Qt::EditRole).toString();

        ui->combo_medter_mo->setCurrentIndex(ui->combo_medter_mo->findData(model_medters.data(model_medters.index(index.row(), 1), Qt::EditRole).toString()));
        //refresh_medter_mp();
        //refresh_medter_ms();
        //ui->combo_medter_mp->setCurrentIndex(ui->combo_medter_mp->findData(model_medters.data(model_medters.index(index.row(), 2), Qt::EditRole).toString()));
        //ui->combo_medter_ms->setCurrentIndex(ui->combo_medter_ms->findData(model_medters.data(model_medters.index(index.row(), 3), Qt::EditRole).toString()));
        ui->ln_medter_code->setText(model_medters.data(model_medters.index(index.row(), 2), Qt::EditRole).toString());
        ui->ln_medter_fio->setText(model_medters.data(model_medters.index(index.row(), 3), Qt::EditRole).toString());
        ui->ln_medter_snils->setText(model_medters.data(model_medters.index(index.row(), 4), Qt::EditRole).toString());
        ui->combo_medter_spec->setCurrentIndex(ui->combo_medter_spec->findData(model_medters.data(model_medters.index(index.row(), 5), Qt::EditRole).toString()));
        //ui->combo_medter_prof->setCurrentIndex(model_medters.data(model_medters.index(index.row(), 9), Qt::EditRole).toInt());

    } else {
        //ui->combo_medter_mo->setCurrentIndex(-1);
        //ui->combo_medter_mp->setCurrentIndex(-1);
        //ui->combo_medter_ms->setCurrentIndex(-1);
        ui->ln_medter_code->setText("");
        ui->ln_medter_fio->setText("");
        ui->ln_medter_snils->setText("");
        //ui->combo_medter_spec->setCurrentIndex(-1);
        //ui->combo_medter_prof->setCurrentIndex(0);
    }
}

void medorgs_wnd::on_bn_add_medter_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_medorgs || !ui->tab_medorgs->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица медорганизаций не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите ЛПУ из списка.");
        return;
    }

    // проверка
    if (ui->combo_medter_mo->currentIndex()<0) {
        QMessageBox::warning(this, "Недостаточно данных", "Выберите медорганизацию, к которой относится новый врач.\n\nСохранение отменено.");
        return;
    }
    /*if (ui->combo_medter_mp->currentIndex()<0) {
        QMessageBox::warning(this, "Недостаточно данных", "Выберите подразделение, к которому относится новый врач.\n\nСохранение отменено.");
        return;
    }*/
    /*if (ui->combo_medter_ms->currentIndex()<0) {
        QMessageBox::warning(this, "Недостаточно данных", "Выберите участок, к которому прикреплён новый врач.\n\nСохранение отменено.");
        return;
    }*/
    if (ui->combo_medter_spec->currentIndex()<0) {
        QMessageBox::warning(this, "Недостаточно данных", "Выберите специализацию нового врача.\n\nСохранение отменено.");
        return;
    }
    /*if (ui->ln_medter_code->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Введите код врача.\n\nСохранение отменено.");
        return;
    }*/
    if (ui->ln_medter_fio->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Введите ФИО врача.\n\nСохранение отменено.");
        return;
    }
    /*if (ui->combo_medter_prof->currentIndex()==0) {
        QMessageBox::warning(this, "Недостаточно данных", "Выберите специализацию (врач/медсестра).\n\nСохранение отменено.");
        return;
    }*/
    if (ui->ln_medter_snils->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Введите СНИЛС врача.\n\nСохранение отменено.");
        return;
    }

    if (ui->ln_medter_snils->text().trimmed().isEmpty()) {
        // СНИЛС не введён - проверки не надо
    } else {
        // проверим СНИЛС на уникальность
        QString sql = "select count(*) from spr_medters "
                      " where snils_mt='" + ui->ln_medter_snils->text().trimmed() + "' ; ";

        QSqlQuery *query = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Проверка СНИЛС врача на уникальность", *query, true, db, data_app) || !query->next()) {
            QMessageBox::warning(this, "Непредвиденная ошибка", "При попытке проверить СНИЛС врача на уникальность произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query;
            return;
        }
        if (query->value(0).toInt()>0) {
            QMessageBox::warning(this,
                                  "Такой СНИЛС врача уже есть",
                                  "В базе данных уже есть врач с таким же СНИЛС.\nСНИЛС врача должен быть уникальным.\n\nСохранение отменено.");
            delete query;
            return;
        }
        delete query;
    }


    if (QMessageBox::question(this,
                              "Нужно подтверждение",
                              "Добавить нового врача?\n",
                              QMessageBox::Yes|QMessageBox::Cancel,
                              QMessageBox::Yes)==QMessageBox::Yes) {
        db.transaction();
        // добавим МО
        QString sql = "insert into spr_medters (code_mo, /*code_mp, code_ms,*/ code_mt, fio_mt, snils_mt, spec_mt/*, prof_mt*/) "
                      " values ("
                      "   '" + ui->combo_medter_mo->currentData().toString() + "', "
                      //"   '" + ui->combo_medter_mp->currentData().toString() + "', "
                      //"   '" + ui->combo_medter_ms->currentData().toString() + "', "
                      "   '" + ui->ln_medter_code->text().trimmed().simplified() + "', "
                      "   '" + ui->ln_medter_fio->text().trimmed().simplified() + "', "
                      "   '" + ui->ln_medter_snils->text().trimmed().simplified().replace("-","").replace(" ","") + "', "
                      "   '" + ui->combo_medter_spec->currentData().toString() + "') "
                      " returning id ; ";

        QSqlQuery *query = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Добавление нового врача", *query, true, db, data_app) || !query->next()) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке добавить нового врача произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query;
            return;
        }
        //data_assig.code_mt = ui->ln_code_mo->text();
        delete query;
        db.commit();

        refresh_medters_tab();
    }
}

void medorgs_wnd::on_combo_medter_mo_currentIndexChanged(int index) {
    /*refresh_medter_mp();
    refresh_medter_ms();*/
}

void medorgs_wnd::on_bn_edit_medter_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_medorgs || !ui->tab_medorgs->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица медорганизаций не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите ЛПУ из списка.");
        return;
    }


    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_medters->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Изменить данные выбранного врача?",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        QModelIndex index = indexes.front();
        QString ID = QString::number(model_medters.data(model_medters.index(index.row(), 0), Qt::EditRole).toInt());


        // проверка
        if (ui->combo_medter_mo->currentIndex()<0) {
            QMessageBox::warning(this, "Недостаточно данных", "Выберите медорганизацию, к которой относится врач.\n\nСохранение отменено.");
            return;
        }
        /*if (ui->combo_medter_mp->currentIndex()<0) {
            QMessageBox::warning(this, "Недостаточно данных", "Выберите подразделение, к которому относится врач.\n\nСохранение отменено.");
            return;
        }*/
        /*if (ui->combo_medter_ms->currentIndex()<0) {
            QMessageBox::warning(this, "Недостаточно данных", "Выберите участок, за которым закреплён врач.\n\nСохранение отменено.");
            return;
        }*/
        if (ui->combo_medter_spec->currentIndex()<0) {
            QMessageBox::warning(this, "Недостаточно данных", "Выберите специализацию врача.\n\nСохранение отменено.");
            return;
        }
        /*if (ui->ln_medter_code->text().trimmed().simplified().isEmpty()) {
            QMessageBox::warning(this, "Недостаточно данных", "Введите код врача.\n\nСохранение отменено.");
            return;
        }*/
        if (ui->ln_medter_snils->text().trimmed().simplified().isEmpty()) {
            QMessageBox::warning(this, "Недостаточно данных", "Введите СНИЛС врача.\n\nСохранение отменено.");
            return;
        }
        /*if (ui->combo_medter_prof->currentIndex()==0) {
            QMessageBox::warning(this, "Недостаточно данных", "Выберите специализацию (врач/медсестра).\n\nСохранение отменено.");
            return;
        }*/
        if (ui->ln_medter_fio->text().trimmed().simplified().isEmpty()) {
            QMessageBox::warning(this, "Недостаточно данных", "Введите ФИО врача.\n\nСохранение отменено.");
            return;
        }

        if (ui->ln_medter_snils->text().trimmed().isEmpty()) {
            // СНИЛС не введён - проверки не надо
        } else {
            // проверим код на уникальность
            QString sql = "select count(*) from spr_medters "
                          " where code_mo='" + ui->combo_medter_mo->currentData().toString() + "' and "
                          "       snils_mt='" + ui->ln_medter_snils->text().trimmed() + "' and "
                          "       id<>" + ID + " ; ";

            QSqlQuery *query = new QSqlQuery(db);
            if (!mySQL.exec(this, sql, "Проверка СНИЛС врача на уникальность", *query, true, db, data_app) || !query->next()) {
                QMessageBox::warning(this, "Непредвиденная ошибка", "При попытке проверить СНИЛС врача на уникальность произошла неожиданная ошибка.\n\nСохранение отменено.");
                delete query;
                return;
            }
            if (query->value(0).toInt()>0) {
                QMessageBox::warning(this,
                                      "Такой СНИЛС врача уже есть",
                                      "В базе данных уже есть врач с таким же СНИЛС.\nСНИЛС врача должен быть уникальным.\n\nСохранение отменено.");
                delete query;
                return;
            }
            delete query;
        }

        db.transaction();
        // добавим фирму
        QString sql_upd = "update spr_medters "
                      " set "
                      "   code_mo='" + ui->combo_medter_mo->currentData().toString() + "', "
                      //"   code_mp='" + ui->combo_medter_mp->currentData().toString() + "', "
                      //"   code_ms='" + ui->combo_medter_ms->currentData().toString() + "', "
                      "   code_mt='" + ui->ln_medter_code->text().trimmed().simplified() + "', "
                      "   fio_mt='" + ui->ln_medter_fio->text().trimmed().simplified() + "', "
                      "   snils_mt='" + ui->ln_medter_snils->text().trimmed().simplified() + "', "
                      "   spec_mt='" + ui->combo_medter_spec->currentData().toString() + "' "
                      " where id=" + ID + " ; ";

        QSqlQuery *query_upd = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_upd, "Обновление врача", *query_upd, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке изменить данные врача произошла неожиданная ошибка.\n\nИзменение отменено.");
            delete query_upd;
            return;
        }
        //data_assig.code_mt = ui->ln_medter_code->text();
        /*data_medter.code_mo = ui->ln_code_mo->text();
        data_medter.name_mo = ui->ln_name_mo->text();*/
        db.commit();

        refresh_medters_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
    //ui->bn_ok->setEnabled(true);
}

void medorgs_wnd::on_bn_delete_medter_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_medters || !ui->tab_medters->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Поиск] и выберите человека из списка.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_medters->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Удалить выбранного врача из базы?\n",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }
        db.transaction();

        QModelIndex index = indexes.front();
        QString ID = QString::number(model_medters.data(model_medters.index(index.row(), 0), Qt::EditRole).toInt());


        QString sql = "delete from spr_medters "
                      " where id=" + ID + " ; ";
        QSqlQuery *query2 = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Удаление участка", *query2, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке удалить участок произошла неожиданная ошибка.\n\nУдаление отменено.");
            delete query2;
            return;
        }
        delete query2;

        db.commit();

        refresh_medters_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void medorgs_wnd::on_bn_filter_clicked() {
    on_bn_refresh_clicked();
}

void medorgs_wnd::on_bn_clear_clicked() {
    ui->ln_filter->clear();
    on_bn_refresh_clicked();
}

void medorgs_wnd::on_ln_filter_editingFinished() {
    on_bn_refresh_clicked();
}

void medorgs_wnd::on_bn_add_medpart_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_medorgs || !ui->tab_medorgs->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица медорганизаций не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите ЛПУ из списка.");
        return;
    }

    if (ui->combo_medpart_type->currentIndex()<0) {
        QMessageBox::warning(this, "Недостаточно данных", "Выберите тип подразделения.\n\nСохранение отменено.");
        return;
    }
    if (ui->ln_medpart_name->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Введите код подразделения ЛПУ.\n\nСохранение отменено.");
        return;
    }
    if (ui->ln_medpart_name->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Введите название подразделения ЛПУ.\n\nСохранение отменено.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_medorgs->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        QModelIndex index = indexes.front();

        int id_medorg = model_medorgs.data(model_medorgs.index(index.row(), 0), Qt::EditRole).toInt();
        QString code_mo = model_medorgs.data(model_medorgs.index(index.row(), 2), Qt::EditRole).toString();

        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Добавить новое подразделений ЛПУ?\n",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Yes) {
            // проверим код на уникальность
            QString sql_test = "select count(*) from spr_medsites "
                               " where code_mo='" + code_mo + "' and "
                               "       code_mp='" + ui->ln_medpart_code->text().trimmed() + "' ; ";

            QSqlQuery *query_test = new QSqlQuery(db);
            if (!mySQL.exec(this, sql_test, "Проверка кода подразделения на уникальность", *query_test, true, db, data_app) || !query_test->next()) {
                QMessageBox::warning(this, "Непредвиденная ошибка", "При попытке проверить код подразделения на уникальность произошла неожиданная ошибка.\n\nСохранение отменено.");
                delete query_test;
                return;
            }
            if (query_test->value(0).toInt()>0) {
                QMessageBox::warning(this,
                                      "Такой код подразделения МО уже есть",
                                      "В базе данных уже есть подразделение данной МО с таким же кодом.\nКод подразделения должен быть уникальным.\n\nСохранение отменено.");
                delete query_test;
                return;
            }

            db.transaction();
            // добавим МО
            QString sql_ins = "insert into spr_medparts (code_mo, code_mp, name_mp, type_mp) "
                          " values ("
                          "   '" + code_mo + "', "
                          "   '" + ui->ln_medpart_code->text().trimmed().simplified() + "', "
                          "   '" + ui->ln_medpart_name->text().trimmed().simplified() + "', "
                          "    " + QString::number(ui->combo_medpart_type->currentData().toInt()) + " ) "
                          " returning id ; ";

            QSqlQuery *query_ins = new QSqlQuery(db);
            if (!mySQL.exec(this, sql_ins, "Добавление нового подразделения", *query_ins, true, db, data_app) || !query_ins->next()) {
                db.rollback();
                QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке добавить новое подразделение ЛПУ произошла неожиданная ошибка.\n\nСохранение отменено.");
                delete query_ins;
                return;
            }
            //data_assig.code_ms = "";
            delete query_ins;
            db.commit();
        }
    }
    refresh_medparts_tab();
    refresh_medsites_tab();
    refresh_medters_tab();
}

void medorgs_wnd::on_tab_medparts_clicked(const QModelIndex &index) {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_medparts || !ui->tab_medparts->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица подразделений медорганизаций не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите подразделение ЛПУ из списка.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_medparts->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        QModelIndex index = indexes.front();

        ui->ln_medpart_code->setText(model_medparts.data(model_medparts.index(index.row(), 2), Qt::EditRole).toString());
        ui->ln_medpart_name->setText(model_medparts.data(model_medparts.index(index.row(), 3), Qt::EditRole).toString());
        ui->combo_medpart_type->setCurrentIndex(ui->combo_medpart_type->findData(model_medparts.data(model_medparts.index(index.row(), 4), Qt::EditRole).toString()));

    } else {
        ui->ln_medpart_code->setText("");
        ui->ln_medpart_name->setText("");
        //ui->combo_medpart_type->setCurrentIndex(-1);
    }
    ui->combo_medsite_mp->setCurrentIndex(ui->combo_medsite_mp->findData(model_medparts.data(model_medparts.index(index.row(), 2), Qt::EditRole).toString()));
    //ui->combo_medter_mp->setCurrentIndex(ui->combo_medsite_mp->findData(model_medparts.data(model_medparts.index(index.row(), 2), Qt::EditRole).toString()));

    refresh_medsite_ters_tab();
}

void medorgs_wnd::on_bn_edit_medpart_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_medparts || !ui->tab_medparts->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица подразделений медорганизаций не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите подразделение ЛПУ из списка.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_medparts->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Изменить данные выбранного подразделения ЛПУ?",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        QModelIndex index = indexes.front();
        QString ID = QString::number(model_medparts.data(model_medparts.index(index.row(), 0), Qt::EditRole).toInt());


        // проверка
        if (ui->combo_medpart_type->currentIndex()<0) {
            QMessageBox::warning(this, "Недостаточно данных", "Выберите тип нового подразделения.\n\nСохранение отменено.");
            return;
        }
        if (ui->ln_medpart_code->text().trimmed().simplified().isEmpty()) {
            QMessageBox::warning(this, "Недостаточно данных", "Введите код подразделения.\n\nСохранение отменено.");
            return;
        }
        if (ui->ln_medpart_name->text().trimmed().simplified().isEmpty()) {
            QMessageBox::warning(this, "Недостаточно данных", "Введите название подразделения.\n\nСохранение отменено.");
            return;
        }

        // проверим код на уникальность
        QString sql = "select count(*) from spr_medparts "
                      " where code_mo='" + model_medparts.data(model_medparts.index(index.row(), 2), Qt::EditRole).toString() + "' and "
                      "       code_mp='" + ui->ln_medpart_code->text().trimmed() + "' and "
                      "       id<>" + ID + " ; ";

        QSqlQuery *query = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Проверка кода подразделения на уникальность", *query, true, db, data_app) || !query->next()) {
            QMessageBox::warning(this, "Непредвиденная ошибка", "При попытке проверить код подразделения на уникальность произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query;
            return;
        }
        if (query->value(0).toInt()>0) {
            QMessageBox::warning(this,
                                  "Такой код подразделения МО уже есть",
                                  "В базе данных уже есть подразделение данной МО с таким же реестровым номером.\nКод подразделения должен быть уникальным.\n\nСохранение отменено.");
            delete query;
            return;
        }


        QString code_mo = model_medparts.data(model_medparts.index(index.row(), 1), Qt::EditRole).toString();

        db.transaction();
        // поравим
        QString sql_upd = "update spr_medparts "
                      " set "
                      "   code_mo='" + code_mo + "', "
                      "   code_mp='" + ui->ln_medpart_code->text().trimmed().simplified() + "', "
                      "   name_mp='" + ui->ln_medpart_name->text().trimmed().simplified() + "', "
                      "   type_mp=" + QString::number(ui->combo_medpart_type->currentData().toInt()) + " "
                      " where id=" + ID + " ; ";

        QSqlQuery *query_upd = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_upd, "Обновление подразделения", *query_upd, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке изменить данные подразделения произошла неожиданная ошибка.\n\nИзменение отменено.");
            delete query_upd;
            return;
        }
        //data_assig.code_ms = ui->ln_medpart_code->text();
        /*data_medpart.name_mo = ui->ln_name_mo->text();*/
        delete query;
        db.commit();

        refresh_medparts_tab();
        refresh_medsites_tab();
        refresh_medters_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void medorgs_wnd::on_bn_refresh_medparts_clicked() {
    refresh_medparts_tab();
}

void medorgs_wnd::on_bn_delete_medpart_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_medparts || !ui->tab_medparts->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Поиск] и выберите подразделение из списка.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_medparts->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Удалить выбранный участок из базы?\n",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }
        db.transaction();

        QModelIndex index = indexes.front();
        QString ID = QString::number(model_medparts.data(model_medparts.index(index.row(), 0), Qt::EditRole).toInt());


        QString sql = "delete from spr_medparts "
                      " where id=" + ID + " ; ";
        QSqlQuery *query2 = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Удаление участка", *query2, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке удалить участок произошла неожиданная ошибка.\n\nУдаление отменено.");
            delete query2;
            return;
        }
        delete query2;

        db.commit();

        refresh_medparts_tab();
        refresh_medsites_tab();
        refresh_medters_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}


void medorgs_wnd::refresh_medsite_mo() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка медорганизаций
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code_mo, '(' || code_mo || ') ' || name_mo "
                  " from public.spr_medorgs s "
                  " where is_head=1 "
                  " order by code_mo ; ";
    mySQL.exec(this, sql, QString("Справочник медорганизаций для таблицы мед.участков"), *query, true, db, data_app);
    ui->combo_medsite_mo->clear();
    while (query->next()) {
        ui->combo_medsite_mo->addItem(query->value(1).toString(), query->value(0).toString());
    }
    //ui->combo_medsite_mo->setCurrentIndex(-1);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void medorgs_wnd::refresh_medsite_mp() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка подразделений
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code_mp, '(' || code_mp || ') ' || name_mp "
                  " from public.spr_medparts u "
                  " where u.code_mo='" + ui->combo_medsite_mo->currentData().toString() + "' "
                  " order by code_mp ; ";
    mySQL.exec(this, sql, QString("Справочник подразделений для таблицы мед.участков"), *query, true, db, data_app);
    ui->combo_medsite_mp->clear();
    while (query->next()) {
        ui->combo_medsite_mp->addItem(query->value(1).toString(), query->value(0).toString());
    }
    //ui->combo_medsite_mp->setCurrentIndex(-1);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void medorgs_wnd::on_ch_medsites_mo_clicked(bool checked) {
    ui->combo_medsite_mo->setEnabled(!checked);
    //ui->combo_medsite_mp->setEnabled(!checked);
}

void medorgs_wnd::on_ch_medters_mo_clicked(bool checked) {
    ui->combo_medter_mo->setEnabled(!checked);
    //if (!checked) {
    //    ui->ch_medters_ms->setChecked(false);
    //    ui->combo_medter_mp->setEnabled(!checked);
    //    ui->combo_medter_ms->setEnabled(!checked);
    //}
    refresh_medters_tab();
}

void medorgs_wnd::on_combo_medsite_mo_activated(const QString &arg1) {
    refresh_medsite_mp();
}

void medorgs_wnd::on_combo_medter_mo_activated(const QString &arg1) {
    //refresh_medter_mp();
}






void medorgs_wnd::on_bn_refresh_medsite_ters_clicked() {
    refresh_medsite_ters_tab();
}
void medorgs_wnd::on_ch_medsite_ters_now_clicked() {
    refresh_medsite_ters_tab();
}

void medorgs_wnd::refresh_medsite_ters_tab() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    this->setCursor(Qt::WaitCursor);

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_medsites->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        QModelIndex index = indexes.front();
        QString code_mo = model_medsites.data(model_medsites.index(index.row(), 1), Qt::EditRole).toString();
        QString code_mp = model_medsites.data(model_medsites.index(index.row(), 2), Qt::EditRole).toString();
        QString code_ms = model_medsites.data(model_medsites.index(index.row(), 3), Qt::EditRole).toString();

        QString medsite_ters_sql;
        medsite_ters_sql  = "select r.id_ms, r.id_mt, ms.code_mo, mo.name_mo, ms.code_mp, mp.name_mp, ms.code_ms, ms.name_ms, mt.fio_mt, mt.snils_mt, "
                            "       case r.prof_mt when 0 then '???' when 1 then 'врач' when 2 then 'медс(-б)' when 3 then 'фелд/ак.' end, "
                            "       r.date_beg, r.date_end ";
        medsite_ters_sql += " from spr_ms_r_mt r "
                            "      join spr_medsites ms on(ms.id=r.id_ms) "
                            "      join spr_medters  mt on(mt.id=r.id_mt) "
                            "      join spr_medparts mp on(mp.code_mo=ms.code_mo and mp.code_mp=ms.code_mp) "
                            "      join spr_medorgs  mo on(mo.code_mo=ms.code_mo and mo.is_head=1) "
                            " where ms.code_mo='" + code_mo + "' "
                            "   and ms.code_mp='" + code_mp + "' "
                            "   and ms.code_ms='" + code_ms + "' ";
        if (ui->ch_medsite_ters_now->isChecked()) {
            medsite_ters_sql += " and (r.date_beg is not NULL and r.date_beg<='" + QDate::currentDate().toString("yyyy-MM-dd") + "') "
                                " and (r.date_end is NULL or r.date_end>'" + QDate::currentDate().toString("yyyy-MM-dd") + "') ";
        }
        medsite_ters_sql += " order by r.prof_mt, mt.fio_mt ; ";

        model_medsite_ters.setQuery(medsite_ters_sql,db);
        QString err2 = model_medsite_ters.lastError().driverText();

        // подключаем модель из БД
        ui->tab_medsite_ters->setModel(&model_medsite_ters);

        // обновляем таблицу
        ui->tab_medsite_ters->reset();

        // задаём ширину колонок
        ui->tab_medsite_ters->setColumnWidth( 0,  1);   // r.id_ms,
        ui->tab_medsite_ters->setColumnWidth( 1,  1);   // r.id_mt,
        ui->tab_medsite_ters->setColumnWidth( 2,  1);   // ms.code_mo,
        ui->tab_medsite_ters->setColumnWidth( 3,  1);   // mo.name_mo,
        ui->tab_medsite_ters->setColumnWidth( 4,  1);   // ms.code_mp,
        ui->tab_medsite_ters->setColumnWidth( 5,  1);   // mp.name_mp,
        ui->tab_medsite_ters->setColumnWidth( 6,  1);   // ms.code_ms,
        ui->tab_medsite_ters->setColumnWidth( 7,  1);   // ms.name_ms,
        ui->tab_medsite_ters->setColumnWidth( 8,150);   // mt.fio_mt,
        ui->tab_medsite_ters->setColumnWidth( 9,  1);   // mt.snils_mt,
        ui->tab_medsite_ters->setColumnWidth(10, 60);   // r.prof_mt,
        ui->tab_medsite_ters->setColumnWidth(11,  2);   // r.date_beg,
        ui->tab_medsite_ters->setColumnWidth(12,  2);   // r.date_end
        // правим заголовки
        model_medsite_ters.setHeaderData( 8, Qt::Horizontal, ("врач"));
        model_medsite_ters.setHeaderData(10, Qt::Horizontal, ("спец."));
        ui->tab_medsite_ters->repaint();
    }

    this->setCursor(Qt::ArrowCursor);
}

void medorgs_wnd::on_bn_add_medsite_ter_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"),
                             ("Нет открытого соединения к базе данных?\n "
                              "Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n "
                              "Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    this->setCursor(Qt::WaitCursor);

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_medsites->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        QModelIndex index = indexes.front();

        s_data_medsite_ter data_medsite_ter;
        data_medsite_ter.id_ms = model_medsites.data(model_medsites.index(index.row(), 0), Qt::EditRole).toInt();
        data_medsite_ter.id_mt = -1;
        data_medsite_ter.code_mo = model_medsites.data(model_medsites.index(index.row(), 1), Qt::EditRole).toString();
        data_medsite_ter.name_mo = model_medsites.data(model_medsites.index(index.row(), 7), Qt::EditRole).toString();
        data_medsite_ter.code_mp = model_medsites.data(model_medsites.index(index.row(), 2), Qt::EditRole).toString();
        data_medsite_ter.name_mp = model_medsites.data(model_medsites.index(index.row(), 8), Qt::EditRole).toString();
        data_medsite_ter.code_ms = model_medsites.data(model_medsites.index(index.row(), 3), Qt::EditRole).toString();
        data_medsite_ter.name_ms = model_medsites.data(model_medsites.index(index.row(), 4), Qt::EditRole).toString();


        // проверим выделенную строку
        QModelIndexList indexes_mt = ui->tab_medters->selectionModel()->selection().indexes();

        if (indexes_mt.size()>0) {
            QModelIndex index_mt = indexes_mt.front();
            data_medsite_ter.id_mt = model_medters.data(model_medters.index(index_mt.row(), 0), Qt::EditRole).toInt();
        }

        delete add_medsite_ter_w;

        add_medsite_ter_w = new add_medsite_ter_wnd(db, data_app, data_medsite_ter, settings, this);
        add_medsite_ter_w->exec();
        refresh_medsite_ters_tab();

    } else {
        QMessageBox::warning(this, "Не выбран участок", "Выберите участок, в который надо добавить участкового врача.");
    }
    this->setCursor(Qt::ArrowCursor);
}

void medorgs_wnd::on_bn_delete_medsite_ter_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"),
                             ("Нет открытого соединения к базе данных?\n "
                              "Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n "
                              "Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_medsite_ters || !ui->tab_medsite_ters->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Выберите ЛПУ, подразделение, участок и участкового врача.");
        return;
    }
    this->setCursor(Qt::WaitCursor);

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_medsite_ters->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Убрать выбранного участкового врача?\n",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }
        db.transaction();

        QModelIndex index = indexes.front();
        QString id_ms = QString::number(model_medsite_ters.data(model_medsite_ters.index(index.row(), 0), Qt::EditRole).toInt());
        QString id_mt = QString::number(model_medsite_ters.data(model_medsite_ters.index(index.row(), 1), Qt::EditRole).toInt());

        QString sql = "update spr_ms_r_mt "
                      " set date_end='" + QDate::currentDate().toString("yyyy-MM-dd") + "' "
                      " where id_ms=" + id_ms + " "
                      "   and id_mt=" + id_mt + "; ";
        QSqlQuery *query2 = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Удаление участкового врача", *query2, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке убрать из участка участкового врача произошла неожиданная ошибка.\n\nУдаление отменено.");
            delete query2;
            return;
        }
        delete query2;

        db.commit();
        refresh_medsite_ters_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
    this->setCursor(Qt::ArrowCursor);
}



void medorgs_wnd::on_bn_ok_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_medorgs || !ui->tab_medorgs->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Поиск] и выберите человека из списка.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes_mo = ui->tab_medorgs->selectionModel()->selection().indexes();

    if (indexes_mo.size()>0) {
        QModelIndex index_mo = indexes_mo.front();
        QString s = model_medorgs.data(model_medorgs.index(index_mo.row(), 2), Qt::EditRole).toString();
        data_assig.code_mo = s;
    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Медицинская организация не выбрана.\n\nВыбор прикрепления отменён.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes_mp = ui->tab_medparts->selectionModel()->selection().indexes();

    if (indexes_mp.size()>0) {
        QModelIndex index_mp = indexes_mp.front();
        QString s = model_medparts.data(model_medparts.index(index_mp.row(), 2), Qt::EditRole).toString();
        data_assig.code_mp = s;
    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Не выбрано подразделение.\n\nВыбор прикрепления отменён.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes_ms = ui->tab_medsites->selectionModel()->selection().indexes();

    if (indexes_ms.size()>0) {
        QModelIndex index_ms = indexes_ms.front();
        QString s = model_medsites.data(model_medsites.index(index_ms.row(), 3), Qt::EditRole).toString();
        data_assig.code_ms = s;
    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Не выбран участок.\n\nВыбор прикрепления отменён.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes_mt = ui->tab_medsite_ters->selectionModel()->selection().indexes();

    if (indexes_mt.size()>0) {
        QModelIndex index_mt = indexes_mt.front();
        QString s = model_medsite_ters.data(model_medsite_ters.index(index_mt.row(), 9), Qt::EditRole).toString();
        data_assig.code_mt = s;
    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Не выбран врач.\n\nВыбор прикрепления отменён.");
        return;
    }

    accept();
}

void medorgs_wnd::on_bn_close_clicked() {
    reject();
}

void medorgs_wnd::on_bn_toCSV_clicked() {
    QString fname_s = QFileDialog::getSaveFileName(this,
                                                   "Куда сохранить файл?",
                                                   QString(),
                                                   "файлы данных (*.csv);;простой текст (*.txt)");
    if (fname_s.isEmpty()) {
        QMessageBox::warning(this,
                             "Не выбрано имя файла",
                             "Имя цедевого файла не задано.\n\n"
                             "Операция отменена.");
        return;
    }
    if (QFile::exists(fname_s)) {
        if (QMessageBox::question(this,
                                  "Удалить старый файл?",
                                  "Файл уже существует.\n"
                                  "Удалить старый файл для того чтобы сохранить новый?",
                                  QMessageBox::Yes|QMessageBox::Cancel)==QMessageBox::Yes) {
            if (!QFile::remove(fname_s)) {
                QMessageBox::warning(this,
                                     "Ошибка при удалении старого файла",
                                     "При удалении старого файла произошла непредвиденная ошибка.\n\n"
                                     "Операция отменена.");
                return;
            }
        }
    }


    // сохраним данные в файл
    QFile file;
    file.setFileName(fname_s);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this,
                             "Ошибка создания файла",
                             "При создании целевого файла произошла ошибка.\n\n"
                             "Операция отменена.");
        return;
    }

    // подготовим поток для сохранения файла
    QTextStream stream(&file);
    QString codec_name = "Windows-1251";
    QTextCodec *codec = QTextCodec::codecForName(codec_name.toLocal8Bit());
    stream.setCodec(codec);


    QString head;
    int cnt_head = model_medorgs.columnCount();
    for (int i=0; i<cnt_head; i++) {
        if (ui->tab_medorgs->columnWidth(i)>1) {
            head += model_medorgs.headerData(i,Qt::Horizontal).toString().replace("\n"," ");
            if (i<cnt_head-1)  head += ";";
        }
    }
    stream << head << "\n";

    // переберём полученные данные и сохраним в файл
    QSqlQuery *query_export = new QSqlQuery(db);
    query_export->exec(medorgs_sql);
    while (query_export->next()) {
        for (int j=0; j<cnt_head; j++) {
            if (ui->tab_medorgs->columnWidth(j)>1) {
                QString value = query_export->value(j).toString().trimmed().replace(";",",");
                if (!value.isEmpty()) {
                    if (j==51)  stream << "'";
                    stream << value;
                    if (j==51)  stream << "'";
                }
                if (j<cnt_head-1)  stream << ";";
            }
        }
        stream << "\n";
    }
    /*int cnt_rows = model_medorgs.rowCount();
    for (int i=0; i<cnt_rows; i++) {
        for (int j=0; j<cnt_head; j++) {
            if (ui->tab_medorgs->columnWidth(j)>1) {
                // данные застрахованного
                QString s = model_medorgs.data(model_medorgs.index(i, j), Qt::EditRole).toString().replace("\n"," ");
                stream << s;
                if (j<cnt_head-1)  stream << ";";
            }
        }
        stream << "\n";
    }*/
    file.close();

    // ===========================
    // собственно открытие шаблона
    // ===========================
    // открытие полученного ODT
    /*long result = (long)*/ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_s.utf16()), 0, 0, SW_NORMAL);
}

void medorgs_wnd::on_ch_mo_assig_data_clicked(bool checked) {
    ui->pan_mo_assig_data->setVisible(checked);
}
void medorgs_wnd::on_ch_assig_settings_clicked(bool checked) {
    ui->pan_assig_settings->setVisible(checked);
}
void medorgs_wnd::on_ch_show_mo_clicked(bool checked) {
    refresh_medorgs_tab();
}
void medorgs_wnd::on_ch_show_letters_clicked(bool checked) {
    refresh_medorgs_tab();
}
