#ifndef PERSONS_WND_H
#define PERSONS_WND_H

#include <QMainWindow>

#include "polices_wnd.h"
#include "representers_wnd.h"
#include "pvp_n_vs_wnd.h"
#include "sessions_wnd.h"
#include "blanks_wnd.h"
#include "work_calendar_wnd.h"
#include "settings_wnd.h"
#include "send_to_fond_wnd.h"
#include "get_from_fond_wnd.h"
#include "send_pol_sms_wnd.h"
#include "export2csv_wnd.h"
#include "medorgs_wnd.h"
#include "export_assig_wnd.h"
#include "export_assig_bystep_wnd.h"
#include "export_assig_by_mo_wnd.h"
#include "print_form_8_wnd.h"
#include "print_vs_by_date_wnd.h"
#include "print_enp_by_date_wnd.h"
#include "print_pers_sexage_wnd.h"
//#include "print_assig_sexage_all_mo_wnd.h"
#include "print_1st_aid_sexage_by_list_wnd.h"
#include "print_assig_sexage_by_list_wnd.h"
#include "print_assig_letters_by_list_wnd.h"
#include "print_pol_pg_wnd.h"
#include "print_pol_monitoring_wnd.h"
#include "change_polis_wnd.h"
#include "change_vizit_wnd.h"
#include "list_assig_medorgs_wnd.h"
#include "list_persons_terrs_wnd.h"
#include "protocols_wnd.h"
#include "add_blank_wnd.h"
#include "add_blank_scan_wnd.h"
#include "name_to_sex_wnd.h"
#include "namepat_to_sex_wnd.h"
#include "info_pers_doubles_wnd.h"
#include "assig_gaps_wnd.h"
#include "quest2fond_wnd.h"
#include "print_pays2mo_wnd.h"
#include "print_pays2mo_rep_pt_wnd.h"
#include "folder_wnd.h"
#include "match_tfoms_wnd.h"
#include "match_assig_tfoms_wnd.h"
#include "export_pers_by_distrs_wnd.h"
#include "msg_wnd.h"
#include "terminals_wnd.h"
#include "add_talk_wnd.h"
#include "talks_wnd.h"
#include "show_tab_wnd.h"
#include "packs_wnd.h"
#include "statistic_acts_wnd.h"
#include "unite_persons_wnd.h"
#include "blanks_pol_act_wnd.h"
#include "print_subconto_wnd.h"
#include "print_vs_register_by_date_wnd.h"
#include "get_date_wnd.h"
#include "print_expired_vs_statistic_wnd.h"
#include "acts_wnd.h"
#include "finder_wnd.h"
#include "beep_guten_morgen_wnd.h"
#include "spr_assig_errors_wnd.h"
#include "spr_insure_errors_wnd.h"
#include "print_dispanserize_plans_wnd.h"
#include "resend_events_wnd.h"
#include "get_year_wnd.h"
#include "disp_plans_wnd.h"
#include "print_assiglay_todate_wnd.h"
#include "print_operator_timeline_wnd.h"
#include "beep_30_min_wnd.h"
#include "blanks_bso_wnd.h"


#include "tfomsFiles_wnd.h"
#include "fiasFiles_wnd.h"

#include "s_data_app.h"
#include "s_filter_pers.h"
#include "s_data_pers.h"
#include "s_data_doc.h"
#include "s_data_assig.h"
#include "s_data_blank.h"

#include "ki_exec_query.h"
#include "ki_funcs.h"
#include "ki_SqlQueryModel.h"

class protocols_wnd;

namespace Ui {
class persons_wnd;
}

class persons_wnd : public QMainWindow
{
    Q_OBJECT
    QSqlDatabase &db;       // это - наша база данных
    QSqlDatabase db_FIAS;   // это - база данных ФИАС
    QSqlDatabase db_ODBC;   // это - ODBC-окошко для генерации dbf
    ki_persSqlQueryModel model_persons;
    ki_polSqlQueryModel model_polises;
    QSqlQueryModel model_events;
    ki_docsSqlQueryModel model_pers_docs;
    exec_SQL mySQL;

    QLabel *statusLab;
    QLabel *statusOper;

    QSettings &settings;
    s_data_app &data_app;
    s_filter_pers filter_pers;
    s_data_pers data_pers;
    s_data_doc data_doc;
    //s_data_assig data_assig;

    QString path_to_photo, path_to_sign;

