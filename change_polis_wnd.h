#ifndef change_polis_WND_H
#define change_polis_WND_H

#include <QDialog>

#include "s_data_app.h"
#include "ki_exec_query.h"
#include "s_data_polis.h"

namespace Ui {
class change_polis_wnd;
}

class change_polis_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    exec_SQL mySQL;

    s_data_polis &data_polis;

    QSettings &settings;
    s_data_app &data_app;

    void refresh_form_polis();
    void refresh_reasons_smo();
    void refresh_reasons_polis();
    void refresh_pol_ves();
    void refresh_points();
    void refresh_operators(int id_point);

public:
    explicit change_polis_wnd(QSqlDatabase &db, s_data_polis &data_polis, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~change_polis_wnd();

private slots:
    void on_ch_r_smo_clicked(bool checked);
    void on_ch_r_polis_clicked(bool checked);
    void on_ch_pol_datbegin_clicked(bool checked);
    void on_ch_pol_datend_clicked(bool checked);
    void on_ch_pol_datstop_clicked(bool checked);
    void on_ch_vs_num_clicked(bool checked);
    void on_ch_pol_sernum_clicked(bool checked);
    void on_ch_pol_enp_clicked(bool checked);
    void on_ch_uec_num_clicked(bool checked);
    void on_ch_date_activate_clicked(bool checked);
    void on_ch_date_get2hand_clicked(bool checked);

    void on_bn_close_clicked();
    void on_bn_save_clicked();

    void on_bn_get2hand_now_clicked();

    void on_line_enp_textChanged(const QString &arg1);
    void on_line_pol_ser_textChanged(const QString &arg1);
    void on_line_pol_num_textChanged(const QString &arg1);
    void on_line_uec_num_textChanged(const QString &arg1);
    void on_line_vs_num_textChanged(const QString &arg1);

    void on_ch_pol_oper_clicked(bool checked);

private:
    Ui::change_polis_wnd *ui;
};

#endif // change_polis_WND_H
