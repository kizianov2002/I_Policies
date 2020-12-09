#ifndef S_DATA_PERSON
#define S_DATA_PERSON

#include <QDate>

typedef struct {
    int id;
    QDate date_ins;
    int id_first_operator;
    int _id_last_operator;
    // self
    bool has_ot, has_death, has_snils;
    QString fam, im, ot, snils;
    int sex, category;
    QString plc_birth, phone_cell, phone_code, phone_home, phone_work, email, guid, note;
    int polis_post_way;
    QDate date_birth, date_death;
    QString dost;   // значение кодируется строкой вида "______" (полные данные) или "123456" (миинимальня полнота данных)
    // self_old
    int id_old;
    bool has_fam_old, has_im_old, has_ot_old, has_death_old, has_snils_old, has_enp_old;
    QString fam_old, im_old, ot_old, snils_old, plc_birth_old,enp_old;
    bool has_sex_old, has_date_birth_old, has_plc_birth_old, has_date_death_old;
    int sex_old;
    QDate date_birth_old, date_death_old;
    QDateTime dt_ins_old, dt_upd_old;
    // udl
    int udl_type;
    QString udl_type_text, udl_ser, udl_num, udl_org_code, udl_org;
    QDate udl_date, udl_date_exp;
    bool udl_date_exp_f;
    // drp
    bool has_drp;
    int drp_type;
    QString drp_type_text, drp_ser, drp_num, drp_org_code, drp_org;
    QDate drp_date, drp_date_exp;
    bool drp_date_exp_f;
    // oksm
    int oksm_birth, oksm_c;
    QString oksm_birth_text, oksm_c_text;
    // rep
    bool has_rep;
    int rep_id, rep_relation;
    QString rep_fio, rep_fam, rep_im, rep_ot;
    int rep_udl_type;
    QString rep_udl_type_text, rep_udl_ser, rep_udl_num, rep_udl_orgcode, rep_udl_org;
    QDate rep_udl_date;
    QString rep_phone_code, rep_phone_home, rep_phone_work, rep_phone_cell;
    // addr_reg
    bool has_reg_addr;
    int id_reg_addr, reg_hous;
    QString reg_index, reg_subj_text, reg_dstr_text, reg_city_text, reg_nasp_text, reg_strt_text, reg_corp, reg_qart;
    QString reg_AOID, reg_HOUSEID;
    QDate reg_date;
    bool bomj;
    QString reg_text;
    // addr_liv
    bool has_liv_addr;
    int id_liv_addr, liv_hous;
    QString liv_index, liv_subj_text, liv_dstr_text, liv_city_text, liv_nasp_text, liv_strt_text, liv_corp, liv_qart;
    QString liv_AOID, liv_HOUSEID;
    QString liv_text;

    // ЕАЭС
    int     eaes_category;
    QString eaes_category_text;
    QString eaes_residence;
    QDate   eaes_residence_exp;

    bool    has_eaes_contract;
    QString eaes_contract;
    QDate   eaes_contract_date;
    QDate   eaes_contract_exp;
    bool    has_eaes_document;
    QString eaes_document;
    QDate   eaes_document_date;
    QDate   eaes_document_exp;

    // assignment
    QString assig_code_mo;
    QString assig_code_mp;
    QString assig_code_ms;
    QString assig_snils_mt;
    int     assig_type;
    QDate   assig_date;
    int     assig_status;

    int status;
    bool has_date_insure_begin;
    QDate _date_insure_begin;
    bool has_date_insure_end;
    QDate _date_insure_end;

    // links
    int id_udl;
    int id_drp;
    int id_polis;
    int id_assig;

    // страховые представители
    int id_insure_agent1;
    QString fio_insure_agent1;
    int id_insure_agent2;
    QString fio_insure_agent2;

} s_data_pers;

#endif // S_DATA_PERSON

