#include "pvp_n_vs_wnd.h"
#include "ui_pvp_n_vs_wnd.h"

pvp_n_vs_wnd::pvp_n_vs_wnd(QSqlDatabase &db, s_data_app &data_app, s_data_pers &data_pers, QSettings &settings, QWidget *parent)
    : db(db), data_app(data_app), data_pers(data_pers), settings(settings), QDialog(parent), ui(new Ui::pvp_n_vs_wnd)
{
    ui->setupUi(this);
    this->resize(1200,1000);

    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 1);

    /*ui->ch_point_types->setChecked(false);
    on_ch_point_types_clicked(false);
    ui->ch_points->setChecked(true);
    on_ch_points_clicked(true);
    ui->ch_operatops->setChecked(true);
    on_ch_operatops_clicked(true);*/

    on_ch_intervals_pt_all_clicked();
    ui->ch_interval_pt_close->setChecked(false);
    on_ch_interval_pt_close_toggled(false);

    refresh_point_types();
    refresh_points();
    refresh_operators(-1);
    refresh_intervals();
    refresh_intervals_pt(-1,-1);

    refresh_point_types_combo();
    refresh_combo_oper_rights_combo();
    refresh_folders();

    refresh_combo_act_obtainers(-1);

    ui->date_interval->setDate(QDate::currentDate());
    ui->date_interval_pt->setDate(QDate::currentDate());

    ui->bns_point_types->setEnabled(data_app.is_tech);
    //ui->bns_intervals->setEnabled(!(data_app.f_readonly));
    //ui->bns_intervals_pt->setEnabled(!(data_app.f_readonly));

    list_points.clear();
    list_points.append(0);
    list_points.append(500);
    list_points.append(400);
    ui->splitter_points->setSizes(list_points);

    list_intervals.clear();
    list_intervals.append(250);
    list_intervals.append(650);
    ui->splitter_intervals->setSizes(list_intervals);

    //ui->bn_edit_interval->setVisible(false);
    ui->bn_delete_interval->setEnabled(false);
    //ui->bn_edit_interval_pt->setVisible(false);
    ui->bn_delete_interval_pt->setEnabled(false);

    ui->date_act->setDate(QDate::currentDate());
    ui->date_act_pt->setDate(QDate::currentDate());

    ui->combo_point_rights->clear();
    ui->combo_point_rights->addItem("(1) обычный ПВП", 1);
    ui->combo_point_rights->addItem("(2) головной офис", 2);
    ui->combo_point_rights->addItem("(3) техподдержка", 3);
    ui->combo_point_rights->addItem("(0) служебный", 0);
}

pvp_n_vs_wnd::~pvp_n_vs_wnd() {
    delete ui;
}
/*
void pvp_n_vs_wnd::on_ch_point_types_clicked(bool checked) {
    ui->tab_point_types->setVisible(checked);
    ui->bns_point_types->setVisible(checked);
    list_points[0] = (checked?10:1);
    ui->splitter_points->setSizes(list_points);
    ui->pan_point_types->resize(QSize(ui->pan_point_types->size().width(),checked?200:20));
    ui->pan_points->resize(QSize(ui->pan_points->size().width(),checked?20:200));
}
void pvp_n_vs_wnd::on_ch_points_clicked(bool checked) {
    ui->tab_points->setVisible(checked);
    ui->bns_points->setVisible(checked);
    list_points[1] = (checked? 8:1);
    ui->splitter_points->setSizes(list_points);
}
void pvp_n_vs_wnd::on_ch_operatops_clicked(bool checked) {
    ui->tab_operators->setVisible(checked);
    ui->bns_operators->setVisible(checked);
    list_points[2] = (checked?10:1);
    ui->splitter_points->setSizes(list_points);
}
void pvp_n_vs_wnd::on_ch_intervals_clicked(bool checked) {
    ui->tab_intervals->setVisible(checked);
    ui->bns_intervals->setVisible(checked);
    list_intervals[0] = (checked? 5:1);
    ui->splitter_intervals->setSizes(list_intervals);
}
void pvp_n_vs_wnd::on_ch_intervals_pt_clicked(bool checked) {
    ui->tab_intervals_pt->setVisible(checked);
    ui->bns_intervals_pt->setVisible(checked);
    list_intervals[1] = (checked?10:1);
    ui->splitter_intervals->setSizes(list_intervals);
}
*/
void pvp_n_vs_wnd::on_bn_close_clicked() {
    close();
}



// --------------------------------------------------------------
// категории ПВП
// --------------------------------------------------------------
void pvp_n_vs_wnd::refresh_point_types() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    // данные персоны
    ui->ln_point_type_name->setText("");
    ui->ln_point_type_code->setText("");

    QString point_types_sql = "select p.id, p.point_type_name, p.point_type_code "
                              " from point_types p ; ";

    model_point_types.setQuery(point_types_sql,db);
    QString err = model_point_types.lastError().driverText();

    // подключаем модель из БД
    ui->tab_point_types->setModel(&model_point_types);

    // обновляем таблицу
    ui->tab_point_types->reset();

    // задаём ширину колонок
    ui->tab_point_types->setColumnWidth( 0, 30);    // p.id
    ui->tab_point_types->setColumnWidth( 1,300);    // p.point_type_name
    ui->tab_point_types->setColumnWidth( 2,140);    // p.point_type_code

    // правим заголовки
    model_point_types.setHeaderData( 1, Qt::Horizontal, ("название типа ПВП"));
    model_point_types.setHeaderData( 2, Qt::Horizontal, ("код типа ПВП"));
    ui->tab_point_types->repaint();

    //refresh_assigs_tab();
    //refresh_links_tab();
    //refresh_addresses_tab();
    this->setCursor(Qt::ArrowCursor);
}

void pvp_n_vs_wnd::on_bn_refresh_point_types_clicked() {
    refresh_point_types();
}

void pvp_n_vs_wnd::on_tab_point_types_clicked(const QModelIndex &index) {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_points || !ui->tab_points->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить].");
        return;
    }
    this->setCursor(Qt::WaitCursor);

    ui->ln_point_type_name->setText (model_point_types.data(model_point_types.index(index.row(), 1), Qt::EditRole).toString());
    ui->ln_point_type_code->setText (model_point_types.data(model_point_types.index(index.row(), 2), Qt::EditRole).toString());

    this->setCursor(Qt::ArrowCursor);
}

void pvp_n_vs_wnd::on_bn_add_point_type_clicked() {
    // Проверим заполнение полей
    if (ui->ln_point_type_name->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Не достаточно данных!", "Название категории пунктов выдачи полисов не может быть пустым.");
        return;
    }
    if (ui->ln_point_type_code->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Не достаточно данных!", "Код категории пунктов выдачи полисов не может быть пустым.");
        return;
    }
    // проверим на уникальность названия
    QString sql_name  = "select id from point_types where (point_type_name='" + ui->ln_point_type_name->text().trimmed() + "') ; ";
    QSqlQuery *query_name = new QSqlQuery(db);
    mySQL.exec(this, sql_name, "Проверка на уникальность имени оператора", *query_name, true, db, data_app);
    if (query_name->size()>0) {
        QMessageBox::warning(this, "Название занято", "Введённое название категории ПВП уже испольуется в другой записи.");
        return;
    }
    delete query_name;

    // проверим на уникальность кода
    QString sql_code  = "select id from point_types where (point_type_code='" + ui->ln_point_type_code->text().trimmed() + "') ; ";
    QSqlQuery *query_code = new QSqlQuery(db);
    mySQL.exec(this, sql_code, "Проверка на уникальность имени оператора", *query_code, true, db, data_app);
    if (query_code->size()>0) {
        QMessageBox::warning(this, "Код занят", "Введённый код категории ПВП уже испольуется в другой записи.");
        return;
    }
    delete query_code;

    // собственно сохранение
    QString sql = "insert into point_types(point_type_code, point_type_name) ";
    sql += " values (";
    sql += " '"  + ui->ln_point_type_code->text().trimmed() + "', ";
    sql += " '"  + ui->ln_point_type_name->text().trimmed() + "') ; ";

    QSqlQuery *query2 = new QSqlQuery(db);
    mySQL.exec(this, sql, "Добавление записи категории ПВП", *query2, true, db, data_app);
    delete query2;

    refresh_point_types();
}

void pvp_n_vs_wnd::on_bn_edit_point_type_clicked() {
    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_point_types->selectionModel()->selection().indexes();
    if (indexes.size()>0) {
        QModelIndex index = indexes.at(0);
        int id = model_point_types.data(model_point_types.index(index.row(), 0), Qt::EditRole).toInt();

        // проверим на уникальность названия
        QString sql_name  = "select id from point_types where (point_type_name='" + ui->ln_point_type_name->text().trimmed() + "') ; ";
        QSqlQuery *query_name = new QSqlQuery(db);
        mySQL.exec(this, sql_name, "Проверка на уникальность имени оператора", *query_name, true, db, data_app);
        if (query_name->size()>0) {
            QMessageBox::warning(this, "Название занято", "Введённое название категории ПВП уже испольуется в другой записи.");
            return;
        }
        delete query_name;

        // проверим на уникальность кода
        QString sql_code  = "select id from point_types where (point_type_code='" + ui->ln_point_type_code->text().trimmed() + "') ; ";
        QSqlQuery *query_code = new QSqlQuery(db);
        mySQL.exec(this, sql_code, "Проверка на уникальность имени оператора", *query_code, true, db, data_app);
        if (query_code->size()>0) {
            QMessageBox::warning(this, "Код занят", "Введённый код категории ПВП уже испольуется в другой записи.");
            return;
        }
        delete query_code;

        // изменим текущую запись
        QString sql = "update point_types set";
        sql += " point_type_name='" + ui->ln_point_type_name->text() + "', ";
        sql += " point_type_code='" + ui->ln_point_type_code->text() + "' ";
        sql += " where id=" + QString::number(id);
        sql += " ; ";

        QSqlQuery *query2 = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Правка записи категории ПВП", *query2, true, db, data_app))
            return;
        delete query2;
        refresh_point_types();
    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано.");
    }
}

