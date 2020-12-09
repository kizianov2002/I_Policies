#include "firms_wnd.h"
#include "ui_firms_wnd.h"

firms_wnd::firms_wnd(QSqlDatabase &db, s_data_app &data_app, s_data_firm &data_firm, QWidget *parent)
   : db(db), data_app(data_app), data_firm(data_firm), QDialog(parent), ui(new Ui::firms_wnd)
{
    ui->setupUi(this);

    refresh_firms_tab();
}

firms_wnd::~firms_wnd() {
    delete ui;
}

void firms_wnd::on_bn_close_clicked() {
    close();
}

void firms_wnd::refresh_firms_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    QString persons_sql = "select id, firm_name, firm_address, firm_director, firm_phones "
                          " from firms ";
    persons_sql += " order by firm_name ; ";

    model_firms.setQuery(persons_sql,db);
    QString err2 = model_firms.lastError().driverText();

    // подключаем модель из БД
    ui->tab_firms->setModel(&model_firms);

    // обновляем таблицу
    ui->tab_firms->reset();

    // задаём ширину колонок
    ui->tab_firms->setColumnWidth( 0,  1);     // id,
    ui->tab_firms->setColumnWidth( 1,200);     // ,
    ui->tab_firms->setColumnWidth( 2,300);     // ,
    ui->tab_firms->setColumnWidth( 3,150);     // ,
    ui->tab_firms->setColumnWidth( 4,200);     // ,

    // правим заголовки
    model_firms.setHeaderData( 1, Qt::Horizontal, ("органитзация"));
    model_firms.setHeaderData( 2, Qt::Horizontal, ("адрес"));
    model_firms.setHeaderData( 3, Qt::Horizontal, ("рукрводиткль"));
    model_firms.setHeaderData( 4, Qt::Horizontal, ("контактная информация"));
    ui->tab_firms->repaint();

    ui->ln_name->setText("");
    ui->ln_address->setText("");
    ui->ln_director->setText("");
    ui->ln_phones->setText("");

    ui->bn_ok->setEnabled(true);

    this->setCursor(Qt::ArrowCursor);
}

void firms_wnd::on_bn_refresh_clicked() {
    refresh_firms_tab();
}

void firms_wnd::on_bn_add_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_firms || !ui->tab_firms->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Поиск] и выберите человека из списка.");
        return;
    }

    // проверка, введено ли название организации
    if (ui->ln_name->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Название организации не может быть пустым.\n\nСохранение отменено.");
        return;
    }

    // проверим название на уникальность
    QString sql = "select count(*) from firms "
                  " where upper(firm_name)='" + ui->ln_name->text().trimmed().simplified().toUpper() + "' ; ";

    QSqlQuery *query = new QSqlQuery(db);
    if (!mySQL.exec(this, sql, "Проверка названия фирмы на уникальность", *query, true, db, data_app) || !query->next()) {
        QMessageBox::warning(this, "Непредвиденная ошибка", "При попытке проверить название фирмы на уникальность произошла неожиданная ошибка.\n\nСохранение отменено.");
        delete query;
        return;
    }
    if (query->value(0).toInt()>0) {
        QMessageBox::warning(this,
                              "Такое название организайии уже есть",
                              "В базе данных уже есть организация с таким названием.\nИзмените название, иначе в процессе ввода данных персоны будет невозможно отличить разные записи друг от друга.\n\nСохранение отменено.");
        delete query;
        return;
    }


    if (QMessageBox::question(this,
                              "Нужно подтверждение",
                              "Добавить новую организацию, в которой работают наши застрахованные?\n",
                              QMessageBox::Yes|QMessageBox::Cancel,
                              QMessageBox::Yes)==QMessageBox::Yes) {
        db.transaction();
        // добавим фирму
        QString sql = "insert into firms (firm_name, firm_director, firm_address, firm_phones) "
                      " values ("
                      "   '" + ui->ln_name->text().trimmed().simplified() + "', "
                      "   '" + ui->ln_director->text().trimmed().simplified() + "', "
                      "   '" + ui->ln_address->text().trimmed().simplified() + "', "
                      "   '" + ui->ln_phones->text().trimmed().simplified() + "') "
                      " returning id ; ";

        QSqlQuery *query = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Добавление новой фирмы", *query, true, db, data_app) || !query->next()) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке добавить новую фирму произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query;
            return;
        }
        data_firm.id = query->value(0).toInt();
        data_firm.name = ui->ln_name->text();
        delete query;
        db.commit();

        refresh_firms_tab();
    }
}

