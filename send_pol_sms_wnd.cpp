#include "send_pol_sms_wnd.h"
#include "ui_send_pol_sms_wnd.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QTableWidgetItem>

send_pol_sms_wnd::send_pol_sms_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    QDialog(parent), db(db), data_app(data_app), settings(settings), ui(new Ui::send_pol_sms_wnd)
{
    ui->setupUi(this);

    refresh_points();
    refresh_fpolis();
    refresh_pers_categ();
    ui->date_vs4from->setDate(QDate::currentDate().addMonths(-1));
    ui->date_vs4to->setDate(QDate::currentDate());

    ui->pan_filter->setVisible(false);
    ui->list_for_sms->setVisible(false);
    ui->bn_sms_reester->setVisible(false);

    //on_bn_gen_list_sms_clicked();
    ui->frame->setEnabled(false);
    ui->bn_sms_reester->setEnabled(false);
    ui->bn_addr_reester->setEnabled(false);
    ui->bn_addr_reester_2->setEnabled(false);
    ui->bn_save_to_csv->setEnabled(false);

    ui->bn_addr_reester->setVisible(ui->rb_back->isChecked());
    ui->bn_addr_reester_2->setVisible(ui->rb_back->isChecked());
    ui->frame_3->setVisible(ui->rb_back->isChecked());

    ui->rb_oldUDLs->setChecked(false);
    ui->pan_olddocs->setEnabled(false);
}

send_pol_sms_wnd::~send_pol_sms_wnd() {
    delete ui;
}

void send_pol_sms_wnd::refresh_pers_categ() {
    this->setCursor(Qt::WaitCursor);

    QString sql = "";
    sql += "select cast(s.code as text) as code, '('||s.code||')  '||s.text as name "
           " from spr_v013 s "
           " order by s.code ; ";
    QSqlQuery *query = new QSqlQuery(db);
    query->exec( sql );
    ui->combo_pers_categ->clear();

    // заполнение списка пунктов выдачи
    ui->combo_pers_categ->addItem(" - все категории - ");
    ui->combo_pers_categ->addItem(" + (1,6)  Все граждане Российской Федерации", "1,6");
    ui->combo_pers_categ->addItem(" + (5,10)  Все беженцы", "5,10");
    ui->combo_pers_categ->addItem(" + (2,3,4,7,8,9)  Все иностранные граждане и лица без гражданства", "2,3,4,7,8,9");
    ui->combo_pers_categ->addItem(" + (11,12,13,14)  Все граждане стран-членов ЕАЭС", "11,12,13,14");
    while (query->next()) {
        ui->combo_pers_categ->addItem( query->value(1).toString(),
                                       QString::number(query->value(0).toInt()) );
    }
    query->finish();
    delete query;
    ui->combo_pers_categ->setCurrentIndex(0);

    this->setCursor(Qt::ArrowCursor);
}

void send_pol_sms_wnd::refresh_points() {
    this->setCursor(Qt::WaitCursor);

    QString sql = "";
    sql += QString("select p.id, '('||p.point_regnum||') '||p.point_name as name"
                   " from points p "
                   " order by p.point_regnum ; ");
    QSqlQuery *query = new QSqlQuery(db);
    query->exec( sql );
    ui->combo_point->clear();

    // заполнение списка пунктов выдачи
    ui->combo_point->addItem(" - все ПВП - ");
    while (query->next()) {
        ui->combo_point->addItem( query->value(1).toString(),
                                  query->value(0).toInt() );
    }
    query->finish();
    delete query;
    ui->combo_point->setCurrentIndex(0);

    this->setCursor(Qt::ArrowCursor);
}

void send_pol_sms_wnd::refresh_fpolis() {
    this->setCursor(Qt::WaitCursor);

    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "";
    sql += QString("select s.code, '('||s.code||') '||s.text as name"
                   " from spr_fpolis s "
                   " where s.code>0 and s.code<3 "
                   " order by s.code ; ");
    query->exec( sql );
    ui->combo_fpolis->clear();

    // заполнение списка пунктов выдачи
    ui->combo_fpolis->addItem(" - любые полисы - ");
    while (query->next()) {
        ui->combo_fpolis->addItem( query->value(1).toString(),
                                  query->value(0).toInt() );
    }
    query->finish();
    delete query;
    ui->combo_fpolis->setCurrentIndex(0);

    this->setCursor(Qt::ArrowCursor);
}

void send_pol_sms_wnd::on_bn_close_clicked() {
    close();
}

