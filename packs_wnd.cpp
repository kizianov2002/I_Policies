#include "packs_wnd.h"
#include "ui_packs_wnd.h"

packs_wnd::packs_wnd(QSqlDatabase &db, s_data_app &data_app, s_data_pack &data_pack, QWidget *parent)
   : db(db), data_app(data_app), data_pack(data_pack), QDialog(parent), ui(new Ui::packs_wnd)
{
    ui->setupUi(this);

    ui->split_packs->setStretchFactor(0,2);
    ui->split_packs->setStretchFactor(1,1);

    refresh_points();
    ui->date->setDate(QDate::currentDate());
    ui->date_S->setDate(QDate::currentDate());
    refresh_stops_tab();
}

packs_wnd::~packs_wnd() {
    delete ui;
}

void packs_wnd::on_bn_close_clicked() {
    close();
}

void packs_wnd::refresh_points() {
    this->setCursor(Qt::WaitCursor);

    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "";
    sql += QString("select p.point_regnum, '('||p.point_regnum||') '||p.point_name as name, p.point_rights "
                   "  from points p "
                   " where status>0 "
                   " order by p.point_regnum, id ; ");
    query->exec( sql );
    ui->combo_point->clear();
    points_rights.clear();

    // заполнение списка пунктов выдачи
    while (query->next()) {
        ui->combo_point->addItem( query->value(1).toString(),
                                  query->value(0).toString() );
        points_rights.append( query->value(2).toInt() );
    }
    query->finish();
    delete query;

    ui->combo_point->setCurrentIndex(0);

    this->setCursor(Qt::ArrowCursor);
}

