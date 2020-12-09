#ifndef beep_sql_H
#define beep_sql_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class beep_sql_wnd;
}

class beep_sql_wnd : public QDialog
{
    Q_OBJECT

public:
    explicit beep_sql_wnd(QWidget *parent = 0);
    ~beep_sql_wnd();

private slots:
    void beep();

private:
    Ui::beep_sql_wnd *ui;
};

#endif // beep_sql_H
