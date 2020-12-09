#include "talks_wnd.h"
#include "ui_talks_wnd.h"

talks_wnd::talks_wnd(QSqlDatabase &db, s_data_talk &data_talks, s_data_app &data_app, QWidget *parent) :
    db(db), data_talks(data_talks), data_app(data_app), QDialog(parent), ui(new Ui::talks_wnd)
{
    ui->setupUi(this);

    add_talk_w = NULL;

    data_talks._id_person_2 = -1;

    QRect rect = this->geometry();
    rect.setLeft(data_app.screen_w/2-450);
    rect.setTop(data_app.screen_h/2-400);
    rect.setHeight(250);
    this->setGeometry(rect);

    ui->ch_other->setChecked(false);
    on_ch_other_clicked(false);
    ui->lab_fio_pers->setText(data_talks.fio);

    refresh_talks_tab();
}

talks_wnd::~talks_wnd() {
    delete ui;
}

void talks_wnd::on_bn_close_clicked() {
    close();
}

void talks_wnd::refresh_talks_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, "Нет доступа к базе данных",
                             "Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n "
                             "Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику.");
        return;
    }

    sql_talks = "select t._id_person, t._id_polis, t._id_blank_pol, t.pol_v, "
                "       case when t._id_polis<0 then ' - ? - ' "
                "            when p.pol_v=2 then right('0000000000' || cast(p.vs_num as text), 11) "
                "            when (p.pol_v=3 and p.enp is NULL) then p.pol_ser || ' №' || p.pol_num "
                "            when (p.pol_v=3 and p.enp is not NULL) then p.pol_ser || ' №' || p.pol_num || ' ЕНП=' || p.enp "
                "            else ' - полис не выдан - ' "
                "       end as pol_id, "
                "       t.id, t.type_talk, t.date_talk, t.result, t.comment "
                "  from talks t "
                "       left join polises p on(p.id=t._id_polis) "
                "       left join blanks_pol bp on(bp.id=t._id_blank_pol) "
                " where _id_person=" + QString::number(data_talks._id_person) + " "
                " order by t.date_talk desc, p.pol_v desc ; ";

    model_talks.setQuery(sql_talks,db);
    QString err2 = model_talks.lastError().driverText();

    // подключаем модель из БД
    ui->talks_tab->setModel(&model_talks);

    // обновляем таблицу
    ui->talks_tab->reset();

    // задаём ширину колонок
    ui->talks_tab->setColumnWidth( 0,  1);     // t._id_person,
    ui->talks_tab->setColumnWidth( 1,  1);     // t._id_polis,
    ui->talks_tab->setColumnWidth( 2,  1);     // t._id_blank_pol,
    ui->talks_tab->setColumnWidth( 3, 60);     // p.pol_v,
    ui->talks_tab->setColumnWidth( 4,130);     // pol_id,
    ui->talks_tab->setColumnWidth( 5,  1);     // t.id,
    ui->talks_tab->setColumnWidth( 6,150);     // t.type_talk,
    ui->talks_tab->setColumnWidth( 7, 75);     // t.date_talk,
    ui->talks_tab->setColumnWidth( 8,200);     // t.result,
    ui->talks_tab->setColumnWidth( 9,200);     // t.comment
    // правим заголовки
    model_talks.setHeaderData( 3, Qt::Horizontal, ("тип"));
    model_talks.setHeaderData( 4, Qt::Horizontal, ("полис"));
    model_talks.setHeaderData( 6, Qt::Horizontal, ("форма общения"));
    model_talks.setHeaderData( 7, Qt::Horizontal, ("дата"));
    model_talks.setHeaderData( 8, Qt::Horizontal, ("результат"));
    model_talks.setHeaderData( 9, Qt::Horizontal, ("комментарий"));

    ui->talks_tab->repaint();

    this->setCursor(Qt::ArrowCursor);
}

void talks_wnd::on_bn_refresh_clicked() {
    refresh_talks_tab();
}

void talks_wnd::on_bn_add_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    data_talks.date_talk = QDate::currentDate();

    delete add_talk_w;
    data_talks.id = -1;
    add_talk_w = new add_talk_wnd(db, &data_talks, data_app, this);
    add_talk_w->exec();

    refresh_talks_tab();
}