void packs_wnd::refresh_packs_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    QString packs_sql = "select c.id, c.pack_name, "
                        "       c.id_file_i,   i.file_name  as file_name_i,   c.date_file_i,   i.n_recs                    as n_recs_i, "
                        "       c.id_file_p,   p.file_name  as file_name_p,   c.date_file_p,   p.n_recs ||' / '||p.n_errs  as n_recs_p, "
                        "       c.id_file_p_1, p1.file_name as file_name_f_1, c.date_file_p_1, p1.n_recs||' / '||p1.n_errs as n_recs_p1,"
                        "       c.id_file_f,   f.file_name  as file_name_f,   c.date_file_f,   f.n_recs                    as n_recs_f  "
                        "  from files_packs c "
                        "       left join files_out_i i on(i.id=c.id_file_i) "
                        "       left join files_in_p  p on(p.id=c.id_file_p) "
                        "       left join files_in_p  p1 on(p1.id=c.id_file_p_1) "
                        "       left join files_in_f  f on(f.id=c.id_file_f) ";

    packs_sql += " where 1=1 ";

    if (ui->ch_point->isChecked()) {
        int rights = points_rights.at(ui->combo_point->currentIndex());
        //switch (rights) {
        //case 0:/*
        //    packs_sql += "   and 2=3 ";
        //    break;*/
        //case 1:
        //case 2:
            packs_sql += "   and right(left(c.pack_name,9),3)='" + ui->combo_point->currentData().toString() + "' ";
        //    break;
        //case 3:
        //    break;
        //}
    }
    if (ui->ch_date->isChecked()) {
        packs_sql += "   and c.date_file_i='" + ui->date->date().toString("yyyy-MM-dd") + "' ";
    }
    packs_sql += " order by c.id desc ; ";


    model_packs.setQuery(packs_sql,db);
    QString err2 = model_packs.lastError().driverText();

    // подключаем модель из БД
    ui->tab_packs->setModel(&model_packs);

    // обновляем таблицу
    ui->tab_packs->reset();

    // задаём ширину колонок
    ui->tab_packs->setColumnWidth( 0,  1);     // id,
    ui->tab_packs->setColumnWidth( 1,110);     // pack_name

    ui->tab_packs->setColumnWidth( 2,  1);     // id_file_i,
    ui->tab_packs->setColumnWidth( 3,110);     // file_name_i
    ui->tab_packs->setColumnWidth( 4, 70);     // date_file_i
    ui->tab_packs->setColumnWidth( 5, 40);     // n_recs_i

    ui->tab_packs->setColumnWidth( 6,  1);     // id_file_p,
    ui->tab_packs->setColumnWidth( 7,115);     // file_name_p
    ui->tab_packs->setColumnWidth( 8, 70);     // date_file_p
    ui->tab_packs->setColumnWidth( 9, 70);     // n_recs_p

    ui->tab_packs->setColumnWidth(10,  1);     // id_file_p1
    ui->tab_packs->setColumnWidth(11,130);     // file_name_p1
    ui->tab_packs->setColumnWidth(12, 70);     // date_file_p1
    ui->tab_packs->setColumnWidth(13, 70);     // n_recs_p1

    ui->tab_packs->setColumnWidth(14,  1);     // id_file_f,
    ui->tab_packs->setColumnWidth(15, 90);     // file_name_f
    ui->tab_packs->setColumnWidth(16, 70);     // date_file_f
    ui->tab_packs->setColumnWidth(17, 40);     // n_recs_f

    // правим заголовки
    model_packs.setHeaderData( 1, Qt::Horizontal, ("посылка"));

    model_packs.setHeaderData( 2, Qt::Horizontal, ("id I"));
    model_packs.setHeaderData( 3, Qt::Horizontal, ("выходной файл"));
    model_packs.setHeaderData( 4, Qt::Horizontal, ("дата I"));
    model_packs.setHeaderData( 5, Qt::Horizontal, ("строк"));

    model_packs.setHeaderData( 6, Qt::Horizontal, ("id P"));
    model_packs.setHeaderData( 7, Qt::Horizontal, ("протокол"));
    model_packs.setHeaderData( 8, Qt::Horizontal, ("дата P"));
    model_packs.setHeaderData( 9, Qt::Horizontal, ("стр/ошиб"));

    model_packs.setHeaderData(10, Qt::Horizontal, ("id P1"));
    model_packs.setHeaderData(11, Qt::Horizontal, ("протокол P1"));
    model_packs.setHeaderData(12, Qt::Horizontal, ("дата P1"));
    model_packs.setHeaderData(13, Qt::Horizontal, ("стр/ошиб"));

    model_packs.setHeaderData(14, Qt::Horizontal, ("id F"));
    model_packs.setHeaderData(15, Qt::Horizontal, ("файл ФЛК"));
    model_packs.setHeaderData(16, Qt::Horizontal, ("дата F"));
    model_packs.setHeaderData(17, Qt::Horizontal, ("строк"));
    ui->tab_packs->repaint();

    this->setCursor(Qt::ArrowCursor);
}

void packs_wnd::on_ch_point_clicked(bool checked) {
    ui->combo_point->setEnabled(checked);
    refresh_packs_tab();
}

void packs_wnd::on_ch_date_clicked(bool checked) {
    ui->date->setEnabled(checked);
    ui->bn_today->setEnabled(checked);
    refresh_packs_tab();
}

void packs_wnd::on_ch_date_S_clicked(bool checked) {
    ui->date_S->setEnabled(checked);
    refresh_stops_tab();
}

void packs_wnd::on_bn_today_clicked() {
    ui->date->setDate(QDate::currentDate());
    refresh_packs_tab();
    ui->date_S->setDate(QDate::currentDate());
    refresh_stops_tab();
}

void packs_wnd::on_date_dateChanged(const QDate &date) {
    refresh_packs_tab();
}

void packs_wnd::on_date_S_dateChanged(const QDate &date) {
    refresh_stops_tab();
}

void packs_wnd::on_bn_refresh_clicked() {
    refresh_packs_tab();
}

