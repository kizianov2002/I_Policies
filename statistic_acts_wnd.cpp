#include "statistic_acts_wnd.h"
#include "ui_statistic_acts_wnd.h"

statistic_acts_wnd::statistic_acts_wnd(QSqlDatabase &db, s_data_app &data_app, s_data_statistic &data_statistic, QWidget *parent)
    : db(db), data_app(data_app), data_statistic(data_statistic), QDialog(parent), ui(new Ui::statistic_acts_wnd)
{
    ui->setupUi(this);

    ui->date_day->setDate(QDate::currentDate());

    refresh_points();
    refresh_operators();
    refresh_years_months();
    refresh_folders();

    ui->date_1->setDate(QDate::currentDate().addMonths(-1));
    ui->date_2->setDate(QDate::currentDate());

    ui->rb_date_year->setChecked(true);
    ui->rb_date_month->setChecked(false);
    ui->rb_date_day->setChecked(false);
    ui->rb_dates->setChecked(false);

    on_rb_date_year_clicked(true);
    on_rb_date_month_clicked(false);
    on_rb_date_day_clicked(false);
    on_rb_dates_clicked(false);
}

statistic_acts_wnd::~statistic_acts_wnd() {
    delete ui;
}

void statistic_acts_wnd::refresh_points() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка медорганизаций
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select id, '('||point_regnum||')  '||point_name as name "
                  " from public.points "
                  " where status=1 and point_regnum<>'000' "
                  " order by point_regnum ; ";
    mySQL.exec(this, sql, QString("Список ПВП"), *query, true, db, data_app);
    ui->combo_point->clear();
    ui->combo_point->addItem(" - все ПВП - ", -1);
    while (query->next()) {
        ui->combo_point->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_point->setCurrentIndex(0);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void statistic_acts_wnd::refresh_operators() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка медорганизаций
    QSqlQuery *query = new QSqlQuery(db);
    QString sql;
    sql = "select o.id, o.oper_fio "
          " from operators o "
          "      left join points p on(o.id_point=p.id) "
          " where o.status=1 ";
    if (ui->combo_point->currentIndex()>0)
        sql += " and p.status=1 and p.id=" + QString::number(ui->combo_point->currentData().toInt()) + " ";
    sql += " order by o.oper_fio ; ";
    mySQL.exec(this, sql, QString("Список инспекторов"), *query, true, db, data_app);
    ui->combo_operator->clear();
    ui->combo_operator->addItem(" - все операторы - ", -1);
    while (query->next()) {
        ui->combo_operator->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_operator->setCurrentIndex(0);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void statistic_acts_wnd::refresh_folders() {
    this->setCursor(Qt::WaitCursor);
    // обновление списка папок
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select id, folder_name "
                  " from public.folders "
                  " where date_open<='" + QDate::currentDate().toString("yyyy-MM-dd") + "' " +
                      ( ui->ch_folder_closed->isChecked() ? QString(" and status=-1 ") : QString(" and status=1 ") ) +
                  " order by folder_name ; ";
    mySQL.exec(this, sql, QString("Список папок"), *query, true, db, data_app);
    ui->combo_folder->clear();
    ui->combo_folder->addItem(" - без учёта папки - ", 0);
    while (query->next()) {
        ui->combo_folder->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    delete query;
    int n = 0;
    ui->combo_folder->setCurrentIndex(n);
    this->setCursor(Qt::ArrowCursor);
}

void statistic_acts_wnd::on_bn_today_clicked() {
    ui->date_day->setDate(QDate::currentDate());

    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}

void statistic_acts_wnd::on_bn_tomon_clicked() {
    ui->date_month->setCurrentIndex(ui->date_month->count()-1);

    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}

void statistic_acts_wnd::on_ch_newVS_clicked(bool checked) {
    ui->ch_P010->setChecked(checked);
    ui->ch_P034->setChecked(checked);
    ui->ch_P035->setChecked(checked);
    ui->ch_P036->setChecked(checked);

    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}
void statistic_acts_wnd::on_ch_otherPol_clicked(bool checked) {
    ui->ch_P031->setChecked(checked);
    ui->ch_P032->setChecked(checked);
    ui->ch_P033->setChecked(checked);

    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}
void statistic_acts_wnd::on_ch_activate_clicked(bool checked) {
    ui->ch_P060->setChecked(checked);

    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}
void statistic_acts_wnd::on_ch_closePol_clicked(bool checked) {
    ui->ch_P021->setChecked(checked);
    ui->ch_P022->setChecked(checked);
    ui->ch_P023->setChecked(checked);
    ui->ch_P024->setChecked(checked);
    ui->ch_P025->setChecked(checked);

    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}
void statistic_acts_wnd::on_ch_dublicate_clicked(bool checked) {
    ui->ch_P061->setChecked(checked);
    ui->ch_P062->setChecked(checked);
    ui->ch_P063->setChecked(checked);

    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}




void statistic_acts_wnd::on_ch_P010_clicked(bool checked) {
    if ( ui->ch_P010->isChecked() &&
         ui->ch_P034->isChecked() &&
         ui->ch_P035->isChecked() &&
         ui->ch_P036->isChecked() ) {
        ui->ch_newVS->setChecked(true);
    } else {
        ui->ch_newVS->setChecked(false);
    }
    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}
void statistic_acts_wnd::on_ch_P034_clicked(bool checked) {
    if ( ui->ch_P010->isChecked() &&
         ui->ch_P034->isChecked() &&
         ui->ch_P035->isChecked() &&
         ui->ch_P036->isChecked() ) {
        ui->ch_newVS->setChecked(true);
    } else {
        ui->ch_newVS->setChecked(false);
    }
    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}
void statistic_acts_wnd::on_ch_P035_clicked(bool checked) {
    if ( ui->ch_P010->isChecked() &&
         ui->ch_P034->isChecked() &&
         ui->ch_P035->isChecked() &&
         ui->ch_P036->isChecked() ) {
        ui->ch_newVS->setChecked(true);
    } else {
        ui->ch_newVS->setChecked(false);
    }
    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}
void statistic_acts_wnd::on_ch_P036_clicked(bool checked) {
    if ( ui->ch_P010->isChecked() &&
         ui->ch_P034->isChecked() &&
         ui->ch_P035->isChecked() &&
         ui->ch_P036->isChecked() ) {
        ui->ch_newVS->setChecked(true);
    } else {
        ui->ch_newVS->setChecked(false);
    }
    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}


void statistic_acts_wnd::on_ch_P031_clicked(bool checked) {
    if ( ui->ch_P031->isChecked() &&
         ui->ch_P032->isChecked() &&
         ui->ch_P033->isChecked() ) {
        ui->ch_otherPol->setChecked(true);
    } else {
        ui->ch_otherPol->setChecked(false);
    }
    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}
void statistic_acts_wnd::on_ch_P032_clicked(bool checked) {
    if ( ui->ch_P031->isChecked() &&
         ui->ch_P032->isChecked() &&
         ui->ch_P033->isChecked() ) {
        ui->ch_otherPol->setChecked(true);
    } else {
        ui->ch_otherPol->setChecked(false);
    }
    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}
void statistic_acts_wnd::on_ch_P033_clicked(bool checked) {
    if ( ui->ch_P031->isChecked() &&
         ui->ch_P032->isChecked() &&
         ui->ch_P033->isChecked() ) {
        ui->ch_otherPol->setChecked(true);
    } else {
        ui->ch_otherPol->setChecked(false);
    }
    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}


void statistic_acts_wnd::on_ch_P060_clicked(bool checked) {
    if ( ui->ch_P060->isChecked() ) {
        ui->ch_activate->setChecked(true);
    } else {
        ui->ch_activate->setChecked(false);
    }
    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}



void statistic_acts_wnd::on_ch_P021_clicked(bool checked) {
    if ( ui->ch_P021->isChecked() &&
         ui->ch_P022->isChecked() &&
         ui->ch_P023->isChecked() &&
         ui->ch_P024->isChecked() &&
         ui->ch_P025->isChecked() ) {
        ui->ch_closePol->setChecked(true);
    } else {
        ui->ch_closePol->setChecked(false);
    }
    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}
void statistic_acts_wnd::on_ch_P022_clicked(bool checked) {
    if ( ui->ch_P021->isChecked() &&
         ui->ch_P022->isChecked() &&
         ui->ch_P023->isChecked() &&
         ui->ch_P024->isChecked() &&
         ui->ch_P025->isChecked() ) {
        ui->ch_closePol->setChecked(true);
    } else {
        ui->ch_closePol->setChecked(false);
    }
    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}
void statistic_acts_wnd::on_ch_P023_clicked(bool checked) {
    if ( ui->ch_P021->isChecked() &&
         ui->ch_P022->isChecked() &&
         ui->ch_P023->isChecked() &&
         ui->ch_P024->isChecked() &&
         ui->ch_P025->isChecked() ) {
        ui->ch_closePol->setChecked(true);
    } else {
        ui->ch_closePol->setChecked(false);
    }
    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}
void statistic_acts_wnd::on_ch_P024_clicked(bool checked) {
    if ( ui->ch_P021->isChecked() &&
         ui->ch_P022->isChecked() &&
         ui->ch_P023->isChecked() &&
         ui->ch_P024->isChecked() &&
         ui->ch_P025->isChecked() ) {
        ui->ch_closePol->setChecked(true);
    } else {
        ui->ch_closePol->setChecked(false);
    }
    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}
void statistic_acts_wnd::on_ch_P025_clicked(bool checked) {
    if ( ui->ch_P021->isChecked() &&
         ui->ch_P022->isChecked() &&
         ui->ch_P023->isChecked() &&
         ui->ch_P024->isChecked() &&
         ui->ch_P025->isChecked() ) {
        ui->ch_closePol->setChecked(true);
    } else {
        ui->ch_closePol->setChecked(false);
    }
    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}



void statistic_acts_wnd::on_ch_P061_clicked(bool checked) {
    if ( ui->ch_P061->isChecked() &&
         ui->ch_P062->isChecked() &&
         ui->ch_P063->isChecked() ) {
        ui->ch_dublicate->setChecked(true);
    } else {
        ui->ch_dublicate->setChecked(false);
    }
    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}
void statistic_acts_wnd::on_ch_P062_clicked(bool checked) {
    if ( ui->ch_P061->isChecked() &&
         ui->ch_P062->isChecked() &&
         ui->ch_P063->isChecked() ) {
        ui->ch_dublicate->setChecked(true);
    } else {
        ui->ch_dublicate->setChecked(false);
    }
    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}
void statistic_acts_wnd::on_ch_P063_clicked(bool checked) {
    if ( ui->ch_P061->isChecked() &&
         ui->ch_P062->isChecked() &&
         ui->ch_P063->isChecked() ) {
        ui->ch_dublicate->setChecked(true);
    } else {
        ui->ch_dublicate->setChecked(false);
    }
    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}


void statistic_acts_wnd::on_rb_date_year_clicked(bool checked) {
    ui->pan_month->setEnabled(ui->rb_date_month->isChecked());
    ui->pan_year->setEnabled(ui->rb_date_year->isChecked());
    ui->pan_day->setEnabled(ui->rb_date_day->isChecked());
    ui->pan_dates->setEnabled(ui->rb_dates->isChecked());

    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}
void statistic_acts_wnd::on_rb_date_month_clicked(bool checked) {
    ui->pan_month->setEnabled(ui->rb_date_month->isChecked());
    ui->pan_year->setEnabled(ui->rb_date_year->isChecked());
    ui->pan_day->setEnabled(ui->rb_date_day->isChecked());
    ui->pan_dates->setEnabled(ui->rb_dates->isChecked());

    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}

void statistic_acts_wnd::on_rb_date_day_clicked(bool checked) {
    ui->pan_month->setEnabled(ui->rb_date_month->isChecked());
    ui->pan_year->setEnabled(ui->rb_date_year->isChecked());
    ui->pan_day->setEnabled(ui->rb_date_day->isChecked());
    ui->pan_dates->setEnabled(ui->rb_dates->isChecked());

    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}

void statistic_acts_wnd::on_rb_dates_clicked(bool checked) {
    ui->pan_month->setEnabled(ui->rb_date_month->isChecked());
    ui->pan_year->setEnabled(ui->rb_date_year->isChecked());
    ui->pan_day->setEnabled(ui->rb_date_day->isChecked());
    ui->pan_dates->setEnabled(ui->rb_dates->isChecked());

    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}



void statistic_acts_wnd::on_date_1_dateChanged(const QDate &date) {
    if (ui->date_2->date()<ui->date_1->date()) {
        ui->date_2->setDate(ui->date_1->date());
    }
    ui->lab_days->setText(QString::number(ui->date_1->date().daysTo(ui->date_2->date())));

    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}

void statistic_acts_wnd::on_date_2_dateChanged(const QDate &date) {
    if (ui->date_2->date()<ui->date_1->date()) {
        ui->date_2->setDate(ui->date_1->date());
    }
    ui->lab_days->setText(QString::number(ui->date_1->date().daysTo(ui->date_2->date())));

    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}

void statistic_acts_wnd::on_combo_point_currentIndexChanged(int index) {
    refresh_operators();

    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}




void statistic_acts_wnd::refresh_years_months() {
    this->setCursor(Qt::WaitCursor);
    // обновление списка папок
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select min(dt_ins) as date_min, max(dt_ins) as date_max "
                  " from public.persons ; ";
    mySQL.exec(this, sql, QString("Диапазон дат страхования персон"), *query, true, db, data_app);

    ui->date_year->clear();
    ui->date_month->clear();
    query->next();
    QDate date_min = query->value(0).toDate();
    QDate date_max = query->value(1).toDate();
    for (int i=date_min.year(); i<=date_max.year(); i++){
        // годы
        ui->date_year->addItem(QString::number(i) + " г.", QDate(i,1,1));

        // месяцы
        if ( (i==date_min.year() && date_min.month()<=1) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=1) ) {
            ui->date_month->addItem(QString::number(i) + ", январь", QDate(i,1,1));
        }
        if ( (i==date_min.year() && date_min.month()<=2) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=2) ) {
            ui->date_month->addItem(QString::number(i) + ", февраль", QDate(i,2,1));
        }
        if ( (i==date_min.year() && date_min.month()<=3) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=3) ) {
            ui->date_month->addItem(QString::number(i) + ", март", QDate(i,3,1));
        }
        if ( (i==date_min.year() && date_min.month()<=4) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=4) ) {
            ui->date_month->addItem(QString::number(i) + ", апрель", QDate(i,4,1));
        }
        if ( (i==date_min.year() && date_min.month()<=5) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=5) ) {
            ui->date_month->addItem(QString::number(i) + ", май", QDate(i,5,1));
        }
        if ( (i==date_min.year() && date_min.month()<=6) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=6) ) {
            ui->date_month->addItem(QString::number(i) + ", июнь", QDate(i,6,1));
        }
        if ( (i==date_min.year() && date_min.month()<=7) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=7) ) {
            ui->date_month->addItem(QString::number(i) + ", июль", QDate(i,7,1));
        }
        if ( (i==date_min.year() && date_min.month()<=8) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=8) ) {
            ui->date_month->addItem(QString::number(i) + ", август", QDate(i,8,1));
        }
        if ( (i==date_min.year() && date_min.month()<=9) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=9) ) {
            ui->date_month->addItem(QString::number(i) + ", сентябрь", QDate(i,9,1));
        }
        if ( (i==date_min.year() && date_min.month()<=10) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=10) ) {
            ui->date_month->addItem(QString::number(i) + ", октябрь", QDate(i,10,1));
        }
        if ( (i==date_min.year() && date_min.month()<=11) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=11) ) {
            ui->date_month->addItem(QString::number(i) + ", ноябрь", QDate(i,11,1));
        }
        if ( (i==date_min.year() && date_min.month()<=12) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=12) ) {
            ui->date_month->addItem(QString::number(i) + ", декабрь", QDate(i,12,1));
        }
    }
    ui->date_year->setCurrentIndex(ui->date_year->count()-1);
    ui->date_month->setCurrentIndex(ui->date_month->count()-1);

    this->setCursor(Qt::ArrowCursor);
}



