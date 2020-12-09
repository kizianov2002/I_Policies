#include "spr_insure_errors_wnd.h"
#include "ui_spr_insure_errors_wnd.h"

spr_insure_errors_wnd::spr_insure_errors_wnd(QSqlDatabase &db, s_data_app &data_app, QWidget *parent) :
    db(db), data_app(data_app), QDialog(parent), ui(new Ui::spr_insure_errors_wnd)
{
    ui->setupUi(this);

    change_error_w = NULL;

    refresh_errors_tab();
}

spr_insure_errors_wnd::~spr_insure_errors_wnd() {
    delete ui;
}

void spr_insure_errors_wnd::on_bn_close_clicked() {
    close();
}

void spr_insure_errors_wnd::on_bn_refresh_clicked() {
    refresh_errors_tab();
}

void spr_insure_errors_wnd::refresh_errors_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    QString errors_sql = "select id, ABS(code), action, text, comment "
                         "  from spr_insure_errors "
                         " order by code ; ";

    model_errors.setQuery(errors_sql,db);
    QString err = model_errors.lastError().driverText();

    // подключаем модель из БД
    ui->tab_errors->setModel(&model_errors);

    // обновляем таблицу
    ui->tab_errors->reset();

    // задаём ширину колонок
    ui->tab_errors->setColumnWidth( 0,  1);    // id
    ui->tab_errors->setColumnWidth( 1, 60);    // code
    ui->tab_errors->setColumnWidth( 2, 70);    // event
    ui->tab_errors->setColumnWidth( 3,300);    // text
    ui->tab_errors->setColumnWidth( 4,500);    // comment

    // правим заголовки
    model_errors.setHeaderData( 1, Qt::Horizontal, ("код\nошибки"));
    model_errors.setHeaderData( 2, Qt::Horizontal, ("действие\nФФОМС"));
    model_errors.setHeaderData( 3, Qt::Horizontal, ("описание ошибки"));
    model_errors.setHeaderData( 4, Qt::Horizontal, ("комментарий"));
    ui->tab_errors->repaint();

    this->setCursor(Qt::ArrowCursor);
}

void spr_insure_errors_wnd::on_tab_errors_clicked(const QModelIndex &index) {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_errors || !ui->tab_errors->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить].");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_errors->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        QModelIndex index = indexes.front();

        ui->lab_code->setText(model_errors.data(model_errors.index(index.row(), 1), Qt::EditRole).toString());
        QString event = model_errors.data(model_errors.index(index.row(), 2), Qt::EditRole).toString();
        if (event=="E") {
            ui->lab_event->setText("запись отклонена");
        } else if (event=="W") {
            ui->lab_event->setText("предупре- ждение");
        } else if (event=="У") {
            ui->lab_event->setText("условная зависимость");
        } else {
            ui->lab_event->setText(" ");
        }
        ui->lab_text->setText(model_errors.data(model_errors.index(index.row(), 3), Qt::EditRole).toString());
        ui->lab_comment->setText(model_errors.data(model_errors.index(index.row(), 4), Qt::EditRole).toString());

    } else {
        ui->lab_code->setText("");
        ui->lab_event->setText("");
        ui->lab_text->setText("");
        ui->lab_comment->setText("");
    }
}


void spr_insure_errors_wnd::on_bn_delete_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_errors || !ui->tab_errors->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить].");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_errors->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Удалить выбранную запись из базы?\n\n"
                                  "(Это необратимая операция)\n",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        QModelIndex index = indexes.front();
        int id_error = model_errors.data(model_errors.index(index.row(), 0), Qt::EditRole).toInt();

        db.transaction();

        QString sql_del = "delete from spr_insure_errors "
                          " where id=" + QString::number(id_error) + " ; ";
        QSqlQuery *query_del = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_del, "Удаление строки ошибки", *query_del, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке удалить строку ошибки произошла неожиданная ошибка.\n\nДействие отменено.");
            delete query_del;
            return;
        }
        delete query_del;

        db.commit();

        refresh_errors_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void spr_insure_errors_wnd::on_bn_add_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_errors || !ui->tab_errors->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить].");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_errors->selectionModel()->selection().indexes();

    s_data_error data_error;

    if (indexes.size()>0) {
        QModelIndex index = indexes.front();

        data_error.id = model_errors.data(model_errors.index(index.row(), 0), Qt::EditRole).toInt();
        data_error.code = model_errors.data(model_errors.index(index.row(), 1), Qt::EditRole).toInt();
        data_error.event = model_errors.data(model_errors.index(index.row(), 2), Qt::EditRole).toString();
        data_error.text = model_errors.data(model_errors.index(index.row(), 3), Qt::EditRole).toString();
        data_error.comment = model_errors.data(model_errors.index(index.row(), 4), Qt::EditRole).toString();

    } else {
        data_error.id = -1;
        data_error.code = 0;
        data_error.event = "";
        data_error.text = "";
        data_error.comment = "";
    }

    // правка данных ошибки в окне change_error_wnd
    delete change_error_w;
    change_error_w = new change_error_wnd(data_error, this);
    if (change_error_w->exec()) {
        if (data_error.id>=0) {

            QString sql_tst = "select count(*) "
                              "  from spr_insure_errors "
                              " where code=" + QString::number(data_error.code) + " ; ";
            QSqlQuery *query_tst = new QSqlQuery(db);
            if (!mySQL.exec(this, sql_tst, "Подсчёт числа готовых строк ошибки", *query_tst, true, db, data_app)) {
                db.rollback();
                QMessageBox::critical(this, "Непредвиденная ошибка", "При подсчёте числа готовых строк ошибки произошла неожиданная ошибка.\n\nДействие отменено.");
                delete query_tst;
                return;
            }
            query_tst->next();
            int cnt = query_tst->value(0).toInt();
            delete query_tst;
            if (cnt>0) {
                if (QMessageBox::question(this,
                                          "Нужно подтверждение",
                                          "В справочнике уже есть " + QString::number(cnt) + " строк(а) с таким же кодом ошибки. При добавлении новой строки, старые данные будут удалены.\n"
                                          "Вы действительно хотите заменить эти данные новыми?\n\n"
                                          "(Это необратимая операция)\n",
                                          QMessageBox::Yes|QMessageBox::Cancel,
                                          QMessageBox::Yes)==QMessageBox::Cancel) {
                    return;
                }
            }

            QString sql_del = "delete from spr_insure_errors "
                              " where code=" + QString::number(data_error.code) + " ; ";
            QSqlQuery *query_del = new QSqlQuery(db);
            if (!mySQL.exec(this, sql_del, "Удаление строки ошибки", *query_del, true, db, data_app)) {
                db.rollback();
                QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке удалить строку ошибки произошла неожиданная ошибка.\n\nДействие отменено.");
                delete query_del;
                return;
            }
            delete query_del;
        }
        QString sql_add = "insert into spr_insure_errors(code, action, text, comment) "
                          "values (" + QString::number(data_error.code) + ""
                                "  ,'" + data_error.event  + "'"
                                "  ,'" + data_error.text    + "'"
                                "  ,'" + data_error.comment + "') ; ";
        QSqlQuery *query_add = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_add, "Добавление строки ошибки", *query_add, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке добавить строку ошибки произошла неожиданная ошибка.\n\nДействие отменено.");
            delete query_add;
            return;
        }
        delete query_add;

        refresh_errors_tab();
    }

}
