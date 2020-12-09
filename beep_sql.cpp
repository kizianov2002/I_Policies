#include "beep_sql.h"
#include "ui_beep_sql.h"
#include <QDesktopWidget>

beep_sql_wnd::beep_sql_wnd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::beep_sql_wnd)
{
    ui->setupUi(this);

    this->setWindowFlags((Qt::FramelessWindowHint));

    QRect screen = QApplication::desktop()->screenGeometry();
    //this->resize(1250, 1000);
    this->move(screen.width()/2-50, screen.height()/2-150);

    QTimer::singleShot(5000, this, SLOT(beep()));
}

beep_sql_wnd::~beep_sql_wnd() {
    delete ui;
}

void beep_sql_wnd::beep() {
    this->show();
    QApplication::processEvents();
}