void send_pol_sms_wnd::on_bn_gen_list_sms_clicked() {
    // сформируем список телефонов полисов вплоть до указанной даты, для которых рассылка не проводилась
    sql_phones = "";

    // полученные банки ЕНП
    if (ui->rb_new->isChecked()) {
        sql_phones += "select distinct "
                      "       COALESCE(e.phone_cell, e.phone_home, e.phone_work) as phone, "
                      "       e.id, COALESCE(e.fam,' ')||' '||COALESCE(e.im,' ')||' '||COALESCE(e.ot,' ') as fio, e.date_birth, "
                      "       vs.id, vs.vs_num, vs.date_begin, "
                      "       bp.pol_ser, bp.pol_num, bp.date_add, bp.date_sms, "
                      "       case qtt.result "
                      "        when -4 then 'возврат письма' "
                      "        when -3 then 'номер / адрес не существует' "
                      "        when -2 then 'чужой номер / адрес' "
                      "        when -1 then 'недозвон / нет ответа' "
                      "        when 1  then 'ответил лично' "
                      "        when 2  then 'ответил родственик / знакомый' "
                      "        when 3  then 'письмо отправлено / получено' "
                      "        else ' - не определён - ' end as result, "
                      "       case "
                      "        when e.id_addr_liv is not NULL "
                      "         then coalesce(al.subj_name,'')||','||coalesce(al.dstr_name,'')||','||coalesce(al.city_name,'')||','||coalesce(al.nasp_name,'')||','||coalesce(al.strt_name,'')||coalesce(al.house,0)||coalesce(ar.quart,'') "
                      "        when e.id_addr_reg is not NULL "
                      "         then coalesce(ar.subj_name,'')||','||coalesce(ar.dstr_name,'')||','||coalesce(ar.city_name,'')||','||coalesce(ar.nasp_name,'')||','||coalesce(ar.strt_name,'')||coalesce(ar.house,0)||coalesce(ar.quart,'') "
                      "        else '' "
                      "       end as address "
                      "  from persons e "
                      "  left join polises vs  on( e.id_polis=vs.id  and vs.pol_v=2  ) "
                      "  left join blanks_pol bp on( e.id=bp.id_person and bp.id in(select max(id) from blanks_pol where id_person=e.id) ) "
                      "  left join polises pol on( bp.id_polis=pol.id and pol.pol_v=3 ) "
                      "  left join addresses ar on( ar.id=e.id_addr_reg) "
                      "  left join addresses al on( al.id=e.id_addr_liv) "
                      "  left join (select tt._id_person, max(tt.result) as result from talks tt where tt.type_talk=3 group by tt._id_person ) qtt on(qtt._id_person=e.id) "
                      " where bp.id is not NULL ";

        if (ui->ch_no_sms->isChecked()) {
            sql_phones+= " and bp.date_sms is NULL ";
        }

        if (ui->ch_no_activate->isChecked() && ui->ch_no_get2hand->isChecked()) {
            sql_phones+= " and ( ( bp.status=0 and pol.id is NULL ) "
                         "    or ( pol.id is not NULL  and  pol.date_activate is not NULL  and pol.date_get2hand is NULL ) ) ";
        } else if (ui->ch_no_activate->isChecked()) {
            sql_phones+= " and ( bp.status=0 and pol.id is NULL ) ";
        } else if (ui->ch_no_get2hand->isChecked()) {
            sql_phones+= " and ( pol.id is not NULL  and  pol.date_activate is not NULL  and pol.date_get2hand is NULL ) ";
        }
    }
    else
    // лица с просроченными ВС
    if (ui->rb_back->isChecked()) {
        // проверка даты
        if (ui->rb_back->isChecked() && ui->date_vs4from->date()<QDate(1950,1,1)) {
            QMessageBox::warning(this, "Задайте предельную дату", "Дата начала диапазона не должны быть раньше 1 января 1950 г.");
        }
        if (ui->rb_back->isChecked() && ui->date_vs4to->date()<QDate(1950,1,1)) {
            QMessageBox::warning(this, "Задайте предельную дату", "Дата конца диапазона не должны быть раньше 1 января 1950 г.");
        }
        if (ui->rb_back->isChecked() && ui->date_vs4from->date()>ui->date_vs4to->date()) {
            QMessageBox::warning(this, "Задайте предельную дату", "Дата конца диапазона не может быть раньше даты начала диапазона.");
        }

        // просроченные ВС
        if (ui->rb_vs4exp->isChecked()) {
            sql_phones += "select distinct "
                          "       COALESCE(e.phone_cell, e.phone_home, e.phone_work) as phone, "
                          "       e.id, COALESCE(e.fam,' ')||' '||COALESCE(e.im,' ')||' '||COALESCE(e.ot,' ') as fio, e.date_birth, "
                          "       vs.id, vs.vs_num, vs.date_begin, "
                          "       bp.pol_ser, bp.pol_num, bp.date_add, bp.date_sms, "
                          "       case qtt.result "
                          "        when -4 then '- возврат письма' "
                          "        when -3 then '- номер / адрес не существует' "
                          "        when -2 then '- чужой номер / адрес' "
                          "        when -1 then '- недозвон / нет ответа' "
                          "        when 1  then '+ ответил лично' "
                          "        when 2  then '+ ответил родственик / знакомый' "
                          "        when 3  then '+ письмо отправлено / получено' "
                          "        else ' - не определён - ' end as result, "
                          "       case "
                          "        when e.id_addr_liv is not NULL "
                          "         then coalesce(al.subj_name,'')||','||coalesce(al.dstr_name,'')||','||coalesce(al.city_name,'')||','||coalesce(al.nasp_name,'')||','||coalesce(al.strt_name,'')||','||coalesce(al.house,0)||','||coalesce(ar.quart,'') "
                          "        when e.id_addr_reg is not NULL "
                          "         then coalesce(ar.subj_name,'')||','||coalesce(ar.dstr_name,'')||','||coalesce(ar.city_name,'')||','||coalesce(ar.nasp_name,'')||','||coalesce(ar.strt_name,'')||','||coalesce(ar.house,0)||','||coalesce(ar.quart,'') "
                          "        else '' "
                          "       end as address "
                          "  from persons e "
                          "  left join addresses ar on( ar.id=e.id_addr_reg) "
                          "  left join addresses al on( al.id=e.id_addr_liv) "
                          "       join polises vs on( e.id_polis=vs.id and vs.pol_v=2 and CURRENT_DATE-vs.date_end>=" + QString::number(ui->spin_vs4exp->value()) + ") "
                          "       join blanks_pol bp on( e.id=bp.id_person and bp.id in(select max(id) from blanks_pol where id_person=e.id) and bp.status=0) "
                          "  left join (select tt._id_person, max(tt.result) as result from talks tt where tt.type_talk=3 group by tt._id_person ) qtt on(qtt._id_person=e.id) "
                          " where COALESCE(e.phone_cell, e.phone_home, e.phone_work) is not NULL ";

        } else
        // ВС, которым осталось 7 дней
        if (ui->rb_vs4days->isChecked()) {
            sql_phones += "select distinct "
                          "       COALESCE(e.phone_cell, e.phone_home, e.phone_work) as phone, "
                          "       e.id, COALESCE(e.fam,' ')||' '||COALESCE(e.im,' ')||' '||COALESCE(e.ot,' ') as fio, e.date_birth, "
                          "       vs.id, vs.vs_num, vs.date_begin, "
                          "       bp.pol_ser, bp.pol_num, bp.date_add, bp.date_sms, "
                          "       case qtt.result "
                          "        when -4 then 'возврат письма' "
                          "        when -3 then 'номер / адрес не существует' "
                          "        when -2 then 'чужой номер / адрес' "
                          "        when -1 then 'недозвон / нет ответа' "
                          "        when 1  then 'ответил лично' "
                          "        when 2  then 'ответил родственик / знакомый' "
                          "        when 3  then 'письмо отправлено / получено' "
                          "        else ' - не определён - ' end as result, "
                          "       case "
                          "        when e.id_addr_liv is not NULL "
                          "         then coalesce(al.subj_name,'')||','||coalesce(al.dstr_name,'')||','||coalesce(al.city_name,'')||','||coalesce(al.nasp_name,'')||','||coalesce(al.strt_name,'')||','||coalesce(al.house,0)||coalesce(ar.quart,'') "
                          "        when e.id_addr_reg is not NULL "
                          "         then coalesce(ar.subj_name,'')||','||coalesce(ar.dstr_name,'')||','||coalesce(ar.city_name,'')||','||coalesce(ar.nasp_name,'')||','||coalesce(ar.strt_name,'')||','||coalesce(ar.house,0)||coalesce(ar.quart,'') "
                          "        else '' "
                          "       end as address "
                          "  from persons e "
                          "  left join addresses ar on( ar.id=e.id_addr_reg) "
                          "  left join addresses al on( al.id=e.id_addr_liv) "
                          "       join polises vs on( e.id_polis=vs.id and vs.pol_v=2 and vs.date_end-CURRENT_DATE=" + QString::number(ui->spin_vs4days->value()) + ") "
                          "       join blanks_pol bp on( e.id=bp.id_person and bp.id in(select max(id) from blanks_pol where id_person=e.id) and bp.status=0) "
                          "  left join (select tt._id_person, max(tt.result) as result from talks tt where tt.type_talk=3 group by tt._id_person ) qtt on(qtt._id_person=e.id) "
                          " where COALESCE(e.phone_cell, e.phone_home, e.phone_work) is not NULL ";

        } else
        // ВС на полисы, полученные за период
        if (ui->rb_vs4date->isChecked()) {
            sql_phones += "select distinct "
                          "       COALESCE(e.phone_cell, e.phone_home, e.phone_work) as phone, "
                          "       e.id, COALESCE(e.fam,' ')||' '||COALESCE(e.im,' ')||' '||COALESCE(e.ot,' ') as fio, e.date_birth, "
                          "       vs.id, vs.vs_num, vs.date_begin, "
                          "       bp.pol_ser, bp.pol_num, bp.date_add, bp.date_sms, "
                          "       case qtt.result "
                          "        when -4 then 'возврат письма' "
                          "        when -3 then 'номер / адрес не существует' "
                          "        when -2 then 'чужой номер / адрес' "
                          "        when -1 then 'недозвон / нет ответа' "
                          "        when 1  then 'ответил лично' "
                          "        when 2  then 'ответил родственик / знакомый' "
                          "        when 3  then 'письмо отправлено / получено' "
                          "        else ' - не определён - ' end as result, "
                          "       case "
                          "        when e.id_addr_liv is not NULL "
                          "         then coalesce(al.subj_name,'')||','||coalesce(al.dstr_name,'')||','||coalesce(al.city_name,'')||','||coalesce(al.nasp_name,'')||','||coalesce(al.strt_name,'')||','||coalesce(al.house,0)||coalesce(ar.quart,'') "
                          "        when e.id_addr_reg is not NULL "
                          "         then coalesce(ar.subj_name,'')||','||coalesce(ar.dstr_name,'')||','||coalesce(ar.city_name,'')||','||coalesce(ar.nasp_name,'')||','||coalesce(ar.strt_name,'')||','||coalesce(ar.house,0)||coalesce(ar.quart,'') "
                          "        else '' "
                          "       end as address "
                          "  from persons e "
                          "  left join addresses ar on( ar.id=e.id_addr_reg) "
                          "  left join addresses al on( al.id=e.id_addr_liv) "
                          "       join polises vs on( e.id=vs.id_person and vs.id in(select max(id) from polises where id_person=e.id) ) "
                          "       join blanks_pol bp on( e.id=bp.id_person and bp.id in(select max(id) from blanks_pol where id_person=e.id) )"
                          "  left join (select tt._id_person, max(tt.result) as result from talks tt where tt.type_talk=3 group by tt._id_person ) qtt on(qtt._id_person=e.id) "
                          " where COALESCE(e.phone_cell, e.phone_home, e.phone_work) is not NULL "
                          "   and bp.status<>4 "
                          "   and (select * from st_person(e.id, NULL))>-100 ";

        }

        if (ui->ch_no_sms->isChecked()) {
            sql_phones+= "   and bp.date_sms is NULL ";
        }
        if (ui->rb_vs4date->isChecked()) {
            sql_phones+= "   and bp.date_add>='" + ui->date_vs4from->date().toString("yyyy-MM-dd") + "' and bp.date_add<='" + ui->date_vs4to->date().toString("yyyy-MM-dd") + "' ";
        }
    }
    else
    // граждане РФ с просроченными паспортами
    if (ui->rb_oldRFpassport->isChecked()) {
        sql_phones += "select distinct "
                      "       COALESCE(e.phone_cell, e.phone_home, e.phone_work) as phone, "
                      "       e.id, COALESCE(e.fam,' ')||' '||COALESCE(e.im,' ')||' '||COALESCE(e.ot,' ') as fio, e.date_birth, "
                      "       NULL, NULL, NULL, NULL, NULL, NULL, NULL, "
                      "       case qtt.result "
                      "        when -4 then 'возврат письма' "
                      "        when -3 then 'номер / адрес не существует' "
                      "        when -2 then 'чужой номер / адрес' "
                      "        when -1 then 'недозвон / нет ответа' "
                      "        when 1  then 'ответил лично' "
                      "        when 2  then 'ответил родственик / знакомый' "
                      "        when 3  then 'письмо отправлено / получено' "
                      "        else ' - не определён - ' end as result, "
                      "       case "
                      "        when e.id_addr_liv is not NULL "
                      "         then coalesce(al.subj_name,'')||','||coalesce(al.dstr_name,'')||','||coalesce(al.city_name,'')||','||coalesce(al.nasp_name,'')||','||coalesce(al.strt_name,'')||','||coalesce(al.house,0)||coalesce(ar.quart,'') "
                      "        when e.id_addr_reg is not NULL "
                      "         then coalesce(ar.subj_name,'')||','||coalesce(ar.dstr_name,'')||','||coalesce(ar.city_name,'')||','||coalesce(ar.nasp_name,'')||','||coalesce(ar.strt_name,'')||','||coalesce(ar.house,0)||coalesce(ar.quart,'') "
                      "        else '' "
                      "       end as address "
                      "  from persons e "
                      "  left join addresses ar on( ar.id=e.id_addr_reg) "
                      "  left join addresses al on( al.id=e.id_addr_liv) "
                      "  left join (select tt._id_person, max(tt.result) as result from talks tt where tt.type_talk=3 group by tt._id_person ) qtt on(qtt._id_person=e.id) "
                      " where e.status in(11,12,13,14) ";
    }
    else
    // просроченные УДЛ/ДРП
    if ( ui->rb_oldUDLs->isChecked()
         || ui->rb_oldDRPs->isChecked() ) {
        sql_phones += "select distinct "
                      "       COALESCE(e.phone_cell, e.phone_home, e.phone_work) as phone, "
                      "       e.id, COALESCE(e.fam,' ')||' '||COALESCE(e.im,' ')||' '||COALESCE(e.ot,' ') as fio, e.date_birth, "
                      "       NULL, NULL, NULL, NULL, NULL, NULL, NULL, "
                      "       case qtt.result "
                      "        when -4 then 'возврат письма' "
                      "        when -3 then 'номер / адрес не существует' "
                      "        when -2 then 'чужой номер / адрес' "
                      "        when -1 then 'недозвон / нет ответа' "
                      "        when 1  then 'ответил лично' "
                      "        when 2  then 'ответил родственик / знакомый' "
                      "        when 3  then 'письмо отправлено / получено' "
                      "        else ' - не определён - ' end as result, "
                      "       case "
                      "        when e.id_addr_liv is not NULL "
                      "         then coalesce(al.subj_name,'')||','||coalesce(al.dstr_name,'')||','||coalesce(al.city_name,'')||','||coalesce(al.nasp_name,'')||','||coalesce(al.strt_name,'')||','||coalesce(al.house,0)||coalesce(ar.quart,'') "
                      "        when e.id_addr_reg is not NULL "
                      "         then coalesce(ar.subj_name,'')||','||coalesce(ar.dstr_name,'')||','||coalesce(ar.city_name,'')||','||coalesce(ar.nasp_name,'')||','||coalesce(ar.strt_name,'')||','||coalesce(ar.house,0)||coalesce(ar.quart,'') "
                      "        else '' "
                      "       end as address "
                      "  from persons e "
                      "  left join persons_docs d on( d.id_person=e.id and d.status=0 and d.doc_type in(#DOC_TYPE#) ) "
                      "  left join addresses ar on( ar.id=e.id_addr_reg) "
                      "  left join addresses al on( al.id=e.id_addr_liv) "
                      "  left join (select tt._id_person, max(tt.result) as result from talks tt where tt.type_talk=3 group by tt._id_person ) qtt on(qtt._id_person=e.id) "
                      " where d.doc_exp is not NULL  and  #DATE_EXP# ";

        QString s_doc_type = "";
        if (ui->rb_oldUDLs->isChecked())
            s_doc_type = "1, 13, 14, 3, 24, 10, 12, 25, 27, 28, 9, 21, 22";
        else if (ui->rb_oldDRPs->isChecked())
            s_doc_type = "11, 23, 26, 29";
        sql_phones = sql_phones.replace("#DOC_TYPE#", s_doc_type);

        QString s_date_exp = "";
        if (ui->rb_doc4days->isChecked())
            s_date_exp = " d.doc_exp>=CURRENT_DATE and d.doc_exp<='" + QDate::currentDate().addDays( ui->spin_doc4days->value() ).toString("yyyy-MM-dd") + "' ";
        else if (ui->rb_doc4exp->isChecked())
            s_date_exp = " d.doc_exp<=CURRENT_DATE and d.doc_exp>='" + QDate::currentDate().addDays( (-1) * ui->spin_doc4exp->value() ).toString("yyyy-MM-dd") + "' ";
        sql_phones = sql_phones.replace("#DATE_EXP#", s_date_exp);
    }
    else
    // просроченные полисы единого образца
    if ( ui->rb_oldENPs->isChecked() ) {
        sql_phones += "select distinct "
                      "       COALESCE(e.phone_cell, e.phone_home, e.phone_work) as phone, "
                      "       e.id, COALESCE(e.fam,' ')||' '||COALESCE(e.im,' ')||' '||COALESCE(e.ot,' ') as fio, e.date_birth, "
                      "       NULL, NULL, NULL, NULL, NULL, NULL, NULL, "
                      "       case qtt.result "
                      "        when -4 then 'возврат письма' "
                      "        when -3 then 'номер / адрес не существует' "
                      "        when -2 then 'чужой номер / адрес' "
                      "        when -1 then 'недозвон / нет ответа' "
                      "        when 1  then 'ответил лично' "
                      "        when 2  then 'ответил родственик / знакомый' "
                      "        when 3  then 'письмо отправлено / получено' "
                      "        else ' - не определён - ' end as result, "
                      "       case "
                      "        when e.id_addr_liv is not NULL "
                      "         then coalesce(al.subj_name,'')||','||coalesce(al.dstr_name,'')||','||coalesce(al.city_name,'')||','||coalesce(al.nasp_name,'')||','||coalesce(al.strt_name,'')||','||coalesce(al.house,0)||coalesce(ar.quart,'') "
                      "        when e.id_addr_reg is not NULL "
                      "         then coalesce(ar.subj_name,'')||','||coalesce(ar.dstr_name,'')||','||coalesce(ar.city_name,'')||','||coalesce(ar.nasp_name,'')||','||coalesce(ar.strt_name,'')||','||coalesce(ar.house,0)||coalesce(ar.quart,'') "
                      "        else '' "
                      "       end as address "
                      "  from persons e "
                      "       join polises p on( p.id=e.id_polis and p.pol_v=3 ) "
                      "  left join addresses ar on( ar.id=e.id_addr_reg) "
                      "  left join addresses al on( al.id=e.id_addr_liv) "
                      "  left join (select tt._id_person, max(tt.result) as result from talks tt where tt.type_talk=3 group by tt._id_person ) qtt on(qtt._id_person=e.id) "
                      " where p.date_end is not NULL  and  #DATE_EXP# ";

        QString s_date_exp = "";
        if (ui->rb_doc4days->isChecked())
            s_date_exp = " p.date_end>=CURRENT_DATE and p.date_end<='" + QDate::currentDate().addDays( ui->spin_doc4days->value() ).toString("yyyy-MM-dd") + "' ";
        else if (ui->rb_doc4exp->isChecked())
            s_date_exp = " p.date_end<=CURRENT_DATE and p.date_end>='" + QDate::currentDate().addDays( (-1) * ui->spin_doc4exp->value() ).toString("yyyy-MM-dd") + "' ";
        sql_phones = sql_phones.replace("#DATE_EXP#", s_date_exp);
    }


    // применим параметры филтрации
    if (ui->combo_pers_categ->currentIndex()>0) {
        sql_phones+= "   and e.category in(" + ui->combo_pers_categ->currentData().toString() + ") ";
    }
    if (ui->combo_fpolis->currentIndex()>0) {
        sql_phones+= "   and vs.f_polis=" + QString::number(ui->combo_fpolis->currentData().toInt()) + " ";
    }
    if (ui->combo_point->currentIndex()>0) {
        sql_phones+= "   and ((vs._id_last_point is NULL) or (vs._id_last_point=" + QString::number(ui->combo_point->currentData().toInt()) + ")) ";
    }
    if (ui->combo_polis_post_way->currentIndex()>0) {
        sql_phones+= "   and e.polis_post_way=" + QString::number(ui->combo_polis_post_way->currentIndex()) + " ";
    }

    // cсортировка по ФИО и д/р
    //sql_phones+= " order by e.fam, e.im, e.ot, e.date_birth ; ";
    sql_phones+= " order by COALESCE(e.fam,' ')||' '||COALESCE(e.im,' ')||' '||COALESCE(e.ot,' '), e.date_birth ; ";

    this->setCursor(Qt::WaitCursor);

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql_phones, QString("Выбор сотовых для новых полученных полисов"), *query, true, db, data_app);
    if (!res) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
    }

    model_phones.setQuery(sql_phones,db);
    QString err2 = model_phones.lastError().driverText();

    // подключаем модель из БД
    ui->tab_phones->setModel(&model_phones);

    // обновляем таблицу
    ui->tab_phones->reset();

    // задаём ширину колонок
    ui->tab_phones->setColumnWidth( 0,100);     // e.phone_cell
    ui->tab_phones->setColumnWidth( 1,  1);     // e.id
    ui->tab_phones->setColumnWidth( 2,200);     // fio
    ui->tab_phones->setColumnWidth( 3, 80);     // e.date_birth
    ui->tab_phones->setColumnWidth( 4,  1);     // pol_id
    ui->tab_phones->setColumnWidth( 5, 70);     // vs.vs_num
    ui->tab_phones->setColumnWidth( 6, 80);     // date_begin
    ui->tab_phones->setColumnWidth( 7, 50);     // bp.pol_ser
    ui->tab_phones->setColumnWidth( 8, 70);     // bp.pol_num
    ui->tab_phones->setColumnWidth( 9, 80);     // bp.date_add
    ui->tab_phones->setColumnWidth(10, 80);     // bp.date_sms
    ui->tab_phones->setColumnWidth(11,150);     // result
    ui->tab_phones->setColumnWidth(12,200);     // address

    // правим заголовки
    model_phones.setHeaderData( 0, Qt::Horizontal, ("телефон"));
    model_phones.setHeaderData( 1, Qt::Horizontal, ("id_pers"));
    model_phones.setHeaderData( 2, Qt::Horizontal, ("ФИО"));
    model_phones.setHeaderData( 3, Qt::Horizontal, ("дата \nрождения"));
    model_phones.setHeaderData( 4, Qt::Horizontal, ("id_pol"));
    model_phones.setHeaderData( 5, Qt::Horizontal, ("номер ВС"));
    model_phones.setHeaderData( 6, Qt::Horizontal, ("дата ВС"));
    model_phones.setHeaderData( 7, Qt::Horizontal, ("серия \nбланка"));
    model_phones.setHeaderData( 8, Qt::Horizontal, ("номер \nбланка"));
    model_phones.setHeaderData( 9, Qt::Horizontal, ("дата \nбланка"));
    model_phones.setHeaderData(10, Qt::Horizontal, ("дата \nСМС"));
    model_phones.setHeaderData(11, Qt::Horizontal, ("результат \nпосл.звонка"));
    model_phones.setHeaderData(12, Qt::Horizontal, ("адрес \nпроживания"));
    ui->tab_phones->repaint();

    if (!ui->ch_filter->isChecked()) {
        ui->tab_phones->clearSelection();
        ui->lab_cnt->setText(QString::number(model_phones.rowCount()));
    } else {
        ui->lab_cnt->setText(QString::number(ui->tab_phones->selectionModel()->selectedIndexes().count()/10));
    }





    // сформируем массив телефонов
    phones.clear();

    if (ui->ch_filter->isChecked()) {
        // добавим только выделенные
        QModelIndexList list = ui->tab_phones->selectionModel()->selectedIndexes();

        for (int i=0; i<list.count(); i++) {
            QModelIndex index = list.at(i);
            if (index.column()!=0)
                continue;

            s_data_phone rec;
            rec.phone    = model_phones.data(model_phones.index(index.row(), 0), Qt::EditRole).toString();
            rec.id_person= model_phones.data(model_phones.index(index.row(), 1), Qt::EditRole).toInt();
            rec.fio      = model_phones.data(model_phones.index(index.row(), 2), Qt::EditRole).toString();
            rec.date_r   = model_phones.data(model_phones.index(index.row(), 3), Qt::EditRole).toDate();
            rec.id_vs    = model_phones.data(model_phones.index(index.row(), 4), Qt::EditRole).toInt();
            rec.vs_num   = model_phones.data(model_phones.index(index.row(), 5), Qt::EditRole).toString();
            rec.vs_date  = model_phones.data(model_phones.index(index.row(), 6), Qt::EditRole).toDate();
            rec.pol_ser  = model_phones.data(model_phones.index(index.row(), 7), Qt::EditRole).toString();
            rec.pol_num  = model_phones.data(model_phones.index(index.row(), 8), Qt::EditRole).toString();
            rec.date_add = model_phones.data(model_phones.index(index.row(), 9), Qt::EditRole).toDate();
            rec.date_sms = model_phones.data(model_phones.index(index.row(),10), Qt::EditRole).toDate();
            rec.result   = model_phones.data(model_phones.index(index.row(),11), Qt::EditRole).toString();
            rec.address  = model_phones.data(model_phones.index(index.row(),12), Qt::EditRole).toString();

            phones.append(rec);
        }
    } else {
        // добавлим все телефоны из запроса

        QSqlQuery *query = new QSqlQuery(db);
        query->exec( this->sql_phones );
        while (query->next()) {

            s_data_phone rec;
            rec.phone    = query->value( 0).toString();
            rec.id_person= query->value( 1).toInt();
            rec.fio      = query->value( 2).toString();
            rec.date_r   = query->value( 3).toDate();
            rec.id_vs    = query->value( 4).toInt();
            rec.vs_num   = query->value( 5).toString();
            rec.vs_date  = query->value( 6).toDate();
            rec.pol_ser  = query->value( 7).toString();
            rec.pol_num  = query->value( 8).toString();
            rec.date_add = query->value( 9).toDate();
            rec.date_sms = query->value(10).toDate();
            rec.result   = query->value(11).toString();
            rec.address  = query->value(12).toString();

            phones.append(rec);
        }
        query->finish();
        delete query;
    }


    this->setCursor(Qt::ArrowCursor);
    ui->frame->setEnabled(true);
    ui->bn_sms_reester->setEnabled(true);
    ui->bn_addr_reester->setEnabled(true);
    ui->bn_addr_reester_2->setEnabled(true);
    ui->bn_save_to_csv->setEnabled(true);
}

