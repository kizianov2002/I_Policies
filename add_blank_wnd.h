#ifndef add_blank_WND_H
#define add_blank_WND_H

#include <QDialog>
#include <QtSql>
#include <QTime>

#include "s_data_app.h"
#include "ki_exec_query.h"

namespace Ui {
class add_blank_wnd;
}

typedef struct {
    int id_blanks_boxes;
    int bc_type;
    int f_polis;
    QString pol_ser, pol_num;
    QString enp;
    QString fam, im, ot;
    int sex;
    QDate date_birth;
    QDate date_exp;
    QString smo_ogrn;
    QString ocato;
    QString eds;
    QDate date_scan_enp;
} s_data_new_blank;

class add_blank_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    exec_SQL mySQL;

    QStringList persons_list;
    s_data_new_blank &data_new_blank;

    QSettings &settings;
    s_data_app &data_app;

    void refresh_comports();
    void refresh_blanks_forms();
    void refresh_blanks_statuses();
    void refresh_persons();

    void on_bn_clear_clicked();

public:
    explicit add_blank_wnd(QSqlDatabase &db, s_data_app &data_app, s_data_new_blank &data_new_blank, QSettings &settings, QWidget *parent = 0);
    //explicit add_blank_wnd(QWidget *parent = 0);
    ~add_blank_wnd();

private slots:
    void on_bn_now_clicked();

    void on_bn_cansel_clicked();

    void on_bn_add_clicked();

    void on_ch_pol_exp_clicked(bool checked);

    void on_bn_exp_now_clicked();

    void on_bn_next_clicked();

    void on_ln_pol_ser_textChanged(const QString &arg1);
    void on_ln_pol_num_textChanged(const QString &arg1);
    void on_ln_enp_textChanged(const QString &arg1);

    void on_ln_filter_fam_returnPressed();

    void on_ln_filter_im_returnPressed();

    void on_ln_filter_ot_returnPressed();

    void on_bn_clear_clicked(bool checked);

    void on_ln_filter_fam_editingFinished();

    void on_ln_filter_im_editingFinished();

    void on_ln_filter_ot_editingFinished();

    void on_ln_enp_textEdited(const QString &arg1);

    void on_ln_pol_ser_textEdited(const QString &arg1);

    void on_ln_pol_num_textEdited(const QString &arg1);

private:
    Ui::add_blank_wnd *ui;
};

#endif // add_blank_WND_H
