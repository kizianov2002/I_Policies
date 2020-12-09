#ifndef get_year_wnd_H
#define get_year_wnd_H

#include <QDialog>

#include "s_data_app.h"
#include "ki_exec_query.h"

namespace Ui {
class get_year_wnd;
}

class get_year_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;

public:
    explicit get_year_wnd(QString header_s, QString message_s, bool only_year_f, bool future_f, int year, int quarter, QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~get_year_wnd();

    QString header_s, message_s;    // заголовок, текстовое сообщение
    int year, quarter;              // год, квартал
    bool only_year_f;               // признак выбора только до года
    bool future_f;                  // признак возможности выбора еще не наступившего года/квартала (+2 года)

    void refresh_years_quarts();

private slots:
    void on_rb_year_clicked(bool checked);
    void on_rb_quarter_clicked(bool checked);

    void on_bn_cansel_clicked();

    void on_bn_save_clicked();


private:
    Ui::get_year_wnd *ui;
};

#endif // get_year_wnd_H
