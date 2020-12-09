#ifndef TALKS_WND_H
#define TALKS_WND_H

#include <QDialog>
#include <QtSql>

#include "s_data_talk.h"
#include "add_talk_wnd.h"
#include "ki_exec_query.h"
#include "ki_sqlquerymodel.h"

namespace Ui {
class talks_wnd;
}

class talks_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    exec_SQL mySQL;

    QString sql_talks;
    ki_talksSqlQueryModel model_talks;
    QString sql_talks_2;
    ki_talksSqlQueryModel model_talks_2;

    s_data_talk &data_talks;
    s_data_app &data_app;

    add_talk_wnd *add_talk_w;

    void refresh_talks_tab();
    void refresh_persons();
    void refresh_talks_tab_2();

public:
    explicit talks_wnd(QSqlDatabase &db, s_data_talk &data_talks, s_data_app &data_app, QWidget *parent = 0);
    ~talks_wnd();

private slots:
    void on_bn_close_clicked();

    void on_bn_refresh_clicked();

    void on_bn_add_clicked();

    void on_bn_edit_clicked();

    void on_talks_tab_activated(const QModelIndex &index);

    void on_bn_delete_clicked();

    void on_ch_other_clicked(bool checked);

    void on_ln_fam_editingFinished();

    void on_ln_im_editingFinished();

    void on_ln_ot_editingFinished();

    void on_bn_refresh_2_clicked();

    void on_bn_copy_all_clicked();

    void on_combo_person_currentIndexChanged(int index);

private:
    Ui::talks_wnd *ui;
};

#endif // TALKS_WND_H
