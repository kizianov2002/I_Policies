#ifndef list_assig_medorgs_WND_H
#define list_assig_medorgs_WND_H

#include <QDialog>
#include <QtSql>
#include <QSettings>

namespace Ui {
class list_assig_medorgs_wnd;
}

#include "s_data_app.h"
#include "ki_exec_query.h"

class list_assig_medorgs_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    QSqlQueryModel model_medorgs;
    exec_SQL mySQL;

    s_data_app &data_app;
    QSettings &settings;

    void refresh_medorgs_tab();

public:
    explicit list_assig_medorgs_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~list_assig_medorgs_wnd();

private slots:
    void on_bn_close_clicked();

    void on_bn_refresh_clicked();

    void on_bn_add_clicked();

    void on_bn_edit_clicked();

    void on_tab_medorgs_clicked(const QModelIndex &index);

    void on_bn_delete_clicked();

private:
    Ui::list_assig_medorgs_wnd *ui;
};

#endif // list_assig_medorgs_WND_H