void pvp_n_vs_wnd::on_bn_delete_point_type_clicked() {
    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_point_types->selectionModel()->selection().indexes();
    if (indexes.size()>0) {
        if (QMessageBox::warning(this, "Нужно подтверждение!", "Вы действительно хотите удалить эту запись?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::No) {
            return;
        }
        QModelIndex index = indexes.at(0);
        int id = model_point_types.data(model_point_types.index(index.row(), 0), Qt::EditRole).toInt();

        // изменим текущую запись
        QString sql = "delete from point_types ";
        sql += " where id=" + QString::number(id);
        sql += " ; ";

        QSqlQuery *query2 = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Удаление записи категории пунктов выдачи", *query2, true, db, data_app)) {
            delete query2;
            return;
        }
        if (query2->size()>0) {
            query2->next();
            data_pers.id = query2->value(0).toInt();
        }
        delete query2;
        refresh_point_types();
    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано.");
    }
}



// --------------------------------------------------------------
// пункты выдачи полисов
// --------------------------------------------------------------
void pvp_n_vs_wnd::refresh_points() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    // данные персоны
    ui->ln_point_name->setText("");
    ui->ln_point_regnum->setText("");
    ui->combo_point_code->setCurrentIndex(0);

    QString points_sql = "select t.id as id_t, t.point_type_name, p.id, p.point_name, p.point_regnum, p.point_code, p.status, p.point_rights, p.address "
                         " from points p left join point_types t on(t.point_type_code=p.point_code) order by p.point_code, p.point_regnum; ";

    model_points.setQuery(points_sql,db);
    QString err = model_points.lastError().driverText();

    // подключаем модель из БД
    ui->tab_points->setModel(&model_points);

    // обновляем таблицу
    ui->tab_points->reset();

    // задаём ширину колонок
    ui->tab_points->setColumnWidth( 0,  1);    // p.id_point_type
    ui->tab_points->setColumnWidth( 1,170);    // p.point_type_name
    ui->tab_points->setColumnWidth( 2,  1);    // p.id
    ui->tab_points->setColumnWidth( 3,180);    // p.point_name
    ui->tab_points->setColumnWidth( 4, 50);    // p.point_regnum
    ui->tab_points->setColumnWidth( 5,  1);    // p.point_code
    ui->tab_points->setColumnWidth( 6, 50);    // p.status
    ui->tab_points->setColumnWidth( 7,  1);    // p.point_rights
    ui->tab_points->setColumnWidth( 8,100);    // p.address

    // правим заголовки
    model_points.setHeaderData( 1, Qt::Horizontal, ("тип ПВП"));
    model_points.setHeaderData( 3, Qt::Horizontal, ("название ПВП"));
    model_points.setHeaderData( 4, Qt::Horizontal, ("код\nПВП"));
    model_points.setHeaderData( 6, Qt::Horizontal, ("статус\nПВП"));
    model_points.setHeaderData( 7, Qt::Horizontal, ("разрешения"));
    model_points.setHeaderData( 8, Qt::Horizontal, ("почтовый адрес"));
    ui->tab_points->repaint();

    //refresh_assigs_tab();
    //refresh_links_tab();
    //refresh_addresses_tab();
    this->setCursor(Qt::ArrowCursor);
    refresh_intervals_pt(-1, -1);
}

void pvp_n_vs_wnd::refresh_point_types_combo() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка категорий ПВП
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select point_type_code, point_type_name "
                  " from public.point_types t "
                  " order by point_type_code ; ";
    mySQL.exec(this, sql, QString("список категорий ПВП"), *query, true, db, data_app);
    ui->combo_point_code->clear();
    ui->combo_point_code->addItem(" - ? - ", -1);
    while (query->next()) {
        int n = query->value(0).toInt();
        ui->combo_point_code->addItem(query->value(1).toString(), n);
    }
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void pvp_n_vs_wnd::refresh_combo_act_obtainers(int id_point) {
    this->setCursor(Qt::WaitCursor);

    // проверим выделенную строку пункта выдачи
    QModelIndexList indexes_point = ui->tab_points->selectionModel()->selection().indexes();

    id_point = -1;
    if (indexes_point.size()>0) {
        QModelIndex index_point = indexes_point.front();
        id_point = model_points.data(model_points.index(index_point.row(), 2), Qt::EditRole).toInt();
        refresh_operators(id_point);
    }

    // обновление выпадающего списка работников ПВП
    QSqlQuery *query = new QSqlQuery(db);
    QString sql;
    sql = "select o.id, o.oper_fio "
          "  from operators o "
          "  left join points p on(o.id_point=p.id) "
          " where o.status=1 ";
    if (id_point>=0)
        sql += " and p.status=1 and p.id=" + QString::number(id_point) + " ";
    sql += " order by o.oper_fio ; ";

    mySQL.exec(this, sql, QString("Список работников ПВП"), *query, true, db, data_app);
    ui->combo_act_obtainer->clear();
    while (query->next()) {
        ui->combo_act_obtainer->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_act_obtainer->setCurrentIndex(0);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void pvp_n_vs_wnd::on_bn_refresh_points_clicked() {
    refresh_points();
}

void pvp_n_vs_wnd::on_tab_points_clicked(const QModelIndex &index) {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_points || !ui->tab_points->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить].");
        return;
    }
    this->setCursor(Qt::WaitCursor);

    ui->ln_point_name->setText (model_points.data(model_points.index(index.row(), 3), Qt::EditRole).toString());
    ui->ln_point_regnum->setText (model_points.data(model_points.index(index.row(), 4), Qt::EditRole).toString());
    ui->combo_point_code->setCurrentIndex (ui->combo_point_code->findData ((model_points.data(model_points.index(index.row(), 5), Qt::EditRole).toInt())));
    ui->combo_point_status->setCurrentIndex (model_points.data(model_points.index(index.row(), 6), Qt::EditRole).toInt());
    ui->combo_point_rights->setCurrentIndex (ui->combo_point_rights->findData(model_points.data(model_points.index(index.row(), 7), Qt::EditRole).toInt()));
    ui->ln_point_address->setText (model_points.data(model_points.index(index.row(), 8), Qt::EditRole).toString());

    // проверим выделенную строку пункта выдачи
    QModelIndexList indexes_point = ui->tab_points->selectionModel()->selection().indexes();

    if (indexes_point.size()>0) {
        QModelIndex index_point = indexes_point.front();
        int id_point = model_points.data(model_points.index(index_point.row(), 2), Qt::EditRole).toInt();
        refresh_operators(id_point);

        // проверим выделенную строку интервала ВС
        QModelIndexList indexes_int = ui->tab_intervals->selectionModel()->selection().indexes();

        if (indexes_int.size()>0) {
            QModelIndex index_int = indexes_int.front();
            int id_int = model_intervals.data(model_intervals.index(index_int.row(), 0), Qt::EditRole).toInt();
            refresh_intervals_pt(id_point, id_int);
        }
    }
    on_tab_intervals_pt_clicked(index);

    this->setCursor(Qt::ArrowCursor);

    refresh_combo_act_obtainers(-1);
}

void pvp_n_vs_wnd::on_bn_add_point_clicked() {
    // Проверим заполнение полей
    if (ui->ln_point_name->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Не достаточно данных!", "Название пункта выдачи полисов не может быть пустым.");
        return;
    }
    if (ui->ln_point_regnum->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Не достаточно данных!", "Введите реестровый номер пункта выдачи полисов.");
        return;
    }
    if (ui->ln_point_address->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Не достаточно данных!", "Введите почтовый адрес пункта выдачи полисов.");
        return;
    }
    if (ui->combo_point_code->currentIndex()==0) {
        QMessageBox::warning(this, "Не достаточно данных!", "Выберите категорию пунка выдачи полисов.");
        return;
    }
    // собственно сохранение
    QString sql = "insert into points(point_regnum, point_code, point_name, point_rights, status, address) "
                  " values ('" + QString("000" + ui->ln_point_regnum->text().trimmed()).right(3) + "',"
                           " " + QString::number(ui->combo_point_code->currentData().toInt())    + ", "
                           "'" + ui->ln_point_name->text()                                       + "',"
                           " " + QString::number(ui->combo_point_rights->currentData().toInt())  + ", "
                           " " + QString::number(ui->combo_point_status->currentIndex())         + ","
                           "'" + ui->ln_point_address->text()                                    + "') "
                           " returning id ; ";

    QSqlQuery *query2 = new QSqlQuery(db);
    mySQL.exec(this, sql, "Добавление записи ПВП", *query2, true, db, data_app);
    delete query2;

    refresh_points();
}

void pvp_n_vs_wnd::on_bn_edit_point_clicked() {
    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_points->selectionModel()->selection().indexes();
    if (indexes.size()>0) {
        QModelIndex index = indexes.at(0);
        int id = model_points.data(model_points.index(index.row(), 2), Qt::EditRole).toInt();

        // изменим текущую запись
        QString sql = "update points "
                      " set point_name='" + ui->ln_point_name->text() + "', "
                      "     point_regnum='" + QString("000" + ui->ln_point_regnum->text().trimmed()).right(3) + "', "
                      "     point_code=" + QString::number(ui->combo_point_code->currentData().toInt()) + ", "
                      "     point_rights=" + QString::number(ui->combo_point_rights->currentData().toInt()) + ", "
                      "     status=" + QString::number(ui->combo_point_status->currentIndex()) + ", "
                      "     address='" + ui->ln_point_address->text() + "' "
                      " where id=" + QString::number(id) + " ; ";

        QSqlQuery *query2 = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Правка записи пункта выдачи", *query2, true, db, data_app))
            return;
        if (query2->size()>0) {
            query2->next();
            data_pers.id = query2->value(0).toInt();
        }
        delete query2;
        refresh_points();
    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано.");
    }
}

