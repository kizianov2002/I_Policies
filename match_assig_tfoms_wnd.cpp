#include "match_assig_tfoms_wnd.h"
#include "ui_match_assig_tfoms_wnd.h"

match_assig_TFOMS_wnd::match_assig_TFOMS_wnd(QSqlDatabase &db_ODBC, s_data_match_assig &data_match_assig, QWidget *parent) :
    db_ODBC(db_ODBC), data_match_assig(data_match_assig), QDialog(parent),
    ui(new Ui::match_assig_TFOMS_wnd)
{
    ui->setupUi(this);

    // обновим список доступных таблиц
    refresh_tablist();
    on_bn_date_assig_clicked();
}

match_assig_TFOMS_wnd::~match_assig_TFOMS_wnd() {
    delete ui;
}

void match_assig_TFOMS_wnd::on_bn_cansel_clicked() {
    reject();
}

void match_assig_TFOMS_wnd::refresh_tablist() {
    ui->comboBox->clear();
    ui->comboBox->addItems(db_ODBC.tables());
}

void match_assig_TFOMS_wnd::on_bn_date_assig_clicked() {
    ui->date_assig->setDate(QDate::currentDate().addDays(-1));
}

void match_assig_TFOMS_wnd::on_comboBox_activated(const QString &arg1) {
    data_match_assig.pack_name = arg1;
}

void match_assig_TFOMS_wnd::on_date_assig_dateChanged(const QDate &date) {
    data_match_assig.date_assig = date;
}

void match_assig_TFOMS_wnd::on_bn_process_clicked() {
    data_match_assig.pack_name = ui->comboBox->currentText();
    data_match_assig.date_assig = ui->date_assig->date();
    if (data_match_assig.pack_name.isEmpty()) {
        QMessageBox::warning(this, "Не хватает данных",
                             "Выберите таблицу ODBC  из которой будут импортированы данные.");
        return;
    }
    if ( data_match_assig.date_assig<QDate(1920,1,1) ||
         data_match_assig.date_assig>QDate::currentDate() ) {
        QMessageBox::warning(this, "Неверные данные",
                             "Дата прикрепления не может быть раньше 01.01.1920 или позже текущего дня.");
        return;
    }
    accept();
}
