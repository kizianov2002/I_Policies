#include "change_vizit_wnd.h"
#include "ui_change_vizit_wnd.h"

change_vizit_wnd::change_vizit_wnd(QSqlDatabase &db, s_data_vizit &data_vizit, s_data_event &data_event, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    db(db),
    data_vizit(data_vizit),
    data_event(data_event),
    data_app(data_app),
    settings(settings),
    QDialog(parent),
    ui(new Ui::change_vizit_wnd)
{
    ui->setupUi(this);

    refresh_event_codes();
    refresh_event_state();
    ui->date_event->setDate(data_event.event_dt.date());
    ui->combo_event_code->setCurrentIndex(0);
    ui->combo_event_state->setCurrentIndex(ui->combo_event_state->findData(data_event.event_status));
    //ui->ch_send_sms->setChecked(data_event.send_sms);

    refresh_vizit_method();
    refresh_reasons_smo();
    refresh_reasons_polis();
    refresh_form_polis();
    ui->date_vizit->setDate(data_vizit.date);

    refresh_combo_UDL();
    refresh_combo_DRP();

    refresh_viz_points();
    refresh_viz_operators(ui->combo_viz_point->currentData().toInt());
    ui->lab_act_point_name->setText(data_event.point_name);
    ui->lab_act_operator_fio->setText(data_event.operator_fio);
    ui->lab_viz_dt_ins->setText(data_vizit.dt_ins.toString("dd.MM.yyyy"));
    ui->lab_viz_dt_upd->setText(data_vizit.dt_upd.toString("dd.MM.yyyy"));
    ui->lab_act_dt_ins->setText(data_event.dt_ins.toString("dd.MM.yyyy"));
    ui->lab_act_dt_upd->setText(data_event.dt_upd.toString("dd.MM.yyyy"));
}

change_vizit_wnd::~change_vizit_wnd() {
    delete ui;
}

void change_vizit_wnd::on_bn_now_clicked() {
    ui->date_vizit->setDate(QDate::currentDate());
}

void change_vizit_wnd::refresh_viz_points() {
    this->setCursor(Qt::WaitCursor);

    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "";
    sql += QString("select p.id, '('||p.point_regnum||') '||p.point_name as name"
                   " from points p "
                   " where status>0 "
                   " order by p.point_regnum, id ; ");
    query->exec( sql );
    ui->combo_viz_point->clear();

    // заполнение списка пунктов выдачи
    while (query->next()) {
        ui->combo_viz_point->addItem( query->value(1).toString(),
                                      query->value(0).toInt() );
    }
    query->finish();
    delete query;

    ui->combo_viz_point->setCurrentIndex(ui->combo_viz_point->findData(data_vizit.id_point));

    this->setCursor(Qt::ArrowCursor);
}

void change_vizit_wnd::refresh_viz_operators(int id_point) {
    this->setCursor(Qt::WaitCursor);

    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "";
    sql += "select u.id, '('||p.point_regnum||')  '||u.oper_fio, u.oper_fio ";
    sql += "  from operators u "
           "  left join points p on u.id_point=p.id "
           " where u.status>0 "
           "   and p.id=" + QString::number(id_point) + " ";

    sql += " order by u.oper_fio ; ";
    query->exec( sql );
    ui->combo_viz_operator->clear();

    // заполнение списка операторов
    while (query->next()) {
        ui->combo_viz_operator->addItem( query->value(1).toString(),
                                         query->value(0).toInt() );
    }
    query->finish();
    delete query;

    // восстановим прежнее значение
    ui->combo_viz_operator->setCurrentIndex(ui->combo_viz_operator->findData(data_vizit.id_operator));

    this->setCursor(Qt::ArrowCursor);
}

void change_vizit_wnd::refresh_event_codes() {
    QStringList l;

    if ( data_event.event_code=="П010"||
         data_event.event_code=="П034"||
         data_event.event_code=="П035"||
         data_event.event_code=="П036"||
         data_event.event_code=="П061"||
         data_event.event_code=="П062"||
         data_event.event_code=="П063" ) {

        l.append(" - не менять - ");
        if ( data_event.event_code!="П010") l.append("П010");
        if ( data_event.event_code!="П034") l.append("П034");
        if ( data_event.event_code!="П035") l.append("П035");
        if ( data_event.event_code!="П036") l.append("П036");
        if ( data_event.event_code!="П061") l.append("П061");
        if ( data_event.event_code!="П062") l.append("П062");
        if ( data_event.event_code!="П063") l.append("П063");
    }
    else if( data_event.event_code=="П021"||
             data_event.event_code=="П022"||
             data_event.event_code=="П023"||
             data_event.event_code=="П024"||
             data_event.event_code=="П025" ) {

        l.append(" - не менять - ");
        if ( data_event.event_code!="П021") l.append("П021");
        if ( data_event.event_code!="П022") l.append("П022");
        if ( data_event.event_code!="П023") l.append("П023");
        if ( data_event.event_code!="П024") l.append("П024");
        if ( data_event.event_code!="П025") l.append("П025");
    }
    else if( data_event.event_code=="П031"||
             data_event.event_code=="П032"||
             data_event.event_code=="П033" ) {

        l.append(" - не менять - ");
        if ( data_event.event_code!="П031") l.append("П031");
        if ( data_event.event_code!="П032") l.append("П032");
        if ( data_event.event_code!="П033") l.append("П033");
    }
    else if( data_event.event_code=="П060" ) {

        l.append(" - не менять - ");
    }

    ui->combo_event_code->clear();
    ui->combo_event_code->addItems(l);
}

void change_vizit_wnd::refresh_event_state() {
    ui->combo_event_state->clear();
    ui->combo_event_state->addItem("(0) новое, в фонд не отправлялось", 0);
    //ui->combo_event_state->addItem("(1) новое, готово к отправке в фонд", 1);
    ui->combo_event_state->addItem("(2) уже отправлено в фонд", 2);
    ui->combo_event_state->addItem("(-1) никогда не отправлять", -1);
}

void change_vizit_wnd::refresh_vizit_method() {
    this->setCursor(Qt::WaitCursor);
    // обновление списка способов подачи заявления
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, '(' || code || ') ' || text "
                  "  from public.spr_method s "
                  " order by code ; ";
    mySQL.exec(this, sql, QString("Справочник способов подачи заявления"), *query, true, db, data_app);
    ui->combo_vizit_method->clear();
    while (query->next()) {
        ui->combo_vizit_method->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_vizit_method->setCurrentIndex(ui->combo_vizit_method->findData(data_vizit.v_method));
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void change_vizit_wnd::refresh_reasons_smo() {
    this->setCursor(Qt::WaitCursor);
    // обновление списка причин подачи заявления
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, '(' || code || ') ' || text "
                  "  from public.spr_rsmo s "
                  " order by code ; ";
    mySQL.exec(this, sql, QString("Справочник причин подачи заявления"), *query, true, db, data_app);
    ui->combo_reason_smo->clear();
    while (query->next()) {
        ui->combo_reason_smo->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_reason_smo->setCurrentIndex(ui->combo_reason_smo->findData(data_vizit.r_smo));
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void change_vizit_wnd::refresh_reasons_polis() {
    this->setCursor(Qt::WaitCursor);
    // обновление списка причин выдачи полиса
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, '(' || code || ') ' || text "
                  "  from public.spr_rpolis s "
                  " order by code ; ";
    mySQL.exec(this, sql, QString("Справочник причин выдачи полиса"), *query, true, db, data_app);
    ui->combo_reason_polis->clear();
    while (query->next()) {
        ui->combo_reason_polis->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_reason_polis->setCurrentIndex(ui->combo_reason_polis->findData(data_vizit.r_polis));
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void change_vizit_wnd::refresh_combo_UDL() {
    this->setCursor(Qt::WaitCursor);
    // обновление списка причин выдачи полиса
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select d.id, d.doc_type, s.text as doc_type, d.doc_ser, d.doc_num, d.doc_date, d.doc_exp "
                  "  from persons_docs d "
                  "  left join spr_f011 s on(s.code=d.doc_type) "
                  " where d.id_person=(select p.id_person "
                                      "  from events e "
                                      "  left join polises p on(p.id=e.id_polis) "
                                      " where e.id=" + QString::number(data_event.id) + ") "
                  "   and d.doc_type in(1,2,3,9,10,12,13,14,21,22,24,25,27,28) "
                  " order by d.doc_date DESC ; ";
    mySQL.exec(this, sql, QString("Список УДЛ щзастрахованного"), *query, true, db, data_app);
    ui->combo_UDL->clear();
    while (query->next()) {
        ui->combo_UDL->addItem(query->value(3).toString() + " " + query->value(4).toString() +
                               ", " + query->value(5).toDate().toString("dd.MM.yyyy") + "/" + query->value(6).toDate().toString("dd.MM.yyyy") +
                               " (" + query->value(2).toString() + ")",
                               query->value(0).toInt());
    }
    ui->combo_UDL->setCurrentIndex(ui->combo_UDL->findData(data_event.id_UDL));
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void change_vizit_wnd::refresh_combo_DRP() {
    this->setCursor(Qt::WaitCursor);
    // обновление списка причин выдачи полиса
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select d.id, d.doc_type, s.text as doc_type, d.doc_ser, d.doc_num, d.doc_date, d.doc_exp "
                  "  from persons_docs d "
                  "  left join spr_f011 s on(s.code=d.doc_type) "
                  " where d.id_person=(select p.id_person "
                                      "  from events e "
                                      "  left join polises p on(p.id=e.id_polis) "
                                      " where e.id=" + QString::number(data_event.id) + ") "
                  "   and d.doc_type in(11,23) "
                  " order by d.doc_date DESC ; ";
    mySQL.exec(this, sql, QString("Список ДРП щзастрахованного"), *query, true, db, data_app);
    ui->combo_DRP->clear();
    while (query->next()) {
        ui->combo_DRP->addItem(query->value(3).toString() + " " + query->value(4).toString() +
                               ", " + query->value(5).toDate().toString("dd.MM.yyyy") + "/" + query->value(6).toDate().toString("dd.MM.yyyy") +
                               " (" + query->value(2).toString() + ")",
                               query->value(0).toInt());
    }
    ui->combo_DRP->setCurrentIndex(ui->combo_DRP->findData(data_event.id_DRP));
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void change_vizit_wnd::refresh_form_polis() {
    this->setCursor(Qt::WaitCursor);
    // обновление списка форм изготовления полиса
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, '(' || code || ') ' || text "
                  "  from public.spr_fpolis s "
                  " order by code ; ";
    mySQL.exec(this, sql, QString("Справочник форм изготовления полиса"), *query, true, db, data_app);
    ui->combo_form_polis->clear();
    while (query->next()) {
        ui->combo_form_polis->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_form_polis->setCurrentIndex(ui->combo_form_polis->findData(data_vizit.f_polis));
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void change_vizit_wnd::on_bn_cansel_clicked() {
    reject();
}

void change_vizit_wnd::on_bn_save_clicked() {
    if (ui->combo_event_state->currentData().toInt()==0) {
        if (QMessageBox::question(this, "Отправить событие в фонд?",
                                  "Выбранный код состояния события означает, что запись события с первой же посылкой будет отправлена в фонд.\n\n"
                                  "Продолжить сохранение?",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }
    }
    if (ui->combo_event_code->currentIndex()>0) {
        data_event.event_code = ui->combo_event_code->currentText();
    }
    data_event.event_dt = QDateTime(ui->date_event->date());
    data_event.event_status = ui->combo_event_state->currentData().toInt();
    data_event.id_UDL = ui->combo_UDL->currentData().toInt();
    data_event.id_DRP = ui->combo_DRP->currentData().toInt();
    //data_event.send_sms = ui->ch_send_sms->isChecked();
    data_vizit.v_method = ui->combo_vizit_method->currentData().toInt();
    data_vizit.f_polis = ui->combo_form_polis->currentData().toInt();
    data_vizit.r_polis = ui->combo_reason_polis->currentData().toInt();
    data_vizit.r_smo = ui->combo_reason_smo->currentData().toInt();
    data_vizit.date = ui->date_vizit->date();
    data_vizit.has_petition = ui->ch_has_petition->isChecked();
    if (ui->ch_viz_oper->isChecked()) {
        data_vizit.id_point = ui->combo_viz_point->currentData().toInt();
        data_vizit.id_operator = ui->combo_viz_operator->currentData().toInt();
    }
    accept();
}

void change_vizit_wnd::on_ch_viz_oper_clicked(bool checked) {
    ui->combo_viz_point->setEnabled(checked);
    ui->combo_viz_operator->setEnabled(checked);
}

void change_vizit_wnd::on_combo_viz_point_activated(int index) {
    refresh_viz_operators(ui->combo_viz_point->currentData().toInt());
}

void change_vizit_wnd::on_combo_UDL_currentIndexChanged(const QString &arg1) {
    //ui->lab_UDL->setText(arg1);
}

void change_vizit_wnd::on_combo_DRP_currentIndexChanged(const QString &arg1) {
    //ui->lab_DRP->setText(arg1);
}
