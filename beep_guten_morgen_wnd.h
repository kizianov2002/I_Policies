#ifndef beep_Guten_Morgen_wnd_H
#define beep_Guten_Morgen_wnd_H

#include <QDialog>

namespace Ui {
class beep_Guten_Morgen_wnd;
}

class beep_Guten_Morgen_wnd : public QDialog
{
    Q_OBJECT

public:
    explicit beep_Guten_Morgen_wnd(QWidget *parent = 0);
    ~beep_Guten_Morgen_wnd();

private:
    Ui::beep_Guten_Morgen_wnd *ui;
};

#endif // beep_Guten_Morgen_wnd_H
