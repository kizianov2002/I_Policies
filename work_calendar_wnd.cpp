#include "work_calendar_wnd.h"
#include "ui_work_calendar_wnd.h"

work_calendar_wnd::work_calendar_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent)
    : db(db), data_app(data_app), settings(settings), QDialog(parent), ui(new Ui::work_calendar_wnd)
{
    ui->setupUi(this);
    ui->spin_year->setValue(QDate::currentDate().year());
    refresh_tab();
    ui->date_date->setDate(QDate::currentDate());
    ui->combo_status->setCurrentIndex(0);
}

work_calendar_wnd::~work_calendar_wnd() {
    delete ui;
}

void work_calendar_wnd::on_bn_close_clicked() {
    close();
}

void work_calendar_wnd::on_bn_refresh_clicked() {
    refresh_tab();
}

void work_calendar_wnd::on_spin_year_editingFinished() {
    int y = ui->spin_year->value();
    if (y<20) {
        ui->spin_year->setValue(2000 + y);
    } else if (y<100) {
        ui->spin_year->setValue(1900 + y);
    }
    refresh_tab();
}

void work_calendar_wnd::refresh_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    // данные персоны

    QString sql =
            "select id, date, status "
            " from work_calendar w "
            " where w.date>='" + QString::number(ui->spin_year->value()) + "-01-01'  and w.date<'" + QString::number(ui->spin_year->value()+1) + "-01-01' ";

    // сортировка
    sql += " order by date ; ";

    model_work_calendar.setQuery(sql,db);
    QString err = model_work_calendar.lastError().driverText();

    // подключаем модель из БД
    ui->tableView->setModel(&model_work_calendar);

    // обновляем таблицу
    ui->tableView->reset();

    // задаём ширину колонок
    ui->tableView->setColumnWidth( 0, 30);    // id
    ui->tableView->setColumnWidth( 1, 80);    // date
    ui->tableView->setColumnWidth( 2,120);    // status

    // правим заголовки
    model_work_calendar.setHeaderData( 0, Qt::Horizontal, ("ID"));
    model_work_calendar.setHeaderData( 1, Qt::Horizontal, ("день"));
    model_work_calendar.setHeaderData( 2, Qt::Horizontal, ("статус"));

    this->setCursor(Qt::ArrowCursor);
}

void work_calendar_wnd::on_tableView_clicked(const QModelIndex &index) {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tableView || !ui->tableView->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\nНажмите кнопку [Обновить].");
        return;
    }
    this->setCursor(Qt::WaitCursor);

    ui->date_date->setDate (model_work_calendar.data(model_work_calendar.index(index.row(), 1), Qt::EditRole).toDate());
    ui->combo_status->setCurrentIndex (1 - (model_work_calendar.data(model_work_calendar.index(index.row(), 2), Qt::EditRole).toInt()));

    this->setCursor(Qt::ArrowCursor);
}

void work_calendar_wnd::on_bn_add_day_clicked() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    // проверим, не занят ли день
    QString sql_tst =
            "select count(*) from work_calendar "
            " where date='" + ui->date_date->date().toString("yyyy-MM-dd") + "' ; ";

    QSqlQuery *query_tst = new QSqlQuery(db);
    bool res_tst = mySQL.exec(this, sql_tst, QString("проверим день рабочего календаря"), *query_tst, true, db, data_app);
    if (!res_tst) {
        delete query_tst;
        this->setCursor(Qt::ArrowCursor);
        QMessageBox::warning(this, "Ошибка при проверке занятости выходного дня", "При проверке занятости выходного дня произошла неожиданная ошибка");
        return;
    }
    if(query_tst->next() && query_tst->value(0).toInt()>0) {
        delete query_tst;
        this->setCursor(Qt::ArrowCursor);
        QMessageBox::warning(this, "Этот день уже добавлен", "Этот день уже добавлен.\n\nИсправьте дату и, при необходимости, повторите операцию.\n");
        return;
    }
    // добавим день
    QString sql =
            "insert into work_calendar(date, status) "
            " values('" + ui->date_date->date().toString("yyyy-MM-dd") + "', " + QString::number(ui->combo_status->currentIndex() -1) + ") ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString("добавим день в рабочий календарь"), *query, true, db, data_app);
    if (!res) {
        delete query;
        this->setCursor(Qt::ArrowCursor);
        QMessageBox::warning(this, "Ошибка при добавлении выходного дня", "При добавлении выходного дня произошла неожиданная ошибка");
        return;
    }
    refresh_tab();
    this->setCursor(Qt::ArrowCursor);
}

void work_calendar_wnd::on_bn_delete_day_clicked() {
    QModelIndexList indexes = ui->tableView->selectionModel()->selection().indexes();
    if (indexes.size()>0) {
        if (QMessageBox::warning(this, "Нужно подтверждение!", "Вы действительно хотите удалить эту запись?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::No) {
            return;
        }
        QModelIndex index = indexes.at(0);
        int id = model_work_calendar.data(model_work_calendar.index(index.row(), 0), Qt::EditRole).toInt();

        // удалим текущую запись
        QString sql =
                "delete from work_calendar "
                " where id=" + QString::number(id) + " ; ";

        QSqlQuery *query2 = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Удаление выходного дня", *query2, true, db, data_app)) {
            delete query2;
            return;
        }
        delete query2;
        refresh_tab();
    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано.");
    }
}

void work_calendar_wnd::on_spin_year_valueChanged(int arg1) {
    refresh_tab();
}
