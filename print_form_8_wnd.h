#ifndef PRINT_FORM_8_WND_H
#define PRINT_FORM_8_WND_H

#include <QDialog>

#include "s_data_app.h"

#include "ki_exec_query.h"

namespace Ui {
class print_form_8_wnd;
}

class print_form_8_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;

    QString month_to_str(QDate date);

    bool f_init;

public:
    explicit print_form_8_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~print_form_8_wnd();

private slots:
    void on_bn_cansel_clicked();

    void on_bn_print_clicked();

    void on_bd_cur_date_clicked();

    void on_bn_report_date_clicked();

    void on_bn_edit_clicked(bool checked);

    void on_ln_smo_director_textChanged(const QString &arg1);

    void on_ch_doc2_clicked(bool checked);

private:
    Ui::print_form_8_wnd *ui;
};

#endif // PRINT_FORM_8_WND_H
