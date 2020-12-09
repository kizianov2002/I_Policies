#include "list_persons_terrs_wnd.h"
#include "ui_list_persons_terrs_wnd.h"

list_persons_terrs_wnd::list_persons_terrs_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent)
   : db(db), data_app(data_app), settings(settings), QDialog(parent), ui(new Ui::list_persons_terrs_wnd)
{
    ui->setupUi(this);

    refresh_terrs_tab();
}

list_persons_terrs_wnd::~list_persons_terrs_wnd() {
    delete ui;
}

void list_persons_terrs_wnd::on_bn_close_clicked() {
    close();
}

void list_persons_terrs_wnd::refresh_terrs_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    QString terrs_sql;
    terrs_sql += "select id, ocato, name, kladr, id_up "
                 " from address_struct "
                 " order by id ; ";

    model_terrs.setQuery(terrs_sql,db);
    QString err2 = model_terrs.lastError().driverText();

    // подключаем модель из БД
    ui->tab_terrs->setModel(&model_terrs);

    // обновляем таблицу
    ui->tab_terrs->reset();

    // задаём ширину колонок
    ui->tab_terrs->setColumnWidth( 0, 50);     // id,
    ui->tab_terrs->setColumnWidth( 1, 80);     // ,
    ui->tab_terrs->setColumnWidth( 2,200);     // ,
    ui->tab_terrs->setColumnWidth( 3,100);     // ,
    ui->tab_terrs->setColumnWidth( 4, 80);     // ,

    // правим заголовки
    model_terrs.setHeaderData( 0, Qt::Horizontal, ("ID"));
    model_terrs.setHeaderData( 1, Qt::Horizontal, ("код ОКАТО"));
    model_terrs.setHeaderData( 2, Qt::Horizontal, ("название территории"));
    model_terrs.setHeaderData( 3, Qt::Horizontal, ("код КЛАДР"));
    model_terrs.setHeaderData( 4, Qt::Horizontal, ("ID старш."));
    ui->tab_terrs->repaint();

    ui->ln_okato->setText("");
    ui->ln_kladr->setText("");
    ui->ln_terr_name->setText("");
    ui->spin_up->setValue(0);

    this->setCursor(Qt::ArrowCursor);
}

void list_persons_terrs_wnd::on_bn_refresh_clicked() {
    refresh_terrs_tab();
}

void list_persons_terrs_wnd::on_bn_add_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_terrs || !ui->tab_terrs->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить].");
        return;
    }

    // проверка, введено ли название
    if (ui->ln_terr_name->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Название территории не может быть пустым.\n\nСохранение отменено.");
        return;
    }
    if (ui->ln_okato->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Код ОКАТО территории не может быть пустым.\n\nСохранение отменено.");
        return;
    }
    if (ui->ln_kladr->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Код КЛАДР территории не может быть пустым.\n\nСохранение отменено.");
        return;
    }

    // проверим название на уникальность
    QString sql = "select count(*) from address_struct "
                  " where upper(name)='" + ui->ln_terr_name->text().trimmed().simplified().toUpper() + "' ; ";

    QSqlQuery *query = new QSqlQuery(db);
    if (!mySQL.exec(this, sql, "Проверка названия на уникальность", *query, true, db, data_app) || !query->next()) {
        QMessageBox::warning(this, "Непредвиденная ошибка", "При попытке проверить название фирмы на уникальность произошла неожиданная ошибка.\n\nСохранение отменено.");
        delete query;
        return;
    }
    if (query->value(0).toInt()>0) {
        QMessageBox::warning(this,
                              "Такое название уже есть",
                              "В базе данных уже есть территория с таким названием.\n\nСохранение отменено.");
        delete query;
        return;
    }


    if (QMessageBox::question(this,
                              "Нужно подтверждение",
                              "Добавить новую территорию?\n",
                              QMessageBox::Yes|QMessageBox::Cancel,
                              QMessageBox::Yes)==QMessageBox::Yes) {
        db.transaction();
        // добавим фирму
        QString sql = "insert into address_struct (ocato, name, kladr, id_up) "
                      " values ("
                      "   '" + ui->ln_okato->text().trimmed().simplified() + "', "
                      "   '" + ui->ln_terr_name->text().trimmed().simplified() + "', "
                      "   '" + ui->ln_kladr->text().trimmed().simplified() + "', "
                      "   " + (ui->spin_up->value()>0 ? QString::number(ui->spin_up->value()) : "NULL") + ") "
                      " returning id ; ";

        QSqlQuery *query = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Добавление новой фирмы", *query, true, db, data_app) || !query->next()) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке добавить новую фирму произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query;
            return;
        }
        delete query;
        db.commit();

    }
    refresh_terrs_tab();
}

