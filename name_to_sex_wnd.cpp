#include "name_to_sex_wnd.h"
#include "ui_name_to_sex_wnd.h"

name_to_sex_wnd::name_to_sex_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent)
    : db(db), data_app(data_app), settings(settings), QDialog(parent), ui(new Ui::name_to_sex_wnd)
{
    ui->setupUi(this);
    refresh_tab();
}

name_to_sex_wnd::~name_to_sex_wnd() {
    delete ui;
}

void name_to_sex_wnd::on_bn_close_clicked() {
    close();
}

void name_to_sex_wnd::on_bn_refresh_clicked() {
    refresh_tab();
}

void name_to_sex_wnd::refresh_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    // данные персоны
    ui->ln_name->clear();
    ui->combo_sex->setCurrentIndex(0);

    QString sql =
            "select s.id, s.name, s.sex, s.date_add, s.status, o.oper_fio "
            " from spr_name_to_sex s "
            "      left join operators o on(o.id=s.id_operator) "
            " where s.name like('" + ui->ln_name_filter->text().trimmed().toUpper() + "%') ";
    if (ui->combo_sex_filter->currentIndex()>0) {
        sql += "and s.sex=" + QString::number(ui->combo_sex_filter->currentIndex()) + " ";
    }

    // сортировка
    sql += " order by sex, name ; ";

    model_name_to_sex.setQuery(sql,db);
    QString err = model_name_to_sex.lastError().driverText();

    // подключаем модель из БД
    ui->tableView->setModel(&model_name_to_sex);

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
    model_name_to_sex.setHeaderData( 0, Qt::Horizontal, ("ID"));
    model_name_to_sex.setHeaderData( 1, Qt::Horizontal, ("имя"));
    model_name_to_sex.setHeaderData( 2, Qt::Horizontal, ("пол"));
    model_name_to_sex.setHeaderData( 3, Qt::Horizontal, ("дата вставки"));
    model_name_to_sex.setHeaderData( 4, Qt::Horizontal, ("ст."));
    model_name_to_sex.setHeaderData( 5, Qt::Horizontal, ("оператор"));

    this->setCursor(Qt::ArrowCursor);
}

void name_to_sex_wnd::on_tableView_clicked(const QModelIndex &index) {
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

    data_name.id = model_name_to_sex.data(model_name_to_sex.index(index.row(), 0), Qt::EditRole).toInt();
    data_name.name = model_name_to_sex.data(model_name_to_sex.index(index.row(), 1), Qt::EditRole).toString();
    data_name.sex = model_name_to_sex.data(model_name_to_sex.index(index.row(), 2), Qt::EditRole).toInt();
    data_name.status = model_name_to_sex.data(model_name_to_sex.index(index.row(), 4), Qt::EditRole).toInt();

    ui->ln_name->setText(model_name_to_sex.data(model_name_to_sex.index(index.row(), 1), Qt::EditRole).toString());
    ui->combo_sex->setCurrentIndex(model_name_to_sex.data(model_name_to_sex.index(index.row(), 2), Qt::EditRole).toInt());
    ui->combo_status->setCurrentIndex(model_name_to_sex.data(model_name_to_sex.index(index.row(), 4), Qt::EditRole).toInt());

    this->setCursor(Qt::ArrowCursor);
}

