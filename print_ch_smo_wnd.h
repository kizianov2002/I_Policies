#ifndef PRINT_CH_SMO_WND_H
#define PRINT_CH_SMO_WND_H

#include <QDialog>

#include "s_data_app.h"
#include "s_data_pers.h"
#include "s_data_doc.h"
#include "s_data_polis.h"
#include "s_data_event.h"
#include "s_data_vizit.h"

#include "ki_exec_query.h"

namespace Ui {
class print_ch_smo_wnd;
}

class print_ch_smo_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    QSqlQueryModel model_polises;
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;
    s_data_pers &data_pers;
    //s_data_doc &data_doc;
    s_data_vizit data_vizit;
    s_data_polis data_polis;
    s_data_event data_event;

    QString date_to_str(QDate date);

public:
    explicit print_ch_smo_wnd(QSqlDatabase &db, s_data_app &data_app, s_data_pers &data_pers, s_data_vizit &data_vizit, s_data_polis &data_polis, QSettings &settings, QWidget *parent = 0);
    ~print_ch_smo_wnd();

private slots:
    void on_bn_close_clicked();

    void on_bn_print_clicked();

    void on_bn_edit_clicked(bool checked);

    void on_bn_cansel_clicked();

private:
    Ui::print_ch_smo_wnd *ui;
};

#endif // PRINT_CH_SMO_WND_H
