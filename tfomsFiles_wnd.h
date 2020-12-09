#ifndef tfomsFiles_wnd_H
#define tfomsFiles_wnd_H

#include <QDialog>

#include "s_data_app.h"
#include "ki_exec_query.h"

class persons_wnd;

namespace Ui {
class tfomsFiles_wnd;
}

class tfomsFiles_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    QSqlDatabase db_ODBC;
    exec_SQL mySQL;
    QSqlQueryModel model_flk;
    QSqlQueryModel model_tfomsFiles;

    persons_wnd *persons_w;
    QSettings &settings;
    s_data_app &data_app;

    void refresh_tfomsFiles_tab();

public:
    explicit tfomsFiles_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, persons_wnd *persons_w, QWidget *parent = 0);
    ~tfomsFiles_wnd();

private slots:
    void on_bn_load_new_tfomsFiles_clicked();

    void on_bn_refresh_clicked();

    void on_bn_close_clicked();

private:
    Ui::tfomsFiles_wnd *ui;
};

#endif // tfomsFiles_wnd_H
