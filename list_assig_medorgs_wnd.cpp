#include "list_assig_medorgs_wnd.h"
#include "ui_list_assig_medorgs_wnd.h"

list_assig_medorgs_wnd::list_assig_medorgs_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent)
   : db(db), data_app(data_app), settings(settings), QDialog(parent), ui(new Ui::list_assig_medorgs_wnd)
{
    ui->setupUi(this);

    refresh_medorgs_tab();
}

list_assig_medorgs_wnd::~list_assig_medorgs_wnd() {
    delete ui;
}

void list_assig_medorgs_wnd::on_bn_close_clicked() {
    close();
}

void list_assig_medorgs_wnd::refresh_medorgs_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    QString medorgs_sql;
    medorgs_sql += "select id, text, code_mo, mo_name, mo_name_full, mo_chif_state, mo_chif_famio, mo_chif_state_dp, mo_chif_famio_dp, mo_chif_greetings, mo_contract_num, mo_contract_date "
                   " from assig_medorgs "
                   " order by id ; ";

    model_medorgs.setQuery(medorgs_sql,db);
    QString err2 = model_medorgs.lastError().driverText();

    // подключаем модель из БД
    ui->tab_medorgs->setModel(&model_medorgs);

    // обновляем таблицу
    ui->tab_medorgs->reset();

    // задаём ширину колонок
    ui->tab_medorgs->setColumnWidth( 0, 50);     // id,
    ui->tab_medorgs->setColumnWidth( 1,150);     // text,
    ui->tab_medorgs->setColumnWidth( 2, 80);     // code_mo,
    ui->tab_medorgs->setColumnWidth( 3,100);     // mo_name,
    ui->tab_medorgs->setColumnWidth( 4,  1);     // mo_name_full,
    ui->tab_medorgs->setColumnWidth( 5,100);     // mo_chif_state,
    ui->tab_medorgs->setColumnWidth( 6,100);     // mo_chif_famio,
    ui->tab_medorgs->setColumnWidth( 7,120);     // mo_chif_state_dp,
    ui->tab_medorgs->setColumnWidth( 8,120);     // mo_chif_famio_dp,
    ui->tab_medorgs->setColumnWidth( 9,120);     // mo_chif_greetings,
    ui->tab_medorgs->setColumnWidth( 10,80);     // mo_contract_num,
    ui->tab_medorgs->setColumnWidth( 11,80);     // mo_contract_date,

    // правим заголовки
    model_medorgs.setHeaderData( 0, Qt::Horizontal, ("ID"));
    model_medorgs.setHeaderData( 1, Qt::Horizontal, ("текст"));
    model_medorgs.setHeaderData( 2, Qt::Horizontal, ("код ЛПУ"));
    model_medorgs.setHeaderData( 3, Qt::Horizontal, ("название ЛПУ"));
    model_medorgs.setHeaderData( 4, Qt::Horizontal, ("название ЛПУ, полное"));
    model_medorgs.setHeaderData( 5, Qt::Horizontal, ("должность \nруководителя"));
    model_medorgs.setHeaderData( 6, Qt::Horizontal, ("ФИО \nруководителя"));
    model_medorgs.setHeaderData( 7, Qt::Horizontal, ("должность \nруководителя, дп"));
    model_medorgs.setHeaderData( 8, Qt::Horizontal, ("ФИО \nруководителя, дп"));
    model_medorgs.setHeaderData( 9, Qt::Horizontal, ("приветственная \nформа"));
    model_medorgs.setHeaderData( 10, Qt::Horizontal,("номер \nдоговора"));
    model_medorgs.setHeaderData( 11, Qt::Horizontal,("дата \nдоговора"));
    ui->tab_medorgs->repaint();

    ui->ln_text->setText("");
    ui->ln_code_mo->setText("");
    ui->ln_mo_name->setText("");
    ui->ln_mo_name_full->setText("");
    ui->ln_chif_state->setText("");
    ui->ln_chif_fio->setText("");
    ui->ln_chif_state_dp->setText("");
    ui->ln_chif_fio_dp->setText("");
    ui->ln_chif_greetings->setText("");
    ui->ln_contract_num->setText("");
    ui->dat_contract_date->setDate(QDate(1900,1,1));

    this->setCursor(Qt::ArrowCursor);
}

