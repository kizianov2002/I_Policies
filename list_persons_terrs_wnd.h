#ifndef list_persons_terrs_WND_H
#define list_persons_terrs_WND_H

#include <QDialog>
#include <QtSql>
#include <QSettings>

namespace Ui {
class list_persons_terrs_wnd;
}

#include "s_data_app.h"
#include "ki_exec_query.h"

class list_persons_terrs_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    QSqlQueryModel model_terrs;
    exec_SQL mySQL;

    s_data_app &data_app;
    QSettings &settings;

    void refresh_terrs_tab();

public:
    explicit list_persons_terrs_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~list_persons_terrs_wnd();

private slots:
    void on_bn_close_clicked();

    void on_bn_refresh_clicked();

    void on_bn_add_clicked();

    void on_bn_edit_clicked();

    void on_tab_terrs_clicked(const QModelIndex &index);

    void on_bn_delete_clicked();

private:
    Ui::list_persons_terrs_wnd *ui;
};

#endif // list_persons_terrs_WND_H
