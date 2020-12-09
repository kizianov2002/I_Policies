#ifndef SPR_ASSIG_ERRORS_WND_H
#define SPR_ASSIG_ERRORS_WND_H

#include <QDialog>
#include <QSqlQueryModel>
#include "change_error_wnd.h"

#include "s_data_app.h"
#include "ki_exec_query.h"

namespace Ui {
class spr_assig_errors_wnd;
}

class spr_assig_errors_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    exec_SQL mySQL;
    QSqlQueryModel model_errors;

    s_data_app &data_app;

    change_error_wnd *change_error_w;

    void refresh_errors_tab();

public:
    explicit spr_assig_errors_wnd(QSqlDatabase &db, s_data_app &data_app, QWidget *parent = 0);
    ~spr_assig_errors_wnd();

private slots:
    void on_bn_close_clicked();

    void on_bn_refresh_clicked();

    void on_tab_errors_clicked(const QModelIndex &index);

    void on_bn_delete_clicked();

    void on_bn_add_clicked();

private:
    Ui::spr_assig_errors_wnd *ui;
};

#endif // SPR_ASSIG_ERRORS_WND_H
