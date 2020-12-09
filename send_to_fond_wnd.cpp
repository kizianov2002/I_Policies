#include "send_to_fond_wnd.h"
#include "ui_send_to_fond_wnd.h"

send_to_fond_wnd::send_to_fond_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent)
    : db(db), data_app(data_app), settings(settings), QDialog(parent), ui(new Ui::send_to_fond_wnd)
{
    ui->setupUi(this);

    export_assig_bystep_w = NULL;
    info_not_sended_w = NULL;
    Gutten_Morgen_w = NULL;

    ui->rb_p040->setVisible(false);

    refresh_points();
    refresh_events();

    xml_files.clear();
    zip_files.clear();
    big_zip_file.clear();

    if (data_app.split_by_ot==1) {
        ui->rb_split->setChecked(true);
        ui->pan_split->setEnabled(true);
    } else {
        ui->rb_all->setChecked(true);
        ui->pan_split->setEnabled(false);
    }

    ui->ch_new_names->setChecked(data_app.f_send2fond_new_IM);
    ui->ch_new_namepats->setChecked(data_app.f_send2fond_new_OT);
    ui->ch_new_assigs->setChecked(data_app.f_send2fond_new_assigs);



    // забекапим старые данные
    QString bkp_name = QDate::currentDate().toString("yyyy-MM-dd") + ".zip";

    if (!QFile::exists(data_app.path_out + "/backup/" + bkp_name)) {
        QDir out_dir(data_app.path_out);

        // сохраним в архив
        QProcess myProcess;
        QStringList files = out_dir.entryList(QString("*.*").split(" "), QDir::Files);
        for (int i=0; i<files.size(); i++) {
            QString com = "\"" + data_app.path_arch + "/7z.exe\" a \"" + data_app.path_out + "/backup/" + bkp_name + "\" \"" + data_app.path_out + "/" + files.at(i) + "\"";
            while (myProcess.execute(com)!=0) {
                if (QMessageBox::warning(this,
                                         "Ошибка при переносе данных в резервную копию",
                                         "При переносе данных в резервную копию произошла непредвиденная ошибка\n\nОпреация прервана.\n\n" +
                                         data_app.path_out + "/backup/" + bkp_name +
                                         "\n\nПроверьте, не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                         QMessageBox::Retry|QMessageBox::Abort,
                                         QMessageBox::Retry)==QMessageBox::Abort) {
                    this->setCursor(Qt::ArrowCursor);
                    return;
        }   }   }

        // удалим забекапленные файлы
        for (int i=0; i<files.count(); i++) {
            bool res = QFile::remove(data_app.path_out + "/" + files.at(i));
        }
    }

    on_rb_genNew_clicked(true);
    on_rb_genOld_clicked(false);

    ui->date_from->setDate(QDate(QDate::currentDate().year(),1,1));
    ui->date_to->setDate(QDate::currentDate());

    //on_ch_old_point_clicked(false);
    //on_ch_old_event_clicked(false);
    on_ch_old_dates_clicked(false);
    /*
    QMessageBox::information(this, "Обновление статусов застрахованных",
                             "Сейчас будет выполнено полное обновление стстусов страхования застрахованных.");
    */
    ui->lab_comment->setText("ОБНОВЛЕНИЕ СТАТУСОВ ЗАСТРАХОВАННЫХ");
    ui->rb_genNew->setVisible(false);
    ui->rb_genOld->setVisible(false);
    ui->pan_genNew->setEnabled(false);
    ui->pan_genOld->setEnabled(false);
    ui->pan_Dn->setEnabled(false);
    QApplication::processEvents();

    // Обновление статусов застрахованных
    /*QString sql = "select * from  update_pers_links(NULL) ; ";

    delete Gutten_Morgen_w;
    Gutten_Morgen_w = new beep_Guten_Morgen_wnd();
    Gutten_Morgen_w->show();
    QApplication::processEvents();

        QSqlQuery *query = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Обновление статусов застрахованных", *query, true, db, data_app))
            QMessageBox::warning(this, "Операция отменена", "При обновлении статусов застрахованных произошла непредвиденная ошибка.\n\nОперация пропущена.");
        delete query;

    Gutten_Morgen_w->hide();
    QApplication::processEvents();
    delete Gutten_Morgen_w;*/

    ui->lab_comment->setText(" ");
    ui->rb_genNew->setVisible(true);
    ui->rb_genOld->setVisible(true);
    ui->pan_genNew->setEnabled(true);
    ui->pan_genOld->setEnabled(true);
    ui->pan_Dn->setEnabled(true);
    QApplication::processEvents();

}

send_to_fond_wnd::~send_to_fond_wnd() {
    delete ui;
}

void send_to_fond_wnd::refresh_points() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка медорганизаций
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select point_regnum, '('||point_regnum||')  '||point_name as name "
                  " from public.points "
                  " where status=1 and point_regnum<>'000' "
                  " order by point_regnum ; ";
    mySQL.exec(this, sql, QString("Список ПВП"), *query, true, db, data_app);
    ui->combo_point->clear();
    ui->combo_point->addItem(" - все ПВП - ", "нет");
    ui->combo_old_point->clear();
    ui->combo_old_point->addItem(" - все ПВП - ", "нет");
    while (query->next()) {
        ui->combo_point->addItem(query->value(1).toString(), query->value(0).toString());
        ui->combo_old_point->addItem(query->value(1).toString(), query->value(0).toString());
    }
    ui->combo_point->setCurrentIndex(0);
    ui->combo_old_point->setCurrentIndex(0);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void send_to_fond_wnd::refresh_events() {
    this->setCursor(Qt::WaitCursor);
    ui->combo_old_event->clear();
    ui->combo_old_event->addItem(" - все события - ", "нет");
    ui->combo_old_event->addItem("П010 - первичный выбор СМО", "П010");
    ui->combo_old_event->addItem("П02х - снятие с учёта", "П02");
    ui->combo_old_event->addItem("   П021 - переход ЗЛ в другую СМО", "П021");
    ui->combo_old_event->addItem("   П022 - смерть ЗЛ", "П022");
    ui->combo_old_event->addItem("   П023 - исправление дублирования", "П023");
    ui->combo_old_event->addItem("   П024 - исправление ошибки в ЕНП", "П024");
    ui->combo_old_event->addItem("   П025 - ЗЛ больше не подлежит ОМС", "П025");
    ui->combo_old_event->addItem("П03х - смена СМО", "П03");
    ui->combo_old_event->addItem("   П031 - по желанию ЗЛ (старый полис)", "П031");
    ui->combo_old_event->addItem("   П032 - смена места жит. (старый полис)", "П032");
    ui->combo_old_event->addItem("   П033 - расторж. договора (старый полис)", "П033");
    ui->combo_old_event->addItem("   П034 - по желанию ЗЛ (новый полис)", "П034");
    ui->combo_old_event->addItem("   П035 - смена места жит. (новый полис)", "П035");
    ui->combo_old_event->addItem("   П036 - растрож. договора (новый полис)", "П036");
    ui->combo_old_event->addItem("П060 - выдача полиса на руки", "П060");
    ui->combo_old_event->addItem("П06х - корректировка данных ЗЛ", "П06");
    ui->combo_old_event->addItem("   П061 - изменение идентифиц. данных", "П061");
    ui->combo_old_event->addItem("   П062 - выдача дубликата", "П062");
    ui->combo_old_event->addItem("   П063 - окончание действия полиса", "П063");

    ui->combo_old_event->setCurrentIndex(0);
    this->setCursor(Qt::ArrowCursor);
}

void send_to_fond_wnd::on_rb_split_clicked() {
    ui->pan_split->setEnabled(ui->rb_split->isChecked());
}

void send_to_fond_wnd::on_rb_all_clicked() {
    ui->pan_split->setEnabled(ui->rb_split->isChecked());
}

void send_to_fond_wnd::on_bn_cansel_clicked() {
    close();
}


