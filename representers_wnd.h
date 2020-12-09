#ifndef representers_WND_H
#define representers_WND_H

#include <QDialog>
#include <QtSql>
#include <QTime>

#include "s_data_app.h"
#include "s_filter_pers.h"
#include "s_data_pers.h"
#include "s_data_doc.h"

#include "ki_exec_query.h"

#include "ki_SqlQueryModel.h"

namespace Ui {
class representers_wnd;
}

class representers_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    ki_persSqlQueryModel model_representers;
    QSqlQueryModel model_pers_docs;
    exec_SQL mySQL;

    QLabel *statusLab;

    QSettings &settings;
    s_data_app &data_app;
    s_filter_pers filter_pers;
    s_data_pers data_pers;
    s_data_doc data_doc;

    QStringList doc_ser_mask, doc_num_mask;

    void b_need_save(bool f=true);

public:
    int rep_id;
    QString rep_fio;
    int rep_sex;

    explicit representers_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~representers_wnd();

    void test_for_UDL();        // проверка, есть ли у человека нужные документы

    void refresh_representers_tab();
    void refresh_pers_data();
    void refresh_pers_docs();
    void refresh_pers_pan_category();
    void refresh_docs_pan_type();

    QString myMASK(QString text, QString mask);         // проверка строки на соответствие маске
    //bool myBYTEA(QString &fname, QString &res_str);     // перевод массива байт в строку bytea PostgreSQL   // перенёс в exec_sql
    bool print_registry();                              // печать рееста ВС

    int add_event(int id_person, int id_police, QString event_cide, QDateTime event_dt);

