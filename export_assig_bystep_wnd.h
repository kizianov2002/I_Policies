#ifndef export_assig_bystep_wnd_h
#define export_assig_bystep_wnd_h

#include <QDialog>
#include "ki_exec_query.h"

#include "s_data_app.h"

namespace Ui {
class export_assig_bystep_wnd;
}

class export_assig_bystep_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных

    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;
    void refresh_points();
    void refresh_fpolis();

    bool print_registry_SMS();

public:
    explicit export_assig_bystep_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~export_assig_bystep_wnd();

private slots:
    void on_bn_close_clicked();

    void on_bn_fname_clicked();

    void on_bn_save_to_csv_clicked();

    void on_date_assig_editingFinished();

private:
    Ui::export_assig_bystep_wnd *ui;
};

#endif // export_assig_bystep_wnd_h