void packs_wnd::on_bn_delete_pack_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_packs || !ui->tab_packs->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите посылку из списка.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes_pack = ui->tab_packs->selectionModel()->selection().indexes();

    if (indexes_pack.size()>0) {
        QModelIndex index_pack = indexes_pack.front();
        db.transaction();

        // данные посылки
        int id_pack   = model_packs.data(model_packs.index(index_pack.row(), 0), Qt::EditRole).isNull() ? -1 :
                        model_packs.data(model_packs.index(index_pack.row(), 0), Qt::EditRole).toInt();
        int id_file_i = model_packs.data(model_packs.index(index_pack.row(), 2), Qt::EditRole).isNull() ? -1 :
                        model_packs.data(model_packs.index(index_pack.row(), 2), Qt::EditRole).toInt();
        int id_file_p = model_packs.data(model_packs.index(index_pack.row(), 6), Qt::EditRole).isNull() ? -1 :
                        model_packs.data(model_packs.index(index_pack.row(), 6), Qt::EditRole).toInt();
        int id_file_p1= model_packs.data(model_packs.index(index_pack.row(),10), Qt::EditRole).isNull() ? -1 :
                        model_packs.data(model_packs.index(index_pack.row(),10), Qt::EditRole).toInt();
        int id_file_f = model_packs.data(model_packs.index(index_pack.row(),14), Qt::EditRole).isNull() ? -1 :
                        model_packs.data(model_packs.index(index_pack.row(),14), Qt::EditRole).toInt();

        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Вы действительно хотите удалить все данные выбранной посылки из базы полисов?\n\n"
                                  "Внимание - это необратимая операция. \nДанные посылки будут полностью удалены из базы и новый файл посылки может получить имя старой, ранее удалённой.\n"
                                  "Данная опция предназначена для удаления из базы посылок, которые так и небыли отправлены в фонд, либо небыли приняты фондом из-за ошибок в данных или нарушения структуры файла.\n",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        // прочитаем id-шки событий из посылки
        QString sql_act;
        sql_act = "select id_event "
                  " from files_r_evts "
                  " where id_file_i=" + QString::number(id_file_i) + " ; ";

        QSqlQuery *query_act = new QSqlQuery(db);
        mySQL.exec(this, sql_act, QString("Выбор событий из посылки"), *query_act, true, db, data_app);
        while (query_act->next()) {
            int id_act = query_act->value(0).toInt();

            // обнулим статус события из посылки
            QString sql_clear;
            sql_clear = "update events "
                        "   set status=0, "
                        "       duty_field = duty_field||'  ! '||CURRENT_DATE||' - признак отправки этого события был снят при удалении его посылки.' "
                        " where id=" + QString::number(id_act) + " ; ";

            QSqlQuery *query_clear = new QSqlQuery(db);
            mySQL.exec(this, sql_clear, QString("Сброс статуса события"), *query_clear, true, db, data_app);
        }
        delete query_act;

        QString sql_del = "";
        sql_del += "delete from files_r_evts where id_file_i=" + QString::number(id_file_i) + " ; ";
        sql_del += "delete from frecs_out_i  where id_file_i=" + QString::number(id_file_i)  + " ; ";
        sql_del += "delete from files_out_i  where id="        + QString::number(id_file_i)  + " ; ";
        sql_del += "delete from frecs_in_p   where id_file_p=" + QString::number(id_file_p)  + " ; ";
        sql_del += "delete from files_in_p   where id="        + QString::number(id_file_p)  + " ; ";
        sql_del += "delete from frecs_in_p   where id_file_p=" + QString::number(id_file_p1) + " ; ";
        sql_del += "delete from files_in_p   where id="        + QString::number(id_file_p1) + " ; ";
        sql_del += "delete from frecs_in_f   where id_file_f=" + QString::number(id_file_f)  + " ; ";
        sql_del += "delete from files_in_f   where id="        + QString::number(id_file_f)  + " ; ";
        sql_del += "delete from files_packs  where id=" + QString::number(id_pack)    + " ; ";
        QSqlQuery *query_del = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_del, "Удаление посылки", *query_del, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка",
                                  "При попытке удалить данные посылки в фонд произошла неожиданная ошибка.\n\n"
                                  "Удаление отменено.");
            delete query_del;
            return;
        }
        data_pack.id = -1;
        data_pack.pack_name = "";
        delete query_del;

        db.commit();

        refresh_packs_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void packs_wnd::on_combo_point_activated(int index) {
    refresh_packs_tab();
}

