#ifndef BARCODE_WND_H
#define BARCODE_WND_H

#include <QDialog>
#include <QtSql>
#include <QTime>

#include "s_data_app.h"

#include "ki_exec_query.h"

namespace Ui {
class barcode_wnd;
}

class barcode_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;

public:
    explicit barcode_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~barcode_wnd();

private:
    Ui::barcode_wnd *ui;
};

#endif // BARCODE_WND_H
