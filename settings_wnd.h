#ifndef SETTINGS_WND_H
#define SETTINGS_WND_H

#include <QDialog>
#include <QtSql>

#include "s_data_app.h"

#include "ki_exec_query.h"

namespace Ui {
class settings_wnd;
}

class settings_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;

    void refresh_folders();

public:
    explicit settings_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~settings_wnd();

    void refresh_ocato();

private slots:
    void on_bn_cansel_clicked();

    void on_combo_ocato_currentIndexChanged(int index);

    void on_bn_save_clicked();

private:
    Ui::settings_wnd *ui;
};

#endif // SETTINGS_WND_H
