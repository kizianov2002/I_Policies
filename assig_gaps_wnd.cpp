#include "assig_gaps_wnd.h"
#include "ui_assig_gaps_wnd.h"

assig_gaps_wnd::assig_gaps_wnd(QSqlDatabase &db, QSqlDatabase &db_FIAS, s_data_app &data_app, QSettings &settings, s_data_assig &data_assig, QWidget *parent)
 : db(db), db_FIAS(db_FIAS), data_app(data_app), settings(settings), data_assig(data_assig), QDialog(parent), ui(new Ui::assig_gaps_wnd)
{
    ui->setupUi(this);

    refresh_combo_subj();
    refresh_combo_dstr();
    refresh_combo_city();
    refresh_combo_nasp();
    refresh_combo_strt();

    refresh_gaps_tab();
}

assig_gaps_wnd::~assig_gaps_wnd() {
    delete ui;
}

void assig_gaps_wnd::on_bn_exit_clicked() {
    close();
}


void assig_gaps_wnd::refresh_combo_subj() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка регионов
    QSqlQuery *query = new QSqlQuery(db_FIAS);
    QString sql = "select cast(regioncode as smallint), offname||' '||shortname "
                  "  from fias.addrobj "
                  " where aolevel=1 "
                  "   and currstatus=0 "
                  " order by offname||' '||shortname ; ";
    query->exec(sql);

    ui->combo_subj->clear();
    ui->combo_subj->addItem(" - все регионы - ", 0);
    while (query->next()) {
        ui->combo_subj->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    delete query;

    if (data_assig.subj==0) {
        ui->combo_subj->setCurrentIndex(0);
    } else {
        ui->combo_subj->setCurrentIndex(ui->combo_subj->findData(data_assig.subj, Qt::UserRole));
    }

    this->setCursor(Qt::ArrowCursor);
}

void assig_gaps_wnd::refresh_combo_dstr() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка районов выбранного региона
    QSqlQuery *query = new QSqlQuery(db_FIAS);
    QString sql = "select cast(areacode as smallint), offname||' '||shortname "
                  "  from fias.addrobj "
                  " where cast(regioncode as smallint)=" + QString::number(ui->combo_subj->currentData().toInt()) + " "
                  "   and aolevel=3 "
                  "   and currstatus=0 "
                  " order by offname||' '||shortname ";
    query->exec(sql);

    ui->combo_dstr->clear();
    ui->combo_dstr->addItem(" - все районы - ", 0);
    while (query->next()) {
        ui->combo_dstr->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_dstr->setCurrentIndex(ui->combo_dstr->findData(data_assig.dstr, Qt::UserRole));
    delete query;

    if (data_assig.dstr==0) {
        ui->combo_dstr->setCurrentIndex(0);
    } else {
        ui->combo_dstr->setCurrentIndex(ui->combo_dstr->findData(data_assig.dstr, Qt::UserRole));
    }

    this->setCursor(Qt::ArrowCursor);
}

void assig_gaps_wnd::refresh_combo_city() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка городов выбранного района
    QSqlQuery *query = new QSqlQuery(db_FIAS);
    QString sql = "select cast(citycode as smallint), offname||' '||shortname "
                  "  from fias.addrobj "
                  " where cast(regioncode as smallint)=" + QString::number(ui->combo_subj->currentData().toInt()) + " "
                  "   and cast(areacode as smallint)=" + QString::number(ui->combo_dstr->currentData().toInt()) + " "
                  "   and aolevel=4 "
                  "   and currstatus=0 "
                  " order by offname||' '||shortname ";
    query->exec(sql);

    ui->combo_city->clear();
    ui->combo_city->addItem(" - все города - ", 0);
    while (query->next()) {
        ui->combo_city->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_city->setCurrentIndex(ui->combo_city->findData(data_assig.city, Qt::UserRole));
    delete query;

    if (data_assig.city==0) {
        ui->combo_city->setCurrentIndex(0);
    } else {
        ui->combo_city->setCurrentIndex(ui->combo_city->findData(data_assig.city, Qt::UserRole));
    }

    this->setCursor(Qt::ArrowCursor);
}

