#ifndef MATCH_ASSIG_TFOMS_WND_H
#define MATCH_ASSIG_TFOMS_WND_H

#include <QtSql>
#include <QDialog>
#include <QMessageBox>

namespace Ui {
class match_assig_TFOMS_wnd;
}

typedef struct {
    QString pack_name;
    QDate date_assig;
} s_data_match_assig;

class match_assig_TFOMS_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db_ODBC;    // это - ODBC-окошко для генерации dbf

    s_data_match_assig &data_match_assig;

    void refresh_tablist();

public:
    explicit match_assig_TFOMS_wnd(QSqlDatabase &db_ODBC, s_data_match_assig &data_match_assig, QWidget *parent = 0);
    ~match_assig_TFOMS_wnd();

private slots:
    void on_bn_cansel_clicked();

    void on_bn_date_assig_clicked();

    void on_comboBox_activated(const QString &arg1);

    void on_date_assig_dateChanged(const QDate &date);

    void on_bn_process_clicked();

private:
    Ui::match_assig_TFOMS_wnd *ui;
};

#endif // MATCH_ASSIG_TFOMS_WND_H
