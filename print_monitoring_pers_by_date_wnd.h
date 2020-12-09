#ifndef print_monitoring_pers_by_date_wnd_H
#define print_monitoring_pers_by_date_wnd_H

#include <QDialog>

#include "s_data_app.h"

#include "ki_exec_query.h"

namespace Ui {
class print_monitoring_pers_by_date_wnd;
}

class print_monitoring_pers_by_date_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;

    QString month_to_str(QDate date);

public:
    explicit print_monitoring_pers_by_date_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~print_monitoring_pers_by_date_wnd();

private slots:
    void on_bn_cansel_clicked();

    void on_bn_print_clicked();

    void on_bn_edit_clicked(bool checked);

    void on_bn_date_old_clicked();

    void on_bn_date_new_clicked();

    void on_ln_smo_oms_chif_textChanged(const QString &arg1);

    void on_bn_date_old_mon_clicked();

private:
    Ui::print_monitoring_pers_by_date_wnd *ui;
};

#endif // print_monitoring_pers_by_date_wnd_H