void statistic_acts_wnd::on_ch_folder_closed_clicked(bool checked) {
    refresh_folders();

    ui->tab_statistic->setEnabled(false);
    ui->lab_cnt->setEnabled(false);
    ui->bn_toCSV->setEnabled(false);
}

void statistic_acts_wnd::on_bn_recalc_clicked() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    statistic_sql = "";

    statistic_sql += "select #DATE_NAME# "
                     "       o.oper_fio, "
                     "       count(*) as cnt "
                     "  from ( select a.id, a.id_polis, a.event_code, a.event_dt, "
                     "                EXTRACT(year FROM a.event_dt) as year, "
                     "                EXTRACT(month FROM a.event_dt) as month, "
                     "                EXTRACT(day FROM a.event_dt) as day "
                     "         from events a "
                     "         where a.event_code in(#eventS#) "
                     "               #DATE_WHERE# "
                     "       ) a "
                     "       left join polises p on(p.id=a.id_polis) "
                     "       left join persons e on(e.id_polis=p.id) "
                     "       left join operators o on(o.id=p._id_first_operator) "
                     "       left join points pt on(pt.id=o.id_point) "
                     "       left join folders f on(f.id=e.id_folder) "
                     " where p.id is not NULL "
                     "   and a.id is not NULL "
                     "   #DATE_WHERE# "
                     "   #POINT_WHERE# "
                     "   #OPER_WHERE# "
                     "   #FOLDER_WHERE# "
                     " group by #DATE_GROUPPING#, o.oper_fio/*, a.event_code*/ "
                     " order by #DATE_GROUPPING#, o.oper_fio/*, a.event_code*/ ";


    // подготовка  #DATE_NAME#
    QString date_name_str = "";
    if (ui->rb_date_year->isChecked()) {
        if (ui->ch_year_by_months->isChecked()) {
            date_name_str += "     ( ( case a.month "
                             "         when  1 then 'январь ' "
                             "         when  2 then 'февраль ' "
                             "         when  3 then 'март ' "
                             "         when  4 then 'апрель ' "
                             "         when  5 then 'май ' "
                             "         when  6 then 'июнь ' "
                             "         when  7 then 'июль ' "
                             "         when  8 then 'август ' "
                             "         when  9 then 'сентябрь ' "
                             "         when 10 then 'октябрь ' "
                             "         when 11 then 'ноябрь ' "
                             "         when 12 then 'декабрь ' "
                             "         end ) || "
                             "       a.year || ' г.' ) as date, ";
        } else {
            date_name_str += "     '" + ui->date_year->currentText() + "' as date, ";
        }
    } else if (ui->rb_date_month->isChecked()) {
        date_name_str += "     '" + ui->date_month->currentText() + "' as date, ";
    } else if (ui->rb_date_day->isChecked()) {
        date_name_str += "     '" + ui->date_day->date().toString("dd.MM.yyyy") + "' as date, ";
    } else if (ui->rb_dates->isChecked()) {
        date_name_str += "     '" + ui->date_1->date().toString("dd.MM.yyyy") + " - " + ui->date_2->date().toString("dd.MM.yyyy") + "' as date, ";
    };
    statistic_sql = statistic_sql.replace("#DATE_NAME#", date_name_str);


    // подготовка  #DATE_WHERE#
    QString date_where_str = "";
    if (ui->rb_date_year->isChecked()) {
        date_where_str += "and a.event_dt>='" + ui->date_year->currentData().toDate().toString("yyyy-MM-dd") + "' ";
        date_where_str += "and a.event_dt< '" + ui->date_year->currentData().toDate().addYears(1).toString("yyyy-MM-dd") + "' ";
    } else if (ui->rb_date_month->isChecked()) {
        date_where_str += "and a.event_dt>='" + ui->date_month->currentData().toDate().toString("yyyy-MM-dd") + "' ";
        date_where_str += "and a.event_dt< '" + ui->date_month->currentData().toDate().addMonths(1).toString("yyyy-MM-dd") + "' ";
    } else if (ui->rb_date_day->isChecked()) {
        date_where_str += "and a.event_dt='" + ui->date_day->date().toString("yyyy-MM-dd") + "' ";
    } else if (ui->rb_dates->isChecked()) {
        date_where_str += "and a.event_dt>='" + ui->date_1->date().toString("yyyy-MM-dd") + "' ";
        date_where_str += "and a.event_dt< '" + ui->date_2->date().toString("yyyy-MM-dd") + "' ";
    };
    statistic_sql = statistic_sql.replace("#DATE_WHERE#", date_where_str);


    // подготовка  #POINT_WHERE#
    QString point_where_str = "";
    if (ui->combo_point->currentIndex()>0) {
        point_where_str += "and pt.id=" + QString::number(ui->combo_point->currentData().toInt()) + " ";
    };
    statistic_sql = statistic_sql.replace("#POINT_WHERE#", point_where_str);


    // подготовка  #OPER_WHERE#
    QString oper_where_str = "";
    if (ui->combo_operator->currentIndex()>0) {
        oper_where_str += "and o.id=" + QString::number(ui->combo_operator->currentData().toInt()) + " ";
    };
    statistic_sql = statistic_sql.replace("#OPER_WHERE#", oper_where_str);


    // подготовка  #FOLDER_WHERE#
    QString folder_where_str = "";
    if (ui->combo_folder->currentIndex()>0) {
        folder_where_str += "and f.id=" + QString::number(ui->combo_folder->currentData().toInt()) + " ";
    };
    statistic_sql = statistic_sql.replace("#FOLDER_WHERE#", folder_where_str);


    // подготовка  #eventS#
    QString events_str = "'TEST_XXX'";
    if (ui->ch_P010->isChecked())
        events_str += ", 'П010'";
    if (ui->ch_P021->isChecked())
        events_str += ", 'П021'";
    if (ui->ch_P022->isChecked())
        events_str += ", 'П022'";
    if (ui->ch_P023->isChecked())
        events_str += ", 'П023'";
    if (ui->ch_P024->isChecked())
        events_str += ", 'П024'";
    if (ui->ch_P025->isChecked())
        events_str += ", 'П025'";
    if (ui->ch_P031->isChecked())
        events_str += ", 'П031'";
    if (ui->ch_P032->isChecked())
        events_str += ", 'П032'";
    if (ui->ch_P033->isChecked())
        events_str += ", 'П033'";
    if (ui->ch_P034->isChecked())
        events_str += ", 'П034'";
    if (ui->ch_P035->isChecked())
        events_str += ", 'П035'";
    if (ui->ch_P036->isChecked())
        events_str += ", 'П036'";
    if (ui->ch_P060->isChecked())
        events_str += ", 'П060'";
    if (ui->ch_P061->isChecked())
        events_str += ", 'П061'";
    if (ui->ch_P062->isChecked())
        events_str += ", 'П062'";
    if (ui->ch_P063->isChecked())
        events_str += ", 'П063'";
    statistic_sql = statistic_sql.replace("#eventS#", events_str);


    QString date_grouping = "";
    if ( ui->rb_date_year->isChecked() &&
         !ui->ch_year_by_months->isChecked() )
        date_grouping = "a.year";
    else if ( ui->rb_date_month->isChecked() ||
              ( ui->rb_date_year->isChecked() &&
                ui->ch_year_by_months->isChecked()) )
        date_grouping = "a.year, a.month";
    else if ( ui->rb_date_day->isChecked() )
        date_grouping = "a.year, a.month, a.day";
    else date_grouping = " 1=1 ";
    statistic_sql = statistic_sql.replace("#DATE_GROUPPING#", date_grouping);



    model_statistic.setQuery(statistic_sql,db);
    QString err2 = model_statistic.lastError().driverText();

    // подключаем модель из БД
    ui->tab_statistic->setModel(&model_statistic);

    // обновляем таблицу
    ui->tab_statistic->reset();

    // задаём ширину колонок
    ui->tab_statistic->setColumnWidth( 0,150);     // id,
    ui->tab_statistic->setColumnWidth( 1,250);     // ,
    ui->tab_statistic->setColumnWidth( 2, 50);     // ,

    // правим заголовки
    model_statistic.setHeaderData( 0, Qt::Horizontal, ("дата"));
    model_statistic.setHeaderData( 1, Qt::Horizontal, ("инспектор"));
    model_statistic.setHeaderData( 2, Qt::Horizontal, ("кол-во"));

    ui->tab_statistic->repaint();

    // подсчитаем общее количество событий
    db.transaction();
    QString sql_cnt = "select sum(cnt) from ( " + statistic_sql + " ) q ; ";

    QSqlQuery *query_cnt = new QSqlQuery(db);
    if (!mySQL.exec(this, sql_cnt, "Подсчёт числа событий за период ", *query_cnt, true, db, data_app)) {
        delete query_cnt;
        QMessageBox::warning(this, "Операция отменена",
                             "При подсчёте числа событий за период произошла ошибка. "
                             "\nОперация отменена.");
        db.rollback();
        return;
    }
    query_cnt->next();
    int cnt = query_cnt->value(0).toInt();
    ui->lab_cnt->setText(QString::number(cnt));

    this->setCursor(Qt::ArrowCursor);

    ui->tab_statistic->setEnabled(true);
    ui->lab_cnt->setEnabled(true);
    ui->bn_toCSV->setEnabled(true);
}