void pvp_n_vs_wnd::on_bn_delete_point_clicked() {
    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_points->selectionModel()->selection().indexes();
    if (indexes.size()>0) {
        if (QMessageBox::warning(this, "Нужно подтверждение!", "Вы действительно хотите удалить эту запись?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::No) {
            return;
        }
        QModelIndex index = indexes.at(0);
        int id = model_points.data(model_points.index(index.row(), 2), Qt::EditRole).toInt();

        // изменим текущую запись
        QString sql = "delete from points ";
        sql += " where id=" + QString::number(id);
        sql += " ; ";

        QSqlQuery *query2 = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Удаление записи пункта выдачи", *query2, true, db, data_app)) {
            delete query2;
            return;
        }
        if (query2->size()>0) {
            query2->next();
            data_pers.id = query2->value(0).toInt();
        }
        delete query2;
        refresh_points();
    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано.");
    }
}



// --------------------------------------------------------------
// операторы ПВП
// --------------------------------------------------------------
void pvp_n_vs_wnd::refresh_operators(int id_point) {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    // данные оператора
    ui->ln_oper_fio->setText("");
    //ui->ln_user_name->setText("");
    ui->ln_user_pass->setText("");
    ui->ln_user_pass2->setText("");
    ui->combo_oper_rights->setCurrentIndex(0);

    QString operators_sql = "select p.id as id_point, p.point_name, o.id as id_oper, o.oper_fio, NULL/*, o.user_name*/, o.oper_rights, o.status ";
    if (data_app.is_tech) {
        operators_sql += ", o.user_pass ";
    }
    operators_sql += "  from operators o left join points p on(p.id=o.id_point) "
                     " where p.id=" + QString::number(id_point) + " "
                     " order by p.point_name, o.oper_fio; ";

    model_operators.setQuery(operators_sql,db);
    QString err = model_operators.lastError().driverText();

    // подключаем модель из БД
    ui->tab_operators->setModel(&model_operators);

    // обновляем таблицу
    ui->tab_operators->reset();

    // задаём ширину колонок
    ui->tab_operators->setColumnWidth( 0,  1);    // p.id
    ui->tab_operators->setColumnWidth( 1,150);    // p.point_name
    ui->tab_operators->setColumnWidth( 2,  1);    // o.id
    ui->tab_operators->setColumnWidth( 3,160);    // o.oper_fio
    ui->tab_operators->setColumnWidth( 4,  1);    // o.user_name
    ui->tab_operators->setColumnWidth( 5,  1);    // o.oper_rights
    ui->tab_operators->setColumnWidth( 6, 50);    // o.status
    if (data_app.is_tech) {
        ui->tab_operators->setColumnWidth( 6, 70);    // o.user_pass
    }

    // правим заголовки
    model_operators.setHeaderData( 1, Qt::Horizontal, ("название ПВП"));
    model_operators.setHeaderData( 3, Qt::Horizontal, ("ФИО орператора"));
    model_operators.setHeaderData( 5, Qt::Horizontal, ("разреш."));
    model_operators.setHeaderData( 6, Qt::Horizontal, ("статус"));
    if (data_app.is_tech) {
        model_operators.setHeaderData( 7, Qt::Horizontal, ("пароль"));
    }
    ui->tab_operators->repaint();

    //refresh_assigs_tab();
    //refresh_links_tab();
    //refresh_addresses_tab();
    this->setCursor(Qt::ArrowCursor);
}

void pvp_n_vs_wnd::refresh_combo_oper_rights_combo() {
}

void pvp_n_vs_wnd::on_bn_refresh_operators_clicked() {
    // проверим выделенную строку пункта выдачи
    QModelIndexList indexes_point = ui->tab_points->selectionModel()->selection().indexes();

    if (indexes_point.size()>0) {
        QModelIndex index_point = indexes_point.front();
        int id_point = model_points.data(model_points.index(index_point.row(), 2), Qt::EditRole).toInt();
        refresh_operators(id_point);
    } else {
        refresh_operators(-1);
    }
}

void pvp_n_vs_wnd::on_tab_operators_clicked(const QModelIndex &index) {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_points || !ui->tab_points->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить].");
        return;
    }
    this->setCursor(Qt::WaitCursor);

    int id_oper = model_operators.data(model_operators.index(index.row(), 2), Qt::EditRole).toInt();
    ui->bn_edit_operator->setEnabled(id_oper==data_app.id_operator);

    ui->ln_oper_fio->setText (model_operators.data(model_operators.index(index.row(), 3), Qt::EditRole).toString());
    //ui->ln_user_name->setText (model_operators.data(model_operators.index(index.row(), 4), Qt::EditRole).toString());
    ui->ln_user_pass->setText ("");
    ui->ln_user_pass2->setText ("");
    ui->combo_oper_rights->setCurrentIndex (model_operators.data(model_operators.index(index.row(), 5), Qt::EditRole).toInt());
    ui->combo_oper_status->setCurrentIndex (model_operators.data(model_operators.index(index.row(), 6), Qt::EditRole).toInt());

    this->setCursor(Qt::ArrowCursor);
}

void pvp_n_vs_wnd::on_bn_add_operator_clicked() {
    // проверим выделенный пункт выдачи
    QModelIndexList indexes = ui->tab_points->selectionModel()->selection().indexes();
    if (indexes.size()>0) {
        QModelIndex index = indexes.at(0);
        int point_id = model_points.data(model_points.index(index.row(), 2), Qt::EditRole).toInt();

        // Проверим заполнение полей
        if (ui->ln_oper_fio->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Не достаточно данных!", "ФИО оператора не может быть пустым.");
            return;
        }
        /*if (ui->ln_user_name->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Не достаточно данных!", "USER NAME не может быть пустым.");
            return;
        }*/
        if (ui->ln_user_pass->text().isEmpty()) {
            QMessageBox::warning(this, "Не достаточно данных!", "Не задан пароль опреатора.");
            return;
        }
        if (ui->ln_user_pass2->text().isEmpty()) {
            QMessageBox::warning(this, "Не достаточно данных!", "Введите пароль ещё раз.");
            return;
        }
        if (ui->ln_user_pass->text()!=ui->ln_user_pass2->text()) {
            QMessageBox::warning(this, "Пароли не совпадают!", "Пароль и подтверждение не совпадают.");
            return;
        }
        if (ui->combo_oper_rights->currentIndex()==0) {
            QMessageBox::warning(this, "Задайте разрешения!", "Разрешения \" - не задано - \" не поддерживаются в данной версии программы.");
            return;
        }
        // проверим на совпадение имени
        QString sql_name  = "select id from operators where (oper_fio='" + ui->ln_oper_fio->text().trimmed() + "') ";
                sql_name += " and id_point=" + QString::number(point_id);
                sql_name += " ; ";
        QSqlQuery *query = new QSqlQuery(db);
        mySQL.exec(this, sql_name, "Проверка на уникальность имени оператора", *query, true, db, data_app);
        if (query->size()>0) {
            QMessageBox::warning(this, "Имя занято", "ФИО оператора или user name уже заняты.");
            return;
        }
        delete query;

        // собственно сохранение
        QString sql = "insert into operators(id_point, user_name, user_pass, oper_fio, oper_rights, status) "
                      " values (" + QString::number(point_id) + ", "
                                " NULL, "
                                "'" + ui->ln_user_pass->text().trimmed() + "', "
                                "'" + ui->ln_oper_fio->text().trimmed() + "', "
                                " " + QString::number(ui->combo_oper_rights->currentIndex()) + ", "
                                " " + QString::number(ui->combo_oper_status->currentIndex()) + ") "
                                " returning id ; ";

        QSqlQuery *query2 = new QSqlQuery(db);
        mySQL.exec(this, sql, "Добавление записи оператора", *query2, true, db, data_app);
        delete query2;

        refresh_operators(point_id);
    } else {
        QMessageBox::warning(this, "Не выбран пункт выдачи полисов", "Сначала выберите пункт выдачи полисов, в котором будет работать новый оператор.");
    }
}

void pvp_n_vs_wnd::on_bn_edit_operator_clicked() {
    // проверим выделенный пункт выдачи
    QModelIndexList indexes = ui->tab_points->selectionModel()->selection().indexes();
    if (indexes.size()>0) {
        QModelIndex index = indexes.at(0);
        int point_id = model_points.data(model_points.index(index.row(), 2), Qt::EditRole).toInt();

        // проверим выделенную строку
        QModelIndexList indexes = ui->tab_operators->selectionModel()->selection().indexes();
        if (indexes.size()>0) {
            QModelIndex index = indexes.at(0);
            int id = model_operators.data(model_operators.index(index.row(), 2), Qt::EditRole).toInt();

            // Проверим заполнение полей
            if (ui->ln_oper_fio->text().trimmed().isEmpty()) {
                QMessageBox::warning(this, "Не достаточно данных!", "ФИО оператора не может быть пустым.");
                return;
            }
            /*if (ui->ln_user_name->text().trimmed().isEmpty()) {
                QMessageBox::warning(this, "Не достаточно данных!", "Поле \"user name\" не может быть пустым.");
                return;
            }*/
            if (ui->ln_user_pass->text().isEmpty()) {
                QMessageBox::warning(this, "Не достаточно данных!", "Не задан пароль опреатора.");
                return;
            }
            if (ui->ln_user_pass2->text().isEmpty()) {
                QMessageBox::warning(this, "Не достаточно данных!", "Введите пароль ещё раз.");
                return;
            }
            if (ui->ln_user_pass->text()!=ui->ln_user_pass2->text()) {
                QMessageBox::warning(this, "Пароли не совпадают!", "Пароль и подтверждение не совпадают.");
                return;
            }
            if (ui->combo_oper_rights->currentIndex()==0) {
                QMessageBox::warning(this, "Задайте разрешения!", "Разрешения \" - не задано - \" не поддерживаются в данной версии программы.");
                return;
            }
            // изменим текущую запись
            QString sql = "update operators "
                          " set id_point=" + QString::number(point_id) + ", "
                              " user_name=NULL, "
                              " user_pass='" + ui->ln_user_pass->text() + "', "
                              " oper_fio='" + ui->ln_oper_fio->text() + "', "
                              " oper_rights=" + QString::number(ui->combo_oper_rights->currentIndex()) + ", "
                              " status=" + QString::number(ui->combo_oper_status->currentIndex()) + " "
                          " where id=" + QString::number(id) + " ; ";

            QSqlQuery *query2 = new QSqlQuery(db);
            if (!mySQL.exec(this, sql, "Обновление записи оператора", *query2, true, db, data_app))
                return;
            if (query2->size()>0) {
                query2->next();
                data_pers.id = query2->value(0).toInt();
            }
            delete query2;
            refresh_operators(point_id);
        }
    } else {
        QMessageBox::warning(this, "Не выбран пункт выдачи полисов", "Сначала выберите пункт выдачи полисов, в котором работает оператор.");
    }
}