void send_pol_sms_wnd::on_date_vs4from_editingFinished() {
    //on_bn_gen_list_sms_clicked();
    ui->frame->setEnabled(false);
    ui->bn_sms_reester->setEnabled(false);
    ui->bn_addr_reester->setEnabled(false);
    ui->bn_addr_reester_2->setEnabled(false);
    ui->frame_3->setVisible(false);
    ui->bn_save_to_csv->setEnabled(false);
}
void send_pol_sms_wnd::on_date_vs4to_editingFinished() {
    //on_bn_gen_list_sms_clicked();
    ui->frame->setEnabled(false);
    ui->bn_sms_reester->setEnabled(false);
    ui->bn_addr_reester->setEnabled(false);
    ui->bn_addr_reester_2->setEnabled(false);
    ui->frame_3->setVisible(false);
    ui->bn_save_to_csv->setEnabled(false);
}
void send_pol_sms_wnd::on_date_to_editingFinished() {
    //on_bn_gen_list_sms_clicked();
    ui->frame->setEnabled(false);
    ui->bn_sms_reester->setEnabled(false);
    ui->bn_addr_reester->setEnabled(false);
    ui->bn_addr_reester_2->setEnabled(false);
    ui->frame_3->setVisible(false);
    ui->bn_save_to_csv->setEnabled(false);
}
void send_pol_sms_wnd::on_ch_phone_clicked() {
    //on_bn_gen_list_sms_clicked();
    ui->frame->setEnabled(false);
    ui->bn_sms_reester->setEnabled(false);
    ui->bn_addr_reester->setEnabled(false);
    ui->bn_addr_reester_2->setEnabled(false);
    ui->frame_3->setVisible(false);
    ui->bn_save_to_csv->setEnabled(false);
}
void send_pol_sms_wnd::on_combo_point_activated(int index) {
    //on_bn_gen_list_sms_clicked();
    ui->frame->setEnabled(false);
    ui->bn_sms_reester->setEnabled(false);
    ui->bn_addr_reester->setEnabled(false);
    ui->bn_addr_reester_2->setEnabled(false);
    ui->frame_3->setVisible(false);
    ui->bn_save_to_csv->setEnabled(false);
}
void send_pol_sms_wnd::on_combo_fpolis_activated(int index) {
    //on_bn_gen_list_sms_clicked();
    ui->frame->setEnabled(false);
    ui->bn_sms_reester->setEnabled(false);
    ui->bn_addr_reester->setEnabled(false);
    ui->bn_addr_reester_2->setEnabled(false);
    ui->frame_3->setVisible(false);
    ui->bn_save_to_csv->setEnabled(false);
}
void send_pol_sms_wnd::on_combo_polis_post_way_activated(int index) {
    //on_bn_gen_list_sms_clicked();
    ui->frame->setEnabled(false);
    ui->bn_sms_reester->setEnabled(false);
    ui->bn_addr_reester->setEnabled(false);
    ui->bn_addr_reester_2->setEnabled(false);
    ui->bn_save_to_csv->setEnabled(false);
}
void send_pol_sms_wnd::on_ch_no_activate_clicked(bool checked) {
    //on_bn_gen_list_sms_clicked();
    ui->frame->setEnabled(false);
    ui->bn_sms_reester->setEnabled(false);
    ui->bn_addr_reester->setEnabled(false);
    ui->bn_addr_reester_2->setEnabled(false);
    ui->frame_3->setVisible(false);
    ui->bn_save_to_csv->setEnabled(false);
}
void send_pol_sms_wnd::on_ch_no_get2hand_clicked(bool checked) {
    //on_bn_gen_list_sms_clicked();
    ui->frame->setEnabled(false);
    ui->bn_sms_reester->setEnabled(false);
    ui->bn_addr_reester->setEnabled(false);
    ui->bn_addr_reester_2->setEnabled(false);
    ui->frame_3->setVisible(false);
    ui->bn_save_to_csv->setEnabled(false);
}
void send_pol_sms_wnd::on_ch_no_sms_clicked(bool checked) {
    //on_bn_gen_list_sms_clicked();
    ui->frame->setEnabled(false);
    ui->bn_sms_reester->setEnabled(false);
    ui->bn_addr_reester->setEnabled(false);
    ui->bn_addr_reester_2->setEnabled(false);
    ui->frame_3->setVisible(false);
    ui->bn_save_to_csv->setEnabled(false);
}

