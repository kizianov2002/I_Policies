#ifndef PROTOCOLS_WND_H
#define PROTOCOLS_WND_H

#include <QDialog>
#include "persons_wnd.h"
#include "show_tab_wnd.h"

#include "s_data_app.h"
#include "ki_exec_query.h"

#include "s_data_file_P.h"
#include "s_data_file_s.h"
#include "s_data_file_f.h"

#include "ki_SqlQueryModel.h"

class persons_wnd;

namespace Ui {
class protocols_wnd;
}

class protocols_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    exec_SQL mySQL;
    QSqlQueryModel model_flk;
    ki_protocolSqlQueryModel model_protocols;

    persons_wnd *persons_w;
    QSettings &settings;
    s_data_app &data_app;

    show_tab_wnd *show_tab_w;

    s_data_file_P data_file_P;
    s_data_file_S data_file_S;
    s_data_file_F data_file_F;

    bool f_yes2all;

    int id_pfile;
    int id_pack;
    int id_ifile;
    int id_person;
    int id_polis;
    int id_event;

    void refresh_packs();
    void refresh_protocols_tab();
    void refresh_flk_tab();

    bool process_P_file (QString file_fullname);
    bool process_S_file (QString file_fullname);
    bool process_K_file (QString file_fullname);
    bool process_F_file (QString file_fullname);
    bool process_U_file (QString file_fullname);
    bool process_EO_file (QString file_fullname);

    bool test_S_file (s_data_file_S &data_file_S);

public:
    explicit protocols_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, persons_wnd *persons_w, QWidget *parent = 0);
    ~protocols_wnd();

private slots:
    void on_bn_close_clicked();

    void on_bn_hide_clicked();

    void on_bn_load_new_protokols_clicked();

    void on_bn_refresh_clicked();

    //void on_ch_only_polis_clicked();
    //void on_ch_only_wrong_clicked();
    //void on_ch_only_wrong_active_clicked();

    void on_tab_protocols_clicked(const QModelIndex &index);

    void on_bn_to_polis_clicked();

    void on_tab_protocols_activated(const QModelIndex &index);

    void on_bn_hide_rec_clicked();

    void on_bn_refresh_flk_clicked();

    void on_bn_hide_flk_clicked();

    void on_tab_flk_clicked(const QModelIndex &index);

    void on_bn_upsize_clicked();
    void on_bn_dnsize_clicked();

    void on_combo_file_currentIndexChanged(int index);
    void on_combo_file_activated(const QString &arg1);
    void on_ch_req_polis_no_clicked(bool checked);
    void on_ch_req_polis_ok_clicked(bool checked);
    void on_ch_req_errors_clicked(bool checked);
    void on_ch_req_ready_clicked(bool checked);
    void on_ch_req_resended_clicked(bool checked);
    void on_ch_req_complete_clicked(bool checked);
    void on_ch_req_close_byhand_clicked(bool checked);

    void on_rb_proto_all_clicked(bool checked);
    void on_rb_proto_day_clicked(bool checked);
    void on_rb_proto_file_clicked(bool checked);
    void on_bn_today_clicked();
    void on_date_file_dateChanged(const QDate &date);

    void on_bn_save_Ifile_clicked();

    void on_bn_save_Pfile_clicked();

    void on_protocols_wnd_finished(int result);

    void on_bn_filter_clear_clicked();
    void on_line_filter_enp_editingFinished();
    void on_line_filter_ser_editingFinished();
    void on_line_filter_num_editingFinished();
    void on_line_filter_comment_editingFinished();

private:
    Ui::protocols_wnd *ui;
};

#endif // PROTOCOLS_WND_H
