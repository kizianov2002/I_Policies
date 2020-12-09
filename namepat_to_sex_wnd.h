#ifndef namepat_to_sex_WND_H
#define namepat_to_sex_WND_H

#include <QDialog>
#include <QtSql>
#include <QSettings>

#include "ki_SqlQueryModel.h"
#include "s_data_app.h"

#include "ki_exec_query.h"

namespace Ui {
class namepat_to_sex_wnd;
}

typedef struct {
    int id;
    QString namepat;
    int sex;
    int status;
} s_data_namepat;

class namepat_to_sex_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    ki_names2sexSqlQueryModel model_namepat_to_sex;
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;
    s_data_namepat data_namepat;

    void refresh_tab();

public:
    explicit namepat_to_sex_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~namepat_to_sex_wnd();

private slots:
    void on_bn_close_clicked();

    void on_bn_refresh_clicked();

    void on_tableView_clicked(const QModelIndex &index);

    void on_bn_add_namepat_clicked();

    void on_bn_delete_namepat_clicked();

    void on_ln_namepat_filter_editingFinished();

    void on_combo_sex_filter_activated(int index);

    void on_bn_edit_namepat_clicked();

private:
    Ui::namepat_to_sex_wnd *ui;
};

#endif // namepat_to_sex_WND_H