#include "qt_windows.h"
#include "qwindowdefs_win.h"
#include <shellapi.h>

void statistic_acts_wnd::on_bn_toCSV_clicked() {
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
    int cnt_head = model_statistic.columnCount();
    for (int i=0; i<cnt_head; i++) {
        if (ui->tab_statistic->columnWidth(i)>1) {
            head += model_statistic.headerData(i,Qt::Horizontal).toString().replace("\n"," ");
            if (i<cnt_head-1)  head += ";";
        }
    }
    stream << head << "\n";

    // переберём полученные данные и сохраним в файл
    QSqlQuery *query_export = new QSqlQuery(db);
    query_export->exec(statistic_sql);
    while (query_export->next()) {
        for (int j=0; j<cnt_head; j++) {
            if (ui->tab_statistic->columnWidth(j)>1) {
                QString value = query_export->value(j).toString().trimmed().replace(";",",");
                if (!value.isEmpty()) {
                    stream << value;
                }
                if (j<cnt_head-1)  stream << ";";
            }
        }
        stream << "\n";
    }
    stream << "\n;" << QString("ВСЕГО: ") << ";" << ui->lab_cnt->text();

    /*int cnt_rows = model_statistic.rowCount();
    for (int i=0; i<cnt_rows; i++) {
        for (int j=0; j<cnt_head; j++) {
            if (ui->tab_statistic->columnWidth(j)>1) {
                // данные застрахованного
                QString s = model_statistic.data(model_statistic.index(i, j), Qt::EditRole).toString().replace("\n"," ");
                stream << s;
                if (j<cnt_head-1)  stream << ";";
            }
        }
        stream << "\n";
    }*/
    file.close();

    // ===========================
    // собственно открытие шаблона
    // ===========================
    // открытие полученного ODT
    /*long result = (long)*/ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_s.utf16()), 0, 0, SW_NORMAL);
}

void statistic_acts_wnd::on_bn_close_clicked() {
    close();
}