void talks_wnd::on_bn_edit_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->talks_tab->selectionModel()->selection().indexes();
    if (indexes.size()>0) {
        this->setCursor(Qt::WaitCursor);
        QModelIndex index = indexes.front();

        s_data_talk data_talk;
        data_talk.id            = (model_talks.data(model_talks.index(index.row(), 5), Qt::EditRole).isNull() ? -1 :
                                   model_talks.data(model_talks.index(index.row(), 5), Qt::EditRole).toInt());      // id
        data_talk._id_person    = (model_talks.data(model_talks.index(index.row(), 0), Qt::EditRole).isNull() ? -1 :
                                   model_talks.data(model_talks.index(index.row(), 0), Qt::EditRole).toInt());      // _id_person
        data_talk._id_polis     = (model_talks.data(model_talks.index(index.row(), 1), Qt::EditRole).isNull() ? -1 :
                                   model_talks.data(model_talks.index(index.row(), 1), Qt::EditRole).toInt());      // id_polis
        data_talk._id_blank_pol = (model_talks.data(model_talks.index(index.row(), 2), Qt::EditRole).isNull() ? -1 :
                                   model_talks.data(model_talks.index(index.row(), 2), Qt::EditRole).toInt());      // _id_blank_pol
        data_talk.fio           = QString(data_talks.fio + " ");
        data_talk.type_talk     = model_talks.data(model_talks.index(index.row(), 6), Qt::EditRole).toInt();     // type_talk
        data_talk.date_talk     = model_talks.data(model_talks.index(index.row(), 7), Qt::EditRole).toDate();    // date_talk
        data_talk.pol_v         = model_talks.data(model_talks.index(index.row(), 3), Qt::EditRole).toInt();     // pol_v
        data_talk.result        = model_talks.data(model_talks.index(index.row(), 8), Qt::EditRole).toInt();     // result
        data_talk.comment       = QString(model_talks.data(model_talks.index(index.row(), 9), Qt::EditRole).toString() + " ");  // comment

        delete add_talk_w;
        add_talk_w = new add_talk_wnd(db, &data_talk, data_app, this);
        add_talk_w->exec();

        refresh_talks_tab();
    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Выберите запись звонка/визита застрахованного для правки.");
    }
}

void talks_wnd::on_talks_tab_activated(const QModelIndex &index) {
    on_bn_edit_clicked();
}

void talks_wnd::on_bn_delete_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->talks_tab || !ui->talks_tab->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Поиск] и выберите человека из списка.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->talks_tab->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this, "Нужно подтверждение", "Удалить выбранную запись звонка/визита из базы?\n",
                                  QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        QModelIndex index = indexes.front();
        db.transaction();

        QString sql = "delete from talks "
                      " where id=" + QString::number(model_talks.data(model_talks.index(index.row(), 5), Qt::EditRole).toInt()) + " ; ";
        QSqlQuery *query2 = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "удаление записи звонка/визита", *query2, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке удалить запись звонка/визита произошла неожиданная ошибка.\n\nУдаление отменено.");
            delete query2;
            return;
        }
        data_talks.id = -1;
        delete query2;

        db.commit();

        refresh_talks_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void talks_wnd::on_ch_other_clicked(bool checked) {
    ui->bn_copy_part->setVisible(false);
    ui->bn_copy_all->setVisible(checked);
    ui->talks_tab_2->setVisible(checked);
    ui->pan_up_2->setVisible(checked);

    if (checked) {
        refresh_persons();
    }
}

void talks_wnd::on_combo_person_currentIndexChanged(int index) {
    data_talks._id_person_2 = ui->combo_person->currentData().toInt();
    refresh_talks_tab_2();
}


void talks_wnd::refresh_persons() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка застрахованных
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select id, COALESCE(fam,'-')||' '||COALESCE(im,'-')||' '||COALESCE(ot,'-')||', '||date_birth || '  (ID=' || id || ')' as pers_fio "
                  " from persons s "
                  " where fam like('" + ui->ln_fam->text().trimmed().toUpper()+ "%') "
                  "   and im  like('" + ui->ln_im->text().trimmed().toUpper() + "%') "
                  "   and ot  like('" + ui->ln_ot->text().trimmed().toUpper() + "%') "
                  "   and (select * from st_person(s.id, NULL))>-100 "
                  " order by COALESCE(fam,'-')||' '||COALESCE(im,'-')||' '||COALESCE(ot,'-')||', '||date_birth || '  (ID=' || id || ')' ; ";
    mySQL.exec(this, sql, QString("список персон"), *query, true, db, data_app);
    ui->combo_person->clear();
    //persons_list.clear();
    while (query->next()) {
        int id = query->value(0).toInt();
        ui->combo_person->addItem(query->value(1).toString(), id);
        //persons_list.append(query->value(1).toString());
    }
    ui->combo_person->setCurrentIndex(0/*ui->combo_person->findData(data_talks._id_person_2)*/);
    delete query;

    refresh_talks_tab_2();
    this->setCursor(Qt::ArrowCursor);
}

void talks_wnd::on_ln_fam_editingFinished() {
    refresh_persons();
}

void talks_wnd::on_ln_im_editingFinished() {
    refresh_persons();
}

void talks_wnd::on_ln_ot_editingFinished() {
    refresh_persons();
}

