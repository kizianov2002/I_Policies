#ifndef POLICES_WND_H
#define POLICES_WND_H

#include <QDialog>

#include "pvp_n_vs_wnd.h"
#include "blanks_wnd.h"
#include "work_calendar_wnd.h"
#include "print_vs_wnd.h"
#include "print_ch_smo_wnd.h"
#include "print_db_pol_wnd.h"
#include "print_notice_pol_wnd.h"
#include "print_notic2_wnd.h"

#include "s_data_app.h"
#include "s_data_pers.h"
//#include "s_data_doc.h"
#include "s_data_polis.h"
#include "s_data_event.h"
#include "s_data_vizit.h"
#include "ki_SqlQueryModel.h"

#include "ki_exec_query.h"

namespace Ui {
class polices_wnd;
}

class polices_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    ki_polSqlQueryModel model_polises;
    exec_SQL mySQL;

    QLabel *statusLab;
    QLabel *statusOper;

    QSettings &settings;
    s_data_app &data_app;
    s_data_pers &data_pers;
    s_data_vizit data_vizit;
    s_data_polis data_polis;
    s_data_event data_event;

    pvp_n_vs_wnd *pvp_n_vs_w;
    blanks_wnd *blanks_w;
    work_calendar_wnd *work_calendar_w;

    print_vs_wnd *print_vs_w;
    print_ch_smo_wnd *print_ch_smo_w;
    print_db_pol_wnd *print_db_pol_w;
    print_notice_pol_wnd *print_notice_pol_w;
    print_notic2_wnd *print_notic2_w;

    void b_need_save (bool f);

    void test_for_udl_exp();
    void test_for_pers_categ();
    bool test_for_dead();

public:
    explicit polices_wnd(QSqlDatabase &db, s_data_app &data_app, s_data_pers &data_pers, QSettings &settings, QWidget *parent = 0);
    ~polices_wnd();

    void start();

    void refresh_vizit_method();
    void refresh_reasons_smo();
    void refresh_reasons_polis();
    void refresh_form_polis();
    void refresh_polises_tab();
    void refresh_terr_ocato();
    void refresh_pol_ves();
    void refresh_polis_data();

    bool load_vizit();
    bool save_vizit();

    // работа с полисами
    bool process_P010(s_data_polis &data_polis, s_data_event &data_event);

    bool process_P021_P022_P023_P024_P025(s_data_polis &data_polis, s_data_event &data_event, int new_status, bool inner_act); // inner_act - признак, что полис закрывается не непосредственно, а в связи с открытием нового полиса
    //bool process_P021(s_data_polis &data_polis, s_data_event &data_event, int new_status);
    //bool process_P022(s_data_polis &data_polis, s_data_event &data_event, int new_status);
    //bool process_P023(s_data_polis &data_polis, s_data_event &data_event, int new_status);
    //bool process_P024(s_data_polis &data_polis, s_data_event &data_event, int new_status);
    //bool process_P025(s_data_polis &data_polis, s_data_event &data_event, int new_status);

    bool process_P031_P032_P033(s_data_polis &data_polis, s_data_event &data_event);

    bool process_P034_P035_P036(s_data_polis &data_polis, s_data_event &data_event);
    //bool process_P034(s_data_polis &data_polis, s_data_event &data_event);
    //bool process_P035(s_data_polis &data_polis, s_data_event &data_event);
    //bool process_P036(s_data_polis &data_polis, s_data_event &data_event);

    bool process_P060(s_data_polis &data_polis, s_data_event &data_event);

    bool process_P061_P062_P063(s_data_polis &data_polis, s_data_event &data_event);
    //bool process_P061(s_data_polis &data_polis, s_data_event &data_event);
    //bool process_P062(s_data_polis &data_polis, s_data_event &data_event);
    //bool process_P063(s_data_polis &data_polis, s_data_event &data_event);

    bool corrID_Polis(int id_polis);    // изменение идентификационнйх данных существующего полиса на основе data...
    bool edit_Polis(int id_polis);      // изменение существующего полиса на основе data...
    bool close_Polis(int id_polis);     // закрытие существующего полиса на основе data...

    QDate myAddDays(QDate date_start, int n_days);

