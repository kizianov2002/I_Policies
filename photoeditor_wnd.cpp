#include "photoeditor_wnd.h"
#include <QMessageBox>
#include <QGraphicsScene>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include "ui_photoeditor_wnd.h"

PhotoEditor_wnd::PhotoEditor_wnd(QString &img_path, int img_type, QSize img_size, QString &res_path, s_data_app &data_app, QWidget *parent)
    :img_path(img_path), img_type(img_type), img_size(img_size), res_path(res_path), data_app(data_app), rubberBand(NULL), QDialog(parent), ui(new Ui::PhotoEditor_wnd)
{
    ui->setupUi(this);

    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    f_crop = false;
    ui->bn_apply->setEnabled(false);
    ui->bn_crop->setEnabled(true);

    /*QGraphicsScene* scene = new QGraphicsScene();
    //QRubberBand* rubberBand = new QRubberBand(QRubberBand::Rectangle, scene);
    //QRubberBand* rubberBand = new QRubberBand(QRubberBand::Rectangle);
    QRubberBand* rubberBand = new QRubberBand(QRubberBand::Rectangle, this->graphicsView);

    QPalette palette;
    palette.setBrush(QPalette::Foreground, QBrush(Qt::green));
    palette.setBrush(QPalette::Base, QBrush(Qt::red));

    rubberBand->setPalette(palette);
    rubberBand->resize(30, 30);

    //scene->addItem(rubberBand);

    this->graphicsView->setScene(scene);
    //this->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
    this->graphicsView->show();*/


    ui->spin_scale->setValue(100);
    ui->spin_left->setValue(0);
    ui->spin_top->setValue(0);
    ui->spin_width->setValue(img_size.width());
    ui->spin_height->setValue(img_size.height());
    if (img_type==3) {
        ui->spin_black->setValue(150);
        ui->spin_white->setValue(230);
    } else {
        ui->spin_black->setValue(20);
        ui->spin_white->setValue(220);
    }

    QString s = img_path;
    img_pixmap = QPixmap(s);

    draw();
}