void firms_wnd::on_bn_ok_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_firms || !ui->tab_firms->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Поиск] и выберите человека из списка.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_firms->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        QModelIndex index = indexes.front();

        data_firm.id = model_firms.data(model_firms.index(index.row(), 0), Qt::EditRole).toInt();
        data_firm.name = model_firms.data(model_firms.index(index.row(), 1), Qt::EditRole).toString();

        accept();
    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void firms_wnd::on_tab_firms_clicked(const QModelIndex &index) {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_firms || !ui->tab_firms->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Поиск] и выберите человека из списка.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_firms->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        QModelIndex index = indexes.front();

        data_firm.id = model_firms.data(model_firms.index(index.row(), 0), Qt::EditRole).toInt();
        data_firm.name = model_firms.data(model_firms.index(index.row(), 1), Qt::EditRole).toString();

        ui->ln_name->setText(model_firms.data(model_firms.index(index.row(), 1), Qt::EditRole).toString());
        ui->ln_address->setText(model_firms.data(model_firms.index(index.row(), 2), Qt::EditRole).toString());
        ui->ln_director->setText(model_firms.data(model_firms.index(index.row(), 3), Qt::EditRole).toString());
        ui->ln_phones->setText(model_firms.data(model_firms.index(index.row(), 4), Qt::EditRole).toString());

    } else {
        ui->ln_name->setText("");
        ui->ln_address->setText("");
        ui->ln_director->setText("");
        ui->ln_phones->setText("");
    }
}



void firms_wnd::on_bn_edit_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_firms || !ui->tab_firms->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Поиск] и выберите человека из списка.");
        return;
    }

    // проверка, введено ли название организации
    if (ui->ln_name->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Название организации не может быть пустым.\n\nСохранение отменено.");
        return;
    }

    // проверим название на уникальность
    QString sql = "select count(*) from firms "
                  " where upper(firm_name)='" + ui->ln_name->text().trimmed().simplified().toUpper() + "' and "
                  "       id<>" + QString::number(data_firm.id) + " ; ";

    QSqlQuery *query = new QSqlQuery(db);
    if (!mySQL.exec(this, sql, "Проверка названия фирмы на уникальность", *query, true, db, data_app) || !query->next()) {
        QMessageBox::warning(this, "Непредвиденная ошибка", "При попытке проверить название фирмы на уникальность произошла неожиданная ошибка.\n\nСохранение отменено.");
        delete query;
        return;
    }
    delete query;

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_firms->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Изменить данные выбранной фирмы?",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        QModelIndex index = indexes.front();
        db.transaction();
        // добавим фирму
        QString sql_upd = "update firms "
                          " set "
                          "   firm_name='" + ui->ln_name->text().trimmed().simplified() + "', "
                          "   firm_director='" + ui->ln_director->text().trimmed().simplified() + "', "
                          "   firm_address='" + ui->ln_address->text().trimmed().simplified() + "', "
                          "   firm_phones='" + ui->ln_phones->text().trimmed().simplified() + "' "
                          " where id=" + QString::number(data_firm.id) + " ; ";

        QSqlQuery *query_upd = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_upd, "Обновление фирмы", *query_upd, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке изменить данные фирмы произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query_upd;
            return;
        }
        data_firm.name = ui->ln_name->text();
        delete query_upd;
        db.commit();

        refresh_firms_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
    ui->bn_ok->setEnabled(true);
}

void firms_wnd::on_bn_delete_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_firms || !ui->tab_firms->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Поиск] и выберите человека из списка.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_firms->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Удалить выбранную запись из базы?\n(Все лица, приписанные к удаляемой фирме будут от неё откреплены)\n",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        QModelIndex index = indexes.front();
        db.transaction();
        // добавим фирму
        QString sql = "update persons "
                      " set "
                      "   id_firm=NULL "
                      " where id_firm=" + QString::number(data_firm.id) + " ; ";
        QSqlQuery *query = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Обновление фирмы", *query, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке изменить данные фирмы произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query;
            return;
        }
        delete query;

        // добавим фирму
        sql = "delete from firms "
              " where id=" + QString::number(data_firm.id) + " ; ";
        QSqlQuery *query2 = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Обновление фирмы", *query2, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке изменить данные фирмы произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query2;
            return;
        }
        data_firm.id = -1;
        data_firm.name = "";
        delete query2;

        db.commit();

        refresh_firms_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void firms_wnd::on_ln_name_editingFinished() {
    ui->bn_ok->setEnabled(false);
}

void firms_wnd::on_ln_address_editingFinished() {
    ui->bn_ok->setEnabled(false);
}

void firms_wnd::on_ln_director_editingFinished() {
    ui->bn_ok->setEnabled(false);
}

void firms_wnd::on_ln_phones_editingFinished() {
    ui->bn_ok->setEnabled(false);
}
