#ifndef ACT_BLANKNUMS_H
#define ACT_BLANKNUMS_H

#include <QDialog>

namespace Ui {
class act_blanknums_wnd;
}

class act_blanknums_wnd : public QDialog
{
    Q_OBJECT

    QStringList &blanknums;

public:
    explicit act_blanknums_wnd(QStringList &blanknums, QWidget *parent = 0);
    ~act_blanknums_wnd();

private slots:
    void on_bn_cansel_clicked();

    void on_bn_ok_clicked();

    void on_rb_list_clicked(bool checked);

    void on_rb_nums_clicked(bool checked);

    void on_spin_min_editingFinished();

    void on_spin_max_editingFinished();

private:
    Ui::act_blanknums_wnd *ui;
};

#endif // ACT_BLANKNUMS_H
