#ifndef get_date_wnd_H
#define get_date_wnd_H

#include <QDialog>

#include "s_data_app.h"
#include "ki_exec_query.h"

namespace Ui {
class get_date_wnd;
}

class get_date_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;

public:
    explicit get_date_wnd(QString header_s, QString message_s, bool interval_f, bool one_day_f, bool side_from_f, bool side_to_f, QDate &date1, QDate &date2, QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~get_date_wnd();

    QString header_s, message_s;    // заголовок, текстовое сообщение
    QDate &date1, &date2;           // дата начала, дата конца
    bool one_day_f;                 // признак выбора дня
    bool interval_f;                // признак выбора интервала дат
    bool side_from_f, side_to_f;    // признак "включая эту дату"

    void refresh_years_mons();

private slots:    
    void on_rb_date_clicked(bool checked);
    void on_rb_month_clicked(bool checked);
    void on_rb_year_clicked(bool checked);
    void on_rb_dates_clicked(bool checked);

    void on_date_from_dateChanged(const QDate &date);
    void on_date_to_dateChanged(const QDate &date);

    void on_bn_cansel_clicked();

    void on_bn_save_clicked();

    void on_bn_tomorrow_clicked();

private:
    Ui::get_date_wnd *ui;
};

#endif // get_date_wnd_H
