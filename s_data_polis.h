#ifndef S_DATA_POLIS
#define S_DATA_POLIS

#include <QDate>

typedef struct {
    bool is_ok;     // признак того, что данные полиса не содержат ошибок
                    // - используется при приверке успешности сохранения записи для последующего отката операции
    int     id;
    int     id_person;
    QString ocato;
    QString ogrn_smo;
    int     r_smo;
    int     r_polis;
    int     f_polis;      // форма бланка
    int     pol_v;
    QString pol_v_text;
    QString pol_type;
    bool    has_enp, has_vs_num, has_pol_sernum, has_uec_num;
    QString vs_num;
    QString pol_ser;
    QString pol_num;
    QString uec_num;
    QString enp;
    bool    has_tfoms_date, has_date_sms_pol, has_date_begin, has_date_end, has_date_stop;
    QDate   tfoms_date, date_sms_pol, date_begin, date_end, date_stop;
    int     in_erp;    // 0 - нет, 1 - да, -1 - отказ, -2 - закрыь вручную, -999 - NULL
    QString order_num;
    QDate   order_date;

    bool    has_date_activate, has_date_get2hand;
    QDate   date_activate, date_get2hand;
    //bool  is_actual;
    int     id_old_polis;

    int _id_first_point;
    QString first_point_name;
    int _id_first_operator;
    QString first_operator_fio;
    int _id_first_event;

    int _id_last_point;
    QString last_point_name;
    int _id_last_operator;
    QString last_operator_fio;
    int _id_last_event;

    QDate   dt_ins, dt_upd;
} s_data_polis;

#endif // S_DATA_POLIS