void assig_gaps_wnd::refresh_combo_nasp() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка нас.пунктов выбранного горда
    QSqlQuery *query = new QSqlQuery(db_FIAS);
    QString sql = "select cast(placecode as smallint), offname||' '||shortname "
                  "  from fias.addrobj "
                  " where cast(regioncode as smallint)=" + QString::number(ui->combo_subj->currentData().toInt()) + " "
                  "   and cast(areacode as smallint)=" + QString::number(ui->combo_dstr->currentData().toInt()) + " "
                  "   and cast(citycode as smallint)=" + QString::number(ui->combo_city->currentData().toInt()) + " "
                  "   and aolevel=6 "
                  "   and currstatus=0 "
                  " order by offname||' '||shortname ";
    query->exec(sql);

    ui->combo_nasp->clear();
    ui->combo_nasp->addItem(" - все нас.пункты - ", 0);
    while (query->next()) {
        ui->combo_nasp->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_nasp->setCurrentIndex(ui->combo_nasp->findData(data_assig.nasp, Qt::UserRole));
    delete query;

    if (data_assig.nasp==0) {
        ui->combo_nasp->setCurrentIndex(0);
    } else {
        ui->combo_nasp->setCurrentIndex(ui->combo_nasp->findData(data_assig.nasp, Qt::UserRole));
    }

    this->setCursor(Qt::ArrowCursor);
}

void assig_gaps_wnd::refresh_combo_strt() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка улиц выбранного населённого пункта
    QSqlQuery *query = new QSqlQuery(db_FIAS);
    QString sql = "select cast(streetcode as smallint), offname||' '||shortname "
                  "  from fias.addrobj "
                  " where cast(regioncode as smallint)=" + QString::number(ui->combo_subj->currentData().toInt()) + " "
                  "   and cast(areacode as smallint)=" + QString::number(ui->combo_dstr->currentData().toInt()) + " "
                  "   and cast(citycode as smallint)=" + QString::number(ui->combo_city->currentData().toInt()) + " "
                  "   and cast(placecode as smallint)=" + QString::number(ui->combo_nasp->currentData().toInt()) + " "
                  "   and aolevel=6 "
                  "   and currstatus=0 "
                  " order by offname||' '||shortname ";
    query->exec(sql);

    ui->combo_strt->clear();
    ui->combo_strt->addItem(" - все улицы - ", 0);
    while (query->next()) {
        ui->combo_strt->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_strt->setCurrentIndex(ui->combo_strt->findData(data_assig.strt, Qt::UserRole));
    delete query;

    if (data_assig.strt==0) {
        ui->combo_strt->setCurrentIndex(0);
    } else {
        ui->combo_strt->setCurrentIndex(ui->combo_strt->findData(data_assig.strt, Qt::UserRole));
    }

    this->setCursor(Qt::ArrowCursor);
}

void assig_gaps_wnd::on_bn_refresh_gaps_clicked() {
    refresh_gaps_tab();
}

void assig_gaps_wnd::on_combo_subj_activated(int index) {
    data_assig.subj = ui->combo_subj->currentData().toInt();
    refresh_combo_dstr();
    refresh_combo_city();
    refresh_combo_nasp();
    refresh_combo_strt();
    refresh_gaps_tab();
}

void assig_gaps_wnd::on_combo_dstr_activated(int index) {
    data_assig.dstr = ui->combo_dstr->currentData().toInt();
    refresh_combo_city();
    refresh_combo_nasp();
    refresh_combo_strt();
    refresh_gaps_tab();
}

void assig_gaps_wnd::on_combo_city_activated(int index) {
    data_assig.city = ui->combo_city->currentData().toInt();
    refresh_combo_nasp();
    refresh_combo_strt();
    refresh_gaps_tab();
}

