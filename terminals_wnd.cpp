#include "terminals_wnd.h"
#include "ui_terminals_wnd.h"

terminals_wnd::terminals_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent)
   : db(db), data_app(data_app), settings(settings), QDialog(parent), ui(new Ui::terminals_wnd)
{
    ui->setupUi(this);

    show_text_w = NULL;

    refresh_points();
    refresh_terminals();
}

terminals_wnd::~terminals_wnd() {
    delete ui;
}

void terminals_wnd::on_bn_close_clicked() {
    close();
}

void terminals_wnd::refresh_points() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка подразделений
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select id, '(' || point_regnum || ') ' || point_name "
                  " from public.points p "
                  " order by point_regnum ; ";
    mySQL.exec(this, sql, "Список пунктов выдачи полисов", *query, true, db, data_app);
    ui->combo_points->clear();
    while (query->next()) {
        ui->combo_points->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    //ui->combo_medter_spec->setCurrentIndex(-1);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void terminals_wnd::refresh_terminals() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    QString terrs_sql;
    terrs_sql += "select t.id_point, p.point_name, t.id, t.terminal_name, t.status, t.contacts, t.dt_last, ini, "
                 "       case when ini is NULL then ' - ' else ' есть ' end as has_ini "
                 " from terminals t "
                 "      left join points p on(p.id=t.id_point) ";

    if (ui->rb_point->isChecked())
        terrs_sql += " order by p.point_name ; ";
    if (ui->rb_term_code->isChecked())
        terrs_sql += " order by t.id ; ";
    if (ui->rb_term_name->isChecked())
        terrs_sql += " order by t.terminal_name ; ";

    model_terminals.setQuery(terrs_sql,db);
    QString err2 = model_terminals.lastError().driverText();

    // подключаем модель из БД
    //proxyModel_terminals.setSourceModel(&model_terminals);
    //ui->tab_terminals->setModel( &proxyModel_terminals );
    //ui->tab_terminals->setSortingEnabled(true);
    ui->tab_terminals->setModel( &model_terminals );
    //ui->tab_terminals->setSortingEnabled(true);

    // обновляем таблицу
    ui->tab_terminals->reset();

    // задаём ширину колонок
    ui->tab_terminals->setColumnWidth( 0, 50);     // id_point,
    ui->tab_terminals->setColumnWidth( 1,200);     // point,
    ui->tab_terminals->setColumnWidth( 2, 50);     // id_terminal,
    ui->tab_terminals->setColumnWidth( 3,250);     // terminal,
    ui->tab_terminals->setColumnWidth( 4, 50);     // status,
    ui->tab_terminals->setColumnWidth( 5, 80);     // contacts,
    ui->tab_terminals->setColumnWidth( 6,120);     // dt_last,
    ui->tab_terminals->setColumnWidth( 7,  1);     // ini
    ui->tab_terminals->setColumnWidth( 8, 50);     // has_ini

    // правим заголовки
    model_terminals.setHeaderData( 0, Qt::Horizontal, ("ID\nПВП"));
    model_terminals.setHeaderData( 1, Qt::Horizontal, ("ПВП"));
    model_terminals.setHeaderData( 2, Qt::Horizontal, ("ID\nтерм"));
    model_terminals.setHeaderData( 3, Qt::Horizontal, ("терминал"));
    model_terminals.setHeaderData( 4, Qt::Horizontal, ("статус"));
    model_terminals.setHeaderData( 5, Qt::Horizontal, ("контакты"));
    model_terminals.setHeaderData( 6, Qt::Horizontal, ("дата/время \nобновления"));
    model_terminals.setHeaderData( 8, Qt::Horizontal, ("есть\nINI"));
    ui->tab_terminals->repaint();

    ui->combo_points->setCurrentIndex(-1);
    ui->ln_terminal_name->setText("");
    ui->te_contacts->setText("");
    ui->combo_status->setCurrentIndex(-1);

    this->setCursor(Qt::ArrowCursor);
}

void terminals_wnd::on_bn_refresh_clicked() {
    refresh_terminals();
}

void terminals_wnd::on_tab_terminals_clicked(const QModelIndex &index) {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_terminals || !ui->tab_terminals->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\nНажмите кнопку [Обновить].");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_terminals->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        QModelIndex index = indexes.front();

        ui->combo_points->setCurrentIndex(ui->combo_points->findData(model_terminals.data(model_terminals.index(index.row(), 0), Qt::EditRole).toInt()));
        ui->ln_terminal_name->setText(model_terminals.data(model_terminals.index(index.row(), 3), Qt::EditRole).toString());
        ui->te_contacts->setText(model_terminals.data(model_terminals.index(index.row(), 5), Qt::EditRole).toString());
        ui->combo_status->setCurrentIndex(model_terminals.data(model_terminals.index(index.row(), 4), Qt::EditRole).toInt());

    } else {
        ui->combo_points->setCurrentIndex(-1);
        ui->ln_terminal_name->clear();
        ui->te_contacts->clear();
        ui->combo_status->setCurrentIndex(-1);
    }
}



