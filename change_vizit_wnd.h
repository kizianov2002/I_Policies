#ifndef CHANGE_VIZIT_WND_H
#define CHANGE_VIZIT_WND_H

#include <QDialog>

#include "s_data_app.h"
#include "ki_exec_query.h"
#include "s_data_vizit.h"
#include "s_data_event.h"


namespace Ui {
class change_vizit_wnd;
}

class change_vizit_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    exec_SQL mySQL;

    s_data_event &data_event;
    s_data_vizit &data_vizit;

    QSettings &settings;
    s_data_app &data_app;

    void refresh_event_codes();
    void refresh_event_state();

    void refresh_vizit_method();
    void refresh_reasons_smo();
    void refresh_reasons_polis();
    void refresh_form_polis();

    void refresh_combo_UDL();
    void refresh_combo_DRP();

    void refresh_viz_points();
    void refresh_viz_operators(int id_point);

public:
    explicit change_vizit_wnd(QSqlDatabase &db, s_data_vizit &data_vizit, s_data_event &data_event, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~change_vizit_wnd();

private slots:
    void on_bn_now_clicked();

    void on_ch_viz_oper_clicked(bool checked);

    void on_combo_viz_point_activated(int index);

    void on_bn_cansel_clicked();
    void on_bn_save_clicked();

    void on_combo_UDL_currentIndexChanged(const QString &arg1);

    void on_combo_DRP_currentIndexChanged(const QString &arg1);

private:
    Ui::change_vizit_wnd *ui;
};

#endif // CHANGE_VIZIT_WND_H
