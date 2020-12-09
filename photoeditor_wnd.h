#ifndef PHOTOEDITOR_WND_H
#define PHOTOEDITOR_WND_H

#include <QDialog>
#include <QPoint>
#include <QRubberBand>
#include "s_data_app.h"

namespace Ui {
class PhotoEditor_wnd;
}

class PhotoEditor_wnd : public QDialog
{
    Q_OBJECT

    QPoint p_from, p_to;
    QRubberBand *rubberBand;

public:
    QString &img_path;
    int img_type;
    QSize img_size;
    QPixmap img_pixmap;
    QImage img_crop;
    //QImage img_brth;
    bool f_crop;
    int i_scale;

    s_data_app &data_app;

    QString &res_path;

    void draw();

public:
    explicit PhotoEditor_wnd(QString &img_path, int img_type, QSize img_size, QString &res_path, s_data_app &data_app, QWidget *parent=0);
    ~PhotoEditor_wnd();

private slots:

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void on_spin_scale_valueChanged(int arg1);

    void on_spin_width_valueChanged(int arg1);
    void on_spin_height_valueChanged(int arg1);

    void on_spin_white_valueChanged(int arg1);
    void on_spin_black_valueChanged(int arg1);

    void on_bn_crop_clicked();
    void on_bn_restore_clicked();
    void on_bn_cansel_clicked();
    void on_bn_apply_clicked();

    void on_pushButton_clicked();

private:
    Ui::PhotoEditor_wnd *ui;
};

#endif // PHOTOEDITOR_WND_H