void pvp_n_vs_wnd::on_bn_delete_operator_clicked() {
    // проверим выделенный пункт выдачи
    QModelIndexList indexes = ui->tab_points->selectionModel()->selection().indexes();
    if (indexes.size()>0) {
        QModelIndex index = indexes.at(0);
        int point_id = model_points.data(model_points.index(index.row(), 2), Qt::EditRole).toInt();

        // проверим выделенную строку
        QModelIndexList indexes = ui->tab_operators->selectionModel()->selection().indexes();
        if (indexes.size()>0) {
            if (QMessageBox::warning(this, "Нужно подтверждение!", "Вы действительно хотите удалить эту запись?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::No) {
                return;
            }
            QModelIndex index = indexes.at(0);
            int id = model_operators.data(model_operators.index(index.row(), 2), Qt::EditRole).toInt();

            // изменим текущую запись
            QString sql = "delete from operators ";
            sql += " where id=" + QString::number(id);
            sql += " ; ";

            QSqlQuery *query2 = new QSqlQuery(db);
            if (!mySQL.exec(this, sql, "Удаление записи оператора", *query2, true, db, data_app)) {
                delete query2;
                return;
            }
            if (query2->size()>0) {
                query2->next();
                data_pers.id = query2->value(0).toInt();
            }
            delete query2;
            refresh_operators(point_id);
        }
    } else {
        QMessageBox::warning(this, "Не выбран пункт выдачи полисов", "Сначала выберите пункт выдачи полисов, в котором работал оператор.");
    }
}



// --------------------------------------------------------------
// интервалы ВС
// --------------------------------------------------------------
void pvp_n_vs_wnd::refresh_intervals() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    // данные персоны
    ui->date_interval->setDate(QDate::currentDate());
    ui->spin_interval_from->setValue(-1);
    ui->spin_interval_to->setValue(-1);
    ui->spin_interval_count->setValue(-1);

    QString intervals_sql = "select p.id, p.date_get, p.vs_from, p.vs_to, p.vs_count, p.act_num, p.act_date "
                            "  from vs_intervals p "
                            " order by p.vs_from ; ";

    model_intervals.setQuery(intervals_sql,db);
    QString err = model_intervals.lastError().driverText();

    // подключаем модель из БД
    ui->tab_intervals->setModel(&model_intervals);

    // обновляем таблицу
    ui->tab_intervals->reset();

    // задаём ширину колонок
    ui->tab_intervals->setColumnWidth( 0, 30);    // p.id
    ui->tab_intervals->setColumnWidth( 1,100);    // p.date_get
    ui->tab_intervals->setColumnWidth( 2, 80);    // p.vs_from
    ui->tab_intervals->setColumnWidth( 3, 80);    // p.vs_to
    ui->tab_intervals->setColumnWidth( 4, 80);    // p.vs_count
    ui->tab_intervals->setColumnWidth( 5,120);    // p.act_num
    ui->tab_intervals->setColumnWidth( 6, 80);    // p.act_date

    // правим заголовки
    model_intervals.setHeaderData( 1, Qt::Horizontal, ("дата \nполучения"));
    model_intervals.setHeaderData( 2, Qt::Horizontal, ("№ ВС от"));
    model_intervals.setHeaderData( 3, Qt::Horizontal, ("№ ВС до"));
    model_intervals.setHeaderData( 4, Qt::Horizontal, ("число ВС"));
    model_intervals.setHeaderData( 5, Qt::Horizontal, ("номер \nакта"));
    model_intervals.setHeaderData( 6, Qt::Horizontal, ("дата \nакта"));
    ui->tab_intervals->repaint();

    this->setCursor(Qt::ArrowCursor);
    refresh_intervals_pt(-1, -1);
}

void pvp_n_vs_wnd::on_tab_intervals_clicked(const QModelIndex &index) {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_points || !ui->tab_points->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите ПВП из списка.");
        return;
    }
    this->setCursor(Qt::WaitCursor);

    ui->date_act->setDate(QDate::currentDate());

    ui->date_interval->setDate(model_intervals.data(model_intervals.index(index.row(), 1), Qt::EditRole).toDate());
    ui->spin_interval_from->setValue(model_intervals.data(model_intervals.index(index.row(), 2), Qt::EditRole).toInt());
    ui->spin_interval_to->setValue(model_intervals.data(model_intervals.index(index.row(), 3), Qt::EditRole).toInt());
    ui->spin_interval_count->setValue(ui->spin_interval_to->value() - ui->spin_interval_from->value() +1);
    ui->ln_act_num->setText(model_intervals.data(model_intervals.index(index.row(), 5), Qt::EditRole).toString());
    ui->date_act->setDate(model_intervals.data(model_intervals.index(index.row(), 6), Qt::EditRole).toDate());

    // проверим выделенную строку интервала ВС
    QModelIndexList indexes_int = ui->tab_intervals->selectionModel()->selection().indexes();

    if (indexes_int.size()>0) {
        QModelIndex index_int = indexes_int.front();
        int id_int = model_intervals.data(model_intervals.index(index_int.row(), 0), Qt::EditRole).toInt();

        // проверим выделенную строку пункта выдачи
        QModelIndexList indexes_point = ui->tab_points->selectionModel()->selection().indexes();

        if (indexes_point.size()>0) {
            QModelIndex index_point = indexes_point.front();
            int id_point = model_points.data(model_points.index(index_point.row(), 2), Qt::EditRole).toInt();

            refresh_intervals_pt(id_point, id_int);
        }
    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано.");
    }
    on_tab_intervals_pt_clicked(index);

    this->setCursor(Qt::ArrowCursor);
}

void pvp_n_vs_wnd::on_spin_interval_from_valueChanged(int arg1) {
    ui->spin_interval_count->setValue(ui->spin_interval_to->value() - ui->spin_interval_from->value() +1);
}

void pvp_n_vs_wnd::on_spin_interval_to_valueChanged(int arg1) {
    ui->spin_interval_count->setValue(ui->spin_interval_to->value() - ui->spin_interval_from->value() +1);
}

void pvp_n_vs_wnd::on_bn_refresh_intervals_clicked() {
    refresh_intervals();
}

void pvp_n_vs_wnd::on_bn_add_interval_clicked() {
    // Проверим заполнение полей
    if (ui->date_interval->date()<QDate::currentDate().addMonths(-1) || ui->date_interval->date()>QDate::currentDate().addMonths(1)) {
        QMessageBox::warning(this, "Недопустимое значение!", "Дата выдачи интервала ВС не может отстоять от текущей более чем на 2 месяца.");
        return;
    }
    if (ui->ln_act_num->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Недопустимое значение!", "Название акта не может быть пустым.");
        return;
    }
    if (ui->spin_interval_from->value()<1) {
        QMessageBox::warning(this, "Недопустимое значение!", "Начальное значение интервала ВС не может быть меньше 1.");
        return;
    }
    if (ui->spin_interval_to->value()<ui->spin_interval_from->value()+5) {
        QMessageBox::warning(this, "Недопустимое значение!", "Конечное значение интервала ВС не может быть меньше чем начальное значение +5.");
        return;
    }
    // проверка на пересечение с уже заданными интервалами
    QString fr = QString::number(ui->spin_interval_from->value());
    QString to = QString::number(ui->spin_interval_to->value());
    QString sql_test = "select id, vs_from, vs_to from vs_intervals where "
                             " (vs_from<="+fr+" and vs_to>="+fr+") or "
                             " (vs_from<="+to+" and vs_to>="+to+") or "
                             " (vs_from>="+fr+" and vs_to<="+to+") or "
                             " (vs_from<="+fr+" and vs_to>="+to+") ";
    QSqlQuery *query_test = new QSqlQuery(db);
    mySQL.exec(this, sql_test, "Проверка интервала ВС на непересечение с другими", *query_test, true, db, data_app);
    if (query_test->size()>0) {
        QString res = "Данный интервал ВС пересекается с другим, ранее введённым:\n\n";
        while (query_test->next()) {
            res += "id=" + QString::number(query_test->value(0).toInt());
            res += "   -   от " + QString::number(query_test->value(1).toInt()) + " до " + QString::number(query_test->value(2).toInt()) + "\n";
        }
        res += "\nОперация отменена.";
        QMessageBox::warning(this, "Пересечение интервалов ВС", res);
        return;
    }

    // собственно сохранение
    QString sql = "insert into vs_intervals(date_get, vs_from, vs_count, vs_to, act_num, act_date) ";
    sql += " values (";
    sql += "'"  + ui->date_interval->date().toString("yyyy-MM-dd") + "', ";
    sql += " "  + fr + ", ";
    sql += " "  + QString::number(ui->spin_interval_count->value()) + ", ";
    sql += " "  + to + ", ";
    sql += "'"  + ui->ln_act_num->text() + "', ";
    sql += "'"  + ui->date_act->date().toString("yyyy-MM-dd") + "') ";
    sql += " returning id ; ";

    QSqlQuery *query = new QSqlQuery(db);
    mySQL.exec(this, sql, "Добавление записи интервала ВС", *query, true, db, data_app);
    query->next();
    int id = query->value(0).toInt();

    // внесём в базу новые ВС
    QString sql_vs = "select * "
                     "  from gen_vs_interval"
                     "  ( " + QString::number(id) + ", "
                     "   '" + ui->date_interval->date().toString("yyyy-MM-dd") + "', "
                     "    " + QString::number(ui->spin_interval_from->value()) + ", "
                     "    " + QString::number(ui->spin_interval_to->value()) + ", "
                     "  ) ; ";

    QSqlQuery *query_vs = new QSqlQuery(db);
    mySQL.exec(this, sql_vs, "Добавление ВС из нового интервала", *query_vs, true, db, data_app);

    delete query_vs;
    delete query;

    refresh_intervals();
}

