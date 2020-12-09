#ifndef msg_WND_H
#define msg_WND_H

#include <QDialog>
#include <QTimer>

#include "s_data_app.h"

#include "ki_exec_query.h"

namespace Ui {
class msg_wnd;
}

class msg_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    QSqlQueryModel model_operators;
    QSqlQueryModel model_resived;
    exec_SQL mySQL;

    QList<int>  getters_id;
    QStringList getters_name;

    QSettings &settings;
    s_data_app &data_app;

    void refresh_operators_tab();
    void refresh_history();

    QTimer *msg_timer;

public:
    explicit msg_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~msg_wnd();

private slots:
    void msg_update();

    void on_bn_send_clicked();

    void on_tab_operators_clicked(const QModelIndex &index);

    void on_bn_clear_clicked();

    void on_bn_oper_refresh_clicked();

    void on_bn_hist_drop_clicked();

    void on_bn_hist_reload_clicked();

    void on_msg_wnd_accepted();

    void on_msg_wnd_rejected();

    void on_msg_wnd_finished(int result);

    void on_bn_close_clicked();

    void on_bn_oper_all_clicked();

private:
    Ui::msg_wnd *ui;
};

#endif // msg_WND_H