void assig_gaps_wnd::on_combo_nasp_activated(int index) {
    data_assig.nasp = ui->combo_nasp->currentData().toInt();
    refresh_combo_strt();
    /*data_assig.subj    = data_assig.subj;
    data_assig.dstr    = data_assig.dstr;
    data_assig.city    = data_assig.city;
    data_assig.nasp    = data_assig.nasp;
    data_assig.strt    = data_assig.strt;*/

    refresh_gaps_tab();
}

void assig_gaps_wnd::on_combo_strt_activated(int index) {
    data_assig.strt = ui->combo_strt->currentData().toInt();
    /*data_assig.subj    = data_assig.subj;
    data_assig.dstr    = data_assig.dstr;
    data_assig.city    = data_assig.city;
    data_assig.nasp    = data_assig.nasp;
    data_assig.strt    = data_assig.strt;*/

    refresh_gaps_tab();
}

void assig_gaps_wnd::on_bn_clear_filter_gaps_clicked() {
    ui->combo_subj->setCurrentIndex(0);
    ui->combo_dstr->setCurrentIndex(0);
    ui->combo_city->setCurrentIndex(0);
    ui->combo_nasp->setCurrentIndex(0);
    ui->combo_strt->setCurrentIndex(0);
    ui->combo_age->setCurrentIndex(0);
    ui->combo_sex->setCurrentIndex(0);

    refresh_gaps_tab();
}

