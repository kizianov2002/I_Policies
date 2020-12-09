#ifndef D_DATE
#define D_DATE

#include <QDate>

class d_date {
public:
    static int dY(QDate d_old, QDate d_new) {
        int dy = d_new.year() - d_old.year() + ( (d_new.month()>d_old.month() || (d_new.month()==d_old.month() && d_new.day()>=d_old.day())) ? 0 : -1 );
        return dy;
    }

    static int dM(QDate d_old, QDate d_new) {
        int dm = d_new.month() - d_old.month();
        if (dm<0)
            dm += 12;
        return dm;
    }

    static int dD(QDate d_old, QDate d_new) {
        int dd = d_new.day() - d_old.day();
        if (dd<0)
            dd += 30;
        return dd;
    }
};

#endif // D_DATE
