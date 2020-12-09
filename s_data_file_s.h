#ifndef S_DATA_FILE_S
#define S_DATA_FILE_S

#include <QDate>

typedef struct {
    int pol_v;
    QString pol_ser;
    QString pol_num;
    bool has_date_begin, has_date_end, has_date_stop;
    QDate date_begin;
    QDate date_end;
    QDate date_stop;
} s_data_file_S_polis;

typedef struct {
    qint64 id_tfoms;
    int stop_reason;
    QString enp;

    bool has_date_death;
    QDate date_death;

    s_data_file_S_polis polis;
} s_data_file_S_stop;

typedef struct {
    QString vers;
    QString filename;
    QString smo_regnum;
    QString point_regnum;
    int n_recs;

    QList<s_data_file_S_stop> stop_list;
} s_data_file_S;

#endif // S_DATA_FILE_S

