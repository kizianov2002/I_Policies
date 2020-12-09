#ifndef show_text_wnd_H
#define show_text_wnd_H

#include <QDialog>

namespace Ui {
class show_text_wnd;
}

class show_text_wnd : public QDialog
{
    Q_OBJECT
    QString header;
    QString &text;
    bool f_readonly;

public:
    explicit show_text_wnd(QString header, QString text, bool f_readonly, QWidget *parent = 0);
    ~show_text_wnd();

private slots:
    void on_bn_close_clicked();

private:
    Ui::show_text_wnd *ui;
};

#endif // show_text_wnd_H