    finder_wnd *finder_w;
    folder_wnd *folder_w;
    representers_wnd *representers_w;
    polices_wnd *polices_w;
    pvp_n_vs_wnd *pvp_n_vs_w;
    blanks_wnd *blanks_w;
    work_calendar_wnd *work_calendar_w;
    sessions_wnd *sessions_w;
    settings_wnd *settings_w;
    send_to_fond_wnd *send_to_fond_w;
    get_from_fond_wnd *get_from_fond_w;
    send_pol_sms_wnd *send_pol_sms_w;
    export2csv_wnd *export2csv_w;
    export_assig_wnd *export_assig_w;
    export_assig_by_mo_wnd *export_assig_by_mo_w;
    export_assig_bystep_wnd *export_assig_bystep_w;
    print_form_8_wnd *print_form_8_w;
    print_vs_by_date_wnd *print_vs_by_date_w;
    print_enp_by_date_wnd *print_enp_by_date_w;
    print_pers_sexage_wnd *print_pers_sexage_w;
    //print_assig_sexage_all_mo_wnd *print_assig_sexage_all_mo_w;
    print_1st_aid_sexage_by_list_wnd *print_1st_aid_sexage_by_list_w;
    print_assig_sexage_by_list_wnd *print_assig_sexage_by_list_w;
    print_assig_letters_by_list_wnd *print_assig_letters_by_list_w;
    print_pol_pg_wnd *print_pol_pg_w;
    print_pol_monitoring_wnd *print_pol_monitoring_w;
    change_polis_wnd *change_polis_w;
    change_vizit_wnd *change_vizit_w;
    protocols_wnd *protocols_w;
    tfomsFiles_wnd *tfomsFiles_w;
    fiasFiles_wnd *fiasFiles_w;
    list_persons_terrs_wnd *list_persons_terrs_w;
    list_assig_medorgs_wnd *list_assig_medorgs_w;
    add_blank_scan_wnd *add_blank_scan_w;
    add_blank_wnd *add_blank_w;
    name_to_sex_wnd *name_to_sex_w;
    namepat_to_sex_wnd *namepat_to_sex_w;
    info_pers_doubles_wnd *info_pers_doubles_w;
    quest2fond_wnd *quest2fond_w;
    print_pays2mo_wnd *print_pays2mo_w;
    print_pays2mo_rep_pt_wnd *print_pays2mo_rep_pt_w;
    match_TFOMS_wnd *match_tfoms_w;
    match_assig_TFOMS_wnd *match_assig_tfoms_w;
    export_pers_by_distrs_wnd *export_pers_by_distrs_w;
    terminals_wnd *terminals_w;
    add_talk_wnd *add_talk_w;
    talks_wnd *talks_w;
    show_tab_wnd *show_tab_w;
    packs_wnd *packs_w;
    statistic_acts_wnd *statistic_acts_w;
    unite_persons_wnd *unite_persons_w;
    blanks_pol_act_wnd *blanks_pol_act_w;
    print_subconto_wnd *print_subconto_w;
    print_vs_register_by_date_wnd *print_vs_register_by_date_w;
    get_date_wnd *get_date_w;
    print_expired_vs_statistic_wnd *print_expired_vs_statistic_w;
    acts_wnd *acts_w;
    beep_Guten_Morgen_wnd *Gutten_Morgen_w;
    spr_assig_errors_wnd *spr_assig_errors_w;
    spr_insure_errors_wnd *spr_insure_errors_w;
    print_dispanserize_plans_wnd *print_dispanserize_plans_w;
    resend_events_wnd *resend_events_w;
    get_year_wnd *get_year_w;
    disp_plans_wnd *disp_plans_w;
    print_assiglay_todate_wnd *print_assiglay_todate_w;
    print_operator_timeline_wnd *print_operator_timeline_w;
    beep_30_min_wnd *beep_30_min_w;
    blanks_BSO_wnd *blanks_bso_w;

    // отобразение сообщения на главном окне
    int id_msg;
    QString msg;
    QDateTime msg_dt;

    msg_wnd *msg_w;
    QTimer *msg_timer;
    int msg_counter;
    bool msg_f;
    bool msg_a;
    QColor msg_bn_color;
    QColor msg_lt_color;

    QTimer *qik_timer;
    int quick_counter;

    bool can_refresh_persons_tab;
    QString sql_pers_flds;
    QString sql_pers_from;
    QString sql_pers_all;
    QString sql_pers_lim;
    QString sql_pers_cnt;

    QString sql_subj_reg;
    QString sql_dstr_reg;
    QString sql_city_reg;
    QString sql_nasp_reg;
    QString sql_strt_reg;
    QString sql_hous_reg;

    QString sql_subj_liv;
    QString sql_dstr_liv;
    QString sql_city_liv;
    QString sql_nasp_liv;
    QString sql_strt_liv;
    QString sql_hous_liv;

    QStringList okato_reg_subj, okato_reg_dstr, okato_reg_city, okato_reg_nasp, okato_reg_strt, okato_reg_hous;
    QStringList okato_liv_subj, okato_liv_dstr, okato_liv_city, okato_liv_nasp, okato_liv_strt, okato_liv_hous;
    QStringList kladr_reg_subj, kladr_reg_dstr, kladr_reg_city, kladr_reg_nasp, kladr_reg_strt, kladr_reg_hous;
    QStringList kladr_liv_subj, kladr_liv_dstr, kladr_liv_city, kladr_liv_nasp, kladr_liv_strt, kladr_liv_hous;
    QStringList index_reg_subj, index_reg_dstr, index_reg_city, index_reg_nasp, index_reg_strt, index_reg_hous;
    QStringList index_liv_subj, index_liv_dstr, index_liv_city, index_liv_nasp, index_liv_strt, index_liv_hous;

    QStringList fiass_reg_subj, fiass_reg_dstr, fiass_reg_city, fiass_reg_nasp, fiass_reg_strt, fiass_reg_hous;
    QStringList fiass_liv_subj, fiass_liv_dstr, fiass_liv_city, fiass_liv_nasp, fiass_liv_strt, fiass_liv_hous;

    QStringList fguid_reg_subj, fguid_reg_dstr, fguid_reg_city, fguid_reg_nasp, fguid_reg_strt, fguid_reg_hous;
    QStringList fguid_liv_subj, fguid_liv_dstr, fguid_liv_city, fguid_liv_nasp, fguid_liv_strt, fguid_liv_hous;

    QStringList reg_strts;
    QList<int>  reg_strts_n;
    QStringList liv_strts;
    QList<int>  liv_strts_n;

    QStringList doc_ser_mask, doc_num_mask;
    QStringList doc_org_code, doc_org_name;

    void b_retro_data(bool f=true);
    void b_need_save(bool f=true);