#include "qt_windows.h"
#include "qwindowdefs_win.h"
#include <shellapi.h>

void send_pol_sms_wnd::on_bn_save_to_csv_clicked() {
    //if (ui->rb_new->isChecked()) {
        if (QMessageBox::question(this,
                                  "Данные будут изменены",
                                  "Внимание!\nПосле этой операции все выгруженные номера будут помечены как разосланные и больше рассылаться не будут.\n"
                                  "(при необходимости их можно выгрузить задним числом.)\n\n"
                                  "Выгрузить номера телефонов?",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }
    //}
    QString selFilter="Данные, разделённые запятыми (*.csv);;Текстовые файлы (*.txt)";
    QString fname = QFileDialog::getSaveFileName (this, "Введите имя CSV-файла для сохранения списка телефонов",
                                                  data_app.path_temp + QDate::currentDate().toString("yyyy-MM-dd") + " - " + data_app.filial_name + (ui->combo_polis_post_way->currentIndex()==1 ? " (прийдут лично)" : (ui->combo_polis_post_way->currentIndex()==2 ? " (заказным письмом)" : "")),
                                                  "Данные, разделённые запятыми (*.csv)",
                                                  &selFilter);
    QString freport = data_app.path_out + "_REPORTS_/" +
            QDate::currentDate().toString("yyyy-MM-dd") + "  -  " + data_app.smo_short + " - " + data_app.filial_name + "  -  sms_report - " + QTime::currentTime().toString("hh.mm.ss") + ".csv";
    if (!fname.isEmpty()) {
        db.transaction();

        phones.clear();

        if (ui->ch_filter->isChecked()) {
            // добавим только выделенные
            QModelIndexList list = ui->tab_phones->selectionModel()->selectedIndexes();

            for (int i=0; i<list.count(); i++) {
                QModelIndex index = list.at(i);
                if (index.column()!=0)
                    continue;

                s_data_phone rec;
                rec.phone    = model_phones.data(model_phones.index(index.row(), 0), Qt::EditRole).toString();
                rec.id_person= model_phones.data(model_phones.index(index.row(), 1), Qt::EditRole).toInt();
                rec.fio      = model_phones.data(model_phones.index(index.row(), 2), Qt::EditRole).toString();
                rec.date_r   = model_phones.data(model_phones.index(index.row(), 3), Qt::EditRole).toDate();
                rec.id_vs    = model_phones.data(model_phones.index(index.row(), 4), Qt::EditRole).toInt();
                rec.vs_num   = model_phones.data(model_phones.index(index.row(), 5), Qt::EditRole).toString();
                rec.vs_date  = model_phones.data(model_phones.index(index.row(), 6), Qt::EditRole).toDate();
                rec.pol_ser  = model_phones.data(model_phones.index(index.row(), 7), Qt::EditRole).toString();
                rec.pol_num  = model_phones.data(model_phones.index(index.row(), 8), Qt::EditRole).toString();
                rec.date_add = model_phones.data(model_phones.index(index.row(), 9), Qt::EditRole).toDate();
                rec.date_sms = model_phones.data(model_phones.index(index.row(),10), Qt::EditRole).toDate();
                rec.result   = model_phones.data(model_phones.index(index.row(),11), Qt::EditRole).toString();
                rec.address  = model_phones.data(model_phones.index(index.row(),12), Qt::EditRole).toString();

                phones.append(rec);
            }
        } else {
            // добавлим все телефоны из таблицы

            for (int i=0; i<model_phones.rowCount(); i++) {
                s_data_phone rec;
                rec.phone    = model_phones.data(model_phones.index(i, 0), Qt::EditRole).toString();
                rec.id_person= model_phones.data(model_phones.index(i, 1), Qt::EditRole).toInt();
                rec.fio      = model_phones.data(model_phones.index(i, 2), Qt::EditRole).toString();
                rec.date_r   = model_phones.data(model_phones.index(i, 3), Qt::EditRole).toDate();
                rec.id_vs    = model_phones.data(model_phones.index(i, 4), Qt::EditRole).toInt();
                rec.vs_num   = model_phones.data(model_phones.index(i, 5), Qt::EditRole).toString();
                rec.vs_date  = model_phones.data(model_phones.index(i, 6), Qt::EditRole).toDate();
                rec.pol_ser  = model_phones.data(model_phones.index(i, 7), Qt::EditRole).toString();
                rec.pol_num  = model_phones.data(model_phones.index(i, 8), Qt::EditRole).toString();
                rec.date_add = model_phones.data(model_phones.index(i, 9), Qt::EditRole).toDate();
                rec.date_sms = model_phones.data(model_phones.index(i,10), Qt::EditRole).toDate();
                rec.result   = model_phones.data(model_phones.index(i,11), Qt::EditRole).toString();
                rec.address  = model_phones.data(model_phones.index(i,12), Qt::EditRole).toString();

                phones.append(rec);
            }
        }

        QFile f(fname);
        f.open(QIODevice::WriteOnly);
        f.write("PHONE\n");

        for (int i=0; i<phones.size(); i++) {
            QString phone = phones.at(i).phone;
            if (phone.left(2)=="89" && phone.length()==11) {
                phone = "7" + phone.right(10);
            }
            f.write(phone.toLocal8Bit());
            f.write("\n");

            // отметим для ВС дату рассылки СМС о готовности бланка единого образца
            QString sql_pol = "";
            sql_pol += "update polises "
                       "   set date_sms_pol=CURRENT_DATE "
                       " where id=" + QString::number(phones.at(i).id_vs) + " "
                       "   and date_sms_pol is NULL ; ";
            QSqlQuery *query_pol = new QSqlQuery(db);
            bool res_pol = mySQL.exec(this, sql_pol, QString("Выбор сотовых для новых полученных полисов"), *query_pol, true, db, data_app);
            if (!res_pol) {
                QMessageBox::warning(this, "Данные не обновлены", "При отметке отсылки СМС по событию произошла непредвиденная ошибка.\n\nОперация отменена.");
                delete query_pol;
                db.rollback();
                return;
            }

            // отметим бланк, как разосланный по СМС
            QString sql_blank = "";
            sql_blank += "update blanks_pol "
                         "   set date_sms=CURRENT_DATE "
                         " where pol_ser='" + phones.at(i).pol_ser + "' and pol_num='" + phones.at(i).pol_num + "' "
                         "   and date_sms is NULL ; ";
            QSqlQuery *query_blank = new QSqlQuery(db);
            bool res_blank = mySQL.exec(this, sql_blank, QString("Отметим бланк, как разосланный по СМС"), *query_blank, true, db, data_app);
            if (!res_blank) {
                QMessageBox::warning(this, "Данные не обновлены", "При отметке отсылки СМС по бланку произошла непредвиденная ошибка.\n\nОперация отменена.");
                delete query_blank;
                db.rollback();
                return;
            }

            // если надо - сделаем запись непланового СМС
            if (ui->rb_back->isChecked()) {
                QString sql = "insert into talks (pol_v, _id_blank_pol, type_talk, date_talk, result, comment, _id_person, _id_polis) "
                              " values ("
                              "   2, "
                              "   NULL, "
                              "   2, "
                              "   '" + QDate::currentDate().toString("yyyy-MM-dd") + "', "
                              "   3, "
                              "   'повторная отправка СМС о готовности полиса', "
                              "   " + QString::number(phones.at(i).id_person) + ", "
                              "   " + QString::number(phones.at(i).id_vs) + ") "
                              " returning id ; ";

                QSqlQuery *query = new QSqlQuery(db);
                if (!mySQL.exec(this, sql, "Добавление записи повторной СМС", *query, true, db, data_app) || !query->next()) {
                    db.rollback();
                    QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке добавить запись повторной отправки СМС произошла неожиданная ошибка.\n\nДействие отменено.");
                    delete query;
                    return;
                }
                delete query;
                db.commit();
            }
        }
        f.close();

        long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname.utf16()), 0, 0, SW_NORMAL);
        if (result<32) {
            QMessageBox::warning(this,
                                 "Файл не найден",
                                 "Файл не найден:\n\n" + fname +
                                 "\nКод ошибки: " + QString::number(result) +
                                 "\n\nПроверьте, был ли сохранён целевой файл и установлено ли в системе средство для его просмотра.");
        }
        //close();

        //------------------------------------------------------------
        // генерация реестра СМС
        //------------------------------------------------------------
        bool res = print_registry_SMS();
        if (!res) {
            this->setCursor(Qt::ArrowCursor);
            db.rollback();
            return;
        }
        db.commit();
    } else {
        QMessageBox::warning(this, "Нечего сохранять", "Сначала выберите номера телефонов, которые хотите выгрузить для отправки СМС.\n\nОперация отменена.");
    }
}