void terminals_wnd::on_bn_edit_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_terminals || !ui->tab_terminals->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\nНажмите кнопку [Обновить].");
        return;
    }

    if (ui->combo_points->currentIndex()<0) {
        QMessageBox::warning(this, "Недостаточно данных", "Выберите ПВП, на котором размещается терминал.\n\nСохранение отменено.");
        return;
    }
    if (ui->ln_terminal_name->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Название терминала не может быть пустым.\n\nСохранение отменено.");
        return;
    }
    if (ui->combo_status->currentIndex()<0) {
        QMessageBox::warning(this, "Недостаточно данных", "Укажите статус терминала.\n\nСохранение отменено.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_terminals->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Изменить данные выбранного терминала?",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        QModelIndex index = indexes.front();

        int id = model_terminals.data(model_terminals.index(index.row(), 2), Qt::EditRole).toInt();

        db.transaction();
        QString sql = "update terminals "
                      " set "
                      "   id_point='" + QString::number(ui->combo_points->currentData().toInt()) + "', "
                      "   terminal_name='" + ui->ln_terminal_name->text().trimmed().simplified() + "', "
                      "   contacts='" + ui->te_contacts->toPlainText().replace("'", "\"") + "', "
                      "   status=" + QString::number(ui->combo_status->currentIndex()) + " "
                      " where id=" + QString::number(id) + " ; ";

        QSqlQuery *query = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Обновление фирмы", *query, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке изменить данные терминала произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query;
            return;
        }
        delete query;
        db.commit();

        refresh_terminals();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void terminals_wnd::on_bn_delete_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_terminals || !ui->tab_terminals->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\nНажмите кнопку [Обновить].");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_terminals->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Удалить выбранную запись из базы?\n",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        QModelIndex index = indexes.front();

        int id = model_terminals.data(model_terminals.index(index.row(), 2), Qt::EditRole).toInt();

        db.transaction();

        // удалим
        QString sql;
        sql = "delete from terminals "
              " where id=" + QString::number(id) + " ; ";
        QSqlQuery *query2 = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Удаление территории", *query2, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке удалить запись терминала произошла неожиданная ошибка.\n\nУдаление отменено.");
            delete query2;
            return;
        }
        delete query2;

        db.commit();

        refresh_terminals();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void terminals_wnd::on_bn_show_INI_clicked() {
    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_terminals->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        QModelIndex index = indexes.front();

        delete show_text_w;
        show_text_w = new show_text_wnd("INI-файл терминала \"" + ui->ln_terminal_name->text() + "\"",
                                        model_terminals.data(model_terminals.index(index.row(), 7), Qt::EditRole).toString().replace("[br]", "\n"),
                                        true, this);
        show_text_w->exec();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void terminals_wnd::on_bn_load_INI_clicked() {
    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_terminals->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Вы действительно хотите полностью заменить настройки этого рабочего места настройками выбранного терминала?\n\n"
                                  "ВНИМАНИМЕ!!!\n"
                                  "Эту операцию можно отменить только вручную переименовав резервную копию INI-файла.\n",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        QModelIndex index = indexes.front();
        QString ini_str = model_terminals.data(model_terminals.index(index.row(), 7), Qt::EditRole).toString().replace("[br]", "\n");
        QFile ini_file;

        // резервная копия
        int n = 0;
        QString bkp_name;
        QString ini_name = data_app.path_install + "_INKO_POLISES/inko_polises_2.ini";
        QFile bkp_file;
        do {
            bkp_name = data_app.path_install + "_INKO_POLISES/inko_polises_2.ini." + QDate::currentDate().toString("yyyy-MM-dd") + ".#" + QString::number(n) + ".backup";
            n++;
            bkp_file.setFileName(bkp_name);
        } while (bkp_file.exists());

        ini_file.copy(ini_name, bkp_name);

        // создадим новый файл
        QTextCodec *codec = QTextCodec::codecForName("cp1251");
        ini_file.setFileName(ini_name);
        ini_file.open(QIODevice::WriteOnly);
        QByteArray encodedString = codec->fromUnicode(ini_str);
        ini_file.write(encodedString);
        ini_file.close();

        QMessageBox::information(this, "Перезапустите программу",
                                 "Настройки программы обновлены.\n\n"
                                 "Перезапустите программу для применения нового файла настроек.");

        close();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void terminals_wnd::on_rb_point_clicked() {
    refresh_terminals();
}

void terminals_wnd::on_rb_term_code_clicked() {
    refresh_terminals();
}

void terminals_wnd::on_rb_term_name_clicked() {
    refresh_terminals();
}
