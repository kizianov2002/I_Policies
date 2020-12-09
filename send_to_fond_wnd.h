#ifndef SEND_TO_FOND_WND_H
#define SEND_TO_FOND_WND_H

#include <QDialog>
#include <QtSql>
#include <QSettings>

#include "export_assig_bystep_wnd.h"
#include "beep_guten_morgen_wnd.h"

#include "info_not_sended_wnd.h"

#include "s_data_app.h"

#include "ki_exec_query.h"
#include "ki_funcs.h"

namespace Ui {
class send_to_fond_wnd;
}

class send_to_fond_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;       // это - наша база данных
    QSqlDatabase db_ODBC;   // это - п
    exec_SQL mySQL;

    export_assig_bystep_wnd *export_assig_bystep_w;
    info_not_sended_wnd *info_not_sended_w;
    beep_Guten_Morgen_wnd *Gutten_Morgen_w;

    QSettings &settings;
    s_data_app &data_app;

    void refresh_points();
    void refresh_events();

public:
    QStringList xml_files;
    QStringList zip_files;
    QString  big_zip_file;
    explicit send_to_fond_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~send_to_fond_wnd();

    //bool myBYTEA(QString &fname, QString& res_str);  // перенесено в exec_sql
    int send(QString point_regnum, bool f_old_data=false);

private slots:
    void on_rb_split_clicked();

    void on_rb_all_clicked();

    void on_bn_cansel_clicked();

    void on_bn_process_clicked();

    void on_rb_genNew_clicked(bool checked);
    void on_rb_genOld_clicked(bool checked);

    //void on_ch_old_point_clicked(bool checked);
    //void on_ch_old_event_clicked(bool checked);
    void on_ch_old_dates_clicked(bool checked);

    void on_date_from_dateChanged(const QDate &date);

    void on_date_to_dateChanged(const QDate &date);

    void on_bn_today_clicked();

    void on_ch_split_refugees_clicked(bool checked);

    void on_ch_split_russians_clicked(bool checked);

    void on_ch_split_foreigners_clicked(bool checked);

    void on_ch_split_russians_zpatname_clicked(bool checked);

    void on_ch_split_foreigners_zpatname_clicked(bool checked);

    void on_ch_split_others_clicked(bool checked);

private:
    Ui::send_to_fond_wnd *ui;
};

#endif // SEND_TO_FOND_WND_H