// формирование журнала обхода
bool send_pol_sms_wnd::print_registry_addresses() {
    ui->bn_addr_reester->setEnabled(false);
    ui->bn_addr_reester_2->setEnabled(false);
    // распаковка шаблона
    QString rep_folder = data_app.path_reports;
    // распаковка шаблона
    QString tmp_folder = data_app.path_out + "_REPORTS_/temp/";
    QString rep_template = rep_folder + "rep_visiting_reester.ods";

    if (!QFile::exists(rep_template)) {
        QMessageBox::warning(this,
                             "Шаблон не найден",
                             "Не найден шаблон печатной формы половозрастной таблицы застрахованных: \n" + rep_template +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return false;
    }
    QDir tmp_dir(tmp_folder);
    if (tmp_dir.exists(tmp_folder) && !tmp_dir.removeRecursively()) {
        QMessageBox::warning(this,
                             "Ошибка при очистке папки",
                             "Не удалось очистить папку временных файлов: \n" + tmp_folder +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return false;
    }
    tmp_dir.mkpath(tmp_folder);
    QProcess myProcess;
    QString com = "\"" + data_app.path_arch + "7z.exe\" e \"" + rep_template + "\" -o\"" + tmp_folder + "\" content.xml";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка распаковки шаблона",
                             "Не удалось распаковать файл контента шаблона\n" + rep_template + " -> content.xml"
                             "\nпечатной формы во временную папку\n" + tmp_folder +
                             "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return false;
    }
    com = "\"" + data_app.path_arch + "7z.exe\" e \"" + rep_template + "\" -o\"" + tmp_folder + "\" styles.xml";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка распаковки шаблона",
                             "Не удалось распаковать файл стилей шаблона \n" + rep_template + " -> styles.xml"
                             "\nпечатной формы во временную папку\n" + tmp_folder +
                             "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return false;
    }

    // ===========================
    // правка контента шаблона
    // ===========================
    // открытие контента шаблона
    QFile f_content(tmp_folder + "/content.xml");
    f_content.open(QIODevice::ReadOnly);

    // подготовим поток для чтения файла
    QTextStream stream(&f_content);
    QString codec_name = "UTF-8";
    QTextCodec *codec = QTextCodec::codecForName(codec_name.toLocal8Bit());
    stream.setCodec(codec);
    QString s_content = stream.readAll();

    //QString s_content = f_content.readAll();
    f_content.close();
    if (s_content.isEmpty()) {
        QMessageBox::warning(this,
                             "Файл контента шаблона пустой",
                             "Файл контента шаблона пустой. Возможно шаблон был испорчен.\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return false;
    }

    // выбор шаблона
    int pos0 = s_content.indexOf("<table:table-row table:style-name=\"ro3\">", 0);
    int pos1 = s_content.indexOf("</table:table-row>", pos0) + 18;
    QString s_row = s_content.mid(pos0, pos1-pos0);

    // правка полей контента шаблона
    QString s_content_new = s_content.left(pos0);
    /*s_content_new = s_content_new.
            replace("#POINT_NAME#", data_app.point_name).
            replace("#CUR_DATE#", QDate::currentDate().toString("dd.MM.yyyy"));*/
    int n = 0;
    for (int i=0; i<phones.size(); i++) {
        n++;
        QString s_row_new = s_row;
        QString adr_str = phones.at(i).address;
        adr_str = adr_str.replace(",,", ",").replace(",", ", ");
        s_row_new = s_row_new.
                replace("#N#", QString::number(n)).

                replace("#FIO_DR#", phones.at(i).fio + ", " + phones.at(i).date_r.toString("dd.MM.yyyy")).
                replace("#VS_NUM#", phones.at(i).vs_num).
                replace("#VS_DATE#", phones.at(i).vs_date.toString("dd.MM.yyyy")).
                replace("#PHONE#", phones.at(i).phone).
                replace("#ADDRESS#", adr_str).
                replace("#RESULT#", phones.at(i).result);
        s_content_new += s_row_new;
    }
    s_content_new += s_content.right(s_content.size()-pos1);

    // сохранение контента шаблона
    QFile f_content_save(tmp_folder + "/content.xml");
    f_content_save.open(QIODevice::WriteOnly);
    f_content_save.write(s_content_new.toUtf8());
    f_content_save.close();
    // подготовим поток для записи файла
    //QTextStream stream_save(&f_content_save);
    //QString codec_name_save = "Windows-1251";
    //QTextCodec *codec_save = QTextCodec::codecForName(codec_name_save.toLocal8Bit());
    //stream_save.setCodec(codec_save);
    //stream_save << s_content_new.toLocal8Bit();
    //f_content_save.close();




    // ===========================
    // архивация шаблона
    // ===========================
    QString fname_res = data_app.path_out + "_REPORTS_/" +
            QDate::currentDate().toString("yyyy-MM-dd") + "  -  " + data_app.smo_short + " - " + data_app.filial_name + "  -  visiting_reester.ods";

    QFile::remove(fname_res);
    while (QFile::exists(fname_res)) {
        if (QMessageBox::warning(this,
                                 "Ошибка сохранения журнала ВС",
                                 "Не удалось сохранить форму половозрастной таблицы застрахованных: \n" + rep_template +
                                 "\n\nво временную папку\n" + fname_res +
                                 "\n\nПроверьте, не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                 QMessageBox::Retry|QMessageBox::Abort,
                                 QMessageBox::Retry)==QMessageBox::Abort) {
            this->setCursor(Qt::ArrowCursor);
            return false;
        }
        QFile::remove(fname_res);
    }
    if (!QFile::copy(rep_template, fname_res) || !QFile::exists(fname_res)) {
        QMessageBox::warning(this, "Ошибка копирования отчёта", "Не удалось скопировать форму половозрастной таблицы застрахованных: \n" + rep_template + "\n\nво временную папку\n" + fname_res + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return false;
    }
    com = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname_res + "\" \"" + tmp_folder + "/content.xml\"";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка обновления контента",
                             "При добавлении нового контента в форму половозрастной таблицы застрахованных произошла непредвиденная ошибка\n\nОпреация прервана.");
    }

    // ===========================
    // собственно открытие шаблона
    // ===========================
    // открытие полученного ODT
    long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);

    ui->bn_addr_reester->setEnabled(true);
    ui->bn_addr_reester_2->setEnabled(true);
    QMessageBox::information(this, "Готово", "Готово");
    //close();
    return true;
}


bool send_pol_sms_wnd::print_registry_SMS() {
    int q_n = phones.size();

    // распаковка шаблона реестра
    QString fname_template = data_app.path_reports + "sms_registry.ods";
    QString temp_folder = data_app.path_temp + "sms_registry";
    if (!QFile::exists(fname_template)) {
        QMessageBox::warning(this,
                             "Шаблон не найден",
                             "Не найден шаблон печатной формы реестра выданных СМС: \n" + fname_template +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return false;
    }
    QDir tmp_dir(temp_folder);
    if (tmp_dir.exists(temp_folder) && !tmp_dir.removeRecursively()) {
        QMessageBox::warning(this,
                             "Ошибка при очистке папки",
                             "Не удалось очистить папку временных файлов: \n" + temp_folder +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return false;
    }
    tmp_dir.mkpath(temp_folder);
    QProcess myProcess;
    QString com = "\"" + data_app.path_arch + "7z.exe\" e \"" + fname_template + "\" -o\"" + temp_folder + "\" content.xml";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка распаковки шаблона",
                             "Не удалось распаковать файл контента шаблона\n" + fname_template + " -> content.xml"
                             "\nпечатной формы во временную папку\n" + temp_folder +
                             "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return false;
    }
    com = "\"" + data_app.path_arch + "7z.exe\" e \"" + fname_template + "\" -o\"" + temp_folder + "\" styles.xml";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка распаковки шаблона",
                             "Не удалось распаковать файл стилей шаблона \n" + fname_template + " -> styles.xml"
                             "\nпечатной формы во временную папку\n" + temp_folder +
                             "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return false;
    }


    // ===========================
    // правка контента шаблона
    // ===========================
    // открытие контента шаблона
    QFile f_content(temp_folder + "/content.xml");
    f_content.open(QIODevice::ReadOnly);
    QString s_content = f_content.readAll();
    f_content.close();
    if (s_content.isEmpty()) {
        QMessageBox::warning(this,
                             "Файл контента шаблона пустой",
                             "Файл контента шаблона пустой. Возможно шаблон был испорчен.\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return false;
    }
    // выбор шаблона
    int pos0 = s_content.indexOf("<table:table-row table:style-name=\"ro3\">", 0);
    int pos1 = s_content.indexOf("</table:table-row>", pos0) + 18;
    QString s_row = s_content.mid(pos0, pos1-pos0);

    // правка полей контента шаблона
    QString s_content_new = s_content.left(pos0);
    /*s_content_new = s_content_new.
            replace("#POINT_NAME#", data_app.point_name).
            replace("#CUR_DATE#", QDate::currentDate().toString("dd.MM.yyyy"));*/
    int n = 0;
    for (int i=0; i<phones.size(); i++) {
        n++;
        QString s_row_new = s_row;
        s_row_new = s_row_new.
                replace("#N#", QString::number(n)).

                //replace("#N_VS#", phones.at(i).num_vs).
                //replace("#D_VS#", phones.at(i).date_vs.toString("dd.MM.yyyy")).
                replace("#N_VS#", phones.at(i).vs_num).
                replace("#D_VS#", phones.at(i).vs_date.toString("dd.MM.yyyy")).
                //replace("#N_VS#", phones.at(i).pol_ser + " " + phones.at(i).pol_num).
                //replace("#D_VS#", phones.at(i).date_add.toString("dd.MM.yyyy")).

                replace("#FIO#", phones.at(i).fio + ", " + phones.at(n-1).date_r.toString("dd.MM.yyyy")).
                replace("#D_SND#", QDate::currentDate().toString("dd.MM.yyyy")).
                replace("#PHONE#", phones.at(i).phone).
                replace("#STATUS#", "ДОСТАВЛЕНО");
        s_content_new += s_row_new;
    }
    s_content_new += s_content.right(s_content.size()-pos1);

    // сохранение контента шаблона
    QFile f_content_save(temp_folder + "/content.xml");
    f_content_save.open(QIODevice::WriteOnly);
    f_content_save.write(s_content_new.toUtf8());
    f_content_save.close();


    // ===========================
    // архивация шаблона
    // ===========================
    QString fname_res = data_app.path_out + "_REPORTS_/" +
            QDate::currentDate().toString("yyyy-MM-dd") + "  -  " + data_app.smo_short + " - " + data_app.filial_name + "  -  sms_registry.ods";
    QFile::remove(fname_res);
    while (QFile::exists(fname_res)) {
        if (QMessageBox::warning(this,
                                 "Ошибка сохранения журнала СМС",
                                 "Не удалось сохранить журнал СМС: \n" + fname_template +
                                 "\n\nво временную папку\n" + fname_res +
                                 "\n\nПроверьте, не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                 QMessageBox::Retry|QMessageBox::Abort,
                                 QMessageBox::Retry)==QMessageBox::Abort) {
            this->setCursor(Qt::ArrowCursor);
            return false;
        }
        QFile::remove(fname_res);
    }
    if (!QFile::copy(fname_template, fname_res) || !QFile::exists(fname_res)) {
        QMessageBox::warning(this, "Ошибка копирования журнала", "Не удалось скопировать журнал СМС: \n" + fname_template + "\n\nво временную папку\n" + fname_res + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return false;
    }
    com = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname_res + "\" \"" + temp_folder + "/content.xml\"";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка обновления контента",
                             "При добавлении нового контента в журнал СМС произошла непредвиденная ошибка\n\nОпреация прервана.");
    }

    // ===========================
    // собственно открытие шаблона
    // ===========================
    // открытие полученного ODT
    long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);

    return true;
}

void send_pol_sms_wnd::on_ch_filter_clicked() {
    ui->pan_filter->setVisible(ui->ch_filter->isChecked());
    ui->tab_phones->clearSelection();

    if (ui->ch_filter->isChecked()) {
        ui->lab_cnt->setText("0");
    } else {
        ui->lab_cnt->setText(QString::number(model_phones.rowCount()));
    }
}

void send_pol_sms_wnd::on_bn_filter_all_clicked() {
    ui->tab_phones->selectAll();
}

void send_pol_sms_wnd::on_bn_filter_none_clicked() {
    ui->tab_phones->clearSelection();
}

void send_pol_sms_wnd::on_tab_phones_clicked(const QModelIndex &index) {
    if (!ui->ch_filter->isChecked()) {
        ui->tab_phones->clearSelection();
        ui->lab_cnt->setText(QString::number(model_phones.rowCount()));
    } else {
        ui->lab_cnt->setText(QString::number(ui->tab_phones->selectionModel()->selectedIndexes().count()/10));
    }
}

