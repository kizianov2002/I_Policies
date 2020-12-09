#ifndef BLANKS_POL_ACT_H
#define BLANKS_POL_ACT_H

#include <QDialog>
#include "print_blanks_pol_act_wnd.h"
#include "ki_exec_query.h"

#include "s_data_app.h"

//typedef QList<s_data_phone> lst_data_phone;

namespace Ui {
class blanks_pol_act_wnd;
}

class blanks_pol_act_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    QSqlQueryModel model_phones;
    //lst_data_phone phones;

    print_blanks_pol_act_wnd *print_blanks_pol_act_w;

    QString sql_tab;
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;
    void refresh_points();
    void refresh_combo_obtainers(int id_point);
    void refresh_fpolis();

    bool print_registry_SMS();

public:
    explicit blanks_pol_act_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~blanks_pol_act_wnd();

private slots:
    void on_bn_close_clicked();

    void on_bn_gen_list_blanks_clicked();

    void on_date_back_from_editingFinished();

    void on_date_back_to_editingFinished();

    void on_date_to_editingFinished();

    void on_bn_generate_clicked();

    void on_ch_filter_clicked();

    void on_bn_filter_all_clicked();

    void on_bn_filter_none_clicked();

    void on_tab_blanks_pol_clicked(const QModelIndex &index);

    void on_ch_phone_clicked();

    void on_combo_point_activated(int index);

    void on_combo_fpolis_activated(int index);

    void on_combo_polis_post_way_activated(int index);

    void on_ch_no_activate_clicked(bool checked);

    void on_ch_no_act_clicked(bool checked);

    void on_ch_no_get2hand_clicked(bool checked);

    void on_date_back_from_dateChanged(const QDate &date);

    void on_date_back_to_dateChanged(const QDate &date);

    void on_ch_to_clicked(bool checked);

    void on_rb_new_clicked(bool checked);

    void on_rb_back_clicked(bool checked);

    void on_combo_point_currentIndexChanged(const QString &arg1);

private:
    Ui::blanks_pol_act_wnd *ui;
};

#endif // BLANKS_POL_ACT_H