int send_to_fond_wnd::send(QString point_regnum, bool f_old_data) {
    db.transaction();

    // подготовим имя файла
    QString f_name = "_";

    if (data_app.ocato=="14000") {
        // Белгород

        int yy = QDate::currentDate().year() ;
        int mm = QDate::currentDate().month() ;
        int dd = QDate::currentDate().day() +1;     // завтра

        QSqlQuery *query_test_file = new QSqlQuery(db);
        int cnt = 0;
        do {
            // отматаем день назад
            dd--;
            if (dd<1) {
                mm--;
                dd=30;
            }
            if (mm<1) {
                yy--;
                mm=12;
            }

            QString sql_test_file;
            f_name = "i";
            f_name += data_app.smo_regnum;
            f_name += "_";
            f_name += point_regnum;
            f_name += "_";
            f_name += QDate::currentDate().toString("MMyy");

            if (!f_old_data) {
                // определим номер посылки в зависимости от параметров фильтрации и региональных требований
                if (ui->rb_all->isChecked()) {
                    f_name += QString("00" + QString::number(dd)).right(2);
                } else if (ui->rb_split->isChecked()) {
                    if (ui->rb_p060->isChecked()) {
                        f_name += "1";
                    }
                    if (ui->ch_split_refugees->isChecked()) {
                        f_name += "6" + QString("00" + QString::number(dd)).right(2);
                    } else if (ui->ch_split_russians->isChecked()) {
                        f_name += QString("00" + QString::number(dd)).right(2);
                    } else if (ui->ch_split_foreigners->isChecked()) {
                        f_name += "7" + QString("00" + QString::number(dd)).right(2);
                    } else if (ui->ch_split_russians_zpatname->isChecked()) {
                        f_name += "8" + QString("00" + QString::number(dd)).right(2);
                    } else if (ui->ch_split_foreigners_zpatname->isChecked()) {
                        f_name += "8" + QString("00" + QString::number(dd)).right(2);
                    } else if (ui->ch_split_others->isChecked()) {
                        f_name += QString("00" + QString::number(dd)).right(2);
                    }
                }
            } else {
                f_name += QString("00" + QString::number(dd)).right(2);
            }

            // проверим, нет ли такого файла в таблице отосланных
            sql_test_file =  "select count(*) from files_out_i "
                             " where file_name='" + f_name + "' ; ";
            bool ftest_res = mySQL.exec(this, sql_test_file, "Проверка имени файла на уникальность", *query_test_file, true, db, data_app);
            if (!ftest_res) {
                delete query_test_file;
                QMessageBox::warning(this, "Операция отменена", "При проверке уникальности имени выгружаемого файла произошла непредвиденная ошибка.\n\nОперация выгрузки данных для ТФОМС отменена");
                db.rollback();
                return 0;
            }
            query_test_file->next();
            cnt = query_test_file->value(0).toInt();
        } while (cnt>0);
        delete query_test_file;

    } else {
        // другие филиалы

        int yy = QDate::currentDate().year() ;
        int mm = QDate::currentDate().month() ;
        int dd = 0;

        QSqlQuery *query_test_file = new QSqlQuery(db);
        int cnt = 0;
        do {
            dd++;
            QString sql_test_file;
            f_name = "i";
            f_name += data_app.smo_regnum;
            f_name += "_";
            f_name += point_regnum;
            f_name += "_";
            f_name += QDate::currentDate().toString("MMyy");

            if (!f_old_data) {
                // определим номер посылки в зависимости от параметров фильтрации и региональных требований
                if (ui->rb_all->isChecked()) {
                    f_name += QString("00" + QString::number(dd)).right(2);
                } else if (ui->rb_split->isChecked()) {
                    if (ui->rb_p060->isChecked()) {
                        f_name += "1";
                    }
                    if (ui->ch_split_refugees->isChecked()) {
                        f_name += "6" + QString("00" + QString::number(dd)).right(2);
                    } else if (ui->ch_split_russians->isChecked()) {
                        f_name += QString("00" + QString::number(dd)).right(2);
                    } else if (ui->ch_split_foreigners->isChecked()) {
                        f_name += "7" + QString("00" + QString::number(dd)).right(2);
                    } else if (ui->ch_split_russians_zpatname->isChecked()) {
                        f_name += "8" + QString("00" + QString::number(dd)).right(2);
                    } else if (ui->ch_split_foreigners_zpatname->isChecked()) {
                        f_name += "8" + QString("00" + QString::number(dd)).right(2);
                    } else if (ui->ch_split_others->isChecked()) {
                        f_name += QString("00" + QString::number(dd)).right(2);
                    }
                }
            } else {
                f_name += QString("00" + QString::number(dd)).right(2);
            }

            // проверим, нет ли такого файла в таблице отосланных
            sql_test_file =  "select count(*) from files_out_i "
                             " where file_name='" + f_name + "' ; ";
            bool ftest_res = mySQL.exec(this, sql_test_file, "Проверка имени файла на уникальность", *query_test_file, true, db, data_app);
            if (!ftest_res) {
                delete query_test_file;
                QMessageBox::warning(this, "Операция отменена", "При проверке уникальности имени выгружаемого файла произошла непредвиденная ошибка.\n\nОперация выгрузки данных для ТФОМС отменена");
                db.rollback();
                return 0;
            }
            query_test_file->next();
            cnt = query_test_file->value(0).toInt();
        } while (cnt>0);

        delete query_test_file;
    }


    if (!f_old_data) {
        // подготовка БД
        QString sql_upd;
        sql_upd = "update events a "
                  "   set status=1 "
                  " where a.status in(0,1) "
                  "   and ( a.event_dt is NULL or a.event_dt<=CURRENT_TIMESTAMP ) "
                  "   and a.id_point in (select pnt.id from points pnt where pnt.point_regnum='" + point_regnum + "') ";

        if (ui->rb_split->isChecked()) {

            sql_upd+= " and a.id_polis in "
                      "  ( select p.id "
                      "      from polises p "
                      "      left join persons e on(e.id=p.id_person) "
                      "     where ";
            {
                if (ui->ch_split_refugees->isChecked()) {
                    sql_upd+= "   e.category in(5, 10) ";
                }
                if (ui->ch_split_russians->isChecked()) {
                    sql_upd+= "   e.category in(1, 6)  and "
                              "   e.ot is not NULL  and "
                              "   e.ot<>'' ";
                }
                if (ui->ch_split_foreigners->isChecked()) {
                    sql_upd+= "   e.category in(2, 3, 7, 8)  and "
                              "   e.ot is not NULL  and "
                              "   e.ot<>'' ";
                }
                if (ui->ch_split_russians_zpatname->isChecked()) {
                    sql_upd+= "   e.category not in(1, 6)  and "
                              "   ( e.ot is NULL or e.ot='' ) ";
                }
                if (ui->ch_split_foreigners_zpatname->isChecked()) {
                    sql_upd+= "   e.category in(2, 3, 7, 8)  and "
                              "   ( e.ot is NULL or e.ot='' ) ";
                }
                if (ui->ch_split_others->isChecked()) {
                    sql_upd+= "   e.category not in(4, 9) ";
                }
            }
            sql_upd+= "  ) ";

            if (ui->rb_p060->isChecked())
                sql_upd += " and a.event_code='П060' ";
            else if (ui->rb_p040->isChecked())
                sql_upd += " and a.event_code='П040' ";
            else sql_upd+= " and a.event_code<>'П060' "
                           " and a.event_code<>'П040' ";

            if (!ui->ch_P02x->isChecked())
                sql_upd += " and left(a.event_code,3)<>'П02' ";
            if (!ui->ch_P040->isChecked())
                sql_upd += " and a.event_code<>'П040' ";

            /* if ( !ui->rb_p040->isChecked() ) {
                sql_upd+= " and a.event_code<>'П040' ";
            }
            if ( !ui->rb_p060->isChecked() ) {
                sql_upd+= " and a.event_code<>'П060' ";
            }*/
        }


        QSqlQuery *query_upd = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_upd, "Подготовка к выбору неотправленных полисов для отправки в ТФОМС", *query_upd, true, db, data_app)) {
            delete query_upd;
            QMessageBox::warning(this, "Операция отменена", "При подготовке к выгрузке данных для ТФОМС произошла неожиданная ошибка.\n\nОперация выгрузки данных для ТФОМС отменена");
            db.rollback();
            return 0;
        }
        delete query_upd;
    }

    QApplication::processEvents();

    // генерация XML для ТФОМС по всем не отправленным данным
    QString sql = "";
    //sql+= "select * from update_pers_links(NULL) ; ";
    sql+= "select distinct p.id as id_polis, p.ocato, p.enp, p.ogrn_smo, p.pol_v, f8.text as pol_v_text, p.pol_type, p.pol_ser, p.pol_num, p.vs_num, "
          "       p.date_begin, p.date_end, p.date_stop, p.in_erp, p.order_num, p.order_date, "
          "       e.id as id_person, e.fam, e.im, e.ot, e.sex, e.category, e.date_birth, e.plc_birth, e.date_death, e.snils, e.oksm_birth, ob.alfa3 as oksm_b_txt, "
          "       e.oksm_c, oc.alfa3 as oksm_c_txt, e.phone_cell, e.email, e.dost, e.rep_id, e.rep_fio, "
          "       rep.rep_fio_full, rep.rep_contact, "
          "       d.doc_type, f11.text as doc_type_text, d.doc_ser, d.doc_num, d.doc_date, d.doc_exp, d.doc_org, d.org_code, d.status, "
          "       drp.doc_type as drp_type, p11.text as drp_type_text, drp.doc_ser as drp_ser, drp.doc_num as drp_num, drp.doc_date as drp_date, drp.doc_exp as drp_exp,"
          "       drp.doc_org as drp_org, drp.org_code as drp_orgcode, drp.status as drp_status, "
          "       foto.bin as foto_bin, sign.bin as sign_bin, "
          "       e.date_reg, e.bomj, ar.post_index, ar.ocato, ar.ocato_full, ar.dstr_name, "
          "       case when (ar.nasp_name is NULL or ar.nasp_name='') then ar.city_name else ar.nasp_name end as nasp_name, "
          "       ar.strt_name, ar.house, ar.corp, ar.quart, "
          "       al.post_index as post_index_liv, al.ocato as ocato_liv, al.ocato_full as ocato_full_liv, al.dstr_name as dstr_name_liv, "
          "       case when (al.nasp_name is NULL or al.nasp_name='') then al.city_name else al.nasp_name end as nasp_name_liv, "
          "       al.strt_name as strt_name_liv, al.house as house_liv, al.corp as corp_liv, al.quart as quart_liv, "
          "       v.v_date, v.v_method, v.has_petition, v.r_smo, v.r_polis, v.f_polis, "
          "       a.event_code, a.event_dt, a.event_time, a.comment, "
          "       ocr.subj_name as subj_reg, ocl.subj_name as subj_liv, "
          "       a.id as id_event, "
          "       old.old_fam, old.old_im, old.old_ot, old.old_sex, old.old_date_birth, old.old_plc_birth, old.old_enp, old.old_snils, "
          "       b.f_polis as blank_fpolis, "
          "       ar.fias_aoid, ar.fias_houseid, al.fias_aoid, al.fias_houseid, "
          "       v.id as id_vizit, d.id as id_udl, drp.id as id_drp, "
          "       a.id_udl as id_udl_act, a.id_drp as id_drp_act, "
          "       p.r_smo, p.r_polis, "
          "       p_old.date_begin as old_date_begin, p_old.date_end as old_date_end, p_old.date_stop as old_date_stop "
          "  from events a "
 //       "  left join files_r_evts r on(a.id=r.id_event) "
          "  left join polises p on(p.id=a.id_polis) "
          "  left join spr_f008 f8 on(p.pol_v=f8.code and f8.date_end is NULL) "
          "  left join polises p_old on(p.id_old_polis=p_old.id and p_old.pol_v=2) "
          "  left join persons e on(p.id_person=e.id) "
          "  left join (select id, fam || ' ' || im || ' ' || coalesce(ot,' ') as rep_fio_full, phone_cell || ', ' || email as rep_contact from persons r /*where r.id=e.rep_id*/) rep on(e.rep_id=rep.id) "

 //       "  left join persons_docs d on(d.id_person=e.id and d.status=1 and d.doc_type in(1,3,9,10,12,13,14,21,22,24,25,27,28) and (d.doc_exp is NULL or d.doc_exp>CURRENT_DATE)) "
          "  left join persons_docs d on(d.id=a.id_udl) "       // тот УДЛ, который прописан в событии, независимо от его текущего статуса !!!

          "  left join spr_f011 f11 on(f11.code=d.doc_type) "

 //       "  left join persons_docs drp on(drp.id_person=e.id and drp.status=1 and drp.doc_type in(11,23) and (drp.doc_exp is NULL or drp.doc_exp>CURRENT_DATE)) "
          "  left join persons_docs drp on(drp.id=a.id_drp) "   // тот ДРП, который прописан в событии, независимо от его текущего статуса !!!

          "  left join spr_f011 p11 on(p11.code=drp.doc_type) "
          "  left join persons_photo foto on(foto.id_person=e.id and foto.type=2) "
          "  left join persons_photo sign on(sign.id_person=e.id and sign.type=3) "
          "  left join spr_oksm ob on(ob.code=e.oksm_birth) "
          "  left join spr_oksm oc on(oc.code=e.oksm_c) "
          "  left join addresses ar on(e.id_addr_reg=ar.id) "
          "  left join addresses al on(e.id_addr_liv=al.id) "
          "  left join vizits v on(v.id=a.id_vizit) "
          "  left join spr_ocato ocr on(ocr.code_ocato=ar.ocato) "
          "  left join spr_ocato ocl on(ocl.code_ocato=al.ocato) "
          "  left join persons_old old on(old.id_person=e.id) "
          "  left join operators oper on(a.id_operator=oper.id) "
          "  left join points pnt on(a.id_point=pnt.id) "

          "  left join blanks_pol b on(b.pol_ser=p.pol_ser and b.pol_num=p.pol_num) "
          "  left join blanks_vs bv on(bv.vs_num=p.vs_num) "
 //       "  left join blanks_uec bu on(bu.uec_num=p.uec_num) "     // нефиг тянуть этот атавизм

          " where left(a.event_code, 3) in('П01'" + QString(ui->ch_P02x->isChecked() ? ", 'П02'" : "") + ", 'П03'" + QString(ui->ch_P040->isChecked() ? ", 'П04'" : "") + ", 'П06') "
          "   and (bv.status is NULL or bv.status>=0)     /* чтобы не пытался выдать испорченные бланки */ ";



    // для новых данных
    if (!f_old_data) {
        sql+= " and pnt.point_regnum='" + point_regnum + "' "
              " and a.status=1 ";

        if (ui->rb_split->isChecked()) {
            if (ui->rb_p060->isChecked())
                sql += " and a.event_code='П060' ";
            else if (ui->rb_p040->isChecked())
                sql += " and a.event_code='П040' ";
            else sql+= " and a.event_code<>'П060' "
                       " and a.event_code<>'П040' ";
        }

        /*
        sql+= " and pnt.point_regnum='" + point_regnum + "' "
              " and a.status in(0,1) ";

        if (ui->rb_split->isChecked()) {
            sql+= " and ";
            if (ui->ch_split_refugees->isChecked()) {
                sql+= " e.category in(5, 10) ";
            }
            if (ui->ch_split_russians->isChecked()) {
                sql+= " e.category in(1, 6)  and "
                      " e.ot is not NULL  and "
                      " e.ot<>'' ";
            }
            if (ui->ch_split_foreigners->isChecked()) {
                sql+= " e.category in(2, 3, 7, 8)  and "
                      " e.ot is not NULL  and "
                      " e.ot<>'' ";
            }
            if (ui->ch_split_russians_zpatname->isChecked()) {
                sql+= " e.category not in(1, 6)  and "
                      " (e.ot is NULL or e.ot='') ";
            }
            if (ui->ch_split_foreigners_zpatname->isChecked()) {
                sql+= " e.category in(2, 3, 7, 8)  and "
                      " (e.ot is NULL or e.ot='') ";
            }
            if (ui->ch_split_others->isChecked()) {
                sql+= " e.category not in(4, 9) ";
            }

            if (ui->rb_p060->isChecked()) {
                sql+= " and a.event_code='П060' ";
            } else {
                sql+= " and a.event_code<>'П060' ";
            }
        }
        */
    } else

    // для выгрузки задним числом
    if (f_old_data) {

        // по ПВП
        if (ui->combo_old_point->currentIndex()>0) {
            QString point_regnum = QString("000" + ui->combo_old_point->currentData().toString()).right(3);
            sql+= " and pnt.point_regnum='" + point_regnum + "' ";
        }

        // по событию
        if (ui->combo_old_event->currentIndex()>0) {
            QString event_code = ui->combo_old_event->currentData().toString();
            if (event_code=="П060") {
                sql+= " and a.event_code='П060' ";
            } else {
                sql+= " and a.event_code like('" + event_code + "%') "
                      " and a.event_code<>'П060' ";
            }
        }

        // по дате события
        if (ui->ch_old_dates->isChecked()) {
            QDate date_from = ui->date_from->date();
            QDate date_to   = ui->date_to->date();
            sql+= " and a.event_dt>='" + date_from.toString("yyyy-MM-dd") + "' ";
            sql+= " and a.event_dt<='" + date_to.toString("yyyy-MM-dd") + "' ";
        }
    }


    if (!f_old_data) {
        sql+= " order by  e.fam, e.im, e.ot, e.date_birth, a.id ; ";
    } else {
        sql+= " order by  a.event_code, a.event_dt ; ";
    }

    QSqlQuery *query = new QSqlQuery(db);
    if (!mySQL.exec(this, sql, "Выбор всех неотправленных полисов для отправки в ТФОМС", *query, true, db, data_app)) {
        delete query;
        QMessageBox::warning(this, "Операция отменена", "При запросе из базы всех неотправленных событий полисов произошла непредвиденная ошибка.\n\nОперация выгрузки данных для ТФОМС отменена.");
        db.rollback();
        return 0;
    }
    if (query->size()==0) {
        delete query;
        //QMessageBox::information(this, "Операция отменена", "Для выбранных условий отбора нет ни одной новой записи.\n\nОперация выгрузки данных для ТФОМС отменена.");
        db.rollback();
        return 0;
    }

    QString s_res_dir = data_app.path_out; //QFileDialog::getExistingDirectory(this, "Выберите папку для сохранения выгружаемого файла", data_app.path_out, 0);
    QString s_res_xml = data_app.path_out + "/" + f_name + ".xml";
    QString s_res_zip = data_app.path_out + "/" + f_name + ".zip";
    int n=0;
    while (QFile::exists(s_res_zip)) {
        n++;
        s_res_zip = s_res_dir + "/" + f_name + QString::number(n) + ".zip";
        s_res_xml = s_res_dir + "/" + f_name + QString::number(n) + ".xml";
    }
    xml_files.append(s_res_xml);
    zip_files.append(s_res_zip);

    int n_rec = 0;
    QList<int> id_events_list;
    int q_n = query->size();

    QString xml = "Проверка кодировки";
    QXmlStreamWriter stream(&xml);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();

    if (data_app.xml_vers=="2.1") {
        stream.writeStartElement("OPLIST"); {
            stream.writeAttribute("FILENAME", f_name);
            stream.writeAttribute("SMOCOD", data_app.smo_regnum);
            stream.writeAttribute("NRECORDS", QString::number(query->size()));
            stream.writeAttribute("PRZCOD", point_regnum);
            stream.writeAttribute("VERS", data_app.xml_vers);
            while ( (n_rec==0 && query->first()) || (n_rec>0 && query->next()) ) {

                int r_smo   = max(query->value(80).toInt(), query->value(108).toInt());
                int r_polis = max(query->value(81).toInt(), query->value(109).toInt());

                // для новых данных ...
                if (!f_old_data) {      // ... обновление сввязок
                    QString sql_link;
                    sql_link = "select * "
                               "from update_pers_links(" + QString::number(query->value(16).toInt()) + ", NULL) ; ";
                    QSqlQuery *query_link = new QSqlQuery(db);
                    query_link->exec( sql_link );
                    delete query_link;
                }

                id_events_list.append(query->value(89).toInt());
                QString event_code = query->value(83).toString();
                QString event_cod_  = event_code.left(3);

                n_rec++;
                if (n_rec%21==0) {
                    ui->lab_cnt->setText(QString::number(n_rec));
                    QApplication::processEvents();
                }
                stream.writeStartElement("","OP");
                {
                    stream.writeTextElement("N_REC", QString::number(n_rec));
                    stream.writeTextElement("TIP_OP", event_code);

                    //if (event_cod_!="П02") {
                        {
                        stream.writeStartElement("PERSON");
                            if (!query->value(17).isNull() && !query->value(17).toString().trimmed().isEmpty())   stream.writeTextElement("FAM", query->value(17).toString());
                            if (!query->value(18).isNull() && !query->value(18).toString().trimmed().isEmpty())   stream.writeTextElement("IM", query->value(18).toString());
                            if (!query->value(19).isNull() && !query->value(19).toString().trimmed().isEmpty())   stream.writeTextElement("OT", query->value(19).toString());
                            if (!query->value(20).isNull())                                                       stream.writeTextElement("W", QString::number(query->value(20).toInt()));
                            else                                                                                  stream.writeTextElement("W", "1");
                            if (!query->value(22).isNull() && query->value(22).toDate()>QDate(1920,1,1))          stream.writeTextElement("DR", query->value(22).toDate().toString("yyyy-MM-dd"));
                            else                                                                                  stream.writeTextElement("DR", QDate::currentDate().toString("yyyy-MM-dd"));
                            if (!query->value(23).isNull() && !query->value(23).toString().trimmed().isEmpty())   stream.writeTextElement("MR", query->value(23).toString().left(100));
                            else                                                                                  stream.writeTextElement("MR", "");
                            if (!query->value(32).isNull()) {
                                QString dost = query->value(32).toString();
                                if (dost.indexOf("1")>=0)
                                    stream.writeTextElement("DOST", "1");
                                if (dost.indexOf("2")>=0)
                                    stream.writeTextElement("DOST", "2");
                                if (dost.indexOf("3")>=0)
                                    stream.writeTextElement("DOST", "3");
                                if (dost.indexOf("4")>=0)
                                    stream.writeTextElement("DOST", "4");
                                if (dost.indexOf("5")>=0)
                                    stream.writeTextElement("DOST", "5");
                                if (dost.indexOf("6")>=0)
                                    stream.writeTextElement("DOST", "6");
                            }
                                if (!query->value(27).isNull() && !query->value(27).toString().trimmed().isEmpty())   stream.writeTextElement("BIRTH_OKSM", query->value(27).toString());
                                else                                                                                  stream.writeTextElement("BIRTH_OKSM", "643");
                            //if (!query->value(29).isNull() && !query->value(29).toString().trimmed().isEmpty())   stream.writeTextElement("C_OKSM", query->value(29).toString());
                            if (!query->value(29).isNull() && !query->value(29).toString().trimmed().isEmpty() && query->value(28).toInt()>0) {
                                stream.writeTextElement("C_OKSM", query->value(29).toString());
                            } else {
                                stream.writeTextElement("C_OKSM", "Б/Г");
                            }
                            if (!query->value(25).isNull() && !query->value(25).toString().trimmed().isEmpty()) {
                                //if (data_app.snils_format==1) {
                                    QString ss = QString(query->value(25).toString() + "00000000000").left(11);
                                    QString ss1 = ss.mid(0,3) + "-" + ss.mid(3,3) + "-" + ss.mid(6,3) + " " + ss.mid(9);
                                    stream.writeTextElement("SS", ss1);
                                //} else {
                                //    stream.writeTextElement("SS", query->value(25).toString());
                                //}
                            }
                            if (!query->value(21).isNull())                                                       stream.writeTextElement("KATEG", QString::number(query->value(21).toInt()));
                            else                                                                                  stream.writeTextElement("KATEG", "1");
                            if (!query->value(30).isNull() && !query->value(30).toString().trimmed().isEmpty())   stream.writeTextElement("PHONE", query->value(30).toString());
                            if (!query->value(31).isNull() && !query->value(31).toString().trimmed().isEmpty())   stream.writeTextElement("EMAIL", query->value(31).toString());
                            if (!query->value(35).isNull() && !query->value(35).toString().trimmed().isEmpty())   stream.writeTextElement("FIOPR", query->value(35).toString());
                            if (!query->value(36).isNull() && !query->value(36).toString().trimmed().isEmpty())   stream.writeTextElement("CONTACT", query->value(36).toString());
                            if (!query->value(24).isNull() && query->value(24).toDate()>QDate(1920,1,1))          stream.writeTextElement("DDEATH", query->value(24).toDate().toString("yyyy-MM-dd"));

                            stream.writeEndElement(); // PERSON
                        }
                    //}


                    //if (event_cod_!="П02") {
                        if ( !(query->value(90).isNull() || query->value(90).toString()=="") ||
                             !(query->value(91).isNull() || query->value(91).toString()=="") ||
                             !(query->value(92).isNull() || query->value(92).toString()=="") ||
                             ! query->value(93).isNull() ||
                             !(query->value(94).isNull() || query->value(94).toString()=="") ||
                             !(query->value(95).isNull() || query->value(95).toString()=="") ||
                             !(query->value(96).isNull() || query->value(96).toString()=="") )  {
                            stream.writeStartElement("OLD_PERSON");
                            {
                                // если есть какие-нибудь старые данные
                                if ( ( !query->value(90).isNull() && !query->value(90).toString().trimmed().isEmpty() ) ||
                                     ( !query->value(91).isNull() && !query->value(91).toString().trimmed().isEmpty() ) ||
                                     ( !query->value(92).isNull() && !query->value(92).toString().trimmed().isEmpty() ) ||
                                     ( !query->value(93).isNull() )                                                     ||
                                     ( !query->value(94).isNull() && !query->value(94).toString().trimmed().isEmpty() ) ||
                                     ( !query->value(95).isNull() && !query->value(95).toString().trimmed().isEmpty() ) ||
                                     ( !query->value(96).isNull() && !query->value(95).toString().trimmed().isEmpty() ) ) {

                                    // соберём полные данные для идентификации застрахованного
                                    if (!query->value(90).isNull() && !query->value(90).toString().trimmed().isEmpty())   stream.writeTextElement("FAM", query->value(90).toString());
                                    //else stream.writeTextElement("FAM",query->value(17).toString());
                                    if (!query->value(91).isNull() && !query->value(91).toString().trimmed().isEmpty())   stream.writeTextElement("IM", query->value(91).toString());
                                    //else stream.writeTextElement("IM", query->value(18).toString());
                                    if (!query->value(92).isNull() && !query->value(92).toString().trimmed().isEmpty())   stream.writeTextElement("OT", query->value(92).toString());
                                    //else stream.writeTextElement("OT", query->value(19).toString());
                                    if (!query->value(93).isNull())                                                       stream.writeTextElement("W", QString::number(query->value(93).toInt()));
                                    else stream.writeTextElement("W", QString::number(query->value(20).toInt()));
                                    if (!query->value(94).isNull() && query->value(94).toDate()>QDate(1920,1,1))         stream.writeTextElement("DR", query->value(94).toDate().toString("yyyy-MM-dd"));
                                    else stream.writeTextElement("DR", query->value(22).toDate().toString("yyyy-MM-dd"));

                                    // OLD_ENP добавляется только при наличии и при смене ЕНП, ДР или пола
                                    if ( !query->value(96).isNull() &&
                                         ( ( !query->value(96).isNull() && query->value( 2).toString()!=query->value(96).toString() ) ||
                                           ( !query->value(93).isNull() && query->value(20).toString()!=query->value(93).toString() ) ||
                                           ( !query->value(94).isNull() && query->value(22).toDate()!=query->value(94).toDate() ) ) ) {
                                       if (!query->value(96).isNull() && !query->value(96).toString().trimmed().isEmpty())   stream.writeTextElement("OLD_ENP", query->value(96).toString());
                                       //else stream.writeTextElement("OLD_ENP", query->value( 2).toString());
                                    }
                                    if (!query->value(95).isNull() && !query->value(95).toString().trimmed().isEmpty())   stream.writeTextElement("MR", query->value(95).toString().left(100));
                                    else stream.writeTextElement("MR", query->value(23).toString().left(100));
                                } else {

                                }
                            }
                            stream.writeEndElement(); // OLD_PERSON
                        }
                    //}


                    //if (event_cod_!="П02") {
                        stream.writeStartElement("ADDRES_G");
                        {
                            if (!query->value(58).isNull())                                                       stream.writeTextElement("BOMG", QString::number(query->value(58).toInt()));
                            else                                                                                  stream.writeTextElement("BOMG", "0");

                            if (data_app.FIAS_flag) {
                                if (!query->value(99).isNull() && !query->value(99).toString().trimmed().isEmpty())   stream.writeTextElement("FIAS_AOID", query->value(99).toString());
                                if (!query->value(100).isNull() && !query->value(100).toString().trimmed().isEmpty()) stream.writeTextElement("FIAS_HOUSEID", query->value(100).toString());
                            }
                            if (!query->value(60).isNull() && !query->value(60).toString().trimmed().isEmpty())   stream.writeTextElement("SUBJ", query->value(60).toString());
                            if (!query->value(59).isNull() && !query->value(59).toString().trimmed().isEmpty())   stream.writeTextElement("INDX", query->value(59).toString());
                            if (!query->value(61).isNull() && !query->value(61).toString().trimmed().isEmpty())   stream.writeTextElement("OKATO", query->value(61).toString());
                            if (!query->value(62).isNull() && !query->value(62).toString().trimmed().isEmpty() && query->value(62).toString().trimmed()!="- нет -")   stream.writeTextElement("RNNAME", query->value(62).toString());
                            if (!query->value(63).isNull() && !query->value(63).toString().trimmed().isEmpty() && query->value(63).toString().trimmed()!="- нет -")   stream.writeTextElement("NPNAME", query->value(63).toString());
                            if (!query->value(64).isNull() && !query->value(64).toString().trimmed().isEmpty() && query->value(64).toString().trimmed()!="- нет -")   stream.writeTextElement("UL", query->value(64).toString());
                            if (!query->value(65).isNull())                                                       stream.writeTextElement("DOM", QString::number(query->value(65).toInt()));
                            if (!query->value(66).isNull() && !query->value(66).toString().trimmed().isEmpty())   stream.writeTextElement("KORP", query->value(66).toString());
                            if (!query->value(67).isNull() && !query->value(67).toString().trimmed().isEmpty())   stream.writeTextElement("KV", query->value(67).toString());
                            if (!query->value(57).isNull() && query->value(57).toDate()>QDate(1920,1,1))         stream.writeTextElement("DREG", query->value(57).toDate().toString("yyyy-MM-dd"));
                        }
                        stream.writeEndElement(); // ADDRES_G
                    //}


                    //if (event_cod_!="П02") {
                        stream.writeStartElement("ADDRES_P");
                        {
                            if (data_app.FIAS_flag) {
                                if (!query->value(101).isNull() && !query->value(101).toString().trimmed().isEmpty())  stream.writeTextElement("FIAS_AOID", query->value(101).toString());
                                if (!query->value(102).isNull() && !query->value(102).toString().trimmed().isEmpty())  stream.writeTextElement("FIAS_HOUSEID", query->value(102).toString());
                            }
                            if (!query->value(69).isNull() && !query->value(69).toString().trimmed().isEmpty())   stream.writeTextElement("SUBJ", query->value(69).toString());
                            if (!query->value(68).isNull() && !query->value(68).toString().trimmed().isEmpty())   stream.writeTextElement("INDX", query->value(68).toString());
                            if (!query->value(70).isNull() && !query->value(70).toString().trimmed().isEmpty())   stream.writeTextElement("OKATO", query->value(70).toString());
                            if (!query->value(71).isNull() && !query->value(71).toString().trimmed().isEmpty() && query->value(71).toString().trimmed()!="- нет -")   stream.writeTextElement("RNNAME", query->value(71).toString());
                            if (!query->value(72).isNull() && !query->value(72).toString().trimmed().isEmpty() && query->value(72).toString().trimmed()!="- нет -")   stream.writeTextElement("NPNAME", query->value(72).toString());
                            if (!query->value(73).isNull() && !query->value(73).toString().trimmed().isEmpty() && query->value(73).toString().trimmed()!="- нет -")   stream.writeTextElement("UL", query->value(73).toString());
                            if (!query->value(74).isNull())                                                       stream.writeTextElement("DOM", QString::number(query->value(74).toInt()));
                            if (!query->value(75).isNull() && !query->value(75).toString().trimmed().isEmpty())   stream.writeTextElement("KORP", query->value(75).toString());
                            if (!query->value(76).isNull() && !query->value(76).toString().trimmed().isEmpty())   stream.writeTextElement("KV", query->value(76).toString());
                        }
                        stream.writeEndElement(); // ADDRES_P
                    //}


                    int f_polis = query->value(82).toInt();

                    if ( event_cod_!="П02"
                         && event_cod_!="П04") {
                        stream.writeStartElement("VIZIT");
                        {
                            if ( !(query->value(10).isNull()) ) {
                                {
                                    if ( query->value(10).isNull() || query->value(10).toDate()<=QDate(1920,1,1) )
                                        /* если дата визита не указана - дата выдачи полиса*/
                                        stream.writeTextElement("DVIZIT",  query->value(77).toDate().toString("yyyy-MM-dd"));
                                    else
                                        stream.writeTextElement("DVIZIT",  query->value(10).toDate().toString("yyyy-MM-dd"));


                                    if (!query->value(78).isNull())
                                        stream.writeTextElement("METHOD",  QString::number(query->value(78).toInt()));
                                    else if ( query->value(35).isNull() || query->value(35).toString().trimmed().isEmpty())
                                         stream.writeTextElement("METHOD",  "1");
                                    else stream.writeTextElement("METHOD",  "2");

                                    if (!query->value(79).isNull())                                 stream.writeTextElement("PETITION",QString::number(query->value(79).toInt()));
                                    else                                                            stream.writeTextElement("PETITION","0");

                                    //QString event_code = query->value(83).toString().toUpper();
                                    if (event_code!="П040") {
                                        if (!query->value(80).isNull() && query->value(80).toInt()>0)   stream.writeTextElement("RSMO",    QString::number(r_smo));
                                    }
                                    if (!query->value(81).isNull() && query->value(81).toInt()>0)       stream.writeTextElement("RPOLIS",  QString::number(r_polis));
                                    else      stream.writeTextElement("RPOLIS",  "4");

                                    // выберем, откуда читать форму выпуска полиса
                                    int FPOLIS = 1;
                                    //event_code = query->value(83).toString().toUpper();
                                    if ( event_code=="П010" ||
                                         event_code=="П034" ||
                                         event_code=="П035" ||
                                         event_code=="П036" ||
                                         event_code=="П061" ||
                                         event_code=="П062" ||
                                         event_code=="П063" ) {
                                        // для ВС форма выпуска берётся по выбранной форме выпуска из визита
                                        if (!query->value(82).isNull())
                                            FPOLIS = query->value(82).toInt();
                                    } else if
                                       ( event_code=="П031" ||
                                         event_code=="П032" ||
                                         event_code=="П033" ||
                                         event_code=="П060" ) {
                                        // для полиса единого образца - по форме выпуска бланка
                                        if (!query->value(98).isNull())
                                            FPOLIS = query->value(98).toInt();
                                    }
                                    if (FPOLIS<1)
                                        FPOLIS=1;
                                    stream.writeTextElement("FPOLIS",  QString::number(FPOLIS));
                                }
                            } else {
                                // сформируем данные визита по данным события
                                {
                                    if ( query->value(10).isNull() || query->value(10).toDate()<=QDate(1920,1,1) )
                                        /* если дата визита не указана - дата выдачи полиса*/
                                        stream.writeTextElement("DVIZIT",  query->value(84).toDate().toString("yyyy-MM-dd"));
                                    else
                                        stream.writeTextElement("DVIZIT",  query->value(84).toDate().toString("yyyy-MM-dd"));


                                    if (!query->value(78).isNull())
                                        stream.writeTextElement("METHOD",  QString::number(query->value(78).toInt()));
                                    else if ( query->value(35).isNull() || query->value(35).toString().trimmed().isEmpty())
                                         stream.writeTextElement("METHOD",  "1");
                                    else stream.writeTextElement("METHOD",  "2");

                                    stream.writeTextElement("PETITION", "0");

                                    //QString event_code = query->value(83).toString().toUpper();
                                    if (event_code!="П040") {
                                        if ( event_code=="П010")
                                            stream.writeTextElement("RSMO", "1");
                                        else if ( event_code=="П031" ||
                                                  event_code=="П034" )
                                            stream.writeTextElement("RSMO", "2");
                                        else if ( event_code=="П032" ||
                                                  event_code=="П035" )
                                            stream.writeTextElement("RSMO", "3");
                                        else if ( event_code=="П033" ||
                                                  event_code=="П036" )
                                            stream.writeTextElement("RSMO", "4");
                                        else stream.writeTextElement("RSMO", QString::number(r_smo));
                                    }

                                    if ( event_code=="П061")
                                        stream.writeTextElement("RPOLIS", "1");
                                    else if ( event_code=="П062" )
                                        stream.writeTextElement("RPOLIS", "4");
                                    else if ( event_code=="П063" )
                                        stream.writeTextElement("RPOLIS", "5");
                                    else stream.writeTextElement("RPOLIS", QString::number(r_polis));

                                    int FPOLIS = query->value(98).toInt();
                                    if (FPOLIS<1)
                                        FPOLIS=1;
                                    stream.writeTextElement("FPOLIS",  QString::number(FPOLIS));
                                }
                            }
                        }
                        stream.writeEndElement(); // VIZIT
                    } else {
                        // сформируем данные визита по данным события
                        stream.writeStartElement("VIZIT");
                        {
                            if ( query->value(10).isNull() || query->value(10).toDate()<=QDate(1920,1,1) )
                                /* если дата визита не указана - дата выдачи полиса*/
                                stream.writeTextElement("DVIZIT",  query->value(84).toDate().toString("yyyy-MM-dd"));
                            else
                                stream.writeTextElement("DVIZIT",  query->value(10).toDate().toString("yyyy-MM-dd"));


                            if (!query->value(78).isNull())
                                stream.writeTextElement("METHOD",  QString::number(query->value(78).toInt()));
                            else if ( query->value(35).isNull() || query->value(35).toString().trimmed().isEmpty())
                                 stream.writeTextElement("METHOD",  "1");
                            else stream.writeTextElement("METHOD",  "2");

                            stream.writeTextElement("PETITION", "0");

                            if ( event_code=="П010")
                                stream.writeTextElement("RSMO", "1");
                            else if ( event_code=="П031" ||
                                      event_code=="П034" )
                                stream.writeTextElement("RSMO", "2");
                            else if ( event_code=="П032" ||
                                      event_code=="П035" )
                                stream.writeTextElement("RSMO", "3");
                            else if ( event_code=="П033" ||
                                      event_code=="П036" )
                                stream.writeTextElement("RSMO", "4");
                            else stream.writeTextElement("RSMO", QString::number(r_smo));

                            if ( event_code=="П061")
                                stream.writeTextElement("RPOLIS", "1");
                            else if ( event_code=="П062" )
                                stream.writeTextElement("RPOLIS", "4");
                            else if ( event_code=="П063" )
                                stream.writeTextElement("RPOLIS", "5");
                            else stream.writeTextElement("RPOLIS", QString::number(r_polis));

                            int FPOLIS = query->value(98).toInt();
                            if (FPOLIS<1)
                                FPOLIS=1;
                            stream.writeTextElement("FPOLIS",  QString::number(FPOLIS));
                        }
                        stream.writeEndElement(); // VIZIT
                    }


                    //if (event_cod_!="П02") {
                        int pol_v = query->value( 4).toInt();
                        stream.writeStartElement("INSURANCE");
                        {
                            if (!query->value( 1).isNull() && !query->value( 1).toString().trimmed().isEmpty())   stream.writeTextElement("TER_ST", query->value( 1).toString());
                            if (!query->value( 2).isNull() && !query->value( 2).toString().trimmed().isEmpty())   stream.writeTextElement("ENP", query->value( 2).toString());
                            if (!query->value( 3).isNull() && !query->value( 3).toString().trimmed().isEmpty())   stream.writeTextElement("OGRNSMO", query->value( 3).toString());

                            stream.writeStartElement("POLIS");
                            {
                                if (!query->value( 4).isNull())   stream.writeTextElement("VPOLIS", QString::number(pol_v));
                                if (pol_v==1) {
                                    if (!query->value( 8).isNull() && !query->value( 8).toString().trimmed().isEmpty())   stream.writeTextElement("NPOLIS", query->value( 8).toString());
                                    if (!query->value( 7).isNull() && !query->value( 7).toString().trimmed().isEmpty()) {
                                        QString s = "0000" + query->value( 7).toString();
                                        stream.writeTextElement("SPOLIS", s.right(4));
                                    }
                                } else if (pol_v==2) {
                                    if (!query->value( 9).isNull() && !query->value( 9).toString().trimmed().isEmpty())   stream.writeTextElement("NPOLIS", query->value( 9).toString());
                                } else if (pol_v==3) {
                                    if (!query->value( 8).isNull() && !query->value( 8).toString().trimmed().isEmpty())
                                        stream.writeTextElement("NPOLIS", QString("0000" + query->value( 7).toString()).right(4) + QString("0000000" + query->value( 8).toString()).right(7));

                                    /*if (!query->value( 7).isNull() && !query->value( 7).toString().trimmed().isEmpty()) {
                                        QString s = "0000" + query->value( 7).toString();
                                        stream.writeTextElement("SPOLIS", s.right(4));
                                    }*/
                                }

                                // в Курске дата начала страхования - это дата выдачи ВС
                                if ( data_app.ocato=="38000" ) {
                                    if (!query->value(110).isNull() && query->value(110).toDate()>QDate(1920,1,1)) {
                                        stream.writeTextElement("DBEG",  query->value(110).toDate().toString("yyyy-MM-dd"));
                                    } else if (!query->value(10).isNull() && query->value(10).toDate()>QDate(1920,1,1)) {
                                        stream.writeTextElement("DBEG",  query->value(10).toDate().toString("yyyy-MM-dd"));
                                    }
                                }
                                // ... для остальных - это дата выдачи полиса
                                else {
                                    if (!query->value(10).isNull() && query->value(10).toDate()>QDate(1920,1,1))
                                        stream.writeTextElement("DBEG",  query->value(10).toDate().toString("yyyy-MM-dd"));
                                }
                                // Плановая дата окончания страхования - всегда DEND полиса
                                if (!query->value(11).isNull() && query->value(11).toDate()>QDate(1920,1,1))
                                    stream.writeTextElement("DEND",  query->value(11).toDate().toString("yyyy-MM-dd"));


                                // DSTOP только для времянок ...
                                if (query->value(4).toInt()==2) {
                                    if (!query->value(12).isNull() && query->value(12).toDate()>QDate(1920,1,1)) {
                                        stream.writeTextElement("DSTOP", query->value(12).toDate().toString("yyyy-MM-dd"));
                                    }
                                }
                                // ... и изъятых полисов единого образца
                                else if ( query->value(4).toInt()==3
                                          && query->value(83).toString().trimmed().toUpper().left(3)=="П02" ) {
                                    if (!query->value(12).isNull() && query->value(12).toDate()>QDate(1920,1,1))   {
                                        stream.writeTextElement("DSTOP", query->value(12).toDate().toString("yyyy-MM-dd"));
                                    } else if (!query->value(11).isNull()) {
                                        stream.writeTextElement("DSTOP",  query->value(11).toDate().toString("yyyy-MM-dd"));
                                    }
                                }
                                // ... для остальных иностранцев, в Белгороде
                                else if ( data_app.ocato=="14000" &&
                                          (query->value(21).toInt()==2 ||
                                           query->value(21).toInt()==3 ||
                                           query->value(21).toInt()==4 ||
                                           query->value(21).toInt()==5 ||
                                           query->value(21).toInt()==7 ||
                                           query->value(21).toInt()==8 ||
                                           query->value(21).toInt()==9 ||
                                           query->value(21).toInt()==10) ) {
                                    if (!query->value(12).isNull() && query->value(12).toDate()>QDate(1920,1,1))   {
                                        stream.writeTextElement("DSTOP", query->value(12).toDate().toString("yyyy-MM-dd"));
                                    } else if (!query->value(11).isNull()) {
                                        stream.writeTextElement("DSTOP",  query->value(11).toDate().toString("yyyy-MM-dd"));
                                    }
                                }
                            }
                            stream.writeEndElement(); // POLIS

                            stream.writeTextElement("ERP", (query->value(2).isNull() ? ( (query->value(13).isNull() || query->value(13).toInt()==0) ? "0" : QString::number(query->value(13).toInt()) ) : "1") );
                            //stream.writeTextElement("ERP", (query->value(13).isNull() ? "0" : QString::number(query->value(13).toInt())));
                            //(query->value(13).isNull() ? "0" : QString::number(query->value(13).toInt())));

                            if (!query->value(14).isNull() && !query->value(14).toString().trimmed().isEmpty() && !query->value(15).isNull()) {
                                stream.writeStartElement("ORDERZ");
                                {
                                    stream.writeTextElement("NORDER", query->value(14).toString());
                                    stream.writeTextElement("DORDER", query->value(15).toDate().toString("yyyy-MM-dd"));
                                }
                                stream.writeEndElement(); // ORDERZ
                            }
                        }
                        stream.writeEndElement(); // INSURANCE
                    //}


                    //if (event_cod_!="П02") {
                        if ( (f_polis==0 || f_polis==2 || f_polis==3) &&
                             ( (!query->value(55).isNull()) && (!query->value(56).isNull()) ) ) {
                            stream.writeStartElement("PERSONB");
                            {
                                if (!query->value(55).isNull())   stream.writeTextElement("TYPE", "2");
                                if (!query->value(55).isNull())   stream.writeTextElement("PHOTO", query->value(55).toByteArray().toBase64());
                            }
                            stream.writeEndElement(); // PERSONB

                            stream.writeStartElement("PERSONB");
                            {
                                if (!query->value(56).isNull())   stream.writeTextElement("TYPE", "3");
                                if (!query->value(56).isNull())   stream.writeTextElement("PHOTO", query->value(56).toByteArray().toBase64());
                            }
                            stream.writeEndElement(); // PERSONB
                        }
                    //}


                    if (event_code!="П060") {
                        stream.writeStartElement("DOC_LIST");
                        {
                            int id_udl = query->value(104).toInt();
                            int id_drp = query->value(105).toInt();
                            if ( !query->value(37).isNull() ) {
                                stream.writeStartElement("DOC");
                                {
                                    if (!query->value(37).isNull())                                                       stream.writeTextElement("DOCTYPE", QString::number(query->value(37).toInt()));
                                    else                                                                                  stream.writeTextElement("DOCTYPE", "14");
                                    if (!query->value(39).isNull() && !query->value(39).toString().trimmed().isEmpty())   stream.writeTextElement("DOCSER", query->value(39).toString());
                                    if (!query->value(40).isNull() && !query->value(40).toString().trimmed().isEmpty())   stream.writeTextElement("DOCNUM", query->value(40).toString());
                                    else                                                                                  stream.writeTextElement("DOCNUM", "-1");
                                    if (!query->value(41).isNull() && query->value(41).toDate()>QDate(1920,1,1))          stream.writeTextElement("DOCDATE", query->value(41).toDate().toString("yyyy-MM-dd"));
                                    else                                                                                  stream.writeTextElement("DOCDATE", QDate::currentDate().toString("yyyy-MM-dd"));
                                    if (!query->value(42).isNull() && query->value(42).toDate()>QDate(1920,1,1))          stream.writeTextElement("DOCEXP", query->value(42).toDate().toString("yyyy-MM-dd"));
                                    if (!query->value(43).isNull() && !query->value(43).toString().trimmed().isEmpty())   stream.writeTextElement("NAME_VP", query->value(43).toString().left(80));
                                }
                                stream.writeEndElement(); // DOC
                            }

                            if ( id_drp!=id_udl
                                 && !query->value(46).isNull() ) {
                                stream.writeStartElement("DOC");
                                {
                                    if (!query->value(46).isNull())                                                       stream.writeTextElement("DOCTYPE", QString::number(query->value(46).toInt()));
                                    if (!query->value(48).isNull() && !query->value(48).toString().trimmed().isEmpty())   stream.writeTextElement("DOCSER", query->value(48).toString());
                                    if (!query->value(49).isNull() && !query->value(49).toString().trimmed().isEmpty())   stream.writeTextElement("DOCNUM", query->value(49).toString());
                                    if (!query->value(50).isNull() && query->value(50).toDate()>QDate(1920,1,1))          stream.writeTextElement("DOCDATE", query->value(50).toDate().toString("yyyy-MM-dd"));
                                    if (!query->value(51).isNull() && query->value(51).toDate()>QDate(1920,1,1))          stream.writeTextElement("DOCEXP", query->value(51).toDate().toString("yyyy-MM-dd"));
                                    if (!query->value(52).isNull() && !query->value(52).toString().trimmed().isEmpty())   stream.writeTextElement("NAME_VP", query->value(52).toString().left(80));
                                }
                                stream.writeEndElement(); // DOC
                            }
                        }
                        stream.writeEndElement(); // DOC_LIST

                    } else {
                        bool has_P010_udl = true;
                        bool has_udl_type, has_udl_ser, has_udl_num, has_udl_org, has_udl_date, has_udl_exp;
                        int udl_type;
                        QString udl_ser, udl_num, udl_org;
                        QDate udl_date, udl_exp;

                        bool has_P010_drp = true;
                        bool has_drp_type, has_drp_ser, has_drp_num, has_drp_org, has_drp_date, has_drp_exp;
                        int drp_type;
                        QString drp_ser, drp_num, drp_org;
                        QDate drp_date, drp_exp;

                        // если есть - получим данные УДЛ последнего события П010, П034, П035, П036, П061, П062, П063
                        QString sql_udl = "select dd.doc_type, dd.doc_ser, dd.doc_num, dd.doc_date, dd.doc_exp, dd.doc_org "
                                          "  from persons_docs dd "
                                          "  left join events aa on(aa.id_udl=dd.id) "
                                          " where aa.id=( select max(a.id) "
                                          "                 from events a "
                                          "                where a.id_person=" + QString::number(query->value(16).toInt()) + " "
                                          "                  and a.event_code in('П010', 'П034', 'П035', 'П036', 'П061', 'П062', 'П063') ) ";

                        QSqlQuery *query_udl = new QSqlQuery(db);
                        if (!mySQL.exec(this, sql_udl, "Получим УДЛ на событие по выдаче ВС", *query_udl, true, db, data_app)) {
                            QMessageBox::warning(this, "Операция отменена",
                                                 "При попытке получить УДЛ на событие по выдаче ВС произошла неожиданная ошибка.\n\n"
                                                 "Будет использован текущй документ персоны.");
                            delete query_udl;
                            has_P010_udl = false;
                            db.rollback();
                            return false;
                        }
                        if (query_udl->next()) {
                            udl_type = query_udl->value(0).toInt();
                            udl_ser  = query_udl->value(1).toString();
                            udl_num  = query_udl->value(2).toString();
                            udl_date = query_udl->value(3).toDate();
                            udl_exp  = query_udl->value(4).toDate();
                            udl_org  = query_udl->value(5).toString();

                            has_udl_type = !query_udl->value(0).isNull();
                            has_udl_ser  = !query_udl->value(1).isNull() && !query_udl->value(1).toString().isEmpty();
                            has_udl_num  = !query_udl->value(2).isNull() && !query_udl->value(2).toString().isEmpty();
                            has_udl_date = !query_udl->value(3).isNull();
                            has_udl_exp  = !query_udl->value(4).isNull();
                            has_udl_org  = !query_udl->value(5).isNull();
                        } else
                            has_P010_udl = false;
                        delete query_udl;

                        if (!has_P010_udl) {
                            udl_type = query->value(37).toInt();
                            udl_ser  = query->value(39).toString();
                            udl_num  = query->value(40).toString();
                            udl_date = query->value(41).toDate();
                            udl_exp  = query->value(42).toDate();
                            udl_org  = query->value(43).toString();

                            has_udl_type = !query->value(37).isNull();
                            has_udl_ser  = !query->value(39).isNull() && !query->value(39).toString().isEmpty();
                            has_udl_num  = !query->value(40).isNull() && !query->value(40).toString().isEmpty();
                            has_udl_date = !query->value(41).isNull();
                            has_udl_exp  = !query->value(42).isNull();
                            has_udl_org  = !query->value(43).isNull();
                        }

                        // если есть - получим данные ДРП последнего события П010, П034, П035, П036, П061, П062, П063
                        QString sql_drp = "select dd.doc_type, dd.doc_ser, dd.doc_num, dd.doc_date, dd.doc_exp, dd.doc_org "
                                          "  from persons_docs dd "
                                          "  left join events aa on(aa.id_drp=dd.id) "
                                          " where aa.id=( select max(a.id) "
                                          "                 from events a "
                                          "                where a.id_person=" + QString::number(query->value(16).toInt()) + " "
                                          "                  and a.event_code in('П010', 'П034', 'П035', 'П036', 'П061', 'П062', 'П063') ) ";

                        QSqlQuery *query_drp = new QSqlQuery(db);
                        if (!mySQL.exec(this, sql_drp, "Получим ДРП на событие по выдаче ВС", *query_drp, true, db, data_app)) {
                            QMessageBox::warning(this, "Операция отменена",
                                                 "При попытке получить ДРП на событие по выдаче ВС произошла неожиданная ошибка.\n\n"
                                                 "Будет использован текущй документ персоны.");
                            delete query_drp;
                            has_P010_drp = false;
                            db.rollback();
                            return false;
                        }
                        if (query_drp->next()) {
                            drp_type = query_drp->value(0).toInt();
                            drp_ser  = query_drp->value(1).toString();
                            drp_num  = query_drp->value(2).toString();
                            drp_date = query_drp->value(3).toDate();
                            drp_exp  = query_drp->value(4).toDate();
                            drp_org  = query_drp->value(5).toString();

                            has_drp_type = !query_drp->value(0).isNull();
                            has_drp_ser  = !query_drp->value(1).isNull() && !query_drp->value(1).toString().isEmpty();
                            has_drp_num  = !query_drp->value(2).isNull() && !query_drp->value(2).toString().isEmpty();
                            has_drp_date = !query_drp->value(3).isNull();
                            has_drp_exp  = !query_drp->value(4).isNull();
                            has_drp_org  = !query_drp->value(5).isNull();
                        } else
                            has_P010_drp = false;
                        delete query_drp;

                        if (!has_P010_drp) {
                            drp_type = query->value(46).toInt();
                            drp_ser  = query->value(48).toString();
                            drp_num  = query->value(49).toString();
                            drp_date = query->value(50).toDate();
                            drp_exp  = query->value(51).toDate();
                            drp_org  = query->value(52).toString();

                            has_drp_type = !query->value(46).isNull();
                            has_drp_ser  = !query->value(48).isNull() && !query->value(48).toString().isEmpty();
                            has_drp_num  = !query->value(49).isNull() && !query->value(49).toString().isEmpty();
                            has_drp_date = !query->value(50).isNull();
                            has_drp_exp  = !query->value(51).isNull();
                            has_drp_org  = !query->value(52).isNull();
                        }



                        stream.writeStartElement("DOC_LIST");
                        {
                            int id_udl = query->value(104).toInt();
                            int id_drp = query->value(105).toInt();
                            if ( !query->value(37).isNull() ) {
                                stream.writeStartElement("DOC");
                                {
                                    if (has_udl_type)   stream.writeTextElement("DOCTYPE", QString::number(udl_type));
                                    else                stream.writeTextElement("DOCTYPE", "14");
                                    if (has_udl_ser )   stream.writeTextElement("DOCSER",  udl_ser);
                                    if (has_udl_num )   stream.writeTextElement("DOCNUM",  udl_num);
                                    else                stream.writeTextElement("DOCNUM",  "-1");
                                    if (has_udl_date)   stream.writeTextElement("DOCDATE", udl_date.toString("yyyy-MM-dd"));
                                    else                stream.writeTextElement("DOCDATE", QDate::currentDate().toString("yyyy-MM-dd"));
                                    if (has_udl_exp )   stream.writeTextElement("DOCEXP",  udl_exp.toString("yyyy-MM-dd"));
                                    if (has_udl_org )   stream.writeTextElement("NAME_VP", udl_org.left(80));
                                }
                                stream.writeEndElement(); // DOC
                            }

                            if ( id_drp!=id_udl
                                 && !query->value(46).isNull() ) {
                                stream.writeStartElement("DOC");
                                {
                                    if (has_drp_type)   stream.writeTextElement("DOCTYPE", QString::number(drp_type));
                                    if (has_drp_ser )   stream.writeTextElement("DOCSER",  drp_ser);
                                    if (has_drp_num )   stream.writeTextElement("DOCNUM",  drp_num);
                                    if (has_drp_date)   stream.writeTextElement("DOCDATE", drp_date.toString("yyyy-MM-dd"));
                                    if (has_drp_exp )   stream.writeTextElement("DOCEXP",  drp_exp.toString("yyyy-MM-dd"));
                                    if (has_drp_org )   stream.writeTextElement("NAME_VP", drp_org.left(80));
                                }
                                stream.writeEndElement(); // DOC
                            }
                        }
                        stream.writeEndElement(); // DOC_LIST
                    }


                    if (event_code!="П060") {
                        // выберем все старые документы
                        QString sql_old_docs = "select doc_type, doc_ser, doc_num, doc_date, doc_exp, doc_org, org_code, status "
                                               "  from persons_docs d "
                                               " where d.id_person=" + QString::number(query->value(16).toInt()) + " "
                                               "   and d.status=2 ; ";

                        QSqlQuery *query_old_docs = new QSqlQuery(db);
                        if (!mySQL.exec(this, sql_old_docs, "Выбор старых документов застрахованного", *query_old_docs, true, db, data_app)) {
                            delete query_old_docs;
                            QMessageBox::warning(this, "Операция отменена",
                                                 "При запросе старых документов застрахованного произошла непредвиденная ошибка.\n\n"
                                                 "Операция выборки старых документов отменена.");
                            db.rollback();
                            return false;
                        } else {
                            if (query_old_docs->size()>0) {
                                stream.writeStartElement("OLDDOC_LIST");
                                while (query_old_docs->next()) {
                                    stream.writeStartElement("OLD_DOC");
                                    {
                                        if ( !query_old_docs->value(0).isNull())                        stream.writeTextElement("DOCTYPE", QString::number(query_old_docs->value(0).toInt()));
                                        else                                                            stream.writeTextElement("DOCTYPE", "14");
                                        if ( !query_old_docs->value(1).isNull() &&
                                             !query_old_docs->value(1).toString().trimmed().isEmpty())  stream.writeTextElement("DOCSER", query_old_docs->value(1).toString());
                                        if ( !query_old_docs->value(2).isNull() &&
                                             !query_old_docs->value(2).toString().trimmed().isEmpty())  stream.writeTextElement("DOCNUM", query_old_docs->value(2).toString());
                                        else                                                            stream.writeTextElement("DOCNUM", "-1");
                                        if ( !query_old_docs->value(3).isNull() &&
                                             query_old_docs->value(3).toDate()>QDate(1920,1,1))         stream.writeTextElement("DOCDATE", query_old_docs->value(3).toDate().toString("yyyy-MM-dd"));
                                        else                                                            stream.writeTextElement("DOCDATE", QDate::currentDate().toString("yyyy-MM-dd"));
                                        if ( !query_old_docs->value(4).isNull() &&
                                             query_old_docs->value(4).toDate()>QDate(1920,1,1))         stream.writeTextElement("DOCEXP", query_old_docs->value(4).toDate().toString("yyyy-MM-dd"));
                                        if ( !query_old_docs->value(5).isNull() &&
                                             !query_old_docs->value(5).toString().trimmed().isEmpty())  stream.writeTextElement("NAME_VP", query_old_docs->value(5).toString().left(80));
                                    }
                                    stream.writeEndElement(); // OLD_DOC
                                }
                                stream.writeEndElement(); // OLDDOC_LIST
                            }
                        }
                    }

                }
                stream.writeEndElement();  // OP

                if (!f_old_data) {

                    // отмена разногласий на отправленный полис
                    QString sql_protocols = "update frecs_in_p "
                                            "   set status=-1, "
                                            "       date_resend=CURRENT_DATE "
                                            " where id_polis=" + QString::number(query->value(0).toInt()) + " ; ";
                    QSqlQuery *query_protocols = new QSqlQuery(db);
                    if (!mySQL.exec(this, sql_protocols, "Закрытие разногласий, связанных с отпраляемой записью", *query_protocols, true, db, data_app)) {
                        QMessageBox::warning(this, "Операция отменена",
                                             "При при закрытии связанных с отправляемой записью разногласий произошла неожиданная ошибка.\n\n"
                                             "Операция выгрузки данных для ТФОМС отменена");
                        delete query_protocols;
                        db.rollback();
                        return 0;
                    }
                    delete query_protocols;
                }


                // Запомним запись с ID_FILE_I = -1
                QString sql_frec =
                                "insert into frecs_out_i (id_file_i, n_rec, id_person, id_polis, id_vizit, id_event) "
                                " values(-1, "
                                " " + QString::number(n_rec) + ", "
                                " " + QString::number(query->value( 16).toInt()) + ", "
                                " " + QString::number(query->value(  0).toInt()) + ", "
                                " " + QString::number(query->value(103).toInt()) + ", "
                                " " + QString::number(query->value( 89).toInt()) + ") ; ";
                QSqlQuery *query_frec = new QSqlQuery(db);
                mySQL.exec(this, sql_frec, "Добавление frec", *query_frec, true, db, data_app);
                delete query_frec;
            }
        }        
        stream.writeEndElement();  // OPLIST
    }

    if (data_app.xml_vers=="2.1a") {
        stream.writeStartElement("OPLIST"); {
            stream.writeAttribute("FILENAME", f_name);
            stream.writeAttribute("SMOCOD", data_app.smo_regnum);
            stream.writeAttribute("NRECORDS", QString::number(query->size()));
            stream.writeAttribute("PRZCOD", point_regnum);
            stream.writeAttribute("VERS", data_app.xml_vers);
            while ( (n_rec==0 && query->first()) || (n_rec>0 && query->next()) ) {

                int r_smo   = max(query->value(80).toInt(), query->value(108).toInt());
                int r_polis = max(query->value(81).toInt(), query->value(109).toInt());

                id_events_list.append(query->value(89).toInt());
                QString event_code = query->value(83).toString();
                QString event_cod_  = event_code.left(3);

                 n_rec++;
                stream.writeStartElement("","OP");
                stream.writeAttribute("TIP_OP", query->value(83).toString());
                stream.writeAttribute("N_REC", QString::number(n_rec));
                {

                    //if (event_cod_!="П02") {
                        stream.writeStartElement("PERSON");
                        {
                            if (!query->value(17).isNull() && !query->value(17).toString().trimmed().isEmpty())   stream.writeAttribute("FAM", query->value(17).toString());
                            if (!query->value(18).isNull() && !query->value(18).toString().trimmed().isEmpty())   stream.writeAttribute("IM", query->value(18).toString());
                            if (!query->value(19).isNull() && !query->value(19).toString().trimmed().isEmpty())   stream.writeAttribute("OT", query->value(19).toString());
                            if (!query->value(20).isNull())                                                       stream.writeAttribute("W", QString::number(query->value(20).toInt()));
                            else                                                                                  stream.writeAttribute("W", "1");
                            if (!query->value(22).isNull() && query->value(22).toDate()>QDate(1920,1,1))         stream.writeAttribute("DR", query->value(22).toDate().toString("yyyy-MM-dd"));
                            else                                                                                  stream.writeAttribute("DR", QDate::currentDate().toString("yyyy-MM-dd"));
                            if (!query->value(23).isNull() && !query->value(23).toString().trimmed().isEmpty())   stream.writeAttribute("MR", query->value(23).toString().left(100));
                            else                                                                                  stream.writeAttribute("MR", "");
                            if (!query->value(27).isNull() && !query->value(27).toString().trimmed().isEmpty())   stream.writeAttribute("BIRTH_OKSM", query->value(27).toString());
                            else                                                                                  stream.writeAttribute("BIRTH_OKSM", "643");
                            if (!query->value(29).isNull() && !query->value(29).toString().trimmed().isEmpty() && query->value(28).toInt()>0) {
                                stream.writeAttribute("C_OKSM", query->value(29).toString());
                            } else {
                                stream.writeAttribute("C_OKSM", "Б/Г");
                            }
                            if (!query->value(21).isNull())                                                       stream.writeAttribute("KATEG", QString::number(query->value(21).toInt()));
                            else                                                                                  stream.writeAttribute("KATEG", "1");

                            //if (!query->value(32).isNull())                                                       stream.writeAttribute("DOST", QString::number(query->value(32).toInt()));
                            if (!query->value(32).isNull()) {
                                QString dost = query->value(32).toString();
                                if (dost.indexOf("1")>=0)
                                    stream.writeAttribute("DOST", "1");
                                if (dost.indexOf("2")>=0)
                                    stream.writeAttribute("DOST", "2");
                                if (dost.indexOf("3")>=0)
                                    stream.writeAttribute("DOST", "3");
                                if (dost.indexOf("4")>=0)
                                    stream.writeAttribute("DOST", "4");
                                if (dost.indexOf("5")>=0)
                                    stream.writeAttribute("DOST", "5");
                                if (dost.indexOf("6")>=0)
                                    stream.writeAttribute("DOST", "6");
                            }

                            //if (!query->value(25).isNull() && !query->value(25).toString().trimmed().isEmpty())   stream.writeAttribute("SS", query->value(25).toString());
                            if (!query->value(25).isNull() && !query->value(25).toString().trimmed().isEmpty()) {
                                //if (data_app.snils_format==1) {
                                    QString ss = QString(query->value(25).toString() + "00000000000").left(11);
                                    QString ss1 = ss.mid(0,3) + "-" + ss.mid(3,3) + "-" + ss.mid(6,3) + " " + ss.mid(9);
                                    stream.writeAttribute("SS", ss1);
                                //} else {
                                //    stream.writeAttribute("SS", query->value(25).toString());
                                //}
                            }
                            if (!query->value(30).isNull() && !query->value(30).toString().trimmed().isEmpty())   stream.writeAttribute("PHONE", query->value(30).toString());
                            if (!query->value(31).isNull() && !query->value(31).toString().trimmed().isEmpty())   stream.writeAttribute("EMAIL", query->value(31).toString());
                            if (!query->value(24).isNull() && query->value(22).toDate()>QDate(1920,1,1))         stream.writeAttribute("DDEATH", query->value(24).toDate().toString("yyyy-MM-dd"));

                            if (!query->value(35).isNull() && !query->value(35).toString().trimmed().isEmpty())   stream.writeAttribute("FIOPR", query->value(35).toString());
                            if (!query->value(36).isNull() && !query->value(36).toString().trimmed().isEmpty())   stream.writeAttribute("CONTACT", query->value(36).toString());
                        }
                        stream.writeEndElement(); // PERSON

                    //}


                    //if (event_cod_!="П02") {
                        if ( !(query->value(90).isNull() || query->value(90).toString()=="") ||
                             !(query->value(91).isNull() || query->value(91).toString()=="") ||
                             !(query->value(92).isNull() || query->value(92).toString()=="") ||
                             !query->value(93).isNull() ||
                             !(query->value(94).isNull() || query->value(94).toString()=="") ||
                             !(query->value(95).isNull() || query->value(95).toString()=="") ||
                             !(query->value(96).isNull() || query->value(96).toString()=="") )  {
                            stream.writeStartElement("OLD_PERSON");
                            {
                                // если есть какие-нибудь старые данные
                                if ( ( !query->value(90).isNull() && !query->value(90).toString().trimmed().isEmpty() ) ||
                                     ( !query->value(91).isNull() && !query->value(91).toString().trimmed().isEmpty() ) ||
                                     ( !query->value(92).isNull() && !query->value(92).toString().trimmed().isEmpty() ) ||
                                     ( !query->value(93).isNull() )                                                     ||
                                     ( !query->value(94).isNull() && !query->value(94).toString().trimmed().isEmpty() ) ||
                                     ( !query->value(95).isNull() && !query->value(95).toString().trimmed().isEmpty() ) ||
                                     ( !query->value(96).isNull() && !query->value(95).toString().trimmed().isEmpty() ) ) {

                                    // соберём полные данные для идентификации застрахованного
                                    if (!query->value(90).isNull() && !query->value(90).toString().trimmed().isEmpty())   stream.writeAttribute("FAM", query->value(90).toString());
                                    //else stream.writeAttribute("FAM", query->value(17).toString());
                                    if (!query->value(91).isNull() && !query->value(91).toString().trimmed().isEmpty())   stream.writeAttribute("IM", query->value(91).toString());
                                    //else stream.writeAttribute("IM", query->value(18).toString());
                                    if (!query->value(92).isNull() && !query->value(92).toString().trimmed().isEmpty())   stream.writeAttribute("OT", query->value(92).toString());
                                    //else stream.writeAttribute("OT", query->value(19).toString());
                                    if (!query->value(93).isNull())                                                       stream.writeAttribute("W", QString::number(query->value(93).toInt()));
                                    else stream.writeAttribute("W", QString::number(query->value(20).toInt()));
                                    if (!query->value(94).isNull() && query->value(94).toDate()>QDate(1920,1,1))         stream.writeAttribute("DR", query->value(94).toDate().toString("yyyy-MM-dd"));
                                    else stream.writeAttribute("DR", query->value(22).toDate().toString("yyyy-MM-dd"));

                                    // OLD_ENP добавляется только при наличии и при смене ЕНП, ДР или пола
                                    if ( !query->value(96).isNull() &&
                                         ( ( !query->value(96).isNull() && query->value( 2).toString()!=query->value(96).toString() ) ||
                                           ( !query->value(93).isNull() && query->value(20).toString()!=query->value(93).toString() ) ||
                                           ( !query->value(94).isNull() && query->value(22).toDate()!=query->value(94).toDate() ) ) ) {
                                       if (!query->value(96).isNull() && !query->value(96).toString().trimmed().isEmpty())   stream.writeAttribute("OLD_ENP", query->value(96).toString());
                                       else stream.writeAttribute("OLD_ENP", query->value( 2).toString());
                                    }
                                    if (!query->value(95).isNull() && !query->value(95).toString().trimmed().isEmpty())   stream.writeAttribute("MR", query->value(95).toString().left(100));
                                    else stream.writeAttribute("MR", query->value(23).toString().left(100));

                                } else {

                                }
                            }
                            stream.writeEndElement(); // OLD_PERSON
                        }
                    //}


                    //f (event_cod_!="П02") {
                        stream.writeStartElement("ADDRES_G");
                        {
                            if (!query->value(57).isNull() && query->value(57).toDate()>QDate(1920,1,1))         stream.writeAttribute("DREG", query->value(57).toDate().toString("yyyy-MM-dd"));
                            if (!query->value(58).isNull())                                                       stream.writeAttribute("BOMG", QString::number(query->value(58).toInt()));
                            else                                                                                  stream.writeAttribute("BOMG", "0");

                            if (data_app.FIAS_flag) {
                                if (!query->value(99).isNull() && !query->value(99).toString().trimmed().isEmpty())   stream.writeAttribute("FIAS_AOID", query->value(99).toString());
                                if (!query->value(100).isNull()&& !query->value(100).toString().trimmed().isEmpty())  stream.writeAttribute("FIAS_HOUSEID", query->value(100).toString());
                            }
                            if (!query->value(60).isNull() && !query->value(60).toString().trimmed().isEmpty())   stream.writeAttribute("SUBJ", query->value(60).toString());
                            if (!query->value(59).isNull() && !query->value(59).toString().trimmed().isEmpty())   stream.writeAttribute("INDX", query->value(59).toString());
                            if (!query->value(61).isNull() && !query->value(61).toString().trimmed().isEmpty())   stream.writeAttribute("OKATO", query->value(61).toString());
                            if (!query->value(62).isNull() && !query->value(62).toString().trimmed().isEmpty() && query->value(62).toString().trimmed()!="- нет -")   stream.writeAttribute("RNNAME", query->value(62).toString());
                            if (!query->value(63).isNull() && !query->value(63).toString().trimmed().isEmpty() && query->value(63).toString().trimmed()!="- нет -")   stream.writeAttribute("NPNAME", query->value(63).toString());
                            if (!query->value(64).isNull() && !query->value(64).toString().trimmed().isEmpty() && query->value(64).toString().trimmed()!="- нет -")   stream.writeAttribute("UL", query->value(64).toString());
                            if (!query->value(65).isNull())                                                       stream.writeAttribute("DOM", QString::number(query->value(65).toInt()));
                            if (!query->value(66).isNull() && !query->value(66).toString().trimmed().isEmpty())   stream.writeAttribute("KORP", query->value(66).toString());
                            if (!query->value(67).isNull() && !query->value(67).toString().trimmed().isEmpty())   stream.writeAttribute("KV", query->value(67).toString());
                        }
                        stream.writeEndElement(); // ADDRES_G

                        stream.writeStartElement("ADDRES_P");
                        {
                            if (data_app.FIAS_flag) {
                                if (!query->value(101).isNull() && !query->value(101).toString().trimmed().isEmpty())  stream.writeAttribute("FIAS_AOID", query->value(101).toString());
                                if (!query->value(102).isNull() && !query->value(102).toString().trimmed().isEmpty())  stream.writeAttribute("FIAS_HOUSEID", query->value(102).toString());
                            }
                            if (!query->value(69).isNull() && !query->value(69).toString().trimmed().isEmpty())   stream.writeAttribute("SUBJ", query->value(69).toString());
                            if (!query->value(68).isNull() && !query->value(68).toString().trimmed().isEmpty())   stream.writeAttribute("INDX", query->value(68).toString());
                            if (!query->value(70).isNull() && !query->value(70).toString().trimmed().isEmpty())   stream.writeAttribute("OKATO", query->value(70).toString());
                            if (!query->value(71).isNull() && !query->value(71).toString().trimmed().isEmpty() && query->value(71).toString().trimmed()!="- нет -")   stream.writeAttribute("RNNAME", query->value(71).toString());
                            if (!query->value(72).isNull() && !query->value(72).toString().trimmed().isEmpty() && query->value(72).toString().trimmed()!="- нет -")   stream.writeAttribute("NPNAME", query->value(72).toString());
                            if (!query->value(73).isNull() && !query->value(73).toString().trimmed().isEmpty() && query->value(73).toString().trimmed()!="- нет -")   stream.writeAttribute("UL", query->value(73).toString());
                            if (!query->value(74).isNull())                                                       stream.writeAttribute("DOM", QString::number(query->value(74).toInt()));
                            if (!query->value(75).isNull() && !query->value(75).toString().trimmed().isEmpty())   stream.writeAttribute("KORP", query->value(75).toString());
                            if (!query->value(76).isNull() && !query->value(76).toString().trimmed().isEmpty())   stream.writeAttribute("KV", query->value(76).toString());
                        }
                        stream.writeEndElement(); // ADDRES_P
                    //}


                    int f_polis = query->value(82).toInt();


                    stream.writeStartElement("VIZIT");
                    if ( event_cod_!="П02"
                         && event_cod_!="П04" ) {
                        if ( !(query->value(77).isNull()) ) {
                            {
                                if ( query->value(77).isNull() || query->value(77).toDate()<=QDate(1920,1,1) )
                                    /* если дата визита не указана - дата выдачи полиса*/
                                    stream.writeAttribute("DVIZIT",  query->value(10).toDate().toString("yyyy-MM-dd"));
                                else
                                    stream.writeAttribute("DVIZIT",  query->value(77).toDate().toString("yyyy-MM-dd"));



                                if (!query->value(78).isNull())
                                    stream.writeAttribute("METHOD",  QString::number(query->value(78).toInt()));
                                else if ( query->value(35).isNull() || query->value(35).toString().trimmed().isEmpty())
                                     stream.writeAttribute("METHOD",  "1");
                                else stream.writeAttribute("METHOD",  "2");

                                if (!query->value(79).isNull())                                               stream.writeAttribute("PETITION",QString::number(query->value(79).toInt()));
                                else                                                                          stream.writeAttribute("PETITION","0");

                                //QString event_code = query->value(83).toString().toUpper();
                                if (event_code!="П040") {
                                    if (!query->value(80).isNull() && query->value(80).toInt()>0)                 stream.writeAttribute("RSMO",    QString::number(r_smo));
                                }
                                if (!query->value(81).isNull() && query->value(81).toInt()>0)                 stream.writeAttribute("RPOLIS",  QString::number(r_polis));

                                // выберем, откуда читать форму выпуска полиса
                                int FPOLIS = 1;
                                event_code = query->value(83).toString().toUpper();
                                if ( event_code=="П010" ||
                                     event_code=="П034" ||
                                     event_code=="П035" ||
                                     event_code=="П036" ||
                                     event_code=="П061" ||
                                     event_code=="П062" ||
                                     event_code=="П063" ) {
                                    // для ВС форма выпуска берётся по выбранной форме выпуска из визита
                                    if (!query->value(82).isNull())
                                        FPOLIS = query->value(82).toInt();
                                } else if
                                   ( event_code=="П031" ||
                                     event_code=="П032" ||
                                     event_code=="П033" ||
                                     event_code=="П060" ) {
                                    // для полиса единого образца - по форме выпуска бланка
                                    if (!query->value(98).isNull())
                                        FPOLIS = query->value(98).toInt();
                                }
                                if (FPOLIS<1)
                                    FPOLIS=1;
                                stream.writeAttribute("FPOLIS",  QString::number(FPOLIS));
                            }

                        } else {
                            // сформируем данные визита по данным события
                            {
                                if ( query->value(84).isNull() || query->value(84).toDate()<=QDate(1920,1,1) )
                                    /* если дата визита не указана - дата выдачи полиса*/
                                    stream.writeAttribute("DVIZIT",  query->value(10).toDate().toString("yyyy-MM-dd"));
                                else
                                if ( query->value(84).toDate()<QDate::currentDate().addDays(-2) ) {
                                    // если дата визита раньше чем 3 дня назад - отправляем текущую дату
                                    stream.writeAttribute("DVIZIT",  QDate::currentDate().toString("yyyy-MM-dd"));

                                    // !!!!!!!! событие устарело - подправим дату на текущую !!!!!!!! //
                                    QString sql_corr;
                                    sql_corr = "update events "
                                               "   set event_dt='" + QDate::currentDate().toString("yyyy-MM-dd") + "' "
                                               " where id=" + QString::number(query->value(89).toInt()) + " ; ";
                                    sql_corr+= "update vizits "
                                               "   set v_date='" + QDate::currentDate().toString("yyyy-MM-dd") + "' "
                                               " where id in(select id_vizit from events where id=" + QString::number(query->value(89).toInt()) + ") ; ";
                                    QSqlQuery *query_corr = new QSqlQuery(db);
                                    query_corr->exec( sql_corr );
                                    delete query_corr;
                                } else
                                    stream.writeAttribute("DVIZIT",  query->value(84).toDate().toString("yyyy-MM-dd"));


                                stream.writeAttribute("METHOD",  "1");
                                stream.writeAttribute("PETITION","0");

                                //QString event_code = query->value(83).toString().toUpper();
                                if (event_code!="П040") {
                                    if (query->value(83).toString()=="П010")
                                        stream.writeAttribute("RSMO", "1");
                                    else if ( event_code=="П031" ||
                                              event_code=="П034" )
                                        stream.writeAttribute("RSMO", "2");
                                    else if ( event_code=="П032" ||
                                              event_code=="П035" )
                                        stream.writeAttribute("RSMO", "3");
                                    else if ( event_code=="П033" ||
                                              event_code=="П036" )
                                        stream.writeAttribute("RSMO", "4");
                                    else stream.writeAttribute("RSMO", QString::number(r_smo));
                                }

                                if ( event_code=="П061")
                                    stream.writeAttribute("RPOLIS", "1");
                                else if ( event_code=="П062")
                                    stream.writeAttribute("RPOLIS", "4");
                                else if ( event_code=="П063")
                                    stream.writeAttribute("RPOLIS", "5");
                                else stream.writeAttribute("RPOLIS", QString::number(r_polis));


                                int FPOLIS = query->value(98).toInt();
                                if (FPOLIS<1)
                                    FPOLIS=1;
                                stream.writeAttribute("FPOLIS",  QString::number(FPOLIS));
                            }
                        }
                    } else {
                        if ( query->value(77).isNull() || query->value(77).toDate()<=QDate(1920,1,1) )
                            /* если дата визита не указана - дата выдачи полиса*/
                            stream.writeAttribute("DVIZIT",  query->value(10).toDate().toString("yyyy-MM-dd"));
                        else
                            stream.writeAttribute("DVIZIT",  query->value(77).toDate().toString("yyyy-MM-dd"));


                        if (!query->value(78).isNull())
                            stream.writeAttribute("METHOD",  QString::number(query->value(78).toInt()));
                        else if ( query->value(35).isNull() || query->value(35).toString().trimmed().isEmpty())
                             stream.writeAttribute("METHOD",  "1");
                        else stream.writeAttribute("METHOD",  "2");

                        if (!query->value(79).isNull())                                               stream.writeAttribute("PETITION",QString::number(query->value(79).toInt()));
                        else                                                                          stream.writeAttribute("PETITION","0");

                        //QString event_code = query->value(83).toString().toUpper();
                        if (event_code!="П040") {
                            if (!query->value(80).isNull() && query->value(80).toInt()>0)             stream.writeAttribute("RSMO",    QString::number(r_smo));
                        }
                        if (!query->value(81).isNull() && query->value(81).toInt()>0)                 stream.writeAttribute("RPOLIS",  QString::number(r_polis));

                        // выберем, откуда читать форму выпуска полиса
                        int FPOLIS = 1;
                        event_code = query->value(83).toString().toUpper();
                        if ( event_code=="П010" ||
                             event_code=="П034" ||
                             event_code=="П035" ||
                             event_code=="П036" ||
                             event_code=="П061" ||
                             event_code=="П062" ||
                             event_code=="П063" ) {
                            // для ВС форма выпуска берётся по выбранной форме выпуска из визита
                            if (!query->value(82).isNull())
                                FPOLIS = query->value(82).toInt();
                        } else if
                           ( event_code=="П031" ||
                             event_code=="П032" ||
                             event_code=="П033" ||
                             event_code=="П060" ) {
                            // для полиса единого образца - по форме выпуска бланка
                            if (!query->value(98).isNull())
                                FPOLIS = query->value(98).toInt();
                        }
                        if (FPOLIS<1)
                            FPOLIS=1;
                        stream.writeAttribute("FPOLIS",  QString::number(FPOLIS));
                    }
                    stream.writeEndElement(); // VIZIT


                    //if (event_cod_!="П02") {
                        int pol_v = query->value( 4).toInt();
                        stream.writeStartElement("INSURANCE");
                        {
                            stream.writeAttribute("ERP", (query->value(2).isNull() ? ( (query->value(13).isNull() || query->value(13).toInt()==0) ? "0" : QString::number(query->value(13).toInt()) ) : "1") );
                            //stream.writeAttribute("ERP", (query->value(13).isNull() ? "0" : QString::number(query->value(13).toInt())));

                            if (!query->value( 1).isNull() && !query->value( 1).toString().trimmed().isEmpty())   stream.writeAttribute("TER_ST", query->value( 1).toString());
                            if (!query->value( 2).isNull() && !query->value( 2).toString().trimmed().isEmpty())   stream.writeAttribute("ENP", query->value( 2).toString());
                            if (!query->value( 3).isNull() && !query->value( 3).toString().trimmed().isEmpty())   stream.writeAttribute("OGRNSMO", query->value( 3).toString());

                            stream.writeStartElement("POLIS");
                            {
                                if (!query->value( 4).isNull())   stream.writeAttribute("VPOLIS", QString::number(pol_v));
                                if (pol_v==1) {
                                    if (!query->value( 7).isNull() && !query->value( 7).toString().trimmed().isEmpty()) {
                                        QString s = "0000" + query->value( 7).toString();
                                        stream.writeAttribute("SPOLIS", s.right(4));
                                    }
                                    stream.writeAttribute("NPOLIS", query->value( 8).toString());
                                } else if (pol_v==2) {
                                    stream.writeAttribute("NPOLIS", query->value( 9).toString());
                                } else if (pol_v==3) {
                                    stream.writeAttribute("NPOLIS", QString("0000" + query->value( 7).toString()).right(4) + QString("0000000" + query->value( 8).toString()).right(7));
                                    //stream.writeAttribute("NPOLIS", QString("0000000" + query->value( 8).toString()).right(7));
                                    //stream.writeAttribute("NPOLIS", query->value( 8).toString());
                                    /*
                                    if (!query->value( 7).isNull() && !query->value( 7).toString().trimmed().isEmpty()) {
                                        QString s = "0000" + query->value( 7).toString();
                                        stream.writeAttribute("SPOLIS", s.right(4));
                                    }*/
                                }

                                // в Курске дата начала страхования - это дата выдачи ВС
                                if ( data_app.ocato=="38000" ) {
                                    if (!query->value(110).isNull() && query->value(110).toDate()>QDate(1920,1,1)) {
                                        stream.writeAttribute("DBEG",  query->value(110).toDate().toString("yyyy-MM-dd"));
                                    } else if (!query->value(10).isNull() && query->value(10).toDate()>QDate(1920,1,1)) {
                                        stream.writeAttribute("DBEG",  query->value(10).toDate().toString("yyyy-MM-dd"));
                                    }
                                }
                                // ... для остальных - это дата выдачи полиса
                                else {
                                    if (!query->value(10).isNull() && query->value(10).toDate()>QDate(1920,1,1))
                                        stream.writeAttribute("DBEG",  query->value(10).toDate().toString("yyyy-MM-dd"));
                                }
                                // Плановая дата окончания страхования - всегда DEND полиса
                                if (!query->value(11).isNull() && query->value(11).toDate()>QDate(1920,1,1))   stream.writeAttribute("DEND",  query->value(11).toDate().toString("yyyy-MM-dd"));

                                // DSTOP только для времянок
                                if (query->value(4).toInt()==2) {
                                    if (!query->value(12).isNull() && query->value(12).toDate()>QDate(1920,1,1))   {
                                        stream.writeAttribute("DSTOP", query->value(12).toDate().toString("yyyy-MM-dd"));
                                    }
                                }
                                // ... и изъятых полисов единого образца
                                else if ( query->value(4).toInt()==3 &&
                                     query->value(83).toString().trimmed().toUpper().left(3)=="П02" ) {
                                    if (!query->value(12).isNull())   {
                                        stream.writeAttribute("DSTOP", query->value(12).toDate().toString("yyyy-MM-dd"));
                                    } else if (!query->value(11).isNull()) {
                                        stream.writeAttribute("DSTOP",  query->value(11).toDate().toString("yyyy-MM-dd"));
                                    }
                                }
                                // ... для остальных иностранцев, в Белгороде
                                else if ( data_app.ocato=="14000" &&
                                          (query->value(21).toInt()==2 ||
                                           query->value(21).toInt()==3 ||
                                           query->value(21).toInt()==4 ||
                                           query->value(21).toInt()==5 ||
                                           query->value(21).toInt()==7 ||
                                           query->value(21).toInt()==8 ||
                                           query->value(21).toInt()==9 ||
                                           query->value(21).toInt()==10) ) {
                                    if (!query->value(12).isNull())   {
                                        stream.writeAttribute("DSTOP", query->value(12).toDate().toString("yyyy-MM-dd"));
                                    } else if (!query->value(11).isNull()) {
                                        stream.writeAttribute("DSTOP",  query->value(11).toDate().toString("yyyy-MM-dd"));
                                    }
                                }
                            }
                            stream.writeEndElement(); // POLIS

                            if (!query->value(14).isNull() && !query->value(14).toString().trimmed().isEmpty() && !query->value(15).isNull()) {
                                stream.writeStartElement("ORDERZ");
                                {
                                    stream.writeAttribute("NORDER", query->value(14).toString());
                                    stream.writeAttribute("DORDER", query->value(15).toDate().toString("yyyy-MM-dd"));
                                }
                                stream.writeEndElement(); // ORDERZ
                            }
                        }
                        stream.writeEndElement(); // INSURANCE
                    //}


                    //if (event_cod_!="П02") {
                        if ( (f_polis==0 || f_polis==2 || f_polis==3) &&
                             ( (!query->value(55).isNull()) && (!query->value(56).isNull()) ) ) {
                            stream.writeStartElement("PERSONB");
                            {
                                if (!query->value(55).isNull())   stream.writeAttribute("TYPE", "2");
                                if (!query->value(55).isNull())   stream.writeAttribute("PHOTO", query->value(55).toByteArray().toBase64());
                            }
                            stream.writeEndElement(); // PERSONB

                            stream.writeStartElement("PERSONB");
                            {
                                if (!query->value(56).isNull())   stream.writeAttribute("TYPE", "3");
                                if (!query->value(56).isNull())   stream.writeAttribute("PHOTO", query->value(56).toByteArray().toBase64());
                            }
                            stream.writeEndElement(); // PERSONB
                        }
                    //}

                    bool p060_docs_f = false;

                    if (event_code=="П060") {
                        bool has_P010_udl = true;
                        bool has_udl_type, has_udl_ser, has_udl_num, has_udl_org, has_udl_date, has_udl_exp;
                        int udl_type;
                        QString udl_ser, udl_num, udl_org;
                        QDate udl_date, udl_exp;

                        bool has_P010_drp = true;
                        bool has_drp_type, has_drp_ser, has_drp_num, has_drp_org, has_drp_date, has_drp_exp;
                        int drp_type;
                        QString drp_ser, drp_num, drp_org;
                        QDate drp_date, drp_exp;

                        // если есть - получим данные УДЛ последнего события П010   , П034, П035, П036, П061, П062, П063
                        QString sql_udl = "select dd.doc_type, dd.doc_ser, dd.doc_num, dd.doc_date, dd.doc_exp, dd.doc_org "
                                          "  from persons_docs dd "
                                          "  left join events aa on(aa.id_udl=dd.id) "
                                          " where aa.id=( select max(a.id) "
                                          "                 from events a "
                                          "                where a.id_person=" + QString::number(query->value(16).toInt()) + " "
                                          "                  and a.event_code in('П010', 'П034', 'П035', 'П036', 'П061', 'П062', 'П063') ) ";

                        QSqlQuery *query_udl = new QSqlQuery(db);
                        if (!mySQL.exec(this, sql_udl, "Получим УДЛ на событие по выдаче ВС", *query_udl, true, db, data_app)) {
                            QMessageBox::warning(this, "Операция отменена",
                                                 "При попытке получить УДЛ на событие по выдаче ВС произошла неожиданная ошибка.\n\n"
                                                 "Будет использован текущй документ персоны.");
                            delete query_udl;
                            has_P010_udl = false;
                            db.rollback();
                            return false;
                        }
                        if (query_udl->next()) {
                            udl_type = query_udl->value(0).toInt();
                            udl_ser  = query_udl->value(1).toString();
                            udl_num  = query_udl->value(2).toString();
                            udl_date = query_udl->value(3).toDate();
                            udl_exp  = query_udl->value(4).toDate();
                            udl_org  = query_udl->value(5).toString();

                            has_udl_type = !query_udl->value(0).isNull();
                            has_udl_ser  = !query_udl->value(1).isNull() && !query_udl->value(1).toString().isEmpty();
                            has_udl_num  = !query_udl->value(2).isNull() && !query_udl->value(2).toString().isEmpty();
                            has_udl_date = !query_udl->value(3).isNull();
                            has_udl_exp  = !query_udl->value(4).isNull();
                            has_udl_org  = !query_udl->value(5).isNull();
                        } else
                            has_P010_udl = false;
                        delete query_udl;

                        if (!has_P010_udl) {
                            udl_type = query->value(37).toInt();
                            udl_ser  = query->value(39).toString();
                            udl_num  = query->value(40).toString();
                            udl_date = query->value(41).toDate();
                            udl_exp  = query->value(42).toDate();
                            udl_org  = query->value(43).toString();

                            has_udl_type = !query->value(37).isNull();
                            has_udl_ser  = !query->value(39).isNull() && !query->value(39).toString().isEmpty();
                            has_udl_num  = !query->value(40).isNull() && !query->value(40).toString().isEmpty();
                            has_udl_date = !query->value(41).isNull();
                            has_udl_exp  = !query->value(42).isNull();
                            has_udl_org  = !query->value(43).isNull();
                        }

                        // если есть - получим данные ДРП последнего события П010, П034, П035, П036, П061, П062, П063
                        QString sql_drp = "select dd.doc_type, dd.doc_ser, dd.doc_num, dd.doc_date, dd.doc_exp, dd.doc_org "
                                          "  from persons_docs dd "
                                          "  left join events aa on(aa.id_drp=dd.id) "
                                          " where aa.id=( select max(a.id) "
                                          "                 from events a "
                                          "                where a.id_person=" + QString::number(query->value(16).toInt()) + " "
                                          "                  and a.event_code in('П010', 'П034', 'П035', 'П036', 'П061', 'П062', 'П063') ) ";

                        QSqlQuery *query_drp = new QSqlQuery(db);
                        if (!mySQL.exec(this, sql_drp, "Получим ДРП на событие по выдаче ВС", *query_drp, true, db, data_app)) {
                            QMessageBox::warning(this, "Операция отменена",
                                                 "При попытке получить ДРП на событие по выдаче ВС произошла неожиданная ошибка.\n\n"
                                                 "Будет использован текущй документ персоны.");
                            delete query_drp;
                            has_P010_drp = false;
                            db.rollback();
                            return false;
                        }
                        if (query_drp->next()) {
                            drp_type = query_drp->value(0).toInt();
                            drp_ser  = query_drp->value(1).toString();
                            drp_num  = query_drp->value(2).toString();
                            drp_date = query_drp->value(3).toDate();
                            drp_exp  = query_drp->value(4).toDate();
                            drp_org  = query_drp->value(5).toString();

                            has_drp_type = !query_drp->value(0).isNull();
                            has_drp_ser  = !query_drp->value(1).isNull() && !query_drp->value(1).toString().isEmpty();
                            has_drp_num  = !query_drp->value(2).isNull() && !query_drp->value(2).toString().isEmpty();
                            has_drp_date = !query_drp->value(3).isNull();
                            has_drp_exp  = !query_drp->value(4).isNull();
                            has_drp_org  = !query_drp->value(5).isNull();
                        } else
                            has_P010_drp = false;
                        delete query_drp;

                        if (!has_P010_drp) {
                            drp_type = query->value(46).toInt();
                            drp_ser  = query->value(48).toString();
                            drp_num  = query->value(49).toString();
                            drp_date = query->value(50).toDate();
                            drp_exp  = query->value(51).toDate();
                            drp_org  = query->value(52).toString();

                            has_drp_type = !query->value(46).isNull();
                            has_drp_ser  = !query->value(48).isNull() && !query->value(48).toString().isEmpty();
                            has_drp_num  = !query->value(49).isNull() && !query->value(49).toString().isEmpty();
                            has_drp_date = !query->value(50).isNull();
                            has_drp_exp  = !query->value(51).isNull();
                            has_drp_org  = !query->value(52).isNull();
                        }



                        stream.writeStartElement("DOC_LIST");
                        {
                            int id_udl = query->value(104).toInt();
                            int id_drp = query->value(105).toInt();
                            if ( !query->value(37).isNull() ) {
                                stream.writeStartElement("DOC");
                                {
                                    if (has_udl_type)   stream.writeAttribute("DOCTYPE", QString::number(udl_type));
                                    else                stream.writeAttribute("DOCTYPE", "14");
                                    if (has_udl_ser )   stream.writeAttribute("DOCSER",  udl_ser);
                                    if (has_udl_num )   stream.writeAttribute("DOCNUM",  udl_num);
                                    else                stream.writeAttribute("DOCNUM",  "-1");
                                    if (has_udl_date)   stream.writeAttribute("DOCDATE", udl_date.toString("yyyy-MM-dd"));
                                    else                stream.writeAttribute("DOCDATE", QDate::currentDate().toString("yyyy-MM-dd"));
                                    if (has_udl_exp )   stream.writeAttribute("DOCEXP",  udl_exp.toString("yyyy-MM-dd"));
                                    if (has_udl_org )   stream.writeAttribute("NAME_VP", udl_org.left(80));

                                    p060_docs_f = true;
                                }
                                stream.writeEndElement(); // DOC
                            }

                            if ( id_drp!=id_udl
                                 && !query->value(46).isNull() ) {
                                stream.writeStartElement("DOC");
                                {
                                    if (has_drp_type)   stream.writeAttribute("DOCTYPE", QString::number(drp_type));
                                    if (has_drp_ser )   stream.writeAttribute("DOCSER",  drp_ser);
                                    if (has_drp_num )   stream.writeAttribute("DOCNUM",  drp_num);
                                    if (has_drp_date)   stream.writeAttribute("DOCDATE", drp_date.toString("yyyy-MM-dd"));
                                    if (has_drp_exp )   stream.writeAttribute("DOCEXP",  drp_exp.toString("yyyy-MM-dd"));
                                    if (has_drp_org )   stream.writeAttribute("NAME_VP", drp_org.left(80));

                                    p060_docs_f = true;
                                }
                                stream.writeEndElement(); // DOC
                            }
                        }
                        stream.writeEndElement(); // DOC_LIST
                    }

                    if ( event_code!="П060"
                         || !p060_docs_f ) {
                        stream.writeStartElement("DOC_LIST");
                        {
                            if (!query->value(37).isNull()) {
                                stream.writeStartElement("DOC");
                                {
                                    if (!query->value(37).isNull())                                                       stream.writeAttribute("DOCTYPE", QString::number(query->value(37).toInt()));
                                    else                                                                                  stream.writeAttribute("DOCTYPE", "14");
                                    if (!query->value(39).isNull() && !query->value(39).toString().trimmed().isEmpty())   stream.writeAttribute("DOCSER", query->value(39).toString());
                                    if (!query->value(40).isNull() && !query->value(40).toString().trimmed().isEmpty())   stream.writeAttribute("DOCNUM", query->value(40).toString());
                                    else                                                                                  stream.writeAttribute("DOCNUM", "-1");
                                    if (!query->value(41).isNull() && query->value(41).toDate()>QDate(1920,1,1))          stream.writeAttribute("DOCDATE", query->value(41).toDate().toString("yyyy-MM-dd"));
                                    else                                                                                  stream.writeAttribute("DOCDATE", QDate::currentDate().toString("yyyy-MM-dd"));
                                    if (!query->value(42).isNull() && query->value(42).toDate()>QDate(1920,1,1))          stream.writeAttribute("DOCEXP", query->value(42).toDate().toString("yyyy-MM-dd"));
                                    if (!query->value(43).isNull() && !query->value(43).toString().trimmed().isEmpty())   stream.writeAttribute("NAME_VP", query->value(43).toString().left(80));
                                }
                                stream.writeEndElement(); // DOC
                            }

                            if (!query->value(46).isNull()) {
                                stream.writeStartElement("DOC");
                                {
                                    if (!query->value(46).isNull())                                                       stream.writeAttribute("DOCTYPE", QString::number(query->value(46).toInt()));
                                    if (!query->value(48).isNull() && !query->value(48).toString().trimmed().isEmpty())   stream.writeAttribute("DOCSER", query->value(48).toString());
                                    if (!query->value(49).isNull() && !query->value(49).toString().trimmed().isEmpty())   stream.writeAttribute("DOCNUM", query->value(49).toString());
                                    if (!query->value(50).isNull() && query->value(50).toDate()>QDate(1920,1,1))          stream.writeAttribute("DOCDATE", query->value(50).toDate().toString("yyyy-MM-dd"));
                                    if (!query->value(51).isNull() && query->value(51).toDate()>QDate(1920,1,1))          stream.writeAttribute("DOCEXP", query->value(51).toDate().toString("yyyy-MM-dd"));
                                    if (!query->value(52).isNull() && !query->value(52).toString().trimmed().isEmpty())   stream.writeAttribute("NAME_VP", query->value(52).toString().left(80));
                                }
                                stream.writeEndElement(); // DOC
                            }
                        }
                        stream.writeEndElement(); // DOC_LIST
                    }


                    if (event_code!="П060") {
                        // выберем все старые документы
                        QString sql_old_docs = "select doc_type, doc_ser, doc_num, doc_date, doc_exp, doc_org, org_code, status "
                                               "  from persons_docs d "
                                               " where d.id_person=" + QString::number(query->value(16).toInt()) + " "
                                               "   and d.status=2 ; ";

                        QSqlQuery *query_old_docs = new QSqlQuery(db);
                        if (!mySQL.exec(this, sql_old_docs, "Выбор старых документов застрахованного", *query_old_docs, true, db, data_app)) {
                            delete query_old_docs;
                            QMessageBox::warning(this, "Операция отменена",
                                                 "При запросе старых документов застрахованного произошла непредвиденная ошибка.\n\n"
                                                 "Операция выборки старых документов отменена.");
                            db.rollback();
                            return false;
                        } else {
                            if (query_old_docs->size()>0) {
                                stream.writeStartElement("OLDDOC_LIST");
                                while (query_old_docs->next()) {
                                    stream.writeStartElement("OLD_DOC");
                                    {
                                        if ( !query_old_docs->value(0).isNull())                        stream.writeAttribute("DOCTYPE", QString::number(query_old_docs->value(0).toInt()));
                                        else                                                            stream.writeAttribute("DOCTYPE", "14");
                                        if ( !query_old_docs->value(1).isNull() &&
                                             !query_old_docs->value(1).toString().trimmed().isEmpty())  stream.writeAttribute("DOCSER", query_old_docs->value(1).toString());
                                        if ( !query_old_docs->value(2).isNull() &&
                                             !query_old_docs->value(2).toString().trimmed().isEmpty())  stream.writeAttribute("DOCNUM", query_old_docs->value(2).toString());
                                        else                                                            stream.writeAttribute("DOCNUM", "-1");
                                        if ( !query_old_docs->value(3).isNull() &&
                                             query_old_docs->value(3).toDate()>QDate(1920,1,1))         stream.writeAttribute("DOCDATE", query_old_docs->value(3).toDate().toString("yyyy-MM-dd"));
                                        else                                                            stream.writeAttribute("DOCDATE", QDate::currentDate().toString("yyyy-MM-dd"));
                                        if ( !query_old_docs->value(4).isNull() &&
                                             query_old_docs->value(4).toDate()>QDate(1920,1,1))         stream.writeAttribute("DOCEXP", query_old_docs->value(4).toDate().toString("yyyy-MM-dd"));
                                        if ( !query_old_docs->value(5).isNull() &&
                                             !query_old_docs->value(5).toString().trimmed().isEmpty())  stream.writeAttribute("NAME_VP", query_old_docs->value(5).toString().left(80));
                                    }
                                    stream.writeEndElement(); // OLD_DOC
                                }
                                stream.writeEndElement(); // OLDDOC_LIST
                            }
                        }
                    }

                }
                stream.writeEndElement();  // OP

                if (!f_old_data) {
                    // отмена разногласий на отправленный полис
                    QString sql_protocols = "update frecs_in_p "
                                            "   set status=-1, "
                                            "       date_resend=CURRENT_DATE "
                                            " where id_polis=" + QString::number(query->value(0).toInt()) + " ; ";
                    QSqlQuery *query_protocols = new QSqlQuery(db);
                    if (!mySQL.exec(this, sql_protocols, "Закрытие разногласий, связанных с отпраляемой записью", *query_protocols, true, db, data_app)) {
                        QMessageBox::warning(this, "Операция отменена",
                                             "При при закрытии связанных с отправляемой записью разногласий произошла неожиданная ошибка.\n\n"
                                             "Операция выгрузки данных для ТФОМС отменена");
                        delete query_protocols;
                        db.rollback();
                        return 0;
                    }
                    delete query_protocols;
                }


                // Запомним запись с ID_FILE_I = -1
                QString sql_frec =
                                "insert into frecs_out_i (id_file_i, n_rec, id_person, id_polis, id_vizit, id_event) "
                                " values(-1, "
                                " " + QString::number(n_rec) + ", "
                                " " + QString::number(query->value( 16).toInt()) + ", "
                                " " + QString::number(query->value(  0).toInt()) + ", "
                                " " + QString::number(query->value(103).toInt()) + ", "
                                " " + QString::number(query->value( 89).toInt()) + ") ; ";
                QSqlQuery *query_frec = new QSqlQuery(db);
                mySQL.exec(this, sql_frec, "Добавление frec", *query_frec, true, db, data_app);
                delete query_frec;
            }
        }        
        stream.writeEndElement();  // OPLIST
    }


    // сохранение отметки, что запись отправлена в фонд
    QString sql_status = "update events "
                         "   set status=2 "
                         " where status=1 ; ";
    QSqlQuery *query_status = new QSqlQuery(db);
    if (!mySQL.exec(this, sql_status, "Пометка новых событий, только что отправленных в ТФОМС", *query_status, true, db, data_app)) {
        QMessageBox::warning(this, "Операция отменена",
                             "При пометке только что отправленных в фонд событий произошла неожиданная ошибка.\n\n"
                             "Операция выгрузки данных для ТФОМС отменена");
        delete query_status;
        db.rollback();
        return 0;
    }
    delete query_status;



    stream.writeEndElement(); // OPLIST
    //...
    stream.writeEndDocument();

    delete query;    
    ui->lab_cnt->setText(QString::number(n_rec));
    QApplication::processEvents();

    // подготовка сохранения файла
    QString s_out_xml = data_app.path_temp + f_name + ".xml";
    QFile::remove(s_out_xml);
    while (QFile::exists(s_out_xml)) {
        if (QMessageBox::warning(this,
                                 "Ошибка выгрузки файла",
                                 "Не удалось сохранить файл выгрузки: \n" + s_out_xml +
                                 "\n\nво временную папку\n" + data_app.path_temp +
                                 "\n\nПроверьте, не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                 QMessageBox::Retry|QMessageBox::Abort,
                                 QMessageBox::Retry)==QMessageBox::Abort) {
            this->setCursor(Qt::ArrowCursor);
            db.rollback();
            return 0;
        }
        QFile::remove(s_out_xml);
    }
    // сохраним файл на диск
    QFile *f_xml = new QFile(s_out_xml);
    f_xml->open(QIODevice::WriteOnly);
    QTextCodec *codec = QTextCodec::codecForName("cp1251");
    xml = xml.replace("<?xml version=\"1.0\"?>", "<?xml version=\"1.0\" encoding=\"windows-1251\"?>");
    xml = xml.replace("Проверка кодировки", "");
    QByteArray encodedString = codec->fromUnicode(xml);
    //QTextCodec* codec = QTextCodec::codecForName("Windows-1251");
    //QTextDecoder *decoder = new QTextDecoder(codec);
    //QByteArray encodedString = decoder->fromUnicode(xml);
    f_xml->write(encodedString);
    f_xml->close();

    // сформируем архив
    QProcess myProcess;
    QDir d_out = data_app.path_out;
    QString s_out_zip = data_app.path_temp + f_name + ".zip";
    QFile::remove(s_out_zip);
    while (QFile::exists(s_out_zip)) {
        if (QMessageBox::warning(this,
                                 "Ошибка выгрузки файла",
                                 "Не удалось заархивировать файл выгрузки: \n" + s_out_zip +
                                 "\n\nво временную папку\n" + data_app.path_temp +
                                 "\n\nПроверьте, не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                 QMessageBox::Retry|QMessageBox::Abort,
                                 QMessageBox::Retry)==QMessageBox::Abort) {
            this->setCursor(Qt::ArrowCursor);
            db.rollback();
            return 0;
        }
        QFile::remove(s_out_zip);
    }
    QString com = "\"" + data_app.path_arch + "7z.exe\" a \"" + s_out_zip + "\" \"" + s_out_xml + "\"";
    while (myProcess.execute(com)!=0) {
        if (QMessageBox::warning(this,
                                 "Ошибка архивации контента",
                                 "При формировании ZIP-архива произошла непредвиденная ошибка\n\nОпреация прервана."
                                 "\n\nПроверьте, не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                 QMessageBox::Retry|QMessageBox::Abort,
                                 QMessageBox::Retry)==QMessageBox::Abort) {
            this->setCursor(Qt::ArrowCursor);
            db.rollback();
            return 0;
        }
    }

    if (!d_out.exists())
        d_out.mkpath(data_app.path_out);

    // добавление файла в базу
    QString bin;
    mySQL.myBYTEA(s_out_zip, bin);
    QString sql_file;
    sql_file = "insert into files_out_i (pack_name, file_name, vers, date_send, smo_regnum, point_regnum, n_recs, bin) ";
    sql_file+= " values("
                    "'" + f_name.right(f_name.length()-1) + "', "
                    "'" + f_name + "', "
                    "'" + data_app.xml_vers + "', "
                    "'" + QDate::currentDate().toString("yyyy-MM-dd") + "', "
                    "'" + data_app.smo_regnum + "', "
                    "'" + point_regnum + "', "
                    " " + QString::number(n_rec) + ", "
                    "'" + bin + "') ";
    sql_file+= " returning id ; ";

    QSqlQuery *query_file = new QSqlQuery(db);
    if (!mySQL.exec(this, sql_file, "Запись резервной копии выгруженного файла в базу полисов", *query_file, true, db, data_app) || !query_file->next()) {
        delete query_file;
        QMessageBox::warning(this, "Операция отменена", "При записи резервной копии выгруженного файла в базу полисов произошла неожиданная ошибка.\n\nОперация выгрузки данных для ТФОМС отменена.");
        db.rollback();
        return 0;
    }
    int id_file_i = query_file->value(0).toInt();

    // Свяжем записи frec с записью файла
    QString sql_flrfr =
                    "update frecs_out_i "
                    "   set id_file_i = " + QString::number(id_file_i) + " "
                    " where id_file_i = -1 ; ";
    QSqlQuery *query_flrfr = new QSqlQuery(db);
    mySQL.exec(this, sql_flrfr, "связывание frec с записью файла", *query_flrfr, true, db, data_app);
    delete query_flrfr;
    delete query_file;

    // ------------------------------------------
    //db.commit();

    // добавим записи в таблицу связей
    //db.transaction();
    // ------------------------------------------

    for (int i=0; i<id_events_list.size(); i++) {
        QString sql_r;
        /*// удалим старые связки этого события с файлами - они уже не нужны
        sql_r = "delete from files_r_evts "
                " where id_event=" + QString::number(id_events_list.at(i)) + " "
                "   and id_file_i=" + QString::number(id_file_i) + " ; ";*/
        // добавим новую связку этого события с новым файлом
        sql_r+= "insert into files_r_evts (id_event, id_file_i, n_rec) "
                "values (" + QString::number(id_events_list.at(i)) + ", " + QString::number(id_file_i) + ", " + QString::number(i) + ") ; ";
        QSqlQuery *query_r = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_r, "Добавление связки (отправленный файл) - (отправленное событие)", *query_r, true, db, data_app)) {
            delete query_r;
            QMessageBox::warning(this, "Операция отменена",
                                 "При добавлении связки (отправленный файл) - (отправленное событие) произошла неожиданная ошибка.\n\n"
                                 "Операция выгрузки данных для ТФОМС отменена.");
            db.rollback();
            return 0;
        }
        delete query_r;
    }


    // генерация записи посылки
    QString pack_name = f_name;
    pack_name = pack_name.replace("i", "");
    QString sql_pack =
            "insert into files_packs (pack_name, id_file_i, id_file_p, id_file_f, date_file_i, date_file_p, date_file_f) "
            " values('" + pack_name + "', "
                    " " + QString::number(id_file_i) + ", "
                    " NULL, "
                    " NULL, "
                    " '" + QDate::currentDate().toString("yyyy-MM-dd") + "', "
                    " NULL, "
                    " NULL) ; ";
    QSqlQuery *query_pack = new QSqlQuery(db);
    if (!mySQL.exec(this, sql_pack, "Добавление посылки", *query_pack, true, db, data_app)) {
        delete query_pack;
        QMessageBox::warning(this, "Операция отменена",
                             "При добавлении посылки произошла неожиданная ошибка.\n\n"
                             "Операция выгрузки данных для ТФОМС отменена.");
        db.rollback();
        return 0;
    }
    delete query_pack;


    // перенос файла в целевую папку
    //if (data_app.f_zip_packs) {
        // перенесём в целевую папку zip-файл
        while (!QFile::copy(s_out_zip, s_res_zip)) {
            if (QMessageBox::warning(this,
                                     "Ошибка выгрузки файла в целевую папку",
                                     "При выгрузке файла в целевую папку произошла непредвиденная ошибка\nФайл: \n" + s_out_zip +
                                     "\n\nПапка: \n" + s_res_dir +
                                     "\n\nПроверьте, существует ли папка и не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                     QMessageBox::Retry|QMessageBox::Abort,
                                     QMessageBox::Retry)==QMessageBox::Abort) {
                this->setCursor(Qt::ArrowCursor);
                db.rollback();
                return 0;
            }
        }
    //}
    if (!data_app.f_zip_packs) {
        // перенесём в целевую папку xml-файл
        while (!QFile::copy(s_out_xml, s_res_xml)) {
            if (QMessageBox::warning(this,
                                     "Ошибка выгрузки файла в целевую папку",
                                     "При выгрузке файла в целевую папку произошла непредвиденная ошибка\nФайл: \n" + s_out_xml +
                                     "\n\nПапка: \n" + s_res_dir +
                                     "\n\nПроверьте, существует ли папка и не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                     QMessageBox::Retry|QMessageBox::Abort,
                                     QMessageBox::Retry)==QMessageBox::Abort) {
                this->setCursor(Qt::ArrowCursor);
                db.rollback();
                return 0;
            }
        }
    }
    f_xml->remove();
    delete f_xml;


    db.commit();
    this->setCursor(Qt::ArrowCursor);

    return n_rec;
}