#include "qt_windows.h"
#include "qwindowdefs_win.h"
#include <shellapi.h>

void packs_wnd::on_bn_save_file_I_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_packs || !ui->tab_packs->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите посылку из списка.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes_pack = ui->tab_packs->selectionModel()->selection().indexes();

    if (indexes_pack.size()>0) {
        QModelIndex index_pack = indexes_pack.front();
        db.transaction();

        // данные посылки
        int id_pack   = model_packs.data(model_packs.index(index_pack.row(), 0), Qt::EditRole).isNull() ? -1 :
                        model_packs.data(model_packs.index(index_pack.row(), 0), Qt::EditRole).toInt();
        int id_file_i = model_packs.data(model_packs.index(index_pack.row(), 2), Qt::EditRole).isNull() ? -1 :
                        model_packs.data(model_packs.index(index_pack.row(), 2), Qt::EditRole).toInt();
        int id_file_p = model_packs.data(model_packs.index(index_pack.row(), 6), Qt::EditRole).isNull() ? -1 :
                        model_packs.data(model_packs.index(index_pack.row(), 6), Qt::EditRole).toInt();
        int id_file_p1= model_packs.data(model_packs.index(index_pack.row(),10), Qt::EditRole).isNull() ? -1 :
                        model_packs.data(model_packs.index(index_pack.row(),10), Qt::EditRole).toInt();
        int id_file_f = model_packs.data(model_packs.index(index_pack.row(),14), Qt::EditRole).isNull() ? -1 :
                        model_packs.data(model_packs.index(index_pack.row(),14), Qt::EditRole).toInt();

        QString sql_sel = "select file_name, bin from files_out_i "
                          " where id=" + QString::number(id_file_i) + " ; ";

        QSqlQuery *query_sel = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_sel, "Получим файл посылки", *query_sel, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка",
                                  "При попытке получить файл посылки произошла неожиданная ошибка.\n\n"
                                  "Действие отменено.");
            delete query_sel;
            return;
        }
        if (query_sel->next()) {
            QString name_file_i = query_sel->value(0).toString();
            QByteArray arr = query_sel->value(1).toByteArray();
            QString fname_res = data_app.path_temp + name_file_i;

            if ( fname_res.right(4).toLower()!=".xml"
                 && fname_res.right(4).toLower()!=".zip"
                 && fname_res.right(4).toLower()!=".7z" )
                fname_res += ".zip";

            //mySQL.myUnBYTEA(BYTEA_str, fname_res);
            QFile f(fname_res);

            f.remove();
            if (!f.open(QIODevice::WriteOnly)) {
                QMessageBox::warning(this,
                                     "Ошибка создания файла",
                                     "При создании целевого файла произошла ошибка.\n\n"
                                     "Операция отменена.");
                return;
            }
            f.write(arr );
            f.close();
            long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);
        } else {
            QMessageBox::warning(this, "Ничего не получено", "При попытке получить данныые файла протоколов ничего не получено.");
        }
        delete query_sel;

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}



