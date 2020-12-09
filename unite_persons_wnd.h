#ifndef UNITE_PERSONS_WND_H
#define UNITE_PERSONS_WND_H

#include <QDialog>
#include <QtSql>
#include "ki_exec_query.h"

#include "s_data_app.h"

typedef struct {
    int id_person1, id_person2;
    QString fam, im, ot;
    QString person1_fio;
    bool f_documents, f_photo, f_assigs, f_vizits, f_polises;
}s_data_pers_unite;

namespace Ui {
class unite_persons_wnd;
}

class unite_persons_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;
    exec_SQL mySQL;

    s_data_pers_unite &data_pers_unite;
    s_data_app &data_app;
    QSettings &settings;

    void refresh_persons();

public:
    explicit unite_persons_wnd(QSqlDatabase &db, s_data_pers_unite &data_pers_unite, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    explicit unite_persons_wnd(QWidget *parent = 0);
    ~unite_persons_wnd();

private slots:
    void on_bn_cansel_clicked();

    void on_ln_filter_fam_editingFinished();

    void on_ln_filter_im_editingFinished();

    void on_ln_filter_ot_editingFinished();

    void on_bn_process_clicked();

private:
    Ui::unite_persons_wnd *ui;
};

#endif // UNITE_PERSONS_WND_H