void send_to_fond_wnd::on_bn_process_clicked() {

    // сохраним настройки


    //data_app.f_send2fond_xmls;
    //data_app.f_send2fond_zips;
    //data_app.f_send2fond_1ZIP;
    data_app.f_send2fond_new_IM = ui->ch_new_names->isChecked();
    data_app.f_send2fond_new_OT = ui->ch_new_namepats->isChecked();
    data_app.f_send2fond_new_assigs = ui->ch_new_assigs->isChecked();
    // [flags]
    settings.beginGroup("flags");
    //settings->setValue("send_to_fond_xmls", data_app.f_send2fond_xmls);
    //settings->setValue("send_to_fond_zips", data_app.f_send2fond_zips);
    //settings->setValue("send_to_fond_1ZIP", data_app.f_send2fond_1ZIP);
    settings.setValue("send_to_fond_new_IM", data_app.f_send2fond_new_IM);
    settings.setValue("send_to_fond_new_OT", data_app.f_send2fond_new_OT);
    settings.setValue("send_to_fond_new_assigs", data_app.f_send2fond_new_assigs);
    settings.endGroup();
    settings.sync();



    ui->bn_process->setEnabled(false);
    ui->lab_dst->setText("-//-");
    ui->lab_cnt->setText("-//-");
    QApplication::processEvents();

    // выгрузка новых данных
    if (ui->rb_genNew->isChecked()) {

        // получим список пунктов выдачи
        QString sql_pts = "select id, point_name, point_regnum "
                          "  from points "
                          " order by point_regnum ; ";
        QSqlQuery *query_pts = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_pts, "Получим список пунктов выдачи", *query_pts, true, db, data_app)) {
            delete query_pts;
            QMessageBox::warning(this, "Операция отменена", "При попытке получить список пунктов выдачи полисов произошла неожиданная ошибка.\n\nОперация выгрузки данных для ТФОМС отменена");
            db.rollback();

            ui->bn_process->setEnabled(true);
            return;
        }
        QString s_res = "Для пунктов выдачи выгружено записей:\n";
        while (query_pts->next()) {

            QString point_regnum = QString(QString("000") + query_pts->value(2).toString()).right(3);

            if ( point_regnum!="000" &&                                             // для техподдержки посылки не формируем
                 ( ui->combo_point->currentIndex()==0 ||                            // выгрузка для всех ПВП
                   ui->combo_point->currentData().toString()==point_regnum) ) {     // или именно этот ПВП выбран

                // индикация
                ui->lab_dst->setText(point_regnum);
                ui->lab_cnt->setText("-//-");
                QApplication::processEvents();

                // !!!!! выгрузка !!!!! //
                int n = send(point_regnum);
                // !!!!! выгрузка !!!!! //

                // проверим, надо ли выгружать этот ПВП
                if (n>0) {        // выгрузка не пустая
                    s_res += "\n   " + query_pts->value(2).toString() + " - " + QString::number(n);
                }
            }
        }
        delete query_pts;


        // в зависимости от настроек, удалим zip-файлы, оставим zip-файлы или удалим zip-файлы и сформируем большой zip-файл
        ui->lab_dst->setText("ZIP");
        ui->lab_cnt->setText("-//-");
        QApplication::processEvents();

        if (data_app.f_send2fond_xmls) {
            // удалим файлы *.zip
            for (int i=0; i<zip_files.size(); i++) {
                QFile::remove(zip_files.at(i));
            }
        } else if (data_app.f_send2fond_zips) {
            // удалим файлы *.xml
            for (int i=0; i<xml_files.size(); i++) {
                QFile::remove(xml_files.at(i));
            }
        } else if (data_app.f_send2fond_1ZIP) {
            // составим название общего архива *.ZIP
            QString zip_name;
            zip_name = "i";
            zip_name += data_app.smo_regnum;
            zip_name += "_ALL_";
            zip_name += QDate::currentDate().toString("MMyy");
            int n = 0;
            QString s_out_zip;
            while (QFile::exists(data_app.path_out + zip_name + QString("00" + QString::number(n)).right(2) + ".zip"))
             {  n++;  }
            s_out_zip = data_app.path_out + zip_name + QString("00" + QString::number(n)).right(2) + ".zip";

            // добавим все *.xml в общий архив *.ZIP
            QProcess myProcess;
            for (int i=0; i<xml_files.size(); i++) {
                //QFile::remove(xml_files.at(i));

                QString com = "\"" + data_app.path_arch + "7z.exe\" a \"" + s_out_zip + "\" \"" + xml_files.at(i) + "\"";
                while (myProcess.execute(com)!=0) {
                    if (QMessageBox::warning(this,
                                             "Ошибка архивации объединённой посылки по всем ПВП",
                                             "При формировании ZIP-архива объединённой посылки по всем ПВП произошла непредвиденная ошибка\n\nОпреация прервана."
                                             "\n\nПроверьте, не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                             QMessageBox::Retry|QMessageBox::Abort,
                                             QMessageBox::Retry)==QMessageBox::Abort) {
                        this->setCursor(Qt::ArrowCursor);

                        ui->bn_process->setEnabled(true);
                        return;
                    }
                }
            }

            // удалим файлы *.xml
            /*for (int i=0; i<xml_files.size(); i++) {
                QFile::remove(xml_files.at(i));
            }*/
            // удалим файлы *.zip
            for (int i=0; i<zip_files.size(); i++) {
                QFile::remove(zip_files.at(i));
            }
        }



        // --------------------------------------
        // генерация списков новых имён и отчеств
        // --------------------------------------
        ui->lab_dst->setText("ODBC");
        ui->lab_cnt->setText("-//-");
        QApplication::processEvents();
        int cnt_IM = 0;
        int cnt_OT = 0;

        if ( ui->ch_new_names->isChecked() ||
             ui->ch_new_namepats->isChecked() ) {

            // подключимся к ODBC
            db_ODBC = QSqlDatabase::addDatabase("QODBC", "INKO_DBF");
            db_ODBC.setConnectOptions();
            // имя источника данных
            db_ODBC.setDatabaseName("INKO_DBF");
            while (!db_ODBC.open()) {
                if (QMessageBox::warning(this, "Не найден ODBC-источник данных INKO_DBF",
                                         QString("Источник данных должен использовать папку, указанную в параметре программы \"path_dbf\":\n"
                                         "При попытке подключиться к испточнику данных ODBC \"INKO_DBF\" операционная система вернула отказ. \n"
                                         "Этот источник данных необходим для генерации и чтения dbf-файлов обмена с ТФОМС. \n"
                                         "Если такого источника данных нет - создайте его.\n\n")
                                         + data_app.path_dbf + "\n\n"
                                         "Попробовать снова?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::No) {

                    ui->bn_process->setEnabled(true);
                    return;
                }
            }

            if (ui->ch_new_names->isChecked()) {
                ui->lab_dst->setText("IM.dbf");
                ui->lab_cnt->setText("-//-");
                QApplication::processEvents();

                // обновим файл имён
                QFile::remove(data_app.path_dbf + "/IM.DBF");
                QFile::copy(data_app.path_dbf + "/IM_src.DBF", data_app.path_dbf + "/IM.DBF");

                // очистим таблицу имён
                QString sql_del_IM = "delete from IM ; ";
                QSqlQuery *query_del_IM = new QSqlQuery(db_ODBC);
                bool res_del_IM = query_del_IM->exec(sql_del_IM);
                QString err = db_ODBC.lastError().databaseText();
                if (!res_del_IM) {
                    delete query_del_IM;
                    QMessageBox::warning(this, "Ошибка очистки таблицы имён",
                                         "При попытке очистить таблицу имён произошла ошибка!\n\n"
                                         "Операция отменена");
                    db_ODBC.close();
                    QSqlDatabase::removeDatabase("INKO_DBF");

                    ui->bn_process->setEnabled(true);
                    return;
                }
                delete query_del_IM;


                // сформируем список новых имён
                QString sql_IM = "select id, name, sex "
                                 "  from spr_name_to_sex s"
                                 " where status=0 ; ";
                QSqlQuery *query_IM = new QSqlQuery(db);
                bool res_IM = query_IM->exec(sql_IM);
                err = db.lastError().databaseText();
                if (!res_IM) {
                    delete query_IM;
                    QMessageBox::warning(this, "Ошибка получения новых имён",
                                         "При попытке получить новые имена произошла ошибка!\n\n"
                                         "Операция отменена");

                    ui->bn_process->setEnabled(true);
                    return;
                }
                while (query_IM->next()) {
                    // добавим имя в файл для отправки
                    QString sql_ins_IM = "insert into IM(name, pol) "
                                         "values ('" + query_IM->value(1).toString() + "', "
                                               " " + QString::number(query_IM->value(2).toInt()) + ") ; ";
                    QSqlQuery *query_ins_IM = new QSqlQuery(db_ODBC);
                    bool res_ins_IM = query_ins_IM->exec(sql_ins_IM);
                    QString err = db_ODBC.lastError().databaseText();
                    if (!res_ins_IM) {
                        delete query_ins_IM;
                        QMessageBox::warning(this, "Ошибка добавления нового имени",
                                             "При попытке добавить новое имя в файл обмена произошла ошибка!\n\n"
                                             "Операция отменена");
                        db_ODBC.close();
                        QSqlDatabase::removeDatabase("INKO_DBF");

                        ui->bn_process->setEnabled(true);
                        return;
                    }
                    cnt_IM++;
                    delete query_ins_IM;
                }
                delete query_IM;
                // поправим статус отправленных имён
                QString sql_stat_IM = "update spr_name_to_sex s "
                                      " set status=1 "
                                      " where status=0 ; ";
                QSqlQuery *query_stat_IM = new QSqlQuery(db);
                bool res_stat_IM = query_stat_IM->exec(sql_stat_IM);
                err = db.lastError().databaseText();
                if (!res_stat_IM) {
                    delete query_stat_IM;
                    QMessageBox::warning(this, "Ошибка обновления статуса новых имён, отправленных в фонд",
                                         "При попытке обновления статуса новых имён, отправленных в фонд, произошла ошибка!\n\n"
                                         "Операция отменена");
                    delete query_stat_IM;

                    ui->bn_process->setEnabled(true);
                    return;
                }

                // скопируем список новых имён в папку OUT
                QFile::copy(data_app.path_dbf + "/IM.DBF", data_app.path_out + "/IM.DBF");
            }


            if (ui->ch_new_namepats->isChecked()) {
                ui->lab_dst->setText("OT.dbf");
                ui->lab_cnt->setText("-//-");
                QApplication::processEvents();

                // обновим файл отчеств
                QFile::remove(data_app.path_dbf + "/OT.DBF");
                QFile::copy(data_app.path_dbf + "/OT_src.DBF", data_app.path_dbf + "/OT.DBF");

                // очистим таблицу отчеств
                QString sql_del_OT = "delete from OT ; ";
                QSqlQuery *query_del_OT = new QSqlQuery(db_ODBC);
                bool res_del_OT = query_del_OT->exec(sql_del_OT);
                QString err = db_ODBC.lastError().driverText() + "\n" + db_ODBC.lastError().databaseText();
                if (!res_del_OT) {
                    delete query_del_OT;
                    QMessageBox::warning(this, "Ошибка очистки таблицы отчеств",
                                         "При попытке очистить таблицу отчеств произошла ошибка!\n\n" +
                                         err + "\n\n"
                                         "Операция отменена");
                    db_ODBC.close();
                    QSqlDatabase::removeDatabase("INKO_DBF");

                    ui->bn_process->setEnabled(true);
                    return;
                }
                delete query_del_OT;


                // сформируем список новых отчеств
                QString sql_OT = "select id, name, sex "
                                 "  from spr_namepat_to_sex s"
                                 " where status=0 ; ";
                QSqlQuery *query_OT = new QSqlQuery(db);
                bool res_OT = query_OT->exec(sql_OT);
                err = db.lastError().databaseText();
                if (!res_OT) {
                    delete query_OT;
                    QMessageBox::warning(this, "Ошибка получения новых отчеств",
                                         "При попытке получить новые отчеств произошла ошибка!\n\n"
                                         "Операция отменена");

                    ui->bn_process->setEnabled(true);
                    return;
                }
                while (query_OT->next()) {
                    // добавим отчество в файл для отправки
                    QString sql_ins_OT = "insert into OT(name, pol) "
                                         "values ('" + query_OT->value(1).toString() + "', "
                                               " " + QString::number(query_OT->value(2).toInt()) + ") ; ";
                    QSqlQuery *query_ins_OT = new QSqlQuery(db_ODBC);
                    bool res_ins_OT = query_ins_OT->exec(sql_ins_OT);
                    QString err = db_ODBC.lastError().databaseText();
                    if (!res_ins_OT) {
                        delete query_ins_OT;
                        QMessageBox::warning(this, "Ошибка добавления нового отчества",
                                             "При попытке добавить новое отчество в файл обмена произошла ошибка!\n\n"
                                             "Операция отменена");
                        db_ODBC.close();
                        QSqlDatabase::removeDatabase("INKO_DBF");

                        ui->bn_process->setEnabled(true);
                        return;
                    }
                    cnt_OT++;
                    delete query_ins_OT;
                }
                delete query_OT;
                // поправим статус отправленных отчеств
                QString sql_stat_OT = "update spr_namepat_to_sex s "
                                      " set status=1 "
                                      " where status=0 ; ";
                QSqlQuery *query_stat_OT = new QSqlQuery(db);
                bool res_stat_OT = query_stat_OT->exec(sql_stat_OT);
                err = db.lastError().databaseText();
                if (!res_stat_OT) {
                    delete query_stat_OT;
                    QMessageBox::warning(this, "Ошибка обновления статуса новых отчеств, отправленных в фонд",
                                         "При попытке обновления статуса новых отчеств, отправленных в фонд, произошла ошибка!\n\n"
                                         "Операция отменена");
                    delete query_stat_OT;

                    ui->bn_process->setEnabled(true);
                    return;
                }
                delete query_stat_OT;

                // скопируем список новых отчеств в папку OUT
                QFile::copy(data_app.path_dbf + "/OT.DBF", data_app.path_out + "/OT.DBF");
            }

            // закроем соединение с ODBC
            db_ODBC.close();
            QSqlDatabase::removeDatabase("INKO_DBF");


            // если есть что отправлять - отправим списки новых имён и отчеств
            QFile::remove(data_app.path_out + "IM.dbf");
            if (cnt_IM>0) {
                QFile::copy(data_app.path_dbf + "IM.dbf", data_app.path_out + "IM.dbf");
            }
            QFile::remove(data_app.path_out + "OT.dbf");
            if (cnt_OT>0) {
                QFile::copy(data_app.path_dbf + "OT.dbf", data_app.path_out + "OT.dbf");
            }
        }


        // -------------------------------------- //
        // сформируем выгрузку новых прикреплений //
        // -------------------------------------- //
        if (ui->ch_new_assigs->isChecked()) {
            ui->lab_dst->setText("assig");
            ui->lab_cnt->setText("-//-");
            QApplication::processEvents();

            // откроем окно "Выгрузка прикреплений по МО"
            delete export_assig_bystep_w;
            export_assig_bystep_w = new export_assig_bystep_wnd(db, data_app, settings, this);
            export_assig_bystep_w->exec();
        }


        QMessageBox::information(this, "Посылка для ТФОМС сформирована", s_res);


        if (ui->rb_split->isChecked()) {
            // ------------------------------------------------- //
            // проверим на события помеченные, но не выгруженные //
            // ------------------------------------------------- //
            QString sql_unsended = "select e.fam, e.im, e.ot, e.date_birth, a.event_code, a.event_dt, p.point_regnum, a.id, a.id_polis, a.id_vizit "
                                   "  from events a "
                                   "  left join polises p on(a.id_polis=p.id) "
                                   "  left join points pt on(pt.id=p._id_last_point) "
                                   "  left join persons e on(e.id=p.id_person) "
                                   "  left join operators o on(o.id=a.id_operator) "
                                   " where a.status=1 and a.event_code<>'П040' "
                                   " order by p.point_code, e.fam, e.im, e.ot, e.date_birth ; ";
            QSqlQuery *query_unsended = new QSqlQuery(db);
            bool res_unsended = query_unsended->exec(sql_unsended);
                QString err = db.lastError().databaseText();
            if (res_unsended && query_unsended->next()) {
                QString msg = "В базе данных осталось " + QString::number(query_unsended->size()) + " событий страхования, уже подготовленных, но так и не выгруженных:\n\n"
                              "ID_EVENT\tданные персоны\n";
                do {
                    msg +=  QString::number(query_unsended->value(7).toInt()) + "\t" +
                            QString::number(query_unsended->value(8).toInt()) + "\t" +
                            QString::number(query_unsended->value(9).toInt()) + "\t" +
                            "-  (ПВП " +
                            query_unsended->value(6).toString() + ")  " +
                            query_unsended->value(0).toString() + " " +
                            query_unsended->value(1).toString() + " " +
                            query_unsended->value(2).toString() + ",  д/р " +
                            query_unsended->value(3).toDate().toString("dd.MM.yyyy") + ";  событие " +
                            query_unsended->value(4).toString() + " от " +
                            query_unsended->value(5).toDate().toString("dd.MM.yyyy") + "\n";
                } while (query_unsended->next());

                msg += "\n\nВНИМАНИЕ! \n"
                       "За каждую просроченную запись страховая компания может быть оштрафована на сумму от 3000 р. за каждый день просрочки. \n"
                       "Максимальный допустимый срок передачи данных в ТФОМС - 3 рабочих дня. \n"
                       "Максимальный допустимый срок передачи данных в ФФОМС - 10 рабочих дней, но в это время входит продолжительность процедур межрегиональого обмена.";

                delete info_not_sended_w;
                info_not_sended_w = new info_not_sended_wnd(msg, this);
                info_not_sended_w->exec();
            }
            delete query_unsended;
        }
        else {
            // ------------------------------------------ //
            // проверим на события не попавшие в выгрузку //
            // ------------------------------------------ //
            QString sql_unsended = "select e.fam, e.im, e.ot, e.date_birth, evt.event_code, evt.event_dt, pt.point_regnum, a.id, a.id_polis, a.id_vizit "
                                   "  from events evt "
                                   "  left join files_r_evts r on(r.id_event=evt.id) "
                                   "  left join polises p on(p.id=evt.id_polis) "
                                   "  left join persons e on(e.id=p.id_person) "
                                   "  left join points pt on(pt.id=p._id_last_point) "
                                   "  left join operators o on(o.id=evt.id_operator) "
                                   " where r.id_file_i is NULL "
                                   "   and evt.event_code<>'П021' "
                                   "   and evt.event_code<>'П022' "
                                   "   and evt.event_code<>'П023' "
                                   "   and evt.event_code<>'П024' "
                                   "   and evt.event_code<>'П025' "
                                   "   and evt.event_code<>'П040' "
                                   " order by pt.point_code, e.fam, e.im, e.ot, e.date_birth ; ";
            QSqlQuery *query_unsended = new QSqlQuery(db);
            bool res_unsended = query_unsended->exec(sql_unsended);
                QString err = db.lastError().databaseText();
            if (res_unsended && query_unsended->next()) {
                QString msg = "В базе данных осталось " + QString::number(query_unsended->size()) + " событий страхования, ещё не выгруженных для ТФОМС:\n\n";
                do {
                    msg +=  QString::number(query_unsended->value(7).toInt()) + "\t" +
                            QString::number(query_unsended->value(8).toInt()) + "\t" +
                            QString::number(query_unsended->value(9).toInt()) + "\t" +
                            "-  (ПВП " +
                            query_unsended->value(6).toString() + ")  " +
                            query_unsended->value(0).toString() + " " +
                            query_unsended->value(1).toString() + " " +
                            query_unsended->value(2).toString() + ",  д/р " +
                            query_unsended->value(3).toDate().toString("dd.MM.yyyy") + ";  событие " +
                            query_unsended->value(4).toString() + " от " +
                            query_unsended->value(5).toDate().toString("dd.MM.yyyy") + "\n";
                } while (query_unsended->next());

                msg += "\n\nВНИМАНИЕ! \n"
                       "За каждую просроченную запись страховая компания может быть оштрафована на сумму от 3000 р. за каждый день просрочки. \n"
                       "Максимальный допустимый срок передачи данных в ТФОМС - 3 рабочих дня. \n"
                       "Максимальный допустимый срок передачи данных в ФФОМС - 10 рабочих дней, но в это время входит продолжительность процедур межрегиональого обмена.";

                delete info_not_sended_w;
                info_not_sended_w = new info_not_sended_wnd(msg, this);
                info_not_sended_w->exec();
            }
            delete query_unsended;
        }
    }


    // выгрузка данных задним числом
    if (ui->rb_genOld->isChecked()) {
        ui->lab_dst->setText("OLD");
        ui->lab_cnt->setText("-//-");

        // !!!!! выгрузка !!!!! //
        int n = send("OLD", true);
        // !!!!! выгрузка !!!!! //

        QMessageBox::information(this, "Выгрузка задним числом сформирована", "Выгрузка задним числом сформирована.");
    }

    ui->bn_process->setEnabled(true);

    // =========================
    // открытие папки сохранения
    // =========================
    long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(data_app.path_out.utf16()), 0, 0, SW_NORMAL);
}

