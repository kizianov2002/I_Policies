#include "namepat_to_sex_wnd.h"
#include "ui_namepat_to_sex_wnd.h"

namepat_to_sex_wnd::namepat_to_sex_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent)
    : db(db), data_app(data_app), settings(settings), QDialog(parent), ui(new Ui::namepat_to_sex_wnd)
{
    ui->setupUi(this);
    refresh_tab();
}

namepat_to_sex_wnd::~namepat_to_sex_wnd() {
    delete ui;
}

void namepat_to_sex_wnd::on_bn_close_clicked() {
    close();
}

void namepat_to_sex_wnd::on_bn_refresh_clicked() {
    refresh_tab();
}

void namepat_to_sex_wnd::refresh_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    // данные персоны
    ui->ln_namepat->clear();
    ui->combo_sex->setCurrentIndex(0);

    QString sql =
            "select s.id, s.name, s.sex, s.date_add, s.status, o.oper_fio "
            " from spr_namepat_to_sex s "
            "      left join operators o on(o.id=s.id_operator) "
            " where s.name like('" + ui->ln_namepat_filter->text().trimmed().toUpper() + "%') ";
    if (ui->combo_sex_filter->currentIndex()>0) {
        sql += "and s.sex=" + QString::number(ui->combo_sex_filter->currentIndex()) + " ";
    }

    // сортировка
    sql += " order by s.sex, s.name ; ";

    model_namepat_to_sex.setQuery(sql,db);
    QString err = model_namepat_to_sex.lastError().driverText();

    // подключаем модель из БД
    ui->tableView->setModel(&model_namepat_to_sex);

    // обновляем таблицу
    ui->tableView->reset();

    // задаём ширину колонок
    ui->tableView->setColumnWidth( 0,  1);    // id
    ui->tableView->setColumnWidth( 1,120);    // name
    ui->tableView->setColumnWidth( 2, 40);    // sex
    ui->tableView->setColumnWidth( 3, 95);    // date_add
    ui->tableView->setColumnWidth( 4, 30);    // oper_fio
    ui->tableView->setColumnWidth( 5,110);    // oper_fio

    // правим заголовки
    model_namepat_to_sex.setHeaderData( 0, Qt::Horizontal, ("ID"));
    model_namepat_to_sex.setHeaderData( 1, Qt::Horizontal, ("отчество"));
    model_namepat_to_sex.setHeaderData( 2, Qt::Horizontal, ("пол"));
    model_namepat_to_sex.setHeaderData( 3, Qt::Horizontal, ("дата вставки"));
    model_namepat_to_sex.setHeaderData( 4, Qt::Horizontal, ("ст."));
    model_namepat_to_sex.setHeaderData( 5, Qt::Horizontal, ("оператор"));

    this->setCursor(Qt::ArrowCursor);
}

void namepat_to_sex_wnd::on_tableView_clicked(const QModelIndex &index) {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tableView || !ui->tableView->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить].");
        return;
    }
    this->setCursor(Qt::WaitCursor);

    data_namepat.id = model_namepat_to_sex.data(model_namepat_to_sex.index(index.row(), 0), Qt::EditRole).toInt();
    data_namepat.namepat = model_namepat_to_sex.data(model_namepat_to_sex.index(index.row(), 1), Qt::EditRole).toString();
    data_namepat.sex = model_namepat_to_sex.data(model_namepat_to_sex.index(index.row(), 2), Qt::EditRole).toInt();
    data_namepat.status = model_namepat_to_sex.data(model_namepat_to_sex.index(index.row(), 4), Qt::EditRole).toInt();

    ui->ln_namepat->setText(model_namepat_to_sex.data(model_namepat_to_sex.index(index.row(), 1), Qt::EditRole).toString());
    ui->combo_sex->setCurrentIndex(model_namepat_to_sex.data(model_namepat_to_sex.index(index.row(), 2), Qt::EditRole).toInt());
    ui->combo_status->setCurrentIndex(model_namepat_to_sex.data(model_namepat_to_sex.index(index.row(), 4), Qt::EditRole).toInt());

    this->setCursor(Qt::ArrowCursor);
}

