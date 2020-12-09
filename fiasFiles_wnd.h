#ifndef fiasFiles_wnd_H
#define fiasFiles_wnd_H

#include <QDialog>

#include "s_data_app.h"
#include "ki_exec_query.h"

class persons_wnd;

namespace Ui {
class fiasFiles_wnd;
}

class fiasFiles_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    QSqlDatabase &db_ODBC;
    QSqlDatabase db_FIAS2;    // это - база данных ФИАС с админскими правами
    exec_SQL mySQL;

    persons_wnd *persons_w;
    QSettings &settings;
    s_data_app &data_app;

    QString FIAS_db_drvr;
    QString FIAS_db_host;
    QString FIAS_db_name;
    QString FIAS_db_port;
    QString FIAS_db_admn;
    QString FIAS_db_admp;

    // начало всей обработки
    QDate date0;
    QTime time0;
    // начало последнего цикла
    QDate date1;
    QTime time1;
    // текущее дата/время
    QDate date;
    QTime time;

    bool try_connect_FIAS();
    bool import_FIAS_file (QString fullname_str, QString &FIAS_import_log, bool f_reload);
    bool import_FIAS_delta(QString fullname_str, QString &FIAS_import_log, bool f_reload);
    bool remove_FIAS_delta(QString fullname_str, QString &FIAS_import_log, bool f_reload);
    void count_FIAS_data();
    void vacuum_FIAS_tables();
    void reindex_FIAS_tables();

public:
    explicit fiasFiles_wnd(QSqlDatabase &db, QSqlDatabase &db_ODBC, s_data_app &data_app, QSettings &settings, persons_wnd *persons_w, QWidget *parent = 0);
    ~fiasFiles_wnd();

private slots:

    void on_bn_close_clicked();

    void on_bn_FIAS_load_clicked();
    void on_bn_FIAS_load4files_clicked();
    void on_bn_FIAS_delta_clicked();

    void on_ch_change_server_clicked(bool checked);

    void on_push_connect_clicked();

    void on_combo_db_driv_activated(int index);

    void on_bn_FIAS_reindex_clicked();
    void on_bn_FIAS_recalc_clicked();
    void on_bn_FIAS_vacuum_clicked();
    void on_bn_FIAS_clear_clicked();

    void on_bn_FIAS_count_clicked();

    void on_bn_FIAS_IDs_clicked();

private:
    Ui::fiasFiles_wnd *ui;
};

#endif // fiasFiles_wnd_H
