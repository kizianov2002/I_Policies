#ifndef match_TFOMS_WND_H
#define match_TFOMS_WND_H

#include <QtSql>
#include <QDialog>
#include <QMessageBox>

#include "s_data_app.h"


namespace Ui {
class match_TFOMS_wnd;
}

typedef struct {
    QString pack_name;
    QDate date_match;
} s_data_match;

class match_TFOMS_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db_ODBC;    // это - ODBC-окошко для генерации dbf

    s_data_app &data_app;
    s_data_match &data_match;

public:
    explicit match_TFOMS_wnd(QSqlDatabase &db_ODBC, s_data_match &data_match, s_data_app &data_app, QWidget *parent = 0);
    ~match_TFOMS_wnd();

private slots:
    void on_bn_cansel_clicked();

    void on_bn_date_match_clicked();

    void on_comboBox_activated(const QString &arg1);

    void on_date_match_dateChanged(const QDate &date);

    void on_bn_process_clicked();

    void on_bn_file_dbf_clicked();

private:
    Ui::match_TFOMS_wnd *ui;
};

#endif // match_TFOMS_WND_H
