#ifndef BLANKS_BSO_WND_H
#define BLANKS_BSO_WND_H

#include <QDialog>
#include <QtSql>

#include "s_data_app.h"

#include "ki_exec_query.h"

namespace Ui {
class blanks_BSO_wnd;
}

class blanks_BSO_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    QSqlQueryModel model_blanks;
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;

    void refresh_combo_blanks_type();
    void refresh_combo_blanks_act();

    void refresh_combo_pnt0();
    void refresh_combo_status0();
    void refresh_combo_pnt1();
    void refresh_combo_status1();

    QString sql_blanks;
    QString sql_cnt;

    void refresh_blanks_tab();

public:
    explicit blanks_BSO_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~blanks_BSO_wnd();

private slots:
    void on_combo_blanks_type_activated(int index);

    void on_bn_refresh_clicked();

    void on_date0_2_editingFinished();
    void on_date1_2_editingFinished();

    void on_combo_blanks_act_activated(int index);
    void on_date0_editingFinished();
    void on_date1_editingFinished();
    void on_combo_pnt0_activated(int index);
    void on_combo_pnt1_activated(int index);
    void on_combo_status0_activated(int index);
    void on_combo_status1_activated(int index);

    void on_ch_filter_from_clicked(bool checked);
    void on_ch_filter_to_clicked(bool checked);

    // автофильтры
    void on_bn_from_clicked();
    void on_bn_from_sp_clicked();
    void on_bn_add_clicked();
    void on_bn_add_sp_clicked();
    void on_bn_dec_clicked();
    void on_bn_dec_sp_clicked();
    void on_bn_to_clicked();
    void on_bn_to_sp_clicked();

    void on_bn_toCSV_clicked();

private:
    Ui::blanks_BSO_wnd *ui;
};

#endif // BLANKS_BSO_WND_H