void send_pol_sms_wnd::on_date_vs4from_dateChanged(const QDate &date) {
    if ( ui->date_vs4from->date()>QDate::currentDate() ) {
        ui->date_vs4from->setDate(QDate::currentDate());
    }
    if ( ui->date_vs4from->date()>ui->date_vs4to->date() ||
         !ui->ch_vs4to->isChecked() ) {
        ui->date_vs4to->setDate(ui->date_vs4from->date());
    }
    int days = ui->date_vs4from->date().daysTo(ui->date_vs4to->date()) +1;
    ui->lab_n_vs4date->setText(" " + QString::number(days) + " дней");

    //on_bn_gen_list_sms_clicked();
    ui->frame->setEnabled(false);
    ui->bn_sms_reester->setEnabled(false);
    ui->bn_addr_reester->setEnabled(false);
    ui->bn_addr_reester_2->setEnabled(false);
    ui->bn_save_to_csv->setEnabled(false);
}

void send_pol_sms_wnd::on_date_vs4to_dateChanged(const QDate &date) {
    if ( ui->date_vs4to->date()>QDate::currentDate() ) {
        ui->date_vs4to->setDate(QDate::currentDate());
    }
    if ( ui->date_vs4from->date()>ui->date_vs4to->date() ||
         !ui->ch_vs4to->isChecked() ) {
        ui->date_vs4to->setDate(ui->date_vs4from->date());
    }
    int days = ui->date_vs4from->date().daysTo(ui->date_vs4to->date()) +1;
    ui->lab_n_vs4date->setText(" " + QString::number(days) + " дней");

    //on_bn_gen_list_sms_clicked();
    ui->frame->setEnabled(false);
    ui->bn_sms_reester->setEnabled(false);
    ui->bn_addr_reester->setEnabled(false);
    ui->bn_addr_reester_2->setEnabled(false);
    ui->bn_save_to_csv->setEnabled(false);
}

void send_pol_sms_wnd::on_ch_vs4to_clicked(bool checked) {
    ui->date_vs4to->setEnabled(checked);
    ui->date_vs4from->setDate(QDate::currentDate().addMonths(-1));
    ui->date_vs4to->setDate(QDate::currentDate());
    if (!checked) {
        ui->date_vs4to->setDate(ui->date_vs4from->date());
    }
    //on_bn_gen_list_sms_clicked();
    ui->frame->setEnabled(false);
    ui->bn_sms_reester->setEnabled(false);
    ui->bn_addr_reester->setEnabled(false);
    ui->bn_addr_reester_2->setEnabled(false);
    ui->bn_save_to_csv->setEnabled(false);
}

void send_pol_sms_wnd::on_spin_vs4days_editingFinished() {
    //on_bn_gen_list_sms_clicked();
    ui->lab_vs4days->setText("будут выбраны все застрахованные, чьи ВС \n истекут через " + QString::number(ui->spin_vs4days->value()) + " дней и бланк полиса готов");
    ui->frame->setEnabled(false);
    ui->bn_sms_reester->setEnabled(false);
    ui->bn_addr_reester->setEnabled(false);
    ui->bn_addr_reester_2->setEnabled(false);
    ui->bn_save_to_csv->setEnabled(false);
}

void send_pol_sms_wnd::on_spin_vs4days_valueChanged(int arg1) {
    on_spin_vs4days_editingFinished();
}

void send_pol_sms_wnd::on_spin_vs4exp_editingFinished() {
    //on_bn_gen_list_sms_clicked();
    ui->lab_vs4exp->setText("будут выбраны все застрахованные чьи ВС \n уже просрочены на " + QString::number(ui->spin_vs4exp->value()) + " дней и бланк полиса готов");
    ui->frame->setEnabled(false);
    ui->bn_sms_reester->setEnabled(false);
    ui->bn_addr_reester->setEnabled(false);
    ui->bn_addr_reester_2->setEnabled(false);
    ui->bn_save_to_csv->setEnabled(false);
}

void send_pol_sms_wnd::on_spin_vs4exp_valueChanged(int arg1) {
    on_spin_vs4exp_editingFinished();
}

void send_pol_sms_wnd::on_bn_toCSV_clicked() {
    QString fname_s = QFileDialog::getSaveFileName(this,
                                                   "Куда сохранить файл?",
                                                   QString(),
                                                   "файлы данных (*.csv);;простой текст (*.txt)");
    if (fname_s.isEmpty()) {
        QMessageBox::warning(this,
                             "Не выбрано имя файла",
                             "Имя цедевого файла не задано.\n\n"
                             "Операция отменена.");
        return;
    }
    if (QFile::exists(fname_s)) {
        if (QMessageBox::question(this,
                                  "Удалить старый файл?",
                                  "Файл уже существует.\n"
                                  "Удалить старый файл для того чтобы сохранить новый?",
                                  QMessageBox::Yes|QMessageBox::Cancel)==QMessageBox::Yes) {
            if (!QFile::remove(fname_s)) {
                QMessageBox::warning(this,
                                     "Ошибка при удалении старого файла",
                                     "При удалении старого файла произошла непредвиденная ошибка.\n\n"
                                     "Операция отменена.");
                return;
            }
        }
    }


    // сохраним данные в файл
    QFile file;
    file.setFileName(fname_s);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this,
                             "Ошибка создания файла",
                             "При создании целевого файла произошла ошибка.\n\n"
                             "Операция отменена.");
        return;
    }

    // подготовим поток для сохранения файла
    QTextStream stream(&file);
    QString codec_name = "Windows-1251";
    QTextCodec *codec = QTextCodec::codecForName(codec_name.toLocal8Bit());
    stream.setCodec(codec);


    QString head;
    int cnt_head = model_phones.columnCount();
    for (int i=0; i<cnt_head; i++) {
        if (i==1 || ui->tab_phones->columnWidth(i)>1) {
            head += model_phones.headerData(i,Qt::Horizontal).toString().replace("\n"," ");
            if (i<cnt_head-1)  head += ";";
        }
    }
    stream << head << "\n";

    // переберём полученные данные и сохраним в файл
    QSqlQuery *query_export = new QSqlQuery(db);
    query_export->exec(sql_phones );
    while (query_export->next()) {
        for (int j=0; j<cnt_head; j++) {
            if (j==1 || ui->tab_phones->columnWidth(j)>1) {
                QString value = query_export->value(j).toString().trimmed().replace(";",",");
                if (!value.isEmpty()) {
                    if (j==51)  stream << "'";
                    stream << value;
                    if (j==51)  stream << "'";
                }
                if (j<cnt_head-1)  stream << ";";
            }
        }
        stream << "\n";
    }
    file.close();

    // ===========================
    // собственно открытие шаблона
    // ===========================
    // открытие полученного ODT
    /*long result = (long)*/ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_s.utf16()), 0, 0, SW_NORMAL);
}

void send_pol_sms_wnd::on_bn_sms_reester_clicked() {
    QString selFilter="Данные, разделённые запятыми (*.csv);;Текстовые файлы (*.txt)";
    QString freport = QFileDialog::getSaveFileName (this, "Введите имя CSV-файла для сохранения списка телефонов",
                                                   (data_app.path_out + "_REPORTS_/" + QDate::currentDate().toString("yyyy-MM-dd") + " - " + data_app.filial_name + " - sms_report - " + QTime::currentTime().toString("hh.mm.ss") + ".csv"),
                                                   "Данные, разделённые запятыми (*.csv)",
                                                   &selFilter);
    if (!freport.isEmpty()) {
        db.transaction();

        QFile frep(freport);
        frep.open(QIODevice::WriteOnly);
        frep.write(QString("Журнал СМС\n").toLocal8Bit());
        QString date_s = "дата: " + QDate::currentDate().toString("yyyy-MM-dd") + ",  ";
        frep.write(date_s.toLocal8Bit());
        QString time_s = "время: " + QTime::currentTime().toString("hh:mm:ss") + "\n\n";
        frep.write(time_s.toLocal8Bit());
        frep.write(QString("ФИО (Д/Р);ТЕЛЕФОН;НОМЕР ВС;ДАТА ВС;СЕРИЯ ПОЛИСА;НОМЕР ПОЛИСА;ДАТА ПОЛУЧЕНИЯ;ДАТА ПОСЛЕДНЕГО СМС\n").toLocal8Bit());

        for (int i=0; i<phones.size(); i++) {
            QString phone = phones.at(i).phone;
            if (phone.left(2)=="89" && phone.length()==11) {
                phone = "7" + phone.right(10);
            }

            // запишем строку в журнале
            QString line = phones.at(i).fio + ", " + phones.at(i).date_r.toString("dd.MM.yyyy") + ";";
            line += phones.at(i).phone   + ";";
            line += phones.at(i).vs_num  + ";";
            line += phones.at(i).vs_date.toString("dd.MM.yyyy") + ";";
            line += phones.at(i).pol_ser  + ";";
            line += phones.at(i).pol_num  + ";";
            line += phones.at(i).date_add.toString("dd.MM.yyyy") + ";";
            line += phones.at(i).date_sms.toString("dd.MM.yyyy");
            frep.write(line.toLocal8Bit());
            frep.write("\n");
        }
        frep.close();


        long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(freport.utf16()), 0, 0, SW_NORMAL);
        if (result<32) {
            QMessageBox::warning(this,
                                 "Файл не найден",
                                 "Файл не найден:\n\n" + freport +
                                 "\nКод ошибки: " + QString::number(result) +
                                 "\n\nПроверьте, был ли сохранён целевой файл и установлено ли в системе средство для его просмотра.");
        }
        //close();

        //------------------------------------------------------------
        // генерация реестра СМС
        //------------------------------------------------------------
        /*bool res = print_registry_SMS();
        if (!res) {
            this->setCursor(Qt::ArrowCursor);
            db.rollback();
            return;
        }*/
        db.commit();
    } else {
        QMessageBox::warning(this, "Нечего сохранять", "Сначала выберите номера телефонов, которые хотите выгрузить для отправки СМС.\n\nОперация отменена.");
    }
}

void send_pol_sms_wnd::on_bn_addr_reester_clicked() {
    bool res = print_registry_addresses();
}

