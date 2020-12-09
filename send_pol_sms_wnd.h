#ifndef SEND_POL_SMS_H
#define SEND_POL_SMS_H

#include <QDialog>
#include "ki_exec_query.h"

#include "s_data_app.h"

typedef struct {
    int id_vs;
    int id_person;
    QString phone;
    QString vs_num;
    QDate vs_date;
    QString pol_num;
    QString pol_ser;
    QDate date_add;
    QString fio;
    QDate date_r;
    QDate date_sms;

    QString address;
    QString result;
} s_data_phone;

typedef QList<s_data_phone> lst_data_phone;

namespace Ui {
class send_pol_sms_wnd;
}

class send_pol_sms_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    QSqlQueryModel model_phones;
    lst_data_phone phones;

    QString sql_phones;
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;
    void refresh_points();
    void refresh_fpolis();
    void refresh_pers_categ();

    bool print_registry_SMS();
    bool print_registry_addresses();

public:
    explicit send_pol_sms_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~send_pol_sms_wnd();

private slots:
    void on_bn_close_clicked();

    void on_bn_gen_list_sms_clicked();

    void on_date_vs4from_editingFinished();
    void on_date_vs4to_editingFinished();

    void on_date_to_editingFinished();

    void on_bn_save_to_csv_clicked();

    void on_ch_filter_clicked();
    void on_bn_filter_all_clicked();
    void on_bn_filter_none_clicked();

    void on_tab_phones_clicked(const QModelIndex &index);

    void on_ch_phone_clicked();

    void on_combo_point_activated(int index);
    void on_combo_fpolis_activated(int index);
    void on_combo_polis_post_way_activated(int index);

    void on_date_vs4from_dateChanged(const QDate &date);
    void on_date_vs4to_dateChanged(const QDate &date);

    void on_ch_vs4to_clicked(bool checked);

    void on_spin_vs4exp_editingFinished();

    void on_spin_vs4days_editingFinished();

    void on_spin_vs4days_valueChanged(int arg1);
    void on_spin_vs4exp_valueChanged(int arg1);

    void on_bn_toCSV_clicked();

    void on_bn_sms_reester_clicked();
    void on_bn_addr_reester_clicked();
    void on_bn_addr_reester_2_clicked();
    void on_bn_sms_reester_2_clicked();

    void on_rb_new_clicked(bool checked);
    void on_rb_back_clicked(bool checked);
    void on_rb_oldUDLs_clicked(bool checked);
    void on_rb_oldDRPs_clicked(bool checked);
    void on_rb_oldENPs_clicked(bool checked);
    void on_rb_oldRFpassport_clicked(bool checked);

    void on_rb_vs4days_clicked(bool checked);
    void on_rb_vs4date_clicked(bool checked);
    void on_rb_vs4exp_clicked(bool checked);

    void on_ch_no_activate_clicked(bool checked);
    void on_ch_no_sms_clicked(bool checked);
    void on_ch_no_get2hand_clicked(bool checked);

    void on_rb_doc4days_clicked(bool checked);
    void on_rb_doc4exp_clicked(bool checked);

    void on_spin_doc4days_valueChanged(int arg1);
    void on_spin_doc4exp_valueChanged(int arg1);

private:
    Ui::send_pol_sms_wnd *ui;
};

#endif // SEND_POL_SMS_H
