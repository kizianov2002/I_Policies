#ifndef terminals_WND_H
#define terminals_WND_H

#include <QDialog>
#include <QtSql>
#include <QSettings>
#include "show_text_wnd.h"

namespace Ui {
class terminals_wnd;
}

#include "s_data_app.h"
#include "ki_exec_query.h"

class terminals_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    QSqlQueryModel model_terminals;
    QSortFilterProxyModel proxyModel_terminals;
    exec_SQL mySQL;
    show_text_wnd *show_text_w;

    s_data_app &data_app;
    QSettings &settings;

    void refresh_points();
    void refresh_terminals();

public:
    explicit terminals_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~terminals_wnd();

private slots:
    void on_bn_close_clicked();

    void on_bn_refresh_clicked();

    void on_bn_edit_clicked();

    void on_tab_terminals_clicked(const QModelIndex &index);

    void on_bn_delete_clicked();

    void on_bn_show_INI_clicked();

    void on_bn_load_INI_clicked();

    void on_rb_point_clicked();

    void on_rb_term_code_clicked();

    void on_rb_term_name_clicked();

private:
    Ui::terminals_wnd *ui;
};

#endif // terminals_WND_H
