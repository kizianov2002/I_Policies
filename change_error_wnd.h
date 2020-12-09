#ifndef CHANGE_ERROR_WND_H
#define CHANGE_ERROR_WND_H

#include <QDialog>

typedef struct {
    int id;
    int code;
    QString event;
    QString text;
    QString comment;
} s_data_error;

namespace Ui {
class change_error_wnd;
}

class change_error_wnd : public QDialog
{
    Q_OBJECT

    s_data_error &data_error;

public:
    explicit change_error_wnd(s_data_error &data_error, QWidget *parent = 0);
    ~change_error_wnd();

private slots:
    void on_bn_cansel_clicked();

    void on_bn_save_clicked();

private:
    Ui::change_error_wnd *ui;
};

#endif // CHANGE_ERROR_WND_H