void packs_wnd::on_bn_save_file_P_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_packs || !ui->tab_packs->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите посылку из списка.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes_pack = ui->tab_packs->selectionModel()->selection().indexes();

    if (indexes_pack.size()>0) {
        QModelIndex index_pack = indexes_pack.front();
        db.transaction();

        // данные посылки
        int id_pack   = model_packs.data(model_packs.index(index_pack.row(), 0), Qt::EditRole).isNull() ? -1 :
                        model_packs.data(model_packs.index(index_pack.row(), 0), Qt::EditRole).toInt();
        int id_file_i = model_packs.data(model_packs.index(index_pack.row(), 2), Qt::EditRole).isNull() ? -1 :
                        model_packs.data(model_packs.index(index_pack.row(), 2), Qt::EditRole).toInt();
        int id_file_p = model_packs.data(model_packs.index(index_pack.row(), 6), Qt::EditRole).isNull() ? -1 :
                        model_packs.data(model_packs.index(index_pack.row(), 6), Qt::EditRole).toInt();
        int id_file_p1= model_packs.data(model_packs.index(index_pack.row(),10), Qt::EditRole).isNull() ? -1 :
                        model_packs.data(model_packs.index(index_pack.row(),10), Qt::EditRole).toInt();
        int id_file_f = model_packs.data(model_packs.index(index_pack.row(),14), Qt::EditRole).isNull() ? -1 :
                        model_packs.data(model_packs.index(index_pack.row(),14), Qt::EditRole).toInt();

        QString sql_sel = "select file_name, bin from files_in_p "
                          " where id=" + QString::number(id_file_p) + " ; ";

        QSqlQuery *query_sel = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_sel, "Получим файл посылки", *query_sel, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка",
                                  "При попытке получить файл протокола произошла неожиданная ошибка.\n\n"
                                  "Действие отменено.");
            delete query_sel;
            return;
        }
        if (query_sel->next()) {
            QString name_file_p = query_sel->value(0).toString();
            QByteArray arr = query_sel->value(1).toByteArray();
            QString fname_res = data_app.path_temp + name_file_p;

            if ( fname_res.right(4).toLower()!=".xml"
                 && fname_res.right(4).toLower()!=".zip"
                 && fname_res.right(4).toLower()!=".7z" )
                fname_res += ".xml";

            //mySQL.myUnBYTEA(BYTEA_str, fname_res);
            QFile f(fname_res);

            f.remove();
            if (!f.open(QIODevice::WriteOnly)) {
                QMessageBox::warning(this,
                                     "Ошибка создания файла",
                                     "При создании целевого файла произошла ошибка.\n\n"
                                     "Операция отменена.");
                return;
            }
            f.write(arr );
            f.close();
            long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);
        } else {
            QMessageBox::warning(this, "Ничего не получено", "При попытке получить данныые файла протоколов ничего не получено.");
        }
        delete query_sel;

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}


void packs_wnd::on_bn_save_file_P1_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_packs || !ui->tab_packs->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите посылку из списка.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes_pack = ui->tab_packs->selectionModel()->selection().indexes();

    if (indexes_pack.size()>0) {
        QModelIndex index_pack = indexes_pack.front();
        db.transaction();

        // данные посылки
        int id_pack   = model_packs.data(model_packs.index(index_pack.row(), 0), Qt::EditRole).isNull() ? -1 :
                        model_packs.data(model_packs.index(index_pack.row(), 0), Qt::EditRole).toInt();
        int id_file_i = model_packs.data(model_packs.index(index_pack.row(), 2), Qt::EditRole).isNull() ? -1 :
                        model_packs.data(model_packs.index(index_pack.row(), 2), Qt::EditRole).toInt();
        int id_file_p = model_packs.data(model_packs.index(index_pack.row(), 6), Qt::EditRole).isNull() ? -1 :
                        model_packs.data(model_packs.index(index_pack.row(), 6), Qt::EditRole).toInt();
        int id_file_p1= model_packs.data(model_packs.index(index_pack.row(),10), Qt::EditRole).isNull() ? -1 :
                        model_packs.data(model_packs.index(index_pack.row(),10), Qt::EditRole).toInt();
        int id_file_f = model_packs.data(model_packs.index(index_pack.row(),14), Qt::EditRole).isNull() ? -1 :
                        model_packs.data(model_packs.index(index_pack.row(),14), Qt::EditRole).toInt();

        QString sql_sel = "select file_name, bin from files_in_p "
                          " where id=" + QString::number(id_file_p1) + " ; ";

        QSqlQuery *query_sel = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_sel, "Получим файл посылки", *query_sel, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка",
                                  "При попытке получить файл предварительного протокола произошла неожиданная ошибка.\n\n"
                                  "Действие отменено.");
            delete query_sel;
            return;
        }
        if (query_sel->next()) {
            QString name_file_p1 = query_sel->value(0).toString();
            QByteArray arr = query_sel->value(1).toByteArray();
            QString fname_res = data_app.path_temp + name_file_p1;

            if ( fname_res.right(4).toLower()!=".xml"
                 && fname_res.right(4).toLower()!=".zip"
                 && fname_res.right(4).toLower()!=".7z" )
                fname_res += ".xml";

            //mySQL.myUnBYTEA(BYTEA_str, fname_res);
            QFile f(fname_res);

            f.remove();
            if (!f.open(QIODevice::WriteOnly)) {
                QMessageBox::warning(this,
                                     "Ошибка создания файла",
                                     "При создании целевого файла произошла ошибка.\n\n"
                                     "Операция отменена.");
                return;
            }
            f.write(arr );
            f.close();
            long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);
        } else {
            QMessageBox::warning(this, "Ничего не получено", "При попытке получить данныые предварительного файла протоколов ничего не получено.");
        }
        delete query_sel;

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}


