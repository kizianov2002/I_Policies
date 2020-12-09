#include "match_tfoms_wnd.h"
#include "ui_match_tfoms_wnd.h"

#include <QFileDialog>

match_TFOMS_wnd::match_TFOMS_wnd(QSqlDatabase &db_ODBC, s_data_match &data_match, s_data_app &data_app, QWidget *parent) :
    db_ODBC(db_ODBC), data_match(data_match), data_app(data_app), QDialog(parent),
    ui(new Ui::match_TFOMS_wnd)
{
    ui->setupUi(this);

    // обновим список доступных таблиц
    ui->date_match->setDate(data_match.date_match);
}

match_TFOMS_wnd::~match_TFOMS_wnd() {
    delete ui;
}

void match_TFOMS_wnd::on_bn_cansel_clicked() {
    reject();
}

void match_TFOMS_wnd::on_bn_date_match_clicked() {
    ui->date_match->setDate(QDate::currentDate().addDays(-1));
}

void match_TFOMS_wnd::on_comboBox_activated(const QString &arg1) {
    data_match.pack_name = arg1;
}

void match_TFOMS_wnd::on_date_match_dateChanged(const QDate &date) {
    data_match.date_match = date;
}

void match_TFOMS_wnd::on_bn_process_clicked() {
    // скопируем файл с данными ТФОМС в папку _DBF_
    QFileInfo fi(ui->line_file_dbf->text());
    if (!fi.exists()) {
        QMessageBox::warning(this,"Файл не найжен",
                             "Не найден файл с данными из ТФОМС.\n\n"
                             "Выберите файл DBF от ТФОМС и повторите попытку.");
        return;
    }
    data_match.pack_name = fi.baseName();
    data_match.date_match = ui->date_match->date();

    QFileInfo fi_dbf(data_app.path_dbf + ".");
    QDir filedir = fi.dir();
    QDir filedir_dbf = fi_dbf.dir();
    if (filedir!=filedir_dbf) {
        QFile::remove(data_app.path_dbf + fi.fileName());
        QFile f(ui->line_file_dbf->text());
        QString base_name = fi.baseName();
        QString s3 = base_name.right(2);
        QString s2 = base_name.right(5).left(2);
        QString s1 = base_name.right(10).left(4);
        QString file_83 = base_name.left(2) + s1 + s2;
        data_match.pack_name = file_83;
        QFile::remove(data_app.path_dbf + file_83 + ".DBF");
        bool res_f = f.copy(data_app.path_dbf + file_83 + ".DBF");
        if (!res_f) {
            QMessageBox::warning(this,"Ошибка при копировании файла",
                                 "Не удалось скопироать файл с данными из ТФОМС в папку _DBF_.\n\n"
                                 "Проверьте, не открыт ли целевой файл во внешней программе.\n"
                                 "Выберите файл DBF от ТФОМС и повторите попытку.");
            return;
        }
    }


    if (data_match.pack_name.isEmpty()) {
        QMessageBox::warning(this, "Не хватает данных",
                             "Выберите таблицу ODBC  из которой будут импортированы данные.");
        return;
    }
    if ( data_match.date_match<QDate(1920,1,1) ||
         data_match.date_match>QDate::currentDate() ) {
        QMessageBox::warning(this, "Неверные данные",
                             "Дата актуальности данных не может быть раньше 01.01.1920 или позже текущего дня.");
        return;
    }
    accept();
}

void match_TFOMS_wnd::on_bn_file_dbf_clicked() {
    ui->line_file_dbf->setText(QFileDialog::getOpenFileName(this, "Выберите DBF-файл с данными застрахованных от ТФОМС", ui->line_file_dbf->text()));
}