void pvp_n_vs_wnd::on_bn_edit_interval_clicked() {
    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_intervals->selectionModel()->selection().indexes();
    if (indexes.size()>0) {
        QModelIndex index = indexes.at(0);
        int id = model_intervals.data(model_intervals.index(index.row(), 0), Qt::EditRole).toInt();

        // Проверим заполнение полей
        //if (ui->date_interval->date()<QDate::currentDate().addMonths(-2) || ui->date_interval->date()>QDate::currentDate().addMonths(2)) {
        //    QMessageBox::warning(this, "Недопустимое значение!", "Дата выдачи интервала ВС для СМО не может отстоять от текущей более чем на 2 месяца.");
        //    return;
        //}
        if (ui->spin_interval_from->value()<1) {
            QMessageBox::warning(this, "Недопустимое значение!", "Начальное значение интервала ВС для СМО не может быть меньше 1.");
            return;
        }
        if (ui->spin_interval_to->value()<ui->spin_interval_from->value()+5) {
            QMessageBox::warning(this, "Недопустимое значение!", "Конечное значение интервала ВС для СМО не может быть меньше чем начальное значение +5.");
            return;
        }
        // проверка на пересечение с уже заданными интервалами
        QString fr = QString::number(ui->spin_interval_from->value());
        QString to = QString::number(ui->spin_interval_to->value());
        QString sql_test = "select id, vs_from, vs_to from vs_intervals where ("
                                 " (vs_from<="+fr+" and vs_to>="+fr+") or "
                                 " (vs_from<="+to+" and vs_to>="+to+") or "
                                 " (vs_from>="+fr+" and vs_to<="+to+") or "
                                 " (vs_from<="+fr+" and vs_to>="+to+") ";
        sql_test += ") and id<>" + QString::number(id) + " ; ";
        QSqlQuery *query_test = new QSqlQuery(db);
        mySQL.exec(this, sql_test, "Проверка интервала ВС для СМО на непересечение с другими", *query_test, true, db, data_app);
        if (query_test->size()>0) {
            QString res = "Данный интервал ВС пересекается с другим, ранее введённым:\n\n";
            while (query_test->next()) {
                res += "id=" + QString::number(query_test->value(0).toInt());
                res += "   -   от " + QString::number(query_test->value(1).toInt()) + " до " + QString::number(query_test->value(2).toInt()) + "\n";
            }
            res += "\nОперация отменена.";
            QMessageBox::warning(this, "Пересечение интервалов ВС для СМО", res);
            return;
        }

        // изменим текущую запись
        QString sql = "update vs_intervals set";
        sql += " date_get='" + ui->date_interval->date().toString("yyyy-MM-dd") + "', ";
        sql += " vs_from=" + QString::number(ui->spin_interval_from->value()) + ", ";
        sql += " vs_count=" + QString::number(ui->spin_interval_count->value()) + ", ";
        sql += " vs_to=" + QString::number(ui->spin_interval_to->value()) + ", ";
        sql += " act_num='" + ui->ln_act_num->text() + "', ";
        sql += " act_date='" + ui->date_act->date().toString("yyyy-MM-dd") + "' ";
        sql += " where id=" + QString::number(id);
        sql += " ; ";

        QSqlQuery *query = new QSqlQuery(db);
        mySQL.exec(this, sql, "Обновление записи оператора", *query, true, db, data_app);
        delete query;
        refresh_intervals();
    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано.");
    }
}

void pvp_n_vs_wnd::on_bn_delete_interval_clicked() {
    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_intervals->selectionModel()->selection().indexes();
    if (indexes.size()>0) {
        if (QMessageBox::warning(this, "Нужно подтверждение!", "Вы действительно хотите удалить эту запись?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::No) {
            return;
        }
        QModelIndex index = indexes.at(0);
        int id = model_intervals.data(model_intervals.index(index.row(), 0), Qt::EditRole).toInt();

        // изменим текущую запись
        QString sql = "delete from vs_intervals ";
        sql += " where id=" + QString::number(id);
        sql += " ; ";

        QSqlQuery *query2 = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Удаление записи интервала ВС для СМО", *query2, true, db, data_app)) {
            delete query2;
            return;
        }
        if (query2->size()>0) {
            query2->next();
            data_pers.id = query2->value(0).toInt();
        }
        delete query2;
        refresh_intervals();
    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано.");
    }
}



// --------------------------------------------------------------
// интервалы ВС для конкретного ПВП
// --------------------------------------------------------------
void pvp_n_vs_wnd::refresh_intervals_pt(int id_point, int id_int) {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    // данные персоны
    ui->date_interval_pt->setDate(QDate::currentDate());
    ui->spin_interval_pt_from->setValue(-1);
    ui->spin_interval_pt_to->setValue(-1);
    ui->spin_interval_pt_count->setValue(-1);

    QString intervals_sql = "select p.id as id_point, p.point_name, i0.id as id_int, i.id, i.date_open, i.vs_from, i.vs_to, i.vs_count, i.date_close, f.id as id_folder, f.folder_name as folder "
                            " from vs_intervals_pt i "
                            "      left join points p on(p.id=i.id_point) "
                            "      left join vs_intervals i0 on(i0.id=i.id_interval) "
                            "      left join folders f on(f.id=i.id_folder) "
                            " where i0.id=" + QString::number(id_int) + " "
                            "   and p.id=" + QString::number(id_point) + " "
                            " order by i.vs_from ; ";

    if (ui->ch_intervals_pt_all->isChecked()) {
        intervals_sql =     "select p.id as id_point, p.point_name, i0.id as id_int, i.id, i.date_open, i.vs_from, i.vs_to, i.vs_count, i.date_close, f.id as id_folder, f.folder_name as folder "
                            " from vs_intervals_pt i "
                            "      left join points p on(p.id=i.id_point) "
                            "      left join vs_intervals i0 on(i0.id=i.id_interval) "
                            "      left join folders f on(f.id=i.id_folder) "
                            " where p.id=" + QString::number(id_point) + " "
                            " order by i.vs_from ; ";
    }

    model_intervals_pt.setQuery(intervals_sql,db);
    QString err = model_intervals_pt.lastError().driverText();

    // подключаем модель из БД
    ui->tab_intervals_pt->setModel(&model_intervals_pt);

    // обновляем таблицу
    ui->tab_intervals_pt->reset();

    // задаём ширину колонок
    ui->tab_intervals_pt->setColumnWidth( 0,  1);    // p.id as id_point,
    ui->tab_intervals_pt->setColumnWidth( 1,100);    // p.point_name,
    ui->tab_intervals_pt->setColumnWidth( 2,  1);    // i0.id as id_int,
    ui->tab_intervals_pt->setColumnWidth( 3, 30);    // i.id,
    ui->tab_intervals_pt->setColumnWidth( 4, 70);    // i.date_open,
    ui->tab_intervals_pt->setColumnWidth( 5, 75);    // i.vs_from,
    ui->tab_intervals_pt->setColumnWidth( 6, 75);    // i.vs_to,
    ui->tab_intervals_pt->setColumnWidth( 7, 50);    // i.count,
    ui->tab_intervals_pt->setColumnWidth( 8, 70);    // i.date_close
    ui->tab_intervals_pt->setColumnWidth( 9,  1);    // f.id
    ui->tab_intervals_pt->setColumnWidth(10,120);    // i.folder

    // правим заголовки
    model_intervals_pt.setHeaderData( 0, Qt::Horizontal, ("id \nПВП"));
    model_intervals_pt.setHeaderData( 1, Qt::Horizontal, ("название \nПВП"));
    model_intervals_pt.setHeaderData( 2, Qt::Horizontal, ("id \nинт."));
    model_intervals_pt.setHeaderData( 4, Qt::Horizontal, ("дата \nоткрытия"));
    model_intervals_pt.setHeaderData( 5, Qt::Horizontal, ("№ ВС от"));
    model_intervals_pt.setHeaderData( 6, Qt::Horizontal, ("№ ВС до"));
    model_intervals_pt.setHeaderData( 7, Qt::Horizontal, ("число \nВС"));
    model_intervals_pt.setHeaderData( 8, Qt::Horizontal, ("дата \nзакрытия"));
    model_intervals_pt.setHeaderData(10, Qt::Horizontal, ("папка"));
    ui->tab_intervals_pt->repaint();

    this->setCursor(Qt::ArrowCursor);

    ui->date_act_pt->setDate(QDate::currentDate());
    on_date_act_pt_editingFinished();
}

void pvp_n_vs_wnd::on_date_interval_pt_close_dateChanged(const QDate &date) {
    if (date<=ui->date_interval_pt->date()) {
        ui->date_interval_pt_close->setDate(ui->date_interval_pt->date().addDays(1));
    }
}

void pvp_n_vs_wnd::on_ch_interval_pt_close_toggled(bool checked) {
    ui->date_interval_pt_close->setVisible(ui->ch_interval_pt_close->isChecked());
    if (ui->date_interval_pt_close->isVisible() && ui->date_interval_pt_close->date()<QDate::currentDate().addMonths(-1)) {
        ui->date_interval_pt_close->setDate(QDate::currentDate());
    }
}

