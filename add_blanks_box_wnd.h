#ifndef ADD_BLANKS_BOX_H
#define ADD_BLANKS_BOX_H

#include <QDialog>
#include <QtSql>
#include <QTime>

#include "s_data_app.h"
#include "ki_exec_query.h"

namespace Ui {
class add_blanks_box_wnd;
}

class add_blanks_box_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;
    QStringList head_regnums;
    QStringList point_regnums;

    void refresh_heads();
    void refresh_head_operators(int id_head);
    void refresh_points();
    void refresh_point_operators(int id_point);

public:
    explicit add_blanks_box_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~add_blanks_box_wnd();

private slots:
    void on_bn_cansel_clicked();

    void on_bn_process_clicked();

    void on_date_head_act_dateChanged(const QDate &date);

    void on_combo_head_currentIndexChanged(int index);

    void on_combo_point_activated(int index);

    void on_combo_point_currentIndexChanged(int index);

    void on_bn_today_clicked();

    void on_date_point_act_dateChanged(const QDate &date);

    void on_date_head_act_editingFinished();

private:
    Ui::add_blanks_box_wnd *ui;
};

#endif // ADD_BLANKS_BOX_H