void list_assig_medorgs_wnd::on_bn_refresh_clicked() {
    refresh_medorgs_tab();
}

void list_assig_medorgs_wnd::on_bn_add_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_medorgs || !ui->tab_medorgs->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить].");
        return;
    }

    // проверка, введено ли название
    if (ui->ln_code_mo->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Реестровый номер медорганизации не может быть пустым.\n\nСохранение отменено.");
        return;
    }
    if (ui->ln_mo_name->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Название медорганизации не может быть пустым.\n\nСохранение отменено.");
        return;
    }
    if (ui->ln_mo_name_full->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Полное название медорганизации не может быть пустым.\n\nСохранение отменено.");
        return;
    }
    if (ui->ln_chif_state->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Название должности руководителя медорганизации не может быть пустым.\n\nСохранение отменено.");
        return;
    }
    if (ui->ln_chif_fio->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Фамилия И.О. руководителя медорганизации не может быть пустым.\n\nСохранение отменено.");
        return;
    }
    if (ui->ln_chif_state_dp->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Название должности руководителя (дательный падеж) медорганизации не может быть пустым.\n\nСохранение отменено.");
        return;
    }
    if (ui->ln_chif_fio_dp->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Фамилия И.О. руководителя медорганизации (дательный падеж) не может быть пустым.\n\nСохранение отменено.");
        return;
    }
    if (ui->ln_chif_greetings->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Форма приветствия руководителя медорганизации не может быть пустой.\n\nСохранение отменено.");
        return;
    }
    if (ui->ln_contract_num->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Номер договора с медорганизацией не может быть пустой.\n\nСохранение отменено.");
        return;
    }
    if (ui->dat_contract_date->date()<QDate(2010,1,1) || ui->dat_contract_date->date()>QDate::currentDate()) {
        QMessageBox::warning(this, "Ошибка в данных", "Дата заключения договора с медорганизацией не может быть раньше 01.01.2010 или позже текущей даты.\n\nСохранение отменено.");
        return;
    }

    // проверим код ЛПУ на уникальность
    QString sql = "select count(*) from assig_medorgs "
                  " where code_mo='" + ui->ln_code_mo->text().trimmed().simplified().toUpper() + "' ; ";

    QSqlQuery *query = new QSqlQuery(db);
    if (!mySQL.exec(this, sql, "Проверка реестрового номера ЛПУ на уникальность", *query, true, db, data_app) || !query->next()) {
        QMessageBox::warning(this, "Непредвиденная ошибка", "При попытке проверить реестровый номер ЛПУ на уникальность произошла неожиданная ошибка.\n\nСохранение отменено.");
        delete query;
        return;
    }
    if (query->value(0).toInt()>0) {
        QMessageBox::warning(this,
                              "Такой реестровый номер уже есть",
                              "В базе данных уже есть ЛПУ с таким реестровым номером.\n\nСохранение отменено.");
        delete query;
        return;
    }


    if (QMessageBox::question(this,
                              "Нужно подтверждение",
                              "Добавить новую ЛПУ?\n",
                              QMessageBox::Yes|QMessageBox::Cancel,
                              QMessageBox::Yes)==QMessageBox::Yes) {
        db.transaction();
        // добавим фирму
        QString sql = "insert into assig_medorgs (text, code_mo, mo_name, mo_name_full, mo_chif_state, mo_chif_famio, mo_chif_state_dp, mo_chif_famio_dp, mo_chif_greetings, mo_contract_num, mo_contract_date) "
                      " values ("
                      "   '" + ui->ln_text->text().trimmed().simplified() + "', "
                      "   '" + ui->ln_code_mo->text().trimmed().simplified() + "', "
                      "   '" + ui->ln_mo_name->text().trimmed().simplified() + "', "
                      "   '" + ui->ln_mo_name_full->text().trimmed().simplified() + "', "
                      "   '" + ui->ln_chif_state->text().trimmed().simplified() + "', "
                      "   '" + ui->ln_chif_fio->text().trimmed().simplified() + "', "
                      "   '" + ui->ln_chif_state_dp->text().trimmed().simplified() + "', "
                      "   '" + ui->ln_chif_fio_dp->text().trimmed().simplified() + "', "
                      "   '" + ui->ln_chif_greetings->text().trimmed().simplified() + "', "
                      "   '" + ui->ln_contract_num->text().trimmed().simplified() + "', "
                      "   '" + ui->dat_contract_date->date().toString("yyyy-MM-dd") + "') "
                      " returning id ; ";

        QSqlQuery *query = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Добавление новой фирмы", *query, true, db, data_app) || !query->next()) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке добавить новую медорганизацию произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query;
            return;
        }
        delete query;
        db.commit();

    }
    refresh_medorgs_tab();
}