private slots:
    void on_bn_close_clicked();

    void on_bn_now_vizit_clicked();
    void on_bn_now_act_clicked();
    void on_bn_now_stop_clicked();

    void on_show_pan_vizit_clicked(bool checked);
    void on_show_pan_polises_clicked(bool checked);
    void on_show_pan_polis_clicked(bool checked);

    void on_bn_vizit_next_clicked();
    void on_bn_polises_back_clicked();
    void on_bn_polises_next_clicked();
    void on_bn_polis_back_clicked();

    void on_rb_previzit_clicked();
    void on_rb_activate_clicked();
    void on_rb_stoppolis_clicked();
    void on_rb_corrdata_clicked();

    void on_bn_vizit_back_clicked();

    void on_bn_refresh_polises_clicked();

    void on_combo_terr_ocato_text_activated(int index);

    void on_bn_polis_save_clicked();

    void on_line_vs_num_selectionChanged();
    void on_line_vs_num_textChanged(const QString &arg1);
    void on_ch_vs_num_stateChanged(int arg1);
    void on_ch_pol_sernum_stateChanged(int arg1);
    void on_ch_uec_num_stateChanged(int arg1);
    void on_ch_pol_enp_stateChanged(int arg1);
    void on_ch_pol_datbegin_stateChanged(int arg1);
    void on_ch_pol_datend_stateChanged(int arg1);
    void on_ch_pol_datstop_stateChanged(int arg1);
    void on_combo_pol_v_currentIndexChanged(int index);

    void on_bn_print_ch_smo_clicked();
    void on_bn_print_vs_clicked();

    void on_bn_p010_clicked();
    void on_bn_p021_clicked();
    void on_bn_p022_clicked();
    void on_bn_p023_clicked();
    void on_bn_p024_clicked();
    void on_bn_p025_clicked();
    void on_bn_p031_clicked();
    void on_bn_p032_clicked();
    void on_bn_p033_clicked();
    void on_bn_p034_clicked();
    void on_bn_p035_clicked();
    void on_bn_p036_clicked();
    void on_bn_p060_clicked();
    void on_bn_p061_clicked();
    void on_bn_p062_clicked();
    void on_bn_p063_clicked();

    void on_tab_polises_doubleClicked(const QModelIndex &index);

    void on_bn_print_notice_pol_clicked();

    /*void on_rb_vizit_pan_reason_smo_clicked(bool checked);

    void on_rb_vizit_pan_reason_polis_clicked(bool checked);*/

    void on_combo_pol_v_activated(int index);
    void on_line_pol_ser_textEdited(const QString &arg1);
    void on_line_pol_num_editingFinished();
    void on_line_vs_num_editingFinished();
    void on_line_enp_cursorPositionChanged(int arg1, int arg2);
    void on_date_pol_begin_editingFinished();
    void on_date_pol_end_editingFinished();
    void on_date_pol_stop_editingFinished();
    void on_ch_pol_erp_clicked();
    void on_ch_pol_datstop_clicked();
    void on_ch_pol_datend_clicked();
    void on_ch_pol_datbegin_clicked();
    void on_ch_pol_enp_clicked();
    void on_ch_vs_num_clicked();
    void on_ch_pol_sernum_clicked();

    void on_bn_print_vs_2_clicked();

    void on_line_pol_ser_textChanged(const QString &arg1);

    void on_line_pol_num_textChanged(const QString &arg1);

    void on_line_enp_textChanged(const QString &arg1);

    void on_bn_polises_delete_clicked();

    void on_bn_polises_spoiled_vs_clicked();

    void on_bn_print_db_pol_clicked();

    void on_bn_pvp_n_vs_clicked();

    void on_bn_blanks_clicked();

    void on_bn_calendar_clicked();

    void on_combo_vizit_pan_form_polis_activated(const QString &arg1);

    void on_combo_vizit_pan_reason_smo_activated(int index);

    void on_combo_vizit_pan_reason_polis_activated(int index);

    void on_bn_layout_clicked();

    void on_ch_get2hand_clicked(bool checked);

    void on_line_vs_num_textEdited(const QString &arg1);

    void on_line_pol_num_textEdited(const QString &arg1);

    void on_line_uec_num_textEdited(const QString &arg1);

    void on_line_enp_textEdited(const QString &arg1);

    void on_combo_vizit_pan_method_activated(int index);

    void on_bn_print_notic2_clicked();

    void on_date_pol_begin_dateChanged(const QDate &date);
    void on_date_pol_end_dateChanged(const QDate &date);
    void on_date_pol_stop_dateChanged(const QDate &date);

private:
    Ui::polices_wnd *ui;
};

#endif // POLICES_WND_H
