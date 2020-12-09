#ifndef WORK_CALENDAR_WND_H
#define WORK_CALENDAR_WND_H

#include <QDialog>
#include <QtSql>
#include <QSettings>

#include "ki_SqlQueryModel.h"
#include "s_data_app.h"

#include "ki_exec_query.h"

namespace Ui {
class work_calendar_wnd;
}

class work_calendar_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    ki_calendarSqlQueryModel model_work_calendar;
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;

    void refresh_tab();

public:
    explicit work_calendar_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~work_calendar_wnd();

private slots:
    void on_bn_close_clicked();

    void on_spin_year_editingFinished();

    void on_bn_refresh_clicked();

    void on_tableView_clicked(const QModelIndex &index);

    void on_bn_add_day_clicked();

    void on_bn_delete_day_clicked();

    void on_spin_year_valueChanged(int arg1);

private:
    Ui::work_calendar_wnd *ui;
};

#endif // WORK_CALENDAR_WND_H
