#ifndef print_pays2mo_wnd_H
#define print_pays2mo_wnd_H

#include <QDialog>

#include "s_data_app.h"
#include "s_data_pers.h"
#include "s_data_doc.h"

#include "ki_exec_query.h"

// параметры генерации заявления
typedef struct {
    int     id_person;
    QString guid;
    QString note;
    QString fam;
    QString im;
    QString ot;
    QDate   date_birth;
    int     sex;

    QString phone_cell;
    QString phone_home;

    int     udl_type;
    QString udl_name;
    QString udl_ser;
    QString udl_num;
    QString udl_code;
    QString udl_org;
    QDate   udl_date;

} s_data_req_pays2mo;

// строка справки
typedef struct {
    QString mo_name;

    //int     n;
    QString usl_ok;
    QString usl_dates;
    QString usl_name;
    float   pay;

} s_data_pays2mo_ln;

// параметры генерации справки
typedef struct {
    int     id;
    int     id_person;
    int     id_point;
    int     id_operator;
    int     n_ref;
    QString fam;
    QString im;
    QString ot;
    QDate   date_birth;
    int     sex;

    QDate   date_from;
    QDate   date_to;
    QDate   date_req;
    QDate   date_take;
    float   pay_sum;
    int     status;

    QList <s_data_pays2mo_ln> lines;
    QString fio_oper;

} s_data_pays2mo;

namespace Ui {
class print_pays2mo_wnd;
}

class print_pays2mo_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    QSqlDatabase db_scores;    // это - база данных счетов
    QSqlQueryModel model_polises;
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;
    s_data_req_pays2mo &data_req_pays2mo;
    s_data_pers &data_pers;
    s_data_doc &data_doc;

    // данные генерации справки
    s_data_pays2mo data_pays2mo;

    QString date_to_str(QDate date);
    bool test_for_docs();

public:
    explicit print_pays2mo_wnd(QSqlDatabase &db, s_data_app &data_app, s_data_req_pays2mo &data_req_pays2mo, s_data_pers &data_pers, s_data_doc &data_doc, QSettings &settings, QWidget *parent = 0);
    ~print_pays2mo_wnd();

private slots:
    void on_bn_close_clicked();

    void on_bn_edit_clicked(bool checked);

    void on_date_from_userDateChanged(const QDate &date);
    void on_date_to_userDateChanged(const QDate &date);

    void on_ch_self_UDL_clicked();
    void on_ch_self_birth_cert_clicked();
    void on_ch_legal_UDL_clicked();
    void on_ch_notary_cert_clicked();
    void on_ch_notary_UDL_clicked();

    void on_rb_by_self_clicked();

    void on_rb_legal_rep_clicked();

    void on_rb_notary_rep_clicked();

    void on_ch_scoresDB_clicked(bool checked);
    void on_bn_save_scoresDB_clicked();

    void on_bn_print_statement_clicked(bool checked);
    void on_bn_print_reference_clicked(bool checked);
    void on_bn_reference_ok_clicked(bool checked);

private:
    Ui::print_pays2mo_wnd *ui;
};

#endif // print_pays2mo_wnd_H
