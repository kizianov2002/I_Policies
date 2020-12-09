#ifndef ASSIG_GAPS_WND_H
#define ASSIG_GAPS_WND_H

#include <QDialog>
#include <QtSql>
#include <QSettings>
#include "ki_exec_query.h"
#include "s_data_app.h"
#include "s_data_assig.h"

namespace Ui {
class assig_gaps_wnd;
}

class assig_gaps_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    QSqlDatabase &db_FIAS;    // это - наша база ФИАС

    QSettings &settings;
    s_data_app &data_app;
    s_data_assig &data_assig;

    QSqlQueryModel model_gaps;
    exec_SQL mySQL;

public:
    explicit assig_gaps_wnd(QSqlDatabase &db, QSqlDatabase &db_FIAS, s_data_app &data_app, QSettings &settings, s_data_assig &data_assig, QWidget *parent = 0);
    ~assig_gaps_wnd();
    void refresh_combo_subj();
    void refresh_combo_dstr();
    void refresh_combo_city();
    void refresh_combo_nasp();
    void refresh_combo_strt();

    void refresh_gaps_tab();

private slots:
    void on_bn_exit_clicked();

    void on_combo_subj_activated(int index);
    void on_combo_dstr_activated(int index);
    void on_combo_city_activated(int index);
    void on_combo_nasp_activated(int index);
    void on_combo_strt_activated(int index);

    void on_bn_refresh_gaps_clicked();
    void on_bn_clear_filter_gaps_clicked();

    void on_combo_age_activated(int index);

    void on_combo_sex_activated(int index);

    void on_ch_show_names_clicked(bool checked);

    void on_bn_remove_clicked();

private:
    Ui::assig_gaps_wnd *ui;
};

#endif // ASSIG_GAPS_WND_H