void pvp_n_vs_wnd::on_spin_interval_pt_from_valueChanged(int arg1) {
    ui->spin_interval_pt_count->setValue( ui->spin_interval_pt_to->value() - ui->spin_interval_pt_from->value() +1);
}

void pvp_n_vs_wnd::on_spin_interval_pt_to_valueChanged(int arg1) {
    ui->spin_interval_pt_count->setValue( ui->spin_interval_pt_to->value() - ui->spin_interval_pt_from->value() +1);
}

void pvp_n_vs_wnd::on_bn_refresh_intervals_pt_clicked() {
    // проверим выделенную строку интервала
    QModelIndexList indexes_points = ui->tab_points->selectionModel()->selection().indexes();

    if (indexes_points.size()>0) {
        QModelIndex index_point = indexes_points.front();
        int id_point = model_points.data(model_points.index(index_point.row(), 2), Qt::EditRole).toInt();

        // проверим выделенную строку интервала
        QModelIndexList indexes_intervals = ui->tab_intervals->selectionModel()->selection().indexes();

        if (indexes_intervals.size()>0) {
            QModelIndex index_interval = indexes_intervals.front();
            int id_interval = model_intervals.data(model_intervals.index(index_interval.row(), 0), Qt::EditRole).toInt();
            refresh_intervals_pt(id_point, id_interval);
        } else {
            refresh_intervals_pt(id_point, -1);
        }
    } else {
        refresh_intervals_pt(-1, -1);
    }
}

void pvp_n_vs_wnd::on_ch_intervals_pt_all_clicked() {
    ui->bns_intervals_pt->setVisible(!(ui->ch_intervals_pt_all->isChecked()));
}

void pvp_n_vs_wnd::on_tab_intervals_pt_clicked(const QModelIndex &index) {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_points || !ui->tab_points->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите интервал из списка.");
        return;
    }
    this->setCursor(Qt::WaitCursor);

    ui->date_interval->setDate(model_intervals_pt.data(model_intervals_pt.index(index.row(), 4), Qt::EditRole).toDate());
    ui->spin_interval_pt_from->setValue(model_intervals_pt.data(model_intervals_pt.index(index.row(), 5), Qt::EditRole).toInt());
    ui->spin_interval_pt_to->setValue(model_intervals_pt.data(model_intervals_pt.index(index.row(), 6), Qt::EditRole).toInt());
    ui->spin_interval_pt_count->setValue(ui->spin_interval_pt_to->value() - ui->spin_interval_pt_from->value() +1);
    ui->date_interval_pt_close->setDate(model_intervals_pt.data(model_intervals_pt.index(index.row(), 8), Qt::EditRole).toDate());
    ui->ch_interval_pt_close->setChecked(!(model_intervals_pt.data(model_intervals_pt.index(index.row(), 8), Qt::EditRole).isNull()));
    ui->date_interval_pt_close->setVisible(!(model_intervals_pt.data(model_intervals_pt.index(index.row(), 8), Qt::EditRole).isNull()));
    bool has_folder = !(model_intervals_pt.data(model_intervals_pt.index(index.row(), 9), Qt::EditRole).isNull());
    ui->ch_interval_pt_folder->setChecked(has_folder);
    ui->combo_folder->setEnabled(has_folder);
    int n = ui->combo_folder->findData(model_intervals_pt.data(model_intervals_pt.index(index.row(), 9), Qt::EditRole).toInt());
    ui->combo_folder->setCurrentIndex(n);

    this->setCursor(Qt::ArrowCursor);
}

void pvp_n_vs_wnd::on_bn_add_interval_pt_clicked() {
    // проверка введённых данных
    if (ui->ch_interval_pt_close->isChecked()) {
        if (QMessageBox::question(this, "Нужно поджтверждение",
                                  "Вы действительно хотите задать дату закрытия интервала?\n\nВ этом случае при наступлении указанной даты номера ВС из интервала станут недоступны для использования даже если еще не расспределены на выданных бланках ВС!",
                                  QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel)==QMessageBox::Cancel) {
            // операция отменена
            return;
        }
    }

    // проверим выделенный интервал ВС для СМО
    QModelIndexList indexes_int = ui->tab_intervals->selectionModel()->selection().indexes();
    if (indexes_int.size()>0) {
        QModelIndex index_int = indexes_int.at(0);
        int id_interval = model_intervals.data(model_intervals.index(index_int.row(), 0), Qt::EditRole).toInt();

        // проверим выделенный ПВП
        QModelIndexList indexes_pnt = ui->tab_points->selectionModel()->selection().indexes();
        if (indexes_pnt.size()>0) {
            QModelIndex index_pnt = indexes_pnt.at(0);
            int id_point = model_points.data(model_points.index(index_pnt.row(), 2), Qt::EditRole).toInt();

            // Проверим заполнение полей
            /*if (ui->date_interval_pt->date()<QDate::currentDate().addMonths(-1) || ui->date_interval->date()>QDate::currentDate().addMonths(1)) {
                QMessageBox::warning(this, "Недопустимое значение!", "Дата выдачи интервала ВС для ПВП не может отстоять от текущей более чем на 1 месяц.");
                return;
            }*/
            if (ui->spin_interval_pt_from->value()<11) {
                QMessageBox::warning(this, "Недопустимое значение!", "Начальное значение интервала ВС не может быть меньше 11 (серия 1 и номер 1).");
                return;
            }
            if (ui->spin_interval_pt_to->value()<ui->spin_interval_pt_from->value()+1) {
                QMessageBox::warning(this, "Недопустимое значение!", "Конечное значение интервала ВС для ПВП не может быть меньше чем начальное значение +1.");
                return;
            }
            // проверка на пересечение с уже заданными интервалами
            QString fr = QString::number(ui->spin_interval_pt_from->value());
            QString to = QString::number(ui->spin_interval_pt_to->value());
            QString sql_test = "select id, vs_from, vs_to from vs_intervals_pt where ("
                                     " (vs_from<="+fr+" and vs_to>="+fr+") or "
                                     " (vs_from<="+to+" and vs_to>="+to+") or "
                                     " (vs_from>="+fr+" and vs_to<="+to+") or "
                                     " (vs_from<="+fr+" and vs_to>="+to+")) "
                                     " and id_point=" + QString::number(id_point) + " ";
            QSqlQuery *query_test = new QSqlQuery(db);
            mySQL.exec(this, sql_test, "Проверка интервала ВС для ПВП на непересечение с другими", *query_test, true, db, data_app);
            if (query_test->size()>0) {
                QString res = "Данный интервал пересекается с другим, ранее введённым:\n\n";
                while (query_test->next()) {
                    res += "id=" + QString::number(query_test->value(0).toInt());
                    res += "   -   от " + QString::number(query_test->value(1).toInt()) + " до " + QString::number(query_test->value(2).toInt()) + "\n";
                }
                res += "\nОперация отменена.";
                QMessageBox::warning(this, "Пересечение интервалов ВС для ПВП", res);
                delete query_test;
                return;
            }
            delete query_test;

            // проверка интервала ВС для ПВП на попададание в выбранный интервал для СМО
            QString sql_test2 = "select id from vs_intervals "
                                " where vs_from<=" +fr+ " and vs_to>=" +to+ " and id=" +QString::number(id_interval)+ " ; ";
            QSqlQuery *query_test2 = new QSqlQuery(db);
            mySQL.exec(this, sql_test2, "Проверка интервала ВС для ПВП на вхождение в старший интервал", *query_test2, true, db, data_app);
            if (query_test2->size()==0) {
                QString res = "Введённый интервал для ПВП не попадает в границы выбранного интервала для СМО.";
                res += "\n\nОперация отменена.";
                QMessageBox::warning(this, "Интервал для ПВП не входит в интервал для СМО!", res);
                delete query_test2;
                return;
            }
            delete query_test2;

            db.transaction();

            // собственно сохранение
            QString sql = "insert into vs_intervals_pt(id_interval, id_point, date_open, vs_from, vs_count, vs_to, date_close, id_folder) "
                          " values ("
                                   " " + QString::number(id_interval)                         + ", "
                                   " " + QString::number(id_point)                            + ", "
                                   "'" + ui->date_interval_pt->date().toString("yyyy-MM-dd")  + "',"
                                   " " + QString::number(ui->spin_interval_pt_from->value())  + ", "
                                   " " + QString::number(ui->spin_interval_pt_count->value()) + ", "
                                   " " + QString::number(ui->spin_interval_pt_to->value())    + ", "
                                   " NULL, "
                                   " " + QString(ui->ch_interval_pt_folder->isChecked() && ui->combo_folder->currentIndex()>=0 ? QString::number(ui->combo_folder->currentData().toInt()) : "NULL") + ") "
                                   " returning id ; ";

            QSqlQuery *query2 = new QSqlQuery(db);
            mySQL.exec(this, sql, "Добавление записи оператора", *query2, true, db, data_app);
            query2->next();
            int id_intr = query2->value(0).toInt();

            // добавление акта и отметка ВС в акте
            QString sql_vs = "select * "
                             "  from gen_vs_interval_pt "
                             "   ( NULL, "
                             "     " + QString::number(id_point)                            + ", "
                             "    '" + ui->date_interval_pt->date().toString("yyyy-MM-dd")  + "',"
                             "    '" + ui->ln_act_num_pt->text()                            + "', "
                             "    '" + ui->date_interval_pt->date().toString("yyyy-MM-dd")  + "',"
                             "     " + QString::number(ui->combo_act_obtainer->currentData().toInt()) + ", "
                             "     " + QString::number(id_intr)                             + ", "
                             "    '" + ui->date_interval_pt->date().toString("yyyy-MM-dd")  + "', "
                             "     " + QString::number(ui->spin_interval_pt_from->value())  + ", "
                             "     " + QString::number(ui->spin_interval_pt_to->value())    + ") "
                             " /*returning id ;*/ ";

            QSqlQuery *query_vs = new QSqlQuery(db);
            if (!mySQL.exec(this, sql_vs, "Добавление акта и отметка ВС а акте", *query_vs, true, db, data_app)) {
                db.rollback();
                QMessageBox::warning(this, "Ошибка добавления интервала для ПВП",
                                     "При добавлении нового интервал полисов для ПВ произошла непредвиденная ошибка.\n"
                                     "Операция отменена.");
                return;
            };
            query_vs->next();
            int cnt = query_vs->value(0).toInt();

            if (cnt!=ui->spin_interval_pt_count->value()) {
                if (QMessageBox::warning(this, "Некоторые бланки уже были заняты?",
                                         "Число бланков ВС передаваемых на ПВП не соответствует арифметической разнице между первым и последним номером интервала.\n"
                                         "Некоторые бланки уже переданы в другие ПВП или не все бланки из интервала получены СМО?\n"
                                         "\n  -  ВС в интервале: " + QString::number(ui->spin_interval_pt_count->value()) +
                                         "\n  -  ВС добавлено: " + QString::number(cnt) +
                                         "\n\nВсё рано сохранить изменения?",
                                         QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel)==QMessageBox::Cancel) {
                    db.rollback();
                    QMessageBox::information(this, "Действие отменено","Действие отменено.");
                    return;
                }
            }
            QMessageBox::information(this, "Бланки переданы",
                                     "Бланки переданы на ПВП. \nАкт сформирован.");
            delete query_vs;
            delete query2;

            db.commit();

            refresh_intervals_pt(id_point,id_interval);
        } else {
            QMessageBox::warning(this, "Не выбран пункт выдачи полисов", "Сначала выберите пункт выдачи полисов, в котором будет работать новый оператор.");
        }
    } else {
        QMessageBox::warning(this, "Не выбран интервал полисов", "Сначала выберите интервал полисов, из которого выбирается интервал ВС для ПВП.");
    }
}

