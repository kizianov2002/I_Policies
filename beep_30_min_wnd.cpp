#include "beep_30_min_wnd.h"
#include "ui_beep_30_min_wnd.h"

beep_30_min_wnd::beep_30_min_wnd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::beep_30_min_wnd)
{
    ui->setupUi(this);

    // начало счётчика - 5 минут
    counter = 300;
    timer = NULL;
    pt_last_act = QCursor::pos();

    // запустим таймер
    this->timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_timer()));
    this->timer->setInterval(1000);
    this->timer->start(1000);
}

beep_30_min_wnd::~beep_30_min_wnd() {
    delete ui;
}

void beep_30_min_wnd::on_timer() {
    counter--;

    QPoint pt_new_act = QCursor::pos();
    if (pt_new_act!=pt_last_act)
        on_action();

    if (counter<0) {
        // прошло 5 минут
        QApplication::closeAllWindows();
        // бай-бай :)
    } else {
        // просто обновим счётчик на форме
        int min = counter/60;
        int sec = counter-(min*60);
        ui->lab_time->setText(QString::number(min) + ":" + QString::number(sec));
        QApplication::processEvents();
    }
}

void beep_30_min_wnd::on_action() {
    // отмена, закрытие окна
    this->timer->stop();
    close();
}
