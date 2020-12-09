#ifndef S_data_file_P
#define S_data_file_P

#include <QDate>

typedef struct {
    int pol_v;
    QString pol_ser;
    QString pol_num;
    bool has_date_begin, has_date_end, has_date_stop;
    QDate date_begin;
    QDate date_end;
    QDate date_stop;
} s_data_file_P_polis;

typedef struct {
    QString ocato;
    QString enp;
    QString ogrn_smo;
    int in_erp;
    s_data_file_P_polis polis;
} s_data_file_P_insurance;

typedef struct {
    int n_rec;
    qint64 id_tfoms;
    int code_erp;
    QString comment;

    s_data_file_P_insurance insurance;
} s_data_file_P_rep;

typedef struct {
    QString vers;
    QString packname;
    QString filename;
    QString smo_regnum;
    QString point_regnum;
    int n_recs;
    int n_errs;

    QList<s_data_file_P_rep> rep_list;
} s_data_file_P;

#endif // S_data_file_P