void pvp_n_vs_wnd::on_bn_edit_interval_pt_clicked() {
    // проверка введённых данных
    if (ui->ch_interval_pt_close->isChecked()) {
        if (QMessageBox::question(this, "Нужно поджтверждение",
                                  "Вы действительно хотите задать дату закрытия интервала?\n\nВ этом случае при наступлении указанной даты номера ВС из интервала станут недоступны для использования даже если еще не расспределены на выданных бланках ВС!",
                                  QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel)==QMessageBox::Cancel) {
            // операция отменена
            return;
        }
    }

    // проверим выделенный интервал ВС для СМО
    QModelIndexList indexes_int = ui->tab_intervals->selectionModel()->selection().indexes();
    if (indexes_int.size()>0) {
        QModelIndex index_int = indexes_int.at(0);
        int id_interval = model_intervals.data(model_intervals.index(index_int.row(), 0), Qt::EditRole).toInt();

        // проверим выделенный ПВП
        QModelIndexList indexes_pnt = ui->tab_points->selectionModel()->selection().indexes();
        if (indexes_pnt.size()>0) {
            QModelIndex index_pnt = indexes_pnt.at(0);
            int id_point = model_points.data(model_points.index(index_pnt.row(), 2), Qt::EditRole).toInt();

            // проверим выделенную строку
            QModelIndexList indexes = ui->tab_intervals_pt->selectionModel()->selection().indexes();
            if (indexes.size()>0) {
                QModelIndex index = indexes.at(0);
                int id = model_intervals_pt.data(model_intervals_pt.index(index.row(), 3), Qt::EditRole).toInt();

                // Проверим заполнение полей
                /*if (ui->date_interval_pt->date()<QDate::currentDate().addMonths(-1) || ui->date_interval->date()>QDate::currentDate().addMonths(1)) {
                    QMessageBox::warning(this, "Недопустимое значение!", "Дата выдачи интервала ВС для ПВП не может отстоять от текущей более чем на 1 месяц.");
                    return;
                }*/
                if (ui->spin_interval_pt_from->value()<1) {
                    QMessageBox::warning(this, "Недопустимое значение!", "Начальное значение интервала ВС не может быть меньше 11 (серия 1 и номер 1).");
                    return;
                }
                if (ui->spin_interval_pt_to->value()<ui->spin_interval_pt_from->value()+5) {
                    QMessageBox::warning(this, "Недопустимое значение!", "Конечное значение интервала ВС для ПВП не может быть меньше чем начальное значение +5.");
                    return;
                }
                // проверка на пересечение с уже заданными интервалами
                QString fr = QString::number(ui->spin_interval_pt_from->value());
                QString to = QString::number(ui->spin_interval_pt_to->value());
                QString sql_test = "select id, vs_from, vs_to from vs_intervals_pt where ("
                                         " (vs_from<="+fr+" and vs_to>="+fr+") or "
                                         " (vs_from<="+to+" and vs_to>="+to+") or "
                                         " (vs_from>="+fr+" and vs_to<="+to+") or "
                                         " (vs_from<="+fr+" and vs_to>="+to+")) "
                                         " and id_point=" + QString::number(id_point) + " "
                                         " and id<>" + QString::number(id) + " ";
                QSqlQuery *query_test = new QSqlQuery(db);
                mySQL.exec(this, sql_test, "Проверка интервала ВС для ПВП на непересечение с другими", *query_test, true, db, data_app);
                if (query_test->size()>0) {
                    QString res = "Данный интервал пересекается с другим, ранее введённым:\n\n";
                    while (query_test->next()) {
                        res += "id=" + QString::number(query_test->value(0).toInt());
                        res += "   -   от " + QString::number(query_test->value(1).toInt()) + " до " + QString::number(query_test->value(2).toInt()) + "\n";
                    }
                    res += "\nОперация отменена.";
                    QMessageBox::warning(this, "Пересечение интервалов ВС для ПВП", res);
                    delete query_test;
                    return;
                }
                delete query_test;

                // проверка интервала ВС для ПВП на попададание в выбранный интервал для СМО
                QString sql_test2 = "select id from vs_intervals "
                                    " where vs_from<=" +fr+ " and vs_to>=" +to+ " and id=" +QString::number(id_interval)+ " ; ";
                QSqlQuery *query_test2 = new QSqlQuery(db);
                mySQL.exec(this, sql_test2, "Проверка интервала ВС для ПВП на вхождение в старший интервал", *query_test2, true, db, data_app);
                if (query_test2->size()==0) {
                    QString res = "Введённый интервал для ПВП ("+fr+", "+to+") \nне попадает в границы выбранного интервала для СМО.";
                    res += "\n\nОперация отменена.";
                    QMessageBox::warning(this, "Интервал для ПВП не входит в интервал для СМО!", res);
                    delete query_test2;
                    return;
                }
                delete query_test2;


                // изменим текущую запись
                QString sql = "update vs_intervals_pt "
                              " set "
                              "     id_interval=" + QString::number(id_interval)                       + ", "
                              "     id_point=" + QString::number(id_point)                             + ", "
                              "     date_open='" + ui->date_interval_pt->date().toString("yyyy-MM-dd") + "',"
                              "     vs_from=" + QString::number(ui->spin_interval_pt_from->value())    + ", "
                              "     vs_count=" + QString::number(ui->spin_interval_pt_count->value())  + ", "
                              "     vs_to=" + QString::number(ui->spin_interval_pt_to->value())        + ", "
                              "     date_close=" + (ui->ch_interval_pt_close->isChecked() ? QString(" '" + ui->date_interval_pt_close->date().toString("yyyy-MM-dd") + "' ") : QString(" NULL ")) + ", "
                              "     id_folder=" + QString(ui->ch_interval_pt_folder->isChecked() && ui->combo_folder->currentIndex()>=0 ? QString::number(ui->combo_folder->currentData().toInt()) : "NULL") + " "
                              "     where id=" + QString::number(id) + " ; ";

                QSqlQuery *query = new QSqlQuery(db);
                mySQL.exec(this, sql, "Обновление записи оператора", *query, true, db, data_app);
                delete query;
            }
            refresh_intervals_pt(id_point,id_interval);
        } else {
            QMessageBox::warning(this, "Не выбран пункт выдачи полисов", "Сначала выберите пункт выдачи полисов, в котором будет работать новый оператор.");
        }
    } else {
        QMessageBox::warning(this, "Не выбран интервал полисов", "Сначала выберите интервал полисов, из которого выбирается интервал ВС для ПВП.");
    }
}