void send_to_fond_wnd::on_rb_genNew_clicked(bool checked) {
    ui->pan_genNew->setEnabled(checked);
    ui->pan_genOld->setEnabled(!checked);
    ui->combo_old_point->setEnabled(!checked);
    ui->combo_old_event->setEnabled(!checked);
    ui->ch_old_dates->setEnabled(!checked);

    if (ui->rb_genOld->isChecked()==checked)
        ui->rb_genOld->setChecked(!checked);
}

void send_to_fond_wnd::on_rb_genOld_clicked(bool checked) {
    ui->pan_genNew->setEnabled(!checked);
    ui->pan_genOld->setEnabled(checked);
    ui->combo_old_point->setEnabled(checked);
    ui->combo_old_event->setEnabled(checked);
    ui->ch_old_dates->setEnabled(checked);

    if (ui->rb_genNew->isChecked()==checked)
        ui->rb_genNew->setChecked(!checked);
}

/*void send_to_fond_wnd::on_ch_old_point_clicked(bool checked) {
    ui->combo_old_point->setEnabled(checked);
}
void send_to_fond_wnd::on_ch_old_event_clicked(bool checked) {
    ui->combo_old_event->setEnabled(checked);
}*/
void send_to_fond_wnd::on_ch_old_dates_clicked(bool checked) {
    ui->date_from->setEnabled(checked);
    ui->date_to->setEnabled(checked);
    ui->lab_date_from->setEnabled(checked);
    ui->lab_date_to->setEnabled(checked);
    ui->lab_days->setEnabled(checked);
    ui->lab_days_2->setEnabled(checked);
}

