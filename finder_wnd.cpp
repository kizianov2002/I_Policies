#include "finder_wnd.h"
#include "ui_finder_wnd.h"
#include "persons_wnd.h"

finder_wnd::finder_wnd(QSqlDatabase &db, QSqlDatabase &db_FIAS, s_data_app &data_app, QSettings &settings, /*persons_wnd *persons_w,*/ QWidget *parent)
    : db(db), db_FIAS(db_FIAS), data_app(data_app), QDialog(parent), settings(settings),/* persons_w(persons_w),*/ ui(new Ui::finder_wnd)
{
    ui->setupUi(this);

    ui->tabWidget->setCurrentIndex(0);

    refresh_fias_address(0,0,0,0,0);
    refresh_docs_pan_type();
}

finder_wnd::~finder_wnd() {
    delete ui;
}

void finder_wnd::on_finder_wnd_accepted() {
    // обновим таблицу персон
    ((persons_wnd*)parent())->finder_off();
}
void finder_wnd::on_finder_wnd_finished(int result) {
    // обновим таблицу персон
    ((persons_wnd*)parent())->finder_off();
}
void finder_wnd::on_finder_wnd_rejected() {
    // обновим таблицу персон
    ((persons_wnd*)parent())->finder_off();
}

void finder_wnd::refresh_docs_pan_type() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка типов документов
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, '('||code||') '||text, mask_ser, mask_num "
                  " from public.spr_f011 s "
                  " order by code ; ";
    mySQL.exec(this, sql, QString("Справочник типов документов"), *query, true, db, data_app);
    ui->combo_doc_type->clear();
    ui->combo_doc_type->addItem(" - выберите тип документа - ", 0);
    while (query->next()) {
        ui->combo_doc_type->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_doc_type->setCurrentIndex(0);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void finder_wnd::refresh_fias_address(int subj, int dstr, int city, int nasp, int strt) {
    QApplication::processEvents();

    this->setCursor(Qt::WaitCursor);

    QString regioncode = subj>=0 ? QString("00" + QString::number(subj)).right(2)
                                 : QString("00" + QString::number(ui->combo_subj->currentData().toInt())).right(2);
    QString areacode   = dstr>=0 ? QString("000" + QString::number(dstr)).right(3)
                                 : QString("000" + QString::number(ui->combo_dstr->currentData().toInt())).right(3);
    QString citycode   = city>=0 ? QString("000" + QString::number(city)).right(3)
                                 : QString("000" + QString::number(ui->combo_city->currentData().toInt())).right(3);
    QString placecode  = nasp>=0 ? QString("000" + QString::number(nasp)).right(3)
                                 : QString("000" + QString::number(ui->combo_nasp->currentData().toInt())).right(3);
    QString streetcode = strt>=0 ? QString("0000" + QString::number(strt)).right(4)
                                 : QString("0000" + QString::number(ui->combo_strt->currentData().toInt())).right(4);

    // регион регистрации
    if (subj>=0) {
        // обновление выпадающего списка регионов
        QPalette pal = ui->lab_subj->palette();
        pal.setColor(QPalette::Text, QColor(0,0,255));
        ui->lab_subj->setPalette(pal);
        QFont font = ui->lab_subj->font();
        font.setBold(true);
        ui->lab_subj->setFont(font);
        QApplication::processEvents();

        ui->combo_subj->clear();
        ui->combo_subj->addItem(" - нет - ", " -//- ");

        QSqlQuery *query = new QSqlQuery(db_FIAS);
        QString sq_ = "select s.regioncode, formalname||' '||shortname as name, RIGHT('00000000000'||TRIM(okato),11) as okato, code, trim(postalcode) as post_index, aoguid, aoid "
                      "  from fias.addrobj s "
                      " where s.aolevel=1 "
                      "   and currstatus=0 "
                      " order by s.formalname ; ";
        query->exec( sq_ );
        while (query->next()) {
            ui->combo_subj->addItem(query->value(1).toString(), query->value(0).toString());
        }
        delete query;
        // зададим текущее значение
        if (subj>0) ui->combo_subj->setCurrentIndex(ui->combo_subj->findData(regioncode));
        else        ui->combo_subj->setCurrentIndex(0);

        pal = ui->lab_subj->palette();
        pal.setColor(QPalette::Text, QColor(0,0,0));
        ui->lab_subj->setPalette(pal);
        font = ui->lab_subj->font();
        font.setBold(false);
        ui->lab_subj->setFont(font);
        QApplication::processEvents();
    }
    if (ui->combo_subj->currentIndex()<0)
        ui->combo_subj->setCurrentIndex(0);

    //район регистрации
    if (dstr>=0) {
        // обновление выпадающего списка районов
        QPalette pal = ui->lab_dstr->palette();
        pal.setColor(QPalette::Text, QColor(0,0,255));
        ui->lab_dstr->setPalette(pal);
        QFont font = ui->lab_dstr->font();
        font.setBold(true);
        ui->lab_dstr->setFont(font);
        QApplication::processEvents();

        ui->combo_dstr->clear();
        ui->combo_dstr->addItem(" - нет - ", " -//- ");

        if (subj!=0) {
            QSqlQuery *query = new QSqlQuery(db_FIAS);
            QString sq_ = "select s.areacode, formalname||' '||shortname as name, RIGHT('00000000000'||TRIM(okato),11) as okato, code, trim(postalcode) as post_index, aoguid, aoid "
                          "  from fias.addrobj s "
                          " where s.regioncode='" + regioncode + "' "
                          "   and s.aolevel=3 "
                          "   and currstatus=0 "
                          " order by s.formalname ; ";
            query->exec( sq_ );
            while (query->next()) {
                ui->combo_dstr->addItem(query->value(1).toString(), query->value(0).toString());
            }
            delete query;
        }
        // зададим текущее значение
        if (dstr>0) ui->combo_dstr->setCurrentIndex(ui->combo_dstr->findData(areacode));
        else        ui->combo_dstr->setCurrentIndex(0);

        pal = ui->lab_dstr->palette();
        pal.setColor(QPalette::Text, QColor(0,0,0));
        ui->lab_dstr->setPalette(pal);
        font = ui->lab_dstr->font();
        font.setBold(false);
        ui->lab_dstr->setFont(font);
        QApplication::processEvents();
    }
    if (ui->combo_dstr->currentIndex()<0)
        ui->combo_dstr->setCurrentIndex(0);

    // город регистрации
    if (city>=0) {
        QPalette pal = ui->lab_city->palette();
        pal.setColor(QPalette::Text, QColor(0,0,255));
        ui->lab_city->setPalette(pal);
        QFont font = ui->lab_city->font();
        font.setBold(true);
        ui->lab_city->setFont(font);
        QApplication::processEvents();

        ui->combo_city->clear();
        ui->combo_city->addItem(" - нет - ", " -//- ");

        if (subj!=0) {
            // обновление выпадающего списка городов
            QSqlQuery *query = new QSqlQuery(db_FIAS);
            QString sq_ = "select s.citycode, formalname||' '||shortname as name, RIGHT('00000000000'||TRIM(okato),11) as okato, code, trim(postalcode) as post_index, aoguid, aoid "
                          "  from fias.addrobj s "
                          " where s.regioncode='" + regioncode + "' "
                          "   and s.areacode='"   + areacode   + "' "
                          "   and s.aolevel=4 "
                          "   and currstatus=0 "
                          " order by s.formalname ; ";
            query->exec( sq_ );
            while (query->next()) {
                ui->combo_city->addItem(query->value(1).toString(), query->value(0).toString());
            }
            delete query;
        }
        // зададим текущее значение
        if (city>0) ui->combo_city->setCurrentIndex(ui->combo_city->findData(citycode));
        else        ui->combo_city->setCurrentIndex(0);

        pal = ui->lab_city->palette();
        pal.setColor(QPalette::Text, QColor(0,0,0));
        ui->lab_city->setPalette(pal);
        font = ui->lab_city->font();
        font.setBold(false);
        ui->lab_city->setFont(font);
        QApplication::processEvents();
    }
    if (ui->combo_city->currentIndex()<0)
        ui->combo_city->setCurrentIndex(0);

    // нас.пункт регистрации
    if (nasp>=0) {
        QPalette pal = ui->lab_nasp->palette();
        pal.setColor(QPalette::Text, QColor(0,0,255));
        ui->lab_nasp->setPalette(pal);
        QFont font = ui->lab_nasp->font();
        font.setBold(true);
        ui->lab_nasp->setFont(font);
        QApplication::processEvents();

        ui->combo_nasp->clear();
        ui->combo_nasp->addItem(" - нет - ", " -//- ");

        if (subj!=0) {
            // обновление выпадающего списка нас.пунктов
            QString sq_ = "select s.placecode, formalname||' '||shortname as name, RIGHT('00000000000'||TRIM(okato),11) as okato, code, trim(postalcode) as post_index, aoguid, aoid "
                          "  from fias.addrobj s "
                          " where s.regioncode='" + regioncode + "' "
                          "   and s.areacode='"   + areacode   + "' "
                          "   and s.citycode='"   + citycode   + "' "
                          "   and s.aolevel=6 "
                          "   and currstatus=0 "
                          " order by s.formalname ; ";
            QSqlQuery *query = new QSqlQuery(db_FIAS);
            query->exec( sq_ );
            while (query->next()) {
                ui->combo_nasp->addItem(query->value(1).toString(), query->value(0).toString());
            }
            delete query;
        }
        // зададим текущее значение
        if (nasp>0) ui->combo_nasp->setCurrentIndex(ui->combo_nasp->findData(placecode));
        else        ui->combo_nasp->setCurrentIndex(0);

        pal = ui->lab_nasp->palette();
        pal.setColor(QPalette::Text, QColor(0,0,0));
        ui->lab_nasp->setPalette(pal);
        font = ui->lab_nasp->font();
        font.setBold(false);
        ui->lab_nasp->setFont(font);
        QApplication::processEvents();
    }
    if (ui->combo_nasp->currentIndex()<0)
        ui->combo_nasp->setCurrentIndex(0);

    // улица регистрации
    if (strt>=0) {
        QPalette pal = ui->lab_strt->palette();
        pal.setColor(QPalette::Text, QColor(0,0,255));
        ui->lab_strt->setPalette(pal);
        QFont font = ui->lab_strt->font();
        font.setBold(true);
        ui->lab_strt->setFont(font);
        QApplication::processEvents();

        ui->combo_strt->clear();
        ui->combo_strt->addItem(" - нет - ", " -//- ");

        if (subj!=0) {
            // обновление выпадающего списка улиц
            QSqlQuery *query = new QSqlQuery(db_FIAS);
            QString sq_ = "select s.streetcode, formalname||' '||shortname as name, RIGHT('00000000000'||TRIM(okato),11) as okato, code, trim(postalcode) as post_index, aoguid, aoid "
                          "  from fias.addrobj s "
                          " where s.regioncode='" + regioncode + "' "
                          "   and s.areacode='"   + areacode   + "' "
                          "   and s.citycode='"   + citycode   + "' "
                          "   and s.placecode='"  + placecode  + "' "
                          "   and s.aolevel=7 "
                          "   and currstatus=0 "
                          " order by s.formalname ; ";
            query->exec( sq_ );
            while (query->next()) {
                ui->combo_strt->addItem(query->value(1).toString(), query->value(0).toString());
            }
            delete query;
        }
        // зададим текущее значение
        int i = ui->combo_strt->findData(streetcode,Qt::UserRole,Qt::MatchExactly);
        //int i = ui->combo_strt->findData(strt,Qt::UserRole,Qt::MatchExactly);
        QString str = ui->combo_strt->itemText(i);
        if (strt>0) ui->combo_strt->setCurrentIndex(i);
        else        ui->combo_strt->setCurrentIndex(0);

        pal = ui->lab_strt->palette();
        pal.setColor(QPalette::Text, QColor(0,0,0));
        ui->lab_strt->setPalette(pal);
        font = ui->lab_strt->font();
        font.setBold(false);
        ui->lab_strt->setFont(font);
        QApplication::processEvents();
    }
    if (ui->combo_strt->currentIndex()<0)
        ui->combo_strt->setCurrentIndex(0);

    this->setCursor(Qt::ArrowCursor);

    QApplication::processEvents();
}

void finder_wnd::on_combo_subj_activated(int index) {
    refresh_fias_address(-1,0,0,0,0);
}
void finder_wnd::on_combo_dstr_activated(int index) {
    refresh_fias_address(-1,-1,0,0,0);
}
void finder_wnd::on_combo_city_activated(int index) {
    refresh_fias_address(-1,-1,-1,0,0);
}
void finder_wnd::on_combo_nasp_activated(int index) {
    refresh_fias_address(-1,-1,-1,-1,0);
}
void finder_wnd::on_combo_strt_activated(int index) {
    refresh_fias_address(-1,-1,-1,-1,-1);
}

void finder_wnd::on_bn_close_clicked() {
    reject();
}

void finder_wnd::on_tabWidget_currentChanged(int index) {
    QString capt = "Поиск\n";
    switch (ui->tabWidget->currentIndex()) {
    case 0:
        capt += "по ВС / полису";
        break;
    case 1:
        capt += "по адресу";
        break;
    case 2:
        capt += "по документу";
        break;
    case 3:
        capt += "по перс.данным";
        break;
    case 4:
        capt += "по контактным данным";
        break;
    case 5:
        capt += "по ID записей";
        break;
    default:
        capt += "???";
        break;
    }
    ui->bn_find->setText(capt);
}

void finder_wnd::on_line_vs_num_editingFinished() {
    ui->ch_vs_num->setChecked(!ui->line_vs_num->text().trimmed().isEmpty());
}
void finder_wnd::on_line_pol_ser_editingFinished() {
    ui->ch_polis->setChecked(!ui->line_pol_ser->text().trimmed().isEmpty() || !ui->line_pol_num->text().trimmed().isEmpty());
}
void finder_wnd::on_line_pol_num_editingFinished() {
    ui->ch_polis->setChecked(!ui->line_pol_ser->text().trimmed().isEmpty() || !ui->line_pol_num->text().trimmed().isEmpty());
}
void finder_wnd::on_line_enp_editingFinished() {
    ui->ch_enp->setChecked(!ui->line_enp->text().trimmed().isEmpty());
}
void finder_wnd::on_combo_doc_type_activated(const QString &arg1) {
    ui->ch_doc_type->setChecked(true);
}
void finder_wnd::on_line_doc_sernum_editingFinished() {
    ui->ch_doc_sernum->setChecked( !ui->line_doc_ser->text().trimmed().isEmpty()
                                   || !ui->line_doc_num->text().trimmed().isEmpty() );
}
void finder_wnd::on_date_doc_start_editingFinished() {
    ui->ch_doc_start->setChecked(true);
}
void finder_wnd::on_date_doc_exp_editingFinished() {
    ui->ch_doc_exp->setChecked(true);
}
void finder_wnd::on_line_doc_ser_editingFinished() {
    ui->ch_doc_sernum->setChecked(!ui->line_doc_ser->text().trimmed().isEmpty() || !ui->line_doc_num->text().trimmed().isEmpty());
}
void finder_wnd::on_line_doc_num_editingFinished() {
    ui->ch_doc_sernum->setChecked(!ui->line_doc_ser->text().trimmed().isEmpty() || !ui->line_doc_num->text().trimmed().isEmpty());
}


void finder_wnd::on_line_phone_editingFinished() {
    ui->line_phone->setText(ui->line_phone->text().simplified().trimmed().replace(" ","").replace("-","").replace("(","").replace(")","").replace("[","").replace("]","").replace("+7","8"));
    ui->ch_phone->setChecked(!ui->line_phone->text().isEmpty());
}
void finder_wnd::on_line_email_editingFinished() {
    ui->line_email->setText(ui->line_email->text().simplified().trimmed().replace(" ","").replace("(","").replace(")","").replace("[","").replace("]",""));
    ui->ch_email->setChecked(!ui->line_email->text().trimmed().isEmpty());
}

void finder_wnd::on_bn_find_clicked() {
    // сформируем пул выбранных записей
    int tab_num = ui->tabWidget->currentIndex();
    switch (tab_num) {
    case 0:
        if ( ui->ch_vs_num->isChecked()
             || ui->ch_polis->isChecked()
             || ui->ch_enp->isChecked() ) {
            QString sql = "delete from persons_find ; "
                          "insert into persons_find(id_person, find_flag) "
                          "select distinct e.id, 1 "
                          "  from persons e "
                          "  left join polises p on(e.id=p.id_person) ";
            bool is_first = true;
            if (ui->ch_vs_num->isChecked()) {
                sql += QString(is_first ? " where " : " and ") + " cast(p.vs_num as text) like('" + ui->line_vs_num->text().trimmed() + "%') ";
                is_first = false;
            }
            if (ui->ch_polis->isChecked()) {
                sql += QString(is_first ? " where " : " and ") + " p.pol_ser like('" + ui->line_pol_ser->text().trimmed() + "%') "
                                                      " and "  + " p.pol_num like('" + ui->line_pol_num->text().trimmed() + "%') ";
                is_first = false;
            }
            if (ui->ch_enp->isChecked()) {
                sql += QString(is_first ? " where " : " and ") + " p.enp like('" + ui->line_enp->text().trimmed() + "%') ";
                is_first = false;
            }

            QSqlQuery *query = new QSqlQuery(db);
            if (!mySQL.exec(this, sql, "Построение фильтра персон", *query, true, db, data_app)) {
                QMessageBox::critical(this, "Непредвиденная ошибка", "При построить фильтр персон произошла неожиданная ошибка.\n\nОперация отменена.");
                delete query;
                return;
            }
            delete query;
        } else {
            QMessageBox::warning(this, "Выберите условие отбора", "Выберите условие отбора");
        }
        break;
    case 1:
        if ( ui->ch_addr_reg->isChecked()
             || ui->ch_addr_liv->isChecked() ) {
            QString sql = "delete from persons_find ; "
                          "insert into persons_find(id_person, find_flag)"
                          "select distinct e.id, 1 "
                          "  from persons e "
                          "  left join addresses ar on(e.id_addr_reg=ar.id) "
                          "  left join addresses al on(e.id_addr_liv=al.id) ";
            bool is_first = true;
            if (ui->ch_addr_reg->isChecked()) {
                if (ui->combo_subj->currentIndex()>0) {
                    sql += QString(is_first ? " where " : " and ") + " ar.subj=" + QString::number(ui->combo_subj->currentData().toInt()) + " ";
                    is_first = false;
                }
                if (ui->combo_dstr->currentIndex()>0) {
                    sql += QString(is_first ? " where " : " and ") + " ar.dstr=" + QString::number(ui->combo_dstr->currentData().toInt()) + " ";
                    is_first = false;
                }
                if (ui->combo_city->currentIndex()>0) {
                    sql += QString(is_first ? " where " : " and ") + " ar.city=" + QString::number(ui->combo_city->currentData().toInt()) + " ";
                    is_first = false;
                }
                if (ui->combo_nasp->currentIndex()>0) {
                    sql += QString(is_first ? " where " : " and ") + " ar.nasp=" + QString::number(ui->combo_nasp->currentData().toInt()) + " ";
                    is_first = false;
                }
                if (ui->combo_strt->currentIndex()>0) {
                    sql += QString(is_first ? " where " : " and ") + " ar.strt=" + QString::number(ui->combo_strt->currentData().toInt()) + " ";
                    is_first = false;
                }
            }
            if (ui->ch_addr_liv->isChecked()) {
                if (ui->combo_subj->currentIndex()>0) {
                    sql += QString(is_first ? " where " : " and ") + " al.subj=" + QString::number(ui->combo_subj->currentData().toInt()) + " ";
                    is_first = false;
                }
                if (ui->combo_dstr->currentIndex()>0) {
                    sql += QString(is_first ? " where " : " and ") + " al.dstr=" + QString::number(ui->combo_dstr->currentData().toInt()) + " ";
                    is_first = false;
                }
                if (ui->combo_city->currentIndex()>0) {
                    sql += QString(is_first ? " where " : " and ") + " al.city=" + QString::number(ui->combo_city->currentData().toInt()) + " ";
                    is_first = false;
                }
                if (ui->combo_nasp->currentIndex()>0) {
                    sql += QString(is_first ? " where " : " and ") + " al.nasp=" + QString::number(ui->combo_nasp->currentData().toInt()) + " ";
                    is_first = false;
                }
                if (ui->combo_strt->currentIndex()>0) {
                    sql += QString(is_first ? " where " : " and ") + " al.strt=" + QString::number(ui->combo_strt->currentData().toInt()) + " ";
                    is_first = false;
                }
            }

            QSqlQuery *query = new QSqlQuery(db);
            if (!mySQL.exec(this, sql, "Построение фильтра персон", *query, true, db, data_app)) {
                QMessageBox::critical(this, "Непредвиденная ошибка", "При построить фильтр персон произошла неожиданная ошибка.\n\nОперация отменена.");
                delete query;
                return;
            }
            delete query;
        } else {
            QMessageBox::warning(this, "Выберите условие отбора", "Выберите условие отбора");
        }
        break;
    case 2:
        if ( ui->ch_doc_type->isChecked()
             || ui->ch_doc_sernum->isChecked()
             || ui->ch_doc_start->isChecked()
             || ui->ch_doc_exp->isChecked()) {
            QString sql = "delete from persons_find ; "
                          "insert into persons_find(id_person, find_flag) "
                          "select distinct e.id, 1 "
                          "  from persons e "
                          "  left join persons_docs d on(e.id=d.id_person) ";
            bool is_first = true;
            if (ui->ch_doc_type->isChecked()) {
                sql += QString(is_first ? " where " : " and ") + " d.doc_type=" + QString::number(ui->combo_doc_type->currentData().toInt()) + " ";
                is_first = false;
            }
            if (ui->ch_doc_sernum->isChecked()) {
                sql += QString(is_first ? " where " : " and ") + " d.doc_ser like('" + ui->line_doc_ser->text().trimmed() + "%') "
                                                      " and "  + " d.doc_num like('" + ui->line_doc_num->text().trimmed() + "%') ";
                is_first = false;
            }
            if (ui->ch_doc_start->isChecked()) {
                sql += QString(is_first ? " where " : " and ") + " d.doc_date='" + ui->date_doc_start->date().toString("yyyy-MM-dd") + "' ";
                is_first = false;
            }
            if (ui->ch_doc_exp->isChecked()) {
                sql += QString(is_first ? " where " : " and ") + " d.doc_exp='" + ui->date_doc_exp->date().toString("yyyy-MM-dd") + "' ";
                is_first = false;
            }

            QSqlQuery *query = new QSqlQuery(db);
            if (!mySQL.exec(this, sql, "Построение фильтра персон", *query, true, db, data_app)) {
                QMessageBox::critical(this, "Непредвиденная ошибка", "При построить фильтр персон произошла неожиданная ошибка.\n\nОперация отменена.");
                delete query;
                return;
            }
            delete query;
        } else {
            QMessageBox::warning(this, "Выберите условие отбора", "Выберите условие отбора");
        }
        break;
    case 3:
        if ( ui->ch_sex->isChecked()
             || ui->ch_age->isChecked()) {
            QString sql = "delete from persons_find ; "
                          "insert into persons_find(id_person, find_flag) "
                          "select distinct e.id, 1 "
                          "  from persons e ";
            bool is_first = true;
            if (ui->ch_sex->isChecked()) {
                if (ui->rb_sex1->isChecked()) {
                    sql += QString(is_first ? " where " : " and ") + " e.sex=1 ";
                    is_first = false;
                } else if (ui->rb_sex2->isChecked()) {
                    sql += QString(is_first ? " where " : " and ") + " e.sex=2 ";
                    is_first = false;
                }
            }
            if (ui->ch_age->isChecked()) {
                if (ui->rb_age1->isChecked()) {
                    sql += QString(is_first ? " where " : " and ") + " e.date_birth>'" + QDate::currentDate().addYears(-18).toString("yyyy-MM-dd") + "' ";
                    is_first = false;
                } else if (ui->rb_age2->isChecked()) {
                    sql += QString(is_first ? " where " : " and ") + " e.date_birth<='" + QDate::currentDate().addYears(-18).toString("yyyy-MM-dd") + "' ";
                    is_first = false;
                }
            }

            QSqlQuery *query = new QSqlQuery(db);
            if (!mySQL.exec(this, sql, "Построение фильтра персон", *query, true, db, data_app)) {
                QMessageBox::critical(this, "Непредвиденная ошибка", "При построить фильтр персон произошла неожиданная ошибка.\n\nОперация отменена.");
                delete query;
                return;
            }
            delete query;
        } else {
            QMessageBox::warning(this, "Выберите условие отбора", "Выберите условие отбора");
        }
        break;
    case 4:
        if ( ui->ch_phone->isChecked()
             || ui->ch_email->isChecked() ) {
            QString sql = "delete from persons_find ; "
                          "insert into persons_find(id_person, find_flag) "
                          "select distinct e.id, 1 "
                          "  from persons e ";
            bool is_first = true;
            if (ui->ch_phone->isChecked()) {
                sql += QString(is_first ? " where " : " and ") + " ( e.phone_cell like('%" + ui->line_phone->text().trimmed() + "') "
                                                                " or e.phone_home like('%" + ui->line_phone->text().trimmed() + "') "
                                                                " or e.phone_work like('%" + ui->line_phone->text().trimmed() + "') ) ";
                is_first = false;
            }
            if (ui->ch_email->isChecked()) {
                sql += QString(is_first ? " where " : " and ") + " e.email like('" + ui->line_email->text().trimmed() + "%') ";
                is_first = false;
            }

            QSqlQuery *query = new QSqlQuery(db);
            if (!mySQL.exec(this, sql, "Построение фильтра персон", *query, true, db, data_app)) {
                QMessageBox::critical(this, "Непредвиденная ошибка", "При построить фильтр персон произошла неожиданная ошибка.\n\nОперация отменена.");
                delete query;
                return;
            }
            delete query;
        } else {
            QMessageBox::warning(this, "Выберите условие отбора", "Выберите условие отбора");
        }
        break;
    case 5:
        if ( ui->ch_id_person->isChecked()
             || ui->ch_id_polis->isChecked()
             || ui->ch_id_event->isChecked()
             || ui->ch_id_vizit->isChecked()) {
            QString sql = "delete from persons_find ; "
                          "insert into persons_find(id_person, find_flag) "
                          "select distinct e.id, 1 "
                          "  from persons e "
                          "  left join polises p on(e.id=p.id_person) "
                          "  left join events a on(p.id=a.id_polis)  "
                          "  left join vizits v on(v.id_person=e.id) ";
            bool is_first = true;
            if (ui->ch_id_person->isChecked()) {
                sql += QString(is_first ? " where " : " and ") + " e.id in(" + ui->line_id_person->text().trimmed().simplified() + ") ";
                is_first = false;
            }
            if (ui->ch_id_polis->isChecked()) {
                sql += QString(is_first ? " where " : " and ") + " p.id in(" + ui->line_id_polis->text().trimmed().simplified() + ") ";
                is_first = false;
            }
            if (ui->ch_id_event->isChecked()) {
                sql += QString(is_first ? " where " : " and ") + " a.id in(" + ui->line_id_event->text().trimmed().simplified() + ") ";
                is_first = false;
            }
            if (ui->ch_id_vizit->isChecked()) {
                sql += QString(is_first ? " where " : " and ") + " v.id in(" + ui->line_id_vizit->text().trimmed().simplified() + ") ";
                is_first = false;
            }

            QSqlQuery *query = new QSqlQuery(db);
            if (!mySQL.exec(this, sql, "Построение фильтра персон", *query, true, db, data_app)) {
                QMessageBox::critical(this, "Непредвиденная ошибка", "При построить фильтр персон произошла неожиданная ошибка.\n\nОперация отменена.");
                delete query;
                return;
            }
            delete query;
        } else {
            QMessageBox::warning(this, "Выберите условие отбора", "Выберите условие отбора");
        }
        break;
    default:
        break;
    }
    // обновим таблицу персон
    ((persons_wnd*)parent())->refresh_persons_tab();
}

void finder_wnd::on_ch_age_clicked(bool checked) {
    ui->rb_age1->setEnabled(checked);
    ui->rb_age2->setEnabled(checked);
    if (!checked) {
        ui->rb_age1->setChecked(false);
        ui->rb_age2->setChecked(false);
    }
}

void finder_wnd::on_ch_sex_clicked(bool checked) {
    ui->rb_sex1->setEnabled(checked);
    ui->rb_sex2->setEnabled(checked);
    if (!checked) {
        ui->rb_sex1->setChecked(false);
        ui->rb_sex2->setChecked(false);
    }
}


void finder_wnd::on_ch_phone_clicked(bool checked) {
    if (!checked) {
        ui->line_phone->clear();
    }
}
void finder_wnd::on_ch_email_clicked(bool checked) {
    if (!checked) {
        ui->line_email->clear();
    }
}


void finder_wnd::on_ch_id_person_clicked(bool checked) {
    if (!checked) {
        ui->line_id_person->clear();
    }
}
void finder_wnd::on_ch_id_polis_clicked(bool checked) {
    if (!checked) {
        ui->line_id_polis->clear();
    }
}
void finder_wnd::on_ch_id_event_clicked(bool checked) {
    if (!checked) {
        ui->line_id_event->clear();
    }
}
void finder_wnd::on_ch_id_vizit_clicked(bool checked) {
    if (!checked) {
        ui->line_id_vizit->clear();
    }
}

void finder_wnd::on_line_id_person_editingFinished() {
    ui->line_id_person->setText(ui->line_id_person->text().trimmed().simplified().replace(" ",",").replace(";",",").replace(".",",").replace(",,",",").replace(",,",","));
    ui->ch_id_person->setChecked(!ui->line_id_person->text().isEmpty());
}
void finder_wnd::on_line_id_polis_editingFinished() {
    ui->line_id_polis->setText(ui->line_id_polis->text().trimmed().simplified().replace(" ",",").replace(";",",").replace(".",",").replace(",,",",").replace(",,",","));
    ui->ch_id_polis->setChecked(!ui->line_id_polis->text().trimmed().isEmpty());
}
void finder_wnd::on_line_id_event_editingFinished() {
    ui->line_id_event->setText(ui->line_id_event->text().trimmed().simplified().replace(" ",",").replace(";",",").replace(".",",").replace(",,",",").replace(",,",","));
    ui->ch_id_event->setChecked(!ui->line_id_event->text().trimmed().isEmpty());
}
void finder_wnd::on_line_id_vizit_editingFinished() {
    ui->line_id_vizit->setText(ui->line_id_vizit->text().trimmed().simplified().replace(" ",",").replace(";",",").replace(".",",").replace(",,",",").replace(",,",","));
    ui->ch_id_vizit->setChecked(!ui->line_id_vizit->text().trimmed().isEmpty());
}