void pvp_n_vs_wnd::on_bn_delete_interval_pt_clicked() {
    // проверим выделенный интервал ВС для СМО
    QModelIndexList indexes_int = ui->tab_intervals->selectionModel()->selection().indexes();
    if (indexes_int.size()>0) {
        QModelIndex index_int = indexes_int.at(0);
        int id_interval = model_intervals.data(model_intervals.index(index_int.row(), 0), Qt::EditRole).toInt();

        // проверим выделенный ПВП
        QModelIndexList indexes_pnt = ui->tab_points->selectionModel()->selection().indexes();
        if (indexes_pnt.size()>0) {
            QModelIndex index_pnt = indexes_pnt.at(0);
            int id_point = model_points.data(model_points.index(index_pnt.row(), 2), Qt::EditRole).toInt();

            // проверим выделенную строку
            QModelIndexList indexes = ui->tab_intervals_pt->selectionModel()->selection().indexes();
            if (indexes.size()>0) {
                if (QMessageBox::warning(this, "Нужно подтверждение!", "Вы действительно хотите удалить эту запись?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::No) {
                    return;
                }
                QModelIndex index = indexes.at(0);
                int id = model_intervals_pt.data(model_intervals_pt.index(index.row(), 0), Qt::EditRole).toInt();

                // изменим текущую запись
                QString sql = "delete from vs_intervals_pt ";
                sql += " where id=" + QString::number(id);
                sql += " ; ";

                QSqlQuery *query2 = new QSqlQuery(db);
                if (!mySQL.exec(this, sql, "Удаление записи интервала ВС для ПВП", *query2, true, db, data_app)) {
                    delete query2;
                    return;
                }
                if (query2->size()>0) {
                    query2->next();
                    data_pers.id = query2->value(0).toInt();
                }
                delete query2;
            }
            refresh_intervals_pt(id_point,id_interval);
        } else {
            QMessageBox::warning(this, "Не выбран пункт выдачи полисов", "Сначала выберите пункт выдачи полисов, в котором будет работать новый оператор.");
        }
    } else {
        QMessageBox::warning(this, "Не выбран интервал полисов", "Сначала выберите интервал полисов, из которого выбирается интервал ВС для ПВП.");
    }
}



/*// --------------------------------------------------------------
// испорченные ВС
// --------------------------------------------------------------
void pvp_n_vs_wnd::refresh_spoiled_vs_pt(int id_point, int id_int) {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    // данные персоны
    ui->spin_spoiled_vs->setValue(-1);
    ui->date_spoiled_vs->setDate(QDate::currentDate());

    QString spoiled_vs_sql = "select p.id as id_point, p.point_name, i0.id as id_int, s.id, s.date_add, s.vs_num "
                             " from vs_spoiled_pt s left join vs_intervals_pt i on(i.id=s.id_interval_pt) left join points p on(p.id=i.id_point) left join vs_intervals i0 on(i0.id=i.id_interval) ";
    if (!(ui->ch_spoiled_vs_all_int->isChecked()) && !(ui->ch_spoiled_vs_all_pvp->isChecked())) {
        spoiled_vs_sql += " where i0.id=" + QString::number(id_int) + " and p.id=" + QString::number(id_point) + " ; ";
    } else if ((ui->ch_spoiled_vs_all_int->isChecked()) && !(ui->ch_spoiled_vs_all_pvp->isChecked()))
        spoiled_vs_sql += " where p.id=" + QString::number(id_point) + " ; ";
    else {
        spoiled_vs_sql += " where i0.id=" + QString::number(id_int) + " ; ";
    }

    model_spoiled_vs.setQuery(spoiled_vs_sql,db);
    QString err = model_spoiled_vs.lastError().driverText();

    // подключаем модель из БД
    ui->tab_spoiled_vs->setModel(&model_spoiled_vs);

    // обновляем таблицу
    ui->tab_spoiled_vs->reset();

    // задаём ширину колонок
    ui->tab_spoiled_vs->setColumnWidth( 0, 50);    // p.id as id_point,
    ui->tab_spoiled_vs->setColumnWidth( 1,150);    // p.point_name,
    ui->tab_spoiled_vs->setColumnWidth( 2, 50);    // i0.id as id_int,
    ui->tab_spoiled_vs->setColumnWidth( 3,  1);    // s.id,
    ui->tab_spoiled_vs->setColumnWidth( 4,100);    // s.date_add,
    ui->tab_spoiled_vs->setColumnWidth( 5,120);    // s.vs_num

    // правим заголовки
    model_spoiled_vs.setHeaderData( 0, Qt::Horizontal, ("id ПВП"));
    model_spoiled_vs.setHeaderData( 1, Qt::Horizontal, ("название ПВП"));
    model_spoiled_vs.setHeaderData( 2, Qt::Horizontal, ("id инт."));
    model_spoiled_vs.setHeaderData( 3, Qt::Horizontal, ("ID"));
    model_spoiled_vs.setHeaderData( 4, Qt::Horizontal, ("дата порчи"));
    model_spoiled_vs.setHeaderData( 5, Qt::Horizontal, ("№ ВС"));
    ui->tab_spoiled_vs->repaint();

    //refresh_assigs_tab();
    //refresh_links_tab();
    //refresh_addresses_tab();
    this->setCursor(Qt::ArrowCursor);
}

void pvp_n_vs_wnd::on_bn_refresh_spoiled_vs_clicked() {
    // проверим выделенную строку интервала
    QModelIndexList indexes_points = ui->tab_points->selectionModel()->selection().indexes();

    if (indexes_points.size()>0) {
        QModelIndex index_point = indexes_points.front();
        int id_point = model_points.data(model_points.index(index_point.row(), 2), Qt::EditRole).toInt();

        // проверим выделенную строку интервала
        QModelIndexList indexes_intervals = ui->tab_intervals->selectionModel()->selection().indexes();

        if (indexes_intervals.size()>0) {
            QModelIndex index_interval = indexes_intervals.front();
            int id_interval = model_intervals.data(model_intervals.index(index_interval.row(), 0), Qt::EditRole).toInt();
            refresh_spoiled_vs_pt(id_point, id_interval);
        } else {
            refresh_spoiled_vs_pt(id_point, -1);
        }
    } else {
        refresh_spoiled_vs_pt(-1, -1);
    }
}

void pvp_n_vs_wnd::on_ch_spoiled_vs_all_int_clicked() {
    ui->bns_spoiled_vs->setVisible(!(ui->ch_spoiled_vs_all_int->isChecked()) && !(ui->ch_spoiled_vs_all_pvp->isChecked()));
}

void pvp_n_vs_wnd::on_ch_spoiled_vs_all_pvp_clicked() {
    on_ch_spoiled_vs_all_int_clicked();
}

void pvp_n_vs_wnd::on_bn_add_spoiled_vs_clicked() {
    // проверка, попадает ли этот номер в какой-нибудь открытый интервал номеров данного ПВП
    QString sql;
    sql= "select v.id as id_vs_inteval_pt "
         " from  vs_intervals_pt v "
         " where v.id_point=" + QString::number(data_app.id_point) + " and "
         "       v.date_open<=CURRENT_DATE and "
         "       ( (v.date_close is NULL) or (v.date_close>CURRENT_DATE) ) and "
         "       " + QString::number(ui->spin_spoiled_vs->text().toInt()) + " between v.vs_from and v.vs_to ; ";
    QSqlQuery *query = new QSqlQuery(db);
    mySQL.exec(this, sql, QString("проверка номера ВС по открытым интервалам"), *query, true);
    if (query->size()<1) {
        QMessageBox::warning(this, "Неверный номер ВС!", "Введённый номер ВС не входит ни в один открытый для данного пункта выдачи интервал номеров ВС!\n\nПроверьте номер ВС и, при необходимости, повторите операцию.");
        delete query;
        return;
    }
    delete query;

    // проверим, нет ли такого номера в числе номеров уже выданных ВС
    sql = "select p.id "
          " from  polises p "
          " where p.pol_v=2 and "
          "       p.vs_num=" + QString::number(ui->spin_spoiled_vs->text().toInt()) + " ; ";
    query = new QSqlQuery(db);
    mySQL.exec(this, sql, QString("проверка номера ВС по открытым интервалам"), *query, true);
    if (query->size()>0) {
        QMessageBox::critical(this, "Номер ВС уже занят!", "Введённый номер ВС уже использован в действующем временном свидетельстве!\n\nПроверьте номер ВС и, при необходимости, повторите операцию.");
        delete query;
        return;
    }
    delete query;

    // проверим, нет ли такого номера в числе уже внесённых номеров испорченных ВС
    sql = "select p.id "
          " from  vs_spoiled_pt p "
          " where p.vs_num=" + QString::number(ui->spin_spoiled_vs->text().toInt()) + " ; ";
    query = new QSqlQuery(db);
    mySQL.exec(this, sql, QString("проверка номера ВС по открытым интервалам"), *query, true);
    if (query->size()>0) {
        QMessageBox::critical(this, "Номер уже внесён в таблицу испорченных ВС!", "Введённый номер ВС уже внесён в базу как номер испорченного бланка ВС!\n\nПроверьте номер ВС и, при необходимости, повторите операцию.");
        delete query;
        return;
    }
    delete query;

    // добавим номер испорченного ВС
    sql = "insert into  vs_spoiled_pt (id_interval_pt, date_add, vs_num) "
          " values(" + QString::number(ui->spin_spoiled_vs->text().toInt()) + " ; ";
    query = new QSqlQuery(db);
    mySQL.exec(this, sql, QString("проверка номера ВС по открытым интервалам"), *query, true);
    if (query->size()>0) {
        QMessageBox::critical(this, "Номер уже внесён в таблицу испорченных ВС!", "Введённый номер ВС уже внесён в базу как номер испорченного бланка ВС!\n\nПроверьте номер ВС и, при необходимости, повторите операцию.");
        delete query;
        return;
    }
    delete query;
}*/


void pvp_n_vs_wnd::refresh_folders() {
    this->setCursor(Qt::WaitCursor);
    // обновление списка папок
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select id, '(' || status || ')  ' || folder_name "
                  " from public.folders "
                  " order by status desc, folder_name asc ; ";
    mySQL.exec(this, sql, QString("Список папок"), *query, true, db, data_app);
    ui->combo_folder->clear();
    while (query->next()) {
        QString name = query->value(1).toString();
        int id = query->value(0).toInt();
        ui->combo_folder->addItem(name, id);
    }
    delete query;
    int n = 0;
    ui->combo_folder->setCurrentIndex(n);
    this->setCursor(Qt::ArrowCursor);
}

void pvp_n_vs_wnd::on_ch_interval_pt_folder_clicked(bool checked) {
    ui->combo_folder->setEnabled(checked);
}

void pvp_n_vs_wnd::on_date_act_pt_editingFinished() {

    // проверим выделенную строку пункта выдачи
    QModelIndexList indexes_point = ui->tab_points->selectionModel()->selection().indexes();

    QString point_regnum = "???";
    if (indexes_point.size()>0) {
        QModelIndex index_point = indexes_point.front();
        point_regnum = model_points.data(model_points.index(index_point.row(), 4), Qt::EditRole).toString();
        ui->ln_act_num_pt->setText(point_regnum + " - " + ui->date_act_pt->date().toString("yyyy-MM-dd"));
    }
}