void list_assig_medorgs_wnd::on_tab_medorgs_clicked(const QModelIndex &index) {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_medorgs || !ui->tab_medorgs->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить].");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_medorgs->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        QModelIndex index = indexes.front();

        ui->ln_text->setText(model_medorgs.data(model_medorgs.index(index.row(), 1), Qt::EditRole).toString());
        ui->ln_code_mo->setText(model_medorgs.data(model_medorgs.index(index.row(), 2), Qt::EditRole).toString());
        ui->ln_mo_name->setText(model_medorgs.data(model_medorgs.index(index.row(), 3), Qt::EditRole).toString());
        ui->ln_mo_name_full->setText(model_medorgs.data(model_medorgs.index(index.row(), 4), Qt::EditRole).toString());
        ui->ln_chif_state->setText(model_medorgs.data(model_medorgs.index(index.row(), 5), Qt::EditRole).toString());
        ui->ln_chif_fio->setText(model_medorgs.data(model_medorgs.index(index.row(), 6), Qt::EditRole).toString());
        ui->ln_chif_state_dp->setText(model_medorgs.data(model_medorgs.index(index.row(), 7), Qt::EditRole).toString());
        ui->ln_chif_fio_dp->setText(model_medorgs.data(model_medorgs.index(index.row(), 8), Qt::EditRole).toString());
        ui->ln_chif_greetings->setText(model_medorgs.data(model_medorgs.index(index.row(), 9), Qt::EditRole).toString());
        ui->ln_contract_num->setText(model_medorgs.data(model_medorgs.index(index.row(), 10), Qt::EditRole).toString());
        ui->dat_contract_date->setDate(model_medorgs.data(model_medorgs.index(index.row(), 11), Qt::EditRole).toDate());

    } else {

        ui->ln_text->setText("");
        ui->ln_code_mo->setText("");
        ui->ln_mo_name->setText("");
        ui->ln_mo_name_full->setText("");
        ui->ln_chif_state->setText("");
        ui->ln_chif_fio->setText("");
        ui->ln_chif_state_dp->setText("");
        ui->ln_chif_fio_dp->setText("");
        ui->ln_chif_greetings->setText("");
        ui->ln_contract_num->setText("");
        ui->dat_contract_date->setDate(QDate(1900,1,1));
    }
}