    QString test_info_1();
    QString test_info_2();
    QString test_info_3();

    bool refresh_fias_connection();


    QString s_info_1;
    QString s_info_2;
    QString s_info_3;
    bool f_info_1;
    bool f_info_2;
    bool f_info_3;


    QString month_to_str(QDate date);

public:
    explicit persons_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent = 0);
    ~persons_wnd();

    void finder_off() ;

    bool test_for_FIAS(bool can_stop);     // проверка, есть ли коды ФИАС
    bool test_for_EAES(bool can_stop);     // проверка ввода данных гражданина ЕАЭС - сейчас ничего не делает
    bool test_for_doubles(bool can_stop);  // проверка записи персоны на дубли. 
    bool test_for_sex(bool can_stop);      // проверка на соответствие имени полу
    bool test_for_SNILS(QString SNILS, int oksm_c, int category, QDate date_birth);    // проверка контрольной суммы СНИЛС
    bool test_for_UDL(bool can_stop, bool f_quick);      // проверка, есть ли у человека нужные документы
                                           // can_stop влияет только на текст сообщения в диалоге "Всё равно сохранить ... " если проверка делается во время сохранения
    void refresh_folders();                // can_stop влияет только на текст сообщения в диалоге "Всё равно сохранить ... " если проверка делается во время сохранения
    void refresh_pers_years_mons();
    void refresh_persons_tab();
    void refresh_polises_tab(int id_person, int id_polis);
    void refresh_events_tab(int id_polis);
    void refresh_pers_data();
    void refresh_pers_docs();
    void refresh_pers_photo();
    void refresh_pers_sign();
    void refresh_pers_eaes();
    void refresh_pers_assig();
    void refresh_pers_pan_oksm();
    void refresh_pers_pan_category();
    void refresh_docs_pan_type();
    void refresh_pers_assig_mo();
    void refresh_pers_assig_mp();
    void refresh_pers_assig_ms();
    void refresh_pers_assig_mt();
    void refresh_insure_agents();
    void refresh_pers_category();
    void refresh_pers_statuses();
    void refresh_pers_points();
    void refresh_pers_operators();
    void refresh_pers_dopdata();

    void select_person_in_tab(int id_person);
    void select_polis_in_tab(int id_polis);
    void select_event_in_tab(int id_event);

    bool save_pers_data();

    QString myMASK(QString text, QString mask);         // проверка строки на соответствие маске
    bool print_registry_vs();                           // печать рееста ВС
    bool print_registry_vs(QDate date1, QDate date2);                 // печать рееста ВС за дату
    bool print_registry_oms();                          // печать рееста полисов ОМС
    bool print_registry_oms(QDate date1, QDate date2);                // печать рееста полисов ОМС за дату

    int add_event (int id_person, int id_police, QString event_cide, QDateTime event_dt);

    void refresh_pers_pan_fiass_reg (int subj, int dstr, int city, int nasp, int strt, QString houseid);
    void refresh_pers_pan_fiass_liv (int subj, int dstr, int city, int nasp, int strt, QString houseid);

    bool reg_assig_by_terr_ok (QString ocato, int subj, int dstr, int city, int nasp, int strt, int hous, QString corp, int sex, int age);
    bool reg_assig_by_terr_gap(QString ocato, int subj, int dstr, int city, int nasp, int strt, int hous, QString corp, int sex, int age);

    void get_pers_to_TFOMS_data_from_perstab (s_data_quest2fond &data_quest);
    void get_pers_to_TFOMS_data_from_controls (s_data_quest2fond &data_quest);

    bool gen_disp_plan(int age);

