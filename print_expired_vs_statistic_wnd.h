#ifndef print_expired_vs_statistic_wnd_H
#define print_expired_vs_statistic_wnd_H

#include <QDialog>
#include <QtSql>
#include <QTime>
#include "show_tab_wnd.h"

#include "ki_exec_query.h"
#include "s_data_app.h"

typedef struct {
    int rep_rep_type;   // 1 - отчёт по ВС, 2 - отчёт по единым полисам
    int rep_pol_type;   // 1 - число полисов, 2 - полный список полисов
    int rep_out_time;   // 1 - истёкшие полисы, 2 - выданные полисв
    QDate date_d;
    bool f_points;
    int id_point;
    QString point_text;
    bool f_years;
    QDate year_d;
    QString year_text;
    bool f_months;
    QDate month_d;
    QString month_text;

    //bool f_delivery;
    //bool f_undeliv_enp;
} s_data_eVStat;

namespace Ui {
class print_expired_vs_statistic_wnd;
}

class print_expired_vs_statistic_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;
    s_data_eVStat &data_eVStat;

    show_tab_wnd *show_tab_w;

    void refresh_points();
    void refresh_years_mons();

public:
    explicit print_expired_vs_statistic_wnd(QSqlDatabase &db, s_data_eVStat &data_eVStat, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~print_expired_vs_statistic_wnd();

private slots:
    void on_bn_cansel_clicked();
    void on_bn_ok_clicked();    
    void on_bn_today_clicked();
    void on_date_date_dateChanged(const QDate &date);

    void on_ch_by_date_clicked(bool checked);
    void on_ch_by_points_clicked(bool checked);
    void on_ch_by_years_clicked(bool checked);
    void on_ch_by_months_clicked(bool checked);

    void on_rb_list_clicked(bool checked);

    void on_rb_count_clicked(bool checked);

private:
    Ui::print_expired_vs_statistic_wnd *ui;
};

#endif // print_expired_vs_statistic_wnd_H
