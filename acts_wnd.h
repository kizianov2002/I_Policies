#ifndef ACTS_WND_H
#define ACTS_WND_H

#include <QDialog>
#include "show_tab_wnd.h"
#include "act_vs2point_wnd.h"
#include "act_pol2smo_wnd.h"
#include "act_pol2point_wnd.h"
#include "print_blanks_pol_act_wnd.h"
#include "print_blanks_act_wnd.h"

#include "s_data_app.h"

#include "ki_sqlquerymodel.h"
#include "ki_exec_query.h"

namespace Ui {
class acts_wnd;
}

class acts_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    QSqlQueryModel model_vs2smo;
    ki_acts_vs2point_SqlQueryModel  model_vs2point;
    ki_acts_pol2smo_SqlQueryModel   model_pol2smo;
    ki_acts_pol2point_SqlQueryModel model_pol2point;
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;

    QStringList head_regnums;

    show_tab_wnd *show_tab_w;
    act_vs2point_wnd *act_vs2point_w;
    act_pol2smo_wnd *act_pol2smo_w;
    act_pol2point_wnd *act_pol2point_w;
    print_blanks_pol_act_wnd *print_blanks_pol_act_w;
    print_blanks_act_wnd *print_blanks_act_w;

    QString month_to_str(QDate date);
    QString date_to_str(QDate date);

    void refresh_vs2smo_tab();
    void refresh_vs2point_tab();
    void refresh_pol2smo_tab();
    void refresh_pol2point_tab();

    void refresh_combo_sender_vs2point();
    void refresh_combo_point_vs2point();
    void refresh_combo_obtainer_vs2point(int id_point);

    void refresh_combo_head_point_pol2smo();
    void refresh_combo_obtainer_pol2smo(int id_point);

    void refresh_combo_sender_pol2point();
    void refresh_combo_point_pol2point();
    void refresh_combo_obtainer_pol2point(int id_point);

    QString sql_vs2smo, sql_vs2point, sql_pol2smo, sql_pol2point;

public:
    explicit acts_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~acts_wnd();

private slots:
    void on_bn_close_clicked();

    void on_bn_refresh_vs2smo_clicked();
    void on_bn_refresh_vs2point_clicked();
    void on_bn_refresh_pol2smo_clicked();
    void on_bn_refresh_pol2point_clicked();

    void on_combo_point_vs2point_activated(int index);
    void on_combo_head_point_pol2smo_activated(int index);
    void on_combo_point_pol2point_activated(int index);

    void on_tab_vs2smo_clicked(const QModelIndex &index);
    void on_tab_vs2point_clicked(const QModelIndex &index);
    void on_tab_pol2point_clicked(const QModelIndex &index);
    void on_tab_pol2smo_clicked(const QModelIndex &index);

    void on_bn_data_vs2smo_clicked();
    void on_bn_data_vs2point_clicked();
    void on_bn_data_pol2smo_clicked();
    void on_bn_data_pol2point_clicked();

    void on_bn_add_vs2point_clicked();
    void on_bn_add_pol2smo_clicked();
    void on_bn_add_pol2point_clicked();

    void on_bn_edit_vs2smo_clicked();
    void on_bn_edit_vs2point_clicked();
    void on_bn_edit_pol2smo_clicked();
    void on_bn_edit_pol2point_clicked();

    void on_bn_del_vs2point_clicked();
    void on_bn_del_pol2smo_clicked();
    void on_bn_del_pol2point_clicked();

    void on_line_act_num_pol2smo_textChanged(const QString &arg1);
    void on_line_act_num_vs2point_textChanged(const QString &arg1);
    void on_line_act_num_pol2point_textChanged(const QString &arg1);

    void on_ch_vs2point_apply_clicked(bool checked);
    void on_ch_pol2point_apply_clicked(bool checked);

    void on_bn_vs2smo_print_clicked();
    void on_bn_vs2point_print_clicked();
    void on_bn_pol2smo_print_clicked();
    void on_bn_pol2point_print_clicked();

    void on_bn_vs2point_print_2_clicked();
    void on_bn_pol2point_print_2_clicked();

    void on_bn_filter_vs2point_clicked();
    void on_bn_filter_pol2point_clicked();
    void on_ln_filter_vs2point_editingFinished();
    void on_ln_filter_pol2point_editingFinished();

    void on_tab_vs2smo_activated(const QModelIndex &index);
    void on_tab_pol2smo_activated(const QModelIndex &index);
    void on_tab_vs2point_activated(const QModelIndex &index);
    void on_tab_pol2point_activated(const QModelIndex &index);

    void on_combo_ord_vs2smo_activated(const QString &arg1);
    void on_combo_ord_pol2smo_activated(const QString &arg1);
    void on_combo_ord_vs2point_activated(const QString &arg1);
    void on_combo_ord_pol2point_activated(const QString &arg1);

    void on_ch_filter_vs2point_clicked(bool checked);
    void on_ch_filter_pol2point_clicked(bool checked);

    void on_bn_toCSV_vs2smo_clicked();

    void on_bn_toCSV_pol2smo_clicked();

    void on_bn_toCSV_vs2point_clicked();

    void on_bn_toCSV_pol2point_clicked();

    void on_bn_vs2point_print_3_clicked();

    void on_bn_pol2point_print_3_clicked();

private:
    Ui::acts_wnd *ui;
};

#endif // ACTS_WND_H
