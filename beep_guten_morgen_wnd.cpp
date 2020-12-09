#include "beep_guten_morgen_wnd.h"
#include "ui_beep_Guten_Morgen_wnd.h"

beep_Guten_Morgen_wnd::beep_Guten_Morgen_wnd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::beep_Guten_Morgen_wnd)
{
    ui->setupUi(this);

    this->setWindowFlags((Qt::FramelessWindowHint));
}

beep_Guten_Morgen_wnd::~beep_Guten_Morgen_wnd()
{
    delete ui;
}
