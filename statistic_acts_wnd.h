#ifndef STATISTIC_ACTS_WND_H
#define STATISTIC_ACTS_WND_H

#include <QDialog>
#include <QtSql>

#include "s_data_app.h"

namespace Ui {
class statistic_acts_wnd;
}

#include "ki_exec_query.h"

typedef struct {
    int id;
    QString name;
} s_data_statistic;

class statistic_acts_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    QSqlQueryModel model_statistic;
    exec_SQL mySQL;

    s_data_app &data_app;
    s_data_statistic &data_statistic;

    QString statistic_sql;

    void refresh_points();
    void refresh_operators();
    void refresh_folders();
    void refresh_years_months();

public:
    explicit statistic_acts_wnd(QSqlDatabase &db, s_data_app &data_app, s_data_statistic &data_statistic, QWidget *parent = 0);
    ~statistic_acts_wnd();

private slots:
    void on_bn_today_clicked();
    void on_bn_tomon_clicked();

    void on_ch_newVS_clicked(bool checked);
    void on_ch_otherPol_clicked(bool checked);
    void on_ch_activate_clicked(bool checked);
    void on_ch_closePol_clicked(bool checked);
    void on_ch_dublicate_clicked(bool checked);

    void on_ch_P010_clicked(bool checked);
    void on_ch_P034_clicked(bool checked);
    void on_ch_P035_clicked(bool checked);
    void on_ch_P036_clicked(bool checked);

    void on_ch_P031_clicked(bool checked);
    void on_ch_P032_clicked(bool checked);
    void on_ch_P033_clicked(bool checked);

    void on_ch_P060_clicked(bool checked);

    void on_ch_P021_clicked(bool checked);
    void on_ch_P022_clicked(bool checked);
    void on_ch_P023_clicked(bool checked);
    void on_ch_P024_clicked(bool checked);
    void on_ch_P025_clicked(bool checked);

    void on_ch_P061_clicked(bool checked);
    void on_ch_P062_clicked(bool checked);
    void on_ch_P063_clicked(bool checked);

    void on_rb_date_year_clicked(bool checked);
    void on_rb_date_month_clicked(bool checked);
    void on_rb_date_day_clicked(bool checked);
    void on_rb_dates_clicked(bool checked);

    void on_date_1_dateChanged(const QDate &date);
    void on_date_2_dateChanged(const QDate &date);

    void on_bn_recalc_clicked();


    void on_ch_folder_closed_clicked(bool checked);

    void on_combo_point_currentIndexChanged(int index);

    void on_bn_toCSV_clicked();

    void on_bn_close_clicked();

private:
    Ui::statistic_acts_wnd *ui;
};

#endif // STATISTIC_ACTS_WND_H
