#include "change_polis_wnd.h"
#include "ui_change_polis_wnd.h"

change_polis_wnd::change_polis_wnd(QSqlDatabase &db, s_data_polis &data_polis, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    db(db),
    data_polis(data_polis),
    data_app(data_app),
    settings(settings),
    QDialog(parent),
    ui(new Ui::change_polis_wnd)
{
    ui->setupUi(this);

    // обновим combo_in_erp
    ui->combo_pol_in_erp->clear();
    ui->combo_pol_in_erp->addItem("нет", 0);
    ui->combo_pol_in_erp->addItem("да", 1);
    ui->combo_pol_in_erp->addItem("отказ", -1);
    //ui->combo_pol_in_erp->addItem("закрыт вручную", -2);
    ui->combo_pol_in_erp->addItem("NULL", -999);

    refresh_form_polis();
    refresh_pol_ves();
    refresh_reasons_smo();
    refresh_reasons_polis();
    refresh_points();
    refresh_operators(-1);

    ui->combo_vizit_pan_form_polis->setCurrentIndex(ui->combo_vizit_pan_form_polis->findData(data_polis.f_polis));
    ui->combo_r_smo->setCurrentIndex(ui->combo_r_smo->findData(data_polis.r_smo));
    ui->combo_r_polis->setCurrentIndex(ui->combo_r_polis->findData(data_polis.r_polis));
    ui->combo_pol_v->setCurrentIndex(ui->combo_pol_v->findData(data_polis.pol_v));
    ui->combo_first_point->setCurrentIndex(ui->combo_first_point->findData(data_polis._id_first_point));
    ui->combo_first_operator->setCurrentIndex(ui->combo_first_operator->findData(data_polis._id_first_operator));
    ui->lab_last_point_name->setText(data_polis.last_point_name);
    ui->lab_last_operator_fio->setText(data_polis.last_operator_fio);
    ui->ch_r_smo->setEnabled(true);
    ui->ch_r_smo->setChecked(data_polis.r_smo>0);
    ui->ch_r_polis->setEnabled(true);
    ui->ch_r_polis->setChecked(data_polis.r_polis>0);
    switch (data_polis.pol_v) {
    case 1:
        ui->ch_vs_num->setEnabled(false);
        ui->ch_vs_num->setChecked(false);
        ui->ch_vs_num->setEnabled(false);
        ui->ch_vs_num->setChecked(false);
        ui->ch_pol_sernum->setEnabled(false);
        ui->ch_pol_sernum->setChecked(false);
        ui->ch_date_activate->setEnabled(false);
        ui->ch_date_activate->setChecked(false);
        ui->ch_date_get2hand->setEnabled(false);
        ui->ch_date_get2hand->setChecked(false);
        break;
    case 2:
        ui->ch_vs_num->setEnabled(true);
        ui->ch_vs_num->setChecked(true);
        ui->ch_pol_sernum->setEnabled(false);
        ui->ch_pol_sernum->setChecked(false);
        ui->ch_date_activate->setEnabled(false);
        ui->ch_date_activate->setChecked(false);
        ui->ch_date_get2hand->setEnabled(false);
        ui->ch_date_get2hand->setChecked(false);
        break;
    case 3:
        ui->ch_vs_num->setEnabled(false);
        ui->ch_vs_num->setChecked(false);
        ui->ch_pol_sernum->setEnabled(true);
        ui->ch_pol_sernum->setChecked(true);
        ui->ch_date_activate->setEnabled(true);
        ui->ch_date_activate->setChecked(false);
        ui->ch_date_get2hand->setEnabled(true);
        ui->ch_date_get2hand->setChecked(false);
        break;
    default:
        ui->ch_vs_num->setEnabled(false);
        ui->ch_vs_num->setChecked(false);
        ui->ch_pol_sernum->setEnabled(false);
        ui->ch_pol_sernum->setChecked(false);
        ui->ch_date_activate->setEnabled(false);
        ui->ch_date_activate->setChecked(false);
        ui->ch_date_get2hand->setEnabled(false);
        ui->ch_date_get2hand->setChecked(false);
        break;
    }

    if (data_polis.has_enp) {
        ui->ch_pol_enp->setChecked(true);
        ui->line_enp->setText(data_polis.enp);
    } else {
        ui->ch_pol_enp->setChecked(false);
        ui->line_enp->setText("");
    }
    if (data_polis.has_pol_sernum) {
        ui->line_pol_ser->setText(data_polis.pol_ser);
        ui->line_pol_num->setText(data_polis.pol_num);
    } else {
        ui->line_pol_ser->setText("");
        ui->line_pol_num->setText("");
    }
    if (data_polis.has_vs_num) {
        //ui->ch_vs_num->setChecked(true);
        ui->line_vs_num->setText(data_polis.vs_num);
    } else {
        //ui->ch_vs_num->setChecked(false);
        ui->line_vs_num->setText("");
    }
    if (data_polis.has_uec_num) {
        ui->ch_uec_num->setChecked(true);
        ui->line_uec_num->setText(data_polis.uec_num);
    } else {
        ui->ch_uec_num->setChecked(false);
        ui->line_uec_num->setText("");
    }
    if (data_polis.has_date_begin) {
        ui->date_pol_begin->setDate(data_polis.date_begin);
        ui->ch_pol_datbegin->setChecked(true);
        ui->date_pol_begin->setVisible(true);
    } else {
        ui->date_pol_begin->setDate(QDate(1900,1,1));
        ui->ch_pol_datbegin->setChecked(false);
        ui->date_pol_begin->setVisible(false);
    }
    if (data_polis.has_date_end) {
        ui->date_pol_end->setDate(data_polis.date_end);
        ui->ch_pol_datend->setChecked(true);
        ui->date_pol_end->setVisible(true);
    } else {
        ui->date_pol_end->setDate(QDate(1900,1,1));
        ui->ch_pol_datend->setChecked(false);
        ui->date_pol_end->setVisible(false);
    }
    if (data_polis.has_date_stop) {
        ui->date_pol_stop->setDate(data_polis.date_stop);
        ui->ch_pol_datstop->setChecked(true);
        ui->date_pol_stop->setVisible(true);
    } else {
        ui->date_pol_stop->setDate(QDate(1900,1,1));
        ui->ch_pol_datstop->setChecked(false);
        ui->date_pol_stop->setVisible(false);
    }
    if (data_polis.has_date_activate) {
        ui->date_activate->setDate(data_polis.date_activate);
        ui->ch_date_activate->setChecked(true);
        ui->date_activate->setVisible(true);
    } else {
        ui->date_activate->setDate(QDate(1900,1,1));
        ui->ch_date_activate->setChecked(false);
        ui->date_activate->setVisible(false);
    }
    if (data_polis.has_date_get2hand) {
        ui->date_get2hand->setDate(data_polis.date_get2hand);
        ui->ch_date_get2hand->setChecked(true);
        ui->date_get2hand->setVisible(true);
        ui->bn_get2hand_now->setChecked(true);
        ui->bn_get2hand_now->setVisible(true);
    } else {
        ui->date_get2hand->setDate(QDate(1900,1,1));
        ui->ch_date_get2hand->setChecked(false);
        ui->date_get2hand->setVisible(false);
        ui->bn_get2hand_now->setChecked(false);
        ui->bn_get2hand_now->setVisible(false);
    }
    ui->combo_pol_in_erp->setCurrentIndex(ui->combo_pol_in_erp->findData(data_polis.in_erp));

    ui->lab_order_num->setText(data_polis.order_num);
    ui->lab_order_date->setText(data_polis.order_date.toString("dd.MM.yyyy"));

    ui->combo_first_point->setCurrentIndex(ui->combo_first_point->findData(data_polis._id_first_point));
    ui->combo_first_operator->setCurrentIndex(ui->combo_first_operator->findData(data_polis._id_first_operator));

    ui->lab_last_point_name->setText(data_polis.last_point_name);
    ui->lab_last_operator_fio->setText(data_polis.last_operator_fio);

    ui->lab_dt_ins->setText(data_polis.dt_ins.toString("dd.MM.yyyy"));
    ui->lab_dt_upd->setText(data_polis.dt_upd.toString("dd.MM.yyyy"));
}

change_polis_wnd::~change_polis_wnd() {
    delete ui;
}

void change_polis_wnd::refresh_form_polis() {
    this->setCursor(Qt::WaitCursor);
    // обновление списка форм изготовления полиса
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, '(' || code || ') ' || text "
                  " from public.spr_fpolis s "
                  " order by code ; ";
    mySQL.exec(this, sql, QString("Справочник форм изготовления полиса"), *query, true, db, data_app);
    ui->combo_vizit_pan_form_polis->clear();
    while (query->next()) {
        ui->combo_vizit_pan_form_polis->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_vizit_pan_form_polis->setCurrentIndex(0);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void change_polis_wnd::refresh_reasons_smo() {
    this->setCursor(Qt::WaitCursor);
    // обновление списка причин подачи заявления
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, '(' || code || ') ' || text "
                  " from public.spr_rsmo s "
                  " order by code ; ";
    mySQL.exec(this, sql, QString("Справочник причин подачи заявления"), *query, true, db, data_app);
    ui->combo_r_smo->clear();
    while (query->next()) {
        ui->combo_r_smo->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_r_smo->setCurrentIndex(0);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void change_polis_wnd::refresh_reasons_polis() {
    this->setCursor(Qt::WaitCursor);
    // обновление списка причин выдачи полиса
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, '(' || code || ') ' || text "
                  " from public.spr_rpolis s "
                  " order by code ; ";
    mySQL.exec(this, sql, QString("Справочник причин выдачи полиса"), *query, true, db, data_app);
    ui->combo_r_polis->clear();
    while (query->next()) {
        ui->combo_r_polis->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_r_polis->setCurrentIndex(0);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void change_polis_wnd::refresh_pol_ves() {
    this->setCursor(Qt::WaitCursor);
    // обновление списка форм изготовления полиса
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, '(' || code || ') ' || text "
                  " from public.spr_f008 s "
                  " order by code ; ";
    mySQL.exec(this, sql, QString("Обновление справочника форм выпуска полисов"), *query, true, db, data_app);
    ui->combo_pol_v->clear();
    while (query->next()) {
        ui->combo_pol_v->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_pol_v->setCurrentIndex(0);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void change_polis_wnd::refresh_points() {
    this->setCursor(Qt::WaitCursor);

    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "";
    sql += QString("select p.id, '('||p.point_regnum||') '||p.point_name as name"
                   " from points p "
                   " where status>0 "
                   " order by p.point_regnum, id ; ");
    query->exec( sql );
    ui->combo_first_point->clear();

    // заполнение списка пунктов выдачи
    while (query->next()) {
        ui->combo_first_point->addItem( query->value(1).toString(),
                                  query->value(0).toInt() );
    }
    query->finish();
    delete query;

    ui->combo_first_point->setCurrentIndex(ui->combo_first_point->findData(data_polis._id_first_point));

    this->setCursor(Qt::ArrowCursor);
}

void change_polis_wnd::refresh_operators(int id_point) {
    this->setCursor(Qt::WaitCursor);

    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "";
    sql += "select u.id, '('||p.point_regnum||')  '||u.oper_fio, u.oper_fio ";
    sql += "  from operators u "
           "  left join points p on u.id_point=p.id "
           " where u.status>0 ";

    sql += " order by u.oper_fio ; ";
    query->exec( sql );
    ui->combo_first_operator->clear();

    // заполнение списка операторов
    while (query->next()) {
        ui->combo_first_operator->addItem( query->value(1).toString(),
                                     query->value(0).toInt() );
    }
    query->finish();
    delete query;

    // восстановим прежнее значение
    ui->combo_first_operator->setCurrentIndex(ui->combo_first_operator->findData(data_polis._id_first_operator));

    this->setCursor(Qt::ArrowCursor);
}


void change_polis_wnd::on_ch_pol_datbegin_clicked(bool checked) {
    ui->date_pol_begin->setEnabled(checked);
    ui->date_pol_begin->setVisible(true);
}

void change_polis_wnd::on_ch_pol_datend_clicked(bool checked) {
    ui->date_pol_end->setEnabled(checked);
    ui->date_pol_end->setVisible(true);
}

void change_polis_wnd::on_ch_pol_datstop_clicked(bool checked) {
    ui->date_pol_stop->setEnabled(checked);
    ui->date_pol_stop->setVisible(true);
}

void change_polis_wnd::on_ch_pol_sernum_clicked(bool checked) {
    ui->line_pol_num->setEnabled(checked);
    ui->line_pol_ser->setEnabled(checked);
}

void change_polis_wnd::on_ch_r_smo_clicked(bool checked) {
    ui->combo_r_smo->setEnabled(checked);
}

void change_polis_wnd::on_ch_r_polis_clicked(bool checked) {
    ui->combo_r_polis->setEnabled(checked);
}

void change_polis_wnd::on_ch_vs_num_clicked(bool checked) {
    ui->line_vs_num->setEnabled(checked);
}

void change_polis_wnd::on_ch_uec_num_clicked(bool checked) {
    ui->line_uec_num->setEnabled(checked);
}

void change_polis_wnd::on_ch_pol_enp_clicked(bool checked) {
    ui->line_enp->setEnabled(checked);
}

void change_polis_wnd::on_ch_date_activate_clicked(bool checked) {
    ui->date_activate->setEnabled(checked);
    ui->date_activate->setVisible(true);
}

void change_polis_wnd::on_ch_date_get2hand_clicked(bool checked) {
    ui->date_get2hand->setEnabled(checked);
    ui->date_get2hand->setVisible(true);
    ui->bn_get2hand_now->setEnabled(checked);
    ui->bn_get2hand_now->setVisible(true);
}

void change_polis_wnd::on_bn_close_clicked() {
    reject();
}

void change_polis_wnd::on_bn_save_clicked() {
    if (QMessageBox::question(this, "Применить сделанные изменения?",
                              "Применить все сделанные изменения в базу даннх?\n",
                              QMessageBox::Yes|QMessageBox::Cancel,
                              QMessageBox::Yes)==QMessageBox::Cancel) {
        return;
    }

    data_polis.f_polis = ui->combo_vizit_pan_form_polis->currentData().toInt();
    if (ui->ch_r_smo->isChecked())
        data_polis.r_smo = ui->combo_r_smo->currentData().toInt();
    else data_polis.r_smo = 0;
    if (ui->ch_r_polis->isChecked())
        data_polis.r_polis = ui->combo_r_polis->currentData().toInt();
    else data_polis.r_polis = 0;
    data_polis.pol_v = ui->combo_pol_v->currentData().toInt();
    switch (data_polis.pol_v) {
    case 2:
        if ( !ui->ch_vs_num->isChecked() ) {
            QMessageBox::warning(this, "Не хватает данных", "Для временного свидетельства должен быть указан номер ВС.");
            return;
        }
        break;
    case 3:
        if ( !ui->ch_pol_sernum->isChecked() ) {
            QMessageBox::warning(this, "Не хватает данных", "Для полиса единого образца должны быть указаны серия и номер бланка полиса.");
            return;
        }
        if ( !ui->ch_pol_enp->isChecked() ) {
            QMessageBox::warning(this, "Не хватает данных", "Для полиса единого образца должен быть указан Единый Номер Полиса.");
            return;
        }
        break;
    default:
        break;
    }

    data_polis.in_erp = ui->combo_pol_in_erp->currentData().toInt();

    data_polis.has_pol_sernum = ui->ch_pol_sernum->isChecked();
    if (ui->ch_pol_sernum->isChecked()) {
        if (ui->line_pol_ser->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Не хватает данных", "Серия полиса единого образца не может быть пустой.");
            return;
        }
        if (ui->line_pol_num->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Не хватает данных", "Номер полиса единого образца не может быть пустым.");
            return;
        }
        data_polis.pol_ser = ui->line_pol_ser->text().trimmed();
        data_polis.pol_num = ui->line_pol_num->text().trimmed();
    } else {
        data_polis.pol_ser.clear();
        data_polis.pol_num.clear();
    }

    data_polis.has_vs_num = ui->ch_vs_num->isChecked();
    if (ui->ch_vs_num->isChecked()) {
        if (ui->line_vs_num->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Не хватает данных", "Номер ВС не может быть пустым.");
            return;
        }
        data_polis.vs_num = ui->line_vs_num->text();
    } else {
        data_polis.vs_num.clear();
    }

    data_polis.has_uec_num = ui->ch_uec_num->isChecked();
    if (ui->ch_uec_num->isChecked()) {
        if (ui->line_uec_num->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Не хватает данных", "Номер УЭК не может быть пустым.\nЕсли УЭК нет - снимите соответствующую галочку.");
            return;
        }
        if (ui->line_uec_num->text().trimmed().length()<19) {
            QMessageBox::warning(this, "Ошибка в данных", "Номер УЭК не может быть короче 19 цифр.\nЕсли УЭК нет - снимите соответствующую галочку.");
            return;
        }
        data_polis.uec_num = ui->line_uec_num->text();
    } else {
        data_polis.uec_num.clear();
    }

    data_polis.has_enp = ui->ch_pol_enp->isChecked();
    if (ui->ch_pol_enp->isChecked()) {
        if (ui->line_enp->text().trimmed().isEmpty() || ui->line_enp->text().trimmed().length()!=16 ) {
            QMessageBox::warning(this, "Ошибка в данных", "Единый номер полиса должен состоять из 16 цифр.");
            return;
        }
        data_polis.enp = ui->line_enp->text();
    } else {
        data_polis.enp.clear();
    }

    data_polis.has_date_begin = ui->ch_pol_datbegin->isChecked();
    if (ui->ch_pol_datbegin->isChecked()) {
        if (ui->date_pol_begin->date()<=QDate(1920,1,1)) {
            QMessageBox::warning(this, "Ошибка в данных", "Дата выдачи полиса должна быть позже 01.01.1920 г.");
            return;
        }
        data_polis.date_begin = ui->date_pol_begin->date();
    } else {
        data_polis.date_begin = QDate(1900,1,1);
    }

    data_polis.has_date_end = ui->ch_pol_datend->isChecked();
    if (ui->ch_pol_datend->isChecked()) {
        if (ui->date_pol_end->date()<=QDate(1920,1,1)) {
            QMessageBox::warning(this, "Ошибка в данных", "Дата изъятия полиса должна быть позже 01.01.1920 г.");
            return;
        }
        data_polis.date_end = ui->date_pol_end->date();
    } else {
        data_polis.date_end = QDate(1900,1,1);
    }

    data_polis.has_date_stop = ui->ch_pol_datstop->isChecked();
    if (ui->ch_pol_datstop->isChecked()) {
        if (ui->date_pol_stop->date()<=QDate(1920,1,1)) {
            QMessageBox::warning(this, "Ошибка в данных", "Плановая дата окончания действия полиса должна быть позже 01.01.1920 г.");
            return;
        }
        data_polis.date_stop = ui->date_pol_stop->date();
    } else {
        data_polis.date_stop = QDate(1900,1,1);
    }

    data_polis.has_date_activate = ui->ch_date_activate->isChecked();
    if (ui->ch_date_activate->isChecked()) {
        if (ui->date_activate->date()<=QDate(1920,1,1)) {
            QMessageBox::warning(this, "Ошибка в данных", "Дата активации полиса должна быть позже 01.01.1920 г.");
            return;
        }
        data_polis.date_activate = ui->date_activate->date();
    } else {
        data_polis.date_activate = QDate(1900,1,1);
    }

    data_polis.has_date_get2hand = ui->ch_date_get2hand->isChecked();
    if (ui->ch_date_get2hand->isChecked()) {
        if (ui->date_get2hand->date()<=QDate(1920,1,1)) {
            QMessageBox::warning(this, "Ошибка в данных", "Дата выдачи полиса на руки должна быть позже 01.01.1920 г.");
            return;
        }
        data_polis.date_get2hand = ui->date_get2hand->date();
    } else {
        data_polis.date_get2hand = QDate(1900,1,1);
    }

    data_polis._id_first_point = ui->combo_first_point->currentData().toInt();
    data_polis._id_first_operator = ui->combo_first_operator->currentData().toInt();

    accept();
}

void change_polis_wnd::on_bn_get2hand_now_clicked() {
    ui->date_get2hand->setDate(QDate::currentDate())    ;
}

void change_polis_wnd::on_line_enp_textChanged(const QString &arg1) {
    QString txt0 = arg1.trimmed(), txt = "";
    for (int i=0; i<txt0.size(); ++i) {
        if (txt.length()>=16)
            break;
        QString c = txt0.mid(i, 1);
        if ( c=="0" || c=="1" || c=="2" || c=="3" || c=="4" || c=="5" || c=="6" || c=="7" || c=="8" || c=="9" ) {
            txt += c;
        }
    }
    ui->line_enp->setText(txt);
    ui->line_enp->setCursorPosition(txt.length());
}

void change_polis_wnd::on_line_pol_ser_textChanged(const QString &arg1) {
    QString txt0 = arg1.trimmed(), txt = "";
    for (int i=0; i<txt0.size(); ++i) {
        if (txt.length()>=4)
            break;
        QString c = txt0.mid(i, 1);
        if ( c=="0" || c=="1" || c=="2" || c=="3" || c=="4" || c=="5" || c=="6" || c=="7" || c=="8" || c=="9" ) {
            txt += c;
        }
    }
    ui->line_pol_ser->setText(txt);
    ui->line_pol_ser->setCursorPosition(txt.length());
}

void change_polis_wnd::on_line_pol_num_textChanged(const QString &arg1) {
    QString txt0 = arg1.trimmed(), txt = "";
    for (int i=0; i<txt0.size(); ++i) {
        if (txt.length()>=11)
            break;
        QString c = txt0.mid(i, 1);
        if ( c=="0" || c=="1" || c=="2" || c=="3" || c=="4" || c=="5" || c=="6" || c=="7" || c=="8" || c=="9" ) {
            txt += c;
        }
    }
    ui->line_pol_num->setText(txt);
    ui->line_pol_num->setCursorPosition(txt.length());
}

void change_polis_wnd::on_line_uec_num_textChanged(const QString &arg1) {
    QString txt0 = arg1.trimmed(), txt = "";
    for (int i=0; i<txt0.size(); ++i) {
        if (txt.length()>=22)
            break;
        QString c = txt0.mid(i, 1);
        if ( c=="0" || c=="1" || c=="2" || c=="3" || c=="4" || c=="5" || c=="6" || c=="7" || c=="8" || c=="9" ) {
            txt += c;
        }
    }
    ui->line_uec_num->setText(txt);
    ui->line_uec_num->setCursorPosition(txt.length());
}

void change_polis_wnd::on_line_vs_num_textChanged(const QString &arg1) {
    QString txt0 = arg1.trimmed(), txt = "";
    for (int i=0; i<txt0.size(); ++i) {
        if (txt.length()>=11)
            break;
        QString c = txt0.mid(i, 1);
        if ( c=="0" || c=="1" || c=="2" || c=="3" || c=="4" || c=="5" || c=="6" || c=="7" || c=="8" || c=="9" ) {
            txt += c;
        }
    }
    ui->line_vs_num->setText(txt);
    ui->line_vs_num->setCursorPosition(txt.length());
}

void change_polis_wnd::on_ch_pol_oper_clicked(bool checked) {
    ui->combo_first_point->setEnabled(checked);
    ui->combo_first_operator->setEnabled(checked);
}
