#ifndef RESEND_EVENTS_WND_H
#define RESEND_EVENTS_WND_H

#include <QDialog>

#include "s_data_app.h"
#include "ki_exec_query.h"

namespace Ui {
class resend_events_wnd;
}

class resend_events_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    exec_SQL mySQL;
    QSqlQueryModel model_events_data;

    QSettings &settings;
    s_data_app &data_app;

    void refresh_tab_events_data(QString ids);

public:
    explicit resend_events_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~resend_events_wnd();

private slots:
    void on_bn_cansel_clicked();

    void on_bn_send_clicked();

    void on_line_events_editingFinished();

    void on_bn_clear_clicked();

private:
    Ui::resend_events_wnd *ui;
};

#endif // RESEND_EVENTS_WND_H
