#ifndef print_dispanserize_plans_wnd_H
#define print_dispanserize_plans_wnd_H

#include <QDialog>

#include "s_data_app.h"
#include "ki_exec_query.h"

namespace Ui {
class print_dispanserize_plans_wnd;
}

class print_dispanserize_plans_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;

    QString date_to_str(QDate date);

public:
    explicit print_dispanserize_plans_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~print_dispanserize_plans_wnd();

private slots:
    void on_bn_close_clicked();

    void on_bn_print_clicked();

private:
    Ui::print_dispanserize_plans_wnd *ui;
};

#endif // print_dispanserize_plans_wnd_H
