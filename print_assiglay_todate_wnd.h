#ifndef PRINT_ASSIGLAY_TODATE_WND_H
#define PRINT_ASSIGLAY_TODATE_WND_H

#include <QDialog>

#include "show_tab_wnd.h"

#include "s_data_app.h"
#include "ki_exec_query.h"

namespace Ui {
class print_assiglay_todate_wnd;
}

class print_assiglay_todate_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    QSqlDatabase db_ODBC;    // это - ODBC-окошко для генерации dbf
    exec_SQL mySQL;

    show_tab_wnd *show_tab_w;

    QSettings &settings;
    s_data_app &data_app;

    bool f_today;
    QDate date_assiglay;

public:
    explicit print_assiglay_todate_wnd(bool f_today, QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~print_assiglay_todate_wnd();

private slots:
    void on_bn_cansel_clicked();

    void on_bn_today_clicked();
    void on_bn_1st_day_clicked();
    void on_date_assiglay_editingFinished();
    void on_ch_bymo_clicked(bool checked);

    void on_bn_save_csv_clicked();
    void on_bn_save_dbf_clicked();
    void on_bn_save_acts_clicked();

    void on_bn_save_csv_softrust_clicked();

private:
    Ui::print_assiglay_todate_wnd *ui;
};

#endif // PRINT_ASSIGLAY_TODATE_WND_H
