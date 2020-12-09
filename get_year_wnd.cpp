#include "get_year_wnd.h"
#include "ui_get_year_wnd.h"

#include <QtSql>

get_year_wnd::get_year_wnd(QString header_s, QString message_s, bool only_year_f, bool future_f, int year, int quarter, QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    db(db),
    data_app(data_app),
    settings(settings),
    year(year),
    quarter(quarter),
    only_year_f(only_year_f),
    future_f(future_f),
    message_s(message_s),
    header_s(header_s),
    QDialog(parent),
    ui(new Ui::get_year_wnd)
{
    ui->setupUi(this);

    this->setWindowTitle(header_s);
    ui->lab_message->setText(message_s);

    year = QDate::currentDate().year();
    quarter = (QDate::currentDate().month()-1)/3 +1;

    refresh_years_quarts();
    ui->combo_year->setCurrentIndex(ui->combo_year->findData(year));
    ui->combo_quarter->setCurrentIndex(ui->combo_quarter->findData(QDate(year, quarter, 1)));

    if (only_year_f) {
        ui->rb_quarter->setVisible(false);
        ui->combo_quarter->setVisible(false);
    }
}

get_year_wnd::~get_year_wnd() {
    delete ui;
}

void get_year_wnd::refresh_years_quarts() {
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select min(dt_ins) as date_min, max(dt_ins) as date_max "
                  "  from public.persons ; ";
    mySQL.exec(this, sql, QString("Диапазон дат страхования персон"), *query, true, db, data_app);

    ui->combo_year->clear();
    ui->combo_quarter->clear();
    query->next();
    QDate date_min = query->value(0).toDate();
    QDate date_max = query->value(1).toDate();
    if (future_f)
        date_max = QDate::currentDate().addYears(2);

    for (int i=QDate::currentDate().year()+1; i>=date_min.year(); i--){
        // годы
        ui->combo_year->addItem(QString::number(i) + " г.", i);

        // кварталы
        ui->combo_quarter->addItem(QString::number(i) + " г., 1 квартал", QDate(i, 1, 1));
        ui->combo_quarter->addItem(QString::number(i) + " г.,  2 квартал", QDate(i, 2, 1));
        ui->combo_quarter->addItem(QString::number(i) + " г.,  3 квартал", QDate(i, 3, 1));
        ui->combo_quarter->addItem(QString::number(i) + " г.,  4 квартал", QDate(i, 4, 1));
    }
}

void get_year_wnd::on_bn_cansel_clicked() {
    reject();
}

void get_year_wnd::on_bn_save_clicked() {
    if (ui->rb_year->isChecked()) {
        year = ui->combo_year->currentData().toInt();
        quarter = -1;
    } else {
        year = ui->combo_quarter->currentData().toDate().year();
        quarter = ui->combo_quarter->currentData().toDate().month();
    }
    accept();
}

void get_year_wnd::on_rb_year_clicked(bool checked) {
    ui->combo_year->setEnabled(checked);
    ui->combo_quarter->setEnabled(!checked);
}

void get_year_wnd::on_rb_quarter_clicked(bool checked) {
    ui->combo_year->setEnabled(!checked);
    ui->combo_quarter->setEnabled(checked);
}
