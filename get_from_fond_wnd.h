#ifndef GET_FROM_FOND_WND_H
#define GET_FROM_FOND_WND_H

#include <QDialog>
#include <QtSql>
#include <QSettings>

#include "s_data_app.h"

#include "ki_exec_query.h"

namespace Ui {
class get_from_fond_wnd;
}

// данные записи
typedef struct {
    // запись
    int n_rec;
    QString id_tfoms;
    int erp_code;
    QString comment;
    // страхование
    QString ocato;
    QString enp;
    QString ogrn_smo;
    int     in_erp;
    // полис
    int     pol_v;
    bool    has_ser;
    QString pol_ser;
    QString pol_num;
    bool    has_date_begin, has_date_end, has_date_stop;
    QDate   date_begin, date_end, date_stop;
} s_p_rec;

typedef QList <s_p_rec> l_p_recs;

// данные файла
typedef struct {
    QString vers;
    QString filename;
    QString packname;
    QString smo_regnum;
    QString point_regnum;
    int n_recs;
    int n_errs;
    l_p_recs recs;
} s_p_file;

class get_from_fond_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;

    s_p_file data_p_file;

    QStringList getDirFiles(const QString& dirName);

    void process_P_file (QString fileName);
    void process_S_file (QString fileName);
    void process_K_file (QString fileName);
    void process_F_file (QString fileName);
    void process_U_file (QString fileName);

public:
    explicit get_from_fond_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~get_from_fond_wnd();

private slots:
    void on_bn_close_clicked();

    void on_bn_files_clicked();

    void on_bn_process_clicked();

private:
    Ui::get_from_fond_wnd *ui;
};

#endif // GET_FROM_FOND_WND_H