void list_assig_medorgs_wnd::on_bn_edit_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_medorgs || !ui->tab_medorgs->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить].");
        return;
    }

    // проверка, введено ли название
    if (ui->ln_code_mo->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Реестровый номер медорганизации не может быть пустым.\n\nСохранение отменено.");
        return;
    }
    if (ui->ln_mo_name->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Название медорганизации не может быть пустым.\n\nСохранение отменено.");
        return;
    }
    if (ui->ln_mo_name_full->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Полное название медорганизации не может быть пустым.\n\nСохранение отменено.");
        return;
    }
    if (ui->ln_chif_state->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Название должности руководителя медорганизации не может быть пустым.\n\nСохранение отменено.");
        return;
    }
    if (ui->ln_chif_fio->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Фамилия И.О. руководителя медорганизации не может быть пустым.\n\nСохранение отменено.");
        return;
    }
    if (ui->ln_chif_state_dp->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Название должности руководителя медорганизации (дательный падеж) не может быть пустым.\n\nСохранение отменено.");
        return;
    }
    if (ui->ln_chif_fio_dp->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Фамилия И.О. руководителя медорганизации (дательный падеж) не может быть пустым.\n\nСохранение отменено.");
        return;
    }
    if (ui->ln_chif_greetings->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Форма приветствия руководителя медорганизации не может быть пустой.\n\nСохранение отменено.");
        return;
    }
    if (ui->ln_contract_num->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Номер договора с медорганизацией не может быть пустой.\n\nСохранение отменено.");
        return;
    }
    if (ui->dat_contract_date->date()<QDate(2010,1,1) || ui->dat_contract_date->date()>QDate::currentDate()) {
        QMessageBox::warning(this, "Ошибка в данных", "Дата заключения договора с медорганизацией не может быть раньше 01.01.2010 или позже текущей даты.\n\nСохранение отменено.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_medorgs->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Изменить данные выбранной территории?",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        QModelIndex index = indexes.front();

        int id = model_medorgs.data(model_medorgs.index(index.row(), 0), Qt::EditRole).toInt();

        // проверим код ЛПУ на уникальность
        QString sql_tst = "select count(*) from assig_medorgs "
                          " where id<>" + QString::number(id) +
                          "       and code_mo='" + ui->ln_code_mo->text().trimmed().simplified().toUpper() + "' ; ";

        QSqlQuery *query_tst = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_tst, "Проверка реестрового номера ЛПУ на уникальность", *query_tst, true, db, data_app) || !query_tst->next()) {
            QMessageBox::warning(this, "Непредвиденная ошибка", "При попытке проверить реестровый номер ЛПУ на уникальность произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query_tst;
            return;
        }
        if (query_tst->value(0).toInt()>0) {
            QMessageBox::warning(this,
                                  "Такой реестровый номер уже есть",
                                  "В базе данных уже есть ЛПУ с таким реестровым номером.\n\nСохранение отменено.");
            delete query_tst;
            return;
        }
        delete query_tst;

        db.transaction();
        // добавим территорию
        QString sql_upd = "update assig_medorgs "
                          " set "
                          "   text='" + ui->ln_text->text().trimmed().simplified() + "', "
                          "   code_mo='" + ui->ln_code_mo->text().trimmed().simplified() + "', "
                          "   mo_name='" + ui->ln_mo_name->text().trimmed().simplified() + "', "
                          "   mo_name_full='" + ui->ln_mo_name_full->text().trimmed().simplified() + "', "
                          "   mo_chif_state='" + ui->ln_chif_state->text().trimmed().simplified() + "', "
                          "   mo_chif_famio='" + ui->ln_chif_fio->text().trimmed().simplified() + "', "
                          "   mo_chif_state_dp='" + ui->ln_chif_state_dp->text().trimmed().simplified() + "', "
                          "   mo_chif_famio_dp='" + ui->ln_chif_fio_dp->text().trimmed().simplified() + "', "
                          "   mo_chif_greetings='" + ui->ln_chif_greetings->text().trimmed().simplified() + "', "
                          "   mo_contract_num='" + ui->ln_contract_num->text().trimmed().simplified() + "', "
                          "   mo_contract_date='" + ui->dat_contract_date->date().toString("yyyy-MM-dd") + "' "
                          " where id=" + QString::number(id) + " ; ";

        QSqlQuery *query_upd = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_upd, "Обновление фирмы", *query_upd, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке изменить данные медорганизации произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query_upd;
            return;
        }
        delete query_upd;
        db.commit();

        refresh_medorgs_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void list_assig_medorgs_wnd::on_bn_delete_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_medorgs || !ui->tab_medorgs->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить].");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_medorgs->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Удалить выбранную запись из базы?\n",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        QModelIndex index = indexes.front();

        int id = model_medorgs.data(model_medorgs.index(index.row(), 0), Qt::EditRole).toInt();

        db.transaction();

        // удалим
        QString sql;
        sql = "delete from assig_medorgs "
              " where id=" + QString::number(id) + " ; ";
        QSqlQuery *query2 = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Удаление медорганизации", *query2, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке удалить медорганизацию произошла неожиданная ошибка.\n\nУдаление отменено.");
            delete query2;
            return;
        }
        delete query2;

        db.commit();

        refresh_medorgs_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}
