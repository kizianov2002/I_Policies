#ifndef PACKS_WND_H
#define PACKS_WND_H

#include <QDialog>
#include <QtSql>

#include "s_data_app.h"

namespace Ui {
class packs_wnd;
}

#include "ki_exec_query.h"
#include "ki_sqlquerymodel.h"

typedef struct {
    int id;
    QString pack_name;
} s_data_pack;

class packs_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    ki_packs_SqlQueryModel model_packs;
    ki_stops_SqlQueryModel model_stops;
    exec_SQL mySQL;

    QList<int> points_rights;

    s_data_app &data_app;
    s_data_pack &data_pack;

    void refresh_points();
    void refresh_packs_tab();
    void refresh_stops_tab();

public:
    explicit packs_wnd(QSqlDatabase &db, s_data_app &data_app, s_data_pack &data_pack, QWidget *parent = 0);
    ~packs_wnd();

private slots:
    void on_bn_close_clicked();

    void on_ch_date_clicked(bool checked);
    void on_date_dateChanged(const QDate &date);
    void on_bn_today_clicked();

    void on_bn_refresh_clicked();

    void on_ch_date_S_clicked(bool checked);
    void on_date_S_dateChanged(const QDate &date);

    void on_bn_refresh_stops_clicked();

    void on_combo_point_activated(int index);

    void on_bn_save_file_I_clicked();
    void on_bn_save_file_P_clicked();
    void on_bn_save_file_P1_clicked();
    void on_bn_save_file_F_clicked();
    void on_bn_save_file_S_clicked();

    void on_bn_delete_pack_clicked();

    void on_ch_point_clicked(bool checked);

private:
    Ui::packs_wnd *ui;
};

#endif // PACKS_WND_H