void assig_gaps_wnd::refresh_gaps_tab() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    this->setCursor(Qt::WaitCursor);
    QApplication::processEvents();

    QString gaps_sql;
    gaps_sql  = "select a.id, a.ocato, " +
                ( ui->ch_show_names->isChecked()
                  ? (" a.subj, fias_textname('" + data_app.FIAS_db_connstring + "', a.subj, 0, 0, 0, 0), "
                     " a.dstr, fias_textname('" + data_app.FIAS_db_connstring + "', a.subj, a.dstr, 0, 0, 0), "
                     " a.city, fias_textname('" + data_app.FIAS_db_connstring + "', a.subj, a.dstr, a.city, 0, 0), "
                     " a.nasp, fias_textname('" + data_app.FIAS_db_connstring + "', a.subj, a.dstr, a.city, a.nasp, 0), "
                     " a.strt, fias_textname('" + data_app.FIAS_db_connstring + "', a.subj, a.dstr, a.city, a.nasp, a.strt), ")
                  : (" a.subj, NULL, "
                     " a.dstr, NULL, "
                     " a.city, NULL, "
                     " a.nasp, NULL, "
                     " a.strt, NULL, ") ) +
                "       a.house, a.corp, "
                "       a.sex, a.age "
                " from assig_by_terr_gaps a " ;
    bool is_first = true;
    if (ui->combo_subj->currentIndex()>0) {
        if (is_first) {
            gaps_sql += " where ";
            is_first = false;
        } else {
            gaps_sql += " and ";
        }
        gaps_sql += " a.subj=" + QString::number(ui->combo_subj->currentData().toInt()) + " ";
    }
    if (ui->combo_dstr->currentIndex()>0) {
        if (is_first) {
            gaps_sql += " where ";
            is_first = false;
        } else {
            gaps_sql += " and ";
        }
        gaps_sql += " a.dstr=" + QString::number(ui->combo_dstr->currentData().toInt()) + " ";
    }
    if (ui->combo_city->currentIndex()>0) {
        if (is_first) {
            gaps_sql += " where ";
            is_first = false;
        } else {
            gaps_sql += " and ";
        }
        gaps_sql += " a.city=" + QString::number(ui->combo_city->currentData().toInt()) + " ";
    }
    if (ui->combo_nasp->currentIndex()>0) {
        if (is_first) {
            gaps_sql += " where ";
            is_first = false;
        } else {
            gaps_sql += " and ";
        }
        gaps_sql += " a.nasp=" + QString::number(ui->combo_nasp->currentData().toInt()) + " ";
    }
    if (ui->combo_strt->currentIndex()>0) {
        if (is_first) {
            gaps_sql += " where ";
            is_first = false;
        } else {
            gaps_sql += " and ";
        }
        gaps_sql += " a.strt=" + QString::number(ui->combo_strt->currentData().toInt()) + " ";
    }
    if (ui->combo_age->currentIndex()>0) {
        if (is_first) {
            gaps_sql += " where ";
            is_first = false;
        } else {
            gaps_sql += " and ";
        }
        gaps_sql += " a.age=" + QString::number(ui->combo_age->currentData().toInt()) + " ";
    }
    if (ui->combo_sex->currentIndex()>0) {
        if (is_first) {
            gaps_sql += " where ";
            is_first = false;
        } else {
            gaps_sql += " and ";
        }
        gaps_sql += " a.sex=" + QString::number(ui->combo_sex->currentData().toInt()) + " ";
    }
    if (ui->ch_show_names->isChecked()) {
        gaps_sql += " order by fias_textname('" + data_app.FIAS_db_connstring + "', a.subj, 0, 0, 0, 0), "
                             " fias_textname('" + data_app.FIAS_db_connstring + "', a.subj, a.dstr, 0, 0, 0), "
                             " fias_textname('" + data_app.FIAS_db_connstring + "', a.subj, a.dstr, a.city, 0, 0), "
                             " fias_textname('" + data_app.FIAS_db_connstring + "', a.subj, a.dstr, a.city, a.nasp, 0), "
                             " fias_textname('" + data_app.FIAS_db_connstring + "', a.subj, a.dstr, a.city, a.nasp, a.strt) ; ";
    } else {
        gaps_sql += " order by a.subj, a.dstr, a.city, a.nasp, a.strt, a.house, a.corp ; ";
    }

    data_assig.subj = ui->combo_subj->currentData().toInt();
    data_assig.dstr = ui->combo_dstr->currentData().toInt();
    data_assig.city = ui->combo_city->currentData().toInt();
    data_assig.nasp = ui->combo_nasp->currentData().toInt();
    data_assig.strt = ui->combo_strt->currentData().toInt();

    model_gaps.setQuery(gaps_sql,db);
    QString err2 = model_gaps.lastError().driverText();

    // подключаем модель из БД
    ui->tab_gaps->setModel(&model_gaps);

    // обновляем таблицу
    ui->tab_gaps->reset();

    // задаём ширину колонок
    ui->tab_gaps->setColumnWidth(  0,  1);     // id
    ui->tab_gaps->setColumnWidth(  1,  2);     // ocato
    if (ui->ch_show_names->isChecked()) {
        ui->tab_gaps->setColumnWidth(  2,  2);     // subj
        ui->tab_gaps->setColumnWidth(  3,150);     // subj_str
        ui->tab_gaps->setColumnWidth(  4,  2);     // dstr
        ui->tab_gaps->setColumnWidth(  5,150);     // dstr_str
        ui->tab_gaps->setColumnWidth(  6,  2);     // city
        ui->tab_gaps->setColumnWidth(  7,150);     // city_str
        ui->tab_gaps->setColumnWidth(  8,  2);     // nasp
        ui->tab_gaps->setColumnWidth(  9,180);     // nasp_str
        ui->tab_gaps->setColumnWidth( 10,  2);     // strt
        ui->tab_gaps->setColumnWidth( 11,180);     // strt_str
    } else {
        ui->tab_gaps->setColumnWidth(  2, 40);     // subj
        ui->tab_gaps->setColumnWidth(  3,  2);     // subj_str
        ui->tab_gaps->setColumnWidth(  4, 40);     // dstr
        ui->tab_gaps->setColumnWidth(  5,  2);     // dstr_str
        ui->tab_gaps->setColumnWidth(  6, 40);     // city
        ui->tab_gaps->setColumnWidth(  7,  2);     // city_str
        ui->tab_gaps->setColumnWidth(  8, 40);     // nasp
        ui->tab_gaps->setColumnWidth(  9,  2);     // nasp_str
        ui->tab_gaps->setColumnWidth( 10, 40);     // strt
        ui->tab_gaps->setColumnWidth( 11,  2);     // strt_str
    }\
    ui->tab_gaps->setColumnWidth( 12, 50);     // house
    ui->tab_gaps->setColumnWidth( 13, 50);     // corp
    ui->tab_gaps->setColumnWidth( 14, 40);     // sex
    ui->tab_gaps->setColumnWidth( 15, 40);     // age
    // правим заголовки
    model_gaps.setHeaderData(  1, Qt::Horizontal, ("ОКАТО"));
    model_gaps.setHeaderData(  3, Qt::Horizontal, ("регион"));
    model_gaps.setHeaderData(  5, Qt::Horizontal, ("район/улус"));
    model_gaps.setHeaderData(  7, Qt::Horizontal, ("город"));
    model_gaps.setHeaderData(  9, Qt::Horizontal, ("нас.пункт"));
    model_gaps.setHeaderData( 11, Qt::Horizontal, ("улица"));
    model_gaps.setHeaderData( 12, Qt::Horizontal, ("дом"));
    model_gaps.setHeaderData( 13, Qt::Horizontal, ("корп."));
    model_gaps.setHeaderData( 14, Qt::Horizontal, ("пол"));
    model_gaps.setHeaderData( 15, Qt::Horizontal, ("возр."));
    ui->tab_gaps->repaint();

    refresh_combo_subj();
    refresh_combo_dstr();
    refresh_combo_city();
    refresh_combo_nasp();
    refresh_combo_strt();

    this->setCursor(Qt::ArrowCursor);
}