void send_pol_sms_wnd::on_bn_addr_reester_2_clicked() {
    QString selFilter="Данные, разделённые запятыми (*.csv);;Текстовые файлы (*.txt)";
    QString freport = QFileDialog::getSaveFileName (this, "Введите имя CSV-файла для сохранения журнала обхода",
                                                   (data_app.path_out + "_REPORTS_/" + QDate::currentDate().toString("yyyy-MM-dd") + " - " + data_app.filial_name + " - visiting_reester.csv"),
                                                   "Данные, разделённые запятыми (*.csv)",
                                                   &selFilter);
    if (!freport.isEmpty()) {
        db.transaction();

        QFile frep(freport);
        frep.open(QIODevice::WriteOnly);
        frep.write(QString("Журнал обхода\n").toLocal8Bit());
        QString date_s = "дата: " + QDate::currentDate().toString("yyyy-MM-dd") + ",  ";
        frep.write(date_s.toLocal8Bit());
        QString time_s = "время: " + QTime::currentTime().toString("hh:mm:ss") + "\n\n";
        frep.write(time_s.toLocal8Bit());
        frep.write(QString("ФИО, Д/Р;НОМЕР ВС;ДАТА ВС;ТЕЛЕФОН;РЕГИОН;РАЙОН;ГОРОД;НАС.ПУНКТ;УЛИЦА;ДОМ;КВАРТИРА;РЕЗУЛЬТАТ ЗВОНКА;ДАТА, РЕЗУЛЬТАТ ПОСЕЩЕНИЯ;ПРИЧИНЫ НЕВОЗМОЖНОСТИ ВРУЧЕНИЯ ПИСЬМЕННОГО ПРИГЛАШЕНИЯДЛЯ ВРУЧЕНИЯ ПОЛИСА ОМС\n").toLocal8Bit());

        for (int i=0; i<phones.size(); i++) {
            QString phone = phones.at(i).phone;
            if (phone.left(2)=="89" && phone.length()==11) {
                phone = "7" + phone.right(10);
            }
            QString adr_str = phones.at(i).address;
            adr_str = adr_str.replace(",", ";");

            // запишем строку в журнале
            QString line = phones.at(i).fio + ", " + phones.at(i).date_r.toString("dd.MM.yyyy") + ";";
            line += phones.at(i).vs_num   + ";";
            line += phones.at(i).vs_date.toString("dd.MM.yyyy") + ";";
            line += phones.at(i).phone    + ";";
            line += adr_str  + ";";
            line += phones.at(i).result  + ";";
            line += ";";
            line += ";";
            frep.write(line.toLocal8Bit());
            frep.write("\n");
        }
        frep.close();

        long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(freport.utf16()), 0, 0, SW_NORMAL);
        if (result<32) {
            QMessageBox::warning(this,
                                 "Файл не найден",
                                 "Файл не найден:\n\n" + freport +
                                 "\nКод ошибки: " + QString::number(result) +
                                 "\n\nПроверьте, был ли сохранён целевой файл и установлено ли в системе средство для его просмотра.");
        }
        //close();

        db.commit();
    } else {
        QMessageBox::warning(this, "Нечего сохранять", "Сначала выберите номера телефонов, которые хотите выгрузить для отправки СМС.\n\nОперация отменена.");
    }
}

void send_pol_sms_wnd::on_bn_sms_reester_2_clicked() {
    db.transaction();

    phones.clear();

    if (ui->ch_filter->isChecked()) {
        // добавим только выделенные
        QModelIndexList list = ui->tab_phones->selectionModel()->selectedIndexes();

        for (int i=0; i<list.count(); i++) {
            QModelIndex index = list.at(i);
            if (index.column()!=0)
                continue;

            s_data_phone rec;
            rec.phone    = model_phones.data(model_phones.index(index.row(), 0), Qt::EditRole).toString();
            rec.id_person= model_phones.data(model_phones.index(index.row(), 1), Qt::EditRole).toInt();
            rec.fio      = model_phones.data(model_phones.index(index.row(), 2), Qt::EditRole).toString();
            rec.date_r   = model_phones.data(model_phones.index(index.row(), 3), Qt::EditRole).toDate();
            rec.id_vs    = model_phones.data(model_phones.index(index.row(), 4), Qt::EditRole).toInt();
            rec.vs_num   = model_phones.data(model_phones.index(index.row(), 5), Qt::EditRole).toString();
            rec.vs_date  = model_phones.data(model_phones.index(index.row(), 6), Qt::EditRole).toDate();
            rec.pol_ser  = model_phones.data(model_phones.index(index.row(), 7), Qt::EditRole).toString();
            rec.pol_num  = model_phones.data(model_phones.index(index.row(), 8), Qt::EditRole).toString();
            rec.date_add = model_phones.data(model_phones.index(index.row(), 9), Qt::EditRole).toDate();
            rec.date_sms = model_phones.data(model_phones.index(index.row(),10), Qt::EditRole).toDate();
            rec.result   = model_phones.data(model_phones.index(index.row(),11), Qt::EditRole).toString();
            rec.address  = model_phones.data(model_phones.index(index.row(),12), Qt::EditRole).toString();

            phones.append(rec);
        }
    } else {
        // добавлим все телефоны из таблицы

        for (int i=0; i<model_phones.rowCount(); i++) {
            s_data_phone rec;
            rec.phone    = model_phones.data(model_phones.index(i, 0), Qt::EditRole).toString();
            rec.id_person= model_phones.data(model_phones.index(i, 1), Qt::EditRole).toInt();
            rec.fio      = model_phones.data(model_phones.index(i, 2), Qt::EditRole).toString();
            rec.date_r   = model_phones.data(model_phones.index(i, 3), Qt::EditRole).toDate();
            rec.id_vs    = model_phones.data(model_phones.index(i, 4), Qt::EditRole).toInt();
            rec.vs_num   = model_phones.data(model_phones.index(i, 5), Qt::EditRole).toString();
            rec.vs_date  = model_phones.data(model_phones.index(i, 6), Qt::EditRole).toDate();
            rec.pol_ser  = model_phones.data(model_phones.index(i, 7), Qt::EditRole).toString();
            rec.pol_num  = model_phones.data(model_phones.index(i, 8), Qt::EditRole).toString();
            rec.date_add = model_phones.data(model_phones.index(i, 9), Qt::EditRole).toDate();
            rec.date_sms = model_phones.data(model_phones.index(i,10), Qt::EditRole).toDate();
            rec.result   = model_phones.data(model_phones.index(i,11), Qt::EditRole).toString();
            rec.address  = model_phones.data(model_phones.index(i,12), Qt::EditRole).toString();

            phones.append(rec);
        }
    }

    //------------------------------------------------------------
    // генерация реестра СМС
    //------------------------------------------------------------
    bool res = print_registry_SMS();
    if (!res) {
        this->setCursor(Qt::ArrowCursor);
        db.rollback();
        return;
    }
    db.commit();
}





void send_pol_sms_wnd::on_rb_new_clicked(bool checked) {
    ui->bn_gen_list_sms->setEnabled(checked);

    ui->rb_new->setChecked(checked);
    //-----------------------------------//
    ui->pan_new->setEnabled(checked);
    ui->ch_no_sms->setChecked(checked);
    ui->ch_no_activate->setChecked(checked);;
    ui->ch_no_get2hand->setChecked(checked);

    ui->rb_back->setChecked(false);
    //-----------------------------------//
    ui->pan_back->setEnabled(false);
    ui->rb_vs4date->setChecked(false);
    ui->rb_vs4exp->setChecked(false);
    ui->rb_vs4days->setChecked(false);
    ui->pan_vs4date->setEnabled(false);
    ui->pan_vs4exp->setEnabled(false);
    ui->pan_vs4days->setEnabled(false);

    ui->rb_oldUDLs->setChecked(false);
    ui->rb_oldENPs->setChecked(false);
    ui->rb_oldDRPs->setChecked(false);
    //-----------------------------------//
    ui->pan_olddocs->setEnabled(false);
    ui->rb_doc4days->setChecked(false);
    ui->rb_doc4exp->setChecked(false);
    ui->pan_doc4days->setEnabled(false);
    ui->pan_doc4exp->setEnabled(false);

    ui->rb_oldRFpassport->setChecked(false);

    //-----------------------------------//
    ui->bn_sms_reester->setEnabled(false);
    ui->bn_addr_reester->setVisible(false);
    ui->bn_addr_reester->setEnabled(false);
    ui->bn_addr_reester_2->setVisible(false);
    ui->bn_addr_reester_2->setEnabled(false);
    ui->bn_save_to_csv->setEnabled(false);
    ui->frame_3->setVisible(false);
    ui->frame->setEnabled(false);
}

void send_pol_sms_wnd::on_rb_back_clicked(bool checked) {
    if (checked) {
        QString s = QInputDialog::getText(this,"Нужно подтверждение",QString("Введите пароль для получения выгрузки задним числом\n") +
                                          (data_app.is_head||data_app.is_tech ? "ТЕЛЕФОНЫ" : "***")).toUpper();
        if (s!="ТЕЛЕФОНЫ" && s!="NTKTAJYS") {
            ui->bn_gen_list_sms->setEnabled(false);

            ui->rb_new->setChecked(false);
            //-----------------------------------//
            ui->pan_new->setEnabled(false);
            ui->ch_no_sms->setChecked(false);
            ui->ch_no_activate->setChecked(false);;
            ui->ch_no_get2hand->setChecked(false);

            ui->rb_back->setChecked(false);
            //-----------------------------------//
            ui->pan_back->setEnabled(false);
            ui->rb_vs4date->setChecked(false);
            ui->rb_vs4exp->setChecked(false);
            ui->rb_vs4days->setChecked(false);
            ui->pan_vs4date->setEnabled(false);
            ui->pan_vs4exp->setEnabled(false);
            ui->pan_vs4days->setEnabled(false);

            ui->rb_oldUDLs->setChecked(false);
            ui->rb_oldENPs->setChecked(false);
            ui->rb_oldDRPs->setChecked(false);
            //-----------------------------------//
            ui->pan_olddocs->setEnabled(false);
            ui->rb_doc4days->setChecked(false);
            ui->rb_doc4exp->setChecked(false);
            ui->pan_doc4days->setEnabled(false);
            ui->pan_doc4exp->setEnabled(false);

            ui->rb_oldRFpassport->setChecked(false);

            //-----------------------------------//
            ui->bn_sms_reester->setEnabled(false);
            ui->bn_addr_reester->setVisible(false);
            ui->bn_addr_reester->setEnabled(false);
            ui->bn_addr_reester_2->setVisible(false);
            ui->bn_addr_reester_2->setEnabled(false);
            ui->bn_save_to_csv->setEnabled(false);
            ui->frame_3->setVisible(false);
            ui->frame->setEnabled(false);
            return;
        } else {
            ui->bn_gen_list_sms->setEnabled(true);

            ui->rb_new->setChecked(false);
            //-----------------------------------//
            ui->pan_new->setEnabled(false);
            ui->ch_no_sms->setChecked(false);
            ui->ch_no_activate->setChecked(false);;
            ui->ch_no_get2hand->setChecked(false);

            ui->rb_back->setChecked(true);
            //-----------------------------------//
            ui->pan_back->setEnabled(true);
            ui->rb_vs4days->setChecked(true);
            ui->rb_vs4exp->setChecked(false);
            ui->rb_vs4date->setChecked(false);
            ui->pan_vs4days->setEnabled(true);
            ui->pan_vs4exp->setEnabled(false);
            ui->pan_vs4date->setEnabled(false);

            ui->rb_oldUDLs->setChecked(false);
            ui->rb_oldENPs->setChecked(false);
            ui->rb_oldDRPs->setChecked(false);
            //-----------------------------------//
            ui->pan_olddocs->setEnabled(false);
            ui->rb_doc4days->setChecked(false);
            ui->rb_doc4exp->setChecked(false);
            ui->pan_doc4days->setEnabled(false);
            ui->pan_doc4exp->setEnabled(false);

            ui->rb_oldRFpassport->setChecked(false);

            //-----------------------------------//
            ui->bn_sms_reester->setEnabled(false);
            ui->bn_addr_reester->setVisible(false);
            ui->bn_addr_reester->setEnabled(false);
            ui->bn_addr_reester_2->setVisible(false);
            ui->bn_addr_reester_2->setEnabled(false);
            ui->bn_save_to_csv->setEnabled(false);
            ui->frame_3->setVisible(false);
            ui->frame->setEnabled(false);
            return;
        }
    }
    ui->bn_gen_list_sms->setEnabled(false);

    ui->rb_new->setChecked(false);
    //-----------------------------------//
    ui->pan_new->setEnabled(false);
    ui->ch_no_sms->setChecked(false);
    ui->ch_no_activate->setChecked(false);;
    ui->ch_no_get2hand->setChecked(false);

    ui->rb_back->setChecked(false);
    //-----------------------------------//
    ui->pan_back->setEnabled(false);
    ui->rb_vs4date->setChecked(false);
    ui->rb_vs4exp->setChecked(false);
    ui->rb_vs4days->setChecked(false);
    ui->pan_vs4date->setEnabled(false);
    ui->pan_vs4exp->setEnabled(false);
    ui->pan_vs4days->setEnabled(false);

    ui->rb_oldUDLs->setChecked(false);
    ui->rb_oldENPs->setChecked(false);
    ui->rb_oldDRPs->setChecked(false);
    //-----------------------------------//
    ui->pan_olddocs->setEnabled(false);
    ui->rb_doc4days->setChecked(false);
    ui->rb_doc4exp->setChecked(false);
    ui->pan_doc4days->setEnabled(false);
    ui->pan_doc4exp->setEnabled(false);

    ui->rb_oldRFpassport->setChecked(false);

    //-----------------------------------//
    ui->bn_sms_reester->setEnabled(false);
    ui->bn_addr_reester->setVisible(false);
    ui->bn_addr_reester->setEnabled(false);
    ui->bn_addr_reester_2->setVisible(false);
    ui->bn_addr_reester_2->setEnabled(false);
    ui->bn_save_to_csv->setEnabled(false);
    ui->frame_3->setVisible(false);
    ui->frame->setEnabled(false);
}

