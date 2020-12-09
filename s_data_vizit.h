#ifndef S_DATA_VIZIT
#define S_DATA_VIZIT

typedef struct {
    int id;
    QDate date;
    int v_method, r_smo, r_polis, f_polis;
    bool has_petition;

    bool is_preinsurez;
    bool is_activation;
    bool is_stoppolis;
    bool is_correction;

    int id_point;
    QString point_name;
    int id_operator;
    QString operator_fio;

    QDate dt_ins, dt_upd;
} s_data_vizit;

#endif // S_DATA_VIZIT

