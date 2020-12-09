#ifndef S_DATA_TALS
#define S_DATA_TALS

typedef struct {
    int id;
    QString fio;
    int pol_v;
    int _id_polis;
    int _id_person;
    int _id_blank_pol;

    // данные конкретного разговора
    int type_talk;
    QDate date_talk;
    int result;
    QString comment;

    // данные второй персоны
    int _id_person_2;
} s_data_talk;

#endif // S_DATA_TALS