void name_to_sex_wnd::on_bn_add_name_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    // проверим заполнение полей
    if (ui->combo_sex->currentIndex()==0) {
        QMessageBox::warning(this, "Укажите пол", "Пол не задан.\nОперация отменена.");
        return;
    }
    if (ui->ln_name->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Укажите имя", "Имя не задано.\nОперация отменена.");
        return;
    }

    this->setCursor(Qt::WaitCursor);
    QString sql_tst =
            "select s.name, s.sex "
            " from spr_name_to_sex s "
            " where name='" + ui->ln_name->text().trimmed().toUpper() + "' "
            "   and sex=" + QString::number(data_name.sex) + " ; ";

    QSqlQuery *query_tst = new QSqlQuery(db);
    bool res_tst = mySQL.exec(this, sql_tst, QString("проверим наличие имени в базе"), *query_tst, true, db, data_app);
    if (!res_tst) {
        delete query_tst;
        QMessageBox::warning(this, "Ошибка при проверке наличие имени в базе", "При проверке наличия имени в базе произошла неожиданная ошибка");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    if(query_tst->next() && query_tst->size()>0) {
        QMessageBox::warning(this, "Это имя уже есть в базе",
                             "Имя " + query_tst->value(0).toString() + " уже есть в базе.\n"
                             "Для имени в базе указан пол " + (query_tst->value(1).toInt()==1 ? "М" : "Ж") + ".\n\n"
                             "При необходимости исправьте имя и повторите операцию.\n");
        delete query_tst;
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    // добавим имя
    QString sql =
            "insert into spr_name_to_sex(name, sex, date_add, id_operator) "
            " values('" + ui->ln_name->text().trimmed().toUpper() + "', " +
                     QString::number(ui->combo_sex->currentIndex()) + ", "
                     "'" + QDate::currentDate().toString("yyyy-MM-dd") + "', " +
                     QString::number(data_app.id_operator) + ") ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString("добавим новое имя в базу"), *query, true, db, data_app);
    if (!res) {
        delete query;
        QMessageBox::warning(this, "Ошибка при добавлении нового имени в базу", "При добавлении нового имени в базу произошла неожиданная ошибка");
        this->setCursor(Qt::ArrowCursor);
        return;
    } else {
        QMessageBox::information(this, "Имя добавлено",
                                 "В справочник имён добавлено имя " + ui->ln_name->text().trimmed().toUpper() + " \n"
                                 "с указанием пола " + (ui->combo_sex->currentIndex()==1 ? "М" : "Ж") + ".\n");
    }
    refresh_tab();
    this->setCursor(Qt::ArrowCursor);
}

void name_to_sex_wnd::on_bn_delete_name_clicked() {
    QModelIndexList indexes = ui->tableView->selectionModel()->selection().indexes();
    if (indexes.size()>0) {
        if (QMessageBox::warning(this, "Нужно подтверждение!", "Вы действительно хотите удалить эту запись?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::No) {
            return;
        }
        QModelIndex index = indexes.at(0);
        int id = model_name_to_sex.data(model_name_to_sex.index(index.row(), 0), Qt::EditRole).toInt();

        // удалим текущую запись
        QString sql =
                "delete from spr_name_to_sex "
                " where id=" + QString::number(id) + " ; ";

        QSqlQuery *query2 = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Удаление имени из базы", *query2, true, db, data_app)) {
            delete query2;
            return;
        }
        delete query2;
        refresh_tab();
    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано.");
    }
}

void name_to_sex_wnd::on_ln_name_filter_editingFinished() {
    refresh_tab();
}

void name_to_sex_wnd::on_combo_sex_filter_activated(int index) {
    refresh_tab();
}

void name_to_sex_wnd::on_bn_edit_name_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    // проверим заполнение полей
    if (ui->combo_sex->currentIndex()==0) {
        QMessageBox::warning(this, "Укажите пол", "Пол не задан.\nОперация отменена.");
        return;
    }
    if (ui->ln_name->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Укажите имя", "Имя не задано.\nОперация отменена.");
        return;
    }

    this->setCursor(Qt::WaitCursor);
    /*// проверим наличие имени в базе
    QString sql_tst =
            "select s.name, s.sex "
            " from spr_name_to_sex s "
            " where name='" + ui->ln_name->text().trimmed().toUpper() + "' "
            "   and sex=" + QString::number(data_name.sex) + " "
            "   and id<>" + QString::number(data_name.id) + " ; ";

    QSqlQuery *query_tst = new QSqlQuery(db);
    bool res_tst = mySQL.exec(this, sql_tst, QString("проверим наличие имени в базе"), *query_tst, true, db, data_app);
    if (!res_tst) {
        delete query_tst;
        QMessageBox::warning(this, "Ошибка при проверке наличие имени в базе", "При проверке наличия имени в базе произошла неожиданная ошибка");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    if(query_tst->next() && query_tst->size()>0) {
        QMessageBox::warning(this, "Это имя уже есть в базе",
                             "Имя " + query_tst->value(0).toString() + " уже есть в базе.\n"
                             "Для имени в базе указан пол " + (query_tst->value(1).toInt()==1 ? "М" : "Ж") + ".\n\n"
                             "При необходимости исправьте имя и повторите операцию.\n");
        delete query_tst;
        this->setCursor(Qt::ArrowCursor);
        return;
    }*/
    // поправим имя
    QString sql =
            "update spr_name_to_sex "
            " set name='" + ui->ln_name->text().trimmed().toUpper() + "', "
            "     sex=" + QString::number(ui->combo_sex->currentIndex()) + ", "
            "     date_add='" + QDate::currentDate().toString("yyyy-MM-dd") + "', "
            "     status=" + QString::number(ui->combo_status->currentIndex()) + ", "
            "     id_operator=" + QString::number(data_app.id_operator) + " "
            " where id=" + QString::number(data_name.id) + " ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString("изменим имя в базе"), *query, true, db, data_app);
    if (!res) {
        delete query;
        QMessageBox::warning(this, "Ошибка при попытке изменить запись имени в базе", "При при попытке изменить запись имени в базе произошла неожиданная ошибка");
        this->setCursor(Qt::ArrowCursor);
        return;
    /*} else {
        QMessageBox::information(this, "Имя поправлено",
                                 "В справочнике имён исправлено имя " + ui->ln_name->text().trimmed().toUpper() + " \n"
                                 "с указанием пола " + (ui->combo_sex->currentIndex()==1 ? "М" : "Ж") + ".\n");*/
    }
    refresh_tab();
    this->setCursor(Qt::ArrowCursor);
}
