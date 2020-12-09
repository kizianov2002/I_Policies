#include "get_date_wnd.h"
#include "ui_get_date_wnd.h"

#include <QtSql>

get_date_wnd::get_date_wnd(QString header_s, QString message_s, bool interval_f, bool one_day_f, bool side_from_f, bool side_to_f, QDate &date1, QDate &date2, QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    db(db),
    data_app(data_app),
    settings(settings),
    date1(date1),
    date2(date2),
    one_day_f(one_day_f),
    interval_f(interval_f),
    side_from_f(side_from_f),
    side_to_f(side_to_f),
    message_s(message_s),
    header_s(header_s),
    QDialog(parent),
    ui(new Ui::get_date_wnd)
{
    ui->setupUi(this);

    this->setWindowTitle(header_s);
    ui->lab_message->setText(message_s);
    ui->date_date->setDate(date1);
    refresh_years_mons();
    ui->date_from->setDate(date1);
    ui->date_to->setDate(date2);

    this->setMinimumHeight(-1);

    if (one_day_f && interval_f) {
        ui->rb_date->setVisible(true);
        ui->rb_month->setVisible(true);
        ui->rb_year->setVisible(true);
        ui->rb_dates->setVisible(true);

        ui->date_date->setVisible(true);
        ui->bn_tomorrow->setVisible(true);
        ui->combo_month->setVisible(true);
        ui->combo_year->setVisible(true);
        ui->date_from->setVisible(true);
        ui->date_to->setVisible(true);
        ui->lab_to->setVisible(true);

        ui->lab_side_from->setVisible(true);
        ui->lab_side_to->setVisible(true);

        ui->rb_dates->setChecked(false);
        ui->rb_date->setChecked(true);

    } else if (one_day_f) {
        ui->rb_date->setVisible(true);
        ui->rb_month->setVisible(false);
        ui->rb_year->setVisible(false);
        ui->rb_dates->setVisible(false);

        ui->date_date->setVisible(true);
        ui->bn_tomorrow->setVisible(true);
        ui->combo_month->setVisible(false);
        ui->combo_year->setVisible(false);
        ui->date_from->setVisible(false);
        ui->date_to->setVisible(false);
        ui->lab_to->setVisible(false);

        ui->lab_side_from->setVisible(false);
        ui->lab_side_to->setVisible(false);

        ui->rb_dates->setChecked(false);
        ui->rb_date->setChecked(true);

    } else if (interval_f) {
        ui->rb_date->setVisible(false);
        ui->rb_month->setVisible(true);
        ui->rb_year->setVisible(true);
        ui->rb_dates->setVisible(true);

        ui->date_date->setVisible(false);
        ui->bn_tomorrow->setVisible(false);        
        ui->combo_month->setVisible(true);
        ui->combo_year->setVisible(true);
        ui->date_from->setEnabled(true);
        ui->date_to->setEnabled(true);
        ui->lab_to->setEnabled(true);

        ui->lab_side_from->setVisible(true);
        ui->lab_side_to->setVisible(true);

        ui->rb_date->setChecked(false);
        ui->rb_dates->setChecked(true);
    }

    if (interval_f) {
        ui->lab_side_from->setVisible(true);
        ui->lab_side_to->setVisible(true);

        if (side_from_f)
            ui->lab_side_from->setText("( включая )");
        else ui->lab_side_from->setText("( не включая )");

        if (side_to_f)
            ui->lab_side_to->setText("( включая )");
        else ui->lab_side_to->setText("( не включая )");

    } else {
        ui->lab_side_from->setVisible(false);
        ui->lab_side_to->setVisible(false);
    }
}

get_date_wnd::~get_date_wnd() {
    delete ui;
}

void get_date_wnd::refresh_years_mons() {
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select min(dt_ins) as date_min, max(dt_ins) as date_max "
                  "  from public.persons ; ";
    mySQL.exec(this, sql, QString("Диапазон дат страхования персон"), *query, true, db, data_app);

    ui->combo_year->clear();
    ui->combo_month->clear();
    query->next();
    QDate date_min = query->value(0).toDate();
    QDate date_max = query->value(1).toDate();

    for (int i=QDate::currentDate().year(); i>=date_min.year(); i--){
        // годы
        ui->combo_year->addItem(QString::number(i) + " г.", QDate(i,1,1));

        // месяцы
        if (date_min.year()==date_max.year()) {
            if ( (date_min.month()<=12 && date_max.month()>=12 ) )
                ui->combo_month->addItem(QString::number(i) + ",  декабрь", QDate(i,12,1));
            if ( (date_min.month()<=11 && date_max.month()>=11 ) )
                ui->combo_month->addItem(QString::number(i) + ",  ноябрь", QDate(i,11,1));
            if ( (date_min.month()<=10 && date_max.month()>=10 ) )
                ui->combo_month->addItem(QString::number(i) + ",  октябрь", QDate(i,10,1));
            if ( (date_min.month()<= 9 && date_max.month()>= 9 ) )
                ui->combo_month->addItem(QString::number(i) + ",  сентябрь", QDate(i,9,1));
            if ( (date_min.month()<= 8 && date_max.month()>= 8 ) )
                ui->combo_month->addItem(QString::number(i) + ",  август", QDate(i,8,1));
            if ( (date_min.month()<= 7 && date_max.month()>= 7 ) )
                ui->combo_month->addItem(QString::number(i) + ",  июль", QDate(i,7,1));
            if ( (date_min.month()<= 6 && date_max.month()>= 6 ) )
                ui->combo_month->addItem(QString::number(i) + ",  июнь", QDate(i,6,1));
            if ( (date_min.month()<= 5 && date_max.month()>= 5 ) )
                ui->combo_month->addItem(QString::number(i) + ",  май", QDate(i,5,1));
            if ( (date_min.month()<= 4 && date_max.month()>= 4 ) )
                ui->combo_month->addItem(QString::number(i) + ",  апрель", QDate(i,4,1));
            if ( (date_min.month()<= 3 && date_max.month()>= 3 ) )
                ui->combo_month->addItem(QString::number(i) + ",  март", QDate(i,3,1));
            if ( (date_min.month()<= 2 && date_max.month()>= 2 ) )
                ui->combo_month->addItem(QString::number(i) + ",  февраль", QDate(i,2,1));
            if ( (date_min.month()<= 1 && date_max.month()>= 1 ) )
                ui->combo_month->addItem(QString::number(i) + ", январь", QDate(i,1,1));

        } else {
            if ( (i==date_min.year() && date_min.month()<=12) ||
                 (i>date_min.year()  && i<date_max.year() )  ||
                 (i==date_max.year() && date_max.month()>=12) ) {
                ui->combo_month->addItem(QString::number(i) + ",  декабрь", QDate(i,12,1));
            }
            if ( (i==date_min.year() && date_min.month()<=11) ||
                 (i>date_min.year()  && i<date_max.year() )  ||
                 (i==date_max.year() && date_max.month()>=11) ) {
                ui->combo_month->addItem(QString::number(i) + ",  ноябрь", QDate(i,11,1));
            }
            if ( (i==date_min.year() && date_min.month()<=10) ||
                 (i>date_min.year()  && i<date_max.year() )  ||
                 (i==date_max.year() && date_max.month()>=10) ) {
                ui->combo_month->addItem(QString::number(i) + ",  октябрь", QDate(i,10,1));
            }
            if ( (i==date_min.year() && date_min.month()<=9) ||
                 (i>date_min.year()  && i<date_max.year() )  ||
                 (i==date_max.year() && date_max.month()>=9) ) {
                ui->combo_month->addItem(QString::number(i) + ",  сентябрь", QDate(i,9,1));
            }
            if ( (i==date_min.year() && date_min.month()<=8) ||
                 (i>date_min.year()  && i<date_max.year() )  ||
                 (i==date_max.year() && date_max.month()>=8) ) {
                ui->combo_month->addItem(QString::number(i) + ",  август", QDate(i,8,1));
            }
            if ( (i==date_min.year() && date_min.month()<=7) ||
                 (i>date_min.year()  && i<date_max.year() )  ||
                 (i==date_max.year() && date_max.month()>=7) ) {
                ui->combo_month->addItem(QString::number(i) + ",  июль", QDate(i,7,1));
            }
            if ( (i==date_min.year() && date_min.month()<=6) ||
                 (i>date_min.year()  && i<date_max.year() )  ||
                 (i==date_max.year() && date_max.month()>=6) ) {
                ui->combo_month->addItem(QString::number(i) + ",  июнь", QDate(i,6,1));
            }
            if ( (i==date_min.year() && date_min.month()<=5) ||
                 (i>date_min.year()  && i<date_max.year() )  ||
                 (i==date_max.year() && date_max.month()>=5) ) {
                ui->combo_month->addItem(QString::number(i) + ",  май", QDate(i,5,1));
            }
            if ( (i==date_min.year() && date_min.month()<=4) ||
                 (i>date_min.year()  && i<date_max.year() )  ||
                 (i==date_max.year() && date_max.month()>=4) ) {
                ui->combo_month->addItem(QString::number(i) + ",  апрель", QDate(i,4,1));
            }
            if ( (i==date_min.year() && date_min.month()<=3) ||
                 (i>date_min.year()  && i<date_max.year() )  ||
                 (i==date_max.year() && date_max.month()>=3) ) {
                ui->combo_month->addItem(QString::number(i) + ",  март", QDate(i,3,1));
            }
            if ( (i==date_min.year() && date_min.month()<=2) ||
                 (i>date_min.year()  && i<date_max.year() )  ||
                 (i==date_max.year() && date_max.month()>=2) ) {
                ui->combo_month->addItem(QString::number(i) + ",  февраль", QDate(i,2,1));
            }
            if ( (i==date_min.year() && date_min.month()<=1) ||
                 (i>date_min.year()  && i<date_max.year() )  ||
                 (i==date_max.year() && date_max.month()>=1) ) {
                ui->combo_month->addItem(QString::number(i) + ", январь", QDate(i,1,1));
            }
        }
    }

}

void get_date_wnd::on_bn_cansel_clicked() {
    reject();
}

void get_date_wnd::on_bn_save_clicked() {
    if (ui->rb_date->isChecked()) {
        date1 = ui->date_date->date();
        date2 = ui->date_date->date();
    } else if (ui->rb_dates->isChecked()) {
        date1 = ui->date_from->date();
        date2 = ui->date_to->date();
    } else if (ui->rb_month->isChecked()) {
        date1 = date2 = ui->combo_month->currentData().toDate();
        date2 = date2.addMonths(1);
        // учтём вхождение концов
        if (!side_from_f)  date1 = date1.addDays(-1);
        if (side_to_f)     date2 = date2.addDays(-1);
    } else if (ui->rb_year->isChecked()) {
        date1 = date2 = ui->combo_year->currentData().toDate();
        date2 = date2.addYears(1);
        // учтём вхождение концов
        if (!side_from_f)  date1 = date1.addDays(-1);
        if (side_to_f)     date2 = date2.addDays(-1);
    } else {
        date1 = QDate::currentDate();
        date2 = QDate::currentDate();
    }
    accept();
}

void get_date_wnd::on_rb_date_clicked(bool checked) {
    if (checked) {
        ui->date_date->setEnabled(true);
        ui->bn_tomorrow->setEnabled(true);
        ui->combo_month->setEnabled(false);
        ui->combo_year->setEnabled(false);
        ui->date_from->setEnabled(false);
        ui->date_to->setEnabled(false);
    }
}

void get_date_wnd::on_rb_month_clicked(bool checked) {
    if (checked) {
        ui->date_date->setEnabled(false);
        ui->bn_tomorrow->setEnabled(false);
        ui->combo_month->setEnabled(true);
        ui->combo_year->setEnabled(false);
        ui->date_from->setEnabled(false);
        ui->date_to->setEnabled(false);
    }
}

void get_date_wnd::on_rb_year_clicked(bool checked) {
    if (checked) {
        ui->date_date->setEnabled(false);
        ui->bn_tomorrow->setEnabled(false);
        ui->combo_month->setEnabled(false);
        ui->combo_year->setEnabled(true);
        ui->date_from->setEnabled(false);
        ui->date_to->setEnabled(false);
    }
}

void get_date_wnd::on_rb_dates_clicked(bool checked) {
    if (checked) {
        ui->date_date->setEnabled(false);
        ui->bn_tomorrow->setEnabled(false);
        ui->combo_month->setEnabled(false);
        ui->combo_year->setEnabled(false);
        ui->date_from->setEnabled(true);
        ui->date_to->setEnabled(true);
    }
}

void get_date_wnd::on_date_from_dateChanged(const QDate &date) {
    if (date>ui->date_to->date()) {
        if      ( side_from_f &&  side_to_f)  ui->date_to->setDate(date);
        else if ( side_from_f ||  side_to_f)  ui->date_to->setDate(date.addDays(1));
        else if (!side_from_f && !side_to_f)  ui->date_to->setDate(date.addDays(2));
    }
}

void get_date_wnd::on_date_to_dateChanged(const QDate &date) {
    if (date<ui->date_from->date()) {
        if      ( side_from_f &&  side_to_f)  ui->date_from->setDate(date);
        else if ( side_from_f ||  side_to_f)  ui->date_from->setDate(date.addDays(-1));
        else if (!side_from_f && !side_to_f)  ui->date_from->setDate(date.addDays(-2));
        //ui->date_from->setDate(date);
    }
}

void get_date_wnd::on_bn_tomorrow_clicked() {
    ui->date_date->setDate(QDate::currentDate().addDays(-1));
}
