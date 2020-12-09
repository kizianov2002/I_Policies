#ifndef FINDER_WND_H
#define FINDER_WND_H

#include <QDialog>
#include <QTimer>

#include "s_data_app.h"

#include "ki_exec_query.h"

namespace Ui {
class finder_wnd;
}

class finder_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    QSqlDatabase &db_FIAS;
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;

    void refresh_docs_pan_type();
    void refresh_fias_address(int subj, int dstr, int city, int nasp, int strt);

public:
    explicit finder_wnd(QSqlDatabase &db, QSqlDatabase &db_FIAS, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~finder_wnd();

private slots:
    void on_bn_close_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_finder_wnd_accepted();

    void on_finder_wnd_finished(int result);

    void on_finder_wnd_rejected();

    void on_ch_age_clicked(bool checked);
    void on_ch_sex_clicked(bool checked);

    void on_combo_subj_activated(int index);
    void on_combo_dstr_activated(int index);
    void on_combo_city_activated(int index);
    void on_combo_nasp_activated(int index);
    void on_combo_strt_activated(int index);

    void on_line_vs_num_editingFinished();
    void on_line_pol_ser_editingFinished();
    void on_line_enp_editingFinished();
    void on_combo_doc_type_activated(const QString &arg1);
    void on_line_doc_sernum_editingFinished();
    void on_date_doc_start_editingFinished();
    void on_date_doc_exp_editingFinished();

    void on_line_pol_num_editingFinished();
    void on_line_doc_ser_editingFinished();
    void on_line_doc_num_editingFinished();

    void on_ch_phone_clicked(bool checked);
    void on_ch_email_clicked(bool checked);

    void on_line_phone_editingFinished();
    void on_line_email_editingFinished();

    void on_ch_id_person_clicked(bool checked);
    void on_ch_id_polis_clicked(bool checked);
    void on_ch_id_event_clicked(bool checked);
    void on_ch_id_vizit_clicked(bool checked);

    void on_line_id_person_editingFinished();
    void on_line_id_polis_editingFinished();
    void on_line_id_event_editingFinished();
    void on_line_id_vizit_editingFinished();

    void on_bn_find_clicked();

private:
    Ui::finder_wnd *ui;
};

#endif // FINDER_WND_H
