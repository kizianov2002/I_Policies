#ifndef ADD_medsite_ter_wnd_H
#define ADD_medsite_ter_wnd_H

#include <QDialog>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include "ki_exec_query.h"
#include "s_data_app.h"

namespace Ui {
class add_medsite_ter_wnd;
}


typedef struct {
    int id_ms;
    int id_mt;
    QString code_mo;
    QString name_mo;
    QString code_mp;
    QString name_mp;
    QString code_ms;
    QString name_ms;
    QString fio_mt;
    QString snils_mt;
    int prof_mt;
    QDate date_beg;
    QDate date_end;
} s_data_medsite_ter;


class add_medsite_ter_wnd : public QDialog
{
    Q_OBJECT

    QSqlDatabase &db;
    s_data_app &data_app;
    s_data_medsite_ter data;
    QSettings &settings;
    exec_SQL mySQL;

    void refresh_medsites();
    void refresh_medters();
public:
    explicit add_medsite_ter_wnd(QSqlDatabase &db, s_data_app &data_app, s_data_medsite_ter &data, QSettings &settings, QWidget *parent = 0);
    ~add_medsite_ter_wnd();

private slots:
    void on_bn_cansel_clicked();

    void on_bn_save_clicked();

private:
    Ui::add_medsite_ter_wnd *ui;
};

#endif // ADD_medsite_ter_wnd_H
