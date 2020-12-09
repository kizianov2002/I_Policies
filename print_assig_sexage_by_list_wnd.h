#ifndef print_assig_sexage_by_list_wnd_H
#define print_assig_sexage_by_list_wnd_H

#include <QDialog>

#include "s_data_app.h"

#include "ki_exec_query.h"

namespace Ui {
class print_assig_sexage_by_list_wnd;
}

class print_assig_sexage_by_list_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;

    QString month_to_str(QDate date);

public:
    explicit print_assig_sexage_by_list_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~print_assig_sexage_by_list_wnd();

private slots:
    void on_bn_cansel_clicked();

    void on_bn_print_clicked();

    void on_bn_edit_clicked(bool checked);

    void on_bn_date_assig_clicked();

    void on_ln_smo_name_textChanged(const QString &arg1);

    void on_ln_filial_director_textChanged(const QString &arg1);

    void on_ln_filial_director_2_textChanged(const QString &arg1);

    void on_ln_filial_chif_textChanged(const QString &arg1);

    void on_ln_terr_name_textChanged(const QString &arg1);

    void on_ln_terr_name_rp_textChanged(const QString &arg1);

    void on_bn_date_cur_clicked();

    void on_bn_first_day_clicked();

private:
    Ui::print_assig_sexage_by_list_wnd *ui;
};

#endif // print_assig_sexage_by_list_wnd_H
