#ifndef print_pol_monitoring_wnd_H
#define print_pol_monitoring_wnd_H

#include <QDialog>

#include "s_data_app.h"
#include "ki_exec_query.h"

namespace Ui {
class print_pol_monitoring_wnd;
}

class print_pol_monitoring_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;

    QString month_to_str(QDate date);
    void set_state();

public:
    explicit print_pol_monitoring_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~print_pol_monitoring_wnd();

private slots:
    void on_bn_cansel_clicked();

    void on_bn_print_clicked();

    void on_bn_edit_clicked(bool checked);

    void on_bn_date_rep_clicked();

    void on_bn_date_rep_2_clicked();

    void on_bn_date_rep_mon_clicked();

    void on_ln_filial_director_textChanged(const QString &arg1);

    void on_ln_filial_director_2_textChanged(const QString &arg1);

    void on_ln_filial_director_fio_textChanged(const QString &arg1);

    void on_ch_date2_clicked(bool checked);

private:
    Ui::print_pol_monitoring_wnd *ui;
};

#endif // print_pol_monitoring_wnd_H
