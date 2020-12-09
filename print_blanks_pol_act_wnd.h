#ifndef print_blanks_pol_act_wnd_H
#define print_blanks_pol_act_wnd_H

#include <QDialog>

#include "s_data_app.h"

#include "ki_exec_query.h"

typedef struct {
    int id;
    QString act_num;
    QDate act_date;
    QString sender;
    QString sender_post;
    QString sender_fio;
    QString obtainer;
    QString obtainer_post;
    QString obtainer_fio;
} s_data_blanks_pol_act;

namespace Ui {
class print_blanks_pol_act_wnd;
}

class print_blanks_pol_act_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    exec_SQL mySQL;

    s_data_blanks_pol_act &data_act;
    QSettings &settings;
    s_data_app &data_app;

    QString month_to_str(QDate date);

public:
    explicit print_blanks_pol_act_wnd(QSqlDatabase &db, s_data_blanks_pol_act &data_act, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~print_blanks_pol_act_wnd();

private slots:
    void on_bn_cansel_clicked();

    void on_bn_print_clicked();

    void on_bn_edit_clicked(bool checked);

private:
    Ui::print_blanks_pol_act_wnd *ui;
};

#endif // print_blanks_pol_act_wnd_H
