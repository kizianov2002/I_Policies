#ifndef export_pers_by_distrs_wnd_h
#define export_pers_by_distrs_wnd_h

#include <QDialog>
#include "ki_exec_query.h"

#include "s_data_app.h"

namespace Ui {
class export_pers_by_distrs_wnd;
}

class export_pers_by_distrs_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных

    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;

    bool print_registry_SMS();

public:
    explicit export_pers_by_distrs_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~export_pers_by_distrs_wnd();

private slots:
    void on_bn_close_clicked();

    void on_bn_fname_clicked();

    void on_bn_save_to_csv_clicked();

private:
    Ui::export_pers_by_distrs_wnd *ui;
};

#endif // export_pers_by_distrs_wnd_h
