#ifndef EXPORT2CSV_WND_H
#define EXPORT2CSV_WND_H

#include <QDialog>
#include <QtSql>

namespace Ui {
class export2csv_wnd;
}
#include "s_data_app.h"
#include "ki_exec_query.h"

class export2csv_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;

public:
    explicit export2csv_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~export2csv_wnd();

private slots:
    void on_pushButton_clicked();

    void on_ch_events_clicked(bool checked);

    void on_ch_vizits_clicked(bool checked);

    void on_ch_polises_clicked(bool checked);

    void on_ch_persons_clicked(bool checked);

    void on_bn_go_clicked();

    void on_ch_pers_fio_clicked();

    void on_ch_assigs_clicked(bool checked);

    void on_ch_reps_clicked(bool checked);

    void on_bn_fname_clicked();

private:
    Ui::export2csv_wnd *ui;
};

#endif // EXPORT2CSV_WND_H