private slots:
    void on_bn_close_clicked();
    void on_act_close_triggered();

    void on_bn_pers_refresh_clicked();

    void on_line_pers_fam_textChanged(const QString &arg1);
    void on_line_pers_im_textChanged(const QString &arg1);
    void on_line_pers_ot_textChanged(const QString &arg1);
    void on_line_pers_snils_textChanged(const QString &arg1);
    void on_combo_pers_sex_currentIndexChanged(int index);
    void on_date_pers_pan_birth_dateChanged(const QDate &date);
    void on_ch_pers_pan_ot_clicked(bool checked);
    void on_combo_pers_pan_sex_currentIndexChanged(int index);
    void on_line_pers_pan_snils_textChanged(const QString &arg1);
    void on_ch_pers_pan_snils_clicked(bool checked);
    void on_date_pers_pan_death_dateChanged(const QDate &date);
    void on_ch_pers_pan_death_clicked(bool checked);
    void on_ln_pers_pan_plc_birth_textChanged(const QString &arg1);
    void on_date_pers_pan_reg_dateChanged(const QDate &date);

    void on_date_pers_birth_dateChanged(const QDate &date);

    void on_ch_pers_birth_clicked(bool checked);

    void on_bn_pers_clear_clicked();

    void on_bn_pers_new_clicked();
    void on_bn_pers_select_clicked();
    void on_bn_pers_pers_pan_back_clicked();
    void on_bn_pers_save_clicked();
    void on_bn_pers_document_clicked();
    void on_bn_pers_doc_back_clicked();
    void on_bn_select_rep_clicked();

    void on_show_pers_tabl_clicked(bool checked);
    void on_show_pers_data_clicked(bool checked);
    void on_show_pers_docs_clicked(bool checked);

    void on_ln_pers_pan_fam_textChanged(const QString &arg1);
    void on_ln_pers_pan_im_textChanged(const QString &arg1);
    void on_ln_pers_pan_ot_textChanged(const QString &arg1);

    void on_ch_pers_pan_ot_stateChanged ( int state );
    void on_ch_pers_pan_snils_stateChanged ( int state );
    void on_ch_pers_pan_death_stateChanged ( int state );

    void refresh_docs_pan_orgcodes();

    void on_bn_docs_refresh_clicked();
    void on_bn_pers_docs_back_clicked();
    void on_bn_docs_new_clicked();
    void on_bn_docs_edit_clicked();
    void on_bn_docs_save_clicked();
    void on_bn_docs_del_clicked();

    void on_tab_pers_docs_clicked(const QModelIndex &index);

    void on_ch_docs_pan_exp_stateChanged(int arg1);

    void on_tab_representers_activated(const QModelIndex &index);

    void on_line_docs_pan_ser_textEdited(const QString &arg1);
    void on_line_docs_pan_num_textEdited(const QString &arg1);

    void on_combo_docs_pan_type_activated(int index);

    void on_bn_docs_cansel_clicked();

    void on_combo_pers_pan_category_activated(int index);
    void on_line_docs_pan_ser_textChanged(const QString &arg1);
    void on_line_docs_pan_num_textChanged(const QString &arg1);
    void on_date_docs_pan_start_dateChanged(const QDate &date);
    void on_date_docs_pan_exp_dateChanged(const QDate &date);
    void on_ch_docs_pan_actual_clicked(bool checked);

    void on_tab_pers_docs_doubleClicked(const QModelIndex &index);

    void on_combo_pers_pan_category_activated(const QString &arg1);

    void on_ch_docs_tab_only_acts_stateChanged(int arg1);

    void on_ln_pers_pan_phone_code_textChanged(const QString &arg1);
    void on_ln_pers_pan_phone_home_textChanged(const QString &arg1);
    void on_ln_pers_pan_phone_work_textChanged(const QString &arg1);
    void on_ln_pers_pan_phone_cell_textChanged(const QString &arg1);
    void on_ln_pers_pan_email_textChanged(const QString &arg1);

    void on_ch_pers_fam_clicked();
    void on_ch_pers_im_clicked();
    void on_ch_pers_ot_clicked();
    void on_ch_pers_sex_clicked();
    void on_ch_pers_birth_clicked();
    void on_ch_pers_snils_clicked();

    void on_combo_docs_pan_orgcode_currentIndexChanged(const QString &arg1);
    void on_combo_docs_pan_orgcode_editTextChanged(const QString &arg1);

    void on_bn_add_orgcode_clicked();

    void on_combo_docs_pan_orgcode_currentTextChanged(const QString &arg1);

    void on_bn_del_orgcode_clicked();

    void on_spin_docs_pan_type_valueChanged(int arg1);

    void on_spin_pers_pan_category_valueChanged(int arg1);

    void on_ln_pers_pan_plc_birth_editingFinished();

    void on_date_pers_pan_birth_editingFinished();
    void on_line_pers_pan_snils_editingFinished();
    void on_date_pers_pan_death_editingFinished();

    void on_ln_pers_pan_phone_code_editingFinished();
    void on_ln_pers_pan_phone_work_editingFinished();
    void on_ln_pers_pan_phone_home_editingFinished();
    void on_ln_pers_pan_phone_cell_editingFinished();
    void on_ln_pers_pan_email_editingFinished();

    void on_spin_pers_pan_category_editingFinished();

    void on_ch_docs_clicked(bool checked);
    void on_ch_phones_clicked(bool checked);
    void on_ch_person_old_clicked(bool checked);

    void on_ln_pers_pan_fam_old_textChanged(const QString &arg1);
    void on_ln_pers_pan_im_old_textChanged(const QString &arg1);
    void on_ln_pers_pan_ot_old_textChanged(const QString &arg1);
    void on_combo_pers_pan_sex_old_currentIndexChanged(int index);
    void on_date_pers_pan_birth_old_dateChanged(const QDate &date);
    void on_line_pers_pan_snils_old_textChanged(const QString &arg1);
    void on_ln_pers_pan_plc_birth_old_textChanged(const QString &arg1);

    void on_ch_pers_pan_fam_old_clicked(bool checked);
    void on_ch_pers_pan_im_old_clicked(bool checked);
    void on_ch_pers_pan_ot_old_clicked(bool checked);
    void on_ch_pers_pan_sex_old_clicked(bool checked);
    void on_ch_pers_pan_snils_old_clicked(bool checked);
    void on_ch_pers_pan_date_birth_old_clicked(bool checked);
    void on_ch_pers_pan_plc_birth_old_clicked(bool checked);

    void on_line_pers_snils_textEdited(const QString &arg1);

    void on_ch_dost_clicked(bool checked);

    void on_ch_dost_fam_clicked(bool checked);
    void on_ch_dost_im_clicked(bool checked);
    void on_ch_dost_ot_clicked(bool checked);
    void on_ch_dost_day_clicked(bool checked);
    void on_ch_dost_mon_clicked(bool checked);
    void on_ch_dost_year_clicked(bool checked);

private:
    Ui::representers_wnd *ui;
};

#endif // representers_WND_H
