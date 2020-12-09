#include "folder_wnd.h"
#include "ui_folder_wnd.h"

folder_wnd::folder_wnd(QSqlDatabase &db, s_data_folder &data_folder, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    QDialog(parent),
    db(db),
    data_app(data_app),
    settings(settings),
    data_folder(data_folder),
    ui(new Ui::folder_wnd)
{
    ui->setupUi(this);

    if (data_folder.id>0) {
        ui->ln_folder_name->setText(data_folder.folder_name);
        ui->date_open->setDate(data_folder.date_open);
        ui->date_close->setDate(data_folder.date_close);
        ui->combo_status->setCurrentIndex(data_folder.status +1);

        ui->date_open->setEnabled(false);
        ui->bn_today->setEnabled(false);
    } else {
        on_bn_today_clicked();
    }
}

folder_wnd::~folder_wnd() {
    delete ui;
}

void folder_wnd::on_bn_today_clicked() {
    ui->date_open->setDate(QDate::currentDate());
}

void folder_wnd::on_pushButton_clicked() {
    reject();
}

void folder_wnd::on_bn_save_clicked() {
    db.transaction();

    if (data_folder.id==-1) {
        // добавим папку
        QString sql_ins = "insert into folders(folder_name, date_open, date_close, status) "
                          " values("
                          " '" + ui->ln_folder_name->text().trimmed().simplified().replace("--", "—") + "', "
                          " '" + ui->date_open->date().toString("yyyy-MM-dd") + "', "
                          " NULL, "
                          " " + QString::number(ui->combo_status->currentIndex() -1) + "); ";
        QSqlQuery *query_ins = new QSqlQuery(db);
        bool res_ins = query_ins->exec(sql_ins);
        QString err1 = db.lastError().databaseText();
        if (!res_ins) {
            delete query_ins;
            QMessageBox::warning(this, "Ошибка при попытке добавить новую папку",
                                 "При опытке добавить новую папку произошла ошибка!\n\n"
                                 "Операция отменена");
            db.rollback();
            return;
        }
    } else {
        // изменим папку
        QString sql_upd = "update folders "
                          " set folder_name='" + ui->ln_folder_name->text().trimmed().simplified().replace("--", "—") + "', "
                          "     date_open='" + ui->date_open->date().toString("yyyy-MM-dd") + "', "
                          "     date_close='" + data_folder.date_close.toString("yyyy-MM-dd") + "', "
                          "     status=" + QString::number(ui->combo_status->currentIndex() -1) + " "
                          " where id=" + QString::number(data_folder.id) + " ; ";
        QSqlQuery *query_upd = new QSqlQuery(db);
        bool res_upd = query_upd->exec(sql_upd);
        QString err1 = db.lastError().databaseText();
        if (!res_upd) {
            delete query_upd;
            QMessageBox::warning(this, "Ошибка при попытке обновить папку",
                                 "При опытке обновить папку произошла ошибка!\n\n"
                                 "Операция отменена");
            db.rollback();
            return;
        }
    }
    db.commit();

    accept();
}

void folder_wnd::on_bn_cansel_clicked() {
    close();
}
