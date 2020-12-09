#ifndef s_data_assig_H
#define s_data_assig_H

#include <QDate>

/*typedef struct {
    QString code_mo;

    bool has_assig_way, has_unsig_date;

    int assig_type, assig_way;
    QDate assig_date;
    QDate unsig_date;

} s_data_assig;*/

typedef struct {
    //int id;

    int subj;
    int dstr;
    int city;
    int nasp;
    int strt;
    int house;
    QString corp;

    QString code_mo;
    QString code_mp;
    QString code_ms;
    QString code_mt;
} s_data_assig;


#endif // S_data_medorg_H

