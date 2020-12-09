#ifndef info_pers_doubles_wnd_H
#define info_pers_doubles_wnd_H

#include <QDialog>

namespace Ui {
class info_pers_doubles_wnd;
}

class info_pers_doubles_wnd : public QDialog
{
    Q_OBJECT
    QString msg;
    bool can_stop, can_arch;
    int &res;       // 1 - сохраняю
                    // 0 - не сохраняю - проверь данные
                    // -1 - я достал записи из архива - проверь данные

public:
    explicit info_pers_doubles_wnd(QString msg, int &res, bool can_stop, bool can_arch, QWidget *parent = 0);
    ~info_pers_doubles_wnd();

private slots:
    void on_bn_ok_clicked();
    void on_bn_cansel_clicked();
    void on_bn_archive_clicked();
    void on_bn_close_clicked();

private:
    Ui::info_pers_doubles_wnd *ui;
};

#endif // info_pers_doubles_wnd_H
