#ifndef QUEST2FOND_WND_H
#define QUEST2FOND_WND_H

#include <QDialog>

#include "s_data_app.h"
#include "ki_exec_query.h"

//#include "persons_wnd.h"
class persons_wnd;

typedef struct {
    int mode;   // 1 - формирование запроса
                // 2 - просмотр ответов

    int id_person;
    QString guid;
    QString note;
    QString fam;
    QString im;
    QString ot;
    int w;
    QDate date_birth;
    QDate date_death;
    QString snils;

    int id_udl;
    int doc_type;
    QString doc_ser;
    QString doc_num;

    int id_polis;
    int pol_v;
    QString pol_ser;
    QString pol_num;
    QDate date_begin;
    QDate date_end;

    QString QOGRN;
    float SUO;
    QString OGRNSMO;
} s_data_quest2fond;

namespace Ui {
class quest2fond_wnd;
}

class quest2fond_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    QSqlDatabase db_ODBC;    // это - ODBC-окошко для генерации dbf
    QSqlQueryModel model_pack;
    QSqlQueryModel model_reply;
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;
    persons_wnd *persons_w;

    s_data_quest2fond &data_quest;

    void refresh_combo_doc_type();
    void refresh_combo_pol_v();
    void refresh_pack_tab();

    void refresh_reply_tab();

public:
    explicit quest2fond_wnd(QSqlDatabase &db, s_data_quest2fond &data_quest, s_data_app &data_app, QSettings &settings, persons_wnd *persons_w, QWidget *parent = 0);
    ~quest2fond_wnd();

private slots:
    void on_bn_gen_dbf_clicked();

    void on_bn_close_clicked();

    void on_ln_fam_editingFinished();
    void on_ln_im_editingFinished();
    void on_ln_ot_editingFinished();
    void on_combo_sex_activated(int index);
    void on_date_birth_dateChanged(const QDate &date);
    void on_ln_snils_editingFinished();
    void on_combo_doc_type_activated(int index);
    void on_ln_doc_ser_editingFinished();
    void on_ln_doc_num_editingFinished();
    void on_date_begin_dateChanged(const QDate &date);
    void on_date_end_dateChanged(const QDate &date);
    void on_ln_QOGRN_editingFinished();
    void on_ln_OGRNSMO_editingFinished();
    void on_ch_date_birth_clicked(bool checked);
    void on_ch_date_begin_clicked(bool checked);
    void on_ch_date_end_clicked(bool checked);
    void on_ch_SUO_clicked(bool checked);

    void on_ch_PERSON_clicked(bool checked);

    void on_ch_UDL_clicked(bool checked);

    void on_ch_POLIS_clicked(bool checked);

    void on_ch_SMO_clicked(bool checked);

    void on_bn_clear_dbf_clicked();

    void on_bn_send_to_TFOMS_clicked();

    void on_bn_get_from_TFOMS_clicked();

    void on_date_now_dateChanged(const QDate &date);

    void on_bn_today_clicked();

    void on_quest2fond_wnd_accepted();
    //void on_quest2fond_wnd_finished(int result);
    //void on_quest2fond_wnd_rejected();
    //void on_quest2fond_wnd_destroyed();

    void on_ch_filter_fio_clicked(bool checked);

    void on_ch_filter_date_clicked(bool checked);

    void on_line_fam_editingFinished();

    void on_line_im_editingFinished();

    void on_line_ot_editingFinished();

    void on_bn_clear_tab_clicked();

    void on_bn_to_pers_clicked();

    void on_tab_reply_activated(const QModelIndex &index);

private:
    Ui::quest2fond_wnd *ui;
};

#endif // QUEST2FOND_WND_H
