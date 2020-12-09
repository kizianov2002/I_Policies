#ifndef PRINT_VS_WND_H
#define PRINT_VS_WND_H

#include <QtSql>
#include <QDialog>
#include <QSettings>
#include <QDate>

#include "s_data_app.h"
#include "s_data_pers.h"
#include "s_data_polis.h"
#include "s_data_doc.h"

#include "ki_exec_query.h"

namespace Ui {
class print_vs_wnd;
}

class print_vs_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;
    s_data_pers &data_pers;
    s_data_polis &data_polis;
    s_data_doc data_doc;

public:
    explicit print_vs_wnd(QSqlDatabase &db, s_data_app &data_app, s_data_pers &data_pers, s_data_polis &data_polis, QSettings &settings, QWidget *parent = 0);
    ~print_vs_wnd();

    QString date_to_str(QDate);
    QString month_to_str(QDate);

private slots:
    void on_bn_edit_clicked(bool checked);

    void on_bn_cansel_clicked();

    void on_bn_print_clicked();

private:
    Ui::print_vs_wnd *ui;
};

#endif // PRINT_VS_WND_H
