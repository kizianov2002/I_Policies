#ifndef BLANKS_WND_H
#define BLANKS_WND_H

#include <QDialog>
#include <QtSql>
#include <QTime>
#include "barcode_wnd.h"
#include "add_blank_scan_wnd.h"
#include "add_blank_wnd.h"

#include "s_data_app.h"
#include "s_data_blank.h"
#include "s_filter_pers.h"

#include "ki_exec_query.h"

namespace Ui {
class blanks_wnd;
}

class blanks_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    QSqlQueryModel model_blanks_vs;
    QSqlQueryModel model_blanks_pol;
    exec_SQL mySQL;

    QStringList persons_list;

    add_blank_scan_wnd *add_blank_scan_w;
    add_blank_wnd *add_blank_w;
    barcode_wnd *barcode_w;

    QSettings &settings;
    s_data_app &data_app;
    s_data_blank &data_blank;

public:
    explicit blanks_wnd(QSqlDatabase &db, s_data_blank &data_blank, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~blanks_wnd();

    int id_point_vs, id_operator_vs;
    int id_point_pol, id_operator_pol;

    void refresh_blanks_points();
    void refresh_blanks_statuses();
    void refresh_polis_forms();
    void refresh_blanks_vs_tab();
    void refresh_blanks_pol_tab();

    void refresh_persons();

private slots:
    void on_bn_close_clicked();

    void on_bn_refresh_blanks_vs_clicked();

    void on_bn_add_blank_vs_clicked();

    void on_combo_filter_vs_status_currentIndexChanged(int index);

    void on_combo_filter_pol_status_currentIndexChanged(int index);

    void on_bn_edit_blank_vs_clicked();

    void on_tab_blanks_vs_clicked(const QModelIndex &index);

    void on_bn_delete_blank_vs_clicked();

    void on_bn_refresh_blanks_pol_clicked();

    void on_bn_add_blank_pol_clicked();

    void on_ln_blank_num_vs_textChanged(const QString &arg1);
    void on_ln_blank_ser_pol_textChanged(const QString &arg1);
    void on_ln_blank_num_pol_textChanged(const QString &arg1);
    void on_ln_enp_textChanged(const QString &arg1);

    void on_tab_blanks_pol_clicked(const QModelIndex &index);

    void on_bn_edit_blank_pol_clicked();

    void on_bn_delete_blank_pol_clicked();

    void on_rb_person_clicked(bool checked);

    void on_rb_vs_clicked(bool checked);

    void on_rb_pol_clicked(bool checked);

    void on_bn_use_filter_clicked();

    void on_bn_clear_filter_clicked();

    void on_ln_snils_textEdited(const QString &arg1);
    void on_ln_vs_num_textEdited(const QString &arg1);
    void on_ln_pol_ser_textEdited(const QString &arg1);
    void on_ln_pol_num_textEdited(const QString &arg1);
    void on_ln_pol_enp_textEdited(const QString &arg1);

    void on_ln_blank_num_pol_cursorPositionChanged(int arg1, int arg2);

    void on_bn_add_blank_clicked();

    void on_bn_scan_blanks_clicked();

    void on_bn_toCSV_vs_clicked();
    void on_bn_toCSV_pol_clicked();

    void on_ch_vs_num_2_clicked(bool checked);

    void on_ln_vs_num_editingFinished();

    void on_ln_vs_num_2_editingFinished();

    void on_ln_filter_fam_editingFinished();
    void on_ln_filter_im_editingFinished();
    void on_ln_filter_ot_editingFinished();

    void on_bn_clear_clicked();

    void on_ln_vs_num_textChanged(const QString &arg1);
    void on_ln_vs_num_2_textChanged(const QString &arg1);
    void on_ln_pol_ser_textChanged(const QString &arg1);
    void on_ln_pol_num_textChanged(const QString &arg1);
    void on_ln_pol_enp_textChanged(const QString &arg1);
    void on_ln_snils_textChanged(const QString &arg1);

    void on_ln_fam_returnPressed();

    void on_ln_im_returnPressed();

    void on_ln_ot_returnPressed();

    void on_date_birth_editingFinished();

    void on_ln_snils_returnPressed();

    void on_ln_vs_num_returnPressed();

    void on_ln_vs_num_2_returnPressed();

    void on_ln_pol_ser_returnPressed();

    void on_ln_pol_num_returnPressed();

    void on_ln_pol_enp_returnPressed();

    void on_combo_filter_vs_point_currentIndexChanged(int index);

    void on_combo_filter_pol_point_currentIndexChanged(int index);

private:
    Ui::blanks_wnd *ui;
};

#endif // BLANKS_WND_H
