#ifndef firmS_WND_H
#define firmS_WND_H

#include <QDialog>
#include <QtSql>

#include "s_data_app.h"

namespace Ui {
class firms_wnd;
}

#include "ki_exec_query.h"

typedef struct {
    int id;
    QString name;
} s_data_firm;

class firms_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    QSqlQueryModel model_firms;
    exec_SQL mySQL;

    s_data_app &data_app;
    s_data_firm &data_firm;

    void refresh_firms_tab();

public:
    explicit firms_wnd(QSqlDatabase &db, s_data_app &data_app, s_data_firm &data_firm, QWidget *parent = 0);
    ~firms_wnd();

private slots:
    void on_bn_close_clicked();

    void on_bn_refresh_clicked();

    void on_bn_add_clicked();

    void on_bn_edit_clicked();

    void on_bn_ok_clicked();

    void on_tab_firms_clicked(const QModelIndex &index);

    void on_bn_delete_clicked();

    void on_ln_name_editingFinished();

    void on_ln_address_editingFinished();

    void on_ln_director_editingFinished();

    void on_ln_phones_editingFinished();

private:
    Ui::firms_wnd *ui;
};

#endif // firmS_WND_H
