#ifndef beep_30_min_wnd_H
#define beep_30_min_wnd_H

#include <QDialog>
#include <QTimer>
#include <QPoint>

namespace Ui {
class beep_30_min_wnd;
}

class beep_30_min_wnd : public QDialog
{
    Q_OBJECT

    QTimer *timer;
    int counter;

    QPoint pt_last_act;

public:
    explicit beep_30_min_wnd(QWidget *parent = 0);
    ~beep_30_min_wnd();

private slots:
    void on_timer();
    void on_action();

private:
    Ui::beep_30_min_wnd *ui;
};

#endif // beep_30_min_wnd_H
