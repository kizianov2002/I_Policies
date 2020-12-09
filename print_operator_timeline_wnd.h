#ifndef PRINT_OPERATOR_TIMELINE_WND_H
#define PRINT_OPERATOR_TIMELINE_WND_H

#include <QDialog>

#include "ki_exec_query.h"

namespace Ui {
class print_operator_timeline_wnd;
}

class print_operator_timeline_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;

    void refresh_combo_operator();
    void refresh_combo_month();

public:
    explicit print_operator_timeline_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~print_operator_timeline_wnd();

private slots:
    void on_bn_print_clicked();

    void on_bn_cansel_clicked();

private:
    Ui::print_operator_timeline_wnd *ui;
};

#endif // PRINT_OPERATOR_TIMELINE_WND_H
