#ifndef print_pays2mo_rep_pt_wnd_H
#define print_pays2mo_rep_pt_wnd_H

#include <QDialog>

#include "s_data_app.h"
#include "ki_exec_query.h"

namespace Ui {
class print_pays2mo_rep_pt_wnd;
}

class print_pays2mo_rep_pt_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;

    bool f_2point;      // признак - "отчёт по ПВП", иначе - общий отчёт

    void refresh_points();
    QString date_to_str(QDate date);

public:
    explicit print_pays2mo_rep_pt_wnd(QSqlDatabase &db, bool f_2point, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~print_pays2mo_rep_pt_wnd();

private slots:
    void on_bn_close_clicked();

    void on_bn_print_clicked();

    void on_date_from_editingFinished();

    void on_date_to_editingFinished();

private:
    Ui::print_pays2mo_rep_pt_wnd *ui;
};

#endif // print_pays2mo_rep_pt_wnd_H
