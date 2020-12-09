#ifndef DISP_PLANS_WND_H
#define DISP_PLANS_WND_H

#include <QDialog>
#include <QtSql>

#include "s_data_app.h"
#include "ki_exec_query.h"
#include "get_year_wnd.h"
#include "show_tab_wnd.h"

namespace Ui {
class disp_plans_wnd;
}

class disp_plans_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    QSqlQueryModel model_disp_plans;
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;

    show_tab_wnd *show_tab_w;

    get_year_wnd *get_year_w;

    void refresh_disp_plans_tab();

    bool gen_disp_plan(int age);
    void show_disp_plan(int year, int quarter, int age);

public:
    explicit disp_plans_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~disp_plans_wnd();

private slots:
    void on_bn_close_clicked();

    void on_bn_refresh_clicked();

    void on_bn_gen_disp_plan_clicked();
    void on_bn_gen_disp_plan_18_clicked();
    void on_bn_gen_prof_plan_clicked();

    void on_bn_delete_data_clicked();

    void on_bn_show_disp_plan_clicked();
    void on_bn_show_disp_plan_18_clicked();
    void on_bn_show_prof_plan_clicked();
    void on_bn_show_disp_plan_dead_clicked();

    void on_bn_gen_disp_count4mo_clicked();
    void on_bn_gen_disp_list4mo_clicked();
    void on_bn_gen_disp_calls4mo_clicked();

    void on_bn_load_autocall_clicked();

    void on_bn_gen_calls_res4mo_clicked();

private:
    Ui::disp_plans_wnd *ui;
};

#endif // DISP_PLANS_WND_H
