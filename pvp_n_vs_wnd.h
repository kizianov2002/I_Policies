#ifndef PVP_N_VS_WND_H
#define PVP_N_VS_WND_H

#include <QDialog>
#include <QtSql>
#include <QDate>

#include "s_data_app.h"
#include "s_data_pers.h"

#include "ki_exec_query.h"

namespace Ui {
class pvp_n_vs_wnd;
}

class pvp_n_vs_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    QSqlQueryModel model_point_types;
    QSqlQueryModel model_points;
    QSqlQueryModel model_operators;
    QSqlQueryModel model_intervals;
    QSqlQueryModel model_intervals_pt;
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;
    s_data_pers &data_pers;

    QList<int> list_points;
    QList<int> list_intervals;

public:
    explicit pvp_n_vs_wnd(QSqlDatabase &db, s_data_app &data_app, s_data_pers &data_pers, QSettings &settings, QWidget *parent = 0);
    ~pvp_n_vs_wnd();

    void refresh_point_types();
    void refresh_points();
    void refresh_point_types_combo();
    void refresh_operators(int id_pvp);
    void refresh_combo_oper_rights_combo();
    void refresh_intervals();
    void refresh_intervals_pt(int id_pvp, int id_int);
    void refresh_folders();
    void refresh_combo_act_obtainers(int id_point);

private slots:
    void on_bn_close_clicked();
    /*
    void on_ch_point_types_clicked(bool checked);
    void on_ch_points_clicked(bool checked);
    void on_ch_operatops_clicked(bool checked);
    void on_ch_intervals_clicked(bool checked);
    void on_ch_intervals_pt_clicked(bool checked);
    */
    void on_tab_point_types_clicked(const QModelIndex &index);
    void on_tab_points_clicked(const QModelIndex &index);
    void on_tab_intervals_clicked(const QModelIndex &index);

    void on_ch_intervals_pt_all_clicked();

    void on_tab_intervals_pt_clicked(const QModelIndex &index);

    void on_bn_refresh_point_types_clicked();

    void on_bn_refresh_points_clicked();

    void on_bn_refresh_operators_clicked();

    void on_bn_refresh_intervals_clicked();

    void on_bn_refresh_intervals_pt_clicked();

    void on_bn_add_point_clicked();

    void on_bn_edit_point_clicked();

    void on_bn_delete_point_clicked();

    void on_bn_add_operator_clicked();

    void on_tab_operators_clicked(const QModelIndex &index);

    void on_bn_delete_operator_clicked();

    void on_bn_edit_operator_clicked();

    void on_bn_add_interval_clicked();

    void on_spin_interval_from_valueChanged(int arg1);

    void on_spin_interval_to_valueChanged(int arg1);

    void on_bn_edit_interval_clicked();

    void on_bn_delete_interval_clicked();

    void on_bn_add_interval_pt_clicked();

    void on_spin_interval_pt_from_valueChanged(int arg1);

    void on_spin_interval_pt_to_valueChanged(int arg1);

    void on_date_interval_pt_close_dateChanged(const QDate &date);

    void on_ch_interval_pt_close_toggled(bool checked);

    void on_bn_edit_interval_pt_clicked();

    void on_bn_delete_interval_pt_clicked();

    void on_bn_add_point_type_clicked();

    void on_bn_delete_point_type_clicked();

    void on_bn_edit_point_type_clicked();

    void on_ch_interval_pt_folder_clicked(bool checked);

    void on_date_act_pt_editingFinished();

private:
    Ui::pvp_n_vs_wnd *ui;
};

#endif // PVP_N_VS_WND_H
