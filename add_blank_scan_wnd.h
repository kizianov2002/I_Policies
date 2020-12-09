#ifndef ADD_BLANK_SCAN_WND_H
#define ADD_BLANK_SCAN_WND_H

#include <QDialog>
#include <QtSql>
#include <QTime>

#include "s_data_app.h"
#include "ki_exec_query.h"
#include "add_blanks_box_wnd.h"
#include "add_blank_wnd.h"

namespace Ui {
class add_blank_scan_wnd;
}

class add_blank_scan_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    QSqlQueryModel model_blanks;
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;

    add_blanks_box_wnd *add_blanks_box_w;
    add_blank_wnd *add_blank_w;

    void refresh_blanks_boxes();
    void refresh_blanks_tab();
    void refresh_comports();

public:
    explicit add_blank_scan_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~add_blank_scan_wnd();

    void on_combo_comport_activated(const QString &arg1);

private slots:
    void on_bn_start_scaner_clicked();

    void on_bn_process_scaner_clicked();

    void on_bn_cansel_clicked();

    void on_bn_add_box_clicked();

    void on_combo_boxes_currentIndexChanged(const QString &arg1);

    void on_combo_boxes_currentIndexChanged(int index);

    void on_bn_refresh_clicked();

    void on_ch_today_clicked();

private:
    Ui::add_blank_scan_wnd *ui;
};

#endif // ADD_BLANK_SCAN_WND_H