void packs_wnd::on_bn_save_file_F_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_packs || !ui->tab_packs->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите посылку из списка.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes_pack = ui->tab_packs->selectionModel()->selection().indexes();

    if (indexes_pack.size()>0) {
        QModelIndex index_pack = indexes_pack.front();
        db.transaction();

        // данные посылки
        int id_pack   = model_packs.data(model_packs.index(index_pack.row(), 0), Qt::EditRole).isNull() ? -1 :
                        model_packs.data(model_packs.index(index_pack.row(), 0), Qt::EditRole).toInt();
        int id_file_i = model_packs.data(model_packs.index(index_pack.row(), 2), Qt::EditRole).isNull() ? -1 :
                        model_packs.data(model_packs.index(index_pack.row(), 2), Qt::EditRole).toInt();
        int id_file_p = model_packs.data(model_packs.index(index_pack.row(), 6), Qt::EditRole).isNull() ? -1 :
                        model_packs.data(model_packs.index(index_pack.row(), 6), Qt::EditRole).toInt();
        int id_file_p1= model_packs.data(model_packs.index(index_pack.row(),10), Qt::EditRole).isNull() ? -1 :
                        model_packs.data(model_packs.index(index_pack.row(),10), Qt::EditRole).toInt();
        int id_file_f = model_packs.data(model_packs.index(index_pack.row(),14), Qt::EditRole).isNull() ? -1 :
                        model_packs.data(model_packs.index(index_pack.row(),14), Qt::EditRole).toInt();

        QString sql_sel = "select file_name, bin from files_in_f "
                          " where id=" + QString::number(id_file_f) + " ; ";

        QSqlQuery *query_sel = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_sel, "Получим файл посылки", *query_sel, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка",
                                  "При попытке получить файл ФЛК произошла неожиданная ошибка.\n\n"
                                  "Действие отменено.");
            delete query_sel;
            return;
        }
        if (query_sel->next()) {
            QString name_file_f = query_sel->value(0).toString();
            QByteArray arr = query_sel->value(1).toByteArray();
            QString fname_res = data_app.path_temp + name_file_f;

            if ( fname_res.right(4).toLower()!=".xml"
                 && fname_res.right(4).toLower()!=".zip"
                 && fname_res.right(4).toLower()!=".7z" )
                fname_res += ".xml";

            //mySQL.myUnBYTEA(BYTEA_str, fname_res);
            QFile f(fname_res);

            f.remove();
            if (!f.open(QIODevice::WriteOnly)) {
                QMessageBox::warning(this,
                                     "Ошибка создания файла",
                                     "При создании целевого файла произошла ошибка.\n\n"
                                     "Операция отменена.");
                return;
            }
            f.write(arr );
            f.close();
            long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);
        } else {
            QMessageBox::warning(this, "Ничего не получено", "При попытке получить данныые файла ФЛК ничего не получено.");
        }
        delete query_sel;

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void packs_wnd::on_bn_refresh_stops_clicked() {
    refresh_stops_tab();
}