void PhotoEditor_wnd::draw() {
    QSize new_size = img_pixmap.size();
    new_size.setWidth( (new_size.width()*ui->spin_scale->value())/100.0 );
    new_size.setHeight( (new_size.height()*ui->spin_scale->value())/100.0 );

    if (f_crop) {
        ui->lab_img->setPixmap(QPixmap::fromImage(img_crop,Qt::AutoColor));
        ui->bn_restore->setEnabled(true);
    } else {
        ui->lab_img->setPixmap(img_pixmap.scaled(new_size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->bn_restore->setEnabled(false);
    }
    this->resize(100,100);
}

PhotoEditor_wnd::~PhotoEditor_wnd() {
    delete ui;
}

void PhotoEditor_wnd::on_spin_scale_valueChanged(int arg1) {
    draw();
}

void PhotoEditor_wnd::on_spin_width_valueChanged(int arg1) {
    draw();
}

void PhotoEditor_wnd::on_spin_height_valueChanged(int arg1) {
    draw();
}

void PhotoEditor_wnd::on_bn_cansel_clicked() {
    res_path = "";
    close();
}

void PhotoEditor_wnd::on_bn_crop_clicked() {
    int w = ui->spin_width->value();
    int h = ui->spin_height->value();
    bool r0 = ui->rb_rot_0->isChecked();
    bool r90 = ui->rb_rot_90->isChecked();
    bool r180 = ui->rb_rot_180->isChecked();
    bool r270 = ui->rb_rot_270->isChecked();
    float b_black = ui->spin_black->value();
    float b_white = ui->spin_white->value();

    // подготовим image
    QImage img_old = ui->lab_img->pixmap()->toImage();
    img_crop = QImage(ui->spin_width->value(), ui->spin_height->value(),QImage::Format_Indexed8);
    img_crop.setColorCount(256);
    for (int i=0; i<256; i++) {
        QRgb c = qRgb(i,i,i);
        img_crop.setColor(i, c);
    }
    // скопируем область из исходного имиджа и сохраним в грейскейл
    if (r0) {
        for (int i=0; i<w; i++) { // строки
            for (int j=0; j<h; j++) { // пикселы
                int x = i+ui->spin_left->value();
                int y = j+ui->spin_top->value();
                int n = 255;
                if (x>=0 && y>=0 && x<ui->lab_img->size().width() && y<ui->lab_img->size().height()) {
                    n = qGray(img_old.pixel(x,y));
                    n = (n-b_black)*255.0/(b_white-b_black);
                    if ( n<0 ) n=0;
                    if (n>255) n=255;
                }
                img_crop.setPixel(i, j, n);
            }
        }
    } else if (r180) {
        for (int i=0; i<w; i++) { // строки
            for (int j=0; j<h; j++) { // пикселы
                int x = i+ui->spin_left->value();
                int y = j+ui->spin_top->value();
                int n = 255;
                if (x>=0 && y>=0 && x<ui->lab_img->size().width() && y<ui->lab_img->size().height()) {
                    n = qGray(img_old.pixel(x,y));
                    n = (n-b_black)*255.0/(b_white-b_black);
                    if ( n<0 ) n=0;
                    if (n>255) n=255;
                }
                img_crop.setPixel(w-i-1, h-j-1, n);
            }
        }
    } else if (r90) {
        for (int i=0; i<h; i++) { // строки
            for (int j=0; j<w; j++) { // пикселы
                int x = i+ui->spin_left->value();
                int y = j+ui->spin_top->value();
                int n = 255;
                if (x>=0 && y>=0 && x<ui->lab_img->size().width() && y<ui->lab_img->size().height()) {
                    n = qGray(img_old.pixel(x,y));
                    n = (n-b_black)*255.0/(b_white-b_black);
                    if ( n<0 ) n=0;
                    if (n>255) n=255;
                }
                img_crop.setPixel(w-j-1, i, n);
            }
        }
    } else if (r270) {
        for (int i=0; i<h; i++) { // строки
            for (int j=0; j<w; j++) { // пикселы
                int x = i+ui->spin_left->value();
                int y = j+ui->spin_top->value();
                int n = 255;
                if (x>=0 && y>=0 && x<ui->lab_img->size().width() && y<ui->lab_img->size().height()) {
                    n = qGray(img_old.pixel(x,y));
                    n = (n-b_black)*255.0/(b_white-b_black);
                    if ( n<0 ) n=0;
                    if (n>255) n=255;
                }
                img_crop.setPixel(j, h-i-1, n);
            }
        }
    }
    QApplication::processEvents();
    i_scale = ui->spin_scale->value();
    ui->spin_scale->setValue(100);
    f_crop = true;
    ui->bn_apply->setEnabled(true);
    ui->bn_crop->setEnabled(false);

    draw();
    this->resize(100,100);
    rubberBand->hide();
}

void PhotoEditor_wnd::on_bn_apply_clicked() {
    // сохраним пиксмеп в темп
    res_path = data_app.path_temp + (img_type==2 ? "photo.jpg" : "sign.jpg");
    if (!f_crop) {
        bool res = img_pixmap.save(res_path, 0, 100);
        if (!res) {
            QMessageBox::critical(this, "Ошибка сохранения JPG", "При сохранении обработанного файла произошла непредвиденная ошибка."
                                                                 "\n1) Убедитесь, что папка " + data_app.path_temp + " существует и доступна."
                                                                 "\n2) Если папка существует, убедитесь, что в ней нет файла photo.jpg или sign.jpg."
                                                                 "\n3) Если файл уже есть - удалите его...\n\nИ попробуйте принять результаты работы снова.");
        }
    } else {
        bool res = img_crop.save(res_path, 0, 100);
        if (!res) {
            QMessageBox::critical(this, "Ошибка сохранения JPG", "При сохранении обработанного файла произошла непредвиденная ошибка."
                                                                 "\n1) Убедитесь, что папка " + data_app.path_temp + " существует и доступна."
                                                                 "\n2) Если папка существует, убедитесь, что в ней нет файла photo.jpg или sign.jpg."
                                                                 "\n3) Если файл уже есть - удалите его...\n\nИ попробуйте принять результаты работы снова.");
        }
    }

    close();
}

void PhotoEditor_wnd::mousePressEvent(QMouseEvent *event)  {
    if (!rubberBand)
        rubberBand = new QRubberBand(QRubberBand::Rectangle, this);

    p_from = event->pos();
    rubberBand->setGeometry(QRect(p_from, QSize()));
    rubberBand->show();
}

void PhotoEditor_wnd::wheelEvent(QWheelEvent *event)  {
    if (!rubberBand)
        rubberBand = new QRubberBand(QRubberBand::Rectangle, this);

    QPoint angle = event->angleDelta();
    float rot = angle.y();
    ui->spin_scale->setValue(ui->spin_scale->value()*(1.0+rot/2000));
    draw();
}

void PhotoEditor_wnd::mouseMoveEvent(QMouseEvent *event)  {
    if (!rubberBand)
        rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    if (f_crop) {
        rubberBand->hide();
    } else {
        rubberBand->show();
        bool r0 = ui->rb_rot_0->isChecked();
        bool r90 = ui->rb_rot_90->isChecked();
        bool r180 = ui->rb_rot_180->isChecked();
        bool r270 = ui->rb_rot_270->isChecked();
        int w = ui->spin_width->value();
        int h = ui->spin_height->value();
        if (r90 || r270) {
            w = ui->spin_height->value();
            h = ui->spin_width->value();
        }
        QPoint a = event->pos(), b = event->pos();
        a.setX (event->pos().x() - w/2);
        a.setY (event->pos().y() - h/2);
        b.setX (event->pos().x() + w/2);
        b.setY (event->pos().y() + h/2);

        if (img_type==2) {
            if (w>ui->lab_img->size().width()) {
                // для маленьких изображения - выравнивание по центру
                int d = (w - ui->lab_img->size().width())/2;
                a.setX(ui->pan_img->pos().x()+2 - d);
                b.setX(ui->pan_img->pos().x()+2 + ui->lab_img->size().width() + d);
            } else if (a.x()<ui->pan_img->pos().x()+2) {
                // если вылазит за рамку - выровняем по рамке
                a.setX(ui->pan_img->pos().x()+2);
                b.setX(w+ui->pan_img->pos().x()+2);
            }
            if (h>ui->lab_img->size().height()) {
                // для маленьких изображения - выравнивание по центру
                int d = (h - ui->lab_img->size().height())/2;
                a.setY(ui->pan_img->pos().y()+2 - d);
                b.setY(ui->pan_img->pos().y()+2 + ui->lab_img->size().height() + d);
            } else if (a.y()<ui->pan_img->pos().y()+2) {
                // если вылазит за рамку - выровняем по рамке
                a.setY(ui->pan_img->pos().y()+2);
                b.setY(h+ui->pan_img->pos().y()+2);
            }
        }
        QRect r = QRect(a, b).normalized();
        rubberBand->setGeometry(r);
        ui->spin_left->setValue(r.x()-ui->pan_img->pos().x()-2);
        ui->spin_top->setValue(r.y()-ui->pan_img->pos().y()-2);
    }
}

void PhotoEditor_wnd::mouseReleaseEvent(QMouseEvent *event)  {
    //rubberBand->hide();
}

void PhotoEditor_wnd::on_bn_restore_clicked() {
    f_crop = false;
    ui->bn_apply->setEnabled(false);
    ui->bn_crop->setEnabled(true);
    ui->spin_scale->setValue(i_scale);
    draw();
    this->resize(100,100);
}

void PhotoEditor_wnd::on_spin_white_valueChanged(int arg1) {
    if (ui->spin_black->value()>(arg1+4)) {
        ui->spin_black->setValue(arg1+5);
    }
}

void PhotoEditor_wnd::on_spin_black_valueChanged(int arg1) {
    if (ui->spin_white->value()<(arg1+4)) {
        ui->spin_white->setValue(arg1+5);
    }
}

void PhotoEditor_wnd::on_pushButton_clicked() {
    this->resize(100,100);
}
