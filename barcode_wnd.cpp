#include "barcode_wnd.h"
#include "ui_barcode_wnd.h"

barcode_wnd::barcode_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent)
    : db(db), data_app(data_app), settings(settings), QDialog(parent), ui(new Ui::barcode_wnd)
{
    ui->setupUi(this);
}

barcode_wnd::~barcode_wnd() {
    delete ui;
}