void packs_wnd::refresh_stops_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    QString stops_sql = "select s.id, s.ocato, s.guid, s.vers, s.file_name, s.date_get, s.smo_regnum, s.n_recs, s.point_regnum  "
                        "  from files_in_s s "
                        " where 1=1 ";

    if (ui->ch_date_S->isChecked())
        stops_sql  +=   "   and s.date_get='" + ui->date_S->date().toString("yyyy-MM-dd") + "' ";

    stops_sql    +=     " order by s.file_name desc ; ";


    model_stops.setQuery(stops_sql,db);
    QString err2 = model_stops.lastError().driverText();

    // подключаем модель из БД
    ui->tab_stops->setModel(&model_stops);

    // обновляем таблицу
    ui->tab_stops->reset();

    // задаём ширину колонок
    ui->tab_stops->setColumnWidth( 0,  1);     // id,
    ui->tab_stops->setColumnWidth( 1,  2);     // ocato
    ui->tab_stops->setColumnWidth( 2,  2);     // guid,
    ui->tab_stops->setColumnWidth( 3,  2);     // vers
    ui->tab_stops->setColumnWidth( 4,140);     // file_name
    ui->tab_stops->setColumnWidth( 5, 67);     // date_get
    ui->tab_stops->setColumnWidth( 6,  2);     // smo_regnum,
    ui->tab_stops->setColumnWidth( 7, 40);     // n_recs
    ui->tab_stops->setColumnWidth( 8, 30);     // point_reegnum
    // правим заголовки
    model_stops.setHeaderData( 4, Qt::Horizontal, ("СТОП-файл"));
    model_stops.setHeaderData( 5, Qt::Horizontal, ("дата S"));
    model_stops.setHeaderData( 7, Qt::Horizontal, ("строк"));
    model_stops.setHeaderData( 8, Qt::Horizontal, ("ПВП"));
    ui->tab_stops->repaint();

    this->setCursor(Qt::ArrowCursor);
}


void packs_wnd::on_bn_save_file_S_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_stops || !ui->tab_stops->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите посылку из списка.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes_stop = ui->tab_stops->selectionModel()->selection().indexes();

    if (indexes_stop.size()>0) {
        QModelIndex index_stop = indexes_stop.front();
        db.transaction();

        // данные посылки
        int id_stop   = model_stops.data(model_stops.index(index_stop.row(), 0), Qt::EditRole).isNull() ? -1 :
                        model_stops.data(model_stops.index(index_stop.row(), 0), Qt::EditRole).toInt();

        QString sql_sel = "select file_name, bin from files_in_s "
                          " where id=" + QString::number(id_stop) + " ; ";

        QSqlQuery *query_sel = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_sel, "Получим файл посылки", *query_sel, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка",
                                  "При попытке получить СТОП-файл произошла неожиданная ошибка.\n\n"
                                  "Действие отменено.");
            delete query_sel;
            return;
        }
        if (query_sel->next()) {
            QString name_file_s = query_sel->value(0).toString();
            QByteArray arr = query_sel->value(1).toByteArray();
            QString fname_res = data_app.path_temp + name_file_s;

            if ( fname_res.right(4).toLower()!=".xml"
                 && fname_res.right(4).toLower()!=".zip"
                 && fname_res.right(4).toLower()!=".7z" )
                fname_res += ".xml";

            //mySQL.myUnBYTEA(BYTEA_str, fname_res);
            QFile f(fname_res);

            f.remove();
            if (!f.open(QIODevice::WriteOnly)) {
                QMessageBox::warning(this,
                                     "Ошибка создания файла",
                                     "При создании целевого файла произошла ошибка.\n\n"
                                     "Операция отменена.");
                return;
            }
            f.write(arr );
            f.close();
            long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);
        } else {
            QMessageBox::warning(this, "Ничего не получено", "При попытке получить данныые СТОП-файла ничего не получено.");
        }
        delete query_sel;

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}