void list_persons_terrs_wnd::on_tab_terrs_clicked(const QModelIndex &index) {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_terrs || !ui->tab_terrs->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить].");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_terrs->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        QModelIndex index = indexes.front();

        ui->ln_okato->setText(model_terrs.data(model_terrs.index(index.row(), 1), Qt::EditRole).toString());
        ui->ln_terr_name->setText(model_terrs.data(model_terrs.index(index.row(), 2), Qt::EditRole).toString());
        ui->ln_kladr->setText(model_terrs.data(model_terrs.index(index.row(), 3), Qt::EditRole).toString());
        ui->spin_up->setValue(model_terrs.data(model_terrs.index(index.row(), 4), Qt::EditRole).toInt());

    } else {
        ui->ln_okato->setText("");
        ui->ln_terr_name->setText("");
        ui->ln_kladr->setText("");
        ui->spin_up->setValue(0);
    }
}



void list_persons_terrs_wnd::on_bn_edit_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_terrs || !ui->tab_terrs->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить].");
        return;
    }

    // проверка, введено ли название
    if (ui->ln_terr_name->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Название территории не может быть пустым.\n\nСохранение отменено.");
        return;
    }
    if (ui->ln_okato->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Код ОКАТО территории не может быть пустым.\n\nСохранение отменено.");
        return;
    }
    if (ui->ln_kladr->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Код КЛАДР территории не может быть пустым.\n\nСохранение отменено.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_terrs->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Изменить данные выбранной территории?",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        QModelIndex index = indexes.front();

        int id = model_terrs.data(model_terrs.index(index.row(), 0), Qt::EditRole).toInt();

        // проверим название на уникальность
        QString sql_test = "select count(*) from address_struct "
                           " where id<>" + QString::number(id) +
                           "       and upper(name)='" + ui->ln_terr_name->text().trimmed().simplified().toUpper() + "' ; ";

        QSqlQuery *query_test = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_test, "Проверка названия на уникальность", *query_test, true, db, data_app) || !query_test->next()) {
            QMessageBox::warning(this, "Непредвиденная ошибка", "При попытке проверить название фирмы на уникальность произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query_test;
            return;
        }
        if (query_test->value(0).toInt()>0) {
            QMessageBox::warning(this,
                                  "Такое название уже есть",
                                  "В базе данных уже есть территория с таким названием.\n\nСохранение отменено.");
            delete query_test;
            return;
        }
        delete query_test;

        db.transaction();
        // добавим территорию
        QString sql = "update address_struct "
                      " set "
                      "   ocato='" + ui->ln_okato->text().trimmed().simplified() + "', "
                      "   name='" + ui->ln_terr_name->text().trimmed().simplified() + "', "
                      "   kladr='" + ui->ln_kladr->text().trimmed().simplified() + "', "
                      "   id_up=" + (ui->spin_up->value()>0 ? QString::number(ui->spin_up->value()) : "NULL") + " "
                      " where id=" + QString::number(id) + " ; ";

        QSqlQuery *query = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Обновление фирмы", *query, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке изменить данные фирмы произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query;
            return;
        }
        delete query;
        db.commit();

        refresh_terrs_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void list_persons_terrs_wnd::on_bn_delete_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_terrs || !ui->tab_terrs->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить].");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_terrs->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Удалить выбранную запись из базы?\n",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        QModelIndex index = indexes.front();

        int id = model_terrs.data(model_terrs.index(index.row(), 0), Qt::EditRole).toInt();

        db.transaction();

        // удалим
        QString sql;
        sql = "delete from address_struct "
              " where id=" + QString::number(id) + " ; ";
        QSqlQuery *query2 = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Удаление территории", *query2, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке удалить территорию произошла неожиданная ошибка.\n\nУдаление отменено.");
            delete query2;
            return;
        }
        delete query2;

        db.commit();

        refresh_terrs_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}
