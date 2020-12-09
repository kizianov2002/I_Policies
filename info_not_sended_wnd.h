#ifndef info_not_sended_wnd_H
#define info_not_sended_wnd_H

#include <QDialog>

namespace Ui {
class info_not_sended_wnd;
}

class info_not_sended_wnd : public QDialog
{
    Q_OBJECT
    QString msg;

public:
    explicit info_not_sended_wnd(QString msg, QWidget *parent = 0);
    ~info_not_sended_wnd();

private slots:
    void on_bn_close_clicked();

private:
    Ui::info_not_sended_wnd *ui;
};

#endif // info_not_sended_wnd_H
