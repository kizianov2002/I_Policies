#ifndef print_pers_sexage_wnd_H
#define print_pers_sexage_wnd_H

#include <QDialog>

#include "s_data_app.h"

#include "ki_exec_query.h"

namespace Ui {
class print_pers_sexage_wnd;
}

class print_pers_sexage_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;

    QString month_to_str(QDate date);

public:
    explicit print_pers_sexage_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~print_pers_sexage_wnd();

private slots:
    void on_bn_cansel_clicked();

    void on_bn_print_clicked();

    void on_bn_edit_clicked(bool checked);

    void on_bn_date_rep_clicked();

    void on_ln_smo_name_textChanged(const QString &arg1);

    void on_ln_tfoms_director_textChanged(const QString &arg1);

    void on_ln_tfoms_director_2_textChanged(const QString &arg1);

    void on_ln_tfoms_chif_textChanged(const QString &arg1);

    void on_ln_filial_director_textChanged(const QString &arg1);

    void on_ln_filial_director_2_textChanged(const QString &arg1);

    void on_ln_filial_chif_textChanged(const QString &arg1);

    void on_bn_date_cur_clicked();

private:
    Ui::print_pers_sexage_wnd *ui;
};

#endif // print_pers_sexage_wnd_H
