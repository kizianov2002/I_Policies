#ifndef add_talk_wnd_H
#define add_talk_wnd_H

#include <QDialog>
#include <QtSql>

#include "s_data_talk.h"
#include "ki_exec_query.h"

namespace Ui {
class add_talk_wnd;
}

class add_talk_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    QList<int> pol_vers;
    QList<int> pol_ids;
    QList<int> blank_ids;

    exec_SQL mySQL;

    s_data_talk *data_talk;
    s_data_app &data_app;

    void refresh_polises();

public:
    explicit add_talk_wnd(QSqlDatabase &db, s_data_talk *data_talk, s_data_app &data_app, QWidget *parent = 0);
    ~add_talk_wnd();

private slots:
    void on_bn_close_clicked();

    void on_bn_save_clicked();

    void on_date_talk_dateChanged(const QDate &date);

    void on_combo_type_talk_currentIndexChanged(int index);

private:
    Ui::add_talk_wnd *ui;
};

#endif // add_talk_wnd_H