void talks_wnd::refresh_talks_tab_2() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, "Нет доступа к базе данных",
                             "Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n "
                             "Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику.");
        return;
    }

    sql_talks_2 = "select t._id_person, t._id_polis, t._id_blank_pol, t.pol_v, "
                  "       case when t._id_polis<0 then ' - ? - ' "
                  "            when p.pol_v=2 then right('0000000000' || cast(p.vs_num as text), 11) "
                  "            when (p.pol_v=3 and p.enp is NULL) then p.pol_ser || ' №' || p.pol_num "
                  "            when (p.pol_v=3 and p.enp is not NULL) then p.pol_ser || ' №' || p.pol_num || ' ЕНП=' || p.enp "
                  "            else ' - полис не выдан - ' "
                  "       end as pol_id, "
                  "       t.id, t.type_talk, t.date_talk, t.result, t.comment "
                  "  from talks t "
                  "       left join polises p on(p.id=t._id_polis) "
                  "       left join blanks_pol bp on(bp.id=t._id_blank_pol) "
                  " where _id_person=" + QString::number(data_talks._id_person_2) + " "
                  " order by t.date_talk desc, p.pol_v desc ; ";

    model_talks_2.setQuery(sql_talks_2,db);
    QString err2 = model_talks_2.lastError().driverText();

    // подключаем модель из БД
    ui->talks_tab_2->setModel(&model_talks_2);

    // обновляем таблицу
    ui->talks_tab_2->reset();

    // задаём ширину колонок
    ui->talks_tab_2->setColumnWidth( 0,  1);     // t._id_person,
    ui->talks_tab_2->setColumnWidth( 1,  1);     // t._id_polis,
    ui->talks_tab_2->setColumnWidth( 2,  1);     // t._id_blank_pol,
    ui->talks_tab_2->setColumnWidth( 3, 60);     // p.pol_v,
    ui->talks_tab_2->setColumnWidth( 4,130);     // pol_id,
    ui->talks_tab_2->setColumnWidth( 5,  1);     // t.id,
    ui->talks_tab_2->setColumnWidth( 6,150);     // t.type_talk,
    ui->talks_tab_2->setColumnWidth( 7, 75);     // t.date_talk,
    ui->talks_tab_2->setColumnWidth( 8,200);     // t.result,
    ui->talks_tab_2->setColumnWidth( 9,200);     // t.comment
    // правим заголовки
    model_talks_2.setHeaderData( 3, Qt::Horizontal, ("тип"));
    model_talks_2.setHeaderData( 4, Qt::Horizontal, ("полис"));
    model_talks_2.setHeaderData( 6, Qt::Horizontal, ("форма общения"));
    model_talks_2.setHeaderData( 7, Qt::Horizontal, ("дата"));
    model_talks_2.setHeaderData( 8, Qt::Horizontal, ("результат"));
    model_talks_2.setHeaderData( 9, Qt::Horizontal, ("комментарий"));

    ui->talks_tab_2->repaint();

    this->setCursor(Qt::ArrowCursor);
}

void talks_wnd::on_bn_refresh_2_clicked() {
    refresh_talks_tab_2();
}

void talks_wnd::on_bn_copy_all_clicked() {
    QSqlQuery *query_data = new QSqlQuery(db);
    query_data->exec(sql_talks);
    while (query_data->next()) {
        QSqlRecord rec = query_data->record();
        int _id_person    = query_data->value(0).toInt();
        int id_polis     = query_data->value(1).toInt();
        int _id_blank_pol = query_data->value(2).toInt();
        int pol_v         = query_data->value(3).toInt();
        int id            = query_data->value(5).toInt();
        int type_talk     = query_data->value(6).toInt();
        QDate date_talk   = query_data->value(7).toDate();
        int result        = query_data->value(8).toInt();
        QString comment   = query_data->value(9).toString();

        QString sql_add = "insert into talks(pol_v, type_talk, date_talk, result, comment, _id_person, id_polis, _id_blank_pol) "
                          " values( " + QString::number(-1) + ", "
                                  " " + QString::number(type_talk) + ", "
                                  " '" + date_talk.toString("yyyy-MM-dd") + "', "
                                  " " + QString::number(result) + ", "
                                  " '" + comment + "', "
                                  " " + QString::number(data_talks._id_person_2) + ", "
                                  " " + QString::number(-1) + ", "
                                  " " + QString::number(-1) + " ) ; ";
        QSqlQuery *query_add = new QSqlQuery(db);
        if (!query_add->exec(sql_add)) {
            delete query_add;
            delete query_data;
            QMessageBox::warning(this, "Ошибка копирования данных",
                                 "При попытке скопировать запись звонка/визита произошла ошибка.\n"
                                 "Операция прервана.\n\n"
                                 "SQL:\n" + sql_add);
            return;
        }
        delete query_add;
    }
    delete query_data;

    refresh_talks_tab_2();
}