void assig_gaps_wnd::on_combo_age_activated(int index) {
    refresh_gaps_tab();
}

void assig_gaps_wnd::on_combo_sex_activated(int index) {
    refresh_gaps_tab();
}

void assig_gaps_wnd::on_ch_show_names_clicked(bool checked) {
    if ( checked
         && QMessageBox::question(this, "Нукжно подтверждение",
                                  "Включить отображение названий адресных элементов?\n\n"
                                  "Это сильно замедлит отображение таблицы.",
                                  QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel)==QMessageBox::Yes) {
        refresh_gaps_tab();
        return;
    } else {
        ui->ch_show_names->setChecked(false);
        refresh_gaps_tab();
    }
}

void assig_gaps_wnd::on_bn_remove_clicked() {
    if (QMessageBox::question(this,
                              "Нужно подтверждение",
                              "Внимание!\nВы действительно хотите удалить выбранные строки?.\n\n"
                              "Строки ошибок прикрепления автоматически удаляются если прикрепление по соответствующему адресу прошло успешно. \n"
                              "Если же при новой попытке прикрепление опять не сработает - то строка будет добавлена снова.\n",
                              QMessageBox::Yes|QMessageBox::Cancel,
                              QMessageBox::Yes)==QMessageBox::Cancel) {
        return;
    }

    // удалим выделенные
    QModelIndexList list = ui->tab_gaps->selectionModel()->selectedIndexes();

    if (list.count()>0) {
        db.transaction();
        for (int i=0; i<list.count(); i++) {
            QModelIndex index = list.at(i);
            if (index.column()!=0)
                continue;

            int id = model_gaps.data(model_gaps.index(index.row(), 0), Qt::EditRole).toInt();

            QString sql_del = "delete from assig_by_terr_gaps d "
                              " where id=" + QString::number(id) + " ; ";
            QSqlQuery *query_del = new QSqlQuery(db);
            bool res_del = query_del->exec(sql_del);
            delete query_del;
        }
        db.commit();
        refresh_gaps_tab();
    } else {
        QMessageBox::information(this, "Ничего не выбрано", "Ничего не выбрано.\n\nНечего удалять.");
    }
}