void send_to_fond_wnd::on_date_from_dateChanged(const QDate &date) {
    if (ui->date_from->date()>QDate::currentDate()) {
        ui->date_from->setDate(QDate::currentDate());
    }
    if (ui->date_from->date()>ui->date_to->date()) {
        ui->date_to->setDate(ui->date_from->date());
    }
    int days = ui->date_from->date().daysTo(ui->date_to->date()) +1;
    ui->lab_days->setText(QString::number(days));
}

void send_to_fond_wnd::on_date_to_dateChanged(const QDate &date) {
    if (ui->date_to->date()>QDate::currentDate()) {
        ui->date_to->setDate(QDate::currentDate());
    }
    if (ui->date_from->date()>ui->date_to->date()) {
        ui->date_to->setDate(ui->date_from->date());
    }
    int days = ui->date_from->date().daysTo(ui->date_to->date()) +1;
    ui->lab_days->setText(QString::number(days));
}

void send_to_fond_wnd::on_bn_today_clicked() {
    ui->date_to->setDate(QDate::currentDate());
}

void send_to_fond_wnd::on_ch_split_refugees_clicked(bool checked) {
    ui->ch_split_refugees->setChecked(checked);
    ui->ch_split_russians->setChecked(false);
    ui->ch_split_russians_zpatname->setChecked(false);
    ui->ch_split_foreigners->setChecked(false);
    ui->ch_split_foreigners_zpatname->setChecked(false);
    ui->ch_split_others->setChecked(false);
}

