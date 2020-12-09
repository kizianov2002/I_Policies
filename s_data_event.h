#ifndef S_DATA_event
#define S_DATA_event

#include <QDateTime>

typedef struct {
    bool is_ok;     // признак того, что данные полиса не содержат ошибок
                    // - используется при приверке успешности сохранения записи для последующего отката операции
    int id;
    int id_polis;
    QString event_code;
    int event_status;
    QDateTime event_dt;
    QTime event_time;
    QString comment;
    //bool send_sms;

    int id_point;
    QString point_name;
    int id_operator;
    QString operator_fio;

    int id_UDL, id_DRP;

    QDate dt_ins, dt_upd;
} s_data_event;

#endif // S_DATA_event

