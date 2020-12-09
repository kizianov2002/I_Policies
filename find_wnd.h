#ifndef FIND_WND_H
#define FIND_WND_H

#include <QDialog>

#include "s_data_app.h"

#include "ki_exec_query.h"

namespace Ui {
class find_wnd;
}

class find_wnd : public QDialog
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
    explicit find_wnd(QSqlDatabase &db, QSqlDatabase &db_FIAS, s_data_app &data_app, QSettings &settings, /*persons_wnd *persons_w,*/ QWidget *parent = 0);
    ~find_wnd();

private slots:
    void on_bn_close_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_combo_subj_activated(int index);
    void on_combo_dstr_activated(int index);
    void on_combo_city_activated(int index);
    void on_combo_nasp_activated(int index);
    void on_combo_strt_activated(int index);

    void on_line_vs_num_editingFinished();
    void on_line_pol_ser_editingFinished();
    void on_line_enp_editingFinished();
    void on_combo_doc_type_activated(const QString &arg1);
    void on_line_doc_ser_editingFinished();
    void on_line_doc_num_editingFinished();
    void on_date_doc_start_editingFinished();
    void on_date_doc_exp_editingFinished();
    void on_ch_doc_is_act_clicked();

    void on_bn_find_clicked();

private:
    Ui::find_wnd *ui;
};

#endif // FIND_WND_H
