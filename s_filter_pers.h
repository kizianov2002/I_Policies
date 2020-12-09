#ifndef S_FILTER_PERS
#define S_FILTER_PERS

typedef struct {
    bool has_fam, has_im, has_ot, has_sex, has_birth, has_snils;
    QString fam, im, ot, snils;
    int sex;
    QDate date_birth;
} s_filter_pers;

#endif // S_FILTER_PERS