void namepat_to_sex_wnd::on_bn_add_namepat_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    // проверим заполнение полей
    if (ui->combo_sex->currentIndex()==0) {
        QMessageBox::warning(this, "Укажите пол", "Пол не задан.\nОперация отменена.");
        return;
    }
    if (ui->ln_namepat->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Укажите отчество", "Отчество не задано.\nОперация отменена.");
        return;
    }

    this->setCursor(Qt::WaitCursor);
    QString sql_tst =
            "select s.name, s.sex "
            " from spr_namepat_to_sex s "
            " where name='" + ui->ln_namepat->text().trimmed().toUpper() + "' "
            "   and sex=" + QString::number(data_namepat.sex) + " ; ";

    QSqlQuery *query_tst = new QSqlQuery(db);
    bool res_tst = mySQL.exec(this, sql_tst, QString("проверим наличие отчества в базе"), *query_tst, true, db, data_app);
    if (!res_tst) {
        delete query_tst;
        QMessageBox::warning(this, "Ошибка при проверке наличие отчества в базе", "При проверке наличия отчества в базе произошла неожиданная ошибка");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    if(query_tst->next() && query_tst->size()>0) {
        QMessageBox::warning(this, "Это отчество уже есть в базе",
                             "Отчество " + query_tst->value(0).toString() + " уже есть в базе.\n"
                             "Для отчества в базе указан пол " + (query_tst->value(1).toInt()==1 ? "М" : "Ж") + ".\n\n"
                             "При необходимости исправьте отчество и повторите операцию.\n");
        delete query_tst;
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    // добавим отчество
    QString sql =
            "insert into spr_namepat_to_sex(name, sex, date_add, id_operator) "
            " values('" + ui->ln_namepat->text().trimmed().toUpper() + "', " +
                     QString::number(ui->combo_sex->currentIndex()) + ", "
                     "'" + QDate::currentDate().toString("yyyy-MM-dd") + "', " +
                     QString::number(data_app.id_operator) + ") ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString("добавим новое отчество в базу"), *query, true, db, data_app);
    if (!res) {
        delete query;
        QMessageBox::warning(this, "Ошибка при добавлении нового отчества в базу", "При добавлении нового отчества в базу произошла неожиданная ошибка");
        this->setCursor(Qt::ArrowCursor);
        return;
    } else {
        QMessageBox::information(this, "Отчество добавлено",
                                 "В справочник отчеств добавлено отчество " + ui->ln_namepat->text().trimmed().toUpper() + " \n"
                                 "с указанием пола " + (ui->combo_sex->currentIndex()==1 ? "М" : "Ж") + ".\n");
    }
    refresh_tab();
    this->setCursor(Qt::ArrowCursor);
}

void namepat_to_sex_wnd::on_bn_delete_namepat_clicked() {
    QModelIndexList indexes = ui->tableView->selectionModel()->selection().indexes();
    if (indexes.size()>0) {
        if (QMessageBox::warning(this, "Нужно подтверждение!", "Вы действительно хотите удалить эту запись?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::No) {
            return;
        }
        QModelIndex index = indexes.at(0);
        int id = model_namepat_to_sex.data(model_namepat_to_sex.index(index.row(), 0), Qt::EditRole).toInt();

        // удалим текущую запись
        QString sql =
                "delete from spr_namepat_to_sex "
                " where id=" + QString::number(id) + " ; ";

        QSqlQuery *query2 = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Удаление отчества из базы", *query2, true, db, data_app)) {
            delete query2;
            return;
        }
        delete query2;
        refresh_tab();
    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано.");
    }
}

void namepat_to_sex_wnd::on_ln_namepat_filter_editingFinished() {
    refresh_tab();
}

void namepat_to_sex_wnd::on_combo_sex_filter_activated(int index) {
    refresh_tab();
}

void namepat_to_sex_wnd::on_bn_edit_namepat_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    // проверим заполнение полей
    if (ui->combo_sex->currentIndex()==0) {
        QMessageBox::warning(this, "Укажите пол", "Пол не задан.\nОперация отменена.");
        return;
    }
    if (ui->ln_namepat->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Укажите отчество", "Отчество не задано.\nОперация отменена.");
        return;
    }

    this->setCursor(Qt::WaitCursor);
    /*// проверим наличие отчества в базе
    QString sql_tst =
            "select s.name, s.sex "
            " from spr_namepat_to_sex s "
            " where name='" + ui->ln_namepat->text().trimmed().toUpper() + "' "
            "   and sex=" + QString::number(data_namepat.sex) + " "
            "   and id<>" + QString::number(data_namepat.id) + " ; ";

    QSqlQuery *query_tst = new QSqlQuery(db);
    bool res_tst = mySQL.exec(this, sql_tst, QString("проверим наличие отчества в базе"), *query_tst, true, db, data_app);
    if (!res_tst) {
        delete query_tst;
        QMessageBox::warning(this, "Ошибка при проверке наличие отчества в базе", "При проверке наличия отчества в базе произошла неожиданная ошибка");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    if(query_tst->next() && query_tst->size()>0) {
        QMessageBox::warning(this, "Это отчество уже есть в базе",
                             "Отчество " + query_tst->value(0).toString() + " уже есть в базе.\n"
                             "Для отчества в базе указан пол " + (query_tst->value(1).toInt()==1 ? "М" : "Ж") + ".\n\n"
                             "При необходимости исправьте отчество и повторите операцию.\n");
        delete query_tst;
        this->setCursor(Qt::ArrowCursor);
        return;
    }*/
    // поправим отчество
    QString sql =
            "update spr_namepat_to_sex "
            " set name='" + ui->ln_namepat->text().trimmed().toUpper() + "', "
            "     sex=" + QString::number(ui->combo_sex->currentIndex()) + ", "
            "     date_add='" + QDate::currentDate().toString("yyyy-MM-dd") + "', "
            "     status=" + QString::number(ui->combo_status->currentIndex()) + ", "
            "     id_operator=" + QString::number(data_app.id_operator) + " "
            " where id=" + QString::number(data_namepat.id) + " ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString("изменим отчество в базе"), *query, true, db, data_app);
    if (!res) {
        delete query;
        QMessageBox::warning(this, "Ошибка при попытке изменить запись имени в базе", "При при попытке изменить запись имени в базе произошла неожиданная ошибка");
        this->setCursor(Qt::ArrowCursor);
        return;
    } else {
        QMessageBox::information(this, "Отчество поправлено",
                                 "В справочнике отчеств исправлено отчество " + ui->ln_namepat->text().trimmed().toUpper() + " \n"
                                 "с указанием пола " + (ui->combo_sex->currentIndex()==1 ? "М" : "Ж") + ".\n");
    }
    refresh_tab();
    this->setCursor(Qt::ArrowCursor);
}
