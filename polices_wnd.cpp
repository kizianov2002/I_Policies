#include "polices_wnd.h"
#include "ui_polices_wnd.h"
#include <QInputDialog>
#include <QStatusBar>
#include <QtSql>
#include <QDate>

#include <winuser.h>

#include "s_data_blank.h"

polices_wnd::polices_wnd(QSqlDatabase &db, s_data_app &data_app, s_data_pers &data_pers/*, s_data_doc &data_doc*/, QSettings &settings, QWidget *parent)
    : db(db), data_app(data_app), data_pers(data_pers)/*, data_doc(data_doc)*/, settings(settings), QDialog(parent), ui(new Ui::polices_wnd)
{
    ui->setupUi(this);

    QFont font = this->font();
    font.setPointSize(data_app.font_size);
    this->setFont(font);

    ui->rb_previzit->setChecked(true);
    ui->pan_first_vizit->setVisible(true);

    ui->rb_activate->setChecked(false);
    ui->pan_activate->setVisible(false);

    ui->rb_stoppolis->setChecked(false);
    ui->pan_stoppolis->setVisible(false);

    ui->rb_corrdata->setChecked(false);

    statusLab = NULL;

    print_vs_w = NULL;
    print_ch_smo_w = NULL;
    print_db_pol_w = NULL;
    print_notice_pol_w = NULL;
    //ui->bn_polises_next->setVisible(false);
    pvp_n_vs_w = NULL;
    blanks_w = NULL;
    work_calendar_w = NULL;
    print_notic2_w = NULL;

    ui->show_pan_vizit->setVisible(false);
    ui->show_pan_polises->setVisible(false);
    ui->show_pan_polis->setVisible(false);

    // раскладка
    long long lay = (long long)GetKeyboardLayout(GetCurrentThreadId());
    if (lay == 68748313) {      //Russian
        ui->bn_layout->setText("Руc");
        QPalette pal = ui->bn_layout->palette();
        pal.setColor(QPalette::Button,QColor(0,255,0,255));
        ui->bn_layout->setPalette(pal);
    } else if(lay == 67699721) {  //English
        ui->bn_layout->setText("Eng");
        QPalette pal = ui->bn_layout->palette();
        pal.setColor(QPalette::Button,QColor(128,128,255,255));
        ui->bn_layout->setPalette(pal);
    } else if(lay == 69338146) {  //Ukrainian
        ui->bn_layout->setText("Укр");
        QPalette pal = ui->bn_layout->palette();
        pal.setColor(QPalette::Button,QColor(255,0,0,255));
        ui->bn_layout->setPalette(pal);
    } else { //unknown language
        ui->bn_layout->setText("???");
        QPalette pal = ui->bn_layout->palette();
        pal.setColor(QPalette::Button,QColor(200,200,200,255));
        ui->bn_layout->setPalette(pal);
    }

    this->setWindowTitle("ИНКО-МЕД - Полисы ОМС - Таблица \"Полисы застрахованного\"   ->   база " + data_app.db_host + ":" + data_app.db_name );

    ui->combo_terr_ocato_text->setEnabled(data_app.is_tech);

    QFont f = this->font();
    f.setPointSize(data_app.font_size);
    this->setFont(f);

    refresh_vizit_method();
    refresh_reasons_smo();
    refresh_reasons_polis();
    refresh_form_polis();

    ui->ch_vizit_pan_has_petition->setChecked(false);

    start();

    QStatusBar *b = new QStatusBar;
    this->layout()->addWidget(b);
    statusLab = new QLabel;
    b->addWidget(statusLab);
    statusLab->setFont(QFont("Arial",10,75,false));
    QPalette pal = statusLab->palette();
    pal.setColor(QPalette::WindowText,QColor(0,0,128));
    statusLab->setPalette(pal);
    statusLab->setText("программа: \"Полисы ОМС\", версия:" + data_app.version + ";  сервер: " + data_app.db_host + ";  БД: " + data_app.db_name + ";  версия: " + data_app.db_vers);

    statusOper = new QLabel;
    b->addWidget(statusOper);
    statusOper->setFont(QFont("Arial",10,50,false));
    pal = statusOper->palette();
    pal.setColor(QPalette::WindowText,QColor(0,80,0));
    statusOper->setPalette(pal);
    statusOper->setText("ПВП: " + data_app.point_name + ";  оператор: " + data_app.operator_fio + "  (OK)");

    // обновим combo_in_erp
    ui->combo_pol_in_erp->clear();
    ui->combo_pol_in_erp->addItem("нет", 0);
    ui->combo_pol_in_erp->addItem("да", 1);
    //ui->combo_pol_in_erp->addItem("отказ", -1);
    //ui->combo_pol_in_erp->addItem("закрыт вручную", -2);
    ui->combo_pol_in_erp->addItem("NULL", -999);

    ui->lab_folder->setText((!data_app.use_folders || data_app.default_folder.isEmpty()) ? " - нет - " : data_app.default_folder);

    on_rb_previzit_clicked();
    on_rb_activate_clicked();

    ui->split_main->setStretchFactor(0,2);
    ui->split_main->setStretchFactor(1,5);
    ui->split_main->setStretchFactor(2,3);

    ui->ch_get2hand->setChecked(true);
    on_ch_get2hand_clicked(true);

    data_polis.id = -1;
    data_polis.id_person = data_pers.id;
    data_polis.ocato = data_app.ocato;
    data_polis.ogrn_smo = "";
    data_polis.pol_v = -1;
    data_polis.pol_v_text = "";
    data_polis.pol_type = -1;
    data_polis.has_enp = "";
    data_polis.has_vs_num = false;
    data_polis.has_pol_sernum = false;
    data_polis.has_uec_num = false;
    data_polis.vs_num = "";
    data_polis.pol_ser = "";
    data_polis.pol_num = "";
    data_polis.uec_num = "";
    data_polis.enp = "";
    data_polis.has_tfoms_date = false;
    data_polis.has_date_sms_pol = false;
    data_polis.has_date_begin = false;
    data_polis.has_date_end = false;
    data_polis.has_date_stop = false;
    data_polis.tfoms_date = QDate(1900,1,1);
    data_polis.date_sms_pol = QDate(1900,1,1);
    data_polis.date_begin = QDate(1900,1,1);
    data_polis.date_end = QDate(1900,1,1);
    data_polis.date_stop = QDate(1900,1,1);
    data_polis.in_erp = 0;
    data_polis.order_num = "";
    data_polis.order_date = QDate(1900,1,1);

    data_polis.has_date_activate = false;
    data_polis.has_date_get2hand = false;
    data_polis.date_activate = QDate(1900,1,1);
    data_polis.date_get2hand = QDate(1900,1,1);

    data_polis.f_polis = 1;
    data_polis.id_old_polis = -1;

    ui->ch_get2hand->setEnabled(data_app.ocato=="38000");
}

polices_wnd::~polices_wnd() {
    delete ui;
}

void polices_wnd::b_need_save (bool f) {
    ui->bn_polis_save->setVisible(f);
    ui->bn_print_vs_2->setVisible(f);
}

QDate polices_wnd::myAddDays(QDate date_start, int n_days) {
    if (n_days<1)
        n_days = 30;

    QDate date_c = date_start;
    int add_days = 1;   // текущий день тоже считаем

    do {
        date_c = date_c.addDays(1);

        // проверим, выходной ли это
        QString sql = "select status from work_calendar where date='" + date_c.toString("yyyy-MM-dd") + "' ; ";
        QSqlQuery *query = new QSqlQuery(db);
        bool res = mySQL.exec(this, sql, QString("Проверка дня на выходной"), *query, true, db, data_app);
        if (!res) {
            db.rollback();
            QMessageBox::warning(this, "Ошибка при проверке дня на выходной",
                                 "Не удалось проверить день на выходной. \n"
                                 "Дата вычислена простым прибавлением 30 рабочих дней без учёта праздников.\n\n"
                                 "Операция отменена.");
            date_c = date_start.addDays(42);
            return date_c;
        }
        query->next();
        bool f_day = query->value(0).isNull();
        int status = query->value(0).toInt();
        delete query;
        int week_day = date_c.dayOfWeek();
        if ( ((week_day==1 || week_day==2 || week_day==3 || week_day==4 || week_day==5) && f_day )
                            // рабочий день, и его нет в календаре праздников
           || status==1 )   // или этот день явно помечен как рабочий
            add_days++;
    } while (add_days<n_days);
    return date_c;
}

void polices_wnd::on_combo_vizit_pan_form_polis_activated(const QString &arg1)
  {  b_need_save (true);  }
void polices_wnd::on_combo_pol_v_activated(int index)
  {  b_need_save (true);  }
void polices_wnd::on_line_pol_ser_textEdited(const QString &arg1) {
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
    b_need_save (true);
}
void polices_wnd::on_line_pol_num_editingFinished()
  {  b_need_save (true);  }
void polices_wnd::on_line_vs_num_editingFinished()
  {  b_need_save (true);  }
void polices_wnd::on_line_enp_cursorPositionChanged(int arg1, int arg2)
  {  b_need_save (true);  }
void polices_wnd::on_ch_pol_erp_clicked()
  {  b_need_save (true);  }
void polices_wnd::on_ch_pol_datstop_clicked()
  {  b_need_save (true);  }
void polices_wnd::on_ch_pol_datend_clicked()
  {  b_need_save (true);  }
void polices_wnd::on_ch_pol_datbegin_clicked()
  {  b_need_save (true);  }
void polices_wnd::on_ch_pol_enp_clicked()
  {  b_need_save (true);  }
void polices_wnd::on_ch_vs_num_clicked()
  {  b_need_save (true);  }
void polices_wnd::on_ch_pol_sernum_clicked()
  {  b_need_save (true);  }
void polices_wnd::on_date_pol_begin_editingFinished() {
    b_need_save (true);
    if (ui->date_pol_begin->date()!=QDate::currentDate()) {
        ui->date_pol_begin->setDate(QDate::currentDate());
        QMessageBox::warning(this, "Действие запрещено", "Новый полис может быть выдан только текущей датой!");
    }
}
void polices_wnd::on_date_pol_end_editingFinished() {
    b_need_save (true);
    if (QDate::currentDate().daysTo(ui->date_pol_end->date())<10) {
        ui->date_pol_end->setDate(QDate::currentDate().addMonths(1).addDays(9));
        data_polis.date_end = QDate::currentDate().addMonths(1).addDays(9);
        QMessageBox::warning(this, "Действие запрещено", "Срок действия нового полиса не может быть меньше 1 месяца!");
    }
}
void polices_wnd::on_date_pol_stop_editingFinished()
  {  b_need_save (true);  }

void polices_wnd::on_show_pan_vizit_clicked(bool checked) {
    ui->show_pan_vizit->setChecked(checked);
    ui->pan_vizit->setEnabled(checked);
    //if (ui->pan_vizit->isEnabled()) ui->pan_vizit->setEnabled(checked);
}
void polices_wnd::on_show_pan_polises_clicked(bool checked) {
    ui->show_pan_polises->setChecked(checked);
    ui->pan_polises->setEnabled(checked);
    ui->pan_polises->setVisible(checked);
}
void polices_wnd::on_show_pan_polis_clicked(bool checked) {
    ui->show_pan_polis->setChecked(checked);
    ui->pan_polis->setEnabled(checked);
    ui->pan_polis->setVisible(checked);
}

void polices_wnd::start() {
    ui->date_vizit_pan->setDate(QDate::currentDate());
    refresh_vizit_method();
    refresh_reasons_smo();
    refresh_reasons_polis();
    refresh_form_polis();

    on_show_pan_vizit_clicked(true);
    on_show_pan_polises_clicked(false);
    on_show_pan_polis_clicked(false);

    ui->show_pan_vizit->setEnabled(true);
    ui->show_pan_polises->setEnabled(false);
    ui->show_pan_polis->setEnabled(false);
}

bool polices_wnd::save_vizit() {
    this->setCursor(Qt::WaitCursor);

    db.transaction();

    // проверим, был ли сегодня визит
    QSqlQuery *query_vz = new QSqlQuery(db);
    QString sql_vz = "select id"
                  " from vizits "
                  " where id_person=" + QString::number(data_pers.id) + " and id_point=" + QString::number(data_app.id_point) + " and v_date='" + data_vizit.date.toString("yyyy-MM-dd") + "' ; ";
    bool res_sg = mySQL.exec(this, sql_vz, QString("Проверка, был ли сегодня другой визит этого же застрахованного"), *query_vz, true, db, data_app);
    if (res_sg && query_vz->size()>0 &&
        QMessageBox::information(this, "Обновление данных визита",
                                 "Этот застрахованный уже посещал сегодня этот пункт выдачи.\n\nОбновить данные визита?",
                                 QMessageBox::Yes|QMessageBox::No,
                                 QMessageBox::No)==QMessageBox::Yes) {
        // визит уже был - обновим
        query_vz->next();
        data_vizit.id = query_vz->value(0).toInt();
        QSqlQuery *query_upd = new QSqlQuery(db);
        QString sql_upd = "update vizits  "
                          " set id_person="   + QString::number(data_pers.id)         + ", "
                          "     id_point="    + QString::number(data_app.id_point)    + ", "
                          "     id_operator=" + QString::number(data_app.id_operator) + "  "
                          //"     v_date='"     + data_vizit.date.toString("yyyy-MM-dd")+ "',"
                          //"     v_method="      + QString::number(data_vizit.v_method)    + ", "
                          //"     has_petition="+ (data_vizit.has_petition ? "1" : "0") + ", "
                          //"     r_smo="       + QString::number(data_vizit.r_smo)     + ", "
                          //"     r_polis="     + QString::number(data_vizit.r_polis)   + ", "
                          //"     f_polis="     + QString::number(data_vizit.f_polis)   + "  "
                          " where id=" + QString::number(data_vizit.id) + " ; ";
        bool res_upd = mySQL.exec(this, sql_upd, QString("Обновление данных повторного визита застрахованного"), *query_upd, true, db, data_app);
        if (!res_upd) {
            db.rollback();
            QMessageBox::warning(this, "Ошибка при обновлении визита", "Не удалось обновить запись визита.\n\nОперация отменена.");
            return false;
        }
        delete query_upd;
    } else if (res_sg && query_vz->size()==0) {
        // добавим новый визит
        QSqlQuery *query_ins = new QSqlQuery(db);
        QString sql_ins = "insert into vizits (id_point, id_operator, id_person, v_date, v_method, has_petition, r_smo, r_polis, f_polis) "
                          " values(" + QString::number(data_app.id_point)    + ", "
                          "        " + QString::number(data_app.id_operator) + ", "
                          "        " + QString::number(data_pers.id)         + ", "
                          "        '"+ data_vizit.date.toString("yyyy-MM-dd")+"', "
                          "        " + QString::number(data_vizit.v_method)    + ", "
                          "        " + (data_vizit.has_petition ? "1" : "0") + ", "
                          "        " + QString::number(data_vizit.r_smo)     + ", "
                          "        " + QString::number(data_vizit.r_polis)   + ", "
                          "        " + QString::number(data_vizit.f_polis)   + ") "
                          " returning id ; ";
        bool res_ins = mySQL.exec(this, sql_ins, QString("Сохранение данных нового визита застрахованного"), *query_ins, true, db, data_app);
        if (res_ins && query_ins->next()) {
            data_vizit.id = query_ins->value(0).toInt();
        } else {
            db.rollback();
            delete query_ins;
            QMessageBox::warning(this, "Ошибка при сохранении визита", "Не удалось сохранить запись визита.\n\nОперация отменена.");
            return false;
        }
        delete query_ins;
    }
    delete query_vz;
    db.commit();
    this->setCursor(Qt::ArrowCursor);
    return true;
}

bool polices_wnd::load_vizit() {
    this->setCursor(Qt::WaitCursor);

    db.transaction();

    // получим последний визит
    QSqlQuery *query_vz = new QSqlQuery(db);
    QString sql_vz = "select v.id, v.v_date, v.v_method, v.has_petition, v.r_smo, v.r_polis, v.f_polis "
                     " from vizits v "
                     "   join ( select max(v_date) as v_date "
                     "          from vizits "
                     "          where id_person=" + QString::number(data_pers.id) + " "
                     "        ) q on(q.v_date=v.v_date) "
                     " where id_person=" + QString::number(data_pers.id) + " ; ";
    bool res_vz = mySQL.exec(this, sql_vz, QString("Получим последний визит застрахованного"), *query_vz, true, db, data_app);
    if (!res_vz || query_vz->size()<1 || !query_vz->next()) {
        // визита небыло - отмена
        this->setCursor(Qt::ArrowCursor);
        QMessageBox::warning(this,
                             "Визита небыло - нечего править",
                             "Для данного застрахованного не найдены данные прежнего визита - нечего править, операция отменена");
        delete query_vz;
        return false;
    }
    //перечитаем данные визита
    data_vizit.id = query_vz->value(0).toInt();
    data_vizit.date = query_vz->value(1).toDate();
    data_vizit.v_method = query_vz->value(2).toInt();
    data_vizit.has_petition = (query_vz->value(2).toInt()>0);
    data_vizit.r_smo = query_vz->value(4).toInt();
    data_vizit.r_polis = query_vz->value(5).toInt();
    data_vizit.f_polis = query_vz->value(6).toInt();
    delete query_vz;
    db.commit();
    this->setCursor(Qt::ArrowCursor);
    return true;
}

void polices_wnd::on_bn_vizit_next_clicked() {    
    // флажок -  это активация
    data_vizit.is_preinsurez = ui->rb_previzit->isChecked();
    data_vizit.is_activation = ui->rb_activate->isChecked();
    data_vizit.is_stoppolis  = ui->rb_stoppolis->isChecked();
    data_vizit.is_correction = ui->rb_corrdata->isChecked();

    if (data_vizit.is_preinsurez) {
        data_vizit.v_method  = ui->combo_vizit_pan_method->currentData().toInt();
        data_vizit.date = ui->date_vizit_pan->date();
        data_vizit.has_petition = ui->ch_vizit_pan_has_petition->isChecked();
        data_vizit.r_smo   = ui->combo_vizit_pan_reason_smo->currentData().toInt();
        data_vizit.r_polis = ui->combo_vizit_pan_reason_polis->currentData().toInt();
        data_vizit.f_polis = ui->combo_vizit_pan_form_polis->currentData().toInt();

        data_polis.r_smo   = ui->combo_vizit_pan_reason_smo->currentData().toInt();
        data_polis.r_polis = ui->combo_vizit_pan_reason_polis->currentData().toInt();
        data_polis.f_polis = ui->combo_vizit_pan_form_polis->currentData().toInt();

        if ( data_vizit.is_preinsurez &&
             data_vizit.r_smo==0 &&
             data_vizit.r_polis==0 ) {
            QMessageBox::warning(this, "Выберите действие", "Выберите причину подачи заявления о выборе СМО или причину подачи заявления о выдаче дубликата / переоформлении полиса.");
            return;
        }
        bool res = save_vizit();

    } else if (data_vizit.is_activation) {
        data_vizit.v_method  = ui->combo_vizit_pan_method_act->currentData().toInt();
        data_vizit.date = ui->date_activate->date();
        data_vizit.has_petition = false;
        data_vizit.r_smo   = 0;
        data_vizit.r_polis = 0;
        data_vizit.f_polis = 1;

        data_polis.r_smo   = 0;
        data_polis.r_polis = 0;
        data_polis.f_polis = 1;

        bool res = save_vizit();

    } else if (data_vizit.is_stoppolis) {
        data_vizit.v_method  = ui->combo_vizit_pan_method_stop->currentData().toInt();
        data_vizit.date = ui->date_stoppolis->date();
        data_vizit.r_smo   = 0;
        data_vizit.r_polis = 0;
        data_vizit.f_polis = 1;

        data_polis.r_smo   = 0;
        data_polis.r_polis = 0;
        data_polis.f_polis = 1;

        bool res = save_vizit();

    } else if (data_vizit.is_correction) {
        if ( !data_app.is_head && !data_app.is_tech ) {
            QMessageBox::warning(this, "В ПВП не доступно", "Правка полисов доступна только в головном офисе.\n");
            return;
        }
        // запрос пароля на правку визита
        if (QInputDialog::getText(this,"Введите пароль",QString("Для непосредственной правки данных визита введите пароль\n") +
                                  (data_app.is_head||data_app.is_tech ? "ВИЗИТ" : "***")).toUpper()!="ВИЗИТ") {
            QMessageBox::warning(this,"Отказ","Неправильный пароль");
            return;
        }

        bool res = load_vizit();
        if (!res) return;
    }

    ui->bn_p010->setVisible( (data_vizit.is_preinsurez) );
    ui->bn_p021->setVisible( (data_vizit.is_stoppolis) );
    ui->bn_p022->setVisible( (data_vizit.is_stoppolis) );
    ui->bn_p023->setVisible( (data_vizit.is_stoppolis) );
    ui->bn_p024->setVisible( (data_vizit.is_stoppolis) );
    ui->bn_p025->setVisible( (data_vizit.is_stoppolis) );
    ui->bn_p031->setVisible( (data_vizit.is_preinsurez) );
    ui->bn_p032->setVisible( (data_vizit.is_preinsurez) );
    ui->bn_p033->setVisible( (data_vizit.is_preinsurez) );
    ui->bn_p034->setVisible( (data_vizit.is_preinsurez) );
    ui->bn_p035->setVisible( (data_vizit.is_preinsurez) );
    ui->bn_p036->setVisible( (data_vizit.is_preinsurez) );
    ui->bn_p061->setVisible( (data_vizit.is_preinsurez) );
    ui->bn_p062->setVisible( (data_vizit.is_preinsurez) );
    ui->bn_p063->setVisible( (data_vizit.is_preinsurez) );
    ui->bn_p060->setVisible( (data_vizit.is_activation) );

    ui->lab_p010->setVisible( (data_vizit.is_preinsurez) );
    ui->lab_p021->setVisible( (data_vizit.is_stoppolis) );
    ui->lab_p022->setVisible( (data_vizit.is_stoppolis) );
    ui->lab_p023->setVisible( (data_vizit.is_stoppolis) );
    ui->lab_p024->setVisible( (data_vizit.is_stoppolis) );
    ui->lab_p025->setVisible( (data_vizit.is_stoppolis) );
    ui->lab_p031->setVisible( (data_vizit.is_preinsurez) );
    ui->lab_p032->setVisible( (data_vizit.is_preinsurez) );
    ui->lab_p033->setVisible( (data_vizit.is_preinsurez) );
    ui->lab_p034->setVisible( (data_vizit.is_preinsurez) );
    ui->lab_p035->setVisible( (data_vizit.is_preinsurez) );
    ui->lab_p036->setVisible( (data_vizit.is_preinsurez) );
    ui->lab_p061->setVisible( (data_vizit.is_preinsurez) );
    ui->lab_p062->setVisible( (data_vizit.is_preinsurez) );
    ui->lab_p063->setVisible( (data_vizit.is_preinsurez) );
    ui->lab_p060->setVisible( (data_vizit.is_activation) );

    ui->ch_get2hand->setVisible( (data_vizit.is_activation) );

    on_show_pan_vizit_clicked(false);
    on_show_pan_polises_clicked(true);
    on_show_pan_polis_clicked(false);

    ui->show_pan_vizit->setEnabled(false);
    ui->show_pan_polises->setEnabled(true);
    ui->show_pan_polis->setEnabled(true);

    on_combo_vizit_pan_reason_smo_activated(ui->combo_vizit_pan_reason_smo->currentIndex());
    on_combo_vizit_pan_reason_polis_activated(ui->combo_vizit_pan_reason_polis->currentIndex());

    refresh_polises_tab();
}

void polices_wnd::on_bn_polises_back_clicked() {
    on_show_pan_vizit_clicked(true);
    on_show_pan_polises_clicked(false);
    on_show_pan_polis_clicked(false);

    ui->show_pan_vizit->setEnabled(true);
    ui->show_pan_polises->setEnabled(false);
    ui->show_pan_polis->setEnabled(false);
}

void polices_wnd::on_bn_polises_next_clicked() {
    if (QInputDialog::getText(this,"Введите пароль",QString("Для непосредственной правки данных полиса введите пароль\n") +
                              (data_app.is_head||data_app.is_tech ? "ПОЛИС" : "***")).toUpper()!="ПОЛИС") {
        QMessageBox::warning(this,"Отказ","Неправильный пароль");
        return;
    }

    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_polises || !ui->tab_polises->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите полис из списка.");
        return;
    }

    refresh_terr_ocato();
    refresh_pol_ves();

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_polises->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        this->setCursor(Qt::WaitCursor);
        QModelIndex index = indexes.front();

        // задаём ширину колонок
        ui->tab_polises->setColumnWidth( 0,  1);    // p.id
        ui->tab_polises->setColumnWidth( 1, 30);    // vs.status
        ui->tab_polises->setColumnWidth( 2, 30);    // pol.status
        ui->tab_polises->setColumnWidth( 3, 50);    // p.ocato
        ui->tab_polises->setColumnWidth( 4,115);    // p.enp
        ui->tab_polises->setColumnWidth( 5, 50);    // p.ogrn_smo
        ui->tab_polises->setColumnWidth( 6,  1);    // p.pol_v as f008_code
        ui->tab_polises->setColumnWidth( 7,140);    // f8.text as f008_text
        ui->tab_polises->setColumnWidth( 8,  1);    // p.pol_type
        ui->tab_polises->setColumnWidth( 9, 70);    // p.vs_num
        ui->tab_polises->setColumnWidth(10, 50);    // p.pol_ser
        ui->tab_polises->setColumnWidth(11, 60);    // p.pol_num
        ui->tab_polises->setColumnWidth(12, 50);    // p.uec_num
        ui->tab_polises->setColumnWidth(13, 80);    // p.date_begin
        ui->tab_polises->setColumnWidth(14, 90);    // p.date_end
        ui->tab_polises->setColumnWidth(15, 80);    // p.date_stop
        ui->tab_polises->setColumnWidth(16, 70);    // p.in_erp
        ui->tab_polises->setColumnWidth(17,  1);    // p.order_num
        ui->tab_polises->setColumnWidth(18,  1);    // p.order_date
        ui->tab_polises->setColumnWidth(19, 60);    // p.f_polis
        ui->tab_polises->setColumnWidth(20,  1);    // p.id_person
        ui->tab_polises->setColumnWidth(21,  1);    // p._id_first_point
        ui->tab_polises->setColumnWidth(22,  1);    // pt1.point_regnum,
        ui->tab_polises->setColumnWidth(23,150);    // pt1.point_name,
        ui->tab_polises->setColumnWidth(24,  1);    // p._id_first_operator,
        ui->tab_polises->setColumnWidth(25, 80);    // op1.oper_fio
        ui->tab_polises->setColumnWidth(26,  1);    // p._id_last_point,
        ui->tab_polises->setColumnWidth(27,  1);    // pt2.point_regnum,
        ui->tab_polises->setColumnWidth(28,150);    // pt2.point_name,
        ui->tab_polises->setColumnWidth(29,  1);    // p._id_last_operator,
        ui->tab_polises->setColumnWidth(30, 80);    // op1.oper_fio
        ui->tab_polises->setColumnWidth(31,  1);    // pol.id
        ui->tab_polises->setColumnWidth(32,  1);    // vs_num
        ui->tab_polises->setColumnWidth(33,  1);    // p.dt_ins
        ui->tab_polises->setColumnWidth(34,  1);    // p.dt_upd
        ui->tab_polises->setColumnWidth(35, 85);    // tfoms_date
        ui->tab_polises->setColumnWidth(36, 85);    // date_sms_pol
        ui->tab_polises->setColumnWidth(37, 85);    // date_activate
        ui->tab_polises->setColumnWidth(38, 85);    // date_get2hand
        // правим заголовки
        model_polises.setHeaderData( 1, Qt::Horizontal, ("ВС"));
        model_polises.setHeaderData( 2, Qt::Horizontal, ("П"));
        model_polises.setHeaderData( 3, Qt::Horizontal, ("ОКАТО"));
        model_polises.setHeaderData( 4, Qt::Horizontal, ("ЕНП"));
        model_polises.setHeaderData( 5, Qt::Horizontal, ("ОГРН\nСМО"));
        model_polises.setHeaderData( 7, Qt::Horizontal, ("тип полиса"));
        model_polises.setHeaderData( 9, Qt::Horizontal, ("№ ВС"));
        model_polises.setHeaderData(10, Qt::Horizontal, ("серия"));
        model_polises.setHeaderData(11, Qt::Horizontal, ("номер"));
        model_polises.setHeaderData(12, Qt::Horizontal, ("№ УЭК"));
        model_polises.setHeaderData(13, Qt::Horizontal, ("начало\nдействия"));
        model_polises.setHeaderData(14, Qt::Horizontal, ("плановое\nокончание"));
        model_polises.setHeaderData(15, Qt::Horizontal, ("дата\nизъятия"));
        model_polises.setHeaderData(16, Qt::Horizontal, ("активация\nв ЕРП?"));
        model_polises.setHeaderData(17, Qt::Horizontal, ("номер заявки на\nизгот. полиса"));
        model_polises.setHeaderData(18, Qt::Horizontal, ("дата заявки на\nизгот. полиса"));
        model_polises.setHeaderData(19, Qt::Horizontal, ("форма\nвыпуска"));
        model_polises.setHeaderData(23, Qt::Horizontal, ("первый ПВП"));
        model_polises.setHeaderData(25, Qt::Horizontal, ("первый\nинспектор"));
        model_polises.setHeaderData(28, Qt::Horizontal, ("последний ПВП"));
        model_polises.setHeaderData(30, Qt::Horizontal, ("последний\nинспектор"));
        model_polises.setHeaderData(31, Qt::Horizontal, ("ID\nЕНП"));
        model_polises.setHeaderData(32, Qt::Horizontal, ("ID\nВС"));
        model_polises.setHeaderData(35, Qt::Horizontal, ("дата заявки\nна бланк ЕНП"));
        model_polises.setHeaderData(36, Qt::Horizontal, ("дата СМС\nо готовн.ЕНП"));
        model_polises.setHeaderData(37, Qt::Horizontal, ("дата\nактивации"));
        model_polises.setHeaderData(38, Qt::Horizontal, ("выдача\nна руки"));
        ui->tab_polises->repaint();

        on_show_pan_vizit_clicked(false);
        on_show_pan_polises_clicked(true);
        on_show_pan_polis_clicked(true);

        ui->split_main->setStretchFactor(0,2);
        ui->split_main->setStretchFactor(1,5);
        ui->split_main->setStretchFactor(2,3);

        refresh_polis_data();
        b_need_save (false);

        // только при первичном выюоре СМО
        ui->lab_print_ch_smo->setEnabled(true/*data_vizit.r_smo>0*/);
        ui->bn_print_ch_smo->setEnabled(true/*data_vizit.r_smo>0*/);
        // только при выдаче дубликата полиса
        ui->lab_prind_db_pol->setEnabled(true/*data_vizit.r_polis>0*/);
        ui->bn_print_db_pol->setEnabled(true/*data_vizit.r_polis>0*/);
        // только для ВС
        ui->lab_print_vs->setEnabled(true/*ui->combo_pol_v->currentData().toInt()==2*/);
        ui->bn_print_vs->setEnabled(true/*ui->combo_pol_v->currentData().toInt()==2*/);

        // обновим значения в полях ввода по выбеденной строке
        ui->combo_vizit_pan_form_polis->setCurrentIndex(data_polis.f_polis);
        data_polis.ocato = model_polises.data(model_polises.index(index.row(), 3), Qt::EditRole).toString();
        ui->line_terr_ocato->setText(data_polis.ocato);
        ui->combo_terr_ocato_text->setCurrentIndex(ui->combo_terr_ocato_text->findData(data_polis.ocato));
        data_polis.ogrn_smo = model_polises.data(model_polises.index(index.row(), 5), Qt::EditRole).toString();
        ui->line_ogrn_smo->setText(data_polis.ogrn_smo);
        ui->line_ogrn_smo_text->setText(data_app.smo_name);
        data_polis.pol_v = model_polises.data(model_polises.index(index.row(), 6), Qt::EditRole).toInt();
        ui->combo_pol_v->setCurrentIndex(ui->combo_pol_v->findData(data_polis.pol_v));
        ui->ch_vs_num->setChecked(!model_polises.data(model_polises.index(index.row(), 9), Qt::EditRole).isNull());
        data_polis.vs_num = model_polises.data(model_polises.index(index.row(), 9), Qt::EditRole).toString();
        ui->line_vs_num->setText(data_polis.vs_num);
        ui->ch_pol_sernum->setChecked(!model_polises.data(model_polises.index(index.row(),11), Qt::EditRole).isNull());
        data_polis.pol_ser = model_polises.data(model_polises.index(index.row(),10), Qt::EditRole).toString();
        ui->line_pol_ser->setText(data_polis.pol_ser);
        data_polis.pol_num = model_polises.data(model_polises.index(index.row(),11), Qt::EditRole).toString();
        ui->line_pol_num->setText(data_polis.pol_num);
        ui->ch_uec_num->setChecked(!model_polises.data(model_polises.index(index.row(),12), Qt::EditRole).isNull());
        data_polis.uec_num = model_polises.data(model_polises.index(index.row(),12), Qt::EditRole).toString();
        ui->line_uec_num->setText(data_polis.uec_num);
        ui->ch_pol_enp->setChecked(!model_polises.data(model_polises.index(index.row(), 4), Qt::EditRole).isNull());
        data_polis.enp = model_polises.data(model_polises.index(index.row(), 4), Qt::EditRole).toString();
        ui->line_enp->setText(data_polis.enp);
        ui->ch_pol_datbegin->setChecked(!model_polises.data(model_polises.index(index.row(),13), Qt::EditRole).isNull());
        data_polis.date_begin = model_polises.data(model_polises.index(index.row(),13), Qt::EditRole).toDate();
        ui->date_pol_begin->setDate(data_polis.date_begin);
        ui->ch_pol_datend->setChecked(!model_polises.data(model_polises.index(index.row(),14), Qt::EditRole).isNull());
        data_polis.date_end = model_polises.data(model_polises.index(index.row(),14), Qt::EditRole).toDate();
        ui->date_pol_end->setDate(data_polis.date_end);
        ui->ch_pol_datstop->setChecked(!model_polises.data(model_polises.index(index.row(),15), Qt::EditRole).isNull());
        data_polis.date_stop = model_polises.data(model_polises.index(index.row(),15), Qt::EditRole).toDate();
        ui->date_pol_stop->setDate(data_polis.date_stop);
        data_polis.in_erp = model_polises.data(model_polises.index(index.row(),16), Qt::EditRole).toInt();
        ui->combo_pol_in_erp->setCurrentIndex(ui->combo_pol_in_erp->findData(data_polis.in_erp));

        this->setCursor(Qt::ArrowCursor);
    } else {
        QMessageBox::warning(this, "Ничего не выбрано!", "Ничего не выбрано!");
    }
}

void polices_wnd::refresh_polis_data() {
    ui->line_terr_ocato->setText(data_polis.ocato);
    ui->combo_terr_ocato_text->setCurrentIndex(ui->combo_terr_ocato_text->findData(data_polis.ocato));
    ui->line_ogrn_smo->setText(data_polis.ogrn_smo);
    ui->line_ogrn_smo_text->setText(data_app.smo_name);
    ui->combo_pol_v->setCurrentIndex(ui->combo_pol_v->findData(data_polis.pol_v));

    ui->ch_pol_sernum->setChecked(data_polis.pol_v==1 || data_polis.pol_v==3);
    ui->ch_vs_num->setChecked(data_polis.pol_v==2);
    ui->ch_pol_enp->setChecked(data_polis.pol_v>1);
    ui->ch_pol_datbegin->setChecked(data_polis.date_begin>QDate(1920,1,1));
    ui->ch_pol_datend->setChecked(data_polis.date_end>QDate(1920,1,1));
    ui->ch_pol_datstop->setChecked(data_polis.date_stop>QDate(1920,1,1));

    ui->line_pol_ser->setText(data_polis.pol_ser);
    ui->line_pol_num->setText(data_polis.pol_num);
    ui->line_vs_num->setText(data_polis.vs_num);
    ui->line_enp->setText(data_polis.enp);
    ui->date_pol_begin->setDate(data_polis.date_begin);
    ui->date_pol_end->setDate(data_polis.date_end);
    ui->date_pol_stop->setDate(data_polis.date_stop);
    ui->combo_pol_in_erp->setCurrentIndex(ui->combo_pol_in_erp->findData(data_polis.in_erp));

    ui->lab_order_num->setText(data_polis.order_num);
    ui->lab_order_date->setText(data_polis.order_date.toString("dd.MM.yyyy"));
}

void polices_wnd::refresh_vizit_method() {
    this->setCursor(Qt::WaitCursor);
    // обновление списка способов подачи заявления
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, '(' || code || ') ' || text "
                  " from public.spr_method s "
                  " order by code ; ";
    mySQL.exec(this, sql, QString("Справочник способов подачи заявления"), *query, true, db, data_app);
    ui->combo_vizit_pan_method->clear();
    ui->combo_vizit_pan_method_act->clear();
    ui->combo_vizit_pan_method_stop->clear();
    while (query->next()) {
        ui->combo_vizit_pan_method->addItem(query->value(1).toString(), query->value(0).toInt());
        ui->combo_vizit_pan_method_act->addItem(query->value(1).toString(), query->value(0).toInt());
        ui->combo_vizit_pan_method_stop->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_vizit_pan_method->setCurrentIndex(data_pers.rep_id>0 ? ui->combo_vizit_pan_method->findData(2) : 0);
    ui->combo_vizit_pan_method_act->setCurrentIndex(data_pers.rep_id>0 ? ui->combo_vizit_pan_method_act->findData(2) : 0);
    ui->combo_vizit_pan_method_stop->setCurrentIndex(data_pers.rep_id>0 ? ui->combo_vizit_pan_method_stop->findData(2) : 0);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void polices_wnd::refresh_reasons_smo() {
    this->setCursor(Qt::WaitCursor);
    // обновление списка причин подачи заявления
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, '(' || code || ') ' || text "
                  " from public.spr_rsmo s "
                  " order by code ; ";
    mySQL.exec(this, sql, QString("Справочник причин подачи заявления"), *query, true, db, data_app);
    ui->combo_vizit_pan_reason_smo->clear();
    while (query->next()) {
        ui->combo_vizit_pan_reason_smo->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_vizit_pan_reason_smo->setCurrentIndex(0);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void polices_wnd::refresh_reasons_polis() {
    this->setCursor(Qt::WaitCursor);
    // обновление списка причин выдачи полиса
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, '(' || code || ') ' || text "
                  " from public.spr_rpolis s "
                  " order by code ; ";
    mySQL.exec(this, sql, QString("Справочник причин выдачи полиса"), *query, true, db, data_app);
    ui->combo_vizit_pan_reason_polis->clear();
    while (query->next()) {
        ui->combo_vizit_pan_reason_polis->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_vizit_pan_reason_polis->setCurrentIndex(0);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void polices_wnd::refresh_form_polis() {
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

void polices_wnd::refresh_terr_ocato() {
    this->setCursor(Qt::WaitCursor);
    // обновление списка форм изготовления полиса
    QString sql = "select code_ocato, '(' || code_ocato || ') ' || subj_name "
                  " from public.spr_ocato s "
                  " order by subj_name ; ";
    QSqlQuery *query = new QSqlQuery(db);
    mySQL.exec(this, sql, QString("Обновление справочника ОКАТО регионов"), *query, true, db, data_app);
    ui->combo_terr_ocato_text->clear();
    while (query->next()) {
        ui->combo_terr_ocato_text->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_terr_ocato_text->setCurrentIndex(0);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void polices_wnd::refresh_pol_ves() {
    this->setCursor(Qt::WaitCursor);
    // обновление списка форм изготовления полиса
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, '(' || code || ') ' || text "
                  " from public.spr_f008 s "
                  " order by code ; ";
    mySQL.exec(this, sql, QString("Обновление справочника типов полисов"), *query, true, db, data_app);
    ui->combo_pol_v->clear();
    while (query->next()) {
        ui->combo_pol_v->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_pol_v->setCurrentIndex(0);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void polices_wnd::on_bn_now_vizit_clicked() {
    ui->date_vizit_pan->setDate(QDate::currentDate());
}

void polices_wnd::on_bn_now_stop_clicked() {
    ui->date_stoppolis->setDate(QDate::currentDate());
}

void polices_wnd::on_bn_close_clicked() {
    this->reject();
}

void polices_wnd::on_bn_vizit_back_clicked() {
    this->accept();
}

void polices_wnd::refresh_polises_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    // данные персоны
    ui->lab_fam->setText(data_pers.fam);
    ui->lab_im->setText(data_pers.im);
    ui->lab_ot->setText(data_pers.ot);
    ui->lab_datr->setText(data_pers.date_birth.toString("dd.MM.yyyy"));
    ui->lab_sex->setText( (data_pers.sex==1) ? "муж." : "жен." );

    QString polises_sql = "select p.id, vs.status, pol.status, p.ocato, p.enp, p.ogrn_smo, p.pol_v as f008_code, f8.text as f008_text, "
                          "       p.pol_type, p.vs_num, p.pol_ser, p.pol_num, p.uec_num, p.date_begin, p.date_end, p.date_stop, "
                          "       p.in_erp, p.order_num, p.order_date, p.f_polis, p.id_person, "
                          "       p._id_first_point, pt1.point_regnum, pt1.point_name, p._id_first_operator, op1.oper_fio, "
                          "       p._id_last_point, pt2.point_regnum, pt2.point_name, p._id_last_operator, op2.oper_fio, "
                          "       pol.id, vs.vs_num, "
                          "       p.dt_ins, p.dt_upd, p.tfoms_date, p.date_sms_pol, p.date_activate, p.date_get2hand, "
                          "       p.tfoms_id, p.tfoms_date, p.r_smo, p.r_polis "
                          " from polises p "
                          "      left join spr_f008 f8 on(p.pol_v=f8.code) "
                          "      left join blanks_vs vs on(vs.vs_num=p.vs_num) "
                          "      left join blanks_pol pol on(pol.pol_ser=p.pol_ser and pol.pol_num=p.pol_num) "
                          "      left join points pt1 on(pt1.id=p._id_first_point) "
                          "      left join operators op1 on(op1.id=p._id_first_operator) "
                          "      left join points pt2 on(pt2.id=p._id_last_point) "
                          "      left join operators op2 on(op2.id=p._id_last_operator) "
                          " where p.id_person=" + QString::number(data_pers.id);
    polises_sql += " order by p.id desc ; ";


    model_polises.setQuery(polises_sql,db);
    QString err = model_polises.lastError().driverText();

    // подключаем модель из БД
    ui->tab_polises->setModel(&model_polises);

    // обновляем таблицу
    ui->tab_polises->reset();

    // задаём ширину колонок
    ui->tab_polises->setColumnWidth( 0,  1);    // p.id
    ui->tab_polises->setColumnWidth( 1, 30);    // vs.status
    ui->tab_polises->setColumnWidth( 2, 30);    // pol.status
    ui->tab_polises->setColumnWidth( 3, 50);    // p.ocato
    ui->tab_polises->setColumnWidth( 4,120);    // p.enp
    ui->tab_polises->setColumnWidth( 5, 50);    // p.ogrn_smo
    ui->tab_polises->setColumnWidth( 6,  1);    // p.pol_v as f008_code
    ui->tab_polises->setColumnWidth( 7,160);    // f8.text as f008_text
    ui->tab_polises->setColumnWidth( 8,  1);    // p.pol_type
    ui->tab_polises->setColumnWidth( 9, 70);    // p.vs_num
    ui->tab_polises->setColumnWidth(10, 50);    // p.pol_ser
    ui->tab_polises->setColumnWidth(11, 60);    // p.pol_num
    ui->tab_polises->setColumnWidth(12, 70);    // p.uec_num
    ui->tab_polises->setColumnWidth(13, 75);    // p.date_begin
    ui->tab_polises->setColumnWidth(14, 75);    // p.date_end
    ui->tab_polises->setColumnWidth(15, 75);    // p.date_stop
    ui->tab_polises->setColumnWidth(16, 80);    // p.in_erp
    ui->tab_polises->setColumnWidth(17,120);    // p.order_num
    ui->tab_polises->setColumnWidth(18,100);    // p.order_date
    ui->tab_polises->setColumnWidth(19, 55);    // p.f_polis
    ui->tab_polises->setColumnWidth(20,  1);    // p.id_person
    ui->tab_polises->setColumnWidth(21,  1);    // p._id_last_point,
    ui->tab_polises->setColumnWidth(22,  1);    // points.point_regnum,
    ui->tab_polises->setColumnWidth(23,150);    // points.point_name,
    ui->tab_polises->setColumnWidth(24,  1);    // p._id_last_operator,
    ui->tab_polises->setColumnWidth(25, 80);    // operators.oper_fio
    ui->tab_polises->setColumnWidth(26,  1);    // pol.id
    ui->tab_polises->setColumnWidth(27,  1);    // vs.id
    ui->tab_polises->setColumnWidth(28,  1);    //
    ui->tab_polises->setColumnWidth(29,  1);    //
    ui->tab_polises->setColumnWidth(30, 85);    // tfoms_date
    ui->tab_polises->setColumnWidth(31, 85);    // date_sms_pol
    ui->tab_polises->setColumnWidth(32, 85);    // date_activate
    ui->tab_polises->setColumnWidth(33, 85);    // date_get2hand

    // правим заголовки
    model_polises.setHeaderData( 1, Qt::Horizontal, ("ВС"));
    model_polises.setHeaderData( 2, Qt::Horizontal, ("П"));
    model_polises.setHeaderData( 3, Qt::Horizontal, ("ОКАТО"));
    model_polises.setHeaderData( 4, Qt::Horizontal, ("ЕНП"));
    model_polises.setHeaderData( 5, Qt::Horizontal, ("ОГРН\nСМО"));
    model_polises.setHeaderData( 7, Qt::Horizontal, ("форма\nвыпуска"));
    model_polises.setHeaderData( 8, Qt::Horizontal, ("тип\nполиса"));
    model_polises.setHeaderData( 9, Qt::Horizontal, ("№ ВС"));
    model_polises.setHeaderData(10, Qt::Horizontal, ("серия"));
    model_polises.setHeaderData(11, Qt::Horizontal, ("номер"));
    model_polises.setHeaderData(12, Qt::Horizontal, ("№ УЭК"));
    model_polises.setHeaderData(13, Qt::Horizontal, ("дата\nвыдачи"));
    model_polises.setHeaderData(14, Qt::Horizontal, ("дата планов.\nокончания"));
    model_polises.setHeaderData(15, Qt::Horizontal, ("дата\nизъятия"));
    model_polises.setHeaderData(16, Qt::Horizontal, ("активация\nв ЕРП?"));
    model_polises.setHeaderData(17, Qt::Horizontal, ("номер заявки на\nизгот. полиса"));
    model_polises.setHeaderData(18, Qt::Horizontal, ("дата заявки на\nизгот. полиса"));
    model_polises.setHeaderData(19, Qt::Horizontal, ("форма\nпол."));
    model_polises.setHeaderData(23, Qt::Horizontal, ("ПВП"));
    model_polises.setHeaderData(25, Qt::Horizontal, ("ФИО\nинспектора"));
    model_polises.setHeaderData(26, Qt::Horizontal, ("ID\nЕНП"));
    model_polises.setHeaderData(27, Qt::Horizontal, ("ID\nВС"));
    model_polises.setHeaderData(30, Qt::Horizontal, ("дата заявки\nна бланк ЕНП"));
    model_polises.setHeaderData(31, Qt::Horizontal, ("дата СМС\nо готовн.ЕНП"));
    model_polises.setHeaderData(32, Qt::Horizontal, ("дата\nактивации"));
    model_polises.setHeaderData(33, Qt::Horizontal, ("выдача\nна руки"));
    ui->tab_polises->repaint();

    //refresh_assigs_tab();
    //refresh_links_tab();
    //refresh_addresses_tab();
    this->setCursor(Qt::ArrowCursor);
}

void polices_wnd::on_bn_refresh_polises_clicked() {
    refresh_polises_tab();
}

void polices_wnd::on_combo_terr_ocato_text_activated(int index) {
    ui->line_terr_ocato->setText(ui->combo_terr_ocato_text->currentData().toString());
}

void polices_wnd::on_line_vs_num_selectionChanged() {
    on_line_vs_num_textChanged(ui->line_vs_num->text());
}

void polices_wnd::on_line_pol_ser_textChanged(const QString &arg1) {
    ui->line_pol_ser->setText(ui->line_pol_ser->text().trimmed().replace(" ", ""));
    ui->line_pol_ser->setCursorPosition(ui->line_pol_ser->text().size());
}

void polices_wnd::on_line_pol_num_textChanged(const QString &arg1) {
    ui->line_pol_num->setText(ui->line_pol_num->text().trimmed().replace(" ", ""));
    ui->line_pol_num->setCursorPosition(ui->line_pol_num->text().size());
}

void polices_wnd::on_line_vs_num_textChanged(const QString &arg1) {
    ui->line_vs_num->setText(ui->line_vs_num->text().trimmed().replace(" ", ""));
    ui->line_vs_num->setCursorPosition(ui->line_vs_num->text().size());
}

void polices_wnd::on_line_enp_textChanged(const QString &arg1) {
    ui->line_enp->setText(ui->line_enp->text().trimmed().replace(" ", ""));
    ui->line_enp->setCursorPosition(ui->line_enp->text().size());
}

void polices_wnd::on_ch_pol_sernum_stateChanged(int arg1) {
    ui->line_pol_ser->setEnabled(arg1>0);
    ui->line_pol_num->setEnabled(arg1>0);
    ui->lab_pol_num->setEnabled(arg1>0);
    ui->line_pol_ser->setVisible(arg1>0);
    ui->line_pol_num->setVisible(arg1>0);
    ui->lab_pol_num->setVisible(arg1>0);
}
void polices_wnd::on_ch_vs_num_stateChanged(int arg1) {
    ui->line_vs_num->setEnabled(arg1>0);
    ui->line_vs_num->setVisible(arg1>0);
}
void polices_wnd::on_ch_uec_num_stateChanged(int arg1) {
    ui->line_uec_num->setEnabled(arg1>0);
    ui->line_uec_num->setVisible(arg1>0);
}
void polices_wnd::on_ch_pol_enp_stateChanged(int arg1) {
    ui->line_enp->setEnabled(arg1>0);
    ui->line_enp->setVisible(arg1>0);
}
void polices_wnd::on_ch_pol_datbegin_stateChanged(int arg1) {
    ui->date_pol_begin->setEnabled(arg1>0 && (data_app.is_head || data_app.is_tech) );
    ui->date_pol_begin->setVisible(arg1>0);
}
void polices_wnd::on_ch_pol_datend_stateChanged(int arg1) {
    ui->date_pol_end->setEnabled(arg1>0 && (data_app.is_head || data_app.is_tech) );
    ui->date_pol_end->setVisible(arg1>0);
}
void polices_wnd::on_ch_pol_datstop_stateChanged(int arg1) {
    ui->date_pol_stop->setEnabled(arg1>0 && (data_app.is_head || data_app.is_tech) );
    ui->date_pol_stop->setVisible(arg1>0);
}

void polices_wnd::on_bn_polis_back_clicked() {
    //on_bn_polis_save_clicked();
    //if (!data_polis.is_ok)  return;

    on_show_pan_vizit_clicked(false);
    on_show_pan_polises_clicked(true);
    on_show_pan_polis_clicked(false);

    /*
    ui->show_pan_polises->setEnabled(true);
    ui->show_pan_polis->setEnabled(false);*/

    refresh_polises_tab();
}

void polices_wnd::test_for_pers_categ() {
    //QMessageBox::information(this, "Категория", QString::number(data_pers.category) + "   eventcode = " + data_event.event_code);

    if ( data_pers.category==2
         || data_pers.category==3
         || data_pers.category==4
         || data_pers.category==5
         || data_pers.category==7
         || data_pers.category==8
         || data_pers.category==9
         || data_pers.category==10
         || data_pers.category==11
         || data_pers.category==12
         || data_pers.category==13
         || data_pers.category==14 ) {

        // для новых временных свидетельств
        if ( data_event.event_code=="П010"
             || data_event.event_code=="П034"
             || data_event.event_code=="П035"
             || data_event.event_code=="П036"
             || data_event.event_code=="П061"
             || data_event.event_code=="П062"
             || data_event.event_code=="П063" ) {

            // форма выпуска полиса  -  бумажный полис
            ui->combo_vizit_pan_form_polis->setCurrentIndex(1);
            data_vizit.f_polis = 1;

            // ограничение срока действия полиса по конец этого года
            QDate date_end = QDate(QDate::currentDate().year(), 12, 31);
            if (QDate::currentDate()>=QDate(QDate::currentDate().year(),11,20)) {
                // начиная с 20 ноября, ограничение - 31 декабря СЛЕДУЮЩЕГО года
                date_end = QDate(QDate::currentDate().year() +1, 12, 31);
            }
            if ( (!data_polis.has_date_end) || (data_polis.date_end<QDate(1920,1,1)) || (data_polis.date_end>date_end) ) {
                QMessageBox::information(this,
                                         "Внимание",
                                         "Для данной категории застрахованного срок действия полиса \n"
                                         "будет автоматически ограничен по конец текущего года, \n"
                                         "а форма выпуска полиса задана - \"бумажный полис\".");
                ui->ch_pol_datend->setEnabled(true);
                ui->ch_pol_datend->setChecked(true);
                ui->date_pol_end->setVisible(true);
                ui->date_pol_end->setDate(date_end);
                data_polis.has_date_end = true;
                data_polis.date_end = date_end;
            }

            if ( (!data_polis.has_date_stop) || (data_polis.date_stop<QDate(1920,1,1)) || (data_polis.date_stop>date_end) ) {
                QMessageBox::information(this,
                                         "Внимание",
                                         "Для данной категории застрахованного плановая дата изъятия \n"
                                         "полиса будет автоматически ограничена по конец текущего года, \n"
                                         "а форма выпуска полиса задана - \"бумажный полис\".");
                ui->ch_pol_datstop->setEnabled(true);
                ui->ch_pol_datstop->setChecked(true);
                ui->date_pol_stop->setVisible(true);
                ui->date_pol_stop->setDate(date_end);
                data_polis.has_date_stop = true;
                data_polis.date_stop = date_end;
            }
        }
        else

        // для новых полисов единого образца
        if ( data_event.event_code=="П060"    /*
             || data_event.event_code=="П031"
             || data_event.event_code=="П032"
             || data_event.event_code=="П033" */ ) {

            // снимаю это ограничение, посколькоу прошло уже почти пол года
            // и все ВС, выданные до 01.01.2017 должны быть заменены на
            // полисы единого образца ...
            // ... хотя я не уверен ...
            //
            // if (data_pers._date_insure_begin<QDate(2017,1,1))
            //    return;     // только после 01.01.2017

            // форма выпуска полиса  -  бумажный полис
            ui->combo_vizit_pan_form_polis->setCurrentIndex(1);
            data_vizit.f_polis = 1;

            // ограничение срока действия полиса по конец этого года
            QDate date_end = QDate(QDate::currentDate().year(), 12, 31);
            if (QDate::currentDate()>=QDate(QDate::currentDate().year(),11,20)) {
                // начиная с 20 ноября, ограничение - 31 декабря СЛЕДУЮЩЕГО года
                date_end = QDate(QDate::currentDate().year() +1, 12, 31);
            }
            if ( (!data_polis.has_date_end) || (data_polis.date_end<QDate(1920,1,1)) || (data_polis.date_end>date_end) ) {
                QMessageBox::information(this,
                                         "Внимание",
                                         "Для данной категории застрахованного срок действия полиса \n"
                                         "будет автоматически ограничен по конец текущего года, \n"
                                         "а форма выпуска полиса задана - \"бумажный полис\".");
                ui->ch_pol_datend->setEnabled(true);
                ui->ch_pol_datend->setChecked(true);
                ui->date_pol_end->setVisible(true);
                ui->date_pol_end->setDate(date_end);
                data_polis.has_date_end = true;
                data_polis.date_end = date_end;
            }

            if ( (!data_polis.has_date_stop) || (data_polis.date_stop<QDate(1920,1,1)) || (data_polis.date_stop>date_end) ) {
                QMessageBox::information(this,
                                         "Внимание",
                                         "Для данной категории застрахованного плановая дата изъятия \n"
                                         "полиса будет автоматически ограничена по конец текущего года, \n"
                                         "а форма выпуска полиса задана - \"бумажный полис\".");
                ui->ch_pol_datstop->setEnabled(true);
                ui->ch_pol_datstop->setChecked(true);
                ui->date_pol_stop->setVisible(true);
                ui->date_pol_stop->setDate(date_end);
                data_polis.has_date_stop = true;
                data_polis.date_stop = date_end;
            }
        }
    }
}

bool polices_wnd::test_for_dead() {
    // проверка, не умер ли застрахованный
    QString sql_dead = "";
    sql_dead += "select e.date_death "       // контрольный запрос - дата смерти
                "  from persons e "
                " where e.id=" + QString::number(data_pers.id) + " ; ";

    QSqlQuery *query_dead = new QSqlQuery(db);
    bool res_dead = mySQL.exec(this, sql_dead, QString("Проверка живучести застрахованного"), *query_dead, true, db, data_app);
    if (res_dead && query_dead->next()) {
        // проверка, не умер ли застрахованный
        QDate date_death = query_dead->value(14).toDate();
        if (!query_dead->value(14).isNull()) {
            // застрахованный уже умер X((
            QMessageBox::warning(this, "Он уже умер  X(_",
                                 "Этот застрахованный уже умер.\n\n"
                                 "Дата смерти  " + date_death.toString("yyyy-MM-dd"));
            data_event.is_ok = false;
            delete query_dead;
            return false;
        }
    }
    return true;
}

void polices_wnd::test_for_udl_exp() {
    //проверка УДЛ на ограничение срока действия
    if ( data_pers.udl_date_exp_f ) {
        if ( data_event.event_code=="П010" ||
             data_event.event_code=="П034" ||
             data_event.event_code=="П035" ||
             data_event.event_code=="П036" ||
             data_event.event_code=="П061" ||
             data_event.event_code=="П062" ||
             data_event.event_code=="П063" ||

             data_event.event_code=="П031" ||
             data_event.event_code=="П032" ||
             data_event.event_code=="П033" ||
             data_event.event_code=="П060" ) {

            if ( (!data_polis.has_date_stop) || (ui->date_pol_stop->date()>data_pers.udl_date_exp) ) {
                // для ВС ограничение срока действия пишется в date_stop
                QMessageBox::information(this,
                                         "Внимание",
                                         "Срок действия ВС будет автоматически ограничен по сроку \n"
                                         "действия УДЛ застрахованного.");
                ui->ch_pol_datstop->setEnabled(true);
                ui->date_pol_stop->setVisible(true);
                ui->ch_pol_datstop->setChecked(data_polis.date_stop>QDate(1920,1,1));
                ui->date_pol_stop->setDate(data_pers.udl_date_exp);
                data_polis.has_date_stop = data_pers.udl_date_exp_f;
                data_polis.date_stop = data_pers.udl_date_exp;

                // если форма выпуска полиса не задана - поставим бумажный полис
                if (ui->combo_vizit_pan_form_polis->currentIndex()==0) {
                    ui->combo_vizit_pan_form_polis->setCurrentIndex(1);
                    data_vizit.f_polis = 1;
                }
            }
        } else
        if ( data_event.event_code=="П031" ||
             data_event.event_code=="П032" ||
             data_event.event_code=="П033" ||
             data_event.event_code=="П060" ||

             data_event.event_code=="П010" ||
             data_event.event_code=="П034" ||
             data_event.event_code=="П035" ||
             data_event.event_code=="П036" ||
             data_event.event_code=="П061" ||
             data_event.event_code=="П062" ||
             data_event.event_code=="П063" ) {

            if ( (!data_polis.has_date_end) || (ui->date_pol_end->date()>data_pers.udl_date_exp) ) {
                // для полиса единого образца ограничение срока действия пишется в date_end
                QMessageBox::information(this,
                                         "Внимание",
                                         "Срок действия полиса единого образца будет автоматически ограничен \n"
                                         "по сроку действия УДЛ застрахованного.");
                ui->ch_pol_datstop->setEnabled(false);
                ui->date_pol_stop->setVisible(false);
                ui->ch_pol_datstop->setChecked(data_polis.date_stop>QDate(1920,1,1));
                ui->ch_pol_datend->setChecked(true);
                ui->ch_pol_datend->setEnabled(true);
                ui->date_pol_end->setVisible(true);
                ui->date_pol_end->setDate(data_pers.udl_date_exp);
                data_polis.has_date_end = data_pers.udl_date_exp_f;
                data_polis.date_end = data_pers.udl_date_exp;
            }
        }
    }
    //проверка ДРП на ограничение срока действия
    if ( data_pers.drp_date_exp_f ) {
        if ( data_event.event_code=="П010" ||
             data_event.event_code=="П034" ||
             data_event.event_code=="П035" ||
             data_event.event_code=="П036" ||
             data_event.event_code=="П061" ||
             data_event.event_code=="П062" ||
             data_event.event_code=="П063" ||

             data_event.event_code=="П031" ||
             data_event.event_code=="П032" ||
             data_event.event_code=="П033" ||
             data_event.event_code=="П060" ) {

            if ( (!data_polis.has_date_stop) || (ui->date_pol_stop->date()>data_pers.drp_date_exp) ) {
                // для ВС ограничение срока действия пишется в date_stop
                QMessageBox::information(this,
                                         "Внимание",
                                         "Срок действия ВС будет автоматически ограничен по сроку \n"
                                         "действия РВП застрахованного.");
                ui->ch_pol_datstop->setEnabled(true);
                ui->date_pol_stop->setVisible(true);
                ui->ch_pol_datstop->setChecked(data_polis.date_stop>QDate(1920,1,1));
                ui->date_pol_stop->setDate(data_pers.drp_date_exp);
                data_polis.has_date_stop = data_pers.drp_date_exp_f;
                data_polis.date_stop = data_pers.drp_date_exp;

                // если форма выпуска полиса не задана - поставим бумажный полис
                if (ui->combo_vizit_pan_form_polis->currentIndex()==0) {
                    ui->combo_vizit_pan_form_polis->setCurrentIndex(1);
                    data_vizit.f_polis = 1;
                }
            }
        } else
        if ( data_event.event_code=="П031" ||
             data_event.event_code=="П032" ||
             data_event.event_code=="П033" ||
             data_event.event_code=="П060" ||

             data_event.event_code=="П010" ||
             data_event.event_code=="П034" ||
             data_event.event_code=="П035" ||
             data_event.event_code=="П036" ||
             data_event.event_code=="П061" ||
             data_event.event_code=="П062" ||
             data_event.event_code=="П063" ) {

            if ( (!data_polis.has_date_end) || (ui->date_pol_end->date()>data_pers.drp_date_exp) ) {
                // для полиса единого образца ограничение срока действия пишется в date_end
                QMessageBox::information(this,
                                         "Внимание",
                                         "Срок действия полиса единого образца будет автоматически ограничен \n"
                                         "по сроку действия РВП застрахованного.");
                ui->ch_pol_datstop->setEnabled(false);
                ui->date_pol_stop->setVisible(false);
                ui->ch_pol_datstop->setChecked(false);
                ui->ch_pol_datend->setChecked(true);
                ui->ch_pol_datend->setEnabled(true);
                ui->date_pol_end->setVisible(true);
                ui->date_pol_end->setDate(data_pers.drp_date_exp);
                data_polis.has_date_end = data_pers.drp_date_exp_f;
                data_polis.date_end = data_pers.drp_date_exp;
            }
        }
    }

    // для иностранцев - дата изъятия полиса ограничивается по сроку действия ДРП
    if ( ( data_pers.category==3
           || data_pers.category==4
           || data_pers.category==5
           || data_pers.category==8
           || data_pers.category==10 )
         && (data_pers.drp_date_exp<data_polis.date_stop) ) {

        ui->ch_pol_datstop->setEnabled(false);
        ui->ch_pol_datstop->setChecked(true);

        if ( data_pers.category==5
             || data_pers.category==10 ) {
            if ( !data_pers.udl_date_exp_f
                 && data_pers.udl_date_exp>QDate(1920,1,1) ) {
                QMessageBox::warning(this, "Не задан срок действия ВУ!",
                                     "Не задан срок действия Разрешения на временное убежище!\n"
                                     "Для застрахованных этой категории обязательно надо задать плановую дату изъятия полиса единого образца, "
                                     "равную дате окончания действия Разрешения на временное убежище.\n\n"
                                     "  -  вернитесь на окно данных персоны \n"
                                     "  -  или задайте плановую дату изъятия полиса вручную\n");
                ui->date_pol_stop->setDate(QDate::currentDate());
                data_polis.date_stop = QDate::currentDate();
            } else {
                ui->date_pol_stop->setDate(data_pers.udl_date_exp);
                data_polis.date_stop = data_pers.udl_date_exp;
                QMessageBox::information(this,
                                         "Внимание",
                                         "Срок действия полиса будет автоматически ограничен \n"
                                         "по сроку действия ДРП застрахованного.");
            }
        } else if ( data_pers.category==3
                    || data_pers.category==4
                    || data_pers.category==8 ) {
            if ( !data_pers.drp_date_exp_f
                 && data_pers.drp_date_exp>QDate(1920,1,1) ) {
                QMessageBox::warning(this, "Не задан срок действия ДРП!",
                                     "Не задан срок действия документа, разрешающего проживание в РФ!\n"
                                     "Для застрахованных этой категории обязательно надо задать плановую дату изъятия полиса единого образца, "
                                     "равную дате окончания действия документа, разрешающего проживание в РФ.\n\n"
                                     "  -  вернитесь на окно данных персоны \n"
                                     "  -  или задайте плановую дату изъятия полиса вручную\n");
                ui->date_pol_stop->setDate(QDate::currentDate());
                data_polis.date_stop = QDate::currentDate();
            } else {
                ui->date_pol_stop->setDate(data_pers.drp_date_exp);
                data_polis.date_stop = data_pers.drp_date_exp;
                QMessageBox::information(this,
                                         "Внимание",
                                         "Срок действия полиса будет автоматически ограничен \n"
                                         "по сроку действия ДРП застрахованного.");
            }
        }
    }

    return;
}

void polices_wnd::on_bn_polis_save_clicked() {
    if (!ui->bn_polis_save->isVisible()) {
        QMessageBox::information(this, "Нечего сохранять", "Нечего сохранять");
        return;
    }

    if ( data_event.event_code=="П031"
         || data_event.event_code=="П032"
         || data_event.event_code=="П033" ) {
        // полис выпускать не надо - форму выпуска полиса не проверяем
    }  else {
        // проверим, указана ли форма изготовления полиса
        if (ui->combo_vizit_pan_form_polis->currentData().toInt()<1) {
            // форма полиса должна быть заполнена в любом случае
            QMessageBox::warning(this,
                                 "Не хватает данных!",
                                 "Не указана форма изготовления бланка полиса.\n\n"
                                 "  1) Для В.С. должна быть выбрана форма заказанного бланка полиса единого образца.\n"
                                 "  2) Для полис единого образца должна быть указана его форма.\n"
                                 "  3) Для перестрахованного лица должна быть указана форма полиса другой СМО, вносимого в нашу базу полисов.\n");
            data_polis.is_ok = false;
            return;
        }
        // проверим соответствие серии форме полиса
        if (ui->ch_pol_sernum->isChecked()) {
            if (ui->line_pol_ser->text().left(ui->line_pol_ser->text().size()-2).toInt() != ui->combo_vizit_pan_form_polis->currentData().toInt()){
                QMessageBox::warning(this, "Ошибка в данных!", "Введённая серия полиса не соответствует выбранной форме выпуска полиса.");
                return;
            }
        }
    }

    test_for_udl_exp();
    test_for_pers_categ();

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
    }

    if (ui->ch_pol_datstop->isChecked() && ui->date_pol_stop->date()<=QDate::currentDate()) {
        if (QMessageBox::question(this, "Нужно подтверждение!", "Введённая дата \"Срок действия УДЛ\" уже прошла.\n\n"
                                                                /*"Оставить так?",
                                                                QMessageBox::Yes|QMessageBox::Cancel,
                                                                QMessageBox::Cancel)==QMessageBox::Cancel*/)) {
            data_polis.is_ok = false;
            return;
        }
    }

    if (ui->combo_pol_v->currentData().toInt()==2) {
        // проверка заполнения полей
        if (ui->line_vs_num->text().isEmpty()) {
            QMessageBox::warning(this, "Не хватает данных!", "Не введён номер временного свидетельства.");
            data_polis.is_ok = false;
            return;
        }
        if (ui->ch_pol_enp->isChecked() && !(ui->line_enp->text().isEmpty() || ui->line_enp->text().length()==16)) {
            QMessageBox::warning(this, "Неверные данные!", "ЕНП (единый номер полиса) должен иметь длину 16 цифр!");
            data_polis.is_ok = false;
            return;
        }
        if (data_pers.category==3 && (!(ui->ch_pol_datstop->isChecked()) || ui->date_pol_stop->date()<QDate(1920,01,01)))  {
            QMessageBox::warning(this, "Не хватает данных!", "Для данной категории застрахованных \n"
                                                             "(Работающий временно проживающий в Российской Федерации иностранный гражданин)\n"
                                                             "надо указывать срок действия УДЛ!\n");
            data_polis.is_ok = false;
            return;
        }
        if (data_pers.category==5 && (!(ui->ch_pol_datstop->isChecked()) || ui->date_pol_stop->date()<QDate(1920,01,01))) {
            QMessageBox::warning(this, "Не хватает данных!", "Для данной категории застрахованных \n"
                                                             "(Работающее лицо, имеющее право на медицинскую помощь в соответствии с Федеральным законом О беженцах)\n"
                                                             "надо указывать срок действия УДЛ!");
            data_polis.is_ok = false;
            return;
        }
        if (data_pers.category==8 && (!(ui->ch_pol_datstop->isChecked()) || ui->date_pol_stop->date()<QDate(1920,01,01))) {
            QMessageBox::warning(this, "Не хватает данных!", "Для данной категории застрахованных \n"
                                                             "(Неработающий временно проживающий в Российской Федерации иностранный гражданин)\n"
                                                             "надо указывать срок действия УДЛ!");
            data_polis.is_ok = false;
            return;
        }
        if (data_pers.category==10 && (!(ui->ch_pol_datstop->isChecked()) || ui->date_pol_stop->date()<QDate(1920,01,01))) {
            QMessageBox::warning(this, "Не хватает данных!", "Для данной категории застрахованных \n"
                                                             "(Неработающее лицо, имеющее право на медицинскую помощь в соответствии с Федеральным законом О беженцах)\n"
                                                             "надо указывать срок действия УДЛ!");
            data_polis.is_ok = false;
            return;
        }
        /*if (ui->line_order_num->text().isEmpty()) {
            QMessageBox::warning(this, "Не хватает данных!", "Не введён номер заявки на изготовление полиса.");
            data_polis.is_ok = false;
            return;
        }
        if (ui->date_order_date->date()!=QDate::currentDate() && QMessageBox::warning(this, "Нужно подтверждение", "Дата заявки на изготовление полиса не совпадает с текущей датой. \n\nПринять?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::No) {
            data_polis.is_ok = false;
            return;
        }*/
        if (ui->combo_vizit_pan_form_polis->currentData().toInt()==0) {
            if (QMessageBox::question(this, "Нужно подтверждение!", "Для полиса единого образца не указана форма изготовления полиса.\n"
                                                                    "Это допустимо, если в нашу базу полисов будет внесён полис другой страховой компании и выпускать новый полис не требуется.\n\n"
                                                                    "Оставить так?",
                                                                    QMessageBox::Yes|QMessageBox::Cancel,
                                                                    QMessageBox::Cancel)==QMessageBox::Cancel) {
                data_polis.is_ok = false;
                return;
            }
        }
    } else if (ui->combo_pol_v->currentData().toInt()==3) {
        // проверка заполнения полей
        if ( !(ui->ch_pol_sernum->isChecked()) ) {
            QMessageBox::warning(this, "Не хватает данных!", "Не введена серия и номер полиса единого образца.");
            data_polis.is_ok = false;
            return;
        }
        if (ui->line_pol_ser->text().isEmpty()) {
            QMessageBox::warning(this, "Не хватает данных!", "Не введена серия полиса единого образца.");
            data_polis.is_ok = false;
            return;
        }
        if (ui->line_pol_num->text().isEmpty()) {
            QMessageBox::warning(this, "Не хватает данных!", "Не введён номер полиса единого образца.");
            data_polis.is_ok = false;
            return;
        }
        if (ui->ch_pol_enp->isChecked() && !(ui->line_enp->text().trimmed().isEmpty() || ui->line_enp->text().length()==16)) {
            QMessageBox::warning(this, "Неверные данные!", "ЕНП (единый номер полиса) должен иметь длину 16 цифр!");
            data_polis.is_ok = false;
            return;
        }
    }

    if (data_vizit.r_smo<=1 && data_vizit.r_polis<=0) {
        // получим причину смены СМО/выдачи полиса из записи последнего ВС
        QString sql_rsmo = "select p.r_smo, p.r_polis "
                           "  from polises p "
                           " where p.id=(select max(pp.id) from polises pp where id_person=" + QString::number(data_pers.id) + " and pol_v=2) ; ";
        QSqlQuery *query_rsmo = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_rsmo, "получим причину смены СМО/выдачи полиса из записи последнего ВС", *query_rsmo, true, db, data_app)) {
            delete query_rsmo;
            return;
        }
        data_vizit.r_smo   = query_rsmo->value(0).toInt();
        data_vizit.r_polis = query_rsmo->value(1).toInt();
        delete query_rsmo;
    }

    // запомним выбранную форму полиса в данных визита
    data_vizit.f_polis = ui->combo_vizit_pan_form_polis->currentData().toInt();
    QString sql_fpolis
            = "update vizits "
              "       set r_smo="   +QString::number(data_vizit.r_smo)+ ", "
              "           r_polis=" +QString::number(data_vizit.r_polis)+ ", "
              "           f_polis=" +QString::number(data_vizit.f_polis)+ " "
              " where id=" +QString::number(data_vizit.id)+ " ; ";

    QSqlQuery *query_fpolis = new QSqlQuery(db);
    bool res_fpolis = mySQL.exec(this, sql_fpolis, QString("Запись выбранной формы изготовления полиса в данные визита"), *query_fpolis, true, db, data_app);
    if (!res_fpolis) {
        QMessageBox::critical(this,
                              "Операция не выполнена!",
                              "При попытке запомнить выбранную форму изготовления полиса произошла ошибка - Операция добавления записи была отменена.");
        delete query_fpolis;
        return;
    }
    delete query_fpolis;


    // запомним данные полиса
    data_polis.ocato = ui->line_terr_ocato->text();
    data_polis.ogrn_smo = ui->line_ogrn_smo->text();
    data_polis.pol_v  = ui->combo_pol_v->currentData().toInt();

    data_polis.has_enp = ui->ch_pol_enp->isChecked();
    data_polis.has_vs_num = ui->ch_vs_num->isChecked();
    data_polis.vs_num = ui->line_vs_num->text();
    data_polis.has_pol_sernum = ui->ch_pol_sernum->isChecked();
    data_polis.enp = ui->line_enp->text().trimmed();
    data_polis.pol_ser  = ui->line_pol_ser->text();
    data_polis.pol_num  = ui->line_pol_num->text();
    data_polis.has_uec_num = ui->ch_uec_num->isChecked();
    data_polis.uec_num = ui->line_uec_num->text();

    data_polis.has_date_begin = ui->ch_pol_datbegin->isChecked();
    data_polis.has_date_end = ui->ch_pol_datend->isChecked();
    data_polis.has_date_stop = ui->ch_pol_datstop->isChecked();
    data_polis.date_begin = ui->date_pol_begin->date();
    data_polis.date_end   = ui->date_pol_end->date();
    data_polis.date_stop  = ui->date_pol_stop->date();

    data_polis.in_erp = ui->combo_pol_in_erp->currentData().toInt();
    //data_polis.order_num = ui->line_order_num->text();
    //data_polis.order_date = ui->date_order_date->date();

    data_polis.is_ok   = true;
    data_polis.r_smo   = data_vizit.r_smo;
    data_polis.r_polis = data_vizit.r_polis;
    data_polis.f_polis = data_vizit.f_polis;
    db.transaction();

    bool res_process = true;
    data_event.id_polis = data_polis.id;
    if (data_event.event_code=="П010") {
        res_process = process_P010(data_polis, data_event);
    } else if ( (data_event.event_code=="П031") || (data_event.event_code=="П032") || (data_event.event_code=="П033") ) {
        res_process = process_P031_P032_P033(data_polis, data_event);
    } else if (data_event.event_code=="П034") {
        res_process = process_P034_P035_P036(data_polis, data_event);
    } else if (data_event.event_code=="П035") {
        res_process = process_P034_P035_P036(data_polis, data_event);
    } else if (data_event.event_code=="П036") {
        res_process = process_P034_P035_P036(data_polis, data_event);
    } else if (data_event.event_code=="П060") {
        res_process = process_P060(data_polis, data_event);
    } else if (data_event.event_code=="П061") {
        res_process = process_P061_P062_P063(data_polis, data_event);
    } else if (data_event.event_code=="П062") {
        res_process = process_P061_P062_P063(data_polis, data_event);
    } else if (data_event.event_code=="П063") {
        res_process = process_P061_P062_P063(data_polis, data_event);
    } else {
        // надо просто запомнить новые данные полиса
        //res_process = process_P040(data_polis, data_event);
    }

    if (!res_process) {
        db.rollback();
    } else {
        db.commit();
        b_need_save (false);
        QMessageBox::information(this, "Запись сохранена", "Запись сохранена");
    }
    b_need_save(false);
    refresh_polises_tab();

    // обновим связку персоны с полисом
    QString sql_link = "select * from update_pers_links(" + QString::number(data_pers.id) + ", NULL) ; ";
    QSqlQuery *query_link = new QSqlQuery(db);
    if (!mySQL.exec(this, sql_link, "Обновление связки с полисом застрахованного", *query_link, true, db, data_app)) {
        delete query_link;
        return;
    }
    delete query_link;

    // посчитаем число полисов и получим последнее круглое значение
    QString sql_cntr = "select 'sum', count(*) "
                       "  from persons "
                       " where status>0 "
                       "union  "
                       "select 'match', coalesce(cast(value as int), 0) "
                       "  from db_data "
                       " where name='last_match' ";
    QSqlQuery *query_cntr = new QSqlQuery(db);
    if (!mySQL.exec(this, sql_cntr, "Проверка на \"Крутое\" число", *query_cntr, true, db, data_app))
        return;
    int cnt_cur = 0;
    int cnt_mth = 0;
    while (query_cntr->next()) {
        QString name = query_cntr->value(0).toString();
        if (name=="sum")
            cnt_cur = query_cntr->value(1).toInt();
        if (name=="match")
            cnt_mth = query_cntr->value(1).toInt();
    }
    if (cnt_cur>cnt_mth) {
        if ( cnt_cur==1 ) {
            QString sql_save = "delete from db_data  where name='last_match' ; "
                               "insert into db_data(name, value) values('last_match', '" + QString::number(cnt_cur) + "') ; ";
            QSqlQuery *query_save = new QSqlQuery(db);
            mySQL.exec(this, sql_save, "Запомним \"Крутое\" число", *query_save, true, db, data_app);
            delete query_save;
            QMessageBox::information(this, "УРА!!!",
                                      "Это - ПЕРВЫЙ застрахованный !");
        } else if (cnt_cur==100 ) {
            QString sql_save = "delete from db_data  where name='last_match' ; "
                               "insert into db_data(name, value) values('last_match', '" + QString::number(cnt_cur) + "') ; ";
            QSqlQuery *query_save = new QSqlQuery(db);
            mySQL.exec(this, sql_save, "Запомним \"Крутое\" число", *query_save, true, db, data_app);
            delete query_save;
            QMessageBox::information(this, "УРА!!!",
                                     "Это - СОТЫЙ застрахованный !");
        } else if (cnt_cur==500 ) {
                QString sql_save = "delete from db_data  where name='last_match' ; "
                                   "insert into db_data(name, value) values('last_match', '" + QString::number(cnt_cur) + "') ; ";
                QSqlQuery *query_save = new QSqlQuery(db);
                mySQL.exec(this, sql_save, "Запомним \"Крутое\" число", *query_save, true, db, data_app);
                delete query_save;
            QMessageBox::information(this, "УРА!!!",
                                     "Это - ПЯТИСОТЫЙ застрахованный !");
        } else if (cnt_cur==1000 ) {
            QString sql_save = "delete from db_data  where name='last_match' ; "
                               "insert into db_data(name, value) values('last_match', '" + QString::number(cnt_cur) + "') ; ";
                QSqlQuery *query_save = new QSqlQuery(db);
                mySQL.exec(this, sql_save, "Запомним \"Крутое\" число", *query_save, true, db, data_app);
                delete query_save;
            QMessageBox::information(this, "УРА!!!",
                                     "Это - ТЫСЯЧНЫЙ застрахованный !");
        } else if (cnt_cur==5000 ) {
            QString sql_save = "delete from db_data  where name='last_match' ; "
                               "insert into db_data(name, value) values('last_match', '" + QString::number(cnt_cur) + "') ; ";
                QSqlQuery *query_save = new QSqlQuery(db);
                mySQL.exec(this, sql_save, "Запомним \"Крутое\" число", *query_save, true, db, data_app);
                delete query_save;
            QMessageBox::information(this, "УРА!!!",
                                     "Это - ПЯТИТЫСЯЧНЫЙ застрахованный !");
        } else if (cnt_cur==10000 ) {
            QString sql_save = "delete from db_data  where name='last_match' ; "
                               "insert into db_data(name, value) values('last_match', '" + QString::number(cnt_cur) + "') ; ";
                QSqlQuery *query_save = new QSqlQuery(db);
                mySQL.exec(this, sql_save, "Запомним \"Крутое\" число", *query_save, true, db, data_app);
                delete query_save;
            QMessageBox::information(this, "УРА!!!",
                                     "Это - 10 000 застрахованный !");
        } else if (cnt_cur==50000 ) {
            QString sql_save = "delete from db_data  where name='last_match' ; "
                               "insert into db_data(name, value) values('last_match', '" + QString::number(cnt_cur) + "') ; ";
                QSqlQuery *query_save = new QSqlQuery(db);
                mySQL.exec(this, sql_save, "Запомним \"Крутое\" число", *query_save, true, db, data_app);
                delete query_save;
            QMessageBox::information(this, "УРА!!!",
                                     "Это - 50 000 застрахованный !");
        } else if (cnt_cur==100000 ) {
            QString sql_save = "delete from db_data  where name='last_match' ; "
                               "insert into db_data(name, value) values('last_match', '" + QString::number(cnt_cur) + "') ; ";
                QSqlQuery *query_save = new QSqlQuery(db);
                mySQL.exec(this, sql_save, "Запомним \"Крутое\" число", *query_save, true, db, data_app);
                delete query_save;
            QMessageBox::information(this, "УРА!!!",
                                     "Это - СТОТЫСЯЧНЫЙ застрахованный !");
        } else if (cnt_cur==500000 ) {
            QString sql_save = "delete from db_data  where name='last_match' ; "
                               "insert into db_data(name, value) values('last_match', '" + QString::number(cnt_cur) + "') ; ";
                QSqlQuery *query_save = new QSqlQuery(db);
                mySQL.exec(this, sql_save, "Запомним \"Крутое\" число", *query_save, true, db, data_app);
                delete query_save;
            QMessageBox::information(this, "УРА!!!",
                                     "Это - 500 000 застрахованный !");
        } else if (cnt_cur==1000000 ) {
            QString sql_save = "delete from db_data  where name='last_match' ; "
                               "insert into db_data(name, value) values('last_match', '" + QString::number(cnt_cur) + "') ; ";
                QSqlQuery *query_save = new QSqlQuery(db);
                mySQL.exec(this, sql_save, "Запомним \"Крутое\" число", *query_save, true, db, data_app);
                delete query_save;
            QMessageBox::information(this, "УРА!!!",
                                     "Это - МИЛЛИОННЫЙ  застрахованный !");
        } else if (cnt_cur==5000000 ) {
            QString sql_save = "delete from db_data  where name='last_match' ; "
                               "insert into db_data(name, value) values('last_match', '" + QString::number(cnt_cur) + "') ; ";
                QSqlQuery *query_save = new QSqlQuery(db);
                mySQL.exec(this, sql_save, "Запомним \"Крутое\" число", *query_save, true, db, data_app);
                delete query_save;
            QMessageBox::information(this, "УРА!!!",
                                     "Это - 5 000 000 застрахованный !");
        } else if (cnt_cur==10000000 ) {
            QString sql_save = "delete from db_data  where name='last_match' ; "
                               "insert into db_data(name, value) values('last_match', '" + QString::number(cnt_cur) + "') ; ";
                QSqlQuery *query_save = new QSqlQuery(db);
                mySQL.exec(this, sql_save, "Запомним \"Крутое\" число", *query_save, true, db, data_app);
                delete query_save;
            QMessageBox::information(this, "УРА!!!",
                                     "Это - 10 000 000 застрахованный !");
        }
    }
    delete query_cntr;

    return;
}

bool polices_wnd::corrID_Polis(int id_polis) {
    // правка идентифицирующих данных полиса
    QString sql;
    sql += "update polises ";
    sql += "   set enp='" +data_polis.enp+ "', ";
    sql += "       pol_type='"+data_polis.pol_type+"', ";
    sql += "       pol_ser='" +data_polis.pol_ser+ "', ";
    sql += "       pol_num='" +data_polis.pol_num+ "', ";
    sql += "       vs_num='"  +data_polis.vs_num+  "' ";
    sql += " where id=" +QString::number(id_polis)+ " ; ";

    QSqlQuery *query_corr = new QSqlQuery(db);
    bool res_corr = mySQL.exec(this, sql, QString("Добавление нового действия оператора"), *query_corr, true, db, data_app);
    if (!res_corr) {
        QMessageBox::critical(this, "Операция не выполнена!", "Добавление нового действия оператора - Операция добавления записи была отменена.");
        data_event.is_ok = false;
        delete query_corr;
        return false;
    }
    delete query_corr;

    return true;
}

bool polices_wnd::edit_Polis(int id_polis) {
    // правка данных полиса
    QString sql;
    sql += "update polises ";
    sql += " set in_erp=" +QString::number(data_polis.in_erp)+ ", ";
    sql += "     order_num='"  +data_polis.order_num+  "' ";
    sql += "     order_date='"  +data_polis.order_date.toString("yyyy-MM-dd")+  "' ";
    if (data_polis.has_date_begin)
        sql += " date_begin='" +data_polis.date_begin.toString("yyyy-MM-dd")+ "', ";
    if (data_polis.has_date_end)
        sql += " date_end='"+data_polis.date_end.toString("yyyy-MM-dd")+"', ";
    if (data_polis.has_date_stop)
        sql += " date_stop='" +data_polis.date_stop.toString("yyyy-MM-dd")+ "', ";
    // отметки о последнем авторе записи
    sql += "     _id_last_point=" +QString::number(data_app.id_point)+ ", ";
    sql += "     _id_last_operator=" +QString::number(data_app.id_operator)+ ", ";
    sql += "     _id_last_event=" +QString::number(data_event.id)+ ", ";
    sql += " where id=" +QString::number(id_polis)+ " ; ";

    QSqlQuery *query_corr = new QSqlQuery(db);
    bool res_corr = mySQL.exec(this, sql, QString("Добавление нового действия оператора"), *query_corr, true, db, data_app);
    if (!res_corr) {
        QMessageBox::critical(this, "Операция не выполнена!", "Добавление нового действия оператора - Операция добавления записи была отменена.");
        data_event.is_ok = false;
        delete query_corr;
        return false;
    }
    delete query_corr;

    return true;
}

bool polices_wnd::close_Polis(int id_polis) {
    return false;
}

void polices_wnd::on_combo_pol_v_currentIndexChanged(int index) {
    //ui->lab_print_vs->setEnabled(ui->combo_pol_v->currentData().toInt()==2);
    ui->bn_print_vs->setEnabled(ui->combo_pol_v->currentData().toInt()==2);
}



// -------------------------------------------------------
// Первичный выбор СМО (выдача временного свидетельства)
// -------------------------------------------------------
void polices_wnd::on_bn_p010_clicked() {
    // причина подачи заявления больше 0
    if (data_vizit.r_smo==0) {
        data_vizit.r_smo = 1;
        ui->combo_vizit_pan_reason_smo->setCurrentIndex(1);
    }
    //заявления на дублкат нет
    {
        data_vizit.r_polis = 0;
        ui->combo_vizit_pan_reason_polis->setCurrentIndex(0);
    }
    // форма полиса 1
    if (data_vizit.f_polis==0) {
        data_vizit.f_polis = 1;
        ui->combo_vizit_pan_form_polis->setCurrentIndex(1);
    }

    // проверка, не умер ли застрахованный
    QString sql_dead = "";
    sql_dead += "select e.date_death "       // контрольный запрос - дата смерти
                "  from persons e "
                " where e.id=" + QString::number(data_pers.id) + " ; ";


    // проверка на дату смерти
    if (!test_for_dead()) {
        data_event.is_ok = false;
        return;
    }


    // проверка действующих полисов
    QString sql = "";
    sql += "select p.pol_v, f8.text as pol_v_text, p.pol_type, p.pol_ser, p.pol_num, p.vs_num, p.date_begin, p.date_end, p.date_stop, p.id, p.in_erp, "
           "       coalesce('('||bv_st.code||') '||bv_st.text, '('||bp_st.code||') '||bp_st.text, ' - ??? - ') as status_text "
           " from persons e "
           "     join polises p on( "
           "         (e.id=p.id_person) and "
           "         (p.date_begin<=CURRENT_DATE) and "
           "         (p.date_stop is NULL or p.date_stop>CURRENT_DATE) ) "
           "     left join spr_f008 f8 on(f8.code=p.pol_v) "
           "     left join blanks_pol bp on(bp.id_polis=p.id) "
           "     left join spr_blank_status bp_st on(bp_st.code=bp.status) "
           "     left join blanks_vs bv on(bv.id_polis=p.id) "
           "     left join spr_blank_status bv_st on(bv_st.code=bv.status) "
           " where e.id=" + QString::number(data_pers.id) + " "
           "   and ((bp.id is not NULL and bp.status>0) or (bv.vs_num is not NULL and bv.status>0)) ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString("Проверка полисов застрахованного"), *query, true, db, data_app);
    if (res && query->size()>0) {
        QString s_out = "Данное событие неприемлемо, поскольку у застрахованного уже есть действующий полис(полисы):\n";
        while (query->next()) {
            s_out += "\n(" + QString::number(query->value(0).toInt()) + ") " + query->value(1).toString() +
                    "\nтип/серия/номер: " + (query->value(2).isNull() ? "- null -" : query->value(2).toString()) + ",  " +
                                            (query->value(3).isNull() ? "- null -" : query->value(3).toString()) + " № " +
                                            (query->value(4).isNull() ? "- null -" : query->value(4).toString()) + ",  ВС " +
                                            (query->value(5).isNull() ? "- null -" : query->value(5).toString()) +
                    "\nоткрыт: " + (query->value(6).isNull() ? "- null -" : query->value(6).toDate().toString("dd.MM.yyyy")) +
                    (query->value(7).isNull() ? ",  нет срока действия " : ( query->value(7).toDate()>QDate::currentDate() ? ",  срок действия: " : ",  срок действия истёк: ") + query->value(7).toDate().toString("dd.MM.yyyy")) +
                    ",  дата изъятия: " + (query->value(8).isNull() ? "- null -" : query->value(8).toDate().toString("dd.MM.yyyy")) + "\n"
                    "статус - " + query->value(11).toString();
        }
        QMessageBox::information(this, "П010 - Полис уже есть!", s_out);
        data_event.is_ok = false;
        delete query;
        return;
    }
    delete query;

    // откроем панель свойств
    on_show_pan_vizit_clicked(false);
    on_show_pan_polises_clicked(true);
    on_show_pan_polis_clicked(true);
    ui->pan_vizit->setEnabled(false);
    ui->pan_polises->setEnabled(false);

    ui->split_main->setStretchFactor(0,2);
    ui->split_main->setStretchFactor(1,5);
    ui->split_main->setStretchFactor(2,3);

    refresh_terr_ocato();
    refresh_pol_ves();

    ui->lab_event_code->setText("П010");
    data_event.event_code = "П010";

    ui->line_terr_ocato->setText(data_app.ocato);
    ui->combo_terr_ocato_text->setCurrentIndex(ui->combo_terr_ocato_text->findData(data_app.ocato.toInt()));
    ui->line_ogrn_smo->setText(data_app.smo_ogrn);
    ui->line_ogrn_smo_text->setText(data_app.smo_name);
    ui->combo_pol_v->setCurrentIndex(ui->combo_pol_v->findData(2));
    ui->combo_pol_v->setEnabled(false);

    ui->line_pol_ser->setText("");
    ui->line_pol_num->setText("");
    ui->ch_pol_sernum->setChecked(false);
    ui->ch_pol_sernum->setEnabled(false);

    ui->ch_vs_num->setChecked(true);
    ui->ch_vs_num->setEnabled(true);

    ui->line_enp->setText("");
    ui->ch_pol_enp->setChecked(false);
    ui->ch_pol_enp->setEnabled(false);

    QDate date_start = data_vizit.date;
    //QDate date_start = QDate::currentDate();
    QDate date_end = myAddDays(date_start, 30);
    data_polis.date_end = date_end;
    data_polis.has_date_end = true;
    data_polis.has_date_stop = false;
    data_polis.date_end = QDate(1900,1,1);

    ui->date_pol_begin->setDate(date_start);
    ui->ch_pol_datbegin->setChecked(true);
    ui->ch_pol_datbegin->setEnabled(true);

    ui->date_pol_end->setDate(date_end);
    data_polis.date_end = date_end;
    ui->ch_pol_datend->setChecked(true);
    ui->ch_pol_datend->setEnabled(true);

    ui->date_pol_stop->setDate(QDate(1900,1,1));
    ui->ch_pol_datstop->setEnabled(true);
    ui->ch_pol_datstop->setChecked(false);
    ui->date_pol_stop->setVisible(true);
    on_ch_pol_datstop_stateChanged(0);
    ui->combo_pol_in_erp->setCurrentIndex(0);

    //ui->date_order_date->setDate(date_start);

    //ui->ch_pol_erp->setChecked(false);
    //ui->ch_pol_erp->setEnabled(false);

    ui->lab_point_name->setText(data_app.point_name);
    ui->lab_operator_name->setText(data_app.operator_fio);
    ui->lab_edit_date->setText(" - не сохранён - ");

    // подготовим свойства полиса для новой времянки нового застрахованного
    data_polis.id = -1;
    data_polis.id_person = data_pers.id;
    data_polis.id_old_polis = -1;
    data_polis.pol_type = "В";
    data_polis.pol_v = 2;
    data_event.id_polis = data_polis.id;
    data_event.event_code = "П010";
    data_event.event_dt = QDateTime(ui->date_vizit_pan->date(), QTime::currentTime());
    data_event.event_time = QTime::currentTime();
    data_event.comment = "Первичный выбор СМО (выдача временного свидетельства) ...";

    // подготовим панель свойств к редактированию данных ВС
    ui->lab_print_vs->setEnabled(true);
    ui->bn_print_vs->setEnabled(true);
    ui->bn_print_vs_2->setEnabled(true);
    ui->lab_print_ch_smo->setEnabled(true);
    ui->bn_print_ch_smo->setEnabled(true);
    ui->lab_prind_db_pol->setEnabled(false);
    ui->bn_print_db_pol->setEnabled(false);
    ui->lab_print_notice->setEnabled(true);
    ui->bn_print_notice_pol->setEnabled(true);

    test_for_udl_exp();
    test_for_pers_categ();

    ui->line_vs_num->setFocus();
    b_need_save (true);
}

void polices_wnd::on_bn_p021_clicked() {
    // причина подачи заявления 0
    {
        data_vizit.r_smo = 0;
        ui->combo_vizit_pan_reason_smo->setCurrentIndex(0);
    }
    //заявления на дублкат нет
    {
        data_vizit.r_polis = 0;
        ui->combo_vizit_pan_reason_polis->setCurrentIndex(0);
    }


    db.transaction();

    ui->lab_event_code->setText("П021");
    data_event.event_code = "П021";

    // закрываем старые полисы
    QString sql_old;
    sql_old =  "select p.id, p.id_person, p.ocato, p.ogrn_smo, p.pol_v, f8.text as pol_v_text, p.pol_type, p.pol_ser, p.pol_num, p.vs_num, p.enp, p.date_begin, p.date_end, p.date_stop, p.in_erp, p.order_num, p.order_date, p._id_last_point, p._id_last_operator, p._id_last_event ";
    sql_old += "  from polises p "
               "  left join spr_f008 f8 on (f8.code=p.pol_v) ";
    sql_old += " where p.id_person=" + QString::number(data_pers.id) + " and ";
    sql_old += "       p.date_begin is not null and ";
    sql_old += "       p.date_begin<=CURRENT_DATE and ";
    sql_old += "       p.date_get2hand<=CURRENT_DATE and ";
    //sql_old += "       ( (p.date_end is NULL) or (p.date_end>CURRENT_DATE) ) and ";
    sql_old += "       ( (p.date_stop is NULL) or (p.date_stop>CURRENT_DATE) ) ; ";

    QSqlQuery *query_old = new QSqlQuery(db);
    bool res_old = mySQL.exec(this, sql_old, QString("Проверка, есть ли действующие полисы"), *query_old, true, db, data_app);
    // проверим старые полисы
    while (query_old->next()) {
        s_data_polis data_old_polis;
        data_old_polis.id = query_old->value(0).toInt();
        data_old_polis.id_person = query_old->value(1).toInt();
        data_old_polis.ocato = query_old->value(2).toString();
        data_old_polis.ogrn_smo = query_old->value(3).toString();
        data_old_polis.pol_v = query_old->value(4).toInt();
        data_old_polis.pol_v_text = query_old->value(5).toString();
        data_old_polis.pol_type = query_old->value(6).toString();
        data_old_polis.has_enp = !(query_old->value(10).isNull());
        data_old_polis.has_pol_sernum = !(query_old->value(8).isNull());
        data_old_polis.has_vs_num = !(query_old->value(9).isNull());
        data_old_polis.pol_ser = query_old->value(7).toString();
        data_old_polis.pol_num = query_old->value(8).toString();
        data_old_polis.vs_num = query_old->value(9).toString();
        data_old_polis.enp = query_old->value(10).toString();
        data_old_polis.has_date_begin = !(query_old->value(11).isNull());
        data_old_polis.has_date_end = !(query_old->value(12).isNull());
        data_old_polis.has_date_stop = !(query_old->value(13).isNull());
        data_old_polis.date_begin = query_old->value(11).toDate();
        data_old_polis.date_end = query_old->value(12).toDate();
        data_old_polis.date_stop = query_old->value(13).toDate();
        data_old_polis.in_erp = query_old->value(14).toInt();
        data_old_polis.order_num = query_old->value(15).toString();
        data_old_polis.order_date = query_old->value(16).toDate();
        data_old_polis._id_last_point = query_old->value(17).toInt();
        data_old_polis._id_last_operator = query_old->value(18).toInt();
        data_old_polis._id_last_event = query_old->value(19).toInt();

        if ((data_old_polis.pol_v==1 || data_old_polis.pol_v==2 || data_old_polis.pol_v==3)) {
            data_polis.id_old_polis = data_old_polis.id;
            if (QMessageBox::question(this,
                                      "Закрыть старый полис ?",
                                      QString("У застрахованного есть действующий полис ОМС единого образца или полис ОМС старого образца нашей СМО: \n") +
                                      "тип: " + query_old->value(4).toString() + "\n" +
                                      "серия/номер: " + query_old->value(7).toString() + " № " + query_old->value(8).toString() + "\n" +
                                      "дата выдачи: " + query_old->value(11).toDate().toString("dd.MM.yyyy") + "\n" +
                                      "№ ВС: " + query_old->value(9).toString() + "\n" +
                                      "ЕНП: " + query_old->value(10).toString() + "\n\n" +
                                      "Событие П021 предполагает закрытие действующего полиса единого образца. \n\nЗакрыть полис ?",
                                      QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Yes)==QMessageBox::Yes) {
                // проверим состояние старого полиса
                int close_stat = -2;
                QStringList list;
                list.append("полис предоставлен и изымается");
                list.append("полис утерян / испорчен по вине застрахованного");
                if (QInputDialog::getItem(this, "Статус закрываемого полиса", "Выберите статус закрываемого полиса:", list, 0, false)=="полис предоставлен и изымается")
                    close_stat = -2;
                else close_stat = -3;

                // закрытие полиса
                s_data_event close_event;
                close_event.id_polis = data_old_polis.id;
                close_event.event_code = "П021";
                close_event.event_dt = QDateTime(ui->date_stoppolis->date());
                close_event.event_time = QTime::currentTime();
                close_event.comment = "полис закрывается по событию П021.";
                if (!process_P021_P022_P023_P024_P025(data_old_polis, close_event, close_stat, false)) {
                    QMessageBox::warning(this, "Ошибка при закрытии старого полиса", "При попытке закрыть старый полис по событию П021 произошла непредвиденная ошибка.\n\nОперация страхования отменена.");
                    data_polis.is_ok = false;
                    delete query_old;
                    db.rollback();
                    return;
                }
                refresh_polises_tab();
            } else {
                // отмена операции
                QMessageBox::warning(this, "Операция прекращения страхования отменена", "Операция прекращения страхования отменена.");
                data_polis.is_ok = false;
                delete query_old;
                db.rollback();
                return;
            }
        }
    }
    delete query_old;

    db.commit();
}

void polices_wnd::on_bn_p022_clicked() {
    // причина подачи заявления 0
    {
        data_vizit.r_smo = 0;
        ui->combo_vizit_pan_reason_smo->setCurrentIndex(0);
    }
    //заявления на дублкат нет
    {
        data_vizit.r_polis = 0;
        ui->combo_vizit_pan_reason_polis->setCurrentIndex(0);
    }


    db.transaction();

    ui->lab_event_code->setText("П022");
    data_event.event_code = "П022";

    // закрываем старые полисы
    QString sql_old;
    sql_old =  "select p.id, p.id_person, p.ocato, p.ogrn_smo, p.pol_v, f8.text as pol_v_text, p.pol_type, p.pol_ser, p.pol_num, p.vs_num, p.enp, p.date_begin, p.date_end, p.date_stop, p.in_erp, p.order_num, p.order_date, p._id_last_point, p._id_last_operator, p._id_last_event ";
    sql_old += " from polises p left join spr_f008 f8 on (f8.code=p.pol_v)";
    sql_old += " where p.id_person=" + QString::number(data_pers.id) + " and ";
    sql_old += "       p.date_begin is not null and ";
    sql_old += "       p.date_begin<=CURRENT_DATE and ";
    sql_old += "       p.date_get2hand<=CURRENT_DATE and ";
    //sql_old += "       ( (p.date_end is NULL) or (p.date_end>CURRENT_DATE) ) and ";
    sql_old += "       ( (p.date_stop is NULL) or (p.date_stop>CURRENT_DATE) ) ; ";

    QSqlQuery *query_old = new QSqlQuery(db);
    bool res_old = mySQL.exec(this, sql_old, QString("Проверка, есть ли действующие полисы"), *query_old, true, db, data_app);
    // проверим старые полисы
    while (query_old->next()) {
        s_data_polis data_old_polis;
        data_old_polis.id = query_old->value(0).toInt();
        data_old_polis.id_person = query_old->value(1).toInt();
        data_old_polis.ocato = query_old->value(2).toString();
        data_old_polis.ogrn_smo = query_old->value(3).toString();
        data_old_polis.pol_v = query_old->value(4).toInt();
        data_old_polis.pol_v_text = query_old->value(5).toString();
        data_old_polis.pol_type = query_old->value(6).toString();
        data_old_polis.has_enp = !(query_old->value(10).isNull());
        data_old_polis.has_pol_sernum = !(query_old->value(8).isNull());
        data_old_polis.has_vs_num = !(query_old->value(9).isNull());
        data_old_polis.pol_ser = query_old->value(7).toString();
        data_old_polis.pol_num = query_old->value(8).toString();
        data_old_polis.vs_num = query_old->value(9).toString();
        data_old_polis.enp = query_old->value(10).toString();
        data_old_polis.has_date_begin = !(query_old->value(11).isNull());
        data_old_polis.has_date_end = !(query_old->value(12).isNull());
        data_old_polis.has_date_stop = !(query_old->value(13).isNull());
        data_old_polis.date_begin = query_old->value(11).toDate();
        data_old_polis.date_end = query_old->value(12).toDate();
        data_old_polis.date_stop = query_old->value(13).toDate();
        data_old_polis.in_erp = query_old->value(14).toInt();
        data_old_polis.order_num = query_old->value(15).toString();
        data_old_polis.order_date = query_old->value(16).toDate();
        data_old_polis._id_last_point = query_old->value(17).toInt();
        data_old_polis._id_last_operator = query_old->value(18).toInt();
        data_old_polis._id_last_event = query_old->value(19).toInt();

        if ((data_old_polis.pol_v==1 || data_old_polis.pol_v==2 || data_old_polis.pol_v==3)) {
            data_polis.id_old_polis = data_old_polis.id;
            if (QMessageBox::question(this,
                                      "Закрыть старый полис ?",
                                      QString("У застрахованного есть действующий полис ОМС единого образца или полис ОМС старого образца нашей СМО: \n") +
                                      "тип: " + query_old->value(4).toString() + "\n" +
                                      "серия/номер: " + query_old->value(7).toString() + " № " + query_old->value(8).toString() + "\n" +
                                      "дата выдачи: " + query_old->value(11).toDate().toString("dd.MM.yyyy") + "\n" +
                                      "№ ВС: " + query_old->value(9).toString() + "\n" +
                                      "ЕНП: " + query_old->value(10).toString() + "\n\n" +
                                      "Событие П022 предполагает закрытие действующего полиса единого образца в связи со смертью застрахованного. \n\nЗакрыть полис ?",
                                      QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Yes)==QMessageBox::Yes) {
                // проверим состояние старого полиса
                int close_stat = -2;
                QStringList list;
                list.append("полис предоставлен и изымается");
                list.append("полис утерян / испорчен по вине застрахованного");
                if (QInputDialog::getItem(this, "Статус закрываемого полиса", "Выберите статус закрываемого полиса:", list, 0, false)=="полис предоставлен и изымается")
                    close_stat = -2;
                else close_stat = -3;

                // закрытие старого полиса
                s_data_event close_event;
                close_event.id_polis = data_old_polis.id;
                close_event.event_code = "П022";
                close_event.event_dt = QDateTime(ui->date_stoppolis->date());
                close_event.event_time = QTime::currentTime();
                close_event.comment = "полис закрывается по событию П022.";
                if (!process_P021_P022_P023_P024_P025(data_old_polis, close_event, close_stat, false)) {
                    QMessageBox::warning(this, "Ошибка при закрытии старого полиса", "При попытке закрыть старый полис по событию П022 произошла непредвиденная ошибка.\n\nОперация страхования отменена.");
                    data_polis.is_ok = false;
                    delete query_old;
                    db.rollback();
                    return;
                }
                refresh_polises_tab();
            } else {
                // отмена операции
                QMessageBox::warning(this, "Операция прекращения страхования отменена", "Операция прекращения страхования отменена.");
                data_polis.is_ok = false;
                delete query_old;
                db.rollback();
                return;
            }
        }
    }
    delete query_old;

    db.commit();
}

void polices_wnd::on_bn_p023_clicked() {
    // причина подачи заявления 0
    {
        data_vizit.r_smo = 0;
        ui->combo_vizit_pan_reason_smo->setCurrentIndex(0);
    }
    //заявления на дублкат нет
    {
        data_vizit.r_polis = 0;
        ui->combo_vizit_pan_reason_polis->setCurrentIndex(0);
    }


    db.transaction();

    ui->lab_event_code->setText("П023");
    data_event.event_code = "П23";

    // закрываем старые полисы
    QString sql_old;
    sql_old =  "select p.id, p.id_person, p.ocato, p.ogrn_smo, p.pol_v, f8.text as pol_v_text, p.pol_type, p.pol_ser, p.pol_num, p.vs_num, p.enp, p.date_begin, p.date_end, p.date_stop, p.in_erp, p.order_num, p.order_date, p._id_last_point, p._id_last_operator, p._id_last_event ";
    sql_old += " from polises p left join spr_f008 f8 on (f8.code=p.pol_v)";
    sql_old += " where p.id_person=" + QString::number(data_pers.id) + " and ";
    sql_old += "       p.date_begin is not null and ";
    sql_old += "       p.date_begin<=CURRENT_DATE and ";
    sql_old += "       p.date_get2hand<=CURRENT_DATE and ";
    //sql_old += "       ( (p.date_end is NULL) or (p.date_end>CURRENT_DATE) ) and ";
    sql_old += "       ( (p.date_stop is NULL) or (p.date_stop>CURRENT_DATE) ) ; ";

    QSqlQuery *query_old = new QSqlQuery(db);
    bool res_old = mySQL.exec(this, sql_old, QString("Проверка, есть ли действующие полисы"), *query_old, true, db, data_app);
    // проверим старые полисы
    while (query_old->next()) {
        s_data_polis data_old_polis;
        data_old_polis.id = query_old->value(0).toInt();
        data_old_polis.id_person = query_old->value(1).toInt();
        data_old_polis.ocato = query_old->value(2).toString();
        data_old_polis.ogrn_smo = query_old->value(3).toString();
        data_old_polis.pol_v = query_old->value(4).toInt();
        data_old_polis.pol_v_text = query_old->value(5).toString();
        data_old_polis.pol_type = query_old->value(6).toString();
        data_old_polis.has_enp = !(query_old->value(10).isNull());
        data_old_polis.has_pol_sernum = !(query_old->value(8).isNull());
        data_old_polis.has_vs_num = !(query_old->value(9).isNull());
        data_old_polis.pol_ser = query_old->value(7).toString();
        data_old_polis.pol_num = query_old->value(8).toString();
        data_old_polis.vs_num = query_old->value(9).toString();
        data_old_polis.enp = query_old->value(10).toString();
        data_old_polis.has_date_begin = !(query_old->value(11).isNull());
        data_old_polis.has_date_end = !(query_old->value(12).isNull());
        data_old_polis.has_date_stop = !(query_old->value(13).isNull());
        data_old_polis.date_begin = query_old->value(11).toDate();
        data_old_polis.date_end = query_old->value(12).toDate();
        data_old_polis.date_stop = query_old->value(13).toDate();
        data_old_polis.in_erp = query_old->value(14).toInt();
        data_old_polis.order_num = query_old->value(15).toString();
        data_old_polis.order_date = query_old->value(16).toDate();
        data_old_polis._id_last_point = query_old->value(17).toInt();
        data_old_polis._id_last_operator = query_old->value(18).toInt();
        data_old_polis._id_last_event = query_old->value(19).toInt();

        if ((data_old_polis.pol_v==1 || data_old_polis.pol_v==2 || data_old_polis.pol_v==3)) {
            data_polis.id_old_polis = data_old_polis.id;
            if (QMessageBox::question(this,
                                      "Закрыть старый полис ?",
                                      QString("У застрахованного есть действующий полис ОМС единого образца или полис ОМС старого образца нашей СМО: \n") +
                                      "тип: " + query_old->value(4).toString() + "\n" +
                                      "серия/номер: " + query_old->value(7).toString() + " № " + query_old->value(8).toString() + "\n" +
                                      "дата выдачи: " + query_old->value(11).toDate().toString("dd.MM.yyyy") + "\n" +
                                      "№ ВС: " + query_old->value(9).toString() + "\n" +
                                      "ЕНП: " + query_old->value(10).toString() + "\n\n" +
                                      "Событие П023 предполагает закрытие действующего полиса единого образца для разрешения дублирующихся записей. \n\nЗакрыть полис ?",
                                      QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Yes)==QMessageBox::Yes) {
                // проверим состояние старого полиса
                int close_stat = -2;
                QStringList list;
                list.append("полис предоставлен и изымается");
                list.append("полис утерян / испорчен по вине застрахованного");
                if (QInputDialog::getItem(this, "Статус закрываемого полиса", "Выберите статус закрываемого полиса:", list, 0, false)=="полис предоставлен и изымается")
                    close_stat = -2;
                else close_stat = -3;

                // закрытие старого полиса
                s_data_event close_event;
                close_event.id_polis = data_old_polis.id;
                close_event.event_code = "П023";
                close_event.event_dt = QDateTime(ui->date_stoppolis->date());
                close_event.event_time = QTime::currentTime();
                close_event.comment = "полис закрывается по событию П023.";
                if (!process_P021_P022_P023_P024_P025(data_old_polis, close_event, close_stat, false)) {
                    QMessageBox::warning(this, "Ошибка при закрытии старого полиса", "При попытке закрыть старый полис по событию П023 произошла непредвиденная ошибка.\n\nОперация страхования отменена.");
                    data_polis.is_ok = false;
                    delete query_old;
                    db.rollback();
                    return;
                }
                refresh_polises_tab();
            } else {
                // отмена операции
                QMessageBox::warning(this, "Операция прекращения страхования отменена", "Операция прекращения страхования отменена.");
                data_polis.is_ok = false;
                delete query_old;
                db.rollback();
                return;
            }
        }
    }
    delete query_old;

    db.commit();
}

void polices_wnd::on_bn_p024_clicked() {
    // причина подачи заявления 0
    {
        data_vizit.r_smo = 0;
        ui->combo_vizit_pan_reason_smo->setCurrentIndex(0);
    }
    //заявления на дублкат нет
    {
        data_vizit.r_polis = 0;
        ui->combo_vizit_pan_reason_polis->setCurrentIndex(0);
    }


    db.transaction();

    ui->lab_event_code->setText("П024");
    data_event.event_code = "П024";

    // закрываем старые полисы
    QString sql_old;
    sql_old =  "select p.id, p.id_person, p.ocato, p.ogrn_smo, p.pol_v, f8.text as pol_v_text, p.pol_type, p.pol_ser, p.pol_num, p.vs_num, p.enp, p.date_begin, p.date_end, p.date_stop, p.in_erp, p.order_num, p.order_date, p._id_last_point, p._id_last_operator, p._id_last_event ";
    sql_old += " from polises p left join spr_f008 f8 on (f8.code=p.pol_v)";
    sql_old += " where p.id_person=" + QString::number(data_pers.id) + " and ";
    sql_old += "       p.date_begin is not null and ";
    sql_old += "       p.date_begin<=CURRENT_DATE and ";
    sql_old += "       p.date_get2hand<=CURRENT_DATE and ";
    //sql_old += "       ( (p.date_end is NULL) or (p.date_end>CURRENT_DATE) ) and ";
    sql_old += "       ( (p.date_stop is NULL) or (p.date_stop>CURRENT_DATE) ) ; ";

    QSqlQuery *query_old = new QSqlQuery(db);
    bool res_old = mySQL.exec(this, sql_old, QString("Проверка, есть ли действующие полисы"), *query_old, true, db, data_app);
    // проверим старые полисы
    while (query_old->next()) {
        s_data_polis data_old_polis;
        data_old_polis.id = query_old->value(0).toInt();
        data_old_polis.id_person = query_old->value(1).toInt();
        data_old_polis.ocato = query_old->value(2).toString();
        data_old_polis.ogrn_smo = query_old->value(3).toString();
        data_old_polis.pol_v = query_old->value(4).toInt();
        data_old_polis.pol_v_text = query_old->value(5).toString();
        data_old_polis.pol_type = query_old->value(6).toString();
        data_old_polis.has_enp = !(query_old->value(10).isNull());
        data_old_polis.has_pol_sernum = !(query_old->value(8).isNull());
        data_old_polis.has_vs_num = !(query_old->value(9).isNull());
        data_old_polis.pol_ser = query_old->value(7).toString();
        data_old_polis.pol_num = query_old->value(8).toString();
        data_old_polis.vs_num = query_old->value(9).toString();
        data_old_polis.enp = query_old->value(10).toString();
        data_old_polis.has_date_begin = !(query_old->value(11).isNull());
        data_old_polis.has_date_end = !(query_old->value(12).isNull());
        data_old_polis.has_date_stop = !(query_old->value(13).isNull());
        data_old_polis.date_begin = query_old->value(11).toDate();
        data_old_polis.date_end = query_old->value(12).toDate();
        data_old_polis.date_stop = query_old->value(13).toDate();
        data_old_polis.in_erp = query_old->value(14).toInt();
        data_old_polis.order_num = query_old->value(15).toString();
        data_old_polis.order_date = query_old->value(16).toDate();
        data_old_polis._id_last_point = query_old->value(17).toInt();
        data_old_polis._id_last_operator = query_old->value(18).toInt();
        data_old_polis._id_last_event = query_old->value(19).toInt();

        if ((data_old_polis.pol_v==1 || data_old_polis.pol_v==2 || data_old_polis.pol_v==3)) {
            data_polis.id_old_polis = data_old_polis.id;
            if (QMessageBox::question(this,
                                      "Закрыть старый полис ?",
                                      QString("У застрахованного есть действующий полис ОМС единого образца или полис ОМС старого образца нашей СМО: \n") +
                                      "тип: " + query_old->value(4).toString() + "\n" +
                                      "серия/номер: " + query_old->value(7).toString() + " № " + query_old->value(8).toString() + "\n" +
                                      "дата выдачи: " + query_old->value(11).toDate().toString("dd.MM.yyyy") + "\n" +
                                      "№ ВС: " + query_old->value(9).toString() + "\n" +
                                      "ЕНП: " + query_old->value(10).toString() + "\n\n" +
                                      "Событие П024 предполагает закрытие действующего полиса единого образца при исправлении ошибок в ЕНП. \n\nЗакрыть полис ?",
                                      QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Yes)==QMessageBox::Yes) {
                // проверим состояние старого полиса
                int close_stat = -2;
                QStringList list;
                list.append("полис предоставлен и изымается");
                list.append("полис утерян / испорчен по вине застрахованного");
                if (QInputDialog::getItem(this, "Статус закрываемого полиса", "Выберите статус закрываемого полиса:", list, 0, false)=="полис предоставлен и изымается")
                    close_stat = -2;
                else close_stat = -3;

                // закрытие старого полиса
                s_data_event close_event;
                close_event.id_polis = data_old_polis.id;
                close_event.event_code = "П024";
                close_event.event_dt = QDateTime(ui->date_stoppolis->date());
                close_event.event_time = QTime::currentTime();
                close_event.comment = "полис закрывается по событию П024.";
                if (!process_P021_P022_P023_P024_P025(data_old_polis, close_event, close_stat, false)) {
                    QMessageBox::warning(this, "Ошибка при закрытии старого полиса", "При попытке закрыть старый полис по событию П024 произошла непредвиденная ошибка.\n\nОперация страхования отменена.");
                    data_polis.is_ok = false;
                    delete query_old;
                    db.rollback();
                    return;
                }
                refresh_polises_tab();
            } else {
                // отмена операции
                QMessageBox::warning(this, "Операция прекращения страхования отменена", "Операция прекращения страхования отменена.");
                data_polis.is_ok = false;
                delete query_old;
                db.rollback();
                return;
            }
        }
    }
    delete query_old;

    db.commit();
}

void polices_wnd::on_bn_p025_clicked() {
    // причина подачи заявления 0
    {
        data_vizit.r_smo = 0;
        ui->combo_vizit_pan_reason_smo->setCurrentIndex(0);
    }
    //заявления на дублкат нет
    {
        data_vizit.r_polis = 0;
        ui->combo_vizit_pan_reason_polis->setCurrentIndex(0);
    }


    db.transaction();

    ui->lab_event_code->setText("П025");
    data_event.event_code = "П025";

    // закрываем старые полисы
    QString sql_old;
    sql_old =  "select p.id, p.id_person, p.ocato, p.ogrn_smo, p.pol_v, f8.text as pol_v_text, p.pol_type, p.pol_ser, p.pol_num, p.vs_num, p.enp, p.date_begin, p.date_end, p.date_stop, p.in_erp, p.order_num, p.order_date, p._id_last_point, p._id_last_operator, p._id_last_event ";
    sql_old += " from polises p left join spr_f008 f8 on (f8.code=p.pol_v)";
    sql_old += " where p.id_person=" + QString::number(data_pers.id) + " and ";
    sql_old += "       p.date_begin is not null and ";
    sql_old += "       p.date_begin<=CURRENT_DATE and ";
    sql_old += "       p.date_get2hand<=CURRENT_DATE and ";
    //sql_old += "       ( (p.date_end is NULL) or (p.date_end>CURRENT_DATE) ) and ";
    sql_old += "       ( (p.date_stop is NULL) or (p.date_stop>CURRENT_DATE) ) ; ";

    QSqlQuery *query_old = new QSqlQuery(db);
    bool res_old = mySQL.exec(this, sql_old, QString("Проверка, есть ли действующие полисы"), *query_old, true, db, data_app);
    // проверим старые полисы
    while (query_old->next()) {
        s_data_polis data_old_polis;
        data_old_polis.id = query_old->value(0).toInt();
        data_old_polis.id_person = query_old->value(1).toInt();
        data_old_polis.ocato = query_old->value(2).toString();
        data_old_polis.ogrn_smo = query_old->value(3).toString();
        data_old_polis.pol_v = query_old->value(4).toInt();
        data_old_polis.pol_v_text = query_old->value(5).toString();
        data_old_polis.pol_type = query_old->value(6).toString();
        data_old_polis.has_enp = !(query_old->value(10).isNull());
        data_old_polis.has_pol_sernum = !(query_old->value(8).isNull());
        data_old_polis.has_vs_num = !(query_old->value(9).isNull());
        data_old_polis.pol_ser = query_old->value(7).toString();
        data_old_polis.pol_num = query_old->value(8).toString();
        data_old_polis.vs_num = query_old->value(9).toString();
        data_old_polis.enp = query_old->value(10).toString();
        data_old_polis.has_date_begin = !(query_old->value(11).isNull());
        data_old_polis.has_date_end = !(query_old->value(12).isNull());
        data_old_polis.has_date_stop = !(query_old->value(13).isNull());
        data_old_polis.date_begin = query_old->value(11).toDate();
        data_old_polis.date_end = query_old->value(12).toDate();
        data_old_polis.date_stop = query_old->value(13).toDate();
        data_old_polis.in_erp = query_old->value(14).toInt();
        data_old_polis.order_num = query_old->value(15).toString();
        data_old_polis.order_date = query_old->value(16).toDate();
        data_old_polis._id_last_point = query_old->value(17).toInt();
        data_old_polis._id_last_operator = query_old->value(18).toInt();
        data_old_polis._id_last_event = query_old->value(19).toInt();

        if ((data_old_polis.pol_v==1 || data_old_polis.pol_v==2 || data_old_polis.pol_v==3)) {
            data_polis.id_old_polis = data_old_polis.id;
            if (QMessageBox::question(this,
                                      "Закрыть старый полис ?",
                                      QString("У застрахованного есть действующий полис ОМС единого образца или полис ОМС старого образца нашей СМО: \n") +
                                      "тип: " + query_old->value(4).toString() + "\n" +
                                      "серия/номер: " + query_old->value(7).toString() + " № " + query_old->value(8).toString() + "\n" +
                                      "дата выдачи: " + query_old->value(11).toDate().toString("dd.MM.yyyy") + "\n" +
                                      "№ ВС: " + query_old->value(9).toString() + "\n" +
                                      "ЕНП: " + query_old->value(10).toString() + "\n\n" +
                                      "Событие П025 предполагает закрытие действующего полиса единого образца в связи с приостановлением действия полиса. \n\nЗакрыть полис ?",
                                      QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Yes)==QMessageBox::Yes) {
                // проверим состояние старого полиса
                int close_stat = -2;
                QStringList list;
                list.append("полис предоставлен и изымается");
                list.append("полис утерян / испорчен по вине застрахованного");
                if (QInputDialog::getItem(this, "Статус закрываемого полиса", "Выберите статус закрываемого полиса:", list, 0, false)=="полис предоставлен и изымается")
                    close_stat = -2;
                else close_stat = -3;

                // закрытие старого полиса
                s_data_event close_event;
                close_event.id_polis = data_old_polis.id;
                close_event.event_code = "П025";
                close_event.event_dt = QDateTime(ui->date_stoppolis->date());
                close_event.event_time = QTime::currentTime();
                close_event.comment = "полис закрывается по событию П025.";
                if (!process_P021_P022_P023_P024_P025(data_old_polis, close_event, close_stat, false)) {
                    QMessageBox::warning(this, "Ошибка при закрытии старого полиса", "При попытке закрыть старый полис по событию П025 произошла непредвиденная ошибка.\n\nОперация страхования отменена.");
                    data_polis.is_ok = false;
                    delete query_old;
                    db.rollback();
                    return;
                }
                refresh_polises_tab();
            } else {
                // отмена операции
                QMessageBox::warning(this, "Операция прекращения страхования отменена", "Операция прекращения страхования отменена.");
                data_polis.is_ok = false;
                delete query_old;
                db.rollback();
                return;
            }
        }
    }
    delete query_old;

    db.commit();
}

void polices_wnd::on_bn_p031_clicked() {
    // причина подачи заявления 2
    {
        data_vizit.r_smo = 2;
        ui->combo_vizit_pan_reason_smo->setCurrentIndex(2);
    }
    //заявления на дублкат нет
    {
        data_vizit.r_polis = 0;
        ui->combo_vizit_pan_reason_polis->setCurrentIndex(0);
    }
    // форма полиса 1
    {
        data_vizit.f_polis = 0;
        ui->combo_vizit_pan_form_polis->setCurrentIndex(0);
    }


    // проверка на дату смерти
    if (!test_for_dead()) {
        data_event.is_ok = false;
        return;
    }


    // проверка действующих полисов
    QString sql = "";
    sql += "select p.pol_v, f8.text as pol_v_text, p.pol_type, p.pol_ser, p.pol_num, p.vs_num, p.date_begin, p.date_end, p.date_stop, p.id, p.in_erp, "
           "       coalesce('('||bv_st.code||') '||bv_st.text, '('||bp_st.code||') '||bp_st.text, ' - ??? - ') as status_text "
           " from persons e "
           "     join polises p on( "
           "         (e.id=p.id_person) and "
           "         (p.date_begin<=CURRENT_DATE) and "
           "         (p.date_stop is NULL or p.date_stop>CURRENT_DATE) ) "
           "     left join spr_f008 f8 on(f8.code=p.pol_v) "
           "     left join blanks_pol bp on(bp.id_polis=p.id) "
           "     left join spr_blank_status bp_st on(bp_st.code=bp.status) "
           "     left join blanks_vs bv on(bv.id_polis=p.id) "
           "     left join spr_blank_status bv_st on(bv_st.code=bv.status) "
           " where e.id=" + QString::number(data_pers.id) + " "
           "   and ((bp.id is not NULL and bp.status>0) or (bv.vs_num is not NULL and bv.status>0)) ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString("Проверка полисов застрахованного"), *query, true, db, data_app);
    if (res && query->size()>0) {
        QString s_out = "Данное событие неприемлемо, поскольку у застрахованного уже есть действующий полис(полисы):\n";
        while (query->next()) {
            s_out += "\n(" + QString::number(query->value(0).toInt()) + ") " + query->value(1).toString() +
                    "\nтип/серия/номер: " + (query->value(2).isNull() ? "- null -" : query->value(2).toString()) + ",  " +
                                            (query->value(3).isNull() ? "- null -" : query->value(3).toString()) + " № " +
                                            (query->value(4).isNull() ? "- null -" : query->value(4).toString()) + ",  ВС " +
                                            (query->value(5).isNull() ? "- null -" : query->value(5).toString()) +
                    "\nоткрыт: " + (query->value(6).isNull() ? "- null -" : query->value(6).toDate().toString("dd.MM.yyyy")) +
                    (query->value(7).isNull() ? ",  нет срока действия " : ( query->value(7).toDate()>QDate::currentDate() ? ",  срок действия: " : ",  срок действия истёк: ") + query->value(7).toDate().toString("dd.MM.yyyy")) +
                    ",  дата изъятия: " + (query->value(8).isNull() ? "- null -" : query->value(8).toDate().toString("dd.MM.yyyy")) + "\n"
                    "статус - " + query->value(11).toString();
        }
        QMessageBox::information(this, "П031 - Полис уже есть!", s_out);
        data_event.is_ok = false;
        delete query;
        return;
    }
    delete query;

    // откроем панель свойств
    on_show_pan_vizit_clicked(false);
    on_show_pan_polises_clicked(true);
    on_show_pan_polis_clicked(true);
    ui->pan_vizit->setEnabled(false);
    ui->pan_polises->setEnabled(false);

    ui->split_main->setStretchFactor(0,2);
    ui->split_main->setStretchFactor(1,5);
    ui->split_main->setStretchFactor(2,3);

    refresh_terr_ocato();
    refresh_pol_ves();

    ui->lab_event_code->setText("П031");
    data_event.event_code = "П031";

    ui->line_terr_ocato->setText(data_app.ocato);
    ui->combo_terr_ocato_text->setCurrentIndex(ui->combo_terr_ocato_text->findData(data_app.ocato.toInt()));
    ui->line_ogrn_smo->setText(data_app.smo_ogrn);
    ui->line_ogrn_smo_text->setText(data_app.smo_name);
    ui->combo_pol_v->setCurrentIndex(ui->combo_pol_v->findData(3));
    ui->combo_pol_v->setEnabled(false);

    ui->line_pol_ser->setText("");  // вводятся старая серия и номер полиса
    ui->line_pol_num->setText("");
    ui->ch_pol_sernum->setChecked(true);
    ui->ch_pol_sernum->setEnabled(true);

    ui->ch_vs_num->setChecked(false);    // ВС не выдаётся
    ui->ch_vs_num->setEnabled(false);

    ui->line_enp->setText("");      //вводится старый ЕНП
    ui->ch_pol_enp->setChecked(true);
    ui->ch_pol_enp->setEnabled(true);

    QDate date_start = data_vizit.date;
    //QDate date_start = QDate::currentDate();
    QDate date_end = myAddDays(date_start, 30);
    data_polis.date_end = date_end;
    data_polis.has_date_end = true;
    data_polis.date_end = QDate(1900,1,1);
    data_polis.has_date_stop = false;

    ui->date_pol_begin->setDate(date_start);
    ui->ch_pol_datbegin->setChecked(true);
    ui->ch_pol_datbegin->setEnabled(true);

    ui->date_pol_end->setDate(date_end);
    data_polis.date_end = date_end;
    ui->ch_pol_datend->setChecked(false);
    ui->ch_pol_datend->setEnabled(true);

    ui->date_pol_stop->setDate(QDate(1900,1,1));
    ui->ch_pol_datstop->setChecked(false);
    ui->ch_pol_datstop->setEnabled(true);
    ui->date_pol_stop->setVisible(true);
    on_ch_pol_datstop_stateChanged(0);
    ui->combo_pol_in_erp->setCurrentIndex(0);

    //ui->date_order_date->setDate(date_start);

    //ui->ch_pol_erp->setChecked(false);
    //ui->ch_pol_erp->setEnabled(true);

    ui->lab_point_name->setText(data_app.point_name);
    ui->lab_operator_name->setText(data_app.operator_fio);
    ui->lab_edit_date->setText(" - не сохранён - ");

    // подготовим свойства полиса для новой времянки нового застрахованного
    data_polis.id = -1;
    data_polis.id_person = data_pers.id;
    data_polis.id_old_polis = -1;
    data_polis.pol_type = "П";
    data_polis.pol_v = 3;
    data_event.id_polis = data_polis.id;
    data_event.event_code = "П031";
    data_event.event_dt = QDateTime(ui->date_vizit_pan->date(), QTime::currentTime());
    data_event.event_time = QTime::currentTime();
    data_event.comment = "Замена СМО по желанию один раз в год (без замены полиса) ...";

    // подготовим панель свойств к редактированию данных полиса единого образца
    ui->lab_print_vs->setEnabled(false);
    ui->bn_print_vs->setEnabled(false);
    ui->bn_print_vs_2->setEnabled(false);
    ui->lab_print_ch_smo->setEnabled(true);
    ui->bn_print_ch_smo->setEnabled(true);
    ui->lab_prind_db_pol->setEnabled(false);
    ui->bn_print_db_pol->setEnabled(false);
    ui->lab_print_notice->setEnabled(true);
    ui->bn_print_notice_pol->setEnabled(true);

    test_for_udl_exp();
    test_for_pers_categ();

    ui->line_pol_ser->setFocus();
    b_need_save (true);
}

void polices_wnd::on_bn_p032_clicked() {
    // причина подачи заявления 3
    {
        data_vizit.r_smo = 3;
        ui->combo_vizit_pan_reason_smo->setCurrentIndex(3);
    }
    //заявления на дублкат нет
    {
        data_vizit.r_polis = 0;
        ui->combo_vizit_pan_reason_polis->setCurrentIndex(0);
    }
    // форма полиса 0
    {
        data_vizit.f_polis = 0;
        ui->combo_vizit_pan_form_polis->setCurrentIndex(0);
    }


    // проверка на дату смерти
    if (!test_for_dead()) {
        data_event.is_ok = false;
        return;
    }


    // проверка действующих полисов
    QString sql = "";
    sql += "select p.pol_v, f8.text as pol_v_text, p.pol_type, p.pol_ser, p.pol_num, p.vs_num, p.date_begin, p.date_end, p.date_stop, p.id, p.in_erp, "
           "       coalesce('('||bv_st.code||') '||bv_st.text, '('||bp_st.code||') '||bp_st.text, ' - ??? - ') as status_text "
           " from persons e "
           "     join polises p on( "
           "         (e.id=p.id_person) and "
           "         (p.date_begin<=CURRENT_DATE) and "
           "         (p.date_stop is NULL or p.date_stop>CURRENT_DATE) ) "
           "     left join spr_f008 f8 on(f8.code=p.pol_v) "
           "     left join blanks_pol bp on(bp.id_polis=p.id) "
           "     left join spr_blank_status bp_st on(bp_st.code=bp.status) "
           "     left join blanks_vs bv on(bv.id_polis=p.id) "
           "     left join spr_blank_status bv_st on(bv_st.code=bv.status) "
           " where e.id=" + QString::number(data_pers.id) + " "
           "   and ((bp.id is not NULL and bp.status>0) or (bv.vs_num is not NULL and bv.status>0)) ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString("Проверка полисов застрахованного"), *query, true, db, data_app);
    if (res && query->size()>0) {
        QString s_out = "Данное событие неприемлемо, поскольку у застрахованного уже есть действующий полис(полисы):\n";
        while (query->next()) {
            s_out += "\n(" + QString::number(query->value(0).toInt()) + ") " + query->value(1).toString() +
                    "\nтип/серия/номер: " + (query->value(2).isNull() ? "- null -" : query->value(2).toString()) + ",  " +
                                            (query->value(3).isNull() ? "- null -" : query->value(3).toString()) + " № " +
                                            (query->value(4).isNull() ? "- null -" : query->value(4).toString()) + ",  ВС " +
                                            (query->value(5).isNull() ? "- null -" : query->value(5).toString()) +
                    "\nоткрыт: " + (query->value(6).isNull() ? "- null -" : query->value(6).toDate().toString("dd.MM.yyyy")) +
                    (query->value(7).isNull() ? ",  нет срока действия " : ( query->value(7).toDate()>QDate::currentDate() ? ",  срок действия: " : ",  срок действия истёк: ") + query->value(7).toDate().toString("dd.MM.yyyy")) +
                    ",  дата изъятия: " + (query->value(8).isNull() ? "- null -" : query->value(8).toDate().toString("dd.MM.yyyy")) + "\n"
                    "статус - " + query->value(11).toString();
        }
        QMessageBox::information(this, "П032 - Полис уже есть!", s_out);
        data_event.is_ok = false;
        delete query;
        return;
    }
    delete query;

    // откроем панель свойств
    on_show_pan_vizit_clicked(false);
    on_show_pan_polises_clicked(true);
    on_show_pan_polis_clicked(true);
    ui->pan_vizit->setEnabled(false);
    ui->pan_polises->setEnabled(false);

    ui->split_main->setStretchFactor(0,2);
    ui->split_main->setStretchFactor(1,5);
    ui->split_main->setStretchFactor(2,3);

    refresh_terr_ocato();
    refresh_pol_ves();

    ui->lab_event_code->setText("П032");
    data_event.event_code = "П032";

    ui->line_terr_ocato->setText(data_app.ocato);
    ui->combo_terr_ocato_text->setCurrentIndex(ui->combo_terr_ocato_text->findData(data_app.ocato.toInt()));
    ui->line_ogrn_smo->setText(data_app.smo_ogrn);
    ui->line_ogrn_smo_text->setText(data_app.smo_name);
    ui->combo_pol_v->setCurrentIndex(ui->combo_pol_v->findData(3));
    ui->combo_pol_v->setEnabled(false);

    ui->line_pol_ser->setText("");  // вводятся старая серия и номер полиса
    ui->line_pol_num->setText("");
    ui->ch_pol_sernum->setChecked(true);
    ui->ch_pol_sernum->setEnabled(true);

    ui->ch_vs_num->setChecked(false);    // ВС не выдаётся
    ui->ch_vs_num->setEnabled(false);

    ui->line_enp->setText("");      //вводится старый ЕНП
    ui->ch_pol_enp->setChecked(true);
    ui->ch_pol_enp->setEnabled(true);

    QDate date_start = data_vizit.date;
    //QDate date_start = QDate::currentDate();
    QDate date_end = myAddDays(date_start, 30);
    data_polis.date_end = date_end;
    data_polis.has_date_end = true;
    data_polis.date_end = QDate(1900,1,1);
    data_polis.has_date_stop = false;

    ui->date_pol_begin->setDate(date_start);
    ui->ch_pol_datbegin->setChecked(true);
    ui->ch_pol_datbegin->setEnabled(true);

    ui->date_pol_end->setDate(date_end);
    data_polis.date_end = date_end;
    ui->ch_pol_datend->setChecked(false);
    ui->ch_pol_datend->setEnabled(true);

    ui->date_pol_stop->setDate(QDate(1900,1,1));
    ui->ch_pol_datstop->setChecked(false);
    ui->ch_pol_datstop->setEnabled(true);
    ui->date_pol_stop->setVisible(true);
    on_ch_pol_datstop_stateChanged(0);
    ui->combo_pol_in_erp->setCurrentIndex(0);

    //ui->date_order_date->setDate(date_start);

    //ui->ch_pol_erp->setChecked(false);
    //ui->ch_pol_erp->setEnabled(true);

    ui->lab_point_name->setText(data_app.point_name);
    ui->lab_operator_name->setText(data_app.operator_fio);
    ui->lab_edit_date->setText(" - не сохранён - ");

    // подготовим свойства полиса для новой времянки нового застрахованного
    data_polis.id = -1;
    data_polis.id_person = data_pers.id;
    data_polis.id_old_polis = -1;
    data_polis.pol_type = "П";
    data_polis.pol_v = 3;
    data_event.id_polis = data_polis.id;
    data_event.event_code = "П032";
    data_event.event_dt = QDateTime(ui->date_vizit_pan->date(), QTime::currentTime());
    data_event.event_time = QTime::currentTime();
    data_event.comment = "Замена СМО по желанию один раз в год (без замены полиса) ...";

    // подготовим панель свойств к редактированию данных полиса единого образца
    ui->lab_print_vs->setEnabled(false);
    ui->bn_print_vs->setEnabled(false);
    ui->bn_print_vs_2->setEnabled(false);
    ui->lab_print_ch_smo->setEnabled(true);
    ui->bn_print_ch_smo->setEnabled(true);
    ui->lab_prind_db_pol->setEnabled(false);
    ui->bn_print_db_pol->setEnabled(false);
    ui->lab_print_notice->setEnabled(true);
    ui->bn_print_notice_pol->setEnabled(true);

    test_for_udl_exp();
    test_for_pers_categ();

    ui->line_pol_ser->setFocus();
    b_need_save (true);
}

void polices_wnd::on_bn_p033_clicked() {
    // причина подачи заявления 4
    {
        data_vizit.r_smo = 4;
        ui->combo_vizit_pan_reason_smo->setCurrentIndex(4);
    }
    //заявления на дублкат нет
    {
        data_vizit.r_polis = 0;
        ui->combo_vizit_pan_reason_polis->setCurrentIndex(0);
    }
    // форма полиса 0
    {
        data_vizit.f_polis = 0;
        ui->combo_vizit_pan_form_polis->setCurrentIndex(0);
    }


    // проверка на дату смерти
    if (!test_for_dead()) {
        data_event.is_ok = false;
        return;
    }


    // проверка действующих полисов
    QString sql = "";
    sql += "select p.pol_v, f8.text as pol_v_text, p.pol_type, p.pol_ser, p.pol_num, p.vs_num, p.date_begin, p.date_end, p.date_stop, p.id, p.in_erp, "
           "       coalesce('('||bv_st.code||') '||bv_st.text, '('||bp_st.code||') '||bp_st.text, ' - ??? - ') as status_text "
           " from persons e "
           "     join polises p on( "
           "         (e.id=p.id_person) and "
           "         (p.date_begin<=CURRENT_DATE) and "
           "         (p.date_stop is NULL or p.date_stop>CURRENT_DATE) ) "
           "     left join spr_f008 f8 on(f8.code=p.pol_v) "
           "     left join blanks_pol bp on(bp.id_polis=p.id) "
           "     left join spr_blank_status bp_st on(bp_st.code=bp.status) "
           "     left join blanks_vs bv on(bv.id_polis=p.id) "
           "     left join spr_blank_status bv_st on(bv_st.code=bv.status) "
           " where e.id=" + QString::number(data_pers.id) + " "
           "   and ((bp.id is not NULL and bp.status>0) or (bv.vs_num is not NULL and bv.status>0)) ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString("Проверка полисов застрахованного"), *query, true, db, data_app);
    if (res && query->size()>0) {
        QString s_out = "Данное событие неприемлемо, поскольку у застрахованного уже есть действующий полис(полисы):\n";
        while (query->next()) {
            s_out += "\n(" + QString::number(query->value(0).toInt()) + ") " + query->value(1).toString() +
                    "\nтип/серия/номер: " + (query->value(2).isNull() ? "- null -" : query->value(2).toString()) + ",  " +
                                            (query->value(3).isNull() ? "- null -" : query->value(3).toString()) + " № " +
                                            (query->value(4).isNull() ? "- null -" : query->value(4).toString()) + ",  ВС " +
                                            (query->value(5).isNull() ? "- null -" : query->value(5).toString()) +
                    "\nоткрыт: " + (query->value(6).isNull() ? "- null -" : query->value(6).toDate().toString("dd.MM.yyyy")) +
                    (query->value(7).isNull() ? ",  нет срока действия " : ( query->value(7).toDate()>QDate::currentDate() ? ",  срок действия: " : ",  срок действия истёк: ") + query->value(7).toDate().toString("dd.MM.yyyy")) +
                    ",  дата изъятия: " + (query->value(8).isNull() ? "- null -" : query->value(8).toDate().toString("dd.MM.yyyy")) + "\n"
                    "статус - " + query->value(11).toString();
        }
        QMessageBox::information(this, "П033 - Полис уже есть!", s_out);
        data_event.is_ok = false;
        delete query;
        return;
    }
    delete query;

    // откроем панель свойств
    on_show_pan_vizit_clicked(false);
    on_show_pan_polises_clicked(true);
    on_show_pan_polis_clicked(true);
    ui->pan_vizit->setEnabled(false);
    ui->pan_polises->setEnabled(false);

    ui->split_main->setStretchFactor(0,2);
    ui->split_main->setStretchFactor(1,5);
    ui->split_main->setStretchFactor(2,3);

    refresh_terr_ocato();
    refresh_pol_ves();

    ui->lab_event_code->setText("П033");
    data_event.event_code = "П033";

    ui->line_terr_ocato->setText(data_app.ocato);
    ui->combo_terr_ocato_text->setCurrentIndex(ui->combo_terr_ocato_text->findData(data_app.ocato.toInt()));
    ui->line_ogrn_smo->setText(data_app.smo_ogrn);
    ui->line_ogrn_smo_text->setText(data_app.smo_name);
    ui->combo_pol_v->setCurrentIndex(ui->combo_pol_v->findData(3));
    ui->combo_pol_v->setEnabled(false);

    ui->line_pol_ser->setText("");  // вводятся старая серия и номер полиса
    ui->line_pol_num->setText("");
    ui->ch_pol_sernum->setChecked(true);
    ui->ch_pol_sernum->setEnabled(true);

    ui->ch_vs_num->setChecked(false);    // ВС не выдаётся
    ui->ch_vs_num->setEnabled(false);

    ui->line_enp->setText("");      //вводится старый ЕНП
    ui->ch_pol_enp->setChecked(true);
    ui->ch_pol_enp->setEnabled(true);

    QDate date_start = data_vizit.date;
    //QDate date_start = QDate::currentDate();
    QDate date_end = myAddDays(date_start, 30);
    data_polis.date_end = date_end;
    data_polis.has_date_end = true;
    data_polis.date_end = QDate(1900,1,1);
    data_polis.has_date_stop = false;

    ui->date_pol_begin->setDate(date_start);
    ui->ch_pol_datbegin->setChecked(true);
    ui->ch_pol_datbegin->setEnabled(true);

    ui->date_pol_end->setDate(date_end);
    data_polis.date_end = date_end;
    ui->ch_pol_datend->setChecked(false);
    ui->ch_pol_datend->setEnabled(true);

    ui->date_pol_stop->setDate(QDate(1900,1,1));
    ui->ch_pol_datstop->setChecked(false);
    ui->ch_pol_datstop->setEnabled(true);
    ui->date_pol_stop->setVisible(true);
    on_ch_pol_datstop_stateChanged(0);
    ui->combo_pol_in_erp->setCurrentIndex(0);

    //ui->date_order_date->setDate(date_start);

    //ui->ch_pol_erp->setChecked(false);
    //ui->ch_pol_erp->setEnabled(true);

    ui->lab_point_name->setText(data_app.point_name);
    ui->lab_operator_name->setText(data_app.operator_fio);
    ui->lab_edit_date->setText(" - не сохранён - ");

    // подготовим свойства полиса для новой времянки нового застрахованного
    data_polis.id = -1;
    data_polis.id_person = data_pers.id;
    data_polis.id_old_polis = -1;
    data_polis.pol_type = "П";
    data_polis.pol_v = 3;
    data_event.id_polis = data_polis.id;
    data_event.event_code = "П033";
    data_event.event_dt = QDateTime(ui->date_vizit_pan->date(), QTime::currentTime());
    data_event.event_time = QTime::currentTime();
    data_event.comment = "Замена СМО по желанию один раз в год (без замены полиса) ...";

    // подготовим панель свойств к редактированию данных полиса единого образца
    ui->lab_print_vs->setEnabled(false);
    ui->bn_print_vs->setEnabled(false);
    ui->bn_print_vs_2->setEnabled(false);
    ui->lab_print_ch_smo->setEnabled(true);
    ui->bn_print_ch_smo->setEnabled(true);
    ui->lab_prind_db_pol->setEnabled(false);
    ui->bn_print_db_pol->setEnabled(false);
    ui->lab_print_notice->setEnabled(true);
    ui->bn_print_notice_pol->setEnabled(true);

    test_for_udl_exp();
    test_for_pers_categ();

    ui->line_pol_ser->setFocus();
    b_need_save (true);
}

// -------------------------------------------------------
// Замена СМО по желанию один раз в год (с заменой полиса)
// -------------------------------------------------------
void polices_wnd::on_bn_p034_clicked() {
    // с 1 ноября по 31 декабря недоступно
    int month = QDate::currentDate().month();
    if (month==11 || month==12) {
        if (QMessageBox::warning(this, "Перестрахование временно недоступно",
                                 "Перестрахование клиентов по П034 нельзя проводить с 1 ноября по 31 декабряя.\n"
                                 "Если операция будет продолжена, застрахованный получит ВС, но полис единого образца будет изготовлен только в январе следующего года.\n\n"
                                 "Продолжить?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::No) {
            return;
        }
    }

    // причина подачи заявления 2
    {
        data_vizit.r_smo = 2;
        ui->combo_vizit_pan_reason_smo->setCurrentIndex(2);
    }
    //заявления на дублкат нет
    {
        data_vizit.r_polis = 0;
        ui->combo_vizit_pan_reason_polis->setCurrentIndex(0);
    }
    // форма полиса >=1
    if (data_vizit.f_polis==0) {
        data_vizit.f_polis = 1;
        ui->combo_vizit_pan_form_polis->setCurrentIndex(1);
    }


    // проверка на дату смерти
    if (!test_for_dead()) {
        data_event.is_ok = false;
        return;
    }


    // проверка действующих полисов
    QString sql = "";
    sql += "select p.pol_v, f8.text as pol_v_text, p.pol_type, p.pol_ser, p.pol_num, p.vs_num, p.date_begin, p.date_end, p.date_stop, p.id, p.in_erp, "
           "       coalesce('('||bv_st.code||') '||bv_st.text, '('||bp_st.code||') '||bp_st.text, ' - ??? - ') as status_text "
           " from persons e "
           "     join polises p on( "
           "         (e.id=p.id_person) and "
           "         (p.date_begin<=CURRENT_DATE) and "
           "         (p.date_stop is NULL or p.date_stop>CURRENT_DATE) ) "
           "     left join spr_f008 f8 on(f8.code=p.pol_v) "
           "     left join blanks_pol bp on(bp.id_polis=p.id) "
           "     left join spr_blank_status bp_st on(bp_st.code=bp.status) "
           "     left join blanks_vs bv on(bv.id_polis=p.id) "
           "     left join spr_blank_status bv_st on(bv_st.code=bv.status) "
           " where e.id=" + QString::number(data_pers.id) + " "
           "   and ((bp.id is not NULL and bp.status>0) or (bv.vs_num is not NULL and bv.status>0)) ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString("Проверка полисов застрахованного"), *query, true, db, data_app);
    if (res && query->size()>0) {
        QString s_out = "Данное событие неприемлемо, поскольку у застрахованного уже есть действующий полис(полисы):\n";
        while (query->next()) {
            s_out += "\n(" + QString::number(query->value(0).toInt()) + ") " + query->value(1).toString() +
                    "\nтип/серия/номер: " + (query->value(2).isNull() ? "- null -" : query->value(2).toString()) + ",  " +
                                            (query->value(3).isNull() ? "- null -" : query->value(3).toString()) + " № " +
                                            (query->value(4).isNull() ? "- null -" : query->value(4).toString()) + ",  ВС " +
                                            (query->value(5).isNull() ? "- null -" : query->value(5).toString()) +
                    "\nоткрыт: " + (query->value(6).isNull() ? "- null -" : query->value(6).toDate().toString("dd.MM.yyyy")) +
                    (query->value(7).isNull() ? ",  нет срока действия " : ( query->value(7).toDate()>QDate::currentDate() ? ",  срок действия: " : ",  срок действия истёк: ") + query->value(7).toDate().toString("dd.MM.yyyy")) +
                    ",  дата изъятия: " + (query->value(8).isNull() ? "- null -" : query->value(8).toDate().toString("dd.MM.yyyy")) + "\n"
                    "статус - " + query->value(11).toString();
        }
        QMessageBox::information(this, "П034 - Полис уже есть!", s_out);
        data_event.is_ok = false;
        delete query;
        return;
    }
    delete query;

    // откроем панель свойств
    on_show_pan_vizit_clicked(false);
    on_show_pan_polises_clicked(true);
    on_show_pan_polis_clicked(true);
    ui->pan_vizit->setEnabled(false);
    ui->pan_polises->setEnabled(false);

    ui->split_main->setStretchFactor(0,2);
    ui->split_main->setStretchFactor(1,5);
    ui->split_main->setStretchFactor(2,3);

    refresh_terr_ocato();
    refresh_pol_ves();

    ui->lab_event_code->setText("П034");
    data_event.event_code = "П034";

    ui->line_terr_ocato->setText(data_app.ocato);
    ui->combo_terr_ocato_text->setCurrentIndex(ui->combo_terr_ocato_text->findData(data_app.ocato.toInt()));
    ui->line_ogrn_smo->setText(data_app.smo_ogrn);
    ui->line_ogrn_smo_text->setText(data_app.smo_name);
    ui->combo_pol_v->setCurrentIndex(ui->combo_pol_v->findData(2));
    ui->combo_pol_v->setEnabled(false);

    ui->line_pol_ser->setText("");
    ui->line_pol_num->setText("");
    ui->ch_pol_sernum->setChecked(false);
    ui->ch_pol_sernum->setEnabled(false);

    ui->ch_vs_num->setChecked(true);
    ui->ch_vs_num->setEnabled(true);

    ui->line_enp->setText("");
    ui->ch_pol_enp->setChecked(false);
    ui->ch_pol_enp->setEnabled(false);

    QDate date_start = data_vizit.date;
    //QDate date_start = QDate::currentDate();
    QDate date_end = myAddDays(date_start, 30);
    data_polis.date_end = date_end;
    data_polis.has_date_end = true;
    data_polis.date_end = QDate(1900,1,1);
    data_polis.has_date_stop = false;

    ui->date_pol_begin->setDate(date_start);
    ui->ch_pol_datbegin->setChecked(true);
    ui->ch_pol_datbegin->setEnabled(true);

    ui->date_pol_end->setDate(date_end);
    data_polis.date_end = date_end;
    ui->ch_pol_datend->setChecked(true);
    ui->ch_pol_datend->setEnabled(true);

    ui->date_pol_stop->setDate(QDate(1900,1,1));
    ui->ch_pol_datstop->setChecked(false);
    ui->ch_pol_datstop->setEnabled(true);
    ui->date_pol_stop->setVisible(true);
    on_ch_pol_datstop_stateChanged(0);
    ui->combo_pol_in_erp->setCurrentIndex(0);

    //ui->date_order_date->setDate(date_start);

    //ui->ch_pol_erp->setChecked(false);
    //ui->ch_pol_erp->setEnabled(true);

    ui->lab_point_name->setText(data_app.point_name);
    ui->lab_operator_name->setText(data_app.operator_fio);
    ui->lab_edit_date->setText(" - не сохранён - ");

    // подготовим свойства полиса для новой времянки нового застрахованного
    data_polis.id = -1;
    data_polis.id_person = data_pers.id;
    data_polis.id_old_polis = -1;
    data_polis.pol_type = "В";
    data_polis.pol_v = 2;
    data_polis.f_polis = 1;
    data_event.id_polis = data_polis.id;
    data_event.event_code = "П034";
    data_event.event_dt = QDateTime(ui->date_vizit_pan->date(), QTime::currentTime());
    data_event.event_time = QTime::currentTime();
    data_event.comment = "Замена СМО по желанию один раз в год (с заменой полиса) ...";

    // подготовим панель свойств к редактированию данных ВС
    ui->lab_print_vs->setEnabled(true);
    ui->bn_print_vs->setEnabled(true);
    ui->bn_print_vs_2->setEnabled(true);
    ui->lab_print_ch_smo->setEnabled(true);
    ui->bn_print_ch_smo->setEnabled(true);
    ui->lab_prind_db_pol->setEnabled(false);
    ui->bn_print_db_pol->setEnabled(false);
    ui->lab_print_notice->setEnabled(true);
    ui->bn_print_notice_pol->setEnabled(true);

    test_for_udl_exp();
    test_for_pers_categ();

    ui->line_pol_ser->setFocus();
    b_need_save (true);
}

// --------------------------------------------------------
// Замена СМО при смене места жительства (с заменой полиса)
// --------------------------------------------------------
void polices_wnd::on_bn_p035_clicked() {
    // причина подачи заявления 3
    {
        data_vizit.r_smo = 3;
        ui->combo_vizit_pan_reason_smo->setCurrentIndex(3);
    }
    //заявления на дублкат нет
    {
        data_vizit.r_polis = 0;
        ui->combo_vizit_pan_reason_polis->setCurrentIndex(0);
    }
    // форма полиса >=1
    if (data_vizit.f_polis==0) {
        data_vizit.f_polis = 1;
        ui->combo_vizit_pan_form_polis->setCurrentIndex(1);
    }


    // проверка на дату смерти
    if (!test_for_dead()) {
        data_event.is_ok = false;
        return;
    }


    // проверка действующих полисов
    QString sql = "";
    sql += "select p.pol_v, f8.text as pol_v_text, p.pol_type, p.pol_ser, p.pol_num, p.vs_num, p.date_begin, p.date_end, p.date_stop, p.id, p.in_erp, "
           "       coalesce('('||bv_st.code||') '||bv_st.text, '('||bp_st.code||') '||bp_st.text, ' - ??? - ') as status_text "
           " from persons e "
           "     join polises p on( "
           "         (e.id=p.id_person) and "
           "         (p.date_begin<=CURRENT_DATE) and "
           "         (p.date_stop is NULL or p.date_stop>CURRENT_DATE) ) "
           "     left join spr_f008 f8 on(f8.code=p.pol_v) "
           "     left join blanks_pol bp on(bp.id_polis=p.id) "
           "     left join spr_blank_status bp_st on(bp_st.code=bp.status) "
           "     left join blanks_vs bv on(bv.id_polis=p.id) "
           "     left join spr_blank_status bv_st on(bv_st.code=bv.status) "
           " where e.id=" + QString::number(data_pers.id) + " "
           "   and ((bp.id is not NULL and bp.status>0) or (bv.vs_num is not NULL and bv.status>0)) ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString("Проверка полисов застрахованного"), *query, true, db, data_app);
    if (res && query->size()>0) {
        QString s_out = "Данное событие неприемлемо, поскольку у застрахованного уже есть действующий полис(полисы):\n";
        while (query->next()) {
            s_out += "\n(" + QString::number(query->value(0).toInt()) + ") " + query->value(1).toString() +
                    "\nтип/серия/номер: " + (query->value(2).isNull() ? "- null -" : query->value(2).toString()) + ",  " +
                                            (query->value(3).isNull() ? "- null -" : query->value(3).toString()) + " № " +
                                            (query->value(4).isNull() ? "- null -" : query->value(4).toString()) + ",  ВС " +
                                            (query->value(5).isNull() ? "- null -" : query->value(5).toString()) +
                    "\nоткрыт: " + (query->value(6).isNull() ? "- null -" : query->value(6).toDate().toString("dd.MM.yyyy")) +
                    (query->value(7).isNull() ? ",  нет срока действия " : ( query->value(7).toDate()>QDate::currentDate() ? ",  срок действия: " : ",  срок действия истёк: ") + query->value(7).toDate().toString("dd.MM.yyyy")) +
                    ",  дата изъятия: " + (query->value(8).isNull() ? "- null -" : query->value(8).toDate().toString("dd.MM.yyyy")) + "\n"
                    "статус - " + query->value(11).toString();
        }
        QMessageBox::information(this, "П035 - Полис уже есть!", s_out);
        data_event.is_ok = false;
        delete query;
        return;
    }
    delete query;

    // откроем панель свойств
    on_show_pan_vizit_clicked(false);
    on_show_pan_polises_clicked(true);
    on_show_pan_polis_clicked(true);
    ui->pan_vizit->setEnabled(false);
    ui->pan_polises->setEnabled(false);

    ui->split_main->setStretchFactor(0,2);
    ui->split_main->setStretchFactor(1,5);
    ui->split_main->setStretchFactor(2,3);

    refresh_terr_ocato();
    refresh_pol_ves();

    ui->lab_event_code->setText("П035");
    data_event.event_code = "П035";

    ui->line_terr_ocato->setText(data_app.ocato);
    ui->combo_terr_ocato_text->setCurrentIndex(ui->combo_terr_ocato_text->findData(data_app.ocato.toInt()));
    ui->line_ogrn_smo->setText(data_app.smo_ogrn);
    ui->line_ogrn_smo_text->setText(data_app.smo_name);
    ui->combo_pol_v->setCurrentIndex(ui->combo_pol_v->findData(2));
    ui->combo_pol_v->setEnabled(false);

    ui->line_pol_ser->setText("");
    ui->line_pol_num->setText("");
    ui->ch_pol_sernum->setChecked(false);
    ui->ch_pol_sernum->setEnabled(false);

    ui->ch_vs_num->setChecked(true);
    ui->ch_vs_num->setEnabled(true);

    ui->line_enp->setText("");
    ui->ch_pol_enp->setChecked(false);
    ui->ch_pol_enp->setEnabled(false);

    QDate date_start = data_vizit.date;
    //QDate date_start = QDate::currentDate();
    QDate date_end = myAddDays(date_start, 30);
    data_polis.date_end = date_end;
    data_polis.has_date_end = true;
    data_polis.date_end = QDate(1900,1,1);
    data_polis.has_date_stop = false;

    ui->date_pol_begin->setDate(date_start);
    ui->ch_pol_datbegin->setChecked(true);
    ui->ch_pol_datbegin->setEnabled(true);

    ui->date_pol_end->setDate(date_end);
    data_polis.date_end = date_end;
    ui->ch_pol_datend->setChecked(true);
    ui->ch_pol_datend->setEnabled(true);

    ui->date_pol_stop->setDate(QDate(1900,1,1));
    ui->ch_pol_datstop->setChecked(false);
    ui->ch_pol_datstop->setEnabled(true);
    ui->date_pol_stop->setVisible(true);
    on_ch_pol_datstop_stateChanged(0);
    ui->combo_pol_in_erp->setCurrentIndex(0);

    //ui->date_order_date->setDate(date_start);

    //ui->ch_pol_erp->setChecked(false);
    //ui->ch_pol_erp->setEnabled(true);

    ui->lab_point_name->setText(data_app.point_name);
    ui->lab_operator_name->setText(data_app.operator_fio);
    ui->lab_edit_date->setText(" - не сохранён - ");

    // подготовим свойства полиса для новой времянки нового застрахованного
    data_polis.id = -1;
    data_polis.id_person = data_pers.id;
    data_polis.id_old_polis = -1;
    data_polis.pol_type = "В";
    data_polis.pol_v = 2;
    data_polis.f_polis = 1;
    data_event.id_polis = data_polis.id;
    data_event.event_code = "П035";
    data_event.event_dt = QDateTime(ui->date_vizit_pan->date(), QTime::currentTime());
    data_event.event_time = QTime::currentTime();
    data_event.comment = "Замена СМО при смене места жительства (с заменой полиса) ...";

    // подготовим панель свойств к редактированию данных ВС
    ui->lab_print_vs->setEnabled(true);
    ui->bn_print_vs->setEnabled(true);
    ui->bn_print_vs_2->setEnabled(true);
    ui->lab_print_ch_smo->setEnabled(true);
    ui->bn_print_ch_smo->setEnabled(true);
    ui->lab_prind_db_pol->setEnabled(false);
    ui->bn_print_db_pol->setEnabled(false);
    ui->lab_print_notice->setEnabled(true);
    ui->bn_print_notice_pol->setEnabled(true);

    test_for_udl_exp();
    test_for_pers_categ();

    ui->line_pol_ser->setFocus();
    b_need_save (true);
}

// --------------------------------------------------------------------
// Замена СМО в связи с расторжением договора со СМО (с заменой полиса)
// --------------------------------------------------------------------
void polices_wnd::on_bn_p036_clicked() {
    // причина подачи заявления 4
    {
        data_vizit.r_smo = 4;
        ui->combo_vizit_pan_reason_smo->setCurrentIndex(4);
    }
    //заявления на дублкат нет
    {
        data_vizit.r_polis = 0;
        ui->combo_vizit_pan_reason_polis->setCurrentIndex(0);
    }
    // форма полиса >=1
    if (data_vizit.f_polis==0) {
        data_vizit.f_polis = 1;
        ui->combo_vizit_pan_form_polis->setCurrentIndex(1);
    }


    // проверка на дату смерти
    if (!test_for_dead()) {
        data_event.is_ok = false;
        return;
    }


    // проверка действующих полисов
    QString sql = "";
    sql += "select p.pol_v, f8.text as pol_v_text, p.pol_type, p.pol_ser, p.pol_num, p.vs_num, p.date_begin, p.date_end, p.date_stop, p.id, p.in_erp, "
           "       coalesce('('||bv_st.code||') '||bv_st.text, '('||bp_st.code||') '||bp_st.text, ' - ??? - ') as status_text "
           " from persons e "
           "     join polises p on( "
           "         (e.id=p.id_person) and "
           "         (p.date_begin<=CURRENT_DATE) and "
           "         (p.date_stop is NULL or p.date_stop>CURRENT_DATE) ) "
           "     left join spr_f008 f8 on(f8.code=p.pol_v) "
           "     left join blanks_pol bp on(bp.id_polis=p.id) "
           "     left join spr_blank_status bp_st on(bp_st.code=bp.status) "
           "     left join blanks_vs bv on(bv.id_polis=p.id) "
           "     left join spr_blank_status bv_st on(bv_st.code=bv.status) "
           " where e.id=" + QString::number(data_pers.id) + " "
           "   and ((bp.id is not NULL and bp.status>0) or (bv.vs_num is not NULL and bv.status>0)) ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString("Проверка полисов застрахованного"), *query, true, db, data_app);
    if (res && query->size()>0) {
        QString s_out = "Данное событие неприемлемо, поскольку у застрахованного уже есть действующий полис(полисы):\n";
        while (query->next()) {
            s_out += "\n(" + QString::number(query->value(0).toInt()) + ") " + query->value(1).toString() +
                    "\nтип/серия/номер: " + (query->value(2).isNull() ? "- null -" : query->value(2).toString()) + ",  " +
                                            (query->value(3).isNull() ? "- null -" : query->value(3).toString()) + " № " +
                                            (query->value(4).isNull() ? "- null -" : query->value(4).toString()) + ",  ВС " +
                                            (query->value(5).isNull() ? "- null -" : query->value(5).toString()) +
                    "\nоткрыт: " + (query->value(6).isNull() ? "- null -" : query->value(6).toDate().toString("dd.MM.yyyy")) +
                    (query->value(7).isNull() ? ",  нет срока действия " : ( query->value(7).toDate()>QDate::currentDate() ? ",  срок действия: " : ",  срок действия истёк: ") + query->value(7).toDate().toString("dd.MM.yyyy")) +
                    ",  дата изъятия: " + (query->value(8).isNull() ? "- null -" : query->value(8).toDate().toString("dd.MM.yyyy")) + "\n"
                    "статус - " + query->value(11).toString();
        }
        QMessageBox::information(this, "П036 - Полис уже есть!", s_out);
        data_event.is_ok = false;
        delete query;
        return;
    }
    delete query;

    // откроем панель свойств
    on_show_pan_vizit_clicked(false);
    on_show_pan_polises_clicked(true);
    on_show_pan_polis_clicked(true);
    ui->pan_vizit->setEnabled(false);
    ui->pan_polises->setEnabled(false);

    ui->split_main->setStretchFactor(0,2);
    ui->split_main->setStretchFactor(1,5);
    ui->split_main->setStretchFactor(2,3);

    refresh_terr_ocato();
    refresh_pol_ves();

    ui->lab_event_code->setText("П036");
    data_event.event_code = "П036";

    ui->line_terr_ocato->setText(data_app.ocato);
    ui->combo_terr_ocato_text->setCurrentIndex(ui->combo_terr_ocato_text->findData(data_app.ocato.toInt()));
    ui->line_ogrn_smo->setText(data_app.smo_ogrn);
    ui->line_ogrn_smo_text->setText(data_app.smo_name);
    ui->combo_pol_v->setCurrentIndex(ui->combo_pol_v->findData(2));
    ui->combo_pol_v->setEnabled(false);

    ui->line_pol_ser->setText("");
    ui->line_pol_num->setText("");
    ui->ch_pol_sernum->setChecked(false);
    ui->ch_pol_sernum->setEnabled(false);

    ui->ch_vs_num->setChecked(true);
    ui->ch_vs_num->setEnabled(true);

    ui->line_enp->setText("");
    ui->ch_pol_enp->setChecked(false);
    ui->ch_pol_enp->setEnabled(false);

    QDate date_start = data_vizit.date;
    //QDate date_start = QDate::currentDate();
    QDate date_end = myAddDays(date_start, 30);
    data_polis.date_end = date_end;
    data_polis.has_date_end = true;
    data_polis.date_end = QDate(1900,1,1);
    data_polis.has_date_stop = false;

    ui->date_pol_begin->setDate(date_start);
    ui->ch_pol_datbegin->setChecked(true);
    ui->ch_pol_datbegin->setEnabled(true);

    ui->date_pol_end->setDate(date_end);
    data_polis.date_end = date_end;
    ui->ch_pol_datend->setChecked(true);
    ui->ch_pol_datend->setEnabled(true);

    ui->date_pol_stop->setDate(QDate(1900,1,1));
    ui->ch_pol_datstop->setChecked(false);
    ui->ch_pol_datstop->setEnabled(true);
    ui->date_pol_stop->setVisible(true);
    on_ch_pol_datstop_stateChanged(0);
    ui->combo_pol_in_erp->setCurrentIndex(0);

    //ui->date_order_date->setDate(date_start);

    //ui->ch_pol_erp->setChecked(false);
    //ui->ch_pol_erp->setEnabled(true);

    ui->lab_point_name->setText(data_app.point_name);
    ui->lab_operator_name->setText(data_app.operator_fio);
    ui->lab_edit_date->setText(" - не сохранён - ");

    // подготовим свойства полиса для новой времянки нового застрахованного
    data_polis.id = -1;
    data_polis.id_person = data_pers.id;
    data_polis.id_old_polis = -1;
    data_polis.pol_type = "В";
    data_polis.pol_v = 2;
    data_polis.f_polis = 1;
    data_event.id_polis = data_polis.id;
    data_event.event_code = "П036";
    data_event.event_dt = QDateTime(ui->date_vizit_pan->date(), QTime::currentTime());
    data_event.event_time = QTime::currentTime();
    data_event.comment = "Замена СМО в связи с расторжением договора со СМО (с заменой полиса) ...";

    // подготовим панель свойств к редактированию данных ВС
    ui->lab_print_vs->setEnabled(true);
    ui->bn_print_vs->setEnabled(true);
    ui->bn_print_vs_2->setEnabled(true);
    ui->lab_print_ch_smo->setEnabled(true);
    ui->bn_print_ch_smo->setEnabled(true);
    ui->lab_prind_db_pol->setEnabled(false);
    ui->bn_print_db_pol->setEnabled(false);
    ui->lab_print_notice->setEnabled(true);
    ui->bn_print_notice_pol->setEnabled(true);

    test_for_udl_exp();
    test_for_pers_categ();

    ui->line_pol_ser->setFocus();
    b_need_save (true);
}

void polices_wnd::on_bn_p060_clicked() {
    QDate date_vs = QDate::currentDate();
    //заявления на дубликат нет
    {
        data_vizit.r_polis = 0;
        data_polis.r_polis = 0;
        ui->combo_vizit_pan_reason_polis->setCurrentIndex(0);
    }
    // форма полиса >=1
    if (data_vizit.f_polis==0) {
        data_vizit.f_polis = 1;
        ui->combo_vizit_pan_form_polis->setCurrentIndex(1);
    }


    // проверка на дату смерти
    if (!test_for_dead()) {
        data_event.is_ok = false;
        return;
    }


    // проверка действующих полисов
    QString sql = "";
    sql += "select p.pol_v, f8.text as pol_v_text, p.pol_type, p.pol_ser, p.pol_num, p.vs_num, p.date_begin, p.date_end, p.date_stop, p.id, p.in_erp, "
           "       coalesce('('||bv_st.code||') '||bv_st.text, '('||bp_st.code||') '||bp_st.text, ' - ??? - ') as status_text, "
           "       e.date_death "       // контрольный запрос - дата смерти
           " from persons e "
           "     join polises p on( "
           "         (e.id=p.id_person) and "
           "         (p.date_begin<=CURRENT_DATE) and "
           "         (p.date_stop is NULL or p.date_stop>CURRENT_DATE) ) "
           "     left join spr_f008 f8 on(f8.code=p.pol_v) "
           "     left join blanks_pol bp on(bp.id_polis=p.id) "
           "     left join spr_blank_status bp_st on(bp_st.code=bp.status) "
           "     left join blanks_vs bv on(bv.id_polis=p.id) "
           "     left join spr_blank_status bv_st on(bv_st.code=bv.status) "
           " where e.id=" + QString::number(data_pers.id) + " "
           "   and ((bp.id is not NULL and bp.status>0) or (bv.vs_num is not NULL and bv.status>0)) "
           "   and p.pol_v<>2 ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString("Проверка полисов застрахованного"), *query, true, db, data_app);
    if (res && query->size()>0) {
        QString s_out = "Данное событие неприемлемо, поскольку у застрахованного уже есть действующий полис(полисы) единого образца:\n";
        while (query->next()) {
            // проверка, не умер ли застрахованный
            QDate date_death = query->value(14).toDate();
            if (!query->value(14).isNull()) {
                // застрахованный уже умер X((
                QMessageBox::warning(this, "Он уже умер  X(_",
                                     "Этот застрахованный уже умер.\n\n"
                                     "Дата смерти  " + date_death.toString("yyyy-MM-dd"));
                break;
            }
            s_out += "\n(" + QString::number(query->value(0).toInt()) + ") " + query->value(1).toString() +
                     "\nтип/серия/номер: " + (query->value(2).isNull() ? "- null -" : query->value(2).toString()) + ",  " +
                                             (query->value(3).isNull() ? "- null -" : query->value(3).toString()) + " № " +
                                             (query->value(4).isNull() ? "- null -" : query->value(4).toString()) + ",  ВС " +
                                             (query->value(5).isNull() ? "- null -" : query->value(5).toString()) +
                     "\nоткрыт: " + (query->value(6).isNull() ? "- null -" : query->value(6).toDate().toString("dd.MM.yyyy")) +
                    (query->value(7).isNull() ? ",  нет срока действия " : ( query->value(7).toDate()>QDate::currentDate() ? ",  срок действия: " : ",  срок действия истёк: ") + query->value(7).toDate().toString("dd.MM.yyyy")) +
                    ",  дата изъятия: " + (query->value(8).isNull() ? "- null -" : query->value(8).toDate().toString("dd.MM.yyyy")) + "\n"
                    "статус - " + query->value(11).toString();

            date_vs = query->value(6).toDate();
        }
        QMessageBox::information(this, "П031 - Полис уже есть!", s_out);
        data_event.is_ok = false;
        delete query;
        return;
    }
    delete query;


    // откроем панель свойств
    on_show_pan_vizit_clicked(false);
    on_show_pan_polises_clicked(true);
    on_show_pan_polis_clicked(true);
    ui->pan_vizit->setEnabled(false);
    ui->pan_polises->setEnabled(false);

    ui->split_main->setStretchFactor(0,2);
    ui->split_main->setStretchFactor(1,5);
    ui->split_main->setStretchFactor(2,3);

    refresh_terr_ocato();
    refresh_pol_ves();

    if (ui->ch_get2hand->isChecked()) {
        ui->lab_event_code->setText("П060");
        data_event.event_code = "П060";
        QPalette pal = ui->lab_event_code->palette();
        pal.setColor(QPalette::WindowText,QColor(0,0,0));
        ui->lab_event_code->setPalette(pal);
        QFont font = ui->lab_event_code->font();
        font.setItalic(false);
        ui->lab_event_code->setFont(font);
    } else {
        ui->lab_event_code->setText("П060  (без выдачи на руки)");
        data_event.event_code = "П060";
        QPalette pal = ui->lab_event_code->palette();
        pal.setColor(QPalette::WindowText,QColor(255,0,0));
        ui->lab_event_code->setPalette(pal);
        QFont font = ui->lab_event_code->font();
        font.setItalic(true);
        ui->lab_event_code->setFont(font);
    }

    ui->line_terr_ocato->setText(data_app.ocato);
    ui->combo_terr_ocato_text->setCurrentIndex(ui->combo_terr_ocato_text->findData(data_app.ocato.toInt()));
    ui->line_ogrn_smo->setText(data_app.smo_ogrn);
    ui->line_ogrn_smo_text->setText(data_app.smo_name);
    ui->combo_pol_v->setCurrentIndex(ui->combo_pol_v->findData(3));
    ui->combo_pol_v->setEnabled(false);

    ui->line_pol_ser->setText("");
    ui->line_pol_num->setText("");
    ui->ch_pol_sernum->setChecked(true);
    ui->ch_pol_sernum->setEnabled(true);

    ui->ch_vs_num->setChecked(false);
    ui->ch_vs_num->setEnabled(false);

    ui->line_enp->setText("");  //("ещё не решил как получить...");
    ui->ch_pol_enp->setChecked(true);
    ui->ch_pol_enp->setEnabled(true);

    QDate date_start = data_vizit.date;
    //QDate date_start = QDate::currentDate();
    // если стоит флаг - дата старта = дате старта ВС
    if (data_app.pol_start_date_by_vs==1) {
        date_start = date_vs;
    }
    //QDate date_end = myAddDays(date_start, 30);

    ui->date_pol_begin->setDate(date_start);
    ui->ch_pol_datbegin->setChecked(true);
    ui->ch_pol_datbegin->setEnabled(true);

    ui->date_pol_end->setDate(QDate(1900,1,1));
    data_polis.date_end = QDate(1900,1,1);
    ui->ch_pol_datend->setChecked(false);
    ui->ch_pol_datend->setEnabled(true);

    ui->date_pol_stop->setDate(QDate(1900,1,1));
    ui->ch_pol_datstop->setChecked(false);
    ui->ch_pol_datstop->setEnabled(true);
    ui->date_pol_stop->setVisible(true);
    on_ch_pol_datstop_stateChanged(0);
    ui->combo_pol_in_erp->setCurrentIndex(1);

    //ui->date_order_date->setDate(date_start);

    //ui->ch_pol_erp->setChecked(false);
    //ui->ch_pol_erp->setEnabled(true);

    ui->lab_point_name->setText(data_app.point_name);
    ui->lab_operator_name->setText(data_app.operator_fio);
    ui->lab_edit_date->setText(" - не сохранён - ");

    // подготовим свойства полиса для нового полиса ОМС единого образца
    data_polis.id = -1;
    data_polis.id_person = data_pers.id;
    data_polis.id_old_polis = -1;
    data_polis.pol_type = "П";
    data_polis.pol_v = 3;
    data_polis.f_polis = 1;




    // получим данные последней времянки
    QString sql_vs = "";
    sql_vs += "select vs.id, vs.date_begin, vs.r_smo, vs.r_polis "
              " from persons e "
              "      join polises vs on ( e.id=vs.id_person "
              "                       and vs.date_begin<=CURRENT_DATE "
              "                       and ( vs.date_stop is NULL "
              "                          or vs.date_stop>CURRENT_DATE ) ) "
              " where vs.id_person=" + QString::number(data_pers.id) + " "
              "   and vs.pol_v=2 "
              " order by vs.id desc ; ";

    QSqlQuery *query_vs = new QSqlQuery(db);
    bool res_vs = mySQL.exec(this, sql_vs, QString("Получим данные последней времянки"), *query_vs, true, db, data_app);
    if (res_vs && query_vs->next()) {
        data_polis.id_old_polis = query_vs->value(0).toInt();

        // дата начала страхования ЕНП = дате выдачи ВС
        data_polis.date_begin = query_vs->value(1).toDate();
        ui->ch_pol_datbegin->setEnabled(true);
        ui->ch_pol_datbegin->setChecked(true);
        ui->date_pol_begin->setEnabled(false);
        ui->date_pol_begin->setDate(data_polis.date_begin);

        data_polis.r_smo   = data_vizit.r_smo   = query_vs->value(2).toInt();
        data_polis.r_polis = data_vizit.r_polis = query_vs->value(3).toInt();
    }
    delete query_vs;




    // зададим дату окончания и изъятия по умолчанию
    data_polis.has_date_end = false;
    data_polis.date_end = QDate(1900,1,1);
    ui->ch_pol_datend->setChecked(false);
    ui->ch_pol_datend->setVisible(true);
    ui->date_pol_end->setVisible(false);
    ui->date_pol_end->setEnabled(false);
    ui->date_pol_end->setDate(QDate(1900,1,1));

    data_polis.has_date_stop = false;
    data_polis.date_stop = QDate(1900,1,1);
    ui->ch_pol_datstop->setChecked(false);
    ui->ch_pol_datstop->setVisible(true);
    ui->date_pol_stop->setVisible(false);
    ui->date_pol_stop->setEnabled(false);
    //ch_pol_datstop->setDate(QDate(1900,1,1));

    // для лиц с ограниченным действием УДЛ зададим date_end и date_stop
    if (data_pers.udl_date_exp_f) {
        data_polis.has_date_end = true;
        data_polis.has_date_stop = false;
        data_polis.date_end = data_pers.udl_date_exp;
        ui->ch_pol_datend->setChecked(true);
        ui->ch_pol_datend->setVisible(true);
        ui->date_pol_end->setVisible(true);
        ui->date_pol_end->setEnabled(false);
        ui->date_pol_end->setDate(data_pers.udl_date_exp);

        data_polis.has_date_stop = true;
        data_polis.date_stop = data_pers.udl_date_exp;
        ui->ch_pol_datstop->setChecked(true);
        ui->ch_pol_datstop->setVisible(true);
        ui->date_pol_stop->setVisible(true);
        ui->date_pol_stop->setEnabled(false);
        ui->date_pol_stop->setDate(data_pers.udl_date_exp);
    }
    // для лиц с ограниченным действием ДРП зададим date_end и date_stop
    if ( data_pers.drp_date_exp_f &&
         (!data_pers.udl_date_exp_f || data_pers.udl_date_exp>data_pers.drp_date_exp) ) {
        data_polis.has_date_end = true;
        data_polis.date_end = data_pers.drp_date_exp;
        ui->ch_pol_datend->setChecked(true);
        ui->ch_pol_datend->setVisible(true);
        ui->date_pol_end->setVisible(true);
        ui->date_pol_end->setEnabled(false);
        ui->date_pol_end->setDate(data_pers.drp_date_exp);

        data_polis.has_date_stop = true;
        data_polis.date_stop = data_pers.drp_date_exp;
        ui->ch_pol_datstop->setChecked(true);
        ui->ch_pol_datstop->setVisible(true);
        ui->date_pol_stop->setVisible(true);
        ui->date_pol_stop->setEnabled(false);
        ui->date_pol_stop->setDate(data_pers.drp_date_exp);
    }
    /*// если известна дата начала страхования - зададим её в качестве date_begin
    if ( data_pers.has_date_insure_begin &&
         data_pers._date_insure_begin>QDate(1920,1,1) ) {
        data_polis.has_date_begin = true;
        data_polis.date_begin = data_pers._date_insure_begin;
        ui->ch_pol_datbegin->setChecked(true);
        ui->ch_pol_datbegin->setVisible(true);
        ui->date_pol_begin->setVisible(true);
        ui->date_pol_begin->setEnabled(false);
        ui->date_pol_begin->setDate(data_pers._date_insure_begin);
    }*/

    // подготовим данные события
    data_event.id_polis = data_polis.id;
    data_event.event_code = "П060";
    data_event.event_dt = QDateTime(ui->date_vizit_pan->date(), QTime::currentTime());
    data_event.event_time = QTime::currentTime();
    data_event.comment = "Выдача на руки полиса единого образца после ВС";

    // подготовим панель свойств к редактированию данных полиса единого образца
    ui->lab_print_vs->setEnabled(false);
    ui->bn_print_vs->setEnabled(false);
    ui->bn_print_vs_2->setEnabled(false);
    ui->lab_print_ch_smo->setEnabled(true);
    ui->bn_print_ch_smo->setEnabled(true);
    ui->lab_prind_db_pol->setEnabled(false);
    ui->bn_print_db_pol->setEnabled(false);
    ui->lab_print_notice->setEnabled(true);
    ui->bn_print_notice_pol->setEnabled(true);

    test_for_udl_exp();
    test_for_pers_categ();

    ui->line_pol_ser->setFocus();
    b_need_save (true);
}

// Изменение сведений о ЗЛ, требующих замены полиса ОМС (изменение или исправление ФИО или места рождения) (без смены СМО)
void polices_wnd::on_bn_p061_clicked() {
    // причина подачи заявления 0
    {
        data_vizit.r_smo = 0;
        ui->combo_vizit_pan_reason_smo->setCurrentIndex(0);
    }
    //заявления на дублкат больше 1
    if (data_vizit.r_polis==0 || data_vizit.r_polis>2) {
        data_vizit.r_polis = 1;
        ui->combo_vizit_pan_reason_polis->setCurrentIndex(1);
    }
    // форма полиса >=1
    if (data_vizit.f_polis==0) {
        data_vizit.f_polis = 1;
        ui->combo_vizit_pan_form_polis->setCurrentIndex(1);
    }


    // проверка на дату смерти
    if (!test_for_dead()) {
        data_event.is_ok = false;
        return;
    }


    // проверка действующих полисов
    QString sql = "";
    sql += "select p.pol_v, f8.text as pol_v_text, p.pol_type, p.pol_ser, p.pol_num, p.vs_num, p.date_begin, p.date_end, p.date_stop, p.id, p.in_erp, "
           "       coalesce('('||bv_st.code||') '||bv_st.text, '('||bp_st.code||') '||bp_st.text, ' - ??? - ') as status_text "
           " from persons e "
           "     join polises p on( "
           "         (e.id=p.id_person) and "
           "         (p.date_begin<=CURRENT_DATE) and "
           "         (p.date_stop is NULL or p.date_stop>CURRENT_DATE) ) "
           "     left join spr_f008 f8 on(f8.code=p.pol_v) "
           "     left join blanks_pol bp on(bp.id_polis=p.id) "
           "     left join spr_blank_status bp_st on(bp_st.code=bp.status) "
           "     left join blanks_vs bv on(bv.id_polis=p.id) "
           "     left join spr_blank_status bv_st on(bv_st.code=bv.status) "
           " where e.id=" + QString::number(data_pers.id) + " "
           "   and ((bp.id is not NULL and bp.status>0) or (bv.vs_num is not NULL and bv.status>0)) ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString("Проверка полисов застрахованного"), *query, true, db, data_app);
    if (!res) {
        QString s_out = "При проверке старых полисов застрахованного произошла неожиданная ошибка.\n\nОперация страхования отменена.";
        QMessageBox::information(this, "П061 - Неожиданная ошибка!", s_out);
        data_event.is_ok = false;
        delete query;
        return;
    } else if (query->size()>1) {
        QString s_out = "Данное событие неприемлемо, поскольку у застрахованного более одного действующего полиса:\n";
        while (query->next()) {
            s_out += "\n(" + QString::number(query->value(0).toInt()) + ") " + query->value(1).toString() +
                    "\nтип/серия/номер: " + (query->value(2).isNull() ? "- null -" : query->value(2).toString()) + ",  " +
                                            (query->value(3).isNull() ? "- null -" : query->value(3).toString()) + " № " +
                                            (query->value(4).isNull() ? "- null -" : query->value(4).toString()) + ",  ВС " +
                                            (query->value(5).isNull() ? "- null -" : query->value(5).toString()) +
                    "\nоткрыт: " + (query->value(6).isNull() ? "- null -" : query->value(6).toDate().toString("dd.MM.yyyy")) +
                    (query->value(7).isNull() ? ",  нет срока действия " : ( query->value(7).toDate()>QDate::currentDate() ? ",  срок действия: " : ",  срок действия истёк: ") + query->value(7).toDate().toString("dd.MM.yyyy")) +
                    ",  дата изъятия: " + (query->value(8).isNull() ? "- null -" : query->value(8).toDate().toString("dd.MM.yyyy")) + "\n"
                    "статус - " + query->value(11).toString();
        }
        QMessageBox::information(this, "П061 - Более одного действующего полиса!", s_out);
        data_event.is_ok = false;
        delete query;
        return;
    }
    // проверка типа полиса
    query->next();
    if (query->value(0)==2) {
        QString s_out = "Данное событие неприемлемо, поскольку действующий полис \nзастрахованного - это ВС, а ВС по такому событию заменить нельзя.\n";
        while (query->next()) {
            s_out += "\n(" + QString::number(query->value(0).toInt()) + ") " + query->value(1).toString() +
                     "\nтип/серия/номер: " + (query->value(2).isNull() ? "- null -" : query->value(2).toString()) + ",  " +
                                             (query->value(3).isNull() ? "- null -" : query->value(3).toString()) + " № " +
                                             (query->value(4).isNull() ? "- null -" : query->value(4).toString()) + ",  ВС " +
                                             (query->value(5).isNull() ? "- null -" : query->value(5).toString()) +
                     "\nоткрыт: " + (query->value(6).isNull() ? "- null -" : query->value(6).toDate().toString("dd.MM.yyyy")) +
                     (query->value(7).isNull() ? ",  нет срока действия " : ( query->value(7).toDate()>QDate::currentDate() ? ",  срок действия: " : ",  срок действия истёк: ") + query->value(7).toDate().toString("dd.MM.yyyy")) +
                     ",  дата изъятия: " + (query->value(8).isNull() ? "- null -" : query->value(8).toDate().toString("dd.MM.yyyy")) + "\n";
        }
        QMessageBox::information(this, "П061 - ВС так закрыть нельзя!", s_out);
        data_event.is_ok = false;
        delete query;
        return;
    }
    int in_erp = query->value(10).toInt();

    // закроем полис
    int id_polis = query->value(9).toInt();
    QString sql_close;
    sql_close =  "update polises p ";
    sql_close += "   set date_stop='" + QDate::currentDate().toString("yyyy-MM-dd") + "', ";
    sql_close += "       _id_last_point=" + QString::number(data_app.id_point) + ", ";
    sql_close += "       _id_last_operator=" + QString::number(data_app.id_operator) + " ";
    sql_close += " where p.id=" + QString::number(id_polis) + " ; ";

    QSqlQuery *query_close = new QSqlQuery(db);
    bool res_close = mySQL.exec(this, sql, QString("Закрытие старого полиса (не ВС)"), *query_close, true, db, data_app);
    if (!res_close) {
        // не удалось закрыть полис
        QMessageBox::information(this, "П061 - Отмена операции", "При попытке закрытия старого полиса произошла ошибка.");
        data_event.is_ok = false;
        delete query_close;
        return;
    }
    delete query_close;
    delete query;

    // откроем панель свойств
    on_show_pan_vizit_clicked(false);
    on_show_pan_polises_clicked(true);
    on_show_pan_polis_clicked(true);
    ui->pan_vizit->setEnabled(false);
    ui->pan_polises->setEnabled(false);

    ui->split_main->setStretchFactor(0,2);
    ui->split_main->setStretchFactor(1,5);
    ui->split_main->setStretchFactor(2,3);

    refresh_terr_ocato();
    refresh_pol_ves();

    ui->lab_event_code->setText("П061");
    data_event.event_code = "П061";

    ui->line_terr_ocato->setText(data_app.ocato);
    ui->combo_terr_ocato_text->setCurrentIndex(ui->combo_terr_ocato_text->findData(data_app.ocato.toInt()));
    ui->line_ogrn_smo->setText(data_app.smo_ogrn);
    ui->line_ogrn_smo_text->setText(data_app.smo_name);
    ui->combo_pol_v->setCurrentIndex(ui->combo_pol_v->findData(2));
    ui->combo_pol_v->setEnabled(false);

    ui->line_pol_ser->setText("");
    ui->line_pol_num->setText("");
    ui->ch_pol_sernum->setChecked(false);
    ui->ch_pol_sernum->setEnabled(false);

    ui->ch_vs_num->setChecked(true);
    ui->ch_vs_num->setEnabled(true);

    ui->line_enp->setText("");
    ui->ch_pol_enp->setChecked(false);
    ui->ch_pol_enp->setEnabled(false);

    QDate date_start = data_vizit.date;
    //QDate date_start = QDate::currentDate();
    QDate date_end = myAddDays(date_start, 30);
    data_polis.date_end = date_end;

    ui->date_pol_begin->setDate(date_start);
    ui->ch_pol_datbegin->setChecked(true);
    ui->ch_pol_datbegin->setEnabled(true);

    ui->date_pol_end->setDate(date_end);
    data_polis.date_end = date_end;
    ui->ch_pol_datend->setChecked(true);
    ui->ch_pol_datend->setEnabled(true);

    ui->date_pol_stop->setDate(QDate(1900,1,1));
    ui->ch_pol_datstop->setChecked(false);
    ui->ch_pol_datstop->setEnabled(true);
    ui->date_pol_stop->setVisible(true);
    on_ch_pol_datstop_stateChanged(0);
    ui->combo_pol_in_erp->setCurrentIndex(in_erp);

    //ui->date_order_date->setDate(date_start);

    //ui->ch_pol_erp->setChecked(false);
    //ui->ch_pol_erp->setEnabled(true);

    ui->lab_point_name->setText(data_app.point_name);
    ui->lab_operator_name->setText(data_app.operator_fio);
    ui->lab_edit_date->setText(" - не сохранён - ");

    // подготовим свойства полиса для новой времянки нового застрахованного
    data_polis.id = -1;
    data_polis.id_person = data_pers.id;
    data_polis.id_old_polis = -1;
    data_polis.pol_type = "В";
    data_polis.pol_v = 2;
    data_event.id_polis = data_polis.id;
    data_event.event_code = "П061";
    data_event.event_dt = QDateTime(ui->date_vizit_pan->date(), QTime::currentTime());
    data_event.event_time = QTime::currentTime();
    data_event.comment = "Изменение сведений о ЗЛ, требующих замены полиса ОМС (изменение или исправление ФИО или места рождения) (без смены СМО) ...";

    // подготовим панель свойств к редактированию данных ВС
    ui->lab_print_vs->setEnabled(true);
    ui->bn_print_vs->setEnabled(true);
    ui->bn_print_vs_2->setEnabled(true);
    ui->lab_print_ch_smo->setEnabled(false);
    ui->bn_print_ch_smo->setEnabled(false);
    ui->lab_prind_db_pol->setEnabled(true);
    ui->bn_print_db_pol->setEnabled(true);
    ui->lab_print_notice->setEnabled(true);
    ui->bn_print_notice_pol->setEnabled(true);

    test_for_udl_exp();
    test_for_pers_categ();

    ui->line_pol_ser->setFocus();
    b_need_save (true);
}

// ----------------------------------
// Выдача дубликата полиса
// ----------------------------------
void polices_wnd::on_bn_p062_clicked() {
    // причина подачи заявления 0
    {
        data_vizit.r_smo = 0;
        ui->combo_vizit_pan_reason_smo->setCurrentIndex(0);
    }
    //заявления на дублкат >= 0
    if (data_vizit.r_polis==0 || data_vizit.r_polis==1 || data_vizit.r_polis==5) {
        data_vizit.r_polis = 2;
        ui->combo_vizit_pan_reason_polis->setCurrentIndex(2);
    }
    // форма полиса >=1
    if (data_vizit.f_polis==0) {
        data_vizit.f_polis = 1;
        ui->combo_vizit_pan_form_polis->setCurrentIndex(1);
    }


    // проверка на дату смерти
    if (!test_for_dead()) {
        data_event.is_ok = false;
        return;
    }


    // проверка действующих полисов
    QString sql = "";
    sql += "select p.pol_v, f8.text as pol_v_text, p.pol_type, p.pol_ser, p.pol_num, p.vs_num, p.date_begin, p.date_end, p.date_stop, p.id, p.in_erp, "
           "       coalesce('('||bv_st.code||') '||bv_st.text, '('||bp_st.code||') '||bp_st.text, ' - ??? - ') as status_text "
           " from persons e "
           "     join polises p on( "
           "         (e.id=p.id_person) and "
           "         (p.date_begin<=CURRENT_DATE) and "
           "         (p.date_stop is NULL or p.date_stop>CURRENT_DATE) ) "
           "     left join spr_f008 f8 on(f8.code=p.pol_v) "
           "     left join blanks_pol bp on(bp.id_polis=p.id) "
           "     left join spr_blank_status bp_st on(bp_st.code=bp.status) "
           "     left join blanks_vs bv on(bv.id_polis=p.id) "
           "     left join spr_blank_status bv_st on(bv_st.code=bv.status) "
           " where e.id=" + QString::number(data_pers.id) + " "
           "   and ((bp.id is not NULL and bp.status>0) or (bv.vs_num is not NULL and bv.status>0)) ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString("Проверка полисов застрахованного"), *query, true, db, data_app);
    if (!res) {
        QString s_out = "При проверке старых полисов застрахованного произошла неожиданная ошибка.\n\nОперация страхования отменена.";
        QMessageBox::information(this, "П062 - Неожиданная ошибка!", s_out);
        data_event.is_ok = false;
        delete query;
        return;
    } else if (query->size()>1) {
        QString s_out = "Данное событие неприемлемо, поскольку у застрахованного более одного действующего полиса:\n";
        while (query->next()) {
            s_out += "\n(" + QString::number(query->value(0).toInt()) + ") " + query->value(1).toString() +
                    "\nтип/серия/номер: " + (query->value(2).isNull() ? "- null -" : query->value(2).toString()) + ",  " +
                                            (query->value(3).isNull() ? "- null -" : query->value(3).toString()) + " № " +
                                            (query->value(4).isNull() ? "- null -" : query->value(4).toString()) + ",  ВС " +
                                            (query->value(5).isNull() ? "- null -" : query->value(5).toString()) +
                    "\nоткрыт: " + (query->value(6).isNull() ? "- null -" : query->value(6).toDate().toString("dd.MM.yyyy")) +
                    (query->value(7).isNull() ? ",  нет срока действия " : ( query->value(7).toDate()>QDate::currentDate() ? ",  срок действия: " : ",  срок действия истёк: ") + query->value(7).toDate().toString("dd.MM.yyyy")) +
                    ",  дата изъятия: " + (query->value(8).isNull() ? "- null -" : query->value(8).toDate().toString("dd.MM.yyyy")) + "\n"
                    "статус - " + query->value(11).toString();
        }
        QMessageBox::information(this, "П062 - Более одного действующего полиса!", s_out);
        data_event.is_ok = false;
        delete query;
        return;
    }
    // проверка типа полиса
    query->next();
    if (query->value(0)==2) {
        QString s_out = "Данное событие неприемлемо, поскольку действующий полис \nзастрахованного - это ВС, а ВС по такому событию заменить нельзя.\n";
        while (query->next()) {
            s_out += "\n(" + QString::number(query->value(0).toInt()) + ") " + query->value(1).toString() +
                     "\nтип/серия/номер: " + (query->value(2).isNull() ? "- null -" : query->value(2).toString()) + ",  " +
                                             (query->value(3).isNull() ? "- null -" : query->value(3).toString()) + " № " +
                                             (query->value(4).isNull() ? "- null -" : query->value(4).toString()) + ",  ВС " +
                                             (query->value(5).isNull() ? "- null -" : query->value(5).toString()) +
                     "\nоткрыт: " + (query->value(6).isNull() ? "- null -" : query->value(6).toDate().toString("dd.MM.yyyy")) +
                     (query->value(7).isNull() ? ",  нет срока действия " : ( query->value(7).toDate()>QDate::currentDate() ? ",  срок действия: " : ",  срок действия истёк: ") + query->value(7).toDate().toString("dd.MM.yyyy")) +
                     ",  дата изъятия: " + (query->value(8).isNull() ? "- null -" : query->value(8).toDate().toString("dd.MM.yyyy")) + "\n";
        }
        QMessageBox::information(this, "П062 - ВС так закрыть нельзя!", s_out);
        data_event.is_ok = false;
        delete query;
        return;
    }
    int in_erp = query->value(10).toInt();

    // закроем полис
    int id_polis = query->value(9).toInt();
    QString sql_close;
    sql_close =  "update polises p ";
    sql_close += "   set date_stop='" + QDate::currentDate().toString("yyyy-MM-dd") + "', ";
    sql_close += "       _id_last_point=" + QString::number(data_app.id_point) + ", ";
    sql_close += "       _id_last_operator=" + QString::number(data_app.id_operator) + " ";
    sql_close += " where p.id=" + QString::number(id_polis) + " ; ";

    QSqlQuery *query_close = new QSqlQuery(db);
    bool res_close = mySQL.exec(this, sql, QString("Закрытие старого полиса (не ВС)"), *query_close, true, db, data_app);
    if (!res_close) {
        // не удалось закрыть полис
        QMessageBox::information(this, "П062 - Отмена операции", "При попытке закрытия старого полиса произошла ошибка.");
        data_event.is_ok = false;
        delete query_close;
        return;
    }
    delete query_close;
    delete query;

    // откроем панель свойств
    on_show_pan_vizit_clicked(false);
    on_show_pan_polises_clicked(true);
    on_show_pan_polis_clicked(true);
    ui->pan_vizit->setEnabled(false);
    ui->pan_polises->setEnabled(false);

    ui->split_main->setStretchFactor(0,2);
    ui->split_main->setStretchFactor(1,5);
    ui->split_main->setStretchFactor(2,3);

    refresh_terr_ocato();
    refresh_pol_ves();

    ui->lab_event_code->setText("П062");
    data_event.event_code = "П062";

    ui->line_terr_ocato->setText(data_app.ocato);
    ui->combo_terr_ocato_text->setCurrentIndex(ui->combo_terr_ocato_text->findData(data_app.ocato.toInt()));
    ui->line_ogrn_smo->setText(data_app.smo_ogrn);
    ui->line_ogrn_smo_text->setText(data_app.smo_name);
    ui->combo_pol_v->setCurrentIndex(ui->combo_pol_v->findData(2));
    ui->combo_pol_v->setEnabled(false);

    ui->line_pol_ser->setText("");
    ui->line_pol_num->setText("");
    ui->ch_pol_sernum->setChecked(false);
    ui->ch_pol_sernum->setEnabled(false);

    ui->ch_vs_num->setChecked(true);
    ui->ch_vs_num->setEnabled(true);

    ui->line_enp->setText("");
    ui->ch_pol_enp->setChecked(false);
    ui->ch_pol_enp->setEnabled(false);

    QDate date_start = data_vizit.date;
    //QDate date_start = QDate::currentDate();
    QDate date_end = myAddDays(date_start, 30);
    data_polis.date_end = date_end;

    ui->date_pol_begin->setDate(date_start);
    ui->ch_pol_datbegin->setChecked(true);
    ui->ch_pol_datbegin->setEnabled(true);

    ui->date_pol_end->setDate(date_end);
    data_polis.date_end = date_end;
    ui->ch_pol_datend->setChecked(true);
    ui->ch_pol_datend->setEnabled(true);

    ui->date_pol_stop->setDate(QDate(1900,1,1));
    ui->ch_pol_datstop->setChecked(false);
    ui->ch_pol_datstop->setEnabled(true);
    ui->date_pol_stop->setVisible(true);
    on_ch_pol_datstop_stateChanged(0);
    ui->combo_pol_in_erp->setCurrentIndex(in_erp);

    //ui->date_order_date->setDate(date_start);

    //ui->ch_pol_erp->setChecked(false);
    //ui->ch_pol_erp->setEnabled(true);

    ui->lab_point_name->setText(data_app.point_name);
    ui->lab_operator_name->setText(data_app.operator_fio);
    ui->lab_edit_date->setText(" - не сохранён - ");

    // подготовим свойства полиса для новой времянки нового застрахованного
    data_polis.id = -1;
    data_polis.id_person = data_pers.id;
    data_polis.id_old_polis = -1;
    data_polis.pol_type = "В";
    data_polis.pol_v = 2;
    data_event.id_polis = data_polis.id;
    data_event.event_code = "П062";
    data_event.event_dt = QDateTime(ui->date_vizit_pan->date(), QTime::currentTime());
    data_event.event_time = QTime::currentTime();
    data_event.comment = "Выдача дубликата полиса ...";

    // подготовим панель свойств к редактированию данных ВС
    ui->lab_print_vs->setEnabled(true);
    ui->bn_print_vs->setEnabled(true);
    ui->bn_print_vs_2->setEnabled(true);
    ui->lab_print_ch_smo->setEnabled(false);
    ui->bn_print_ch_smo->setEnabled(false);
    ui->lab_prind_db_pol->setEnabled(true);
    ui->bn_print_db_pol->setEnabled(true);
    ui->lab_print_notice->setEnabled(true);
    ui->bn_print_notice_pol->setEnabled(true);

    test_for_udl_exp();
    test_for_pers_categ();

    ui->line_pol_ser->setFocus();
    b_need_save (true);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------
// Выдача полиса по причине окончания действия ранее выданного полиса (предоставление документов, продляющих срок действия права на ОМС для иностранных граждан)
// -------------------------------------------------------------------------------------------------------------------------------------------------------------
void polices_wnd::on_bn_p063_clicked() {
    // причина подачи заявления 0
    {
        data_vizit.r_smo = 0;
        ui->combo_vizit_pan_reason_smo->setCurrentIndex(0);
    }
    //заявления на дубликат = 5
    if (data_vizit.r_polis<5) {
        data_vizit.r_polis = 5;
        ui->combo_vizit_pan_reason_polis->setCurrentIndex(5);
    }
    // форма полиса >=1
    if (data_vizit.f_polis==0) {
        data_vizit.f_polis = 1;
        ui->combo_vizit_pan_form_polis->setCurrentIndex(1);
    }


    // проверка на дату смерти
    if (!test_for_dead()) {
        data_event.is_ok = false;
        return;
    }


    // проверка действующих полисов
    QString sql = "";
    sql += "select p.pol_v, f8.text as pol_v_text, p.pol_type, p.pol_ser, p.pol_num, p.vs_num, p.date_begin, p.date_end, p.date_stop, p.id, p.in_erp, "
           "       coalesce('('||bv_st.code||') '||bv_st.text, '('||bp_st.code||') '||bp_st.text, ' - ??? - ') as status_text "
           " from persons e "
           "     join polises p on( "
           "         (e.id=p.id_person) and "
           "         (p.date_begin<=CURRENT_DATE) and "
           "         (p.date_stop is NULL or p.date_stop>CURRENT_DATE) ) "
           "     left join spr_f008 f8 on(f8.code=p.pol_v) "
           "     left join blanks_pol bp on(bp.id_polis=p.id) "
           "     left join spr_blank_status bp_st on(bp_st.code=bp.status) "
           "     left join blanks_vs bv on(bv.id_polis=p.id) "
           "     left join spr_blank_status bv_st on(bv_st.code=bv.status) "
           " where e.id=" + QString::number(data_pers.id) + " "
           "   and ((bp.id is not NULL and bp.status>0) or (bv.vs_num is not NULL and bv.status>0)) ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString("Проверка полисов застрахованного"), *query, true, db, data_app);
    if (!res) {
        QString s_out = "При проверке старых полисов застрахованного произошла неожиданная ошибка.\n\nОперация страхования отменена.";
        QMessageBox::information(this, "П063 - Неожиданная ошибка!", s_out);
        data_event.is_ok = false;
        delete query;
        return;
    } else if (query->size()>1) {
        QString s_out = "Данное событие неприемлемо, поскольку у застрахованного более одного действующего полиса:\n";
        while (query->next()) {
            s_out += "\n(" + QString::number(query->value(0).toInt()) + ") " + query->value(1).toString() +
                    "\nтип/серия/номер: " + (query->value(2).isNull() ? "- null -" : query->value(2).toString()) + ",  " +
                                            (query->value(3).isNull() ? "- null -" : query->value(3).toString()) + " № " +
                                            (query->value(4).isNull() ? "- null -" : query->value(4).toString()) + ",  ВС " +
                                            (query->value(5).isNull() ? "- null -" : query->value(5).toString()) +
                    "\nоткрыт: " + (query->value(6).isNull() ? "- null -" : query->value(6).toDate().toString("dd.MM.yyyy")) +
                    (query->value(7).isNull() ? ",  нет срока действия " : ( query->value(7).toDate()>QDate::currentDate() ? ",  срок действия: " : ",  срок действия истёк: ") + query->value(7).toDate().toString("dd.MM.yyyy")) +
                    ",  дата изъятия: " + (query->value(8).isNull() ? "- null -" : query->value(8).toDate().toString("dd.MM.yyyy")) + "\n"
                    "статус - " + query->value(11).toString();
        }
        QMessageBox::information(this, "П063 - Более одного действующего полиса!", s_out);
        data_event.is_ok = false;
        delete query;
        return;
    }
    // проверка типа полиса
    query->next();
    if (query->value(0)==2) {
        QString s_out = "Данное событие неприемлемо, поскольку действующий полис \nзастрахованного - это ВС, а ВС по такому событию заменить нельзя.\n";
        while (query->next()) {
            s_out += "\n(" + QString::number(query->value(0).toInt()) + ") " + query->value(1).toString() +
                     "\nтип/серия/номер: " + (query->value(2).isNull() ? "- null -" : query->value(2).toString()) + ",  " +
                                             (query->value(3).isNull() ? "- null -" : query->value(3).toString()) + " № " +
                                             (query->value(4).isNull() ? "- null -" : query->value(4).toString()) + ",  ВС " +
                                             (query->value(5).isNull() ? "- null -" : query->value(5).toString()) +
                     "\nоткрыт: " + (query->value(6).isNull() ? "- null -" : query->value(6).toDate().toString("dd.MM.yyyy")) +
                     (query->value(7).isNull() ? ",  нет срока действия " : ( query->value(7).toDate()>QDate::currentDate() ? ",  срок действия: " : ",  срок действия истёк: ") + query->value(7).toDate().toString("dd.MM.yyyy")) +
                     ",  дата изъятия: " + (query->value(8).isNull() ? "- null -" : query->value(8).toDate().toString("dd.MM.yyyy")) + "\n";
        }
        QMessageBox::information(this, "П063 - ВС так закрыть нельзя!", s_out);
        data_event.is_ok = false;
        delete query;
        return;
    }
    int in_erp = query->value(10).toInt();

    // закроем полис
    int id_polis = query->value(9).toInt();
    QString sql_close;
    sql_close =  "update polises p ";
    sql_close += "   set date_stop='" + QDate::currentDate().toString("yyyy-MM-dd") + "', ";
    sql_close += "       _id_last_point=" + QString::number(data_app.id_point) + ", ";
    sql_close += "       _id_last_operator=" + QString::number(data_app.id_operator) + " ";
    sql_close += " where p.id=" + QString::number(id_polis) + " ; ";

    QSqlQuery *query_close = new QSqlQuery(db);
    bool res_close = mySQL.exec(this, sql, QString("Закрытие старого полиса (не ВС)"), *query_close, true, db, data_app);
    if (!res_close) {
        // не удалось закрыть полис
        QMessageBox::information(this, "П063 - Отмена операции", "При попытке закрытия старого полиса произошла ошибка.");
        data_event.is_ok = false;
        delete query_close;
        return;
    }
    delete query_close;
    delete query;

    // откроем панель свойств
    on_show_pan_vizit_clicked(false);
    on_show_pan_polises_clicked(true);
    on_show_pan_polis_clicked(true);
    ui->pan_vizit->setEnabled(false);
    ui->pan_polises->setEnabled(false);

    ui->split_main->setStretchFactor(0,2);
    ui->split_main->setStretchFactor(1,5);
    ui->split_main->setStretchFactor(2,3);

    refresh_terr_ocato();
    refresh_pol_ves();

    ui->lab_event_code->setText("П063");
    data_event.event_code = "П063";

    ui->line_terr_ocato->setText(data_app.ocato);
    ui->combo_terr_ocato_text->setCurrentIndex(ui->combo_terr_ocato_text->findData(data_app.ocato.toInt()));
    ui->line_ogrn_smo->setText(data_app.smo_ogrn);
    ui->line_ogrn_smo_text->setText(data_app.smo_name);
    ui->combo_pol_v->setCurrentIndex(ui->combo_pol_v->findData(2));
    ui->combo_pol_v->setEnabled(false);

    ui->line_pol_ser->setText("");
    ui->line_pol_num->setText("");
    ui->ch_pol_sernum->setChecked(false);
    ui->ch_pol_sernum->setEnabled(false);

    ui->ch_vs_num->setChecked(true);
    ui->ch_vs_num->setEnabled(true);

    ui->line_enp->setText("");
    ui->ch_pol_enp->setChecked(false);
    ui->ch_pol_enp->setEnabled(false);

    QDate date_start = data_vizit.date;
    //QDate date_start = QDate::currentDate();
    QDate date_end = myAddDays(date_start, 30);
    data_polis.date_end = date_end;

    ui->date_pol_begin->setDate(date_start);
    ui->ch_pol_datbegin->setChecked(true);
    ui->ch_pol_datbegin->setEnabled(true);
    data_polis.has_date_begin = true;

    ui->date_pol_end->setDate(date_end);
    data_polis.date_end = date_end;
    ui->ch_pol_datend->setChecked(true);
    ui->ch_pol_datend->setEnabled(true);
    data_polis.has_date_end = true;

    ui->date_pol_stop->setDate(QDate(1900,1,1));
    ui->ch_pol_datstop->setChecked(false);
    ui->ch_pol_datstop->setEnabled(true);
    ui->date_pol_stop->setVisible(true);
    data_polis.has_date_stop = false;

    on_ch_pol_datstop_stateChanged(0);
    ui->combo_pol_in_erp->setCurrentIndex(in_erp);

    //ui->date_order_date->setDate(date_start);

    //ui->ch_pol_erp->setChecked(false);
    //ui->ch_pol_erp->setEnabled(true);

    ui->lab_point_name->setText(data_app.point_name);
    ui->lab_operator_name->setText(data_app.operator_fio);
    ui->lab_edit_date->setText(" - не сохранён - ");

    // подготовим свойства полиса для новой времянки нового застрахованного
    data_polis.id = -1;
    data_polis.id_person = data_pers.id;
    data_polis.id_old_polis = -1;
    data_polis.pol_type = "В";
    data_polis.pol_v = 2;
    data_event.id_polis = data_polis.id;
    data_event.event_code = "П063";
    data_event.event_dt = QDateTime(ui->date_vizit_pan->date(), QTime::currentTime());
    data_event.event_time = QTime::currentTime();
    data_event.comment = "Выдача полиса по причине окончания действия ранее выданного полиса (предоставление документов, продляющих срок действия права на ОМС для иностранных граждан) ...";

    // подготовим панель свойств к редактированию данных ВС
    ui->lab_print_vs->setEnabled(true);
    ui->bn_print_vs->setEnabled(true);
    ui->bn_print_vs_2->setEnabled(true);
    ui->lab_print_ch_smo->setEnabled(false);
    ui->bn_print_ch_smo->setEnabled(false);
    ui->lab_prind_db_pol->setEnabled(true);
    ui->bn_print_db_pol->setEnabled(true);
    ui->lab_print_notice->setEnabled(true);
    ui->bn_print_notice_pol->setEnabled(true);

    test_for_udl_exp();
    test_for_pers_categ();

    ui->line_pol_ser->setFocus();
    b_need_save (true);
}

void polices_wnd::on_tab_polises_doubleClicked(const QModelIndex &index) {
    on_bn_polises_next_clicked();
}

void polices_wnd::on_bn_print_ch_smo_clicked() {
    // проверим ЕНП
    if ( data_event.event_code=="П031" ||
         data_event.event_code=="П032" ||
         data_event.event_code=="П033" )
    {
        if ( ui->line_enp->text().trimmed().isEmpty() ) {
            QMessageBox::information(this,"Генерация заявления отменена","Для данного события необходимо ввести ЕНП длиной 16 цифр.\n\nГенерация заявления отменена.");
            return;
        }
        if ( ( !ui->line_enp->text().trimmed().isEmpty() ) &&
             ( ui->line_enp->text().trimmed().length()!=16 ) ) {
            QMessageBox::information(this,"Генерация заявления отменена","Для данного события необходимо ввести ЕНП длиной 16 цифр.\n\nГенерация заявления отменена.");
            return;
        }
        if ( ui->line_pol_ser->text().trimmed().isEmpty() || ui->line_pol_num->text().trimmed().isEmpty() ) {
            QMessageBox::information(this,"Генерация заявления отменена","Для данного события необходимо ввести серию и номер полиса.\n\nГенерация заявления отменена.");
            return;
        }
    }

    // запомним послендие данные полиса
    data_vizit.v_method  = ui->combo_vizit_pan_method->currentData().toInt();
    data_vizit.date = ui->date_vizit_pan->date();
    data_vizit.has_petition = ui->ch_vizit_pan_has_petition->isChecked();
    data_vizit.r_smo   = ui->combo_vizit_pan_reason_smo->currentData().toInt();
    data_vizit.r_polis = ui->combo_vizit_pan_reason_polis->currentData().toInt();
    data_vizit.f_polis = ui->combo_vizit_pan_form_polis->currentData().toInt();
    data_polis.has_vs_num = ui->ch_vs_num->isChecked();
    data_polis.vs_num = (data_polis.has_vs_num ? ui->line_vs_num->text().trimmed() : "");
    data_polis.has_uec_num = ui->ch_uec_num->isChecked();
    data_polis.uec_num = (data_polis.has_uec_num ? ui->line_uec_num->text().trimmed() : "");
    data_polis.has_date_begin = ui->ch_pol_datbegin->isChecked();
    data_polis.date_begin = (data_polis.has_date_begin ? ui->date_pol_begin->date() : QDate(1900,1,1));
    data_polis.has_date_end = ui->ch_pol_datend->isChecked();
    data_polis.date_end = (data_polis.has_date_end ? ui->date_pol_end->date() : QDate(1900,1,1));
    data_polis.has_date_stop = ui->ch_pol_datstop->isChecked();
    data_polis.date_stop = (data_polis.has_date_stop ? ui->date_pol_stop->date() : QDate(1900,1,1));
    data_polis.in_erp = ui->combo_pol_in_erp->currentData().toInt();
    data_polis.has_pol_sernum = ui->ch_pol_sernum->isChecked();
    data_polis.pol_ser = (data_polis.has_pol_sernum ? ui->line_pol_ser->text().trimmed() : "");
    data_polis.pol_num = (data_polis.has_pol_sernum ? ui->line_pol_num->text().trimmed() : "");
    data_polis.f_polis = ui->combo_vizit_pan_form_polis->currentData().toInt();

    data_polis.has_enp = ui->ch_pol_enp->isChecked();
    if (data_polis.has_enp && ui->line_enp->text().length()==16)
        data_polis.enp = ui->line_enp->text().trimmed();

    if ( ( data_event.event_code=="П010" ||
           data_event.event_code=="П034" ||
           data_event.event_code=="П035" ||
           data_event.event_code=="П036" ||
           data_event.event_code=="П061" ||
           data_event.event_code=="П062" ||
           data_event.event_code=="П063" ) &&
          data_polis.vs_num.isEmpty() ) {
        QMessageBox::information(this,"Генерация заявления отменена","Данное событие предполагает выдачу ВС.\nНе введён номер ВС.\n\nГенерация заявления отменена.");
        return;
    }
    if ( data_polis.date_begin<QDate(1920,1,2) ) {
        QMessageBox::information(this,"Генерация заявления отменена","Дата выдачи полиса не введена или раньше 01.01.1920г.\n\nГенерация заявления отменена.");
        return;
    }

    delete  print_ch_smo_w;
    print_ch_smo_w = new print_ch_smo_wnd(db, data_app, data_pers, data_vizit, data_polis, settings, this);
    print_ch_smo_w->exec();
}
void polices_wnd::on_bn_print_notice_pol_clicked() {
}

void polices_wnd::on_bn_print_vs_clicked() {
    // запомним послендие данные полиса
    data_vizit.v_method  = ui->combo_vizit_pan_method->currentData().toInt();
    data_vizit.date = ui->date_vizit_pan->date();
    data_vizit.has_petition = ui->ch_vizit_pan_has_petition->isChecked();
    data_vizit.r_smo   = ui->combo_vizit_pan_reason_smo->currentData().toInt();
    data_vizit.r_polis = ui->combo_vizit_pan_reason_polis->currentData().toInt();
    data_vizit.f_polis = ui->combo_vizit_pan_form_polis->currentData().toInt();
    data_polis.has_vs_num = ui->ch_vs_num->isChecked();
    data_polis.vs_num = (data_polis.has_vs_num ? ui->line_vs_num->text().trimmed() : "");
    data_polis.has_uec_num = ui->ch_uec_num->isChecked();
    data_polis.uec_num = (data_polis.has_uec_num ? ui->line_uec_num->text().trimmed() : "");
    data_polis.has_date_begin = ui->ch_pol_datbegin->isChecked();
    data_polis.date_begin = (data_polis.has_date_begin ? ui->date_pol_begin->date() : QDate(1900,1,1));
    data_polis.has_date_end = ui->ch_pol_datend->isChecked();
    data_polis.date_end = (data_polis.has_date_end ? ui->date_pol_end->date() : QDate(1900,1,1));
    data_polis.has_date_stop = ui->ch_pol_datstop->isChecked();
    data_polis.date_stop = (data_polis.has_date_stop ? ui->date_pol_stop->date() : QDate(1900,1,1));
    data_polis.in_erp = ui->combo_pol_in_erp->currentData().toInt();
    data_polis.has_pol_sernum = ui->ch_pol_sernum->isChecked();
    data_polis.pol_ser = (data_polis.has_pol_sernum ? ui->line_pol_ser->text().trimmed() : "");
    data_polis.pol_num = (data_polis.has_pol_sernum ? ui->line_pol_num->text().trimmed() : "");
    data_polis.r_smo   = ui->combo_vizit_pan_reason_smo->currentData().toInt();
    data_polis.r_polis = ui->combo_vizit_pan_reason_polis->currentData().toInt();
    data_polis.f_polis = ui->combo_vizit_pan_form_polis->currentData().toInt();

    /*if (!data_polis.is_ok) {
        QMessageBox::information(this,"Генерация ВС отменена","Недостаточно данных, генерация ВС отменена.");
        return;
    }*/
    delete  print_vs_w;
    print_vs_w = new print_vs_wnd(db, data_app, data_pers, data_polis, settings, this);
    print_vs_w->exec();
}

void polices_wnd::on_bn_print_vs_2_clicked() {
    on_bn_polis_save_clicked();
    on_bn_print_vs_clicked();

    //on_bn_polis_back_clicked();
}

// ---------------------------------
// --- П010 ------------------------
// ---------------------------------
bool polices_wnd::process_P010 (s_data_polis &data_polis, s_data_event &data_event) {
    // П010 - новый застрахованный с ВС
    if (data_event.event_code!="П010" || data_polis.pol_v!=2) {
        QMessageBox::critical(this, "Не тот код события или тип полиса!", "Обработчик события П010 получил данные события <" + data_event.event_code + ">\nТип полиса - "+QString::number(data_polis.pol_v)+"\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        return false;
    };

    data_polis.is_ok = true;

    // проверим, есть ли в базе и свободен ли такой банк ВС
    QString sql_vs;
    sql_vs = "select bv.vs_num, bv.status, '('||bv.status||') '||s.text "
             "  from blanks_vs bv "
             "  left join spr_blank_status s on(s.code=bv.status) "
             " where bv.vs_num=" + QString::number(ui->line_vs_num->text().toInt()) + " ";

    QSqlQuery *query_vs = new QSqlQuery(db);
    mySQL.exec(this, sql_vs, QString("проверка номера ВС на существование и доступность бланка"), *query_vs, true, db, data_app);
    if (query_vs->size()<1) {
        QMessageBox::warning(this, "Неверный номер ВС!",
                             "Бланка ВС с номером " + QString::number(ui->line_vs_num->text().toInt()) + " нет в базе данных!\n\n"
                             "Исправьте номер ВС и повторите операцию.");
        data_polis.is_ok = false;
        delete query_vs;
        return false;
    }
    query_vs->next();
    int vs_status = query_vs->value(1).toInt();
    QString vs_status_text = query_vs->value(2).toString();
    if (vs_status!=0) {
        QMessageBox::warning(this, "Бланк ВС с этим номером не доступен!",
                             "Статус бланка ВС с номером " + QString::number(ui->line_vs_num->text().toInt()) + " - \n" + vs_status_text + "\n\n"
                             "Исправьте номер ВС и повторите операцию.");
        data_polis.is_ok = false;
        delete query_vs;
        return false;
    }
    delete query_vs;

    // проверим, попадает ли номер ВС в открытый интервал
    QString sql_int;
    sql_int= "select v.id as id_vs_inteval_pt, pt.id as id_point, '('||pt.point_regnum||') '||pt.point_name as point_name, f.folder_name "
             "  from vs_intervals_pt v "
             "  left join points pt on(pt.id=v.id_point) "
             "  left join folders f on(v.id_folder=f.id) "
             " where v.date_open<=CURRENT_DATE "
             "   and ( (v.date_close is NULL) or (v.date_close>CURRENT_DATE) ) "
             "   and " + QString::number(ui->line_vs_num->text().toInt()) + " between v.vs_from and v.vs_to ";
    QSqlQuery *query_int = new QSqlQuery(db);
    mySQL.exec(this, sql_int, QString("проверка номера ВС по открытым диапазонам"), *query_int, true, db, data_app);

    query_int->next();
    int id_int_pt = query_int->value(0).toInt();
    int id_point  = query_int->value(1).toInt();
    QString point_name  = query_int->value(2).toString().trimmed().simplified();
    QString folder_name = query_int->value(3).toString().trimmed().simplified();

    // найден ли интервал
    if (query_int->size()<1) {
        QMessageBox::warning(this, "Неверный номер ВС!",
                             "Введённый номер ВС не входит ни в один из звестных интервалов номеров ВС!\n\n"
                             "Исправьте номер ВС и повторите операцию.");
        data_polis.is_ok = false;
        delete query_int;
        return false;
    }
    delete query_int;

    // соответствие интервала и ПВП
    if (id_point!=data_app.id_point) {
        QString pss;
        do {
            pss = QInputDialog::getText(this,"Несоответсивие кода ПВП !!!",
                                        "Бланк ВС с номером " + QString::number(ui->line_vs_num->text().toInt()) + " распределён на другой ПВП - " + point_name + "\n\n"
                                        "что не соответствует активному пункту выдачи полисов - " + data_app.point_name + "\n\n"
                                        "Для того чтобы всё равно использовать этот бланк\n - введите пароль - " +
                                        (data_app.is_head||data_app.is_tech ? "ПЕРЕДАЧА" : "***") +
                                        "\n\nПустая строка - отмена сохранения.").toUpper();
            if (pss.isEmpty())
                return false;
        } while (pss!="ПЕРЕДАЧА" && pss!="GTHTLFXF");
    }

    // соответствие интервала и папки
    data_app.default_folder = data_app.default_folder.trimmed().simplified();
    if ( !folder_name.isEmpty()
         && !data_app.default_folder.isEmpty()
         && data_app.default_folder!=folder_name ) {
        QString pss;
        do {
            pss = QInputDialog::getText(this,"Несоответсивие папки !!!",
                                        "Бланк ВС распределён в папку - " + folder_name + ",\n"
                                        "а текущая папка - " + data_app.default_folder + "\n\n"
                                        "Для того чтобы всё равно использовать этот бланк\n - введите пароль - " +
                                        (data_app.is_head||data_app.is_tech ? "ПАПОЧКА" : "***") +
                                        "\n\nПустая строка - отмена сохранения.").toUpper();
            if (pss.isEmpty())
                return false;
        } while (pss!="ПАПОЧКА" && pss!="GFGJXRF");
    }

    /*// проверим, не испорчен ли бланк ВС с таким номером
    QString sql_tst;
    sql_tst = "select b.vs_num "
                " from  blanks_vs b "
                " where b.vs_num=" + ui->line_vs_num->text().trimmed() + " "
                "   and b.status=-1 ; ";
    QSqlQuery* query_tst = new QSqlQuery(db);
    mySQL.exec(this, sql_tst, QString("проверка, не испорчен ли бланк ВС с таким номером"), *query_tst, true, db, data_app);
    if (query_tst->size()>0) {
        QMessageBox::critical(this, "Бланк ВС испорчен!", "Бланк ВС с указанным номером помечен как испорченный!\n\nИсправьте номер ВС и повторите операцию.");
        data_polis.is_ok = false;
        delete query_tst;
        return false;
    }
    delete query_tst;*/

    // проверим, свободен ли бланк ВС с таким номером
    QString sql_tst2;
    sql_tst2 = "select b.vs_num, s.code, '('||s.code||') '||s.text "
               "  from blanks_vs b "
               "  left join spr_blank_status s on(s.code=b.status) "
               " where b.vs_num=" + ui->line_vs_num->text().trimmed() + " "
               "   and b.status<>0 ; ";
    QSqlQuery* query_tst2 = new QSqlQuery(db);
    mySQL.exec(this, sql_tst2, QString("проверка, свободен ли бланк ВС с таким номером"), *query_tst2, true, db, data_app);
    if (query_tst2->next()) {
        QString status_text = query_tst2->value(2).toString();
        QMessageBox::critical(this, "Номер ВС занят!",
                              "Бланк ВС с указанным номером уже был использован ране!\n" + status_text + "\n\nИсправьте номер ВС и повторите операцию.");
        data_polis.is_ok = false;
        delete query_tst2;
        return false;
    }
    delete query_tst2;

    /*// если бумажный полис единого образца (серия и номер бланка)
    if (data_event.event_code=="П060") {
        // проверим, свободны ли номер и серия полиса
        QString sql_enp;
        sql_enp = "select b.pol_ser, b.pol_num "
                  " from  blanks_pol b "
                  " where v.pol_ser=" + ui->line_pol_ser->text().trimmed() + " and v.pol_num='" + ui->line_pol_num->text().trimmed() + "' ; ";
        QSqlQuery *query_enp = new QSqlQuery(db);
        mySQL.exec(this, sql_enp, QString("проверка, свободны ли серия/номер полиса"), *query_enp, true);
        if (query_enp->size()>0) {
            QMessageBox::critical(this, "Серия и номер полиса уже используются!", "Бланк полиса с указанными серией и номером уже используется!\n Исправьте серию и номер бленка полиса и повторите операцию.");
            data_polis.is_ok = false;
            delete query_enp;
            return false;
        }
        delete query_enp;
    }*/

    // проверка, можно ли применять это событие и надо ли закрывать старые полисы
    QString sql_old = "";
    sql_old += "select p.id, p.id_person, p.ocato, p.ogrn_smo, p.pol_v, f8.text as pol_v_text, p.pol_type, p.pol_ser, p.pol_num, p.vs_num, p.enp, p.date_begin, p.date_end, p.date_stop, p.in_erp, p.order_num, p.order_date, p._id_last_point, p._id_last_operator, p._id_last_event "
               "  from polises p "
               "  left join spr_f008 f8 on(f8.code=p.pol_v) "
               "  left join blanks_pol bp on(bp.id_polis=p.id) "
               "  left join blanks_vs bv on(bv.id_polis=p.id) "
               " where p.id_person=" + QString::number(data_pers.id) + " "
               "   and ((bp.id is not NULL and bp.status>0) or (bv.vs_num is not NULL and bv.status>0)) "
               "   and p.date_begin is not null  "
               "   and p.date_begin<=CURRENT_DATE  "
               "   and ( (p.date_stop is NULL) or (p.date_stop>CURRENT_DATE) ) ; ";

    QSqlQuery *query_old = new QSqlQuery(db);
    bool res_old = mySQL.exec(this, sql_old, QString("Новый полис - проверка, есть ли действующие старые полисы"), *query_old, true, db, data_app);
    // проверим старые полисы
    while (query_old->next()) {
        s_data_polis data_old_polis;
        data_old_polis.id = query_old->value(0).toInt();
        data_old_polis.id_person = query_old->value(1).toInt();
        data_old_polis.ocato = query_old->value(2).toString();
        data_old_polis.ogrn_smo = query_old->value(3).toString();
        data_old_polis.pol_v = query_old->value(4).toInt();
        data_old_polis.pol_v_text = query_old->value(5).toString();
        data_old_polis.pol_type = query_old->value(6).toString();
        data_old_polis.has_enp = !(query_old->value(10).isNull());
        data_old_polis.has_pol_sernum = !(query_old->value(8).isNull());
        data_old_polis.has_vs_num = !(query_old->value(9).isNull());
        data_old_polis.pol_ser = query_old->value(7).toString();
        data_old_polis.pol_num = query_old->value(8).toString();
        data_old_polis.vs_num = query_old->value(9).toString();
        data_old_polis.enp = query_old->value(10).toString();
        data_old_polis.has_date_begin = !(query_old->value(11).isNull());
        data_old_polis.has_date_end = !(query_old->value(12).isNull());
        data_old_polis.has_date_stop = !(query_old->value(13).isNull());
        data_old_polis.date_begin = query_old->value(11).toDate();
        data_old_polis.date_end = query_old->value(12).toDate();
        data_old_polis.date_stop = query_old->value(13).toDate();
        data_old_polis.in_erp = query_old->value(14).toInt();
        data_old_polis.order_num = query_old->value(15).toString();
        data_old_polis.order_date = query_old->value(16).toDate();
        data_old_polis._id_last_point = query_old->value(17).toInt();
        data_old_polis._id_last_operator = query_old->value(18).toInt();
        data_old_polis._id_last_event = query_old->value(19).toInt();

        // другого действующего полиса быть не должно
        if (data_old_polis.pol_v==2) {
            QMessageBox::warning(this,
                                 "Невозможно открыть ВС!",
                                 QString("Невозможно открыть новое временное свидетельство, поскольку у застрахованного уже есть другое действующее ВС:\n ") +
                                 "номер: " + QString::number(query_old->value(9).toInt()) + "\n" +
                                 "дата выдачи: " + query_old->value(11).toDate().toString("dd.MM.yyyy") + "\n" +
                                 "№ ВС: " + query_old->value(9).toString() + "\n" +
                                 "ЕНП: " + query_old->value(10).toString() + "\n\n" +
                                 "Надо дождаться активации этого ВС из ТФОМС и, при необходимости, выдать новое ВС с закрытием полученного полиса единого образца.");
            data_polis.is_ok = false;
            delete query_old;
            return false;
        }
        if ((data_old_polis.pol_v==1 || data_old_polis.pol_v==3)) {
            data_polis.id_old_polis = data_old_polis.id;
            QMessageBox::warning(this,
                                 "Полис уже есть",
                                 QString("У застрахованного уже есть действующий полис ОМС единого образца или полис ОМС старого образца нашей СМО: \n") +
                                 "тип: " + query_old->value(4).toString() + "\n" +
                                 "серия/номер: " + query_old->value(7).toString() + " № " + query_old->value(8).toString() + "\n" +
                                 "дата выдачи: " + query_old->value(11).toDate().toString("dd.MM.yyyy") + "\n" +
                                 "№ ВС: " + query_old->value(9).toString() + "\n" +
                                 "ЕНП: " + query_old->value(10).toString() + "\n\n" +
                                 "Событие П010 предполагает, что у застрахованного вообще нет полиса ОМС. \nВыберите другое событие.\n\nОперация страхования отменена.");
            // отмена операции
            data_polis.is_ok = false;
            delete query_old;
            return false;
        }
    }
    delete query_old;

    // добавим бланк ВС
    /*QString sql_add_blank =
            "insert into blanks_vs(vs_num, id_point, id_operator, date_add, status, id_person, id_polis) "
            " values(" + ui->line_vs_num->text().trimmed() + ", "
                     + QString::number(data_app.id_point) + ", "
                     + QString::number(data_app.id_operator) + ", "
                     + "'" + QDate::currentDate().toString("dd.MM.yyyy") + "', "
                     + " 1, "
                     + " " + QString::number(data_pers.id) + ", "
                     + " NULL) "
                     + " returning vs_num ; ";*/
    QString sql_add_blank =
            "update blanks_vs "
            "   set id_point=" + QString::number(data_app.id_point) + ", "
            "       id_operator=" + QString::number(data_app.id_operator) + ", "
            //"       date_add='" + QDate::currentDate().toString("dd.MM.yyyy") + "', "
            "       status=1, "
            "       id_person=" + QString::number(data_pers.id) + ", "
            "       id_polis=NULL, "
            "       date_spent = coalesce(date_spent, CURRENT_DATE) "
            " where vs_num='" + ui->line_vs_num->text().trimmed() + "' ; ";
    QSqlQuery *query_add_blank = new QSqlQuery(db);
    mySQL.exec(this, sql_add_blank, QString("Добавление бланка ВС"), *query_add_blank, true, db, data_app);
    if (query_add_blank->size()==0) {
        QMessageBox::critical(this, "Ошибка при добавлении бланка ВС!", "При добавлении бланка ВС произошла неожиданная ошибка!\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        delete query_add_blank;
        return false;
    }
    delete query_add_blank;

    // добавим полис в базу
    QString sql;
    sql = "insert into polises (id_person, ocato, enp, ogrn_smo, pol_v, pol_type, pol_ser, pol_num, vs_num, date_begin, date_end, date_stop, in_erp, /*order_num, order_date,*/ id_old_polis, _id_last_point, _id_last_operator, _id_last_event, f_polis, date_get2hand, r_smo, r_polis) ";
    sql +=  " values (";
    sql +=  " "  + QString::number(data_polis.id_person) + ", ";
    sql +=  " '" + data_polis.ocato.trimmed() + "', ";
    sql +=  " '" + data_polis.enp.trimmed() + "', ";
    sql +=  " '" + data_polis.ogrn_smo.trimmed() + "', ";
    sql +=  " "  + QString::number(data_polis.pol_v) + ", ";
    sql +=  " '" + data_polis.pol_type.trimmed() + "', ";
    sql +=  (ui->ch_pol_sernum->isChecked()   ? (" '" + data_polis.pol_ser + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_sernum->isChecked()   ? (" '" + data_polis.pol_num + "'") : "NULL") + ", ";
    sql +=  (ui->ch_vs_num->isChecked()    ? (" '" + data_polis.vs_num  + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datbegin->isChecked() ? (" '" + data_polis.date_begin.toString("yyyy-MM-dd") + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datend->isChecked()   ? (" '" + data_polis.date_end.toString("yyyy-MM-dd")   + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datstop->isChecked()  ? (" '" + data_polis.date_stop.toString("yyyy-MM-dd")  + "'") : "NULL") + ", ";
    sql +=  " "  + QString::number(data_polis.in_erp) + ", ";
    //sql +=  " '" + data_polis.order_num.trimmed() + "', ";
    //sql +=  " '" + data_polis.order_date.toString("yyyy-MM-dd") + "', ";
    sql +=  (data_polis.id_old_polis<0 ? "NULL" : QString::number(data_polis.id_old_polis)) + ", ";
    sql +=  QString::number(data_app.id_point) + ", ";
    sql +=  QString::number(data_app.id_operator) + ", ";
    sql +=  "NULL, ";
    sql +=  QString::number(ui->combo_vizit_pan_form_polis->currentIndex()) + ", ";
    sql +=  " '" + QDate::currentDate().toString("yyyy-MM-dd") + "', ";
    sql +=  QString::number(data_vizit.r_smo) + ", ";
    sql +=  QString::number(data_vizit.r_polis) + " ";
    sql +=  ")  returning id ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString("П010 - Добавление нового ВС"), *query, true, db, data_app);
    if (!res) {
        QMessageBox::critical(this, "Операция не выполнена!", "П010 - Добавление нового ВС - Операция страхования отменена.");
        data_polis.is_ok = false;
        delete query;
        return false;
    } else {
        while (query->next()) {
            data_polis.id = query->value(0).toInt();
            data_event.is_ok = true;

            // добавим событие в базу
            QString sql = "insert into events (id_point, id_operator, id_polis, id_vizit, event_code, event_dt, event_time, comment, status, id_person, id_person_old, id_udl, id_drp) ";
            sql +=  " values (";
            sql +=  " "  + QString::number(data_app.id_point) + ", ";
            sql +=  " "  + QString::number(data_app.id_operator) + ", ";
            sql +=  " "  + QString::number(data_polis.id) + ", ";
            sql +=  " "  + QString::number(data_vizit.id) + ", ";
            sql +=  " '" + data_event.event_code.trimmed() + "', ";
            sql +=  " '" + data_event.event_dt.toString("yyyy-MM-dd") + "', ";
            sql +=  " '" + data_event.event_time.toString("hh:mm:ss") + "', ";
            sql +=  " '" + data_event.comment.trimmed() + "', 0, ";
            sql +=  " "  + QString::number(data_pers.id) + ", ";
            sql +=  " "  + (data_pers.id_old>0 ? QString::number(data_pers.id_old) : "NULL") + ", ";
            sql +=  " "  + (data_pers.id_udl>0 ? QString::number(data_pers.id_udl) : "NULL") + ", ";
            sql +=  " "  + (data_pers.id_drp>0 ? QString::number(data_pers.id_drp) : "NULL") + ") ";
            sql +=  " returning id ; ";

            QSqlQuery *query_act = new QSqlQuery(db);
            bool res_act = mySQL.exec(this, sql, QString("Добавление нового события"), *query_act, true, db, data_app);
            if (!res_act || !query_act->next()) {
                QMessageBox::critical(this, "Операция не выполнена!", "П010 - Добавление нового события - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_act;
                return false;
            }

            // добавим _id_last_event в полис
            data_polis._id_last_event = query_act->value(0).toInt();
            delete query_act;
            QString sql_pa = "update polises "
                             "   set _id_last_event=" + QString::number(data_polis._id_last_event) + " "
                             " where id=" + QString::number(data_polis.id) + " ; ";

            QSqlQuery *query_pa = new QSqlQuery(db);
            bool res_pa = mySQL.exec(this, sql_pa, QString("Добавление id события в полис"), *query_pa, true, db, data_app);
            if (!res_pa) {
                QMessageBox::critical(this, "Операция не выполнена!", "П010 - Добавление id события в полис - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_pa;
                return false;
            }
            delete query_pa;

            // добавим id_polis в бланк ВС
            QString sql_pb = "update blanks_vs "
                             "   set id_polis=" + QString::number(data_polis.id) + " "
                             " where vs_num=" + data_polis.vs_num + " ; ";

            QSqlQuery *query_pb = new QSqlQuery(db);
            bool res_pb = mySQL.exec(this, sql_pb, QString("Добавление id события в полис"), *query_pb, true, db, data_app);
            if (!res_pb) {
                QMessageBox::critical(this, "Операция не выполнена!", "П010 - Добавление id полиса в бланк - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_pb;
                return false;
            }
            delete query_pb;
        }
    }
    delete query;

    // запомним форму полиса в данных визита
    QString sql_polf = "update vizits "
                       " set f_polis=" + QString::number(data_polis.f_polis) + " "
                       " where id=" + QString::number(data_vizit.id) + " ; ";
    QSqlQuery *query_polf = new QSqlQuery(db);
    bool res_polf = mySQL.exec(this, sql_polf, QString("запись формы полиса в визит"), *query_polf, true, db, data_app);
    if (!res_polf) {
        QMessageBox::critical(this, "Операция не выполнена!", "При записи формы выпуска полиса в визит произошла ошибка - операция отменена отменена.");
        delete query_polf;
        return false;
    }
    delete query_polf;

    // запомним статус персоны - 1
    QString sql_pers_stat = "update persons "
                            " set status=1, "
                            "     _date_insure_begin=COALESCE(_date_insure_begin, CURRENT_DATE), "
                            "     _date_insure_end=NULL "
                            " where id=" + QString::number(data_pers.id) + " ; ";
    QSqlQuery *query_pers_stat = new QSqlQuery(db);
    bool res_pers_stat = mySQL.exec(this, sql_pers_stat, QString("Правка статуса персоны (1)"), *query_pers_stat, true, db, data_app);
    if (!res_pers_stat) {
        QMessageBox::critical(this, "Операция не выполнена!", "При правке статуса персоны (1) произошла ошибка - операция отменена отменена.");
        delete query_pers_stat;
        return false;
    }
    delete query_pers_stat;

    data_polis.is_ok = true;
    return true;
}


// ----------------------------------
// -- П021, П022, П023, П024, П025 --
// ----------------------------------
bool polices_wnd::process_P021_P022_P023_P024_P025 (s_data_polis &data_polis, s_data_event &data_event, int new_status, bool inner_act) {
    // П021 - закрытие полиса
    if ( ( data_event.event_code!="П021" &&
           data_event.event_code!="П022" &&
           data_event.event_code!="П023" &&
           data_event.event_code!="П024" &&
           data_event.event_code!="П025"
         ) /*|| data_polis.pol_v!=2*/ ) {
        QMessageBox::critical(this, "Не тот код события или тип полиса!", "Обработчик события П021, П022, П023, П024, П025 получил данные события <" + data_event.event_code + ">\nТип полиса - "+QString::number(data_polis.pol_v)+"\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        return false;
    };

    if (new_status!=-2 && new_status!=-3 && new_status!=-5) {
        QMessageBox::critical(this, "Новый статус полиса не соответствует событиям П021, П022, П023, П024, П025!", "Новый статус полиса, <" + QString::number(new_status) + ">, не входит в допустимое множество \n(2<полис изъят>, 3<полис испорчен>). \n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        return false;
    }

    data_polis.is_ok = true;

    if (data_polis.pol_v==1 || data_polis.pol_v==3) {
        // закроем бланк полиса и все не закрытые бланки ВС
        QString sql_close;
        sql_close = "update blanks_pol "
                    "   set status=" + QString::number(new_status) + ", "
                    "       date_spent = coalesce(date_spent, '" + data_vizit.date.toString("yyyy-MM-dd") + "'), "
                    "       date_waste = coalesce(date_spent, '" + data_vizit.date.toString("yyyy-MM-dd") + "') "
                    " where pol_ser='" + data_polis.pol_ser + "' and pol_num='" + data_polis.pol_num + "' ; "

                    "update blanks_vs "
                    "   set status=" + QString::number(new_status) + ", "
                    "       date_spent = coalesce(date_spent, '" + data_vizit.date.toString("yyyy-MM-dd") + "'), "
                    "       date_waste = coalesce(date_spent, '" + data_vizit.date.toString("yyyy-MM-dd") + "') "
                    " where id_person=" + QString::number(data_polis.id_person) + " "
                    "   and status=1 ; ";
        QSqlQuery *query_close = new QSqlQuery(db);
        mySQL.exec(this, sql_close, QString("П02х - Закрытие бланка полиса"), *query_close, true, db, data_app);
        if (query_close->size()==0) {
            QMessageBox::critical(this, "Ошибка при закрытии бланка полиса!", "При закрытии бланка полиса произошла неожиданная ошибка!\n\nОперация страхования отменена.");
            data_polis.is_ok = false;
            delete query_close;
            return false;
        }
        delete query_close;

        // при закрытии полиса единого образца - так же закроем все не закрытые ВС этой персоны в базе
        QString sql_vs;
        sql_vs = "update polises "
                 "   set date_stop='" + data_vizit.date.toString("yyyy-MM-dd") + "', "
                 "       _id_last_point=" + QString::number(data_app.id_point) + ", "
                 "       _id_last_operator=" + QString::number(data_app.id_operator) + " "
                 " where id_person=" + QString::number(data_polis.id_person) + " "
                 "   and pol_v=2 "
                 "   and date_stop is NULL ; ";

        QSqlQuery *query_vs = new QSqlQuery(db);
        bool res_vs = mySQL.exec(this, sql_vs, QString("П02х - Закрытие ещё не закрытых бланков ВС"), *query_vs, true, db, data_app);
        if (!res_vs) {
            QMessageBox::critical(this, "Операция не выполнена!", "П021, П022, П023, П024, П025 - Закрытие незакрытых ВС для закрываемого полиса - Операция страхования отменена.");
            data_polis.is_ok = false;
            delete query_vs;
            return false;
        }
        delete query_vs;

    } else if (data_polis.pol_v==2) {
        // закроем бланк ВС
        QString sql_close =
              "update blanks_vs "
              "   set status=" + QString::number(new_status) + ", "
              "       date_spent = coalesce(date_spent, '" + data_vizit.date.toString("yyyy-MM-dd") + "'), "
              "       date_waste = coalesce(date_spent, '" + data_vizit.date.toString("yyyy-MM-dd") + "') "
              " where vs_num=" + data_polis.vs_num + " ; ";
        QSqlQuery *query_close = new QSqlQuery(db);
        mySQL.exec(this, sql_close, QString("Закрытие бланка полиса"), *query_close, true, db, data_app);
        if (query_close->size()==0) {
            QMessageBox::critical(this, "Ошибка при закрытии бланка полиса!", "При закрытии бланка полиса произошла неожиданная ошибка!\n\nОперация страхования отменена.");
            data_polis.is_ok = false;
            delete query_close;
            return false;
        }
        delete query_close;
    }

    // закроем полис в базе
    QString sql_pol;
    sql_pol = "update polises "
              "   set date_stop='" + data_vizit.date.toString("yyyy-MM-dd") + "', "
              "       _id_last_point=" + QString::number(data_app.id_point) + ", "
              "       _id_last_operator=" + QString::number(data_app.id_operator) + " " +
              QString(( !( data_polis.has_date_end )
                        || data_polis.date_end>data_vizit.date )
                      ? QString(", date_end='" + data_vizit.date.toString("yyyy-MM-dd") + "' ")
                      : "" ) +
              " where id=" + QString::number(data_polis.id) + " ; ";

    QSqlQuery *query_pol = new QSqlQuery(db);
    bool res_pol = mySQL.exec(this, sql_pol, QString("П02х - Закрытие полиса"), *query_pol, true, db, data_app);
    if (!res_pol) {
        QMessageBox::critical(this, "Операция не выполнена!", "П021, П022, П023, П024, П025 - Закрытие полиса - Операция страхования отменена.");
        data_polis.is_ok = false;
        delete query_pol;
        return false;
    }
    delete query_pol;

    // добавим событие на отправку, если надо ...
    if ( !inner_act || data_app.send_p02_in_p06>0 ) {
        // добавим событие в базу
        QString sql = "insert into events (id_point, id_operator, id_polis, id_vizit, event_code, event_dt, event_time, comment, status, id_person, id_person_old, id_udl, id_drp) ";
        sql +=  " values (";
        sql +=  " "  + QString::number(data_app.id_point) + ", ";
        sql +=  " "  + QString::number(data_app.id_operator) + ", ";
        sql +=  " "  + QString::number(data_event.id_polis) + ", ";
        sql +=  " "  + QString::number(data_vizit.id) + ", ";
        sql +=  " '" + data_event.event_code + "', ";
        sql +=  " '" + data_event.event_dt.toString("yyyy-MM-dd") + "', ";
        sql +=  " '" + data_event.event_time.toString("hh:mm:ss") + "', ";
        sql +=  " '" + data_event.comment + "', -1,";
        sql +=  " "  + QString::number(data_pers.id) + ", ";
        sql +=  " "  + (data_pers.id_old>0 ? QString::number(data_pers.id_old) : "NULL") + ", ";
        sql +=  " "  + (data_pers.id_udl>0 ? QString::number(data_pers.id_udl) : "NULL") + ", ";
        sql +=  " "  + (data_pers.id_drp>0 ? QString::number(data_pers.id_drp) : "NULL") + ") ";
        sql +=  " returning id ; ";

        QSqlQuery *query_act = new QSqlQuery(db);
        bool res_act = mySQL.exec(this, sql, QString("П02х - Добавление события"), *query_act, true, db, data_app);
        if (!res_act || !query_act->next()) {
            QMessageBox::critical(this, "Операция не выполнена!", "П021, П022, П023, П024, П025 - Добавление события - Операция страхования отменена.");
            data_event.is_ok = false;
            delete query_act;
            return false;
        }

        // добавим _id_last_event в полис
        data_polis._id_last_event = query_act->value(0).toInt();
        QString sql_pa = "update polises "
                         "   set _id_last_event=" + QString::number(data_polis._id_last_event) + " "
                         " where id=" + QString::number(data_polis.id) + " ; ";
        delete query_act;

        QSqlQuery *query_pa = new QSqlQuery(db);
        bool res_pa = mySQL.exec(this, sql_pa, QString("П021 - Добавление id события в полис"), *query_pa, true, db, data_app);
        if (!res_pa) {
            QMessageBox::critical(this, "Операция не выполнена!", "П021, П022, П023, П024, П025 - Добавление id события в полис - Операция страхования отменена.");
            data_event.is_ok = false;
            delete query_pa;
            return false;
        }
        delete query_pa;

        if (data_event.event_code=="П021") {
            // запомним статус персоны - -3
            QString sql_pers_stat = "update persons "
                                    " set status=-3, "
                                    "     _date_insure_end=CURRENT_DATE "
                                    " where id=" + QString::number(data_pers.id) + " ; ";
            QSqlQuery *query_pers_stat = new QSqlQuery(db);
            bool res_pers_stat = mySQL.exec(this, sql_pers_stat, QString("Правка статуса персоны (-3)"), *query_pers_stat, true, db, data_app);
            if (!res_pers_stat) {
                QMessageBox::critical(this, "Операция не выполнена!", "При правке статуса персоны (-3) произошла ошибка - операция отменена отменена.");
                delete query_pers_stat;
                return false;
            }
            delete query_pers_stat;
        } else if (data_event.event_code=="П022") {
            // запомним статус персоны - -1
            QString sql_pers_stat = "update persons "
                                    " set status=-1, "
                                    "     _date_insure_end=CURRENT_DATE "
                                    " where id=" + QString::number(data_pers.id) + " ; ";
            QSqlQuery *query_pers_stat = new QSqlQuery(db);
            bool res_pers_stat = mySQL.exec(this, sql_pers_stat, QString("Правка статуса персоны (-1)"), *query_pers_stat, true, db, data_app);
            if (!res_pers_stat) {
                QMessageBox::critical(this, "Операция не выполнена!", "При правке статуса персоны (-1) произошла ошибка - операция отменена отменена.");
                delete query_pers_stat;
                return false;
            }
            delete query_pers_stat;
        } else {
            // запомним статус персоны - 0
            QString sql_pers_stat = "update persons "
                                    " set status=-1, "
                                    "     _date_insure_end=CURRENT_DATE "
                                    " where id=" + QString::number(data_pers.id) + " ; ";
            QSqlQuery *query_pers_stat = new QSqlQuery(db);
            bool res_pers_stat = mySQL.exec(this, sql_pers_stat, QString("Правка статуса персоны (-1)"), *query_pers_stat, true, db, data_app);
            if (!res_pers_stat) {
                QMessageBox::critical(this, "Операция не выполнена!", "При правке статуса персоны (-1) произошла ошибка - операция отменена отменена.");
                delete query_pers_stat;
                return false;
            }
            delete query_pers_stat;
        }
    }

    // обновим связку персоны с прикреплением
    QString sql_link = "select * from update_pers_links(" + QString::number(data_pers.id) + ", NULL) ; ";
    QSqlQuery *query_link = new QSqlQuery(db);
    mySQL.exec(this, sql_link, "Обновление связки с прикреплением застрахованного", *query_link, true, db, data_app);
    delete query_link;

    data_polis.is_ok = true;
    return true;
}


// ---------------------------------
// --- П031, П032, П033 ------------
// ---------------------------------
bool polices_wnd::process_P031_P032_P033 (s_data_polis &data_polis, s_data_event &data_event) {
    // П031, П032, П033 - Замена СМО 1 раз в год по желанию без замены полиса
    if ( data_event.event_code!="П031" &&
         data_event.event_code!="П032" &&
         data_event.event_code!="П033" ) {
        QMessageBox::critical(this, "Не тот код события или тип полиса!", "Обработчик события П031, П032, П033 получил данные события <" + data_event.event_code + ">\nТип полиса - "+QString::number(data_polis.pol_v)+"\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        return false;
    };
    // проверка длины введённой серии полиса
    if (ui->ch_pol_sernum->isChecked() && !(ui->line_pol_ser->text().trimmed().isEmpty()) && ui->line_pol_ser->text().length()!=4) {
        if (QMessageBox::question(this,
                                  "Нужно подтверждание",
                                  "Внимание!\n"
                                  "Длина введённой серии полиса не равна 4.\n\n"
                                  "Всё равно сохранить полис?",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Cancel)==QMessageBox::Cancel) {
            data_polis.is_ok = false;
            return false;
        }
    }

    /*// запросим форму полиса
    int close_stat = -2;
    QStringList c_list;
    QStringList s_list;
    QSqlQuery *query_fpolis = new QSqlQuery(db);
    QString sql_fpolis = "select code, '(' || code || ') ' || text "
                          " from public.spr_fpolis s "
                          " where code>0 "
                          " order by code ; ";
    mySQL.exec(this, sql_fpolis, QString("Справочник форм изготовления полиса"), *query_fpolis, true);
    c_list.clear();
    s_list.clear();
    while (query_fpolis->next()) {
        c_list.append(query_fpolis->value(0).toString());
        s_list.append(query_fpolis->value(1).toString());
    }
    do {
        ui->combo_vizit_pan_form_polis->setCurrentIndex(
                    ui->combo_vizit_pan_form_polis->findText (
                        QInputDialog::getItem(this, "Укажите форму полиса",
                                              "Укажите форму полиса, который будет добавлен в базу данных:",
                                              s_list, 0, false)
                    ) );
        data_vizit.f_polis = ui->combo_vizit_pan_form_polis->currentData().toInt();
    } while (data_vizit.f_polis==0);
    */
    data_vizit.f_polis = ui->combo_vizit_pan_form_polis->currentData().toInt();

    // запишем форму изготовления полиса в базу
    QSqlQuery *query_vizit = new QSqlQuery(db);
    QString sql_vizit = "update vizits "
                        "   set f_polis=" + QString::number(data_vizit.f_polis) + " "
                        " where id=" + QString::number(data_vizit.id) + " ; ";
    bool res_vizit = mySQL.exec(this, sql_vizit, QString("Справочник форм изготовления полиса"), *query_vizit, true, db, data_app);
    if (!res_vizit) {
        QMessageBox::critical(this, "Операция не выполнена!", "При попытке записать в данные визита форму изготовления добавляемого полиса произошла непредвиденная ошибка.\nОперация страхования отменена!");
        data_polis.is_ok = false;
        delete query_vizit;
        return false;
    }
    delete query_vizit;

    data_polis.is_ok = true;

    // проверка наличия нужных полей
    if (data_polis.pol_v<2) {
        QMessageBox::warning(this, "Недостаточно данных!", "Не выбран или недопустимый тип включаемого в базу ИНКО-МЕД полиса другой СМО.\n\nВыберите тип полиса и повторите операцию.");
        data_polis.is_ok = false;
        return false;
    }
    if (data_polis.pol_ser.isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных!", "Не введена серия включаемого в базу ИНКО-МЕД полиса другой СМО.\n\nВведите серию бланка полиса и повторите операцию.");
        data_polis.is_ok = false;
        return false;
    }
    if (data_polis.pol_num.isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных!", "Не введён номер включаемого в базу ИНКО-МЕД полиса другой СМО.\n\nВведите номер бланка полиса и повторите операцию.");
        data_polis.is_ok = false;
        return false;
    }
    if (data_polis.enp.isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных!", "Не введён ЕНП включаемого в базу ИНКО-МЕД полиса другой СМО.\n\nВведите ЕНП и повторите операцию.");
        data_polis.is_ok = false;
        return false;
    }
    if ( !(data_polis.has_date_begin) ) {
        QMessageBox::warning(this, "Недостаточно данных!", "Не введена дата выдачи включаемого в базу ИНКО-МЕД полиса другой СМО.\n\nВведите дату выдачи полиса и повторите операцию.");
        data_polis.is_ok = false;
        return false;
    }
    if ( data_polis.has_date_end ) {
        QMessageBox::warning(this, "Полис закрыт?", "Задана дата окончания действия включаемого в базу ИНКО-МЕД полиса другой СМО. Полис не должен быть закрыт.\n\nУберите дату окончания действия полиса и повторите операцию.");
        data_polis.is_ok = false;
        return false;
    }
    if ( data_polis.has_date_stop ) {
        QMessageBox::warning(this, "Полис изъят?", "Задана дата изъятия включаемого в базу ИНКО-МЕД полиса другой СМО. Полис не должен быть изъят.\n\nУберите дату изъятия полиса и повторите операцию.");
        data_polis.is_ok = false;
        return false;
    }

    // проверка, можно ли применять это событие и надо ли закрывать старые полисы
    QString sql_old = "";
    sql_old += "select p.id, p.id_person, p.ocato, p.ogrn_smo, p.pol_v, f8.text as pol_v_text, p.pol_type, p.pol_ser, p.pol_num, p.vs_num, p.enp, p.date_begin, p.date_end, p.date_stop, p.in_erp, p.order_num, p.order_date, p._id_last_point, p._id_last_operator, p._id_last_event "
               "  from polises p "
               "  left join spr_f008 f8 on(f8.code=p.pol_v) "
               "  left join blanks_pol bp on(bp.id_polis=p.id) "
               "  left join blanks_vs bv on(bv.id_polis=p.id) "
               " where p.id_person=" + QString::number(data_pers.id) + " "
               "   and ((bp.id is not NULL and bp.status>0) or (bv.vs_num is not NULL and bv.status>0)) "
               "   and p.date_begin is not null  "
               "   and p.date_begin<=CURRENT_DATE  "
               "   and ( (p.date_stop is NULL) or (p.date_stop>CURRENT_DATE) ) ; ";

    QSqlQuery *query_old = new QSqlQuery(db);
    bool res_old = mySQL.exec(this, sql_old, QString("Новый полис - проверка, есть ли действующие старые полисы"), *query_old, true, db, data_app);
    // проверим старые полисы
    while (query_old->next()) {
        s_data_polis data_old_polis;
        data_old_polis.id = query_old->value(0).toInt();
        data_old_polis.id_person = query_old->value(1).toInt();
        data_old_polis.ocato = query_old->value(2).toString();
        data_old_polis.ogrn_smo = query_old->value(3).toString();
        data_old_polis.pol_v = query_old->value(4).toInt();
        data_old_polis.pol_v_text = query_old->value(5).toString();
        data_old_polis.pol_type = query_old->value(6).toString();
        data_old_polis.has_enp = !(query_old->value(10).isNull());
        data_old_polis.has_pol_sernum = !(query_old->value(8).isNull());
        data_old_polis.has_vs_num = !(query_old->value(9).isNull());
        data_old_polis.pol_ser = query_old->value(7).toString();
        data_old_polis.pol_num = query_old->value(8).toString();
        data_old_polis.vs_num = query_old->value(9).toString();
        data_old_polis.enp = query_old->value(10).toString();
        data_old_polis.has_date_begin = !(query_old->value(11).isNull());
        data_old_polis.has_date_end = !(query_old->value(12).isNull());
        data_old_polis.has_date_stop = !(query_old->value(13).isNull());
        data_old_polis.date_begin = query_old->value(11).toDate();
        data_old_polis.date_end = query_old->value(12).toDate();
        data_old_polis.date_stop = query_old->value(13).toDate();
        data_old_polis.in_erp = query_old->value(14).toInt();
        data_old_polis.order_num = query_old->value(15).toString();
        data_old_polis.order_date = query_old->value(16).toDate();
        data_old_polis._id_last_point = query_old->value(17).toInt();
        data_old_polis._id_last_operator = query_old->value(18).toInt();
        data_old_polis._id_last_event = query_old->value(19).toInt();

        // другого действующего полиса быть не должно
        //if ((data_old_polis.pol_v==1 || data_old_polis.pol_v==2 || data_old_polis.pol_v==3)) {
        QMessageBox::warning(this,
                             "Полис единого образца уже есть!",
                             QString("Невозможно выполнить замену СМО, поскольку у застрахованного уже есть наш полис единого образца:\n ") +
                             "тип: " + query_old->value(4).toString() + "\n" +
                             "серия/номер: " + query_old->value(7).toString() + " № " + query_old->value(8).toString() + "\n" +
                             "дата выдачи: " + query_old->value(11).toDate().toString("dd.MM.yyyy") + "\n" +
                             "№ ВС: " + query_old->value(9).toString() + "\n" +
                             "ЕНП: " + query_old->value(10).toString() + "\n\n" +
                             "Операция страхования отменена.");
        data_polis.is_ok = false;
        delete query_old;
        return false;
        //}
    }
    delete query_old;

    // проверим, есть ли бланк полиса в таблице бланков полисов и свободен ли он
    QString sql_blank;
    sql_blank= "select b.status, '('||b.status||') '||s.text "
               "  from blanks_pol b left join spr_blank_status s on(s.code=b.status) "
               " where b.pol_ser='" + data_polis.pol_ser + "' and "
               "       b.pol_num='" + data_polis.pol_num + "' and "
               "       b.status>0 ; ";
    QSqlQuery *query_blank = new QSqlQuery(db);
    bool res_blank = mySQL.exec(this, sql_blank, QString("проверка существования и занятости бланка с заданной серией и номером"), *query_blank, true, db, data_app);
    if ( !res_blank || query_blank->size()>0 ) {
        query_blank->next();
        QMessageBox::warning(this, "Бланк полиса уже используется!",
                             "Введённые серия и номер соответствуют действующему бланку полиса уже выданному наруки дастрахованному!\n"
                             "Статус бланка: " + query_blank->value(1).toString() + "\n\n"
                             "Исправьте серию/номер бланка и повторите операцию.");
        data_polis.is_ok = false;
        delete query_blank;
        return false;
    }
    delete query_blank;

    // проверим, есть ли в таблице бланков полисов ранее внесённый, но не используемый бланк
    QString sql_blank2;
    sql_blank2= "select id, status "
                "  from blanks_pol "
                " where pol_ser='" + data_polis.pol_ser + "' and "
                "       pol_num='" + data_polis.pol_num + "' and "
                "       status<=0 ; ";
    QSqlQuery *query_blank2 = new QSqlQuery(db);
    bool res_blank2 = mySQL.exec(this, sql_blank2, QString("проверка существования и занятости бланка с заданной серией и номером"), *query_blank2, true, db, data_app);

    if (query_blank2->next()) {
        // бланк уже есть - используем его повторно
        int id_blank2 = query_blank2->value(0).toInt();
        QString sql_reblank;
        sql_reblank= "update blanks_pol "
                     "   set status=4 "
                     " where id=" + QString::number(id_blank2) + " ; ";
        QSqlQuery *query_reblank = new QSqlQuery(db);

        QMessageBox::critical(this, "Повторное страхование",
                              "Этот человек уже был застрахован в нашей компании.\n"
                              "Cтарый бланк будет использован повторно.");

        bool res_reblank = mySQL.exec(this, sql_reblank, QString("Бланк уже есть и закрыт - откроем его"), *query_reblank, true, db, data_app);
        if (!res_reblank) {
            QMessageBox::critical(this, "Операция не выполнена!", "При попытке восстановить ранее закрытый бланк произошла ошибка - операция отменена.");
            delete query_reblank;
            delete query_blank2;
            return false;
        }
        delete query_reblank;

    } else {
        // добавим бланк в базу
        QString sql_add_blank =
              "insert into blanks_pol(pol_ser, pol_num, id_point, id_operator, date_add, status, enp, date_scan_enp, id_person, id_polis) "
              "values ('" + ui->line_pol_ser->text().trimmed() + "', "
                        + ui->line_pol_num->text().trimmed() + ", "
                        + QString::number(data_app.id_point) + ", "
                        + QString::number(data_app.id_operator) + ", "
                        + " '" + QDate::currentDate().toString("dd.MM.yyyy") + "', "
                        + " 4, "
                        + " '" + ui->line_enp->text().trimmed() + "', "
                        + " NULL, "
                        + " " + QString::number(data_pers.id) + ", "
                        + " NULL ) ; ";
        QSqlQuery *query_add_blank = new QSqlQuery(db);
        mySQL.exec(this, sql_add_blank, QString("Добавление чужого бланка полиса"), *query_add_blank, true, db, data_app);
        if (query_add_blank->size()==0) {
            QMessageBox::critical(this, "Ошибка при чужого бланка полиса!", "При добавлении чужого бланка полиса произошла неожиданная ошибка!\n\nОперация страхования отменена.");
            data_polis.is_ok = false;
            delete query_add_blank;
            return false;
        }
        delete query_add_blank;
    }
    delete query_blank;

    // добавим полис в базу
    QString sql;
    sql  =  "insert into polises (id_person, ocato, enp, ogrn_smo, pol_v, pol_type, pol_ser, pol_num, vs_num, date_begin, date_end, date_stop, in_erp, /*order_num, order_date,*/ id_old_polis, _id_last_point, _id_last_operator, _id_last_event, f_polis, date_get2hand, r_smo, r_polis) ";
    sql +=  " values (";
    sql +=  " "  + QString::number(data_polis.id_person) + ", ";
    sql +=  " '" + data_polis.ocato.trimmed() + "', ";
    sql +=  " '" + data_polis.enp.trimmed() + "', ";
    sql +=  " '" + data_polis.ogrn_smo.trimmed() + "', ";
    sql +=  " "  + QString::number(data_polis.pol_v) + ", ";
    sql +=  " '" + data_polis.pol_type.trimmed() + "', ";
    sql +=  (ui->ch_pol_sernum->isChecked()   ? (" '" + (QString("0000"+data_polis.pol_ser).right(4)) + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_sernum->isChecked()   ? (" '" + data_polis.pol_num + "'") : "NULL") + ", ";
    sql +=  (ui->ch_vs_num->isChecked()    ? (" '" + data_polis.vs_num  + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datbegin->isChecked() ? (" '" + data_polis.date_begin.toString("yyyy-MM-dd") + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datend->isChecked()   ? (" '" + data_polis.date_end.toString("yyyy-MM-dd")   + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datstop->isChecked()  ? (" '" + data_polis.date_stop.toString("yyyy-MM-dd")  + "'") : "NULL") + ", ";
    sql +=  " "  + QString::number(data_polis.in_erp) + ", ";
    //sql +=  " '" + data_polis.order_num.trimmed() + "', ";
    //sql +=  " '" + data_polis.order_date.toString("yyyy-MM-dd") + "', ";
    sql +=  (data_polis.id_old_polis<0 ? "NULL" : QString::number(data_polis.id_old_polis)) + ", ";
    sql +=  QString::number(data_app.id_point) + ", ";
    sql +=  QString::number(data_app.id_operator) + ", ";
    sql +=  "NULL, ";
    sql +=  QString::number(ui->combo_vizit_pan_form_polis->currentIndex()) + ", ";
    sql +=  " '" + QDate::currentDate().toString("yyyy-MM-dd") + "', ";
    sql +=  QString::number(data_vizit.r_smo) + ", ";
    sql +=  QString::number(data_vizit.r_polis) + " ) ";
    sql +=  " returning id ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString("П031, П032, П033 - Добавление чужого полиса ОМС"), *query, true, db, data_app);
    if (!res) {
        QMessageBox::critical(this, "Операция не выполнена!", "П031, П032, П033 - Добавление чужого полиса ОМС - Операция страхования отменена.");
        data_polis.is_ok = false;
        delete query;
        return false;
    } else {
        while (query->next()) {
            data_polis.id = query->value(0).toInt();
            data_event.is_ok = true;

            // добавим событие в базу
            QString sql = "insert into events (id_point, id_operator, id_polis, id_vizit, event_code, event_dt, event_time, comment, status, id_person, id_person_old, id_udl, id_drp) ";
            sql +=  " values (";
            sql +=  " "  + QString::number(data_app.id_point) + ", ";
            sql +=  " "  + QString::number(data_app.id_operator) + ", ";
            sql +=  " "  + QString::number(data_polis.id) + ", ";
            sql +=  " "  + QString::number(data_vizit.id) + ", ";
            sql +=  " '" + data_event.event_code.trimmed() + "', ";
            sql +=  " '" + data_event.event_dt.toString("yyyy-MM-dd") + "', ";
            sql +=  " '" + data_event.event_time.toString("hh:mm:ss") + "', ";
            sql +=  " '" + data_event.comment.trimmed() + "', 0,";
            sql +=  " "  + QString::number(data_pers.id) + ", ";
            sql +=  " "  + (data_pers.id_old>0 ? QString::number(data_pers.id_old) : "NULL") + ", ";
            sql +=  " "  + (data_pers.id_udl>0 ? QString::number(data_pers.id_udl) : "NULL") + ", ";
            sql +=  " "  + (data_pers.id_drp>0 ? QString::number(data_pers.id_drp) : "NULL") + ") ";
            sql +=  " returning id ; ";

            QSqlQuery *query_act = new QSqlQuery(db);
            bool res_act = mySQL.exec(this, sql, QString("П031, П032, П033 - Добавление нового события"), *query_act, true, db, data_app);
            if (!res_act || !query_act->next()) {
                QMessageBox::critical(this, "Операция не выполнена!", "П031, П032, П033 - Добавление нового события - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_act;
                return false;
            }

            // добавим _id_last_event в полис
            data_polis._id_last_event = query_act->value(0).toInt();
            delete query_act;
            QString sql_pa = "update polises "
                             "   set _id_last_event=" + QString::number(data_polis._id_last_event) + " "
                             " where id=" + QString::number(data_polis.id) + " ; ";

            QSqlQuery *query_pa = new QSqlQuery(db);
            bool res_pa = mySQL.exec(this, sql_pa, QString("П031, П032, П033 - Добавление id события в полис"), *query_pa, true, db, data_app);
            if (!res_pa) {
                QMessageBox::critical(this, "Операция не выполнена!", "П031, П032, П033 - Добавление id события в полис - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_pa;
                return false;
            }
            delete query_pa;

            // добавим id_polis в бланк полиса
            QString sql_pb = "update blanks_pol "
                             "   set id_polis=" + QString::number(data_polis.id) + " "
                             " where pol_ser='" + data_polis.pol_ser + "' "
                             "   and pol_num='" + data_polis.pol_num + "' ; ";

            QSqlQuery *query_pb = new QSqlQuery(db);
            bool res_pb = mySQL.exec(this, sql_pb, QString("Добавление id события в полис"), *query_pb, true, db, data_app);
            if (!res_pb) {
                QMessageBox::critical(this, "Операция не выполнена!", "П031, П032, П033 - Добавление id полиса в бланк - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_pb;
                return false;
            }
            delete query_pb;
        }
    }
    delete query;

    // запомним форму полиса в данных визита
    QString sql_polf = "update vizits "
                       "   set f_polis=" + QString::number(data_polis.f_polis) + " "
                       " where id=" + QString::number(data_vizit.id) + " ; ";
    QSqlQuery *query_polf = new QSqlQuery(db);
    bool res_polf = mySQL.exec(this, sql_polf, QString("запись формы полиса в визит"), *query_polf, true, db, data_app);
    if (!res_polf) {
        QMessageBox::critical(this, "Операция не выполнена!", "При записи формы выпуска полиса в визит произошла ошибка - операция отменена отменена.");
        delete query_polf;
        return false;
    }
    delete query_polf;

    // запомним статус персоны - 3
    QString sql_pers_stat = "update persons "
                            " set status=3, "
                            "     _date_insure_begin=COALESCE(_date_insure_begin, CURRENT_DATE), "
                            "     _date_insure_end=NULL "
                            " where id=" + QString::number(data_pers.id) + " ; ";
    QSqlQuery *query_pers_stat = new QSqlQuery(db);
    bool res_pers_stat = mySQL.exec(this, sql_pers_stat, QString("Правка статуса персоны (3)"), *query_pers_stat, true, db, data_app);
    if (!res_pers_stat) {
        QMessageBox::critical(this, "Операция не выполнена!", "При правке статуса персоны (3) произошла ошибка - операция отменена отменена.");
        delete query_pers_stat;
        return false;
    }
    delete query_pers_stat;

    data_polis.is_ok = true;
    return true;
}



// ---------------------------------
// --- П034, П035, П036 ------------
// ---------------------------------
bool polices_wnd::process_P034_P035_P036 (s_data_polis &data_polis, s_data_event &data_event) {
    // П034 - замена СМО с выдачей ВС - один раз в год по желанию лица
    // П035 - замена СМО с выдачей ВС - при смене места жительства
    // П036 - замена СМО с выдачей ВС - в связи с расторжением старого договора
    if ( ( data_event.event_code!="П034"
           && data_event.event_code!="П035"
           && data_event.event_code!="П036" )
         || data_polis.pol_v!=2 ) {
        QMessageBox::critical(this, "Не тот код события или тип полиса!",
                              "Обработчик события " + data_event.event_code + " получил данные события <" + data_event.event_code + ">\nТип полиса - "+QString::number(data_polis.pol_v)+"\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        return false;
    };

    data_polis.is_ok = true;

    // проверим, попадает ли номер ВС в открытый интервал
    QString sql_int;
    sql_int= "select v.id as id_vs_inteval_pt "
             " from vs_intervals_pt v "
             "      left join folders f on(v.id_folder=f.id) "
             " where v.id_point=" + QString::number(data_app.id_point) + " "
             //"   and ( (f.folder_name is NULL) or (CHAR_LENGTH('" + data_app.default_folder + "')<1) or (f.folder_name=('" + data_app.default_folder + "')) ) "
             "   and v.date_open<=CURRENT_DATE "
             "   and ( (v.date_close is NULL) or (v.date_close>CURRENT_DATE) ) "
             "   and " + QString::number(ui->line_vs_num->text().toInt()) + " between v.vs_from and v.vs_to ";
    if (data_app.use_folders)
        sql_int += "   and ( (f.folder_name is NULL) or (CHAR_LENGTH('" + data_app.default_folder + "')<1) or (f.folder_name=('" + data_app.default_folder + "')) ) ; ";

    QSqlQuery *query_int = new QSqlQuery(db);
    mySQL.exec(this, sql_int, QString("проверка номера ВС по открытым диапазонам"), *query_int, true, db, data_app);
    if (query_int->size()<1) {
        QMessageBox::warning(this, "Неверный номер ВС!", "Введённый номер ВС не входит ни в один из интервалов номеров ВС данного пункта выдачи!\n\nИсправьте номер ВС и повторите операцию.");
        data_polis.is_ok = false;
        delete query_int;
        return false;
    }
    delete query_int;

    /*// проверим, не испорчен ли бланк ВС с таким номером
    QString sql_tst;
    sql_tst = "select b.vs_num "
                " from  blanks_vs b "
                " where b.vs_num=" + ui->line_vs_num->text().trimmed() + " "
                "   and b.status=-1 ; ";
    QSqlQuery* query_tst = new QSqlQuery(db);
    mySQL.exec(this, sql_tst, QString("проверка, не испорчен ли бланк ВС с таким номером"), *query_tst, true, db, data_app);
    if (query_tst->size()>0) {
        QMessageBox::critical(this, "Бланк ВС испорчен!", "Бланк ВС с указанным номером помечен как испорченный!\n\nИсправьте номер ВС и повторите операцию.");
        data_polis.is_ok = false;
        delete query_tst;
        return false;
    }
    delete query_tst;*/

    // проверим, свободен ли бланк ВС с таким номером
    QString sql_tst2;
    sql_tst2 = "select b.vs_num, s.code, '('||s.code||') '||s.text "
               "  from blanks_vs b "
               "  left join spr_blank_status s on(s.code=b.status) "
               " where b.vs_num=" + ui->line_vs_num->text().trimmed() + " "
               "   and b.status<>0 ; ";
    QSqlQuery* query_tst2 = new QSqlQuery(db);
    mySQL.exec(this, sql_tst2, QString("проверка, свободен ли бланк ВС с таким номером"), *query_tst2, true, db, data_app);
    if (query_tst2->next()) {
        QString status_text = query_tst2->value(2).toString();
        QMessageBox::critical(this, "Номер ВС занят!",
                              "Бланк ВС с указанным номером уже был использован ране!\n" + status_text + "\n\nИсправьте номер ВС и повторите операцию.");
        data_polis.is_ok = false;
        delete query_tst2;
        return false;
    }
    delete query_tst2;

    /*// если бумажный полис единого образца (серия и номер бланка)
    if (data_event.event_code=="П060") {
        // проверим, свободны ли номер и серия полиса
        QString sql_enp;
        sql_enp = "select b.pol_ser, b.pol_num "
                  " from  blanks_pol b "
                  " where v.pol_ser=" + ui->line_pol_ser->text().trimmed() + " and v.pol_num=" + ui->line_pol_num->text().trimmed() + " ; ";
        QSqlQuery *query_enp = new QSqlQuery(db);
        mySQL.exec(this, sql_enp, QString("проверка, свободны ли серия/номер полиса"), *query_enp, true);
        if (query_enp->size()>0) {
            QMessageBox::critical(this, "Серия и номер полиса уже используются!", "Бланк полиса с указанными серией и номером уже используется!\n Исправьте серию и номер бленка полиса и повторите операцию.");
            data_polis.is_ok = false;
            delete query_enp;
            return false;
        }
        delete query_enp;
    }*/

    // проверка, можно ли применять это событие и надо ли закрывать старые полисы
    QString sql_old = "";
    sql_old += "select p.id, p.id_person, p.ocato, p.ogrn_smo, p.pol_v, f8.text as pol_v_text, p.pol_type, p.pol_ser, p.pol_num, p.vs_num, p.enp, p.date_begin, p.date_end, p.date_stop, p.in_erp, p.order_num, p.order_date, p._id_last_point, p._id_last_operator, p._id_last_event "
               "  from polises p "
               "  left join spr_f008 f8 on(f8.code=p.pol_v) "
               "  left join blanks_pol bp on(bp.id_polis=p.id) "
               "  left join blanks_vs bv on(bv.id_polis=p.id) "
               " where p.id_person=" + QString::number(data_pers.id) + " "
               "   and ((bp.id is not NULL and bp.status>0) or (bv.vs_num is not NULL and bv.status>0)) "
               "   and p.date_begin is not null  "
               "   and p.date_begin<=CURRENT_DATE  "
               "   and ( (p.date_stop is NULL) or (p.date_stop>CURRENT_DATE) ) ; ";

    QSqlQuery *query_old = new QSqlQuery(db);
    bool res_old = mySQL.exec(this, sql_old, QString("Новый полис - проверка, есть ли действующие старые полисы"), *query_old, true, db, data_app);
    // проверим старые полисы
    while (query_old->next()) {
        s_data_polis data_old_polis;
        data_old_polis.id = query_old->value(0).toInt();
        data_old_polis.id_person = query_old->value(1).toInt();
        data_old_polis.ocato = query_old->value(2).toString();
        data_old_polis.ogrn_smo = query_old->value(3).toString();
        data_old_polis.pol_v = query_old->value(4).toInt();
        data_old_polis.pol_v_text = query_old->value(5).toString();
        data_old_polis.pol_type = query_old->value(6).toString();
        data_old_polis.has_enp = !(query_old->value(10).isNull());
        data_old_polis.has_pol_sernum = !(query_old->value(8).isNull());
        data_old_polis.has_vs_num = !(query_old->value(9).isNull());
        data_old_polis.pol_ser = query_old->value(7).toString();
        data_old_polis.pol_num = query_old->value(8).toString();
        data_old_polis.vs_num = query_old->value(9).toString();
        data_old_polis.enp = query_old->value(10).toString();
        data_old_polis.has_date_begin = !(query_old->value(11).isNull());
        data_old_polis.has_date_end = !(query_old->value(12).isNull());
        data_old_polis.has_date_stop = !(query_old->value(13).isNull());
        data_old_polis.date_begin = query_old->value(11).toDate();
        data_old_polis.date_end = query_old->value(12).toDate();
        data_old_polis.date_stop = query_old->value(13).toDate();
        data_old_polis.in_erp = query_old->value(14).toInt();
        data_old_polis.order_num = query_old->value(15).toString();
        data_old_polis.order_date = query_old->value(16).toDate();
        data_old_polis._id_last_point = query_old->value(17).toInt();
        data_old_polis._id_last_operator = query_old->value(18).toInt();
        data_old_polis._id_last_event = query_old->value(19).toInt();

        // другого действующего полиса быть не должно
        if (data_old_polis.pol_v==2) {
            QMessageBox::warning(this,
                                 "Невозможно открыть ВС!",
                                 QString("Невозможно открыть новое временное свидетельство, поскольку у застрахованного уже есть другое действующее ВС:\n ") +
                                 "номер: " + QString::number(query_old->value(9).toInt()) + "\n" +
                                 "дата выдачи: " + query_old->value(11).toDate().toString("dd.MM.yyyy") + "\n" +
                                 "№ ВС: " + query_old->value(9).toString() + "\n" +
                                 "ЕНП: " + query_old->value(10).toString() + "\n\n" +
                                 "Надо дождаться активации этого ВС из ТФОМС и, при необходимости, выдать новое ВС с закрытием полученного полиса единого образца.");
            data_polis.is_ok = false;
            delete query_old;
            return false;
        }
        if ((data_old_polis.pol_v==1 || data_old_polis.pol_v==3)) {
            data_polis.id_old_polis = data_old_polis.id;
            QMessageBox::warning(this,
                                 "Полис уже есть",
                                 QString("У застрахованного уже есть действующий полис ОМС единого образца или полис ОМС старого образца нашей СМО: \n") +
                                 "тип: " + query_old->value(4).toString() + "\n" +
                                 "серия/номер: " + query_old->value(7).toString() + " № " + query_old->value(8).toString() + "\n" +
                                 "дата выдачи: " + query_old->value(11).toDate().toString("dd.MM.yyyy") + "\n" +
                                 "№ ВС: " + query_old->value(9).toString() + "\n" +
                                 "ЕНП: " + query_old->value(10).toString() + "\n\n" +
                                 "Событие П034 предполагает, что у застрахованного нет действующего полиса ОМС нашей СМО, а есть полис другой СМО, который надо заменить.\n"
                                 "Событие П035 предполагает, что у застрахованного нет действующего полиса ОМС нашей СМО и его надо застраховать в связи с переездом на нашу территорию. \n"
                                 "Событие П036 предполагает, что полис ОМС застрахованного был закрыт в связи с расторжением договора.\n"
                                 "\nВыберите другое событие.\nОперация отменена.");
            // отмена операции
            data_polis.is_ok = false;
            delete query_old;
            return false;
        }
    }
    delete query_old;

    // добавим бланк ВС
    /*QString sql_add_blank =
            "insert into blanks_vs(vs_num, id_point, id_operator, date_add, status, id_person, id_polis) "
            " values(" + ui->line_vs_num->text().trimmed() + ", "
                       + QString::number(data_app.id_point) + ", "
                       + QString::number(data_app.id_operator) + ", "
                       + "'" + QDate::currentDate().toString("dd.MM.yyyy") + "', "
                       + " 1, "
                       + " " + QString::number(data_pers.id) + ", "
                       + " NULL) "
                       + " returning vs_num ; ";*/
    QString sql_add_blank =
            "update blanks_vs "
            "   set id_point=" + QString::number(data_app.id_point) + ", "
            "       id_operator=" + QString::number(data_app.id_operator) + ", "
            //"       date_add='" + QDate::currentDate().toString("dd.MM.yyyy") + "', "
            "       status=1, "
            "       id_person=" + QString::number(data_pers.id) + ", "
            "       id_polis=NULL, "
            "       date_spent = coalesce(date_spent, CURRENT_DATE) "
            " where vs_num='" + ui->line_vs_num->text().trimmed() + "' ; ";
    QSqlQuery *query_add_blank = new QSqlQuery(db);
    bool res_add_blank = mySQL.exec(this, sql_add_blank, QString("Добавление бланка ВС"), *query_add_blank, true, db, data_app);
    if (!res_add_blank || query_add_blank->size()==0) {
        QMessageBox::critical(this, "Ошибка при добавлении бланка ВС!", "При добавлении бланка ВС произошла неожиданная ошибка!\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        delete query_add_blank;
        return false;
    }
    delete query_add_blank;

    // добавим полис в базу
    QString sql;
    sql  =  "insert into polises (id_person, ocato, enp, ogrn_smo, pol_v, pol_type, pol_ser, pol_num, vs_num, date_begin, date_end, date_stop, in_erp, /*order_num, order_date,*/ id_old_polis, _id_last_point, _id_last_operator, _id_last_event, f_polis, date_get2hand, r_smo, r_polis) ";
    sql +=  " values (";
    sql +=  " "  + QString::number(data_polis.id_person) + ", ";
    sql +=  " '" + data_polis.ocato.trimmed() + "', ";
    sql +=  " '" + data_polis.enp.trimmed() + "', ";
    sql +=  " '" + data_polis.ogrn_smo.trimmed() + "', ";
    sql +=  " "  + QString::number(data_polis.pol_v) + ", ";
    sql +=  " '" + data_polis.pol_type.trimmed() + "', ";
    sql +=  (ui->ch_pol_sernum->isChecked()   ? (" '" + data_polis.pol_ser + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_sernum->isChecked()   ? (" '" + data_polis.pol_num + "'") : "NULL") + ", ";
    sql +=  (ui->ch_vs_num->isChecked()    ? (" '" + data_polis.vs_num  + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datbegin->isChecked() ? (" '" + data_polis.date_begin.toString("yyyy-MM-dd") + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datend->isChecked()   ? (" '" + data_polis.date_end.toString("yyyy-MM-dd")   + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datstop->isChecked()  ? (" '" + data_polis.date_stop.toString("yyyy-MM-dd")  + "'") : "NULL") + ", ";
    sql +=  " "  + QString::number(data_polis.in_erp) + ", ";
    //sql +=  " '" + data_polis.order_num.trimmed() + "', ";
    //sql +=  " '" + data_polis.order_date.toString("yyyy-MM-dd") + "', ";
    sql +=  (data_polis.id_old_polis<0 ? "NULL" : QString::number(data_polis.id_old_polis)) + ", ";
    sql +=  QString::number(data_app.id_point) + ", ";
    sql +=  QString::number(data_app.id_operator) + ", ";
    sql +=  "NULL, ";
    sql +=  QString::number(ui->combo_vizit_pan_form_polis->currentIndex()) + ", ";
    sql +=  " '" + QDate::currentDate().toString("yyyy-MM-dd") + "', ";
    sql +=  QString::number(data_vizit.r_smo) + ", ";
    sql +=  QString::number(data_vizit.r_polis) + " ) ";
    sql +=  " returning id ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString(data_event.event_code + " - Добавление нового ВС"), *query, true, db, data_app);
    if (!res) {
        QMessageBox::critical(this, "Операция не выполнена!", data_event.event_code + " - При добавлении нового ВС произошла неожиданная ошибка.\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        delete query;
        return false;
    } else {
        while (query->next()) {
            data_polis.id = query->value(0).toInt();
            data_event.is_ok = true;

            // для старых полисов принудительно зададим дату закрытия не позже даты открытия нового полиса
            QString sql_ps = "update polises "
                             "   set date_stop='" + data_polis.date_begin.toString("yyyy-MM-dd") + "' "
                             " where id_person=" + QString::number(data_polis.id_person) + " "
                             "   and id<>" + QString::number(data_polis.id) + " "
                             "   and ( (date_stop is NULL) or (date_stop>'" + data_polis.date_begin.toString("yyyy-MM-dd") + "') ) ; ";

            QSqlQuery *query_ps = new QSqlQuery(db);
            bool res_ps = mySQL.exec(this, sql_ps, QString(data_event.event_code + " - Ограничение срока действия старых полисов"), *query_ps, true, db, data_app);
            if (!res_ps) {
                QMessageBox::critical(this, "Операция не выполнена!", data_event.event_code + " - Ограничение срока действия старых полисов - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_ps;
                return false;
            }
            delete query_ps;


            // добавим событие в базу
            QString sql = "insert into events (id_point, id_operator, id_polis, id_vizit, event_code, event_dt, event_time, comment, status, id_person, id_person_old, id_udl, id_drp) ";
            sql +=  " values (";
            sql +=  " "  + QString::number(data_app.id_point) + ", ";
            sql +=  " "  + QString::number(data_app.id_operator) + ", ";
            sql +=  " "  + QString::number(data_polis.id) + ", ";
            sql +=  " "  + QString::number(data_vizit.id) + ", ";
            sql +=  " '" + data_event.event_code.trimmed() + "', ";
            sql +=  " '" + data_event.event_dt.toString("yyyy-MM-dd") + "', ";
            sql +=  " '" + data_event.event_time.toString("hh:mm:ss") + "', ";
            sql +=  " '" + data_event.comment.trimmed() + "', 0,";
            sql +=  " "  + QString::number(data_pers.id) + ", ";
            sql +=  " "  + (data_pers.id_old>0 ? QString::number(data_pers.id_old) : "NULL") + ", ";
            sql +=  " "  + (data_pers.id_udl>0 ? QString::number(data_pers.id_udl) : "NULL") + ", ";
            sql +=  " "  + (data_pers.id_drp>0 ? QString::number(data_pers.id_drp) : "NULL") + ") ";
            sql +=  " returning id ; ";

            QSqlQuery *query_act = new QSqlQuery(db);
            bool res_act = mySQL.exec(this, sql, QString(data_event.event_code + " - Добавление нового события"), *query_act, true, db, data_app);
            if (!res_act || !query_act->next()) {
                QMessageBox::critical(this, "Операция не выполнена!", data_event.event_code + " - Добавление нового события - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_act;
                return false;
            }

            // добавим _id_last_event в полис
            data_polis._id_last_event = query_act->value(0).toInt();
            delete query_act;
            QString sql_pa = "update polises "
                             "   set _id_last_event=" + QString::number(data_polis._id_last_event) + " "
                             " where id=" + QString::number(data_polis.id) + " ; ";

            QSqlQuery *query_pa = new QSqlQuery(db);
            bool res_pa = mySQL.exec(this, sql_pa, QString(data_event.event_code + " - Добавление id события в полис"), *query_pa, true, db, data_app);
            if (!res_pa) {
                QMessageBox::critical(this, "Операция не выполнена!", data_event.event_code + " - Добавление id события в полис - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_pa;
                return false;
            }
            delete query_pa;

            // добавим id_polis в бланк ВС
            QString sql_pb = "update blanks_vs "
                             "   set id_polis=" + QString::number(data_polis.id) + " "
                             " where vs_num=" + data_polis.vs_num + " ; ";

            QSqlQuery *query_pb = new QSqlQuery(db);
            bool res_pb = mySQL.exec(this, sql_pb, QString("Добавление id события в полис"), *query_pb, true, db, data_app);
            if (!res_pb) {
                QMessageBox::critical(this, "Операция не выполнена!", data_event.event_code + " - Добавление id полиса в бланк - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_pb;
                return false;
            }
            delete query_pb;
        }
    }
    delete query;

    // запомним форму полиса в данных визита
    QString sql_polf = "update vizits "
                       " set f_polis=" + QString::number(data_polis.f_polis) + " "
                       " where id=" + QString::number(data_vizit.id) + " ; ";
    QSqlQuery *query_polf = new QSqlQuery(db);
    bool res_polf = mySQL.exec(this, sql_polf, QString("запись формы полиса в визит"), *query_polf, true, db, data_app);
    if (!res_polf) {
        QMessageBox::critical(this, "Операция не выполнена!", "При записи формы выпуска полиса в визит произошла ошибка - операция отменена отменена.");
        delete query_polf;
        return false;
    }
    delete query_polf;

    // запомним статус персоны - 1
    QString sql_pers_stat = "update persons "
                            "   set status=1, "
                            "       _date_insure_begin=COALESCE(_date_insure_begin, CURRENT_DATE), "
                            "       _date_insure_end=NULL "
                            " where id=" + QString::number(data_pers.id) + " ; ";
    QSqlQuery *query_pers_stat = new QSqlQuery(db);
    bool res_pers_stat = mySQL.exec(this, sql_pers_stat, QString("Правка статуса персоны (1)"), *query_pers_stat, true, db, data_app);
    if (!res_pers_stat) {
        QMessageBox::critical(this, "Операция не выполнена!", "При правке статуса персоны (1) произошла ошибка - операция отменена отменена.");
        delete query_pers_stat;
        return false;
    }
    delete query_pers_stat;

    data_polis.is_ok = true;
    return true;
}


/*
// ---------------------------------
// --- П034 ------------------------
// ---------------------------------
bool polices_wnd::process_P034 (s_data_polis &data_polis, s_data_event &data_event) {
    // П034 - замена СМО с выдачей ВС - один раз в год по желанию лица
    if (data_event.event_code!="П034" || data_polis.pol_v!=2) {
        QMessageBox::critical(this, "Не тот код события или тип полиса!", "Обработчик события П034 получил данные события <" + data_event.event_code + ">\nТип полиса - "+QString::number(data_polis.pol_v)+"\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        return false;
    };

    data_polis.is_ok = true;

    // проверим, попадает ли номер ВС в открытый интервал
    QString sql_int;
    sql_int= "select v.id as id_vs_inteval_pt "
             " from vs_intervals_pt v "
             "      left join folders f on(v.id_folder=f.id) "
             " where v.id_point=" + QString::number(data_app.id_point) + " "
             //"   and ( (f.folder_name is NULL) or (CHAR_LENGTH('" + data_app.default_folder + "')<1) or (f.folder_name=('" + data_app.default_folder + "')) ) "
             "   and v.date_open<=CURRENT_DATE "
             "   and ( (v.date_close is NULL) or (v.date_close>CURRENT_DATE) ) "
             "   and " + QString::number(ui->line_vs_num->text().toInt()) + " between v.vs_from and v.vs_to ";
    if (data_app.use_folders)
        sql_int += "   and ( (f.folder_name is NULL) or (CHAR_LENGTH('" + data_app.default_folder + "')<1) or (f.folder_name=('" + data_app.default_folder + "')) ) ; ";

    QSqlQuery *query_int = new QSqlQuery(db);
    mySQL.exec(this, sql_int, QString("проверка номера ВС по открытым диапазонам"), *query_int, true, db, data_app);
    if (query_int->size()<1) {
        QMessageBox::warning(this, "Неверный номер ВС!", "Введённый номер ВС не входит ни в один из интервалов номеров ВС данного пункта выдачи!\n\nИсправьте номер ВС и повторите операцию.");
        data_polis.is_ok = false;
        delete query_int;
        return false;
    }
    delete query_int;

    // проверим, свободен ли бланк ВС с таким номером
    QString sql_tst2;
    sql_tst2 = "select b.vs_num, s.code, '('||s.code||') '||s.text "
               "  from blanks_vs b "
               "  left join spr_blank_status s on(s.code=b.status) "
               " where b.vs_num=" + ui->line_vs_num->text().trimmed() + " "
               "   and b.status<>0 ; ";
    QSqlQuery* query_tst2 = new QSqlQuery(db);
    mySQL.exec(this, sql_tst2, QString("проверка, свободен ли бланк ВС с таким номером"), *query_tst2, true, db, data_app);
    if (query_tst2->next()) {
        QString status_text = query_tst2->value(2).toString();
        QMessageBox::critical(this, "Номер ВС занят!",
                              "Бланк ВС с указанным номером уже был использован ране!\n" + status_text + "\n\nИсправьте номер ВС и повторите операцию.");
        data_polis.is_ok = false;
        delete query_tst2;
        return false;
    }
    delete query_tst2;

    // проверка, можно ли применять это событие и надо ли закрывать старые полисы
    QString sql_old = "";
    sql_old += "select p.id, p.id_person, p.ocato, p.ogrn_smo, p.pol_v, f8.text as pol_v_text, p.pol_type, p.pol_ser, p.pol_num, p.vs_num, p.enp, p.date_begin, p.date_end, p.date_stop, p.in_erp, p.order_num, p.order_date, p._id_last_point, p._id_last_operator, p._id_last_event "
           " from polises p "
           "     left join spr_f008 f8 on(f8.code=p.pol_v) "
           "     left join blanks_pol bp on(bp.id_polis=p.id) "
           "     left join blanks_vs bv on(bv.id_polis=p.id) "
           " where p.id_person=" + QString::number(data_pers.id) + " "
           "   and ((bp.id is not NULL and bp.status>0) or (bv.vs_num is not NULL and bv.status>0)) "
           "   and p.date_begin is not null  "
           "   and p.date_begin<=CURRENT_DATE  "
           "   and ( (p.date_stop is NULL) or (p.date_stop>CURRENT_DATE) ) ; ";

    QSqlQuery *query_old = new QSqlQuery(db);
    bool res_old = mySQL.exec(this, sql_old, QString("Новый полис - проверка, есть ли действующие старые полисы"), *query_old, true, db, data_app);
    // проверим старые полисы
    while (query_old->next()) {
        s_data_polis data_old_polis;
        data_old_polis.id = query_old->value(0).toInt();
        data_old_polis.id_person = query_old->value(1).toInt();
        data_old_polis.ocato = query_old->value(2).toString();
        data_old_polis.ogrn_smo = query_old->value(3).toString();
        data_old_polis.pol_v = query_old->value(4).toInt();
        data_old_polis.pol_v_text = query_old->value(5).toString();
        data_old_polis.pol_type = query_old->value(6).toString();
        data_old_polis.has_enp = !(query_old->value(10).isNull());
        data_old_polis.has_pol_sernum = !(query_old->value(8).isNull());
        data_old_polis.has_vs_num = !(query_old->value(9).isNull());
        data_old_polis.pol_ser = query_old->value(7).toString();
        data_old_polis.pol_num = query_old->value(8).toString();
        data_old_polis.vs_num = query_old->value(9).toString();
        data_old_polis.enp = query_old->value(10).toString();
        data_old_polis.has_date_begin = !(query_old->value(11).isNull());
        data_old_polis.has_date_end = !(query_old->value(12).isNull());
        data_old_polis.has_date_stop = !(query_old->value(13).isNull());
        data_old_polis.date_begin = query_old->value(11).toDate();
        data_old_polis.date_end = query_old->value(12).toDate();
        data_old_polis.date_stop = query_old->value(13).toDate();
        data_old_polis.in_erp = query_old->value(14).toInt();
        data_old_polis.order_num = query_old->value(15).toString();
        data_old_polis.order_date = query_old->value(16).toDate();
        data_old_polis._id_last_point = query_old->value(17).toInt();
        data_old_polis._id_last_operator = query_old->value(18).toInt();
        data_old_polis._id_last_event = query_old->value(19).toInt();

        // другого действующего полиса быть не должно
        if (data_old_polis.pol_v==2) {
            QMessageBox::warning(this,
                                 "Невозможно открыть ВС!",
                                 QString("Невозможно открыть новое временное свидетельство, поскольку у застрахованного уже есть другое действующее ВС:\n ") +
                                 "номер: " + QString::number(query_old->value(9).toInt()) + "\n" +
                                 "дата выдачи: " + query_old->value(11).toDate().toString("dd.MM.yyyy") + "\n" +
                                      "№ ВС: " + query_old->value(9).toString() + "\n" +
                                 "ЕНП: " + query_old->value(10).toString() + "\n\n" +
                                 "Надо дождаться активации этого ВС из ТФОМС и, при необходимости, выдать новое ВС с закрытием полученного полиса единого образца.");
            data_polis.is_ok = false;
            delete query_old;
            return false;
        }
        if ((data_old_polis.pol_v==1 || data_old_polis.pol_v==3)) {
            data_polis.id_old_polis = data_old_polis.id;
            QMessageBox::warning(this,
                                 "Полис уже есть",
                                 QString("У застрахованного уже есть действующий полис ОМС единого образца или полис ОМС старого образца нашей СМО: \n") +
                                 "тип: " + query_old->value(4).toString() + "\n" +
                                 "серия/номер: " + query_old->value(7).toString() + " № " + query_old->value(8).toString() + "\n" +
                                 "дата выдачи: " + query_old->value(11).toDate().toString("dd.MM.yyyy") + "\n" +
                                      "№ ВС: " + query_old->value(9).toString() + "\n" +
                                 "ЕНП: " + query_old->value(10).toString() + "\n\n" +
                                 "Событие П034 предполагает, что у застрахованного нет действующего полиса ОМС нашей СМО, а есть полис другой СМО, который надо заменить. \n\nВыберите другое событие.\nОперация отменена.");
            // отмена операции
            data_polis.is_ok = false;
            delete query_old;
            return false;
        }
    }
    delete query_old;

    // добавим бланк ВС
    QString sql_add_blank =
            "update blanks_vs "
            "   set id_point=" + QString::number(data_app.id_point) + ", "
            "       id_operator=" + QString::number(data_app.id_operator) + ", "
            //"       date_add='" + QDate::currentDate().toString("dd.MM.yyyy") + "', "
            "       status=1, "
            "       id_person=" + QString::number(data_pers.id) + ", "
            "       id_polis=NULL, "
            "       date_spent = coalesce(date_spent, CURRENT_DATE) "
            " where vs_num='" + ui->line_vs_num->text().trimmed() + "' ; ";
    QSqlQuery *query_add_blank = new QSqlQuery(db);
    bool res_add_blank = mySQL.exec(this, sql_add_blank, QString("Добавление бланка ВС"), *query_add_blank, true, db, data_app);
    if (!res_add_blank || query_add_blank->size()==0) {
        QMessageBox::critical(this, "Ошибка при добавлении бланка ВС!", "При добавлении бланка ВС произошла неожиданная ошибка!\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        delete query_add_blank;
        return false;
    }
    delete query_add_blank;

    // добавим полис в базу
    QString sql;
    sql = "insert into polises (id_person, ocato, enp, ogrn_smo, pol_v, pol_type, pol_ser, pol_num, vs_num, date_begin, date_end, date_stop, in_erp, id_old_polis, _id_last_point, _id_last_operator, _id_last_event, f_polis, date_get2hand) ";
    sql +=  " values (";
    sql +=  " "  + QString::number(data_polis.id_person) + ", ";
    sql +=  " '" + data_polis.ocato.trimmed() + "', ";
    sql +=  " '" + data_polis.enp.trimmed() + "', ";
    sql +=  " '" + data_polis.ogrn_smo.trimmed() + "', ";
    sql +=  " "  + QString::number(data_polis.pol_v) + ", ";
    sql +=  " '" + data_polis.pol_type.trimmed() + "', ";
    sql +=  (ui->ch_pol_sernum->isChecked()   ? (" '" + data_polis.pol_ser + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_sernum->isChecked()   ? (" '" + data_polis.pol_num + "'") : "NULL") + ", ";
    sql +=  (ui->ch_vs_num->isChecked()    ? (" '" + data_polis.vs_num  + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datbegin->isChecked() ? (" '" + data_polis.date_begin.toString("yyyy-MM-dd") + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datend->isChecked()   ? (" '" + data_polis.date_end.toString("yyyy-MM-dd")   + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datstop->isChecked()  ? (" '" + data_polis.date_stop.toString("yyyy-MM-dd")  + "'") : "NULL") + ", ";
    sql +=  " "  + QString::number(data_polis.in_erp) + ", ";
    //sql +=  " '" + data_polis.order_num.trimmed() + "', ";
    //sql +=  " '" + data_polis.order_date.toString("yyyy-MM-dd") + "', ";
    sql +=  (data_polis.id_old_polis<0 ? "NULL" : QString::number(data_polis.id_old_polis)) + ", ";
    sql +=  QString::number(data_app.id_point) + ", ";
    sql +=  QString::number(data_app.id_operator) + ", ";
    sql +=  "NULL, ";
    sql +=  QString::number(ui->combo_vizit_pan_form_polis->currentIndex()) + ", ";
    sql +=  " '" + QDate::currentDate().toString("yyyy-MM-dd") + "' ) ";
    sql +=  " returning id ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString("П034 - Добавление нового ВС"), *query, true, db, data_app);
    if (!res) {
        QMessageBox::critical(this, "Операция не выполнена!", "П034 - При добавлении нового ВС произошла неожиданная ошибка.\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        delete query;
        return false;
    } else {
        while (query->next()) {
            data_polis.id = query->value(0).toInt();
            data_event.is_ok = true;

            // добавим событие в базу
            QString sql = "insert into events (id_point, id_operator, id_polis, id_vizit, event_code, event_dt, event_time, comment, status, id_person, id_person_old, id_udl, id_drp) ";
            sql +=  " values (";
            sql +=  " "  + QString::number(data_app.id_point) + ", ";
            sql +=  " "  + QString::number(data_app.id_operator) + ", ";
            sql +=  " "  + QString::number(data_polis.id) + ", ";
            sql +=  " "  + QString::number(data_vizit.id) + ", ";
            sql +=  " '" + data_event.event_code.trimmed() + "', ";
            sql +=  " '" + data_event.event_dt.toString("yyyy-MM-dd") + "', ";
            sql +=  " '" + data_event.event_time.toString("hh:mm:ss") + "', ";
            sql +=  " '" + data_event.comment.trimmed() + "', 0,";
            sql +=  " "  + QString::number(data_pers.id) + ", ";
            sql +=  " "  + (data_pers.id_old>0 ? QString::number(data_pers.id_old) : "NULL") + ", ";
            sql +=  " "  + (data_pers.id_udl>0 ? QString::number(data_pers.id_udl) : "NULL") + ", ";
            sql +=  " "  + (data_pers.id_drp>0 ? QString::number(data_pers.id_drp) : "NULL") + ") ";
            sql +=  " returning id ; ";

            QSqlQuery *query_act = new QSqlQuery(db);
            bool res_act = mySQL.exec(this, sql, QString("П034 - Добавление нового события"), *query_act, true, db, data_app);
            if (!res_act || !query_act->next()) {
                QMessageBox::critical(this, "Операция не выполнена!", "П034 - Добавление нового события - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_act;
                return false;
            }

            // добавим _id_last_event в полис
            data_polis._id_last_event = query_act->value(0).toInt();
            QString sql_pa = "update polises "
                             "   set _id_last_event=" + QString::number(data_polis._id_last_event) + " "
                             " where id=" + QString::number(data_polis.id) + " ; ";

            QSqlQuery *query_pa = new QSqlQuery(db);
            bool res_pa = mySQL.exec(this, sql_pa, QString("П034 - Добавление id события в полис"), *query_pa, true, db, data_app);
            if (!res_pa) {
                QMessageBox::critical(this, "Операция не выполнена!", "П034 - Добавление id события в полис - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_pa;
                return false;
            }
            delete query_pa;

            // добавим id_polis в бланк ВС
            QString sql_pb = "update blanks_vs "
                             "   set id_polis=" + QString::number(data_polis.id) + " "
                             " where vs_num=" + data_polis.vs_num + " ; ";

            QSqlQuery *query_pb = new QSqlQuery(db);
            bool res_pb = mySQL.exec(this, sql_pb, QString("Добавление id события в полис"), *query_pb, true, db, data_app);
            if (!res_pb) {
                QMessageBox::critical(this, "Операция не выполнена!", "П034 - Добавление id полиса в бланк - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_pb;
                return false;
            }
            delete query_pb;
        }
    }
    delete query;

    // запомним форму полиса в данных визита
    QString sql_polf = "update vizits "
                       " set f_polis=" + QString::number(data_polis.f_polis) + " "
                       " where id=" + QString::number(data_vizit.id) + " ; ";
    QSqlQuery *query_polf = new QSqlQuery(db);
    bool res_polf = mySQL.exec(this, sql_polf, QString("запись формы полиса в визит"), *query_polf, true, db, data_app);
    if (!res_polf) {
        QMessageBox::critical(this, "Операция не выполнена!", "При записи формы выпуска полиса в визит произошла ошибка - операция отменена отменена.");
        delete query_polf;
        return false;
    }

    // запомним статус персоны - 1
    QString sql_pers_stat = "update persons "
                            " set status=1, "
                            "     _date_insure_begin=COALESCE(_date_insure_begin, CURRENT_DATE), "
                            "     _date_insure_end=NULL "
                            " where id=" + QString::number(data_pers.id) + " ; ";
    QSqlQuery *query_pers_stat = new QSqlQuery(db);
    bool res_pers_stat = mySQL.exec(this, sql_pers_stat, QString("Правка статуса персоны (1)"), *query_pers_stat, true, db, data_app);
    if (!res_pers_stat) {
        QMessageBox::critical(this, "Операция не выполнена!", "При правке статуса персоны (1) произошла ошибка - операция отменена отменена.");
        delete query_pers_stat;
        return false;
    }

    data_polis.is_ok = true;
    return true;
}



// ---------------------------------
// --- П035 ------------------------
// ---------------------------------
bool polices_wnd::process_P035 (s_data_polis &data_polis, s_data_event &data_event) {
    // П035 - замена СМО с выдачей ВС - при смене места жительства
    if (data_event.event_code!="П035" || data_polis.pol_v!=2) {
        QMessageBox::critical(this, "Не тот код события или тип полиса!", "Обработчик события П035 получил данные события <" + data_event.event_code + ">\nТип полиса - "+QString::number(data_polis.pol_v)+"\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        return false;
    };

    data_polis.is_ok = true;

    // проверим, попадает ли номер ВС в открытый интервал
    QString sql_int;
    sql_int= "select v.id as id_vs_inteval_pt "
             " from vs_intervals_pt v "
             "      left join folders f on(v.id_folder=f.id) "
             " where v.id_point=" + QString::number(data_app.id_point) + " "
             //"   and ( (f.folder_name is NULL) or (CHAR_LENGTH('" + data_app.default_folder + "')<1) or (f.folder_name=('" + data_app.default_folder + "')) ) "
             "   and v.date_open<=CURRENT_DATE "
             "   and ( (v.date_close is NULL) or (v.date_close>CURRENT_DATE) ) "
             "   and " + QString::number(ui->line_vs_num->text().toInt()) + " between v.vs_from and v.vs_to ";
    if (data_app.use_folders)
        sql_int += "   and ( (f.folder_name is NULL) or (CHAR_LENGTH('" + data_app.default_folder + "')<1) or (f.folder_name=('" + data_app.default_folder + "')) ) ; ";

    QSqlQuery *query_int = new QSqlQuery(db);
    mySQL.exec(this, sql_int, QString("проверка номера ВС по открытым диапазонам"), *query_int, true, db, data_app);
    if (query_int->size()<1) {
        QMessageBox::warning(this, "Неверный номер ВС!", "Введённый номер ВС не входит ни в один из интервалов номеров ВС данного пункта выдачи!\n\nИсправьте номер ВС и повторите операцию.");
        data_polis.is_ok = false;
        delete query_int;
        return false;
    }
    delete query_int;

    // проверим, свободен ли бланк ВС с таким номером
    QString sql_tst2;
    sql_tst2 = "select b.vs_num, s.code, '('||s.code||') '||s.text "
               "  from blanks_vs b "
               "  left join spr_blank_status s on(s.code=b.status) "
               " where b.vs_num=" + ui->line_vs_num->text().trimmed() + " "
               "   and b.status<>0 ; ";
    QSqlQuery* query_tst2 = new QSqlQuery(db);
    mySQL.exec(this, sql_tst2, QString("проверка, свободен ли бланк ВС с таким номером"), *query_tst2, true, db, data_app);
    if (query_tst2->next()) {
        QString status_text = query_tst2->value(2).toString();
        QMessageBox::critical(this, "Номер ВС занят!",
                              "Бланк ВС с указанным номером уже был использован ране!\n" + status_text + "\n\nИсправьте номер ВС и повторите операцию.");
        data_polis.is_ok = false;
        delete query_tst2;
        return false;
    }
    delete query_tst2;

    // проверка, можно ли применять это событие и надо ли закрывать старые полисы
    QString sql_old = "";
    sql_old += "select p.id, p.id_person, p.ocato, p.ogrn_smo, p.pol_v, f8.text as pol_v_text, p.pol_type, p.pol_ser, p.pol_num, p.vs_num, p.enp, p.date_begin, p.date_end, p.date_stop, p.in_erp, p.order_num, p.order_date, p._id_last_point, p._id_last_operator, p._id_last_event "
           " from polises p "
           "     left join spr_f008 f8 on(f8.code=p.pol_v) "
           "     left join blanks_pol bp on(bp.id_polis=p.id) "
           "     left join blanks_vs bv on(bv.id_polis=p.id) "
           " where p.id_person=" + QString::number(data_pers.id) + " "
           "   and ((bp.id is not NULL and bp.status>0) or (bv.vs_num is not NULL and bv.status>0)) "
           "   and p.date_begin is not null  "
           "   and p.date_begin<=CURRENT_DATE  "
           "   and ( (p.date_stop is NULL) or (p.date_stop>CURRENT_DATE) ) ; ";

    QSqlQuery *query_old = new QSqlQuery(db);
    bool res_old = mySQL.exec(this, sql_old, QString("Новый полис - проверка, есть ли действующие старые полисы"), *query_old, true, db, data_app);
    // проверим старые полисы
    while (query_old->next()) {
        s_data_polis data_old_polis;
        data_old_polis.id = query_old->value(0).toInt();
        data_old_polis.id_person = query_old->value(1).toInt();
        data_old_polis.ocato = query_old->value(2).toString();
        data_old_polis.ogrn_smo = query_old->value(3).toString();
        data_old_polis.pol_v = query_old->value(4).toInt();
        data_old_polis.pol_v_text = query_old->value(5).toString();
        data_old_polis.pol_type = query_old->value(6).toString();
        data_old_polis.has_enp = !(query_old->value(10).isNull());
        data_old_polis.has_pol_sernum = !(query_old->value(8).isNull());
        data_old_polis.has_vs_num = !(query_old->value(9).isNull());
        data_old_polis.pol_ser = query_old->value(7).toString();
        data_old_polis.pol_num = query_old->value(8).toString();
        data_old_polis.vs_num = query_old->value(9).toString();
        data_old_polis.enp = query_old->value(10).toString();
        data_old_polis.has_date_begin = !(query_old->value(11).isNull());
        data_old_polis.has_date_end = !(query_old->value(12).isNull());
        data_old_polis.has_date_stop = !(query_old->value(13).isNull());
        data_old_polis.date_begin = query_old->value(11).toDate();
        data_old_polis.date_end = query_old->value(12).toDate();
        data_old_polis.date_stop = query_old->value(13).toDate();
        data_old_polis.in_erp = query_old->value(14).toInt();
        data_old_polis.order_num = query_old->value(15).toString();
        data_old_polis.order_date = query_old->value(16).toDate();
        data_old_polis._id_last_point = query_old->value(17).toInt();
        data_old_polis._id_last_operator = query_old->value(18).toInt();
        data_old_polis._id_last_event = query_old->value(19).toInt();

        // другого действующего полиса быть не должно
        if (data_old_polis.pol_v==2) {
            QMessageBox::warning(this,
                                 "Невозможно открыть ВС!",
                                 QString("Невозможно открыть новое временное свидетельство, поскольку у застрахованного уже есть другое действующее ВС:\n ") +
                                 "номер: " + QString::number(query_old->value(9).toInt()) + "\n" +
                                 "дата выдачи: " + query_old->value(11).toDate().toString("dd.MM.yyyy") + "\n" +
                                      "№ ВС: " + query_old->value(9).toString() + "\n" +
                                 "ЕНП: " + query_old->value(10).toString() + "\n\n" +
                                 "Надо дождаться активации этого ВС из ТФОМС и, при необходимости, выдать новое ВС с закрытием полученного полиса единого образца.");
            data_polis.is_ok = false;
            delete query_old;
            return false;
        }
        if ((data_old_polis.pol_v==1 || data_old_polis.pol_v==3)) {
            data_polis.id_old_polis = data_old_polis.id;
            QMessageBox::warning(this,
                                 "Полис уже есть",
                                 QString("У застрахованного уже есть действующий полис ОМС единого образца или полис ОМС старого образца нашей СМО: \n") +
                                 "тип: " + query_old->value(4).toString() + "\n" +
                                 "серия/номер: " + query_old->value(7).toString() + " № " + query_old->value(8).toString() + "\n" +
                                 "дата выдачи: " + query_old->value(11).toDate().toString("dd.MM.yyyy") + "\n" +
                                      "№ ВС: " + query_old->value(9).toString() + "\n" +
                                 "ЕНП: " + query_old->value(10).toString() + "\n\n" +
                                 "Событие П035 предполагает, что у застрахованного нет действующего полиса ОМС нашей СМО и его надо застраховать в связи с переездом на нашу территорию. \n\nВыберите другое событие.\nОперация отменена.");
            // отмена операции
            data_polis.is_ok = false;
            delete query_old;
            return false;
        }
    }
    delete query_old;

    // добавим бланк ВС
    QString sql_add_blank =
            "update blanks_vs "
            "   set id_point=" + QString::number(data_app.id_point) + ", "
            "       id_operator=" + QString::number(data_app.id_operator) + ", "
            //"       date_add='" + QDate::currentDate().toString("dd.MM.yyyy") + "', "
            "       status=1, "
            "       id_person=" + QString::number(data_pers.id) + ", "
            "       id_polis=NULL, "
            "       date_spent = coalesce(date_spent, CURRENT_DATE) "
            " where vs_num='" + ui->line_vs_num->text().trimmed() + "' ; ";
    QSqlQuery *query_add_blank = new QSqlQuery(db);
    mySQL.exec(this, sql_add_blank, QString("Добавление бланка ВС"), *query_add_blank, true, db, data_app);
    if (query_add_blank->size()==0) {
        QMessageBox::critical(this, "Ошибка при добавлении бланка ВС!", "При добавлении бланка ВС произошла неожиданная ошибка!\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        delete query_add_blank;
        return false;
    }
    delete query_add_blank;

    // добавим полис в базу
    QString sql;
    sql = "insert into polises (id_person, ocato, enp, ogrn_smo, pol_v, pol_type, pol_ser, pol_num, vs_num, date_begin, date_end, date_stop, in_erp, id_old_polis, _id_last_point, _id_last_operator, _id_last_event, f_polis, date_get2hand) ";
    sql +=  " values (";
    sql +=  " "  + QString::number(data_polis.id_person) + ", ";
    sql +=  " '" + data_polis.ocato.trimmed() + "', ";
    sql +=  " '" + data_polis.enp.trimmed() + "', ";
    sql +=  " '" + data_polis.ogrn_smo.trimmed() + "', ";
    sql +=  " "  + QString::number(data_polis.pol_v) + ", ";
    sql +=  " '" + data_polis.pol_type.trimmed() + "', ";
    sql +=  (ui->ch_pol_sernum->isChecked()   ? (" '" + data_polis.pol_ser + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_sernum->isChecked()   ? (" '" + data_polis.pol_num + "'") : "NULL") + ", ";
    sql +=  (ui->ch_vs_num->isChecked()    ? (" '" + data_polis.vs_num  + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datbegin->isChecked() ? (" '" + data_polis.date_begin.toString("yyyy-MM-dd") + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datend->isChecked()   ? (" '" + data_polis.date_end.toString("yyyy-MM-dd")   + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datstop->isChecked()  ? (" '" + data_polis.date_stop.toString("yyyy-MM-dd")  + "'") : "NULL") + ", ";
    sql +=  " "  + QString::number(data_polis.in_erp) + ", ";
    //sql +=  " '" + data_polis.order_num.trimmed() + "', ";
    //sql +=  " '" + data_polis.order_date.toString("yyyy-MM-dd") + "', ";
    sql +=  (data_polis.id_old_polis<0 ? "NULL" : QString::number(data_polis.id_old_polis)) + ", ";
    sql +=  QString::number(data_app.id_point) + ", ";
    sql +=  QString::number(data_app.id_operator) + ", ";
    sql +=  "NULL, ";
    sql +=  QString::number(ui->combo_vizit_pan_form_polis->currentIndex()) + ", ";
    sql +=  " '" + QDate::currentDate().toString("yyyy-MM-dd") + "' ) ";
    sql +=  " returning id ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString("П035 - Добавление нового ВС"), *query, true, db, data_app);
    if (!res) {
        QMessageBox::critical(this, "Операция не выполнена!", "П035 - При добавлении нового ВС произошла неожиданная ошибка.\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        delete query;
        return false;
    } else {
        while (query->next()) {
            data_polis.id = query->value(0).toInt();
            data_event.is_ok = true;

            // добавим событие в базу
            QString sql = "insert into events (id_point, id_operator, id_polis, id_vizit, event_code, event_dt, event_time, comment, status, id_person, id_person_old, id_udl, id_drp) ";
            sql +=  " values (";
            sql +=  " "  + QString::number(data_app.id_point) + ", ";
            sql +=  " "  + QString::number(data_app.id_operator) + ", ";
            sql +=  " "  + QString::number(data_polis.id) + ", ";
            sql +=  " "  + QString::number(data_vizit.id) + ", ";
            sql +=  " '" + data_event.event_code.trimmed() + "', ";
            sql +=  " '" + data_event.event_dt.toString("yyyy-MM-dd") + "', ";
            sql +=  " '" + data_event.event_time.toString("hh:mm:ss") + "', ";
            sql +=  " '" + data_event.comment.trimmed() + "', 0,";
            sql +=  " "  + QString::number(data_pers.id) + ", ";
            sql +=  " "  + (data_pers.id_old>0 ? QString::number(data_pers.id_old) : "NULL") + ", ";
            sql +=  " "  + (data_pers.id_udl>0 ? QString::number(data_pers.id_udl) : "NULL") + ", ";
            sql +=  " "  + (data_pers.id_drp>0 ? QString::number(data_pers.id_drp) : "NULL") + ") ";
            sql +=  " returning id ; ";

            QSqlQuery *query_act = new QSqlQuery(db);
            bool res_act = mySQL.exec(this, sql, QString("П035 - Добавление нового события"), *query_act, true, db, data_app);
            if (!res_act || !query_act->next()) {
                QMessageBox::critical(this, "Операция не выполнена!", "П035 - Добавление нового события - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_act;
                return false;
            }

            // добавим _id_last_event в полис
            data_polis._id_last_event = query_act->value(0).toInt();
            QString sql_pa = "update polises "
                             "   set _id_last_event=" + QString::number(data_polis._id_last_event) + " "
                             " where id=" + QString::number(data_polis.id) + " ; ";

            QSqlQuery *query_pa = new QSqlQuery(db);
            bool res_pa = mySQL.exec(this, sql_pa, QString("П035 - Добавление id события в полис"), *query_pa, true, db, data_app);
            if (!res_pa) {
                QMessageBox::critical(this, "Операция не выполнена!", "П035 - Добавление id события в полис - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_pa;
                return false;
            }
            delete query_pa;

            // добавим id_polis в бланк ВС
            QString sql_pb = "update blanks_vs "
                             "   set id_polis=" + QString::number(data_polis.id) + " "
                             " where vs_num=" + data_polis.vs_num + " ; ";

            QSqlQuery *query_pb = new QSqlQuery(db);
            bool res_pb = mySQL.exec(this, sql_pb, QString("Добавление id события в полис"), *query_pb, true, db, data_app);
            if (!res_pb) {
                QMessageBox::critical(this, "Операция не выполнена!", "П010 - Добавление id полиса в бланк - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_pb;
                return false;
            }
            delete query_pb;
        }
    }
    delete query;

    // запомним форму полиса в данных визита
    QString sql_polf = "update vizits "
                       " set f_polis=" + QString::number(data_polis.f_polis) + " "
                       " where id=" + QString::number(data_vizit.id) + " ; ";
    QSqlQuery *query_polf = new QSqlQuery(db);
    bool res_polf = mySQL.exec(this, sql_polf, QString("запись формы полиса в визит"), *query_polf, true, db, data_app);
    if (!res_polf) {
        QMessageBox::critical(this, "Операция не выполнена!", "При записи формы выпуска полиса в визит произошла ошибка - операция отменена отменена.");
        delete query_polf;
        return false;
    }

    // запомним статус персоны - 1
    QString sql_pers_stat = "update persons "
                            " set status=1, "
                            "     _date_insure_begin=COALESCE(_date_insure_begin, CURRENT_DATE), "
                            "     _date_insure_end=NULL "
                            " where id=" + QString::number(data_pers.id) + " ; ";
    QSqlQuery *query_pers_stat = new QSqlQuery(db);
    bool res_pers_stat = mySQL.exec(this, sql_pers_stat, QString("Правка статуса персоны (1)"), *query_pers_stat, true, db, data_app);
    if (!res_pers_stat) {
        QMessageBox::critical(this, "Операция не выполнена!", "При правке статуса персоны (1) произошла ошибка - операция отменена отменена.");
        delete query_pers_stat;
        return false;
    }

    data_polis.is_ok = true;
    return true;
}



// ---------------------------------
// --- П036 ------------------------
// ---------------------------------
bool polices_wnd::process_P036 (s_data_polis &data_polis, s_data_event &data_event) {
    // П036 - замена СМО с выдачей ВС - в связи с расторжением старого договора
    if (data_event.event_code!="П036" || data_polis.pol_v!=2) {
        QMessageBox::critical(this, "Не тот код события или тип полиса!", "Обработчик события П036 получил данные события <" + data_event.event_code + ">\nТип полиса - "+QString::number(data_polis.pol_v)+"\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        return false;
    };

    data_polis.is_ok = true;

    // проверим, попадает ли номер ВС в открытый интервал
    QString sql_int;
    sql_int= "select v.id as id_vs_inteval_pt "
             " from vs_intervals_pt v "
             "      left join folders f on(v.id_folder=f.id) "
             " where v.id_point=" + QString::number(data_app.id_point) + " "
             //"   and ( (f.folder_name is NULL) or (CHAR_LENGTH('" + data_app.default_folder + "')<1) or (f.folder_name=('" + data_app.default_folder + "')) ) "
             "   and v.date_open<=CURRENT_DATE "
             "   and ( (v.date_close is NULL) or (v.date_close>CURRENT_DATE) ) "
             "   and " + QString::number(ui->line_vs_num->text().toInt()) + " between v.vs_from and v.vs_to ";
    if (data_app.use_folders)
        sql_int += "   and ( (f.folder_name is NULL) or (CHAR_LENGTH('" + data_app.default_folder + "')<1) or (f.folder_name=('" + data_app.default_folder + "')) ) ; ";

    QSqlQuery *query_int = new QSqlQuery(db);
    mySQL.exec(this, sql_int, QString("проверка номера ВС по открытым диапазонам"), *query_int, true, db, data_app);
    if (query_int->size()<1) {
        QMessageBox::warning(this, "Неверный номер ВС!", "Введённый номер ВС не входит ни в один из интервалов номеров ВС данного пункта выдачи!\n\nИсправьте номер ВС и повторите операцию.");
        data_polis.is_ok = false;
        delete query_int;
        return false;
    }
    delete query_int;

    // проверим, свободен ли бланк ВС с таким номером
    QString sql_tst2;
    sql_tst2 = "select b.vs_num, s.code, '('||s.code||') '||s.text "
               "  from blanks_vs b "
               "  left join spr_blank_status s on(s.code=b.status) "
               " where b.vs_num=" + ui->line_vs_num->text().trimmed() + " "
               "   and b.status<>0 ; ";
    QSqlQuery* query_tst2 = new QSqlQuery(db);
    mySQL.exec(this, sql_tst2, QString("проверка, свободен ли бланк ВС с таким номером"), *query_tst2, true, db, data_app);
    if (query_tst2->next()) {
        QString status_text = query_tst2->value(2).toString();
        QMessageBox::critical(this, "Номер ВС занят!",
                              "Бланк ВС с указанным номером уже был использован ране!\n" + status_text + "\n\nИсправьте номер ВС и повторите операцию.");
        data_polis.is_ok = false;
        delete query_tst2;
        return false;
    }
    delete query_tst2;

    // проверка, можно ли применять это событие и надо ли закрывать старые полисы
    QString sql_old = "";
    sql_old += "select p.id, p.id_person, p.ocato, p.ogrn_smo, p.pol_v, f8.text as pol_v_text, p.pol_type, p.pol_ser, p.pol_num, p.vs_num, p.enp, p.date_begin, p.date_end, p.date_stop, p.in_erp, p.order_num, p.order_date, p._id_last_point, p._id_last_operator, p._id_last_event "
           " from polises p "
           "     left join spr_f008 f8 on(f8.code=p.pol_v) "
           "     left join blanks_pol bp on(bp.id_polis=p.id) "
           "     left join blanks_vs bv on(bv.id_polis=p.id) "
           " where p.id_person=" + QString::number(data_pers.id) + " "
           "   and ((bp.id is not NULL and bp.status>0) or (bv.vs_num is not NULL and bv.status>0)) "
           "   and p.date_begin is not null  "
           "   and p.date_begin<=CURRENT_DATE  "
           "   and ( (p.date_stop is NULL) or (p.date_stop>CURRENT_DATE) ) ; ";

    QSqlQuery *query_old = new QSqlQuery(db);
    bool res_old = mySQL.exec(this, sql_old, QString("Новый полис - проверка, есть ли действующие старые полисы"), *query_old, true, db, data_app);
    // проверим старые полисы
    while (query_old->next()) {
        s_data_polis data_old_polis;
        data_old_polis.id = query_old->value(0).toInt();
        data_old_polis.id_person = query_old->value(1).toInt();
        data_old_polis.ocato = query_old->value(2).toString();
        data_old_polis.ogrn_smo = query_old->value(3).toString();
        data_old_polis.pol_v = query_old->value(4).toInt();
        data_old_polis.pol_v_text = query_old->value(5).toString();
        data_old_polis.pol_type = query_old->value(6).toString();
        data_old_polis.has_enp = !(query_old->value(10).isNull());
        data_old_polis.has_pol_sernum = !(query_old->value(8).isNull());
        data_old_polis.has_vs_num = !(query_old->value(9).isNull());
        data_old_polis.pol_ser = query_old->value(7).toString();
        data_old_polis.pol_num = query_old->value(8).toString();
        data_old_polis.vs_num = query_old->value(9).toString();
        data_old_polis.enp = query_old->value(10).toString();
        data_old_polis.has_date_begin = !(query_old->value(11).isNull());
        data_old_polis.has_date_end = !(query_old->value(12).isNull());
        data_old_polis.has_date_stop = !(query_old->value(13).isNull());
        data_old_polis.date_begin = query_old->value(11).toDate();
        data_old_polis.date_end = query_old->value(12).toDate();
        data_old_polis.date_stop = query_old->value(13).toDate();
        data_old_polis.in_erp = query_old->value(14).toInt();
        data_old_polis.order_num = query_old->value(15).toString();
        data_old_polis.order_date = query_old->value(16).toDate();
        data_old_polis._id_last_point = query_old->value(17).toInt();
        data_old_polis._id_last_operator = query_old->value(18).toInt();
        data_old_polis._id_last_event = query_old->value(19).toInt();

        // другого действующего полиса быть не должно
        if (data_old_polis.pol_v==2) {
            QMessageBox::warning(this,
                                 "Невозможно открыть ВС!",
                                 QString("Невозможно открыть новое временное свидетельство, поскольку у застрахованного уже есть другое действующее ВС:\n ") +
                                 "номер: " + QString::number(query_old->value(9).toInt()) + "\n" +
                                 "дата выдачи: " + query_old->value(11).toDate().toString("dd.MM.yyyy") + "\n" +
                                      "№ ВС: " + query_old->value(9).toString() + "\n" +
                                 "ЕНП: " + query_old->value(10).toString() + "\n\n" +
                                 "Надо дождаться активации этого ВС из ТФОМС и, при необходимости, выдать новое ВС с закрытием полученного полиса единого образца.");
            data_polis.is_ok = false;
            delete query_old;
            return false;
        }
        if ((data_old_polis.pol_v==1 || data_old_polis.pol_v==3)) {
            data_polis.id_old_polis = data_old_polis.id;
            QMessageBox::warning(this,
                                 "Полис уже есть",
                                 QString("У застрахованного уже есть действующий полис ОМС единого образца или полис ОМС старого образца нашей СМО: \n") +
                                 "тип: " + query_old->value(4).toString() + "\n" +
                                 "серия/номер: " + query_old->value(7).toString() + " № " + query_old->value(8).toString() + "\n" +
                                 "дата выдачи: " + query_old->value(11).toDate().toString("dd.MM.yyyy") + "\n" +
                                      "№ ВС: " + query_old->value(9).toString() + "\n" +
                                 "ЕНП: " + query_old->value(10).toString() + "\n\n" +
                                 "Событие П036 предполагает, что полис ОМС застрахованного был закрыт в связи с расторжением договора. \n\nЗакройте действующий полис ОМС по событию П025 и повторите операцию.\nОперация отменена.");
            // отмена операции
            data_polis.is_ok = false;
            delete query_old;
            return false;
        }
    }
    delete query_old;

    // добавим бланк ВС
    QString sql_add_blank =
            "update blanks_vs "
            "   set id_point=" + QString::number(data_app.id_point) + ", "
            "       id_operator=" + QString::number(data_app.id_operator) + ", "
            //"       date_add='" + QDate::currentDate().toString("dd.MM.yyyy") + "', "
            "       status=1, "
            "       id_person=" + QString::number(data_pers.id) + ", "
            "       id_polis=NULL, "
            "       date_spent = coalesce(date_spent, CURRENT_DATE) "
            " where vs_num='" + ui->line_vs_num->text().trimmed() + "' ; ";
    QSqlQuery *query_add_blank = new QSqlQuery(db);
    mySQL.exec(this, sql_add_blank, QString("Добавление бланка ВС"), *query_add_blank, true, db, data_app);
    if (query_add_blank->size()==0) {
        QMessageBox::critical(this, "Ошибка при добавлении бланка ВС!", "При добавлении бланка ВС произошла неожиданная ошибка!\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        delete query_add_blank;
        return false;
    }
    delete query_add_blank;

    // добавим полис в базу
    QString sql;
    sql = "insert into polises (id_person, ocato, enp, ogrn_smo, pol_v, pol_type, pol_ser, pol_num, vs_num, date_begin, date_end, date_stop, in_erp, id_old_polis, _id_last_point, _id_last_operator, _id_last_event, f_polis, date_get2hand) ";
    sql +=  " values (";
    sql +=  " "  + QString::number(data_polis.id_person) + ", ";
    sql +=  " '" + data_polis.ocato.trimmed() + "', ";
    sql +=  " '" + data_polis.enp.trimmed() + "', ";
    sql +=  " '" + data_polis.ogrn_smo.trimmed() + "', ";
    sql +=  " "  + QString::number(data_polis.pol_v) + ", ";
    sql +=  " '" + data_polis.pol_type.trimmed() + "', ";
    sql +=  (ui->ch_pol_sernum->isChecked()   ? (" '" + data_polis.pol_ser + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_sernum->isChecked()   ? (" '" + data_polis.pol_num + "'") : "NULL") + ", ";
    sql +=  (ui->ch_vs_num->isChecked()    ? (" '" + data_polis.vs_num  + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datbegin->isChecked() ? (" '" + data_polis.date_begin.toString("yyyy-MM-dd") + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datend->isChecked()   ? (" '" + data_polis.date_end.toString("yyyy-MM-dd")   + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datstop->isChecked()  ? (" '" + data_polis.date_stop.toString("yyyy-MM-dd")  + "'") : "NULL") + ", ";
    sql +=  " "  + QString::number(data_polis.in_erp) + ", ";
    //sql +=  " '" + data_polis.order_num.trimmed() + "', ";
    //sql +=  " '" + data_polis.order_date.toString("yyyy-MM-dd") + "', ";
    sql +=  (data_polis.id_old_polis<0 ? "NULL" : QString::number(data_polis.id_old_polis)) + ", ";
    sql +=  QString::number(data_app.id_point) + ", ";
    sql +=  QString::number(data_app.id_operator) + ", ";
    sql +=  "NULL, ";
    sql +=  QString::number(ui->combo_vizit_pan_form_polis->currentIndex()) + ", ";
    sql +=  " '" + QDate::currentDate().toString("yyyy-MM-dd") + "' ) ";
    sql +=  " returning id ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString("П036 - Добавление нового ВС"), *query, true, db, data_app);
    if (!res) {
        QMessageBox::critical(this, "Операция не выполнена!", "П036 - При добавлении нового ВС произошла неожиданная ошибка.\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        delete query;
        return false;
    } else {
        while (query->next()) {
            data_polis.id = query->value(0).toInt();
            data_event.is_ok = true;

            // добавим событие в базу
            QString sql = "insert into events (id_point, id_operator, id_polis, id_vizit, event_code, event_dt, event_time, comment, status, id_person, id_person_old, id_udl, id_drp) ";
            sql +=  " values (";
            sql +=  " "  + QString::number(data_app.id_point) + ", ";
            sql +=  " "  + QString::number(data_app.id_operator) + ", ";
            sql +=  " "  + QString::number(data_polis.id) + ", ";
            sql +=  " "  + QString::number(data_vizit.id) + ", ";
            sql +=  " '" + data_event.event_code.trimmed() + "', ";
            sql +=  " '" + data_event.event_dt.toString("yyyy-MM-dd") + "', ";
            sql +=  " '" + data_event.event_time.toString("hh:mm:ss") + "', ";
            sql +=  " '" + data_event.comment.trimmed() + "', 0,";
            sql +=  " "  + QString::number(data_pers.id) + ", ";
            sql +=  " "  + (data_pers.id_old>0 ? QString::number(data_pers.id_old) : "NULL") + ", ";
            sql +=  " "  + (data_pers.id_udl>0 ? QString::number(data_pers.id_udl) : "NULL") + ", ";
            sql +=  " "  + (data_pers.id_drp>0 ? QString::number(data_pers.id_drp) : "NULL") + ") ";
            sql +=  " returning id ; ";

            QSqlQuery *query_act = new QSqlQuery(db);
            bool res_act = mySQL.exec(this, sql, QString("П036 - Добавление нового события"), *query_act, true, db, data_app);
            if (!res_act || !query_act->next()) {
                QMessageBox::critical(this, "Операция не выполнена!", "П036 - Добавление нового события - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_act;
                return false;
            }

            // добавим _id_last_event в полис
            data_polis._id_last_event = query_act->value(0).toInt();
            QString sql_pa = "update polises "
                             "   set _id_last_event=" + QString::number(data_polis._id_last_event) + " "
                             " where id=" + QString::number(data_polis.id) + " ; ";

            QSqlQuery *query_pa = new QSqlQuery(db);
            bool res_pa = mySQL.exec(this, sql_pa, QString("П036 - Добавление id события в полис"), *query_pa, true, db, data_app);
            if (!res_pa) {
                QMessageBox::critical(this, "Операция не выполнена!", "П036 - Добавление id события в полис - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_pa;
                return false;
            }
            delete query_pa;

            // добавим id_polis в бланк ВС
            QString sql_pb = "update blanks_vs "
                             "   set id_polis=" + QString::number(data_polis.id) + " "
                             " where vs_num=" + data_polis.vs_num + " ; ";

            QSqlQuery *query_pb = new QSqlQuery(db);
            bool res_pb = mySQL.exec(this, sql_pb, QString("Добавление id события в полис"), *query_pb, true, db, data_app);
            if (!res_pb) {
                QMessageBox::critical(this, "Операция не выполнена!", "П010 - Добавление id полиса в бланк - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_pb;
                return false;
            }
            delete query_pb;
        }
    }
    delete query;

    // запомним форму полиса в данных визита
    QString sql_polf = "update vizits "
                       " set f_polis=" + QString::number(data_polis.f_polis) + " "
                       " where id=" + QString::number(data_vizit.id) + " ; ";
    QSqlQuery *query_polf = new QSqlQuery(db);
    bool res_polf = mySQL.exec(this, sql_polf, QString("запись формы полиса в визит"), *query_polf, true, db, data_app);
    if (!res_polf) {
        QMessageBox::critical(this, "Операция не выполнена!", "При записи формы выпуска полиса в визит произошла ошибка - операция отменена отменена.");
        delete query_polf;
        return false;
    }

    // запомним статус персоны - 1
    QString sql_pers_stat = "update persons "
                            " set status=1, "
                            "     _date_insure_begin=COALESCE(_date_insure_begin, CURRENT_DATE), "
                            "     _date_insure_end=NULL "
                            " where id=" + QString::number(data_pers.id) + " ; ";
    QSqlQuery *query_pers_stat = new QSqlQuery(db);
    bool res_pers_stat = mySQL.exec(this, sql_pers_stat, QString("Правка статуса персоны (1)"), *query_pers_stat, true, db, data_app);
    if (!res_pers_stat) {
        QMessageBox::critical(this, "Операция не выполнена!", "При правке статуса персоны (1) произошла ошибка - операция отменена отменена.");
        delete query_pers_stat;
        return false;
    }

    data_polis.is_ok = true;
    return true;
}
*/


// ---------------------------------
// --- П060 ------------------------
// ---------------------------------
bool polices_wnd::process_P060 (s_data_polis &data_polis, s_data_event &data_event) {
    // П060 - выдача на руки полиса единого образца после ВС
    if (data_event.event_code!="П060" || data_polis.pol_v!=3) {
        QMessageBox::critical(this, "Не тот код события или тип полиса!", "Обработчик события П060 получил данные события <" + data_event.event_code + ">\nТип полиса - "+QString::number(data_polis.pol_v)+"\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        return false;
    };
    // проверка длины введённой серии полиса
    if (ui->ch_pol_sernum->isChecked() && !(ui->line_pol_ser->text().trimmed().isEmpty()) && ui->line_pol_ser->text().length()!=4) {
        if (QMessageBox::question(this,
                                  "Нужно подтверждание",
                                  "Внимание!\n"
                                  "Длина введённой серии полиса не равна 4.\n\n"
                                  "Всё равно сохранить полис?",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Cancel)==QMessageBox::Cancel) {
            data_polis.is_ok = false;
            return false;
        }
    }

    data_polis.is_ok = true;

    // проверим, есть ли бланк полиса в таблице бланков полисов и свободен ли он
    QString sql_blank;
    sql_blank= "select b.status, s.text, b.enp "
               "  from blanks_pol b left join spr_blank_status s on(s.code=b.status) "
               " where b.pol_ser='" + data_polis.pol_ser + "' and "
               "       b.pol_num='" + data_polis.pol_num + "' and "
               "       b.id_person in (NULL, " + QString::number(data_pers.id) + ") ; ";
    QSqlQuery *query_blank = new QSqlQuery(db);
    mySQL.exec(this, sql_blank, QString("проверка существования и занятости бланка с заданной серией и номером"), *query_blank, true, db, data_app);
    if ( query_blank->size()<1 ) {
        QMessageBox::warning(this, "Бланк полиса не найден!",
                             "Введённые серия и номер не соответствуют ни одному известному бланку полиса, выпущенному для этого человека!\n\n"
                             "Добавьте персонифицированный бланк в таблицу бланков полисов или исправьте серию/номер бланка и повторите операцию.");
        data_polis.is_ok = false;
        delete query_blank;
        return false;
    }
    // проверка бланка на дублирование
    if ( query_blank->size()>1 ) {
        QMessageBox::warning (this, "Дублирование в таблице бланков!",
                              "Указанные серия и номер бланка встречаются в нескольких записях таблицы бланков!\n\n"
                              "Это ненормальная ситуация - обратитесь в службу тех-поддержки.");
        data_polis.is_ok = false;
        delete query_blank;
        return false;
    }
    // проверка соответствия ЕНП
    query_blank->next();
    QString enp_blank = query_blank->value(2).toString().trimmed();
    if ( (query_blank->size()==1) && (!ui->line_enp->text().trimmed().isEmpty()) && (enp_blank!=ui->line_enp->text().trimmed()) ) {
        QMessageBox::warning (this, "Несовпадение номера ЕНП!",
                              "Введённый ЕНП (" + ui->line_enp->text().trimmed() + ") не совпадает с номером ЕНП, сопоставленным выбранному бланку (" + enp_blank + ").\n\n"
                              "Операция отменена.");
        data_polis.is_ok = false;
        delete query_blank;
        return false;
    }
    // проверка на занятость бланка
    while (query_blank->next()) {
        int st = query_blank->value(0).toInt();
        QString st_txt = query_blank->value(1).toString();
        if (st!=0) {
            QMessageBox::warning(this, "Бланк уже используется!",
                                 "Введённые серия и номер соответствуют бланку полиса со статусом\n"
                                 "(" + QString::number(st) + ") " + st_txt + "\n\n"
                                 "Исправьте серию/номер бланка и повторите операцию.");
            data_polis.is_ok = false;
            delete query_blank;
            return false;
        }
    }
    delete query_blank;

    // проверка, можно ли применять это событие и надо ли закрывать старые полисы
    QString sql_old = "";
    sql_old += "select p.id, p.id_person, p.ocato, p.ogrn_smo, p.pol_v, f8.text as pol_v_text, p.pol_type, p.pol_ser, p.pol_num, p.vs_num, p.enp, p.date_begin, p.date_end, p.date_stop, p.in_erp, p.order_num, p.order_date, p._id_last_point, p._id_last_operator, p._id_last_event "
               "  from polises p "
               "  left join spr_f008 f8 on(f8.code=p.pol_v) "
               "  left join blanks_pol bp on(bp.id_polis=p.id) "
               "  left join blanks_vs bv on(bv.id_polis=p.id) "
               " where p.id_person=" + QString::number(data_pers.id) + " "
               "   and ((bp.id is not NULL and bp.status>0) or (bv.vs_num is not NULL and bv.status>0)) "
               "   and p.date_begin is not null  "
               "   and p.date_begin<=CURRENT_DATE  "
               "   and ( (p.date_stop is NULL) or (p.date_stop>CURRENT_DATE) ) ; ";

    QSqlQuery *query_old = new QSqlQuery(db);
    bool res_old = mySQL.exec(this, sql_old, QString("Новый полис - проверка, есть ли действующие старые полисы"), *query_old, true, db, data_app);
    // проверим старые полисы
    while (query_old->next()) {
        s_data_polis data_old_polis;
        data_old_polis.id = query_old->value(0).toInt();
        data_old_polis.id_person = query_old->value(1).toInt();
        data_old_polis.ocato = query_old->value(2).toString();
        data_old_polis.ogrn_smo = query_old->value(3).toString();
        data_old_polis.pol_v = query_old->value(4).toInt();
        data_old_polis.pol_v_text = query_old->value(5).toString();
        data_old_polis.pol_type = query_old->value(6).toString();
        data_old_polis.has_enp = !(query_old->value(10).isNull());
        data_old_polis.has_pol_sernum = !(query_old->value(8).isNull());
        data_old_polis.has_vs_num = !(query_old->value(9).isNull());
        data_old_polis.pol_ser = query_old->value(7).toString();
        data_old_polis.pol_num = query_old->value(8).toString();
        data_old_polis.vs_num = query_old->value(9).toString();
        data_old_polis.enp = query_old->value(10).toString().trimmed();
        data_old_polis.has_date_begin = !(query_old->value(11).isNull());
        data_old_polis.has_date_end = !(query_old->value(12).isNull());
        data_old_polis.has_date_stop = !(query_old->value(13).isNull());
        data_old_polis.date_begin = query_old->value(11).toDate();
        data_old_polis.date_end = query_old->value(12).toDate();
        data_old_polis.date_stop = query_old->value(13).toDate();
        data_old_polis.in_erp = query_old->value(14).toInt();
        data_old_polis.order_num = query_old->value(15).toString();
        data_old_polis.order_date = query_old->value(16).toDate();
        data_old_polis._id_last_point = query_old->value(17).toInt();
        data_old_polis._id_last_operator = query_old->value(18).toInt();
        data_old_polis._id_last_event = query_old->value(19).toInt();

        // другого действующего полиса быть не должно
        if ((data_old_polis.pol_v==1 || data_old_polis.pol_v==3)) {
            QMessageBox::warning(this,
                                 "Полис единого образца уже есть!",
                                 QString("Невозможно открыть новый полис единого образца, поскольку у застрахованного уже есть другой действующий полис единого образца:\n ") +
                                 "тип: " + query_old->value(4).toString() + "\n" +
                                 "серия/номер: " + query_old->value(7).toString() + " № " + query_old->value(8).toString() + "\n" +
                                 "дата выдачи: " + query_old->value(11).toDate().toString("dd.MM.yyyy") + "\n" +
                                 "№ ВС: " + query_old->value(9).toString() + "\n" +
                                 "ЕНП: " + query_old->value(10).toString() + "\n\n" +
                                 "Операция страхования отменена.");
            data_polis.is_ok = false;
            delete query_old;
            return false;
        }
        // закроем действующее ВС
        if ( data_old_polis.pol_v==2
             && ui->ch_get2hand->isChecked() ) {        // без выдачи полиса на руки старое ВС не закрываем
            data_polis.id_old_polis = data_old_polis.id;
            if (QMessageBox::question(this,
                                      "Закрыть ВС ?",
                                      QString("У застрахованного есть действующее временное свидетельство: \n") +
                                      "номер ВС: " + query_old->value(9).toString() + "\n" +
                                      "дата выдачи: " + query_old->value(11).toDate().toString("dd.MM.yyyy") + "\n" +
                                      "№ ВС: " + query_old->value(9).toString() + "\n" +
                                      "ЕНП: " + query_old->value(10).toString() + "\n\n" +
                                      "Событие П060 предполагает закрытие ВС и открытие нового полиса единого образца. \n\nЗакрыть ВС ?",
                                      QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Yes)==QMessageBox::Yes) {
                // проверим состояние старого полиса
                int close_stat = -2;
                QStringList list;
                list.append("ВС предоставлено и изымается");
                list.append("ВС утеряно / испорчено по вине застрахованного");
                if (QInputDialog::getItem(this, "Статус закрываемого ВС", "Выберите статус закрываемого ВС:", list, 0, false)=="полис ВС предоставлен и изымается")
                    close_stat = -2;
                else close_stat = -3;

            } else {
                // отмена операции
                QMessageBox::warning(this, "Операция страхования отменена", "При наличии действующего полиса единого образца выдача нового полиса единого образца невозможна.\n\nПри необходимости закройте действующий полис по событию П021 и повторите операцию.");
                data_polis.is_ok = false;
                delete query_old;
                return false;
            }
        }
    }
    delete query_old;

    // добавим полис в базу
    QString sql;
    sql = "insert into polises (id_person, ocato, enp, ogrn_smo, pol_v, pol_type, pol_ser, pol_num, vs_num, date_begin, date_end, date_stop, in_erp, /*order_num, order_date,*/ id_old_polis, _id_last_point, _id_last_operator, _id_last_event, f_polis, date_activate, date_get2hand, r_smo, r_polis) ";
    sql +=  " values (";
    sql +=  " "  + QString::number(data_polis.id_person) + ", ";
    sql +=  " '" + data_polis.ocato.trimmed() + "', ";
    sql +=  " '" + data_polis.enp.trimmed() + "', ";
    sql +=  " '" + data_polis.ogrn_smo.trimmed() + "', ";
    sql +=  " "  + QString::number(data_polis.pol_v) + ", ";
    sql +=  " '" + data_polis.pol_type.trimmed() + "', ";
    sql +=  (ui->ch_pol_sernum->isChecked()   ? (" '" + (QString("0000"+data_polis.pol_ser).right(4)) + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_sernum->isChecked()   ? (" '" + data_polis.pol_num + "'") : "NULL") + ", ";
    sql +=  (ui->ch_vs_num->isChecked()    ? (" '" + data_polis.vs_num  + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datbegin->isChecked() ? (" '" + data_polis.date_begin.toString("yyyy-MM-dd") + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datend->isChecked()   ? (" '" + data_polis.date_end.toString("yyyy-MM-dd")   + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datstop->isChecked()  ? (" '" + data_polis.date_stop.toString("yyyy-MM-dd")  + "'") : "NULL") + ", ";
    sql +=  " "  + QString::number(data_polis.in_erp) + ", ";
    //sql +=  " '" + data_polis.order_num.trimmed() + "', ";
    //sql +=  " '" + data_polis.order_date.toString("yyyy-MM-dd") + "', ";
    sql +=  (data_polis.id_old_polis<0 ? "NULL" : QString::number(data_polis.id_old_polis)) + ", ";
    sql +=  QString::number(data_app.id_point) + ", ";
    sql +=  QString::number(data_app.id_operator) + ", ";
    sql +=  "NULL, ";
    sql +=  QString::number(ui->combo_vizit_pan_form_polis->currentIndex()) + ", ";
    if (ui->ch_get2hand->isChecked()) {
        sql +=  " CURRENT_DATE, ";
        sql +=  " CURRENT_DATE, ";
    } else {
        sql +=  " CURRENT_DATE, ";
        sql +=  " NULL, ";
    }
    sql +=  QString::number(data_vizit.r_smo) + ", ";
    sql +=  QString::number(data_vizit.r_polis) + " ) ";
    sql +=  " returning id ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString("П060 - Добавление нового полиса ОМС"), *query, true, db, data_app);
    if (!res) {
        QMessageBox::critical(this, "Операция не выполнена!", "П060 - Добавление нового полиса ОМС - Операция страхования отменена.");
        data_polis.is_ok = false;
        delete query;
        return false;
    } else {
        while (query->next()) {
            data_polis.id = query->value(0).toInt();
            data_event.is_ok = true;

            // добавим событие в базу
            QString sql = "insert into events (id_point, id_operator, id_polis, id_vizit, event_code, event_dt, event_time, comment, status, id_person, id_person_old, id_udl, id_drp) ";
            sql +=  " values (";
            sql +=  " "  + QString::number(data_app.id_point) + ", ";
            sql +=  " "  + QString::number(data_app.id_operator) + ", ";
            sql +=  " "  + QString::number(data_polis.id) + ", ";
            sql +=  " "  + QString::number(data_vizit.id) + ", ";
            sql +=  " '" + data_event.event_code.trimmed() + "', ";
            sql +=  " '" + data_event.event_dt.toString("yyyy-MM-dd") + "', ";
            sql +=  " '" + data_event.event_time.toString("hh:mm:ss") + "', ";
            sql +=  " '" + data_event.comment.trimmed() + "', 0,";
            sql +=  " "  + QString::number(data_pers.id) + ", ";
            sql +=  " "  + (data_pers.id_old>0 ? QString::number(data_pers.id_old) : "NULL") + ", ";
            sql +=  " "  + (data_pers.id_udl>0 ? QString::number(data_pers.id_udl) : "NULL") + ", ";
            sql +=  " "  + (data_pers.id_drp>0 ? QString::number(data_pers.id_drp) : "NULL") + ") ";
            sql +=  " returning id ; ";

            QSqlQuery *query_act = new QSqlQuery(db);
            bool res_act = mySQL.exec(this, sql, QString("П060 - Добавление нового события"), *query_act, true, db, data_app);
            if (!res_act || !query_act->next()) {
                QMessageBox::critical(this, "Операция не выполнена!", "П060 - Добавление нового события - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_act;
                return false;
            }

            // добавим _id_last_event в полис
            data_polis._id_last_event = query_act->value(0).toInt();
            delete query_act;
            QString sql_pa = "update polises "
                             "   set _id_last_event=" + QString::number(data_polis._id_last_event) + " "
                             " where id=" + QString::number(data_polis.id) + " ; ";

            QSqlQuery *query_pa = new QSqlQuery(db);
            bool res_pa = mySQL.exec(this, sql_pa, QString("П060 - Добавление id события в полис"), *query_pa, true, db, data_app);
            if (!res_pa) {
                QMessageBox::critical(this, "Операция не выполнена!", "П060 - Добавление id события в полис - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_pa;
                return false;
            }
            delete query_pa;

            // обновим статус бланка полиса
            QString sql_b = "update blanks_pol  "
                            "   set status=1, "
                            "       id_polis=" + QString::number(data_polis.id) + ", "
                            "       id_person=" + QString::number(data_pers.id) + ", "
                            "       date_spent = coalesce(date_spent, CURRENT_DATE) "
                            " where pol_ser" + QString(data_polis.pol_ser.isEmpty() ? " is NULL " : QString("='" + ("0000"+data_polis.pol_ser).right(4) + "' ")) + " and "
                            "       pol_num='" + data_polis.pol_num + "' ; ";

            QSqlQuery *query_b = new QSqlQuery(db);
            bool res_b = mySQL.exec(this, sql_b, QString("П060 - правка статуса бланка"), *query_b, true, db, data_app);
            delete query_b;
        }
    }
    delete query;

    // запомним форму полиса в данных визита
    QString sql_polf = "update vizits "
                       " set f_polis=" + QString::number(data_polis.f_polis) + " "
                       " where id=" + QString::number(data_vizit.id) + " ; ";
    QSqlQuery *query_polf = new QSqlQuery(db);
    bool res_polf = mySQL.exec(this, sql_polf, QString("запись формы полиса в визит"), *query_polf, true, db, data_app);
    if (!res_polf) {
        QMessageBox::critical(this, "Операция не выполнена!", "При записи формы выпуска полиса в визит произошла ошибка - операция отменена отменена.");
        delete query_polf;
        return false;
    }
    delete query_polf;

    // запомним статус персоны - 2
    QString sql_pers_stat = "update persons "
                            " set status=2, "
                            "     _date_insure_begin=COALESCE(_date_insure_begin, CURRENT_DATE), "
                            "     _date_insure_end=NULL "
                            " where id=" + QString::number(data_pers.id) + " ; ";
    QSqlQuery *query_pers_stat = new QSqlQuery(db);
    bool res_pers_stat = mySQL.exec(this, sql_pers_stat, QString("Правка статуса персоны (2)"), *query_pers_stat, true, db, data_app);
    if (!res_pers_stat) {
        QMessageBox::critical(this, "Операция не выполнена!", "При правке статуса персоны (2) произошла ошибка - операция отменена отменена.");
        delete query_pers_stat;
        return false;
    }
    delete query_pers_stat;

    data_polis.is_ok = true;
    return true;
}


/*
// ---------------------------------
// --- П061 ------------------------
// ---------------------------------
bool polices_wnd::process_P061 (s_data_polis &data_polis, s_data_event &data_event) {
    // П061 - изменение сведений о застрахованном лице, требующее заменя полиса
    if (data_event.event_code!="П061" || data_polis.pol_v!=2) {
        QMessageBox::critical(this, "Не тот код события или тип полиса!", "Обработчик события П061 получил данные события <" + data_event.event_code + ">\nТип полиса - "+QString::number(data_polis.pol_v)+"\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        return false;
    };

    data_polis.is_ok = true;

    // проверим, попадает ли номер ВС в открытый интервал
    QString sql_int;
    sql_int= "select v.id as id_vs_inteval_pt "
             " from vs_intervals_pt v "
             "      left join folders f on(v.id_folder=f.id) "
             " where v.id_point=" + QString::number(data_app.id_point) + " "
             //"   and ( (f.folder_name is NULL) or (CHAR_LENGTH('" + data_app.default_folder + "')<1) or (f.folder_name=('" + data_app.default_folder + "')) ) "
             "   and v.date_open<=CURRENT_DATE "
             "   and ( (v.date_close is NULL) or (v.date_close>CURRENT_DATE) ) "
             "   and " + QString::number(ui->line_vs_num->text().toInt()) + " between v.vs_from and v.vs_to ";
    if (data_app.use_folders)
        sql_int += "   and ( (f.folder_name is NULL) or (CHAR_LENGTH('" + data_app.default_folder + "')<1) or (f.folder_name=('" + data_app.default_folder + "')) ) ; ";

    QSqlQuery *query_int = new QSqlQuery(db);
    mySQL.exec(this, sql_int, QString("проверка номера ВС по открытым диапазонам"), *query_int, true, db, data_app);
    if (query_int->size()<1) {
        QMessageBox::warning(this, "Неверный номер ВС!", "Введённый номер ВС не входит ни в один из интервалов номеров ВС данного пункта выдачи!\n\nИсправьте номер ВС и повторите операцию.");
        data_polis.is_ok = false;
        delete query_int;
        return false;
    }
    delete query_int;


    // проверим, свободен ли бланк ВС с таким номером
    QString sql_tst2;
    sql_tst2 = "select b.vs_num, s.code, '('||s.code||') '||s.text "
               "  from blanks_vs b "
               "  left join spr_blank_status s on(s.code=b.status) "
               " where b.vs_num=" + ui->line_vs_num->text().trimmed() + " "
               "   and b.status<>0 ; ";
    QSqlQuery* query_tst2 = new QSqlQuery(db);
    mySQL.exec(this, sql_tst2, QString("проверка, свободен ли бланк ВС с таким номером"), *query_tst2, true, db, data_app);
    if (query_tst2->next()) {
        QString status_text = query_tst2->value(2).toString();
        QMessageBox::critical(this, "Номер ВС занят!",
                              "Бланк ВС с указанным номером уже был использован ране!\n" + status_text + "\n\nИсправьте номер ВС и повторите операцию.");
        data_polis.is_ok = false;
        delete query_tst2;
        return false;
    }
    delete query_tst2;

    // проверка, можно ли применять это событие и надо ли закрывать старые полисы
    QString sql_old = "";
    sql_old += "select p.id, p.id_person, p.ocato, p.ogrn_smo, p.pol_v, f8.text as pol_v_text, p.pol_type, p.pol_ser, p.pol_num, p.vs_num, p.enp, p.date_begin, p.date_end, p.date_stop, p.in_erp, p.order_num, p.order_date, p._id_last_point, p._id_last_operator, p._id_last_event "
           " from polises p "
           "     left join spr_f008 f8 on(f8.code=p.pol_v) "
           "     left join blanks_pol bp on(bp.id_polis=p.id) "
           "     left join blanks_vs bv on(bv.id_polis=p.id) "
           " where p.id_person=" + QString::number(data_pers.id) + " "
           "   and ((bp.id is not NULL and bp.status>0) or (bv.vs_num is not NULL and bv.status>0)) "
           "   and p.date_begin is not null  "
           "   and p.date_begin<=CURRENT_DATE  "
           "   and ( (p.date_stop is NULL) or (p.date_stop>CURRENT_DATE) ) ; ";

    QSqlQuery *query_old = new QSqlQuery(db);
    bool res_old = mySQL.exec(this, sql_old, QString("Новый полис - проверка, есть ли действующие старые полисы"), *query_old, true, db, data_app);
    // проверим старые полисы
    while (query_old->next()) {
        s_data_polis data_old_polis;
        data_old_polis.id = query_old->value(0).toInt();
        data_old_polis.id_person = query_old->value(1).toInt();
        data_old_polis.ocato = query_old->value(2).toString();
        data_old_polis.ogrn_smo = query_old->value(3).toString();
        data_old_polis.pol_v = query_old->value(4).toInt();
        data_old_polis.pol_v_text = query_old->value(5).toString();
        data_old_polis.pol_type = query_old->value(6).toString();
        data_old_polis.has_enp = !(query_old->value(10).isNull());
        data_old_polis.has_pol_sernum = !(query_old->value(8).isNull());
        data_old_polis.has_vs_num = !(query_old->value(9).isNull());
        data_old_polis.pol_ser = query_old->value(7).toString();
        data_old_polis.pol_num = query_old->value(8).toString();
        data_old_polis.vs_num = query_old->value(9).toString();
        data_old_polis.enp = query_old->value(10).toString();
        data_old_polis.has_date_begin = !(query_old->value(11).isNull());
        data_old_polis.has_date_end = !(query_old->value(12).isNull());
        data_old_polis.has_date_stop = !(query_old->value(13).isNull());
        data_old_polis.date_begin = query_old->value(11).toDate();
        data_old_polis.date_end = query_old->value(12).toDate();
        data_old_polis.date_stop = query_old->value(13).toDate();
        data_old_polis.in_erp = query_old->value(14).toInt();
        data_old_polis.order_num = query_old->value(15).toString();
        data_old_polis.order_date = query_old->value(16).toDate();
        data_old_polis._id_last_point = query_old->value(17).toInt();
        data_old_polis._id_last_operator = query_old->value(18).toInt();
        data_old_polis._id_last_event = query_old->value(19).toInt();

        // другого действующего полиса быть не должно
        if (data_old_polis.pol_v==2) {
            QMessageBox::warning(this,
                                 "Невозможно открыть ВС!",
                                 QString("Невозможно открыть новое временное свидетельство, поскольку у застрахованного уже есть другое действующее ВС:\n ") +
                                 "номер: " + QString::number(query_old->value(9).toInt()) + "\n" +
                                 "дата выдачи: " + query_old->value(11).toDate().toString("dd.MM.yyyy") + "\n" +
                                      "№ ВС: " + query_old->value(9).toString() + "\n" +
                                 "ЕНП: " + query_old->value(10).toString() + "\n\n" +
                                 "Надо дождаться активации этого ВС из ТФОМС и, при необходимости, выдать новое ВС с закрытием полученного полиса единого образца.");
            data_polis.is_ok = false;
            delete query_old;
            return false;
        }
        if ((data_old_polis.pol_v==1 || data_old_polis.pol_v==3)) {
            data_polis.id_old_polis = data_old_polis.id;
            if (QMessageBox::question(this,
                                      "Закрыть старый полис ?",
                                      QString("У застрахованного есть действующий полис ОМС единого образца или полис ОМС старого образца нашей СМО: \n") +
                                      "тип: " + query_old->value(4).toString() + "\n" +
                                      "серия/номер: " + query_old->value(7).toString() + " № " + query_old->value(8).toString() + "\n" +
                                      "дата выдачи: " + query_old->value(11).toDate().toString("dd.MM.yyyy") + "\n" +
                                      "№ ВС: " + query_old->value(9).toString() + "\n" +
                                      "ЕНП: " + query_old->value(10).toString() + "\n\n" +
                                      "Событие П061 предполагает изменение идентификации застрахованного, делающее невозможным использование старого полиса. \n\nЗакрыть старый полис ?",
                                      QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Yes)==QMessageBox::Yes) {
                // проверим состояние старого полиса
                int close_stat = -2;
                QStringList list;
                list.append("полис предоставлен и изымается");
                list.append("полис утерян / испорчен по вине застрахованного");
                if (QInputDialog::getItem(this, "Статус закрываемого полиса", "Выберите статус закрываемого полиса:", list, 0, false)=="полис предоставлен и изымается")
                    close_stat = -2;
                else close_stat = -3;

                // закрытие старого полиса
                s_data_event close_event;
                close_event.id_polis = data_old_polis.id;
                close_event.event_code = "П021";
                close_event.event_dt = QDateTime::currentDateTime();
                close_event.event_time = QTime::currentTime();
                close_event.comment = "старый полис закрывается по событию П061, по причине изменения сведений о застрахованном лице, что сделало невозможным его идентификацию по старому полису.";
                if (!process_P021_P022_P023_P024_P025(data_old_polis, close_event, close_stat, true)) {
                    QMessageBox::warning(this, "Ошибка при закрытии старого полиса", "При попытке закрыть старый полис по событию П021 произошла непредвиденная ошибка.\nПопробуйте закрыть этот старый полис вручную и повторите операцию.\n\nОперация страхования отменена.");
                    data_polis.is_ok = false;
                    delete query_old;
                    return false;
                }
            } else {
                // отмена операции
                QMessageBox::warning(this, "Операция страхования отменена", "При наличии действующего старого полиса ОМС событие П061 неприменимо.\n\nПри необходимости закройте действующий полис ОМС по событию П021 и повторите операцию.");
                data_polis.is_ok = false;
                delete query_old;
                return false;
            }
        }
    }
    delete query_old;

    // добавим бланк ВС
    QString sql_add_blank =
            "update blanks_vs "
            "   set id_point=" + QString::number(data_app.id_point) + ", "
            "       id_operator=" + QString::number(data_app.id_operator) + ", "
            //"       date_add='" + QDate::currentDate().toString("dd.MM.yyyy") + "', "
            "       status=1, "
            "       id_person=" + QString::number(data_pers.id) + ", "
            "       id_polis=NULL, "
            "       date_spent = coalesce(date_spent, CURRENT_DATE) "
            " where vs_num='" + ui->line_vs_num->text().trimmed() + "' ; ";
    QSqlQuery *query_add_blank = new QSqlQuery(db);
    mySQL.exec(this, sql_add_blank, QString("Добавление бланка ВС"), *query_add_blank, true, db, data_app);
    if (query_add_blank->size()==0) {
        QMessageBox::critical(this, "Ошибка при добавлении бланка ВС!", "При добавлении бланка ВС произошла неожиданная ошибка!\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        delete query_add_blank;
        return false;
    }
    delete query_add_blank;

    // добавим полис в базу
    QString sql;
    sql = "insert into polises (id_person, ocato, enp, ogrn_smo, pol_v, pol_type, pol_ser, pol_num, vs_num, date_begin, date_end, date_stop, in_erp, id_old_polis, _id_last_point, _id_last_operator, _id_last_event, f_polis, date_get2hand) ";
    sql +=  " values (";
    sql +=  " "  + QString::number(data_polis.id_person) + ", ";
    sql +=  " '" + data_polis.ocato.trimmed() + "', ";
    sql +=  " '" + data_polis.enp.trimmed() + "', ";
    sql +=  " '" + data_polis.ogrn_smo.trimmed() + "', ";
    sql +=  " "  + QString::number(data_polis.pol_v) + ", ";
    sql +=  " '" + data_polis.pol_type.trimmed() + "', ";
    sql +=  (ui->ch_pol_sernum->isChecked()   ? (" '" + data_polis.pol_ser + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_sernum->isChecked()   ? (" '" + data_polis.pol_num + "'") : "NULL") + ", ";
    sql +=  (ui->ch_vs_num->isChecked()    ? (" '" + data_polis.vs_num  + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datbegin->isChecked() ? (" '" + data_polis.date_begin.toString("yyyy-MM-dd") + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datend->isChecked()   ? (" '" + data_polis.date_end.toString("yyyy-MM-dd")   + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datstop->isChecked()  ? (" '" + data_polis.date_stop.toString("yyyy-MM-dd")  + "'") : "NULL") + ", ";
    sql +=  " "  + QString::number(data_polis.in_erp) + ", ";
    //sql +=  " '" + data_polis.order_num.trimmed() + "', ";
    //sql +=  " '" + data_polis.order_date.toString("yyyy-MM-dd") + "', ";
    sql +=  (data_polis.id_old_polis<0 ? "NULL" : QString::number(data_polis.id_old_polis)) + ", ";
    sql +=  QString::number(data_app.id_point) + ", ";
    sql +=  QString::number(data_app.id_operator) + ", ";
    sql +=  "NULL, ";
    sql +=  QString::number(ui->combo_vizit_pan_form_polis->currentIndex()) + ", ";
    sql +=  " '" + QDate::currentDate().toString("yyyy-MM-dd") + "' ) ";
    sql +=  " returning id ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString("П061 - Добавление нового ВС"), *query, true, db, data_app);
    if (!res) {
        QMessageBox::critical(this, "Операция не выполнена!", "П061 - При добавлении нового ВС произошла неожиданная ошибка.\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        delete query;
        return false;
    } else {
        while (query->next()) {
            data_polis.id = query->value(0).toInt();
            data_event.is_ok = true;

            // добавим событие в базу
            QString sql = "insert into events (id_point, id_operator, id_polis, id_vizit, event_code, event_dt, event_time, comment, status, id_person, id_person_old, id_udl, id_drp) ";
            sql +=  " values (";
            sql +=  " "  + QString::number(data_app.id_point) + ", ";
            sql +=  " "  + QString::number(data_app.id_operator) + ", ";
            sql +=  " "  + QString::number(data_polis.id) + ", ";
            sql +=  " "  + QString::number(data_vizit.id) + ", ";
            sql +=  " '" + data_event.event_code.trimmed() + "', ";
            sql +=  " '" + data_event.event_dt.toString("yyyy-MM-dd") + "', ";
            sql +=  " '" + data_event.event_time.toString("hh:mm:ss") + "', ";
            sql +=  " '" + data_event.comment.trimmed() + "', 0,";
            sql +=  " "  + QString::number(data_pers.id) + ", ";
            sql +=  " "  + (data_pers.id_old>0 ? QString::number(data_pers.id_old) : "NULL") + ", ";
            sql +=  " "  + (data_pers.id_udl>0 ? QString::number(data_pers.id_udl) : "NULL") + ", ";
            sql +=  " "  + (data_pers.id_drp>0 ? QString::number(data_pers.id_drp) : "NULL") + ") ";
            sql +=  " returning id ; ";

            QSqlQuery *query_act = new QSqlQuery(db);
            bool res_act = mySQL.exec(this, sql, QString("П061 - Добавление нового события"), *query_act, true, db, data_app);
            if (!res_act || !query_act->next()) {
                QMessageBox::critical(this, "Операция не выполнена!", "П061 - Добавление нового события - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_act;
                return false;
            }

            // добавим _id_last_event в полис
            data_polis._id_last_event = query_act->value(0).toInt();
            QString sql_pa = "update polises "
                             "   set _id_last_event=" + QString::number(data_polis._id_last_event) + " "
                             " where id=" + QString::number(data_polis.id) + " ; ";

            QSqlQuery *query_pa = new QSqlQuery(db);
            bool res_pa = mySQL.exec(this, sql_pa, QString("П061 - Добавление id события в полис"), *query_pa, true, db, data_app);
            if (!res_pa) {
                QMessageBox::critical(this, "Операция не выполнена!", "П061 - Добавление id события в полис - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_pa;
                return false;
            }
            delete query_pa;

            // добавим id_polis в бланк ВС
            QString sql_pb = "update blanks_vs "
                             "   set id_polis=" + QString::number(data_polis.id) + " "
                             " where vs_num=" + data_polis.vs_num + " ; ";

            QSqlQuery *query_pb = new QSqlQuery(db);
            bool res_pb = mySQL.exec(this, sql_pb, QString("Добавление id события в полис"), *query_pb, true, db, data_app);
            if (!res_pb) {
                QMessageBox::critical(this, "Операция не выполнена!", "П010 - Добавление id полиса в бланк - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_pb;
                return false;
            }
            delete query_pb;
        }
    }
    delete query;

    // запомним форму полиса в данных визита
    QString sql_polf = "update vizits "
                       " set f_polis=" + QString::number(data_polis.f_polis) + " "
                       " where id=" + QString::number(data_vizit.id) + " ; ";
    QSqlQuery *query_polf = new QSqlQuery(db);
    bool res_polf = mySQL.exec(this, sql_polf, QString("запись формы полиса в визит"), *query_polf, true, db, data_app);
    if (!res_polf) {
        QMessageBox::critical(this, "Операция не выполнена!", "При записи формы выпуска полиса в визит произошла ошибка - операция отменена отменена.");
        delete query_polf;
        return false;
    }

    // запомним статус персоны - 1
    QString sql_pers_stat = "update persons "
                            " set status=1, "
                            "     _date_insure_begin=COALESCE(_date_insure_begin, CURRENT_DATE), "
                            "     _date_insure_end=NULL "
                            " where id=" + QString::number(data_pers.id) + " ; ";
    QSqlQuery *query_pers_stat = new QSqlQuery(db);
    bool res_pers_stat = mySQL.exec(this, sql_pers_stat, QString("Правка статуса персоны (1)"), *query_pers_stat, true, db, data_app);
    if (!res_pers_stat) {
        QMessageBox::critical(this, "Операция не выполнена!", "При правке статуса персоны (1) произошла ошибка - операция отменена отменена.");
        delete query_pers_stat;
        return false;
    }

    data_polis.is_ok = true;
    return true;
}



// ---------------------------------
// --- П062 ------------------------
// ---------------------------------
bool polices_wnd::process_P062 (s_data_polis &data_polis, s_data_event &data_event) {
    // П062 - выдача дубликата полиса без изменения данных застрахованного
    if (data_event.event_code!="П062" || data_polis.pol_v!=2) {
        QMessageBox::critical(this, "Не тот код события или тип полиса!", "Обработчик события П062 получил данные события <" + data_event.event_code + ">\nТип полиса - "+QString::number(data_polis.pol_v)+"\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        return false;
    };

    data_polis.is_ok = true;

    // проверим, попадает ли номер ВС в открытый интервал
    QString sql_int;
    sql_int= "select v.id as id_vs_inteval_pt "
             " from vs_intervals_pt v "
             "      left join folders f on(v.id_folder=f.id) "
             " where v.id_point=" + QString::number(data_app.id_point) + " "
             //"   and ( (f.folder_name is NULL) or (CHAR_LENGTH('" + data_app.default_folder + "')<1) or (f.folder_name=('" + data_app.default_folder + "')) ) "
             "   and v.date_open<=CURRENT_DATE "
             "   and ( (v.date_close is NULL) or (v.date_close>CURRENT_DATE) ) "
             "   and " + QString::number(ui->line_vs_num->text().toInt()) + " between v.vs_from and v.vs_to ";
    if (data_app.use_folders)
        sql_int += "   and ( (f.folder_name is NULL) or (CHAR_LENGTH('" + data_app.default_folder + "')<1) or (f.folder_name=('" + data_app.default_folder + "')) ) ; ";

    QSqlQuery *query_int = new QSqlQuery(db);
    mySQL.exec(this, sql_int, QString("проверка номера ВС по открытым диапазонам"), *query_int, true, db, data_app);
    if (query_int->size()<1) {
        QMessageBox::warning(this, "Неверный номер ВС!", "Введённый номер ВС не входит ни в один из интервалов номеров ВС данного пункта выдачи!\n\nИсправьте номер ВС и повторите операцию.");
        data_polis.is_ok = false;
        delete query_int;
        return false;
    }
    delete query_int;

    // проверим, свободен ли бланк ВС с таким номером
    QString sql_tst2;
    sql_tst2 = "select b.vs_num, s.code, '('||s.code||') '||s.text "
               "  from blanks_vs b "
               "  left join spr_blank_status s on(s.code=b.status) "
               " where b.vs_num=" + ui->line_vs_num->text().trimmed() + " "
               "   and b.status<>0 ; ";
    QSqlQuery* query_tst2 = new QSqlQuery(db);
    mySQL.exec(this, sql_tst2, QString("проверка, свободен ли бланк ВС с таким номером"), *query_tst2, true, db, data_app);
    if (query_tst2->next()) {
        QString status_text = query_tst2->value(2).toString();
        QMessageBox::critical(this, "Номер ВС занят!",
                              "Бланк ВС с указанным номером уже был использован ране!\n" + status_text + "\n\nИсправьте номер ВС и повторите операцию.");
        data_polis.is_ok = false;
        delete query_tst2;
        return false;
    }
    delete query_tst2;

    // проверка, можно ли применять это событие и надо ли закрывать старые полисы
    QString sql_old = "";
    sql_old += "select p.id, p.id_person, p.ocato, p.ogrn_smo, p.pol_v, f8.text as pol_v_text, p.pol_type, p.pol_ser, p.pol_num, p.vs_num, p.enp, p.date_begin, p.date_end, p.date_stop, p.in_erp, p.order_num, p.order_date, p._id_last_point, p._id_last_operator, p._id_last_event "
           " from polises p "
           "     left join spr_f008 f8 on(f8.code=p.pol_v) "
           "     left join blanks_pol bp on(bp.id_polis=p.id) "
           "     left join blanks_vs bv on(bv.id_polis=p.id) "
           " where p.id_person=" + QString::number(data_pers.id) + " "
           "   and ((bp.id is not NULL and bp.status>0) or (bv.vs_num is not NULL and bv.status>0)) "
           "   and p.date_begin is not null  "
           "   and p.date_begin<=CURRENT_DATE  "
           "   and ( (p.date_stop is NULL) or (p.date_stop>CURRENT_DATE) ) ; ";

    QSqlQuery *query_old = new QSqlQuery(db);
    bool res_old = mySQL.exec(this, sql_old, QString("Новый полис - проверка, есть ли действующие старые полисы"), *query_old, true, db, data_app);
    // проверим старые полисы
    while (query_old->next()) {
        s_data_polis data_old_polis;
        data_old_polis.id = query_old->value(0).toInt();
        data_old_polis.id_person = query_old->value(1).toInt();
        data_old_polis.ocato = query_old->value(2).toString();
        data_old_polis.ogrn_smo = query_old->value(3).toString();
        data_old_polis.pol_v = query_old->value(4).toInt();
        data_old_polis.pol_v_text = query_old->value(5).toString();
        data_old_polis.pol_type = query_old->value(6).toString();
        data_old_polis.has_enp = !(query_old->value(10).isNull());
        data_old_polis.has_pol_sernum = !(query_old->value(8).isNull());
        data_old_polis.has_vs_num = !(query_old->value(9).isNull());
        data_old_polis.pol_ser = query_old->value(7).toString();
        data_old_polis.pol_num = query_old->value(8).toString();
        data_old_polis.vs_num = query_old->value(9).toString();
        data_old_polis.enp = query_old->value(10).toString();
        data_old_polis.has_date_begin = !(query_old->value(11).isNull());
        data_old_polis.has_date_end = !(query_old->value(12).isNull());
        data_old_polis.has_date_stop = !(query_old->value(13).isNull());
        data_old_polis.date_begin = query_old->value(11).toDate();
        data_old_polis.date_end = query_old->value(12).toDate();
        data_old_polis.date_stop = query_old->value(13).toDate();
        data_old_polis.in_erp = query_old->value(14).toInt();
        data_old_polis.order_num = query_old->value(15).toString();
        data_old_polis.order_date = query_old->value(16).toDate();
        data_old_polis._id_last_point = query_old->value(17).toInt();
        data_old_polis._id_last_operator = query_old->value(18).toInt();
        data_old_polis._id_last_event = query_old->value(19).toInt();

        // другого действующего полиса быть не должно
        if (data_old_polis.pol_v==2) {
            QMessageBox::warning(this,
                                 "Невозможно открыть ВС!",
                                 QString("Невозможно открыть новое временное свидетельство, поскольку у застрахованного уже есть другое действующее ВС:\n ") +
                                 "номер: " + QString::number(query_old->value(9).toInt()) + "\n" +
                                 "дата выдачи: " + query_old->value(11).toDate().toString("dd.MM.yyyy") + "\n" +
                                      "№ ВС: " + query_old->value(9).toString() + "\n" +
                                 "ЕНП: " + query_old->value(10).toString() + "\n\n" +
                                 "Надо дождаться активации этого ВС из ТФОМС и, при необходимости, выдать новое ВС с закрытием полученного полиса единого образца.");
            data_polis.is_ok = false;
            delete query_old;
            return false;
        }
        if ((data_old_polis.pol_v==1 || data_old_polis.pol_v==3)) {
            data_polis.id_old_polis = data_old_polis.id;
            if (QMessageBox::question(this,
                                      "Закрыть старый полис ?",
                                      QString("У застрахованного есть действующий полис ОМС единого образца или полис ОМС старого образца нашей СМО: \n") +
                                      "тип: " + query_old->value(4).toString() + "\n" +
                                      "серия/номер: " + query_old->value(7).toString() + " № " + query_old->value(8).toString() + "\n" +
                                      "дата выдачи: " + query_old->value(11).toDate().toString("dd.MM.yyyy") + "\n" +
                                      "№ ВС: " + query_old->value(9).toString() + "\n" +
                                      "ЕНП: " + query_old->value(10).toString() + "\n\n" +
                                      "Событие П062 предполагает утерю или порчу бланка старого полиса. \n\nПометьть старый полис как утеряный/испорченный по вине застрахованного ?",
                                      QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Yes)==QMessageBox::Yes) {
                // проверим состояние старого полиса
                int close_stat = -2;
                QStringList list;
                list.append("полис предоставлен и изымается");
                list.append("полис утерян / испорчен по вине застрахованного");
                if (QInputDialog::getItem(this, "Статус закрываемого полиса", "Выберите статус закрываемого полиса:", list, 0, false)=="полис предоставлен и изымается")
                    close_stat = -2;
                else close_stat = -3;

                // закрытие старого полиса
                s_data_event close_event;
                close_event.id_polis = data_old_polis.id;
                close_event.event_code = "П021";
                close_event.event_dt = QDateTime::currentDateTime();
                close_event.event_time = QTime::currentTime();
                close_event.comment = "старый полис закрывается по событию П062, по причине утери или порчи по вине застрахованного.";
                if (!process_P021_P022_P023_P024_P025(data_old_polis, close_event, close_stat, true)) {
                    QMessageBox::warning(this, "Ошибка при закрытии старого полиса", "При попытке закрыть старый полис по событию П021 произошла непредвиденная ошибка.\nПопробуйте закрыть этот старый полис вручную и повторите операцию.\n\nОперация страхования отменена.");
                    data_polis.is_ok = false;
                    delete query_old;
                    return false;
                }
            } else {
                // отмена операции
                QMessageBox::warning(this, "Операция страхования отменена", "При наличии действующего старого полиса ОМС событие П062 неприменимо.\n\nПри необходимости закройте действующий полис ОМС по событию П021 и повторите операцию.");
                data_polis.is_ok = false;
                delete query_old;
                return false;
            }
        }
    }
    delete query_old;

    // добавим бланк ВС
    QString sql_add_blank =
            "update blanks_vs "
            "   set id_point=" + QString::number(data_app.id_point) + ", "
            "       id_operator=" + QString::number(data_app.id_operator) + ", "
            //"       date_add='" + QDate::currentDate().toString("dd.MM.yyyy") + "', "
            "       status=1, "
            "       id_person=" + QString::number(data_pers.id) + ", "
            "       id_polis=NULL, "
            "       date_spent = coalesce(date_spent, CURRENT_DATE) "
            " where vs_num='" + ui->line_vs_num->text().trimmed() + "' ; ";
    QSqlQuery *query_add_blank = new QSqlQuery(db);
    mySQL.exec(this, sql_add_blank, QString("Добавление бланка ВС"), *query_add_blank, true, db, data_app);
    if (query_add_blank->size()==0) {
        QMessageBox::critical(this, "Ошибка при добавлении бланка ВС!", "При добавлении бланка ВС произошла неожиданная ошибка!\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        delete query_add_blank;
        return false;
    }
    delete query_add_blank;

    // добавим полис в базу
    QString sql;
    sql = "insert into polises (id_person, ocato, enp, ogrn_smo, pol_v, pol_type, pol_ser, pol_num, vs_num, date_begin, date_end, date_stop, in_erp, id_old_polis, _id_last_point, _id_last_operator, _id_last_event, f_polis, date_get2hand) ";
    sql +=  " values (";
    sql +=  " "  + QString::number(data_polis.id_person) + ", ";
    sql +=  " '" + data_polis.ocato.trimmed() + "', ";
    sql +=  " '" + data_polis.enp.trimmed() + "', ";
    sql +=  " '" + data_polis.ogrn_smo.trimmed() + "', ";
    sql +=  " "  + QString::number(data_polis.pol_v) + ", ";
    sql +=  " '" + data_polis.pol_type.trimmed() + "', ";
    sql +=  (ui->ch_pol_sernum->isChecked()   ? (" '" + data_polis.pol_ser + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_sernum->isChecked()   ? (" '" + data_polis.pol_num + "'") : "NULL") + ", ";
    sql +=  (ui->ch_vs_num->isChecked()    ? (" '" + data_polis.vs_num  + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datbegin->isChecked() ? (" '" + data_polis.date_begin.toString("yyyy-MM-dd") + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datend->isChecked()   ? (" '" + data_polis.date_end.toString("yyyy-MM-dd")   + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datstop->isChecked()  ? (" '" + data_polis.date_stop.toString("yyyy-MM-dd")  + "'") : "NULL") + ", ";
    sql +=  " "  + QString::number(data_polis.in_erp) + ", ";
    //sql +=  " '" + data_polis.order_num.trimmed() + "', ";
    //sql +=  " '" + data_polis.order_date.toString("yyyy-MM-dd") + "', ";
    sql +=  (data_polis.id_old_polis<0 ? "NULL" : QString::number(data_polis.id_old_polis)) + ", ";
    sql +=  QString::number(data_app.id_point) + ", ";
    sql +=  QString::number(data_app.id_operator) + ", ";
    sql +=  "NULL, ";
    sql +=  QString::number(ui->combo_vizit_pan_form_polis->currentIndex()) + ", ";
    sql +=  " '" + QDate::currentDate().toString("yyyy-MM-dd") + "' ) ";
    sql +=  " returning id ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString("П062 - Добавление нового ВС"), *query, true, db, data_app);
    if (!res) {
        QMessageBox::critical(this, "Операция не выполнена!", "П062 - При добавлении нового ВС произошла неожиданная ошибка.\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        delete query;
        return false;
    } else {
        while (query->next()) {
            data_polis.id = query->value(0).toInt();
            data_event.is_ok = true;

            // добавим событие в базу
            QString sql = "insert into events (id_point, id_operator, id_polis, id_vizit, event_code, event_dt, event_time, comment, status, id_person, id_person_old, id_udl, id_drp) ";
            sql +=  " values (";
            sql +=  " "  + QString::number(data_app.id_point) + ", ";
            sql +=  " "  + QString::number(data_app.id_operator) + ", ";
            sql +=  " "  + QString::number(data_polis.id) + ", ";
            sql +=  " "  + QString::number(data_vizit.id) + ", ";
            sql +=  " '" + data_event.event_code.trimmed() + "', ";
            sql +=  " '" + data_event.event_dt.toString("yyyy-MM-dd") + "', ";
            sql +=  " '" + data_event.event_time.toString("hh:mm:ss") + "', ";
            sql +=  " '" + data_event.comment.trimmed() + "', 0,";
            sql +=  " "  + QString::number(data_pers.id) + ", ";
            sql +=  " "  + (data_pers.id_old>0 ? QString::number(data_pers.id_old) : "NULL") + ", ";
            sql +=  " "  + (data_pers.id_udl>0 ? QString::number(data_pers.id_udl) : "NULL") + ", ";
            sql +=  " "  + (data_pers.id_drp>0 ? QString::number(data_pers.id_drp) : "NULL") + ") ";
            sql +=  " returning id ; ";

            QSqlQuery *query_act = new QSqlQuery(db);
            bool res_act = mySQL.exec(this, sql, QString("П062 - Добавление нового события"), *query_act, true, db, data_app);
            if (!res_act || !query_act->next()) {
                QMessageBox::critical(this, "Операция не выполнена!", "П062 - Добавление нового события - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_act;
                return false;
            }

            // добавим _id_last_event в полис
            data_polis._id_last_event = query_act->value(0).toInt();
            QString sql_pa = "update polises "
                             "   set _id_last_event=" + QString::number(data_polis._id_last_event) + " "
                             " where id=" + QString::number(data_polis.id) + " ; ";

            QSqlQuery *query_pa = new QSqlQuery(db);
            bool res_pa = mySQL.exec(this, sql_pa, QString("П062 - Добавление id события в полис"), *query_pa, true, db, data_app);
            if (!res_pa) {
                QMessageBox::critical(this, "Операция не выполнена!", "П062 - Добавление id события в полис - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_pa;
                return false;
            }
            delete query_pa;

            // добавим id_polis в бланк ВС
            QString sql_pb = "update blanks_vs "
                             "   set id_polis=" + QString::number(data_polis.id) + " "
                             " where vs_num=" + data_polis.vs_num + " ; ";

            QSqlQuery *query_pb = new QSqlQuery(db);
            bool res_pb = mySQL.exec(this, sql_pb, QString("Добавление id события в полис"), *query_pb, true, db, data_app);
            if (!res_pb) {
                QMessageBox::critical(this, "Операция не выполнена!", "П010 - Добавление id полиса в бланк - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_pb;
                return false;
            }
            delete query_pb;
        }
    }
    delete query;

    // запомним форму полиса в данных визита
    QString sql_polf = "update vizits "
                       " set f_polis=" + QString::number(data_polis.f_polis) + " "
                       " where id=" + QString::number(data_vizit.id) + " ; ";
    QSqlQuery *query_polf = new QSqlQuery(db);
    bool res_polf = mySQL.exec(this, sql_polf, QString("запись формы полиса в визит"), *query_polf, true, db, data_app);
    if (!res_polf) {
        QMessageBox::critical(this, "Операция не выполнена!", "При записи формы выпуска полиса в визит произошла ошибка - операция отменена отменена.");
        delete query_polf;
        return false;
    }

    // запомним статус персоны - 1
    QString sql_pers_stat = "update persons "
                            " set status=1, "
                            "     _date_insure_begin=COALESCE(_date_insure_begin, CURRENT_DATE), "
                            "     _date_insure_end=NULL "
                            " where id=" + QString::number(data_pers.id) + " ; ";
    QSqlQuery *query_pers_stat = new QSqlQuery(db);
    bool res_pers_stat = mySQL.exec(this, sql_pers_stat, QString("Правка статуса персоны (1)"), *query_pers_stat, true, db, data_app);
    if (!res_pers_stat) {
        QMessageBox::critical(this, "Операция не выполнена!", "При правке статуса персоны (1) произошла ошибка - операция отменена отменена.");
        delete query_pers_stat;
        return false;
    }

    data_polis.is_ok = true;
    return true;
}



// ---------------------------------
// --- П063 ------------------------
// ---------------------------------
bool polices_wnd::process_P063 (s_data_polis &data_polis, s_data_event &data_event) {
    // П063 - выдача нового полиса для иностранных граждан с продлённым сроком проживания в России
    if (data_event.event_code!="П063" || data_polis.pol_v!=2) {
        QMessageBox::critical(this, "Не тот код события или тип полиса!", "Обработчик события П063 получил данные события <" + data_event.event_code + ">\nТип полиса - "+QString::number(data_polis.pol_v)+"\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        return false;
    };

    data_polis.is_ok = true;

    // проверим, попадает ли номер ВС в открытый интервал
    QString sql_int;
    sql_int= "select v.id as id_vs_inteval_pt "
             " from vs_intervals_pt v "
             "      left join folders f on(v.id_folder=f.id) "
             " where v.id_point=" + QString::number(data_app.id_point) + " "
             //"   and ( (f.folder_name is NULL) or (CHAR_LENGTH('" + data_app.default_folder + "')<1) or (f.folder_name=('" + data_app.default_folder + "')) ) "
             "   and v.date_open<=CURRENT_DATE "
             "   and ( (v.date_close is NULL) or (v.date_close>CURRENT_DATE) ) "
             "   and " + QString::number(ui->line_vs_num->text().toInt()) + " between v.vs_from and v.vs_to ";
    if (data_app.use_folders)
        sql_int += "   and ( (f.folder_name is NULL) or (CHAR_LENGTH('" + data_app.default_folder + "')<1) or (f.folder_name=('" + data_app.default_folder + "')) ) ; ";

    QSqlQuery *query_int = new QSqlQuery(db);
    mySQL.exec(this, sql_int, QString("проверка номера ВС по открытым диапазонам"), *query_int, true, db, data_app);
    if (query_int->size()<1) {
        QMessageBox::warning(this, "Неверный номер ВС!", "Введённый номер ВС не входит ни в один из интервалов номеров ВС данного пункта выдачи!\n\nИсправьте номер ВС и повторите операцию.");
        data_polis.is_ok = false;
        delete query_int;
        return false;
    }
    delete query_int;

    // проверим, свободен ли бланк ВС с таким номером
    QString sql_tst2;
    sql_tst2 = "select b.vs_num, s.code, '('||s.code||') '||s.text "
               "  from blanks_vs b "
               "  left join spr_blank_status s on(s.code=b.status) "
               " where b.vs_num=" + ui->line_vs_num->text().trimmed() + " "
               "   and b.status<>0 ; ";
    QSqlQuery* query_tst2 = new QSqlQuery(db);
    mySQL.exec(this, sql_tst2, QString("проверка, свободен ли бланк ВС с таким номером"), *query_tst2, true, db, data_app);
    if (query_tst2->next()) {
        QString status_text = query_tst2->value(2).toString();
        QMessageBox::critical(this, "Номер ВС занят!",
                              "Бланк ВС с указанным номером уже был использован ране!\n" + status_text + "\n\nИсправьте номер ВС и повторите операцию.");
        data_polis.is_ok = false;
        delete query_tst2;
        return false;
    }
    delete query_tst2;

    // проверка, можно ли применять это событие и надо ли закрывать старые полисы
    QString sql_old = "";
    sql_old += "select p.id, p.id_person, p.ocato, p.ogrn_smo, p.pol_v, f8.text as pol_v_text, p.pol_type, p.pol_ser, p.pol_num, p.vs_num, p.enp, p.date_begin, p.date_end, p.date_stop, p.in_erp, p.order_num, p.order_date, p._id_last_point, p._id_last_operator, p._id_last_event "
           " from polises p "
           "     left join spr_f008 f8 on(f8.code=p.pol_v) "
           "     left join blanks_pol bp on(bp.id_polis=p.id) "
           "     left join blanks_vs bv on(bv.id_polis=p.id) "
           " where p.id_person=" + QString::number(data_pers.id) + " "
           "   and ((bp.id is not NULL and bp.status>0) or (bv.vs_num is not NULL and bv.status>0)) "
           "   and p.date_begin is not null  "
           "   and p.date_begin<=CURRENT_DATE  "
           "   and ( (p.date_stop is NULL) or (p.date_stop>CURRENT_DATE) ) ; ";

    QSqlQuery *query_old = new QSqlQuery(db);
    bool res_old = mySQL.exec(this, sql_old, QString("Новый полис - проверка, есть ли действующие старые полисы"), *query_old, true, db, data_app);
    // проверим старые полисы
    while (query_old->next()) {
        s_data_polis data_old_polis;
        data_old_polis.id = query_old->value(0).toInt();
        data_old_polis.id_person = query_old->value(1).toInt();
        data_old_polis.ocato = query_old->value(2).toString();
        data_old_polis.ogrn_smo = query_old->value(3).toString();
        data_old_polis.pol_v = query_old->value(4).toInt();
        data_old_polis.pol_v_text = query_old->value(5).toString();
        data_old_polis.pol_type = query_old->value(6).toString();
        data_old_polis.has_enp = !(query_old->value(10).isNull());
        data_old_polis.has_pol_sernum = !(query_old->value(8).isNull());
        data_old_polis.has_vs_num = !(query_old->value(9).isNull());
        data_old_polis.pol_ser = query_old->value(7).toString();
        data_old_polis.pol_num = query_old->value(8).toString();
        data_old_polis.vs_num = query_old->value(9).toString();
        data_old_polis.enp = query_old->value(10).toString();
        data_old_polis.has_date_begin = !(query_old->value(11).isNull());
        data_old_polis.has_date_end = !(query_old->value(12).isNull());
        data_old_polis.has_date_stop = !(query_old->value(13).isNull());
        data_old_polis.date_begin = query_old->value(11).toDate();
        data_old_polis.date_end = query_old->value(12).toDate();
        data_old_polis.date_stop = query_old->value(13).toDate();
        data_old_polis.in_erp = query_old->value(14).toInt();
        data_old_polis.order_num = query_old->value(15).toString();
        data_old_polis.order_date = query_old->value(16).toDate();
        data_old_polis._id_last_point = query_old->value(17).toInt();
        data_old_polis._id_last_operator = query_old->value(18).toInt();
        data_old_polis._id_last_event = query_old->value(19).toInt();

        // другого действующего полиса быть не должно
        if (data_old_polis.pol_v==2) {
            QMessageBox::warning(this,
                                 "Невозможно открыть ВС!",
                                 QString("Невозможно открыть новое временное свидетельство, поскольку у застрахованного уже есть другое действующее ВС:\n ") +
                                 "тип: " + query_old->value(4).toString() + "\n" +
                                 "номер: " + QString::number(query_old->value(9).toInt()) + "\n" +
                                 "дата выдачи: " + query_old->value(11).toDate().toString("dd.MM.yyyy") + "\n" +
                                      "№ ВС: " + query_old->value(9).toString() + "\n" +
                                 "ЕНП: " + query_old->value(10).toString() + "\n\n" +
                                 "Надо дождаться активации этого ВС из ТФОМС и, при необходимости, выдать новое ВС с закрытием полученного полиса единого образца.");
            data_polis.is_ok = false;
            delete query_old;
            return false;
        }
        if ((data_old_polis.pol_v==1 || data_old_polis.pol_v==3)) {
            data_polis.id_old_polis = data_old_polis.id;
            if (QMessageBox::question(this,
                                      "Закрыть старый полис ?",
                                      QString("У застрахованного есть действующий полис ОМС единого образца или полис ОМС старого образца нашей СМО: \n") +
                                      "тип: " + query_old->value(4).toString() + "\n" +
                                      "серия/номер: " + query_old->value(7).toString() + " № " + query_old->value(8).toString() + "\n" +
                                      "дата выдачи: " + query_old->value(11).toDate().toString("dd.MM.yyyy") + "\n" +
                                      "№ ВС: " + query_old->value(9).toString() + "\n" +
                                      "ЕНП: " + query_old->value(10).toString() + "\n\n" +
                                      "Событие П063 предполагает закрытие прежнего полиса по причине окончания действия права на ОМС для иностранных граждан.\n\nПометьть старый полис как закрытый и изъятый ?",
                                      QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Yes)==QMessageBox::Yes) {
                // проверим состояние старого полиса
                int close_stat = -2;
                QStringList list;
                list.append("полис предоставлен и изымается");
                list.append("полис утерян / испорчен по вине застрахованного");
                if (QInputDialog::getItem(this, "Статус закрываемого полиса", "Выберите статус закрываемого полиса:", list, 0, false)=="полис предоставлен и изымается")
                    close_stat = -2;
                else close_stat = -3;

                // закрытие старого полиса
                s_data_event close_event;
                close_event.id_polis = data_old_polis.id;
                close_event.event_code = "П025";
                close_event.event_dt = QDateTime::currentDateTime();
                close_event.event_time = QTime::currentTime();
                close_event.comment = "старый полис закрывается по причине открытия нового, по событию П063. Вероятно закрываемый полис перестал действовать в связи с окончанием действия ДРП.";
                if (!process_P021_P022_P023_P024_P025(data_old_polis, close_event, close_stat, true)) {
                    QMessageBox::warning(this, "Ошибка при закрытии старого полиса", "При попытке закрыть старый полис по событию П021 произошла непредвиденная ошибка.\nПопробуйте закрыть этот старый полис вручную и повторите операцию.\n\nОперация страхования отменена.");
                    data_polis.is_ok = false;
                    delete query_old;
                    return false;
                }
            } else {
                // отмена операции
                QMessageBox::warning(this, "Операция страхования отменена", "При наличии действующего старого полиса ОМС событие П063 неприменимо.\n\nПри необходимости закройте действующий полис ОМС по событию П021 и повторите операцию.");
                data_polis.is_ok = false;
                delete query_old;
                return false;
            }
        }
    }
    delete query_old;

    // добавим бланк ВС
    QString sql_add_blank =
            "update blanks_vs "
            "   set id_point=" + QString::number(data_app.id_point) + ", "
            "       id_operator=" + QString::number(data_app.id_operator) + ", "
            //"       date_add='" + QDate::currentDate().toString("dd.MM.yyyy") + "', "
            "       status=1, "
            "       id_person=" + QString::number(data_pers.id) + ", "
            "       id_polis=NULL, "
            "       date_spent = coalesce(date_spent, CURRENT_DATE) "
            " where vs_num='" + ui->line_vs_num->text().trimmed() + "' ; ";
    QSqlQuery *query_add_blank = new QSqlQuery(db);
    mySQL.exec(this, sql_add_blank, QString("Добавление бланка ВС"), *query_add_blank, true, db, data_app);
    if (query_add_blank->size()==0) {
        QMessageBox::critical(this, "Ошибка при добавлении бланка ВС!", "При добавлении бланка ВС произошла неожиданная ошибка!\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        delete query_add_blank;
        return false;
    }
    delete query_add_blank;

    // добавим полис в базу
    QString sql;
    sql = "insert into polises (id_person, ocato, enp, ogrn_smo, pol_v, pol_type, pol_ser, pol_num, vs_num, date_begin, date_end, date_stop, in_erp, id_old_polis, _id_last_point, _id_last_operator, _id_last_event, f_polis, date_get2hand) ";
    sql +=  " values (";
    sql +=  " "  + QString::number(data_polis.id_person) + ", ";
    sql +=  " '" + data_polis.ocato.trimmed() + "', ";
    sql +=  " '" + data_polis.enp.trimmed() + "', ";
    sql +=  " '" + data_polis.ogrn_smo.trimmed() + "', ";
    sql +=  " "  + QString::number(data_polis.pol_v) + ", ";
    sql +=  " '" + data_polis.pol_type.trimmed() + "', ";
    sql +=  (ui->ch_pol_sernum->isChecked()   ? (" '" + data_polis.pol_ser + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_sernum->isChecked()   ? (" '" + data_polis.pol_num + "'") : "NULL") + ", ";
    sql +=  (ui->ch_vs_num->isChecked()    ? (" '" + data_polis.vs_num  + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datbegin->isChecked() ? (" '" + data_polis.date_begin.toString("yyyy-MM-dd") + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datend->isChecked()   ? (" '" + data_polis.date_end.toString("yyyy-MM-dd")   + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datstop->isChecked()  ? (" '" + data_polis.date_stop.toString("yyyy-MM-dd")  + "'") : "NULL") + ", ";
    sql +=  " "  + QString::number(data_polis.in_erp) + ", ";
    //sql +=  " '" + data_polis.order_num.trimmed() + "', ";
    //sql +=  " '" + data_polis.order_date.toString("yyyy-MM-dd") + "', ";
    sql +=  (data_polis.id_old_polis<0 ? "NULL" : QString::number(data_polis.id_old_polis)) + ", ";
    sql +=  QString::number(data_app.id_point) + ", ";
    sql +=  QString::number(data_app.id_operator) + ", ";
    sql +=  "NULL, ";
    sql +=  QString::number(ui->combo_vizit_pan_form_polis->currentIndex()) + ", ";
    sql +=  " '" + QDate::currentDate().toString("yyyy-MM-dd") + "' ) ";
    sql +=  " returning id ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString("П063 - Добавление нового ВС"), *query, true, db, data_app);
    if (!res) {
        QMessageBox::critical(this, "Операция не выполнена!", "П063 - При добавлении нового ВС произошла неожиданная ошибка.\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        delete query;
        return false;
    } else {
        while (query->next()) {
            data_polis.id = query->value(0).toInt();
            data_event.is_ok = true;

            // добавим событие в базу
            QString sql = "insert into events (id_point, id_operator, id_polis, id_vizit, event_code, event_dt, event_time, comment, status, id_person, id_person_old, id_udl, id_drp) ";
            sql +=  " values (";
            sql +=  " "  + QString::number(data_app.id_point) + ", ";
            sql +=  " "  + QString::number(data_app.id_operator) + ", ";
            sql +=  " "  + QString::number(data_polis.id) + ", ";
            sql +=  " "  + QString::number(data_vizit.id) + ", ";
            sql +=  " '" + data_event.event_code.trimmed() + "', ";
            sql +=  " '" + data_event.event_dt.toString("yyyy-MM-dd") + "', ";
            sql +=  " '" + data_event.event_time.toString("hh:mm:ss") + "', ";
            sql +=  " '" + data_event.comment.trimmed() + "', 0,";
            sql +=  " "  + QString::number(data_pers.id) + ", ";
            sql +=  " "  + (data_pers.id_old>0 ? QString::number(data_pers.id_old) : "NULL") + ", ";
            sql +=  " "  + (data_pers.id_udl>0 ? QString::number(data_pers.id_udl) : "NULL") + ", ";
            sql +=  " "  + (data_pers.id_drp>0 ? QString::number(data_pers.id_drp) : "NULL") + ") ";
            sql +=  " returning id ; ";

            QSqlQuery *query_act = new QSqlQuery(db);
            bool res_act = mySQL.exec(this, sql, QString("П063 - Добавление нового события"), *query_act, true, db, data_app);
            if (!res_act || !query_act->next()) {
                QMessageBox::critical(this, "Операция не выполнена!", "П063 - Добавление нового события - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_act;
                return false;
            }

            // добавим _id_last_event в полис
            data_polis._id_last_event = query_act->value(0).toInt();
            QString sql_pa = "update polises "
                             "   set _id_last_event=" + QString::number(data_polis._id_last_event) + " "
                             " where id=" + QString::number(data_polis.id) + " ; ";

            QSqlQuery *query_pa = new QSqlQuery(db);
            bool res_pa = mySQL.exec(this, sql_pa, QString("П063 - Добавление id события в полис"), *query_pa, true, db, data_app);
            if (!res_pa) {
                QMessageBox::critical(this, "Операция не выполнена!", "П063 - Добавление id события в полис - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_pa;
                return false;
            }
            delete query_pa;

            // добавим id_polis в бланк ВС
            QString sql_pb = "update blanks_vs "
                             "   set id_polis=" + QString::number(data_polis.id) + " "
                             " where vs_num=" + data_polis.vs_num + " ; ";

            QSqlQuery *query_pb = new QSqlQuery(db);
            bool res_pb = mySQL.exec(this, sql_pb, QString("Добавление id события в полис"), *query_pb, true, db, data_app);
            if (!res_pb) {
                QMessageBox::critical(this, "Операция не выполнена!", "П010 - Добавление id полиса в бланк - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_pb;
                return false;
            }
            delete query_pb;
        }
    }
    delete query;

    // запомним форму полиса в данных визита
    QString sql_polf = "update vizits "
                       " set f_polis=" + QString::number(data_polis.f_polis) + " "
                       " where id=" + QString::number(data_vizit.id) + " ; ";
    QSqlQuery *query_polf = new QSqlQuery(db);
    bool res_polf = mySQL.exec(this, sql_polf, QString("запись формы полиса в визит"), *query_polf, true, db, data_app);
    if (!res_polf) {
        QMessageBox::critical(this, "Операция не выполнена!", "При записи формы выпуска полиса в визит произошла ошибка - операция отменена отменена.");
        delete query_polf;
        return false;
    }

    // запомним статус персоны - 1
    QString sql_pers_stat = "update persons "
                            " set status=1, "
                            "     _date_insure_begin=COALESCE(_date_insure_begin, CURRENT_DATE), "
                            "     _date_insure_end=NULL "
                            " where id=" + QString::number(data_pers.id) + " ; ";
    QSqlQuery *query_pers_stat = new QSqlQuery(db);
    bool res_pers_stat = mySQL.exec(this, sql_pers_stat, QString("Правка статуса персоны (1)"), *query_pers_stat, true, db, data_app);
    if (!res_pers_stat) {
        QMessageBox::critical(this, "Операция не выполнена!", "При правке статуса персоны (1) произошла ошибка - операция отменена отменена.");
        delete query_pers_stat;
        return false;
    }

    data_polis.is_ok = true;
    return true;
}
*/



// ---------------------------------
// --- П061 П062 П063 --------------
// ---------------------------------
bool polices_wnd::process_P061_P062_P063 (s_data_polis &data_polis, s_data_event &data_event) {
    // П061 - изменение сведений о застрахованном лице, требующее заменя полиса
    // П062 - выдача дубликата полиса без изменения данных застрахованного
    // П063 - выдача нового полиса для иностранных граждан с продлённым сроком проживания в России
    if ( (data_event.event_code!="П061"
          && data_event.event_code!="П062"
          && data_event.event_code!="П063")
       || data_polis.pol_v!=2) {
        QMessageBox::critical(this, "Не тот код события или тип полиса!", "Обработчик события П061, П062, П063 получил данные события <" + data_event.event_code + ">\nТип полиса - "+QString::number(data_polis.pol_v)+"\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        return false;
    };

    data_polis.is_ok = true;

    // проверим, попадает ли номер ВС в открытый интервал
    QString sql_int;
    sql_int= "select v.id as id_vs_inteval_pt "
             "  from vs_intervals_pt v "
             "       left join folders f on(v.id_folder=f.id) "
             " where v.id_point=" + QString::number(data_app.id_point) + " "
             //"   and ( (f.folder_name is NULL) or (CHAR_LENGTH('" + data_app.default_folder + "')<1) or (f.folder_name=('" + data_app.default_folder + "')) ) "
             "   and v.date_open<=CURRENT_DATE "
             "   and ( (v.date_close is NULL) or (v.date_close>CURRENT_DATE) ) "
             "   and " + QString::number(ui->line_vs_num->text().toInt()) + " between v.vs_from and v.vs_to ";
    if (data_app.use_folders)
        sql_int += "   and ( (f.folder_name is NULL) or (CHAR_LENGTH('" + data_app.default_folder + "')<1) or (f.folder_name=('" + data_app.default_folder + "')) ) ; ";

    QSqlQuery *query_int = new QSqlQuery(db);
    mySQL.exec(this, sql_int, QString("проверка номера ВС по открытым диапазонам"), *query_int, true, db, data_app);
    if (query_int->size()<1) {
        QMessageBox::warning(this, "Неверный номер ВС!", "Введённый номер ВС не входит ни в один из интервалов номеров ВС данного пункта выдачи!\n\nИсправьте номер ВС и повторите операцию.");
        data_polis.is_ok = false;
        delete query_int;
        return false;
    }
    delete query_int;

    /*// проверим, не испорчен ли бланк ВС с таким номером
    QString sql_tst;
    sql_tst = "select b.vs_num "
                " from  blanks_vs b "
                " where b.vs_num=" + ui->line_vs_num->text().trimmed() + " "
                "   and b.status=-1 ; ";
    QSqlQuery* query_tst = new QSqlQuery(db);
    mySQL.exec(this, sql_tst, QString("проверка, не испорчен ли бланк ВС с таким номером"), *query_tst, true, db, data_app);
    if (query_tst->size()>0) {
        QMessageBox::critical(this, "Бланк ВС испорчен!", "Бланк ВС с указанным номером помечен как испорченный!\n\nИсправьте номер ВС и повторите операцию.");
        data_polis.is_ok = false;
        delete query_tst;
        return false;
    }
    delete query_tst;*/

    // проверим, свободен ли бланк ВС с таким номером
    QString sql_tst2;
    sql_tst2 = "select b.vs_num, s.code, '('||s.code||') '||s.text "
               "  from blanks_vs b "
               "  left join spr_blank_status s on(s.code=b.status) "
               " where b.vs_num=" + ui->line_vs_num->text().trimmed() + " "
               "   and b.status<>0 ; ";
    QSqlQuery* query_tst2 = new QSqlQuery(db);
    mySQL.exec(this, sql_tst2, QString("проверка, свободен ли бланк ВС с таким номером"), *query_tst2, true, db, data_app);
    if (query_tst2->next()) {
        QString status_text = query_tst2->value(2).toString();
        QMessageBox::critical(this, "Номер ВС занят!",
                              "Бланк ВС с указанным номером уже был использован ране!\n" + status_text + "\n\nИсправьте номер ВС и повторите операцию.");
        data_polis.is_ok = false;
        delete query_tst2;
        return false;
    }
    delete query_tst2;

    /*// если бумажный полис единого образца (серия и номер бланка)
    if (data_event.event_code=="П060") {
        // проверим, свободны ли номер и серия полиса
        QString sql_enp;
        sql_enp = "select b.pol_ser, b.pol_num "
                  " from  blanks_pol b "
                  " where v.pol_ser=" + ui->line_pol_ser->text().trimmed() + " and v.pol_num=" + ui->line_pol_num->text().trimmed() + " ; ";
        QSqlQuery *query_enp = new QSqlQuery(db);
        mySQL.exec(this, sql_enp, QString("проверка, свободны ли серия/номер полиса"), *query_enp, true);
        if (query_enp->size()>0) {
            QMessageBox::critical(this, "Серия и номер полиса уже используются!", "Бланк полиса с указанными серией и номером уже используется!\n Исправьте серию и номер бленка полиса и повторите операцию.");
            data_polis.is_ok = false;
            delete query_enp;
            return false;
        }
        delete query_enp;
    }*/

    // проверка, можно ли применять это событие и надо ли закрывать старые полисы
    QString sql_old = "";
    sql_old += "select p.id, p.id_person, p.ocato, p.ogrn_smo, p.pol_v, f8.text as pol_v_text, p.pol_type, p.pol_ser, p.pol_num, p.vs_num, p.enp, p.date_begin, p.date_end, p.date_stop, p.in_erp, p.order_num, p.order_date, p._id_last_point, p._id_last_operator, p._id_last_event "
               "  from polises p "
               "  left join spr_f008 f8 on(f8.code=p.pol_v) "
               "  left join blanks_pol bp on(bp.id_polis=p.id) "
               "  left join blanks_vs bv on(bv.id_polis=p.id) "
               " where p.id_person=" + QString::number(data_pers.id) + " "
               "   and ((bp.id is not NULL and bp.status>0) or (bv.vs_num is not NULL and bv.status>0)) "
               "   and p.date_begin is not null  "
               "   and p.date_begin<=CURRENT_DATE  "
               "   and ( (p.date_stop is NULL) or (p.date_stop>CURRENT_DATE) ) ; ";

    QSqlQuery *query_old = new QSqlQuery(db);
    bool res_old = mySQL.exec(this, sql_old, QString("Новый полис - проверка, есть ли действующие старые полисы"), *query_old, true, db, data_app);
    // проверим старые полисы
    while (query_old->next()) {
        s_data_polis data_old_polis;
        data_old_polis.id = query_old->value(0).toInt();
        data_old_polis.id_person = query_old->value(1).toInt();
        data_old_polis.ocato = query_old->value(2).toString();
        data_old_polis.ogrn_smo = query_old->value(3).toString();
        data_old_polis.pol_v = query_old->value(4).toInt();
        data_old_polis.pol_v_text = query_old->value(5).toString();
        data_old_polis.pol_type = query_old->value(6).toString();
        data_old_polis.has_enp = !(query_old->value(10).isNull());
        data_old_polis.has_pol_sernum = !(query_old->value(8).isNull());
        data_old_polis.has_vs_num = !(query_old->value(9).isNull());
        data_old_polis.pol_ser = query_old->value(7).toString();
        data_old_polis.pol_num = query_old->value(8).toString();
        data_old_polis.vs_num = query_old->value(9).toString();
        data_old_polis.enp = query_old->value(10).toString();
        data_old_polis.has_date_begin = !(query_old->value(11).isNull());
        data_old_polis.has_date_end = !(query_old->value(12).isNull());
        data_old_polis.has_date_stop = !(query_old->value(13).isNull());
        data_old_polis.date_begin = query_old->value(11).toDate();
        data_old_polis.date_end = query_old->value(12).toDate();
        data_old_polis.date_stop = query_old->value(13).toDate();
        data_old_polis.in_erp = query_old->value(14).toInt();
        data_old_polis.order_num = query_old->value(15).toString();
        data_old_polis.order_date = query_old->value(16).toDate();
        data_old_polis._id_last_point = query_old->value(17).toInt();
        data_old_polis._id_last_operator = query_old->value(18).toInt();
        data_old_polis._id_last_event = query_old->value(19).toInt();

        // другого действующего полиса быть не должно
        if (data_old_polis.pol_v==2) {
            QMessageBox::warning(this,
                                 "Невозможно открыть ВС!",
                                 QString("Невозможно открыть новое временное свидетельство, поскольку у застрахованного уже есть другое действующее ВС:\n ") +
                                 "тип: " + query_old->value(4).toString() + "\n" +
                                 "номер: " + QString::number(query_old->value(9).toInt()) + "\n" +
                                 "дата выдачи: " + query_old->value(11).toDate().toString("dd.MM.yyyy") + "\n" +
                                 "№ ВС: " + query_old->value(9).toString() + "\n" +
                                 "ЕНП: " + query_old->value(10).toString() + "\n\n" +
                                 "Надо дождаться активации этого ВС из ТФОМС и, при необходимости, выдать новое ВС с закрытием полученного полиса единого образца.");
            data_polis.is_ok = false;
            delete query_old;
            return false;
        }
        if ((data_old_polis.pol_v==1 || data_old_polis.pol_v==3)) {
            data_polis.id_old_polis = data_old_polis.id;
            if (QMessageBox::question(this,
                                      "Закрыть старый полис ?",
                                      QString("У застрахованного есть действующий полис ОМС единого образца или полис ОМС старого образца нашей СМО: \n") +
                                      "тип: " + query_old->value(4).toString() + "\n" +
                                      "серия/номер: " + query_old->value(7).toString() + " № " + query_old->value(8).toString() + "\n" +
                                      "дата выдачи: " + query_old->value(11).toDate().toString("dd.MM.yyyy") + "\n" +
                                      "№ ВС: " + query_old->value(9).toString() + "\n" +
                                      "ЕНП: " + query_old->value(10).toString() + "\n\n" +
                                      "Событие " + data_event.event_code + " предполагает изменение идентификации застрахованного, делающее невозможным использование старого полиса. \n\nЗакрыть старый полис ?",
                                      QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Yes)==QMessageBox::Yes) {
                // проверим состояние старого полиса
                int close_stat = -2;
                QStringList list;
                list.append("полис предоставлен и изымается");
                list.append("полис утерян / испорчен по вине застрахованного");
                if (QInputDialog::getItem(this, "Статус закрываемого полиса", "Выберите статус закрываемого полиса:", list, 0, false)=="полис предоставлен и изымается")
                    close_stat = -2;
                else close_stat = -3;

                // закрытие старого полиса
                s_data_event close_event;
                close_event.id_polis = data_old_polis.id;
                close_event.event_code = (data_event.event_code=="П063" ? "П025" : "П021");
                close_event.event_dt = QDateTime::currentDateTime();
                close_event.event_time = QTime::currentTime();
                if (data_event.event_code=="П061")
                    close_event.comment = "старый полис закрывается по событию П061, по причине изменения сведений о застрахованном лице, что сделало невозможным его идентификацию по старому полису.";
                else if (data_event.event_code=="П062")
                    close_event.comment = "старый полис закрывается по событию П062, по причине утери или порчи по вине застрахованного.";
                else if (data_event.event_code=="П063")
                    close_event.comment = "старый полис закрывается по причине открытия нового, по событию П063. Вероятно закрываемый полис перестал действовать в связи с окончанием действия ДРП.";

                if (!process_P021_P022_P023_P024_P025(data_old_polis, close_event, close_stat, true)) {
                    QMessageBox::warning(this, "Ошибка при закрытии старого полиса", "При попытке закрыть старый полис по событию П021 произошла непредвиденная ошибка.\nПопробуйте закрыть этот старый полис вручную и повторите операцию.\n\nОперация страхования отменена.");
                    data_polis.is_ok = false;
                    delete query_old;
                    return false;
                }
            } else {
                // отмена операции
                QMessageBox::warning(this, "Операция страхования отменена", "При наличии действующего старого полиса ОМС событие " + data_event.event_code + " неприменимо.\n\nПри необходимости закройте действующий полис ОМС по событию П021 и повторите операцию.");
                data_polis.is_ok = false;
                delete query_old;
                return false;
            }
        }
    }
    delete query_old;

    // добавим бланк ВС
    /*QString sql_add_blank =
            "insert into blanks_vs(vs_num, id_point, id_operator, date_add, status, id_person, id_polis) "
            " values(" + ui->line_vs_num->text().trimmed() + ", "
                       + QString::number(data_app.id_point) + ", "
                       + QString::number(data_app.id_operator) + ", "
                       + "'" + QDate::currentDate().toString("dd.MM.yyyy") + "', "
                       + " 1, "
                       + " " + QString::number(data_pers.id) + ", "
                       + " NULL) "
                       + " returning vs_num ; ";*/
    QString sql_add_blank =
            "update blanks_vs "
            "   set id_point=" + QString::number(data_app.id_point) + ", "
            "       id_operator=" + QString::number(data_app.id_operator) + ", "
            //"       date_add='" + QDate::currentDate().toString("dd.MM.yyyy") + "', "
            "       status=1, "
            "       id_person=" + QString::number(data_pers.id) + ", "
            "       id_polis=NULL, "
            "       date_spent = coalesce(date_spent, CURRENT_DATE) "
            " where vs_num='" + ui->line_vs_num->text().trimmed() + "' ; ";
    QSqlQuery *query_add_blank = new QSqlQuery(db);
    mySQL.exec(this, sql_add_blank, QString("Добавление бланка ВС"), *query_add_blank, true, db, data_app);
    if (query_add_blank->size()==0) {
        QMessageBox::critical(this, "Ошибка при добавлении бланка ВС!", "При добавлении бланка ВС произошла неожиданная ошибка!\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        delete query_add_blank;
        return false;
    }
    delete query_add_blank;

    // добавим полис в базу
    QString sql;
    sql = "insert into polises (id_person, ocato, enp, ogrn_smo, pol_v, pol_type, pol_ser, pol_num, vs_num, date_begin, date_end, date_stop, in_erp, /*order_num, order_date,*/ id_old_polis, _id_last_point, _id_last_operator, _id_last_event, f_polis, date_get2hand, r_smo, r_polis) ";
    sql +=  " values (";
    sql +=  " "  + QString::number(data_polis.id_person) + ", ";
    sql +=  " '" + data_polis.ocato.trimmed() + "', ";
    sql +=  " '" + data_polis.enp.trimmed() + "', ";
    sql +=  " '" + data_polis.ogrn_smo.trimmed() + "', ";
    sql +=  " "  + QString::number(data_polis.pol_v) + ", ";
    sql +=  " '" + data_polis.pol_type.trimmed() + "', ";
    sql +=  (ui->ch_pol_sernum->isChecked()   ? (" '" + data_polis.pol_ser + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_sernum->isChecked()   ? (" '" + data_polis.pol_num + "'") : "NULL") + ", ";
    sql +=  (ui->ch_vs_num->isChecked()    ? (" '" + data_polis.vs_num  + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datbegin->isChecked() ? (" '" + data_polis.date_begin.toString("yyyy-MM-dd") + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datend->isChecked()   ? (" '" + data_polis.date_end.toString("yyyy-MM-dd")   + "'") : "NULL") + ", ";
    sql +=  (ui->ch_pol_datstop->isChecked()  ? (" '" + data_polis.date_stop.toString("yyyy-MM-dd")  + "'") : "NULL") + ", ";
    sql +=  " "  + QString::number(data_polis.in_erp) + ", ";
    //sql +=  " '" + data_polis.order_num.trimmed() + "', ";
    //sql +=  " '" + data_polis.order_date.toString("yyyy-MM-dd") + "', ";
    sql +=  (data_polis.id_old_polis<0 ? "NULL" : QString::number(data_polis.id_old_polis)) + ", ";
    sql +=  QString::number(data_app.id_point) + ", ";
    sql +=  QString::number(data_app.id_operator) + ", ";
    sql +=  "NULL, ";
    sql +=  QString::number(ui->combo_vizit_pan_form_polis->currentIndex()) + ", ";
    sql +=  " '" + QDate::currentDate().toString("yyyy-MM-dd") + "', ";
    sql +=  QString::number(data_vizit.r_smo) + ", ";
    sql +=  QString::number(data_vizit.r_polis) + " ) ";
    sql +=  " returning id ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString(data_event.event_code + " - Добавление нового ВС"), *query, true, db, data_app);
    if (!res) {
        QMessageBox::critical(this, "Операция не выполнена!", data_event.event_code + " - При добавлении нового ВС произошла неожиданная ошибка.\n\nОперация страхования отменена.");
        data_polis.is_ok = false;
        delete query;
        return false;
    } else {
        while (query->next()) {
            data_polis.id = query->value(0).toInt();
            data_event.is_ok = true;

            // для старых полисов принудительно зададим дату закрытия не позже даты открытия нового полиса
            QString sql_ps = "update polises "
                             "   set date_stop='" + data_polis.date_begin.toString("yyyy-MM-dd") + "' "
                             " where id_person=" + QString::number(data_polis.id_person) + " "
                             "   and id<>" + QString::number(data_polis.id) + " "
                             "   and ( (date_stop is NULL) or (date_stop>'" + data_polis.date_begin.toString("yyyy-MM-dd") + "') ) ; ";

            QSqlQuery *query_ps = new QSqlQuery(db);
            bool res_ps = mySQL.exec(this, sql_ps, QString(data_event.event_code + " - Ограничение срока действия старых полисов"), *query_ps, true, db, data_app);
            if (!res_ps) {
                QMessageBox::critical(this, "Операция не выполнена!", data_event.event_code + " - Ограничение срока действия старых полисов - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_ps;
                return false;
            }
            delete query_ps;


            // добавим событие в базу
            QString sql = "insert into events (id_point, id_operator, id_polis, id_vizit, event_code, event_dt, event_time, comment, status, id_person, id_person_old, id_udl, id_drp) ";
            sql +=  " values (";
            sql +=  " "  + QString::number(data_app.id_point) + ", ";
            sql +=  " "  + QString::number(data_app.id_operator) + ", ";
            sql +=  " "  + QString::number(data_polis.id) + ", ";
            sql +=  " "  + QString::number(data_vizit.id) + ", ";
            sql +=  " '" + data_event.event_code.trimmed() + "', ";
            sql +=  " '" + data_event.event_dt.toString("yyyy-MM-dd") + "', ";
            sql +=  " '" + data_event.event_time.toString("hh:mm:ss") + "', ";
            sql +=  " '" + data_event.comment.trimmed() + "', 0,";
            sql +=  " "  + QString::number(data_pers.id) + ", ";
            sql +=  " "  + (data_pers.id_old>0 ? QString::number(data_pers.id_old) : "NULL") + ", ";
            sql +=  " "  + (data_pers.id_udl>0 ? QString::number(data_pers.id_udl) : "NULL") + ", ";
            sql +=  " "  + (data_pers.id_drp>0 ? QString::number(data_pers.id_drp) : "NULL") + ") ";
            sql +=  " returning id ; ";

            QSqlQuery *query_act = new QSqlQuery(db);
            bool res_act = mySQL.exec(this, sql, QString(data_event.event_code + " - Добавление нового события"), *query_act, true, db, data_app);
            if (!res_act || !query_act->next()) {
                QMessageBox::critical(this, "Операция не выполнена!", data_event.event_code + " - Добавление нового события - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_act;
                return false;
            }

            // добавим _id_last_event в полис
            data_polis._id_last_event = query_act->value(0).toInt();
            delete query_act;
            QString sql_pa = "update polises "
                             "   set _id_last_event=" + QString::number(data_polis._id_last_event) + " "
                             " where id=" + QString::number(data_polis.id) + " ; ";

            QSqlQuery *query_pa = new QSqlQuery(db);
            bool res_pa = mySQL.exec(this, sql_pa, QString(data_event.event_code + " - Добавление id события в полис"), *query_pa, true, db, data_app);
            if (!res_pa) {
                QMessageBox::critical(this, "Операция не выполнена!", data_event.event_code + " - Добавление id события в полис - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_pa;
                return false;
            }
            delete query_pa;

            // добавим id_polis в бланк ВС
            QString sql_pb = "update blanks_vs "
                             "   set id_polis=" + QString::number(data_polis.id) + " "
                             " where vs_num=" + data_polis.vs_num + " ; ";

            QSqlQuery *query_pb = new QSqlQuery(db);
            bool res_pb = mySQL.exec(this, sql_pb, QString("Добавление id события в полис"), *query_pb, true, db, data_app);
            if (!res_pb) {
                QMessageBox::critical(this, "Операция не выполнена!", "П010 - Добавление id полиса в бланк - Операция страхования отменена.");
                data_event.is_ok = false;
                delete query_pb;
                return false;
            }
            delete query_pb;
        }
    }
    delete query;

    // запомним форму полиса в данных визита
    QString sql_polf = "update vizits "
                       "   set f_polis=" + QString::number(data_polis.f_polis) + " "
                       " where id=" + QString::number(data_vizit.id) + " ; ";
    QSqlQuery *query_polf = new QSqlQuery(db);
    bool res_polf = mySQL.exec(this, sql_polf, QString("запись формы полиса в визит"), *query_polf, true, db, data_app);
    if (!res_polf) {
        QMessageBox::critical(this, "Операция не выполнена!", "При записи формы выпуска полиса в визит произошла ошибка - операция отменена отменена.");
        delete query_polf;
        return false;
    }
    delete query_polf;

    // запомним статус персоны - 1
    QString sql_pers_stat = "update persons "
                            " set status=1, "
                            "     _date_insure_begin=COALESCE(_date_insure_begin, CURRENT_DATE), "
                            "     _date_insure_end=NULL "
                            " where id=" + QString::number(data_pers.id) + " ; ";
    QSqlQuery *query_pers_stat = new QSqlQuery(db);
    bool res_pers_stat = mySQL.exec(this, sql_pers_stat, QString("Правка статуса персоны (1)"), *query_pers_stat, true, db, data_app);
    if (!res_pers_stat) {
        QMessageBox::critical(this, "Операция не выполнена!", "При правке статуса персоны (1) произошла ошибка - операция отменена отменена.");
        delete query_pers_stat;
        return false;
    }
    delete query_pers_stat;

    data_polis.is_ok = true;
    return true;
}



/*void polices_wnd::on_rb_vizit_pan_reason_smo_clicked(bool checked) {
    ui->combo_vizit_pan_reason_smo->setEnabled(checked);
    ui->combo_vizit_pan_reason_polis->setEnabled(!checked);
    if (checked) {
        ui->combo_vizit_pan_reason_polis->setCurrentIndex(0);
    } else {
        ui->combo_vizit_pan_reason_smo->setCurrentIndex(0);
    }
}

void polices_wnd::on_rb_vizit_pan_reason_polis_clicked(bool checked) {
    ui->combo_vizit_pan_reason_smo->setEnabled(!checked);
    ui->combo_vizit_pan_reason_polis->setEnabled(checked);
    if (checked) {
        ui->combo_vizit_pan_reason_smo->setCurrentIndex(0);
    } else {
        ui->combo_vizit_pan_reason_polis->setCurrentIndex(0);
    }
}*/

void polices_wnd::on_bn_polises_delete_clicked() {
    if (QInputDialog::getText(this,"Введите пароль",QString("Для удаления полиса без отправки данных в ТФОМС введите пароль\n") +
                              (data_app.is_head||data_app.is_tech ? "УДАЛИТЬ ПОЛИС" : "***")).toUpper()!="УДАЛИТЬ ПОЛИС") {
        QMessageBox::warning(this,"Отказ","Неправильный пароль");
        return;
    }

    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_polises || !ui->tab_polises->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите полис из списка.");
        return;
    }

    db.transaction();

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_polises->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        this->setCursor(Qt::WaitCursor);
        QModelIndex index = indexes.front();

        int id_polis      = model_polises.data(model_polises.index(index.row(), 0), Qt::EditRole).toInt();
        bool has_blank_vs =!(model_polises.data(model_polises.index(index.row(), 9), Qt::EditRole).isNull());
        QString vs_num    = model_polises.data(model_polises.index(index.row(), 9), Qt::EditRole).toString();
        bool has_blank_pol=!(model_polises.data(model_polises.index(index.row(), 11), Qt::EditRole).isNull());
        QString pol_ser   = model_polises.data(model_polises.index(index.row(), 10), Qt::EditRole).toString();
        QString pol_num   = model_polises.data(model_polises.index(index.row(), 11), Qt::EditRole).toString();


        // проверка связанных файлов обмена с фондом
        /*QString sql_test_files = "select id, fr.n_rec, tr.n_rec "
                               " from events a "
                               "      left join files_r_evts tr on(tr.id_event=a.id) "
                               "      left join files_r_from_fond fr on(fr.id_event=a.id)"
                               " where fr.n_rec is not NULL or tr.n_rec is not NULL ; ";*/
        QString sql_test_files = "select id, tr.n_rec "
                               " from events a "
                               "      left join files_r_evts tr on(tr.id_event=a.id) "
                               " where tr.n_rec is not NULL ; ";
        QSqlQuery *query_test_files = new QSqlQuery(db);
        bool res_files = mySQL.exec(this, sql_test_files, QString("Проверка файлов обмена, связанных с удаляемым полисом"), *query_test_files, true, db, data_app);
        if (!res_files || query_test_files->size()>0) {
            QMessageBox::critical(this, "Полис уже отправлен в ТФОМС!", "Удаляемый полис уже был отправлен в ТФОМС и не может быть удалён без отправки соответствующего события!\n\nПолис не удалён.");
            delete query_test_files;
            db.rollback();
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        delete query_test_files;

        // проверка связанных событий
        QString sql_test_act = "select count(*) from events where id_polis=" + QString::number(id_polis) + " ; ";
        QSqlQuery *query_test_act = new QSqlQuery(db);
        bool res_act = mySQL.exec(this, sql_test_act, QString("Подсчёт событий, связанных с удаляемым полисом"), *query_test_act, true, db, data_app);
        if (res_act && query_test_act->size()>0 && query_test_act->next()) {
            int cnt_act = query_test_act->value(0).toInt();
            if (QMessageBox::question(this,
                                      "Удалить связанные события?",
                                      "С удаляемой записью полиса связаны " + QString::number(cnt_act) + " событий.\nИх надо удалить.\n\nУдалить эти события?",
                                      QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel)==QMessageBox::Yes) {
                // удалим связанные записи событий
                QString sql_del_act =
                        "delete from events a where a.id_polis=" + QString::number(id_polis) + " ; ";
                QSqlQuery *query_del_act = new QSqlQuery(db);
                bool res_del_act = mySQL.exec(this, sql_del_act, QString("Проверка файлов обмена, связанных с удаляемым полисом"), *query_del_act, true, db, data_app);
                if (!res_del_act) {
                    QMessageBox::warning(this, "Не удалось удалить события!", "При попытке удалить связанные с удаляемым полисом события произошла ошибка!\n\nПолис не удалён.");
                    delete query_test_act;
                    delete query_del_act;
                    db.rollback();
                    this->setCursor(Qt::ArrowCursor);
                    return;
                }
                delete query_del_act;
            }
        }
        delete query_test_act;

        // удалим запись полиса
        QString sql_del_pol =
                "delete from polises p where p.id=" + QString::number(id_polis) + " ; ";
        QSqlQuery *query_del_pol = new QSqlQuery(db);
        bool res_del_pol = mySQL.exec(this, sql_del_pol, QString("Удаление полиса вручную, без отправки в ТФОМС"), *query_del_pol, true, db, data_app);
        if (!res_del_pol) {
            QMessageBox::warning(this, "Не удалось удалить полис!", "При попытке удалить полис без отправки в фонд произошла ошибка!\n\nПолис не удалён.");
            delete query_del_pol;
            db.rollback();
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        delete query_del_pol;

        if (has_blank_pol) {
            // удалим запись бланка полиса
            QString sql_del_blank_pol =
                    "delete from blanks_pol b where b.pol_ser='" + pol_ser + "' and b.pol_num=" + pol_num + " ; ";
            QSqlQuery *query_del_blank_pol = new QSqlQuery(db);
            bool res_del_blank_pol = mySQL.exec(this, sql_del_blank_pol, QString("Удаление бланка полиса вручную, без отправки в ТФОМС"), *query_del_blank_pol, true, db, data_app);
            if (!res_del_blank_pol) {
                QMessageBox::warning(this, "Не удалось удалить бланк полиса!", "При попытке удалить бланк полиса без отправки в фонд произошла ошибка!\n\nПолис не удалён.");
                delete query_del_blank_pol;
                db.rollback();
                this->setCursor(Qt::ArrowCursor);
                return;
            }
            delete query_del_blank_pol;
        }

        if (has_blank_vs) {
            // удалим запись бланка ВС
            QString sql_del_blank_vs =
                    "delete from blanks_vs b where b.vs_num=" + vs_num + " ; ";
            QSqlQuery *query_del_blank_vs = new QSqlQuery(db);
            bool res_del_blank_vs = mySQL.exec(this, sql_del_blank_vs, QString("Удаление бланка ВС вручную, без отправки в ТФОМС"), *query_del_blank_vs, true, db, data_app);
            if (!res_del_blank_vs) {
                QMessageBox::warning(this, "Не удалось удалить бланк ВС!", "При попытке удалить бланк ВС без отправки в фонд произошла ошибка!\n\nПолис не удалён.");
                delete query_del_blank_vs;
                db.rollback();
                this->setCursor(Qt::ArrowCursor);
                return;
            }
            delete query_del_blank_vs;
        }

        this->setCursor(Qt::ArrowCursor);
    } else {
        QMessageBox::warning(this, "Ничего не выбрано!", "Ничего не выбрано!");
    }

    db.commit();
    refresh_polises_tab();
}

void polices_wnd::on_bn_polises_spoiled_vs_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if ( data_app.ocato=="14000"
         && data_app.is_head<1
         && data_app.is_tech<1) {
        QMessageBox::warning(this, ("Доступно только в головном офисе!"), ("Добавление испорченных бланков ВС доступно только в головном офисе!"));
        return;
    }

    QString sp_vs = QInputDialog::getText(this,"Введите номер испорченного бланка","Введите номер испорченного бланка:").trimmed().replace(" ", "").replace("-", "").left(10);
    long i64 = sp_vs.toLong();

    if (QMessageBox::warning(this,
                             "Нужно подтверждение",
                             "Вы действительно хотите пометить как \"испорченный\" бланк ВС с номером \n\n" + QString::number(i64) + " ?",
                             QMessageBox::Yes|QMessageBox::No,
                             QMessageBox::No)==QMessageBox::No) {
        return;
    }

    db.transaction();
    this->setCursor(Qt::WaitCursor);

    // проверим, попадает ли номер ВС в открытый интервал
    QString sql_int;
    sql_int= "select v.id as id_vs_inteval_pt "
             " from vs_intervals_pt v "
             "      left join folders f on(v.id_folder=f.id) "
             " where v.id_point=" + QString::number(data_app.id_point) + " "
             //"   and ( (f.folder_name is NULL) or (CHAR_LENGTH('" + data_app.default_folder + "')<1) or (f.folder_name=('" + data_app.default_folder + "')) ) "
             "   and v.date_open<=CURRENT_DATE "
             "   and ( (v.date_close is NULL) or (v.date_close>CURRENT_DATE) ) "
             "   and " + QString::number(ui->line_vs_num->text().toInt()) + " between v.vs_from and v.vs_to ";
    if (data_app.use_folders)
        sql_int += "   and ( (f.folder_name is NULL) or (CHAR_LENGTH('" + data_app.default_folder + "')<1) or (f.folder_name=('" + data_app.default_folder + "')) ) ; ";

    QSqlQuery *query_int = new QSqlQuery(db);
    mySQL.exec(this, sql_int, QString("проверка номера ВС по открытым диапазонам"), *query_int, true, db, data_app);
    if (query_int->size()<1) {
        QMessageBox::warning(this,
                             "Этот бланк ВС был выделен для другого ПВП!",
                             "Введённый номер ВС не входит ни в один из интервалов номеров ВС данного пункта выдачи. "
                             "Отметить как \"испорченный\" бланк ВС можно только на том пункте выдачи, для которого он выделялся.\n\n"
                             "Операция отменена.");
        data_polis.is_ok = false;
        delete query_int;
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    delete query_int;

    // проверим, есть ли испорченный ВС в базе
    QString sql_ts;
    sql_ts = "select b.vs_num "
             "  from blanks_vs b "
             " where b.vs_num=" + QString::number(i64) + " ; ";
    QSqlQuery* query_ts = new QSqlQuery(db);
    bool res_ts = mySQL.exec(this, sql_ts, QString("Проверка, есть ли испорченный ВС в базе"), *query_ts, true, db, data_app);
    if (!res_ts) {
        this->setCursor(Qt::ArrowCursor);
        QMessageBox::critical(this,
                              "Ошибка при проверке испорченного ВС!",
                              "При проверке номера испорченного бланка ВС произошла непредвиденная ошибка!\n\n"
                              "Бланк не помечен.");
        data_polis.is_ok = false;
        delete query_ts;
        return;
    }
    if (query_ts->size()==0) {
            this->setCursor(Qt::ArrowCursor);
            QMessageBox::critical(this,
                                  "Такого номера ВС нет в базе!",
                                  "Такого номера ВС нет в базе\n\n"
                                  "Исправьте номер ВС и, при необходимости, повторите операцию.");
            data_polis.is_ok = false;
            delete query_ts;
            return;
        //}
    }
    delete query_ts;

    // проверим, не выдан ли испорченный ВС на руки
    QString sql_tst;
    sql_tst = "select b.vs_num, '('||b.status||') '||s.text as text "
              "  from blanks_vs b left join spr_blank_status s on(b.status=s.code)"
              " where b.vs_num=" + QString::number(i64) + " and b.status in(-5,-3,-2,-1,1,4); ";
    QSqlQuery* query_tst = new QSqlQuery(db);
    bool res_tst = mySQL.exec(this, sql_tst, QString("Проверка, не выдан ли на руки \"испорченный\" бланк ВС"), *query_tst, true, db, data_app);
    if (!res_tst) {
        this->setCursor(Qt::ArrowCursor);
        QMessageBox::critical(this,
                              "Ошибка при проверке \"испорченного\" ВС!",
                              "При проверке номера \"испорченного\" бланка ВС произошла непредвиденная ошибка!\n\n"
                              "Бланк не помечен.");
        data_polis.is_ok = false;
        delete query_tst;
        return;
    }
    if (query_tst->size()>0 && query_tst->next()) {
        QString status = query_tst->value(1).toString();
        if (QMessageBox::warning(this,
                                 "\"Испорченный\" бланк ВС выдан на руки!",
                                 "Бланк ВС с указанным номером уже использован со статусом:\n" +
                                 status + "\n\n"
                                 "ВНИМАНИЕ! Бланки ВС являются бланками строгой отчётности.\n\n"
                                 " - Если бланк уже выдан - застрахованный должен вернуть его в СМО.\n"
                                 " - Пока этот банк у застрахованного, помечать его как испорченный не рекомендуется.\n\n"
                                 "Пометить этот бланк ВС как испорченный, а полис закрыть?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::No) {
            //data_polis.is_ok = false;
            this->setCursor(Qt::ArrowCursor);
            delete query_tst;
            return;
        }
    }
    delete query_tst;

    // пометим бланк ВС как испорченный
    QString sql_sp_blank_vs =
            "update blanks_vs b "
            "   set status=-1, "
            "       date_spent = coalesce(date_spent, CURRENT_DATE), "
            "       id_operator=" + QString::number(data_app.id_operator) + " "
            " where b.vs_num=" + QString::number(i64) + " ; ";
    QSqlQuery *query_sp_blank_vs = new QSqlQuery(db);
    bool res_sp_blank_vs = mySQL.exec(this, sql_sp_blank_vs, QString("Пометка испорченного бланка ВС"), *query_sp_blank_vs, true, db, data_app);
    if (!res_sp_blank_vs) {
        this->setCursor(Qt::ArrowCursor);
        QMessageBox::warning(this,
                             "Не удалось добавить \"испорченный\" бланк ВС!",
                             "При попытке добавить \"испорченный\" бланк ВС произошла ошибка!\n\nБланк не помечен.");
        delete query_sp_blank_vs;
        db.rollback();
        return;
    }
    delete query_sp_blank_vs;

    // закроем, если есть, полис с этим номером ВС
    QString sql_close_polis =
            "update polises p "
            "   set date_stop = CURRENT_DATE,"
            "       date_spent = coalesce(date_spent, CURRENT_DATE), "
            "       _id_last_point=" + QString::number(data_app.id_point) + ", "
            "       _id_last_operator=" + QString::number(data_app.id_operator) + " "
            " where p.pol_v=2 "
            "   and p.vs_num=" + QString::number(i64) + " ; ";
    QSqlQuery *query_close_polis = new QSqlQuery(db);
    bool res_close_polis = mySQL.exec(this, sql_close_polis, QString("Закроем полис по номеру испорченного ВС"), *query_close_polis, true, db, data_app);
    if (!res_close_polis) {
        this->setCursor(Qt::ArrowCursor);
        QMessageBox::warning(this,
                             "Не удалось закрыть полис по номеру испорченного ВС!",
                             "При попытке закрыть полис по номеру испорченного ВС произошла ошибка!\n\nБланк не помечен.");
        delete query_close_polis;
        db.rollback();
        return;
    }
    delete query_close_polis;

    db.commit();
    this->setCursor(Qt::ArrowCursor);
    refresh_polises_tab();

    QMessageBox::information(this, "Бланк ВС отмечен как испорченный",
                             "Бланк ВС отмечен как испорченный.");
}

void polices_wnd::on_bn_print_db_pol_clicked() {
    // запомним последние данные полиса
    data_vizit.v_method  = ui->combo_vizit_pan_method->currentData().toInt();
    data_vizit.date = ui->date_vizit_pan->date();
    data_vizit.has_petition = ui->ch_vizit_pan_has_petition->isChecked();
    data_vizit.r_smo   = ui->combo_vizit_pan_reason_smo->currentData().toInt();
    data_vizit.r_polis = ui->combo_vizit_pan_reason_polis->currentData().toInt();
    data_vizit.f_polis = ui->combo_vizit_pan_form_polis->currentData().toInt();
    data_polis.has_vs_num = ui->ch_vs_num->isChecked();
    data_polis.vs_num = (data_polis.has_vs_num ? ui->line_vs_num->text().trimmed() : "");
    data_polis.has_uec_num = ui->ch_uec_num->isChecked();
    data_polis.uec_num = (data_polis.has_uec_num ? ui->line_uec_num->text().trimmed() : "");
    data_polis.has_date_begin = ui->ch_pol_datbegin->isChecked();
    data_polis.date_begin = (data_polis.has_date_begin ? ui->date_pol_begin->date() : QDate(1900,1,1));
    data_polis.has_date_end = ui->ch_pol_datend->isChecked();
    data_polis.date_end = (data_polis.has_date_end ? ui->date_pol_end->date() : QDate(1900,1,1));
    data_polis.has_date_stop = ui->ch_pol_datstop->isChecked();
    data_polis.date_stop = (data_polis.has_date_stop ? ui->date_pol_stop->date() : QDate(1900,1,1));
    data_polis.in_erp = ui->combo_pol_in_erp->currentData().toInt();
    data_polis.has_pol_sernum = ui->ch_pol_sernum->isChecked();
    data_polis.pol_ser = (data_polis.has_pol_sernum ? ui->line_pol_ser->text().trimmed() : "");
    data_polis.pol_num = (data_polis.has_pol_sernum ? ui->line_pol_num->text().trimmed() : "");
    data_polis.r_smo   = ui->combo_vizit_pan_reason_smo->currentData().toInt();
    data_polis.r_polis = ui->combo_vizit_pan_reason_polis->currentData().toInt();
    data_polis.f_polis = ui->combo_vizit_pan_form_polis->currentData().toInt();

    if (data_polis.vs_num.isEmpty()) {
        QMessageBox::information(this,"Генерация заявления отменена","Не введён номер ВС.\n\nГенерация заявления отменена.");
        return;
    }
    if ( data_polis.date_begin<QDate(1920,1,2) ) {
        QMessageBox::information(this,"Генерация заявления отменена","Дата выдачи полиса не введена или раньше 01.01.1920г.\n\nГенерация заявления отменена.");
        return;
    }
    /*on_bn_polis_save_clicked();
    if (!data_polis.is_ok) {
        QMessageBox::information(this,"Генерация заявления отменена","Генерация заявления отменена.");
        return;
    }*/

    delete  print_db_pol_w;
    print_db_pol_w = new print_db_pol_wnd(db, data_app, data_pers, data_vizit, data_polis, settings, this);
    print_db_pol_w->exec();
}

void polices_wnd::on_bn_now_act_clicked() {
    ui->date_activate->setDate(QDate::currentDate());
}

void polices_wnd::on_rb_previzit_clicked() {
    ui->date_vizit_pan->setDate(QDate::currentDate());
    ui->date_activate->setDate(QDate::currentDate());
    ui->date_stoppolis->setDate(QDate::currentDate());

    if (ui->rb_previzit->isChecked()) {
        ui->pan_first_vizit->setVisible(true);
        ui->pan_activate->setVisible(false);
        ui->pan_stoppolis->setVisible(false);
    }
}

void polices_wnd::on_rb_activate_clicked() {
    ui->date_vizit_pan->setDate(QDate::currentDate());
    ui->date_activate->setDate(QDate::currentDate());
    ui->date_stoppolis->setDate(QDate::currentDate());

    if (ui->rb_activate->isChecked()) {
        ui->pan_first_vizit->setVisible(false);
        ui->pan_activate->setVisible(true);
        ui->pan_stoppolis->setVisible(false);
    }
}

void polices_wnd::on_rb_stoppolis_clicked() {
    ui->date_vizit_pan->setDate(QDate::currentDate());
    ui->date_activate->setDate(QDate::currentDate());
    ui->date_stoppolis->setDate(QDate::currentDate());

    if (ui->rb_stoppolis->isChecked()) {
        ui->pan_first_vizit->setVisible(false);
        ui->pan_activate->setVisible(false);
        ui->pan_stoppolis->setVisible(true);
    }
}

void polices_wnd::on_rb_corrdata_clicked() {
    ui->date_vizit_pan->setDate(QDate::currentDate());
    ui->date_activate->setDate(QDate::currentDate());
    ui->date_stoppolis->setDate(QDate::currentDate());

    if (ui->rb_corrdata->isChecked()) {
        ui->pan_first_vizit->setVisible(false);
        ui->pan_activate->setVisible(false);
        ui->pan_stoppolis->setVisible(false);
    }
}

void polices_wnd::on_bn_pvp_n_vs_clicked() {
    // откроем окно "ПВП и ВС"
    delete pvp_n_vs_w;
    pvp_n_vs_w = new pvp_n_vs_wnd(db, data_app, data_pers, settings, this);
    pvp_n_vs_w->exec();
}

void polices_wnd::on_bn_blanks_clicked() {
    s_data_blank data_b;
    data_b.id = -1;
    data_b.enp = "";
    data_b.fam = "";
    data_b.im = "";
    data_b.ot = "";
    data_b.id_person = -1;
    data_b.id_polis = -1;
    data_b.pol_num = "";
    data_b.pol_num = "";
    data_b.snils = "";
    data_b.snils = "";
    // откроем окно "Бланки полисов"
    delete blanks_w;
    blanks_w = new blanks_wnd(db, data_b, data_app, settings, this);
    blanks_w->exec();
}

void polices_wnd::on_bn_calendar_clicked() {
    // откроем окно "Рабочий календарь"
    delete work_calendar_w;
    work_calendar_w = new work_calendar_wnd(db, data_app, settings, this);
    work_calendar_w->exec();
}


void polices_wnd::on_combo_vizit_pan_reason_smo_activated(int index) {
    ui->combo_vizit_pan_reason_polis->setEnabled(true);

    switch (index) {
    case 1: {
        ui->combo_vizit_pan_reason_polis->setEnabled(false);
        ui->combo_vizit_pan_reason_polis->setCurrentIndex(0);

        ui->bn_p010->setVisible(true);
        ui->bn_p031->setVisible(false);
        ui->bn_p032->setVisible(false);
        ui->bn_p033->setVisible(false);
        ui->bn_p034->setVisible(false);
        ui->bn_p035->setVisible(false);
        ui->bn_p036->setVisible(false);

        ui->lab_p010->setEnabled(true);
        ui->lab_p031->setEnabled(false);
        ui->lab_p032->setEnabled(false);
        ui->lab_p033->setEnabled(false);
        ui->lab_p034->setEnabled(false);
        ui->lab_p035->setEnabled(false);
        ui->lab_p036->setEnabled(false);
        }
        break;
    case 2: {
        ui->combo_vizit_pan_reason_polis->setEnabled(false);
        ui->combo_vizit_pan_reason_polis->setCurrentIndex(0);

        ui->bn_p010->setVisible(false);
        ui->bn_p031->setVisible(true);
        ui->bn_p032->setVisible(false);
        ui->bn_p033->setVisible(false);
        ui->bn_p034->setVisible(true);
        ui->bn_p035->setVisible(false);
        ui->bn_p036->setVisible(false);

        ui->lab_p010->setEnabled(false);
        ui->lab_p031->setEnabled(true);
        ui->lab_p032->setEnabled(false);
        ui->lab_p033->setEnabled(false);
        ui->lab_p034->setEnabled(true);
        ui->lab_p035->setEnabled(false);
        ui->lab_p036->setEnabled(false);
        }
        break;
    case 3: {
        ui->combo_vizit_pan_reason_polis->setEnabled(false);
        ui->combo_vizit_pan_reason_polis->setCurrentIndex(0);

        ui->bn_p010->setVisible(false);
        ui->bn_p031->setVisible(false);
        ui->bn_p032->setVisible(true);
        ui->bn_p033->setVisible(false);
        ui->bn_p034->setVisible(false);
        ui->bn_p035->setVisible(true);
        ui->bn_p036->setVisible(false);

        ui->lab_p010->setEnabled(false);
        ui->lab_p031->setEnabled(false);
        ui->lab_p032->setEnabled(true);
        ui->lab_p033->setEnabled(false);
        ui->lab_p034->setEnabled(false);
        ui->lab_p035->setEnabled(true);
        ui->lab_p036->setEnabled(false);
        }
        break;
    case 4: {
        ui->combo_vizit_pan_reason_polis->setEnabled(false);
        ui->combo_vizit_pan_reason_polis->setCurrentIndex(0);

        ui->bn_p010->setVisible(false);
        ui->bn_p031->setVisible(false);
        ui->bn_p032->setVisible(false);
        ui->bn_p033->setVisible(true);
        ui->bn_p034->setVisible(false);
        ui->bn_p035->setVisible(false);
        ui->bn_p036->setVisible(true);

        ui->lab_p010->setEnabled(false);
        ui->lab_p031->setEnabled(false);
        ui->lab_p032->setEnabled(false);
        ui->lab_p033->setEnabled(true);
        ui->lab_p034->setEnabled(false);
        ui->lab_p035->setEnabled(false);
        ui->lab_p036->setEnabled(true);
        }
        break;
    default: {
        ui->bn_p010->setVisible(false);
        ui->bn_p031->setVisible(false);
        ui->bn_p032->setVisible(false);
        ui->bn_p033->setVisible(false);
        ui->bn_p034->setVisible(false);
        ui->bn_p035->setVisible(false);
        ui->bn_p036->setVisible(false);

        ui->lab_p010->setEnabled(false);
        ui->lab_p031->setEnabled(false);
        ui->lab_p032->setEnabled(false);
        ui->lab_p033->setEnabled(false);
        ui->lab_p034->setEnabled(false);
        ui->lab_p035->setEnabled(false);
        ui->lab_p036->setEnabled(false);
        }
    }

    // П034 не доступно с 1 ноября по 31 декабря
    /*bool f1 = ui->bn_p034->isVisible();
    bool f2 = QDate::currentDate().month()<11;
    ui->bn_p034->setEnabled(f1 && f2);
    ui->lab_p034->setEnabled(f1 && f2);*/
}

void polices_wnd::on_combo_vizit_pan_reason_polis_activated(int index) {
    ui->combo_vizit_pan_reason_smo->setEnabled(true);

    switch (index) {
    case 1: {
        ui->combo_vizit_pan_reason_smo->setEnabled(false);
        ui->combo_vizit_pan_reason_smo->setCurrentIndex(0);

        ui->bn_p061->setVisible(true);
        ui->bn_p062->setVisible(false);
        ui->bn_p063->setVisible(false);

        ui->lab_p061->setEnabled(true);
        ui->lab_p062->setEnabled(false);
        ui->lab_p063->setEnabled(false);
        }
        break;
    case 2:
    case 3:
    case 4: {
        ui->combo_vizit_pan_reason_smo->setEnabled(false);
        ui->combo_vizit_pan_reason_smo->setCurrentIndex(0);

        ui->bn_p061->setVisible(false);
        ui->bn_p062->setVisible(true);
        ui->bn_p063->setVisible(false);

        ui->lab_p061->setEnabled(false);
        ui->lab_p062->setEnabled(true);
        ui->lab_p063->setEnabled(false);
        }
        break;
    case 5: {
        ui->combo_vizit_pan_reason_smo->setEnabled(false);
        ui->combo_vizit_pan_reason_smo->setCurrentIndex(0);

        ui->bn_p061->setVisible(false);
        ui->bn_p062->setVisible(false);
        ui->bn_p063->setVisible(true);

        ui->lab_p061->setEnabled(false);
        ui->lab_p062->setEnabled(false);
        ui->lab_p063->setEnabled(true);
        }
        break;
    default: {
        ui->bn_p061->setVisible(false);
        ui->bn_p062->setVisible(false);
        ui->bn_p063->setVisible(false);

        ui->lab_p061->setEnabled(false);
        ui->lab_p062->setEnabled(false);
        ui->lab_p063->setEnabled(false);
        }
    }
}

void polices_wnd::on_bn_layout_clicked() {
    //long long lay0 = (long long)ActivateKeyboardLayout(HKL(1), 0x00000008);
    long long lay = (long long)GetKeyboardLayout(GetCurrentThreadId());
    if (lay == 68748313) {      //Russian
        ui->bn_layout->setText("Руc");
        QPalette pal = ui->bn_layout->palette();
        pal.setColor(QPalette::Button,QColor(0,255,0,255));
        ui->bn_layout->setPalette(pal);
    } else if(lay == 67699721) {  //English
        ui->bn_layout->setText("Eng");
        QPalette pal = ui->bn_layout->palette();
        pal.setColor(QPalette::Button,QColor(128,128,255,255));
        ui->bn_layout->setPalette(pal);
    } else if(lay == 69338146) {  //Ukrainian
        ui->bn_layout->setText("Укр");
        QPalette pal = ui->bn_layout->palette();
        pal.setColor(QPalette::Button,QColor(255,0,0,255));
        ui->bn_layout->setPalette(pal);
    } else { //unknown language
        ui->bn_layout->setText("???");
        QPalette pal = ui->bn_layout->palette();
        pal.setColor(QPalette::Button,QColor(200,200,200,255));
        ui->bn_layout->setPalette(pal);
    }
}

void polices_wnd::on_ch_get2hand_clicked(bool checked) {
    if (checked) {
        QPalette pal = ui->pan_polis->palette();
        pal.setColor(QPalette::Base,QColor(255,255,255));
        ui->pan_polis->setPalette(pal);
    } else {
        QPalette pal = ui->pan_polis->palette();
        pal.setColor(QPalette::Base,QColor(255,255,0));
        ui->pan_polis->setPalette(pal);
    }
}

void polices_wnd::on_line_vs_num_textEdited(const QString &arg1) {
    QString txt0 = arg1.trimmed(), txt = "";
    for (int i=0; i<txt0.size(); ++i) {
        if (txt.length()>=14)
            break;
        QString c = txt0.mid(i, 1);
        if ( c=="0" || c=="1" || c=="2" || c=="3" || c=="4" || c=="5" || c=="6" || c=="7" || c=="8" || c=="9" ) {
            txt += c;
        }
    }
    ui->line_vs_num->setText(txt);
    ui->line_vs_num->setCursorPosition(txt.length());
}

void polices_wnd::on_line_pol_num_textEdited(const QString &arg1) {
    QString txt0 = arg1.trimmed(), txt = "";
    for (int i=0; i<txt0.size(); ++i) {
        if (txt.length()>=14)
            break;
        QString c = txt0.mid(i, 1);
        if ( c=="0" || c=="1" || c=="2" || c=="3" || c=="4" || c=="5" || c=="6" || c=="7" || c=="8" || c=="9" ) {
            txt += c;
        }
    }
    ui->line_pol_num->setText(txt);
    ui->line_pol_num->setCursorPosition(txt.length());
}

void polices_wnd::on_line_uec_num_textEdited(const QString &arg1) {
    QString txt0 = arg1.trimmed(), txt = "";
    for (int i=0; i<txt0.size(); ++i) {
        if (txt.length()>=30)
            break;
        QString c = txt0.mid(i, 1);
        if ( c=="0" || c=="1" || c=="2" || c=="3" || c=="4" || c=="5" || c=="6" || c=="7" || c=="8" || c=="9" ) {
            txt += c;
        }
    }
    ui->line_uec_num->setText(txt);
    ui->line_uec_num->setCursorPosition(txt.length());
}

void polices_wnd::on_line_enp_textEdited(const QString &arg1) {
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

void polices_wnd::on_combo_vizit_pan_method_activated(int index) {
    int method = ui->combo_vizit_pan_method->currentData().toInt();
    if ( !data_pers.rep_fio.isEmpty()
         && method!=2) {
        if (QMessageBox::warning(this, "Нужно подтверждение",
                                 "Этот человек обратился в СМО через представителя.\n"
                                 "Вы уверены, что хотите вручную задать метод обращения лично?\n"
                                 "В случае отказа, метод визита будет автоматически изменён на: \n"
                                 " \"(2) Через представителя\"",
                                 QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel)==QMessageBox::Cancel) {
            ui->combo_vizit_pan_method->setCurrentIndex(ui->combo_vizit_pan_method->findData(2));
        }
    }
}

void polices_wnd::on_bn_print_notic2_clicked() {
    data_vizit.v_method  = ui->combo_vizit_pan_method->currentData().toInt();
    data_vizit.date = ui->date_vizit_pan->date();
    data_vizit.has_petition = ui->ch_vizit_pan_has_petition->isChecked();
    data_vizit.r_smo   = ui->combo_vizit_pan_reason_smo->currentData().toInt();
    data_vizit.r_polis = ui->combo_vizit_pan_reason_polis->currentData().toInt();
    data_vizit.f_polis = ui->combo_vizit_pan_form_polis->currentData().toInt();
    data_polis.r_smo   = ui->combo_vizit_pan_reason_smo->currentData().toInt();
    data_polis.r_polis = ui->combo_vizit_pan_reason_polis->currentData().toInt();
    data_polis.f_polis = ui->combo_vizit_pan_form_polis->currentData().toInt();

    delete  print_notic2_w;
    print_notic2_w = new print_notic2_wnd(db, data_app, data_pers, data_vizit, data_polis, settings, this);
    print_notic2_w->exec();
}

void polices_wnd::on_date_pol_begin_dateChanged(const QDate &date) {
    // дата выдачи полиса не может быть позже чем через 5 дней
    if (date>QDate::currentDate().addDays(5))
        ui->date_pol_begin->setDate(QDate::currentDate().addDays(5));
}

void polices_wnd::on_date_pol_end_dateChanged(const QDate &date) {
    // срок действия полиса не может быть меньше чем 5 дней
    if (date<QDate::currentDate().addDays(5))
        ui->date_pol_end->setDate(QDate::currentDate().addDays(5));
}

void polices_wnd::on_date_pol_stop_dateChanged(const QDate &date) {
    // дата изъятия полиса не может быть раньше чем через 5 дней
    if (date<QDate::currentDate().addDays(5))
        ui->date_pol_stop->setDate(QDate::currentDate().addDays(5));
}