private slots:
    void msg_update();
    void qik_update();

    void on_bn_close_clicked();
    void on_act_close_triggered();

    void on_bn_pers_refresh_clicked();

    void on_line_pers_fam_textChanged(const QString &arg1);
    void on_line_pers_im_textChanged(const QString &arg1);
    void on_line_pers_ot_textChanged(const QString &arg1);
    void on_line_pers_snils_textChanged(const QString &arg1);
    void on_line_pers_enp_textChanged(const QString &arg1);
    void on_line_pers_vs_textChanged(const QString &arg1);

    void on_combo_pers_sex_currentIndexChanged(int index);
    void on_date_pers_pan_birth_dateChanged(const QDate &date);
    void on_ch_pers_pan_ot_clicked(bool checked);
    void on_combo_pers_pan_sex_currentIndexChanged(int index);
    void on_line_pers_pan_snils_textChanged(const QString &arg1);
    void on_ch_pers_pan_snils_clicked(bool checked);
    void on_pan_rep_clicked(bool checked);
    void on_date_pers_pan_death_dateChanged(const QDate &date);
    void on_ch_pers_pan_death_clicked(bool checked);
    void on_ln_pers_pan_plc_birth_textChanged(const QString &arg1);
    void on_date_pers_pan_reg_dateChanged(const QDate &date);
    void on_ch_pers_pan_reg_clicked(bool checked);
    void on_ch_pers_pan_liv_clicked(bool checked);
    void on_ln_pers_pan_phone_code_textChanged(const QString &arg1);
    void on_ln_pers_pan_phone_cell_textChanged(const QString &arg1);
    void on_ln_pers_pan_phone_home_textChanged(const QString &arg1);
    void on_ln_pers_pan_phone_work_textChanged(const QString &arg1);
    void on_ln_pers_pan_email_textChanged(const QString &arg1);

    void on_date_pers_birth_dateChanged(const QDate &date);
    void on_date_pers_birth_2_dateChanged(const QDate &date);

    void on_ch_pers_birth_clicked(bool checked);

    void on_bn_pers_clear_clicked();

    void on_bn_pers_new_clicked();
    void on_bn_pers_select_clicked();    
    void on_bn_pers_polices_2_clicked();

    void on_bn_pers_pers_pan_back_clicked();
    void on_bn_pers_save_clicked();
    void on_bn_pers_document_clicked();
    void on_bn_pers_doc_back_clicked();
    void on_bn_pers_polices_clicked();

    void on_show_pers_tabl_clicked(bool checked);
    void on_show_pers_data_clicked(bool checked);
    void on_show_pers_docs_clicked(bool checked);

    void on_act_font_6_triggered();
    void on_act_font_8_triggered();
    void on_act_font_10_triggered();
    void on_act_font_12_triggered();
    void on_act_font_15_triggered();

    void on_ln_pers_pan_fam_textChanged(const QString &arg1);
    void on_ln_pers_pan_im_textChanged(const QString &arg1);
    void on_ln_pers_pan_ot_textChanged(const QString &arg1);

    void on_ch_pers_pan_ot_stateChanged ( int state );
    void on_pan_rep_toggled (bool arg1);
    void on_ch_pers_pan_snils_stateChanged ( int state );
    void on_ch_pers_pan_death_stateChanged ( int state );
    void on_ch_pers_pan_reg_stateChanged ( int state );
    void on_ch_pers_pan_liv_stateChanged ( int state );

    void on_combo_pers_pan_oksm_rname_currentIndexChanged(int index);
    void on_combo_pers_pan_oksm_cname_currentIndexChanged(int index);

    /*void on_ln_pers_pan_reg_subj_textChanged(const QString &arg1);
    void on_ln_pers_pan_reg_dstr_textChanged(const QString &arg1);
    void on_ln_pers_pan_reg_city_textChanged(const QString &arg1);
    void on_ln_pers_pan_reg_nasp_textChanged(const QString &arg1);
    void on_ln_pers_pan_reg_strt_textChanged(const QString &arg1);*/
    void on_combo_pers_pan_reg_subj_currentIndexChanged(int index);
    void on_combo_pers_pan_reg_dstr_currentIndexChanged(int index);
    void on_combo_pers_pan_reg_city_currentIndexChanged(int index);
    void on_combo_pers_pan_reg_nasp_currentIndexChanged(int index);
    void on_combo_pers_pan_reg_strt_currentIndexChanged(int index);

    /*void on_ln_pers_pan_liv_subj_textChanged(const QString &arg1);
    void on_ln_pers_pan_liv_dstr_textChanged(const QString &arg1);
    void on_ln_pers_pan_liv_city_textChanged(const QString &arg1);
    void on_ln_pers_pan_liv_nasp_textChanged(const QString &arg1);
    void on_ln_pers_pan_liv_strt_textChanged(const QString &arg1);*/
    void on_combo_pers_pan_liv_subj_currentIndexChanged(int index);
    void on_combo_pers_pan_liv_dstr_currentIndexChanged(int index);
    void on_combo_pers_pan_liv_city_currentIndexChanged(int index);
    void on_combo_pers_pan_liv_nasp_currentIndexChanged(int index);
    void on_combo_pers_pan_liv_strt_currentIndexChanged(int index);

    void on_combo_pers_pan_reg_subj_activated(int index);
    void on_combo_pers_pan_reg_dstr_activated(int index);
    void on_combo_pers_pan_reg_city_activated(int index);
    void on_combo_pers_pan_reg_nasp_activated(int index);    
    void on_combo_pers_pan_reg_strt_activated(int index);

    void on_combo_pers_pan_liv_subj_activated(int index);
    void on_combo_pers_pan_liv_dstr_activated(int index);
    void on_combo_pers_pan_liv_city_activated(int index);
    void on_combo_pers_pan_liv_nasp_activated(int index);
    void on_combo_pers_pan_liv_strt_activated(int index);

    void refresh_docs_pan_orgcodes();

    void on_bn_docs_refresh_clicked();
    void on_bn_pers_docs_back_clicked();
    void on_bn_docs_new_clicked();
    void on_bn_docs_edit_clicked();
    void on_bn_docs_save_clicked();
    void on_bn_docs_del_clicked();

    void on_tab_pers_docs_clicked(const QModelIndex &index);

    void on_ch_docs_pan_exp_stateChanged(int arg1);

    void on_tab_persons_activated(const QModelIndex &index);

    void on_line_docs_pan_ser_textEdited(const QString &arg1);
    void on_line_docs_pan_num_textEdited(const QString &arg1);

    void on_combo_docs_pan_type_activated(int index);

    void on_bn_docs_cansel_clicked();

    void on_bn_docs_pan_photo_set_clicked();
    void on_bn_docs_pan_sign_set_clicked();
    void on_bn_docs_pan_photo_del_clicked();
    void on_bn_docs_pan_sign_del_clicked();

    void on_combo_pers_pan_category_activated(int index);
    void on_line_docs_pan_ser_textChanged(const QString &arg1);
    void on_line_docs_pan_num_textChanged(const QString &arg1);
    void on_date_docs_pan_start_dateChanged(const QDate &date);
    void on_date_docs_pan_exp_dateChanged(const QDate &date);
    void on_ch_docs_pan_actual_clicked(bool checked);
    void on_ch_docs_pan_olddoc_clicked(bool checked);

    void on_tab_pers_docs_doubleClicked(const QModelIndex &index);

    void on_combo_pers_pan_category_activated(const QString &arg1);

    void on_ch_docs_tab_only_acts_stateChanged(int arg1);

    void on_act_export_tfoms_triggered();

    void on_bn_pers_pan_rep_clicked();

    void on_act_help_triggered();

    void on_act_about_triggered();

    void on_spin_pers_pan_oksm_r_valueChanged(int arg1);
    void on_spin_pers_pan_oksm_c_valueChanged(int arg1);

    void on_ln_pers_pan_reg_subj_textEdited(const QString &arg1);
    void on_ln_pers_pan_reg_dstr_textEdited(const QString &arg1);
    void on_ln_pers_pan_reg_city_textEdited(const QString &arg1);
    void on_ln_pers_pan_reg_nasp_textEdited(const QString &arg1);
    void on_ln_pers_pan_reg_strt_textEdited(const QString &arg1);
    void on_ln_pers_pan_reg_hous_textEdited(const QString &arg1);
    void on_ln_pers_pan_reg_corp_textEdited(const QString &arg1);
    void on_ln_pers_pan_reg_qart_textEdited(const QString &arg1);
    void on_ln_pers_pan_reg_index_textEdited(const QString &arg1);

    void on_ln_pers_pan_liv_subj_textEdited(const QString &arg1);
    void on_ln_pers_pan_liv_dstr_textEdited(const QString &arg1);
    void on_ln_pers_pan_liv_city_textEdited(const QString &arg1);
    void on_ln_pers_pan_liv_nasp_textEdited(const QString &arg1);
    void on_ln_pers_pan_liv_strt_textEdited(const QString &arg1);
    void on_ln_pers_pan_liv_hous_textEdited(const QString &arg1);
    void on_ln_pers_pan_liv_corp_textEdited(const QString &arg1);
    void on_ln_pers_pan_liv_qart_textEdited(const QString &arg1);
    void on_ln_pers_pan_liv_index_textEdited(const QString &arg1);

    void on_ln_pers_pan_phone_code_textEdited(const QString &arg1);
    void on_ln_pers_pan_phone_home_textEdited(const QString &arg1);
    void on_ln_pers_pan_phone_work_textEdited(const QString &arg1);
    void on_ln_pers_pan_phone_cell_textEdited(const QString &arg1);
    void on_ln_pers_pan_email_textEdited(const QString &arg1);

    void on_ch_pers_fam_clicked();
    void on_ch_pers_im_clicked();
    void on_ch_pers_ot_clicked();
    void on_ch_pers_sex_clicked();
    void on_ch_pers_birth_clicked();
    void on_ch_pers_snils_clicked();
    void on_ch_pers_vs_clicked();
    void on_ch_pers_enp_clicked();

    //void on_combo_docs_pan_orgcode_currentIndexChanged(const QString &arg1);
    //void on_combo_docs_pan_orgcode_editTextChanged(const QString &arg1);
    //void on_combo_docs_pan_orgcode_currentTextChanged(const QString &arg1);
    void on_combo_docs_pan_orgcode_activated(int index);

    void on_bn_add_orgcode_clicked();

    void on_bn_del_orgcode_clicked();

    void on_spin_pers_pan_category_valueChanged(int arg1);

    void on_act_gen_reester_vs_triggered();

    void on_act_gen_reester_oms_triggered();

    void on_combo_pers_pan_rep_rel_activated(int index);

    void on_act_PVP_n_VS_triggered();

    void on_act_blanks_triggered();

    void on_ln_pers_pan_plc_birth_editingFinished();

    //void on_ln_pers_pan_fam_editingFinished();
    //void on_ln_pers_pan_im_editingFinished();
    //void on_ln_pers_pan_ot_editingFinished();
    void on_date_pers_pan_birth_editingFinished();
    void on_line_pers_pan_snils_editingFinished();
    void on_date_pers_pan_death_editingFinished();
    void on_combo_pers_pan_oksm_rname_activated(const QString &arg1);
    void on_combo_pers_pan_oksm_cname_activated(const QString &arg1);

    void on_ln_pers_pan_reg_hous_editingFinished();
    void on_ln_pers_pan_reg_corp_editingFinished();
    void on_ln_pers_pan_reg_qart_editingFinished();
    void on_ln_pers_pan_reg_index_editingFinished();
    void on_ln_pers_pan_reg_text_editingFinished();
    void on_date_pers_pan_reg_editingFinished();
    void on_ln_pers_pan_liv_hous_editingFinished();
    void on_ln_pers_pan_liv_corp_editingFinished();
    void on_ln_pers_pan_liv_qart_editingFinished();
    void on_ln_pers_pan_liv_index_editingFinished();
    void on_ln_pers_pan_liv_text_editingFinished();

    void on_ln_pers_pan_phone_code_editingFinished();
    void on_ln_pers_pan_phone_work_editingFinished();
    void on_ln_pers_pan_phone_home_editingFinished();
    void on_ln_pers_pan_phone_cell_editingFinished();
    void on_ln_pers_pan_email_editingFinished();

    void on_spin_pers_pan_category_editingFinished();

    void on_ch_docs_clicked(bool checked);
    void on_ch_photo_clicked(bool checked);
    //void on_ch_address_clicked(bool checked);
    void on_ch_phones_clicked(bool checked);
    void on_ch_country_clicked(bool checked);
    void on_ch_rep_clicked(bool checked);
    void on_ch_person_clicked(bool checked);
    void on_ch_person_old_clicked(bool checked);
    void on_ch_eaes_clicked(bool checked);

    void on_act_work_calendar_triggered();

    void on_ln_pers_pan_fam_old_textChanged(const QString &arg1);
    void on_ln_pers_pan_im_old_textChanged(const QString &arg1);
    void on_ln_pers_pan_ot_old_textChanged(const QString &arg1);
    void on_combo_pers_pan_sex_old_currentIndexChanged(int index);
    void on_date_pers_pan_birth_old_dateChanged(const QDate &date);
    void on_line_pers_pan_snils_old_textChanged(const QString &arg1);
    void on_line_pers_pan_enp_old_textChanged(const QString &arg1);
    void on_ln_pers_pan_plc_birth_old_textChanged(const QString &arg1);

    void on_ch_pers_pan_fam_old_clicked(bool checked);
    void on_ch_pers_pan_im_old_clicked(bool checked);
    void on_ch_pers_pan_ot_old_clicked(bool checked);
    void on_ch_pers_pan_sex_old_clicked(bool checked);
    void on_ch_pers_pan_snils_old_clicked(bool checked);
    void on_ch_pers_pan_enp_old_clicked(bool checked);
    void on_ch_pers_pan_date_birth_old_clicked(bool checked);
    void on_ch_pers_pan_plc_birth_old_clicked(bool checked);

    void on_ch_pers_pan_reg_toggled(bool checked);
    void on_ch_pers_pan_liv_toggled(bool checked);

    void on_line_pers_pan_rep_textChanged(const QString &arg1);

    void on_act_INI_triggered();

    void on_tab_persons_clicked(const QModelIndex &index);

    void on_ch_only_act_clicked(bool checked);

    void on_act_import_tfoms_triggered();

    void on_act_sms_triggered();

    void on_ch_p040_clicked(bool checked);

    void on_bn_pers_pol_close_clicked();

    void on_tab_polises_clicked(const QModelIndex &index);

    void on_bn_pers_act_change_clicked();

    void on_ch_assig_clicked(bool checked);

    void on_act_to_CSV_triggered();

    void on_bn_pers_assig_mo_clicked();

    void on_combo_pers_assig_mo_activated(int index);
    void on_combo_pers_assig_mp_currentIndexChanged(const QString &arg1);
    void on_combo_pers_assig_ms_currentIndexChanged(const QString &arg1);
    void on_combo_pers_assig_mt_currentIndexChanged(const QString &arg1);

    void on_combo_pers_assig_type_currentIndexChanged(const QString &arg1);

    void on_date_pers_assig_date_editingFinished();

    void on_b_save_assig_clicked();

    void on_bn_assig_now_clicked();

    void on_act_assig_triggered();

    void on_bn_pers_act_resent_clicked();

    void on_bn_delete_assig_clicked();

    void on_bn_auto_assig_clicked();

    void on_act_form_8_triggered();

    void on_act_vs_blanks_triggered();

    void on_act_enp_blanks_triggered();

    void on_act_pers_sexage_triggered();

    void on_act_assig_sexage_by_list_triggered();

    //void on_act_assig_sexage_all_mo_triggered();

    void on_act_assig_by_MO_triggered();

    void on_act_assig_letters_by_list_triggered();

    void on_ch_pers_polis_clicked(bool checked);
    void on_ch_pers_polis_2_clicked(bool checked);
    void on_ch_pers_old_vs_clicked(bool checked);
    void on_ch_pers_blanks_clicked(bool checked);
    void on_ch_pers_errors_clicked(bool checked);
    void on_ch_pers_actNget_clicked(bool checked);
    void on_ch_pers_addr_reg_clicked(bool checked);
    void on_ch_pers_addr_reg_2_clicked(bool checked);
    //void on_ch_pers_assig_clicked(bool checked);
    void on_ch_pers_assig_2_clicked(bool checked);
    void on_ch_pers_assig_ter_clicked(bool checked);
    void on_ch_pers_assig_ter_2_clicked(bool checked);
    void on_ch_pers_assig_errors_clicked(bool checked);
    void on_ch_pers_assig_mo_clicked(bool checked);
    void on_line_pers_assig_snils_mt_editingFinished();
    void on_ch_pers_tfoms_ok_clicked(bool checked);
    void on_ch_pers_tfoms_no_clicked(bool checked);

    void on_date_pol_end_dateChanged(const QDate &date);
    void on_ch_pol_end_clicked(bool checked);



    void on_ch_pers_date_clicked(bool checked);
    void on_ch_pers_month_clicked(bool checked);
    void on_ch_pers_year_clicked(bool checked);
    void on_ch_pers_day_clicked(bool checked);
    void on_date_pers_day_dateChanged(const QDate &date);
    void on_date_pers_day_editingFinished();
    void on_date_pers_date_dateChanged(const QDate &date);
    void on_date_pers_date_editingFinished();
    void on_ch_pers_dates_clicked(bool checked);
    void on_date_pers_date_from_dateChanged(const QDate &date);
    void on_date_pers_date_to_dateChanged(const QDate &date);



    void on_ch_pol_date_clicked(bool checked);
    void on_ch_pol_month_clicked(bool checked);
    void on_ch_pol_year_clicked(bool checked);
    void on_ch_pol_day_clicked(bool checked);
    void on_date_pol_day_dateChanged(const QDate &date);
    void on_date_pol_day_editingFinished();
    void on_date_pol_date_dateChanged(const QDate &date);
    void on_date_pol_date_editingFinished();
    void on_ch_pol_dates_clicked(bool checked);
    void on_date_pol_date_from_dateChanged(const QDate &date);
    void on_date_pol_date_to_dateChanged(const QDate &date);



    void on_ch_insure_date_clicked(bool checked);
    void on_ch_insure_month_clicked(bool checked);
    void on_ch_insure_year_clicked(bool checked);
    void on_ch_insure_day_clicked(bool checked);
    void on_date_insure_day_dateChanged(const QDate &date);
    void on_date_insure_day_editingFinished();
    void on_date_insure_date_dateChanged(const QDate &date);
    void on_date_insure_date_editingFinished();
    void on_ch_insure_dates_clicked(bool checked);
    void on_date_insure_date_from_dateChanged(const QDate &date);
    void on_date_insure_date_to_dateChanged(const QDate &date);



    void on_line_pers_fam_editingFinished();
    void on_line_pers_im_editingFinished();
    void on_line_pers_ot_editingFinished();
    void on_line_pers_snils_editingFinished();
    void on_line_pers_enp_editingFinished();
    void on_line_pers_vs_editingFinished();
    void on_line_pers_vs_2_editingFinished();

    void on_act_sessions_triggered();

    void on_combo_docs_pan_type_currentIndexChanged(int index);

    void on_act_pol_pg_triggered();
    void on_act_pol_monitoring_triggered();

    void on_bn_pers_viz_change_clicked();

    void on_tab_events_activated(const QModelIndex &index);

    void on_bn_pers_drop_clicked();

    void on_bn_pers_pol_change_clicked();

    void on_tab_polises_activated(const QModelIndex &index);

    void on_show_protokols_clicked();

    void on_act_list_terr_triggered();

    void on_act_list_medorgs_triggered();

    void on_spin_pers_assig_mo_editingFinished();

    void on_ch_pers_status_clicked(bool checked);
    void on_combo_pers_status_activated(int index);

    void on_ch_stateless_clicked(bool checked);

    void on_bn_guten_Morgen_clicked();

    void on_bn_pers_pol_drop_clicked();

    void on_date_pers_birth_editingFinished();
    void on_date_pers_birth_2_editingFinished();

    void on_show_add_blanks_clicked();

    void on_act_spoiled_vs_triggered();

    void on_act_assig2_triggered();

    void on_act_name_to_sex_triggered();
    void on_act_namepat_to_sex_triggered();

    void on_spin_docs_pan_type_editingFinished();

    void on_bn_pers_assig_gaps_clicked();
    void on_act_assig_gaps_triggered();

    void on_bn_to_TFOMS_clicked();
    void on_bn_from_TFOMS_clicked();

    void on_bn_to_TFOMS_2_clicked();
    void on_bn_from_TFOMS_2_clicked();

    void on_bn_toCSV_clicked();

    void on_bn_pays2mo_clicked();
    void on_act_pays2mo_pnt_triggered();

    void on_act_match_TFOMS_triggered();
    void on_act_match_assigs_triggered();

    void on_ch_folder_clicked(bool checked);
    void on_ch_folder_closed_clicked(bool checked);
    void on_bn_folder_new_clicked();

    void on_bn_folder_open_close_clicked();

    void on_bn_folder_empty_clicked();

    void on_bn_folder_del_clicked();

    void on_combo_folder_activated(int index);

    void on_act_medorgs_triggered();

    void on_ch_tab_limit_clicked(bool checked);

    void on_spin_tab_limit_editingFinished();

    void on_act_import_names_triggered();

    void on_act_import_namepats_triggered();

    void on_bn_minimize_clicked();

    void on_ch_insure_begin_clicked(bool checked);

    void on_ch_insure_end_clicked(bool checked);

    void on_date_insure_begin_editingFinished();

    void on_date_insure_end_editingFinished();

    void on_bn_layout_clicked();

    void on_act_persXdistrs_to_CSV_triggered();

    void on_group_person_docs_clicked(bool checked);

    void on_group_person_data_clicked(bool checked);

    void on_bn_msg_clicked();

    void on_ch_pers_vs_clicked(bool checked);

    void on_ch_pers_vs_2_clicked(bool checked);

    void on_act_terminals_triggered();

    void on_combo_pers_month_activated(int index);
    void on_combo_pers_year_activated(int index);

    void on_combo_pol_month_activated(int index);
    void on_combo_pol_year_activated(int index);

    void on_combo_insure_month_activated(int index);
    void on_combo_insure_year_activated(int index);

    void on_split_main_splitterMoved(int pos, int index);

    void on_bn_pers_history_clicked();
    void on_bn_polis_history_clicked();

    void on_combo_polis_post_way_currentIndexChanged(int index);

    void on_ch_pers_point_clicked(bool checked);
    void on_ch_pers_operator_clicked(bool checked);

    void on_combo_pers_point_activated(int index);
    void on_combo_pers_operator_activated(int index);

    void on_bn_act_history_clicked();

    void on_ch_pers_category_clicked(bool checked);
    void on_ch_pers_status_f_clicked(bool checked);

    void on_combo_pers_status_f_activated(int index);

    void on_bn_pers_talks_clicked();

    void on_bn_pers_talk_clicked();

    void on_act_packs_triggered();

    void on_act_statistic_triggered();

    void on_bn_show_tfoms_files_clicked();

    void on_group_person_docs_toggled(bool arg1);

    void on_bn_to_TFOMS_all_clicked();

    void on_act_import_fias_triggered();

    void on_line_pers_snils_textEdited(const QString &arg1);
    void on_line_pers_assig_snils_mt_textEdited(const QString &arg1);

    void on_line_pers_vs_textEdited(const QString &arg1);

    void on_line_pers_vs_2_textEdited(const QString &arg1);

    void on_line_pers_enp_textEdited(const QString &arg1);

    void on_tab_events_clicked(const QModelIndex &index);

    void on_bn_pers_status_auto_clicked();

    void on_act_failed_enp_triggered();

    void on_act_expired_vs_triggered();

    void on_group_filters_clicked(bool checked);

    void on_combo_pers_category_currentIndexChanged(int index);

    void on_combo_pers_category_activated(int index);

    void on_combo_pers_status_f_currentIndexChanged(int index);

    void on_ch_pers_dopdata_clicked(bool checked);

    void on_bn_pers_unite_clicked();

    void on_act_make_blanks_pol_act_triggered();

    void on_ch_pers_assig_mt_clicked(bool checked);

    void on_act_subconto_triggered();

    void on_act_vs_reg_by_date_triggered();

    void on_act_vs_expired4date_triggered();

    void on_act_acts_triggered();

    void on_act_enp_scan_triggered();

    void on_act_enp_byhand_triggered();

    //void on_ch_pers_pan_reg_hous_toggled(bool checked);
    //void on_ch_pers_pan_liv_hous_toggled(bool checked);

    void on_combo_pers_pan_reg_hous_activated(const QString &arg1);
    void on_combo_pers_pan_liv_hous_activated(const QString &arg1);

    void on_bn_save_Ifile_clicked();
    void on_bn_save_Pfile_clicked();

    void on_ch_assig_mt_by_medsite_clicked(bool checked);

    void on_combo_pers_assig_ms_currentIndexChanged(int index);

    void on_ch_dost_clicked(bool checked);

    void on_ch_dost_fam_clicked(bool checked);
    void on_ch_dost_im_clicked(bool checked);
    void on_ch_dost_ot_clicked(bool checked);
    void on_ch_dost_day_clicked(bool checked);
    void on_ch_dost_mon_clicked(bool checked);
    void on_ch_dost_year_clicked(bool checked);

    void on_bn_finder_clicked(bool checked);

    void on_act_errors4days_triggered();

    void on_act_pays2mo_smo_sum_triggered();

    void on_bn_pers_pan_old_data_clicked(bool checked);

    void on_act_refresh_fias_triggered();

    void on_combo_pers_pan_category_currentIndexChanged(int index);

    void on_act_disp_triggered();
    void on_act_disp_18_triggered();
    void on_act_disp_dead_triggered();

    void on_ch_folder_toggled(bool checked);
    void on_ch_retro_toggled(bool checked);

    void on_act_prof_triggered();

    void on_act_assig_errors_triggered();

    void on_act_insure_errors_triggered();

    void on_bn_spr_insure_errors_clicked();

    void on_bn_spr_assig_errors_clicked();

    void on_act_print_disp2mo_triggered();

    void on_line_eaes_residence_editingFinished();
    void on_ch_eaes_reside_exp_clicked(bool checked);

    void on_act_load_ms_reeester_triggered();

    void on_act_gen_reester_vs2_triggered();

    void on_act_gen_reester_oms2_triggered();

    void on_act_guten_morgen_triggered();

    void on_act_guten_morgen_all_triggered();

    void on_act_asglay_today_triggered();

    void on_act_asglay_todate_triggered();

    void on_act_resend_events_triggered();

    void on_ch_pers_pan_reg_text_clicked(bool checked);
    void on_ch_pers_pan_liv_text_clicked(bool checked);

    void on_act_disp_cnt_18_triggered();

    void on_act_disp_cnt_triggered();

    void on_act_vs4dates_triggered();

    void on_combo_pers_assig_mo_currentIndexChanged(int index);

    void on_combo_pers_insure_agent1_currentIndexChanged(int index);
    void on_combo_pers_insure_agent2_currentIndexChanged(int index);

    void on_act_disp_plans_triggered();

    void on_act_report_boxes_scan_triggered();

    void on_ch_retro_clicked(bool checked);

    void on_date_retro_editingFinished();

    void on_act_tab1_triggered();

    void on_act_tab2_triggered();

    void on_act_pol2points_triggered();

    void on_bn_pers_archive_clicked();
    void on_bn_pers_unarch_clicked();
    void on_ch_archive_clicked(bool checked);
    void on_ch_trash_clicked(bool checked);

    void on_act_asgcnt_bydate_triggered();
    void on_act_asgcnt_today_triggered();

    void on_act_newborners_triggered();
    void on_act_outsiders_triggered();
    void on_act_other_terr_triggered();

    void on_act_count2months_triggered();

    void on_act_free_pol2point_triggered();
    void on_act_free_pol2point_list_triggered();
    void on_act_free_vs2point_triggered();
    void on_act_free_vs2point_list_triggered();
    void on_act_spoil_vs2point_triggered();
    void on_act_spoil_vs2point_list_triggered();

    void on_act_newENP4oldVS_triggered();

    void on_act_operator_timeline_triggered();

    void on_act_BSO_vs_triggered();

    void on_act_BSO_paper_triggered();

    void on_act_BSO_electro_triggered();

    void on_act_1st_aid_sexage_by_list_triggered();

    void on_act_polises_n_vs_by_point_triggered();

    void on_ch_id_field_clicked(bool checked);
    void on_ch_id_guid_clicked(bool checked);
    void on_ch_id_note_clicked(bool checked);

    void on_bn_save_note_clicked();

    void on_bn_delete_note_clicked();

    void on_line_note_returnPressed();

    void on_bn_font_up_clicked();

    void on_bn_font_dn_clicked();

    void on_bn_save_Sfile_clicked();

    void on_act_BSO_blanks_triggered();

private:
    Ui::persons_wnd *ui;
};

#endif // PERSONS_WND_H