void send_pol_sms_wnd::on_rb_oldUDLs_clicked(bool checked) {
    ui->bn_gen_list_sms->setEnabled(checked);

    ui->rb_new->setChecked(false);
    //-----------------------------------//
    ui->pan_new->setEnabled(false);
    ui->ch_no_sms->setChecked(false);
    ui->ch_no_activate->setChecked(false);;
    ui->ch_no_get2hand->setChecked(false);

    ui->rb_back->setChecked(false);
    //-----------------------------------//
    ui->pan_back->setEnabled(false);
    ui->rb_vs4date->setChecked(false);
    ui->rb_vs4exp->setChecked(false);
    ui->rb_vs4days->setChecked(false);
    ui->pan_vs4date->setEnabled(false);
    ui->pan_vs4exp->setEnabled(false);
    ui->pan_vs4days->setEnabled(false);

    ui->rb_oldUDLs->setChecked(checked);
    ui->rb_oldDRPs->setChecked(false);
    ui->rb_oldENPs->setChecked(false);
    //-----------------------------------//
    ui->pan_olddocs->setEnabled(checked);
    ui->rb_doc4days->setChecked(checked);
    ui->rb_doc4exp->setChecked(false);
    ui->pan_doc4days->setEnabled(checked);
    ui->pan_doc4exp->setEnabled(false);

    ui->rb_oldRFpassport->setChecked(false);

    //-----------------------------------//
    ui->bn_sms_reester->setEnabled(false);
    ui->bn_addr_reester->setVisible(false);
    ui->bn_addr_reester->setEnabled(false);
    ui->bn_addr_reester_2->setVisible(false);
    ui->bn_addr_reester_2->setEnabled(false);
    ui->bn_save_to_csv->setEnabled(false);
    ui->frame_3->setVisible(false);
    ui->frame->setEnabled(false);
}

void send_pol_sms_wnd::on_rb_oldDRPs_clicked(bool checked) {
    ui->bn_gen_list_sms->setEnabled(checked);

    ui->rb_new->setChecked(false);
    //-----------------------------------//
    ui->pan_new->setEnabled(false);
    ui->ch_no_sms->setChecked(false);
    ui->ch_no_activate->setChecked(false);;
    ui->ch_no_get2hand->setChecked(false);

    ui->rb_back->setChecked(false);
    //-----------------------------------//
    ui->pan_back->setEnabled(false);
    ui->rb_vs4date->setChecked(false);
    ui->rb_vs4exp->setChecked(false);
    ui->rb_vs4days->setChecked(false);
    ui->pan_vs4date->setEnabled(false);
    ui->pan_vs4exp->setEnabled(false);
    ui->pan_vs4days->setEnabled(false);

    ui->rb_oldUDLs->setChecked(false);
    ui->rb_oldDRPs->setChecked(checked);
    ui->rb_oldENPs->setChecked(false);
    //-----------------------------------//
    ui->pan_olddocs->setEnabled(checked);
    ui->rb_doc4days->setChecked(checked);
    ui->rb_doc4exp->setChecked(false);
    ui->pan_doc4days->setEnabled(checked);
    ui->pan_doc4exp->setEnabled(false);

    ui->rb_oldRFpassport->setChecked(false);

    //-----------------------------------//
    ui->bn_sms_reester->setEnabled(false);
    ui->bn_addr_reester->setVisible(false);
    ui->bn_addr_reester->setEnabled(false);
    ui->bn_addr_reester_2->setVisible(false);
    ui->bn_addr_reester_2->setEnabled(false);
    ui->bn_save_to_csv->setEnabled(false);
    ui->frame_3->setVisible(false);
    ui->frame->setEnabled(false);
}

void send_pol_sms_wnd::on_rb_oldENPs_clicked(bool checked) {
    ui->bn_gen_list_sms->setEnabled(checked);

    ui->rb_new->setChecked(false);
    //-----------------------------------//
    ui->pan_new->setEnabled(false);
    ui->ch_no_sms->setChecked(false);
    ui->ch_no_activate->setChecked(false);;
    ui->ch_no_get2hand->setChecked(false);

    ui->rb_back->setChecked(false);
    //-----------------------------------//
    ui->pan_back->setEnabled(false);
    ui->rb_vs4date->setChecked(false);
    ui->rb_vs4exp->setChecked(false);
    ui->rb_vs4days->setChecked(false);
    ui->pan_vs4date->setEnabled(false);
    ui->pan_vs4exp->setEnabled(false);
    ui->pan_vs4days->setEnabled(false);

    ui->rb_oldUDLs->setChecked(false);
    ui->rb_oldDRPs->setChecked(false);
    ui->rb_oldENPs->setChecked(checked);
    //-----------------------------------//
    ui->pan_olddocs->setEnabled(checked);
    ui->rb_doc4days->setChecked(checked);
    ui->rb_doc4exp->setChecked(false);
    ui->pan_doc4days->setEnabled(checked);
    ui->pan_doc4exp->setEnabled(false);

    ui->rb_oldRFpassport->setChecked(false);

    //-----------------------------------//
    ui->bn_sms_reester->setEnabled(false);
    ui->bn_addr_reester->setVisible(false);
    ui->bn_addr_reester->setEnabled(false);
    ui->bn_addr_reester_2->setVisible(false);
    ui->bn_addr_reester_2->setEnabled(false);
    ui->bn_save_to_csv->setEnabled(false);
    ui->frame_3->setVisible(false);
    ui->frame->setEnabled(false);
}

void send_pol_sms_wnd::on_rb_oldRFpassport_clicked(bool checked) {
    ui->bn_gen_list_sms->setEnabled(checked);

    ui->rb_new->setChecked(false);
    //-----------------------------------//
    ui->pan_new->setEnabled(false);
    ui->ch_no_sms->setChecked(false);
    ui->ch_no_activate->setChecked(false);;
    ui->ch_no_get2hand->setChecked(false);

    ui->rb_back->setChecked(false);
    //-----------------------------------//
    ui->pan_back->setEnabled(false);
    ui->rb_vs4date->setChecked(false);
    ui->rb_vs4exp->setChecked(false);
    ui->rb_vs4days->setChecked(false);
    ui->pan_vs4date->setEnabled(false);
    ui->pan_vs4exp->setEnabled(false);
    ui->pan_vs4days->setEnabled(false);

    ui->rb_oldUDLs->setChecked(false);
    ui->rb_oldDRPs->setChecked(false);
    ui->rb_oldENPs->setChecked(false);
    //-----------------------------------//
    ui->pan_olddocs->setEnabled(false);
    ui->rb_doc4days->setChecked(false);
    ui->rb_doc4exp->setChecked(false);
    ui->pan_doc4days->setEnabled(false);
    ui->pan_doc4exp->setEnabled(false);

    ui->rb_oldRFpassport->setChecked(checked);

    //-----------------------------------//
    ui->bn_sms_reester->setEnabled(false);
    ui->bn_addr_reester->setVisible(false);
    ui->bn_addr_reester->setEnabled(false);
    ui->bn_addr_reester_2->setVisible(false);
    ui->bn_addr_reester_2->setEnabled(false);
    ui->bn_save_to_csv->setEnabled(false);
    ui->frame_3->setVisible(false);
    ui->frame->setEnabled(false);
}



void send_pol_sms_wnd::on_rb_vs4days_clicked(bool checked) {
    ui->pan_vs4days->setEnabled(checked);
    ui->pan_vs4date->setEnabled(!checked);
    ui->pan_vs4exp->setEnabled(!checked);
    //------------------------------//
    ui->frame->setEnabled(false);
    ui->bn_sms_reester->setEnabled(false);
    ui->bn_addr_reester->setEnabled(false);
    ui->bn_addr_reester_2->setEnabled(false);
    ui->bn_save_to_csv->setEnabled(false);
}
void send_pol_sms_wnd::on_rb_vs4date_clicked(bool checked) {
    ui->pan_vs4days->setEnabled(!checked);
    ui->pan_vs4date->setEnabled(checked);
    ui->pan_vs4exp->setEnabled(!checked);
    //------------------------------//
    ui->frame->setEnabled(false);
    ui->bn_sms_reester->setEnabled(false);
    ui->bn_addr_reester->setEnabled(false);
    ui->bn_addr_reester_2->setEnabled(false);
    ui->bn_save_to_csv->setEnabled(false);
}
void send_pol_sms_wnd::on_rb_vs4exp_clicked(bool checked) {
    ui->pan_vs4days->setEnabled(!checked);
    ui->pan_vs4date->setEnabled(!checked);
    ui->pan_vs4exp->setEnabled(checked);
    //------------------------------//
    ui->frame->setEnabled(false);
    ui->bn_sms_reester->setEnabled(false);
    ui->bn_addr_reester->setEnabled(false);
    ui->bn_addr_reester_2->setEnabled(false);
    ui->bn_save_to_csv->setEnabled(false);
}



void send_pol_sms_wnd::on_rb_doc4days_clicked(bool checked) {
    ui->pan_doc4days->setEnabled(checked);
    ui->pan_doc4exp->setEnabled(!checked);
    //------------------------------//
    ui->frame->setEnabled(false);
    ui->bn_sms_reester->setEnabled(false);
    ui->bn_addr_reester->setEnabled(false);
    ui->bn_addr_reester_2->setEnabled(false);
    ui->bn_save_to_csv->setEnabled(false);
}
void send_pol_sms_wnd::on_rb_doc4exp_clicked(bool checked) {
    ui->pan_doc4days->setEnabled(!checked);
    ui->pan_doc4exp->setEnabled(checked);
    //------------------------------//
    ui->frame->setEnabled(false);
    ui->bn_sms_reester->setEnabled(false);
    ui->bn_addr_reester->setEnabled(false);
    ui->bn_addr_reester_2->setEnabled(false);
    ui->bn_save_to_csv->setEnabled(false);
}

void send_pol_sms_wnd::on_spin_doc4days_valueChanged(int arg1) {
    ui->lab_doc4days->setText("будут выбраны все застрахованные, чьи \nдокументы истекут через " + QString::number(ui->spin_doc4days->value()) + " или менее дней");
}

void send_pol_sms_wnd::on_spin_doc4exp_valueChanged(int arg1) {
    ui->lab_doc4exp->setText("будут выбраны все застрахованные, чьи \n документы уже просрочены на срок до " + QString::number(ui->spin_doc4exp->value()) + " дней");
}