void send_to_fond_wnd::on_ch_split_russians_clicked(bool checked) {
    ui->ch_split_refugees->setChecked(false);
    ui->ch_split_russians->setChecked(checked);
    ui->ch_split_russians_zpatname->setChecked(false);
    ui->ch_split_foreigners->setChecked(false);
    ui->ch_split_foreigners_zpatname->setChecked(false);
    ui->ch_split_others->setChecked(false);
}

void send_to_fond_wnd::on_ch_split_foreigners_clicked(bool checked) {
    ui->ch_split_refugees->setChecked(false);
    ui->ch_split_russians->setChecked(false);
    ui->ch_split_russians_zpatname->setChecked(false);
    ui->ch_split_foreigners->setChecked(checked);
    ui->ch_split_foreigners_zpatname->setChecked(false);
    ui->ch_split_others->setChecked(false);
}

void send_to_fond_wnd::on_ch_split_russians_zpatname_clicked(bool checked) {
    ui->ch_split_refugees->setChecked(false);
    ui->ch_split_russians->setChecked(false);
    ui->ch_split_russians_zpatname->setChecked(checked);
    ui->ch_split_foreigners->setChecked(false);
    ui->ch_split_foreigners_zpatname->setChecked(false);
    ui->ch_split_others->setChecked(false);
}

void send_to_fond_wnd::on_ch_split_foreigners_zpatname_clicked(bool checked) {
    ui->ch_split_refugees->setChecked(false);
    ui->ch_split_russians->setChecked(false);
    ui->ch_split_russians_zpatname->setChecked(false);
    ui->ch_split_foreigners->setChecked(false);
    ui->ch_split_foreigners_zpatname->setChecked(checked);
    ui->ch_split_others->setChecked(false);
}

void send_to_fond_wnd::on_ch_split_others_clicked(bool checked) {
    ui->ch_split_refugees->setChecked(false);
    ui->ch_split_russians->setChecked(false);
    ui->ch_split_russians_zpatname->setChecked(false);
    ui->ch_split_foreigners->setChecked(false);
    ui->ch_split_foreigners_zpatname->setChecked(false);
    ui->ch_split_others->setChecked(checked);
}
