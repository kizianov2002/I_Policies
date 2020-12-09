#ifndef FOLDER_WND_H
#define FOLDER_WND_H

#include <QDate>
#include <QDialog>
#include <QSettings>
#include "ki_exec_query.h"

#include "s_data_app.h"

typedef struct {
    int id;
    QString folder_name;
    QDate date_open;
    QDate date_close;
    int status;
} s_data_folder;

namespace Ui {
class folder_wnd;
}

class folder_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных

    exec_SQL mySQL;

    s_data_folder &data_folder;
    QSettings &settings;
    s_data_app &data_app;

public:
    explicit folder_wnd(QSqlDatabase &db, s_data_folder &data_folder, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~folder_wnd();

private slots:
    void on_pushButton_clicked();

    void on_bn_save_clicked();

    void on_bn_today_clicked();

    void on_bn_cansel_clicked();

private:
    Ui::folder_wnd *ui;
};

#endif // FOLDER_WND_H
