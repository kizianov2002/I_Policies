#include "export_assig_wnd.h"
#include "ui_export_assig_wnd.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QTextStream>
#include <QTableWidgetItem>

export_assig_wnd::export_assig_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    QDialog(parent), db(db), data_app(data_app), settings(settings), ui(new Ui::export_assig_wnd)
{
    ui->setupUi(this);
    ui->bn_save_to_csv->setEnabled(false);

    ui->ln_fname->setText(data_app.path_out + "_ASSIG_DATA_/");

    if(this->data_app.ocato=="38000") {
        ui->ch_assig_medter->setChecked(false);
    } else {
        ui->ch_assig_medter->setChecked(true);
    }

    on_date_assig_editingFinished();
    on_bn_first_day_clicked();
    //on_date_assig_editingFinished();
}

export_assig_wnd::~export_assig_wnd() {
    delete ui;
}

void export_assig_wnd::on_bn_close_clicked() {
    close();
}

void export_assig_wnd::on_bn_first_day_clicked() {
    ui->date_assig->setDate(QDate(QDate::currentDate().year(), QDate::currentDate().month(), 1).addDays(-1));
    on_date_assig_editingFinished();
}

void export_assig_wnd::on_bn_today_clicked() {
    ui->date_assig->setDate(QDate::currentDate().addDays(-1));
    on_date_assig_editingFinished();
}

void export_assig_wnd::on_date_assig_editingFinished() {
    QString fname_s = ui->ln_fname->text();

    // имя файла
    QDir d(fname_s);
    if (fname_s.toLower().contains(".csv")) {
        d.cdUp();
        fname_s = d.path();
    }
    fname_s += "/MO1" + data_app.smo_regnum + ui->date_assig->date().toString("yyyyMMdd") + ".csv";
    fname_s = fname_s.replace("\\","/").replace("//","/").replace("\\","/").replace("//","/");

    if (QFile::exists(fname_s)) {
        if (QMessageBox::question(this,
                                  "Удалить старый файл?",
                                  "Файл уже существует.\n"
                                  "" + fname_s + "\n\n"
                                  "Удалить старый файл для того чтобы сохранить новый?",
                                  QMessageBox::Yes|QMessageBox::Cancel)==QMessageBox::Yes) {
            if (!QFile::remove(fname_s)) {
                QMessageBox::warning(this,
                                     "Ошибка при удалении старого файла",
                                     "При удалении старого файла произошла непредвиденная ошибка.\n\n"
                                     "Операция отменена.");
                ui->bn_save_to_csv->setEnabled(false);
                return;
            }
        }
    }
    ui->ln_fname->setText(fname_s);
    ui->bn_save_to_csv->setEnabled(true);
}

void export_assig_wnd::on_bn_fname_clicked() {

    // Курск
    if (data_app.assig_vers==0) {
        QString fname_s = QFileDialog::getSaveFileName(this,
                                                       "Куда сохранить файл прикреплений?",
                                                       ui->ln_fname->text(),
                                                       "файлы данных (*.csv);;простой текст (*.txt)");
        if (fname_s.isEmpty()) {
            QMessageBox::warning(this,
                                 "Не выбрано имя файла",
                                 "Имя целевого файла не задано.\n\n"
                                 "Операция отменена.");
            ui->bn_save_to_csv->setEnabled(false);
            return;
        }
        if (QFile::exists(fname_s)) {
            if (QMessageBox::question(this,
                                      "Удалить старый файл?",
                                      "Файл уже существует.\n"
                                      "Удалить старый файл для того чтобы сохранить новый?",
                                      QMessageBox::Yes|QMessageBox::Cancel)==QMessageBox::Yes) {
                if (!QFile::remove(fname_s)) {
                    QMessageBox::warning(this,
                                         "Ошибка при удалении старого файла",
                                         "При удалении старого файла произошла непредвиденная ошибка.\n\n"
                                         "Операция отменена.");
                    ui->bn_save_to_csv->setEnabled(false);
                    return;
                }
            }
        }
        ui->ln_fname->setText(fname_s);
    } else

    // Белгород
    if (data_app.assig_vers==1) {
        QString fname_s = QFileDialog::getExistingDirectory(this,
                                                            "Куда сохранить файл прикреплений?",
                                                            ui->ln_fname->text());
        if (fname_s.isEmpty()) {
            QMessageBox::warning(this,
                                 "Не выбрана целевая папка",
                                 "Имя целевой папки не задано.\n\n"
                                 "Операция отменена.");
            ui->bn_save_to_csv->setEnabled(false);
            return;
        }

        // имя файла
        fname_s += "/MO1" + data_app.smo_regnum + ui->date_assig->date().toString("yyyyMMdd") + ".csv";

        if (QFile::exists(fname_s)) {
            if (QMessageBox::question(this,
                                      "Удалить старый файл?",
                                      "Файл уже существует.\n"
                                      "" + fname_s + "\n\n"
                                      "Удалить старый файл для того чтобы сохранить новый?",
                                      QMessageBox::Yes|QMessageBox::Cancel)==QMessageBox::Yes) {
                if (!QFile::remove(fname_s)) {
                    QMessageBox::warning(this,
                                         "Ошибка при удалении старого файла",
                                         "При удалении старого файла произошла непредвиденная ошибка.\n\n"
                                         "Операция отменена.");
                    ui->bn_save_to_csv->setEnabled(false);
                    return;
                }
            }
        }
        ui->ln_fname->setText(fname_s);
    }
    ui->bn_save_to_csv->setEnabled(true);
}

#include "qt_windows.h"
#include "qwindowdefs_win.h"
#include <shellapi.h>

void export_assig_wnd::on_bn_save_to_csv_clicked() {
    // для Белгорода
    //if (data_app.assig_vers==1) {
    QString fname_s = ui->ln_fname->text();

    // проверка введённых полей
    if (fname_s.isEmpty()) {
        QMessageBox::warning(this,
                             "Не выбрано имя файла",
                             "Имя целевого файла не задано.\n\n"
                             "Операция отменена.");
        return;
    }
    QFile file;
    file.setFileName(fname_s);
    if (file.exists()) {
        if (QMessageBox::question(this,
                                  "Удалить старый файл?",
                                  "Файл уже существует.\n"
                                  "Удалить старый файл для того чтобы сохранить новый?",
                                  QMessageBox::Yes|QMessageBox::Cancel)==QMessageBox::Yes) {
            if (!QFile::remove(fname_s)) {
                QMessageBox::warning(this,
                                     "Ошибка при удалении старого файла",
                                     "При удалении старого файла произошла непредвиденная ошибка.\n\n"
                                     "Операция отменена.");
                return;
            }
        }
    }

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this,
                             "Ошибка создания файла",
                             "При создании целевого файла произошла ошибка.\n\n"
                             "Операция отменена.");
        return;
    }

    // подготовим поток для сохранения файла
    QTextStream stream(&file);
    QString codec_name = ui->combo_encoding->currentText();
    QTextCodec *codec = QTextCodec::codecForName(codec_name.toLocal8Bit());
    stream.setCodec(codec);

    int n=0;
    QString head = "\"Действие\";"
                   "\"Тип_ДПФС\";\"ИД_полиса\";\"ЕНП\";"
                   "\"Фамилия\";\"Имя\";\"Отчество\";"
                   "\"Дата_рождения\";\"Место_рождения\";"
                   "\"Тип_УДЛ\";\"УДЛ\";\"Дата_УДЛ\";\"Орган_УДЛ\";"
                   "\"СНИЛС\";"
                   "\"ИД_МО\";\"Способ_прикрепления\";\"Тип_прикрепления\";\"Дата_прикрепления\";\"Дата_открепления\";"
                   "\"ОИД_ЛПУ\";\"Код_подразделения\";\"Код_участка\";\"СНИЛС_врача\";\"специализация_врача\"";
    if (data_app.assig_csv_headers) {
        //stream << head;
        //n++;
    }

    // переберём полученные данные и сохраним в файл
    {   // выберем данные для сохранения
        QString date = ui->date_assig->date().toString("yyyy-MM-dd");
        QString sql;
        sql += "select * from update_pers_links('#DATE_REP#') ; ";
        sql += "select distinct "
               "     p.pol_type, "
               "     case "
               "       when (p.pol_type='В') "
               "         then cast(p.vs_num as varchar(100)) "
               "       when (p.pol_ser is NULL or p.pol_ser='') "
               "         then cast(p.pol_num as varchar(100)) "
               "       else cast((p.pol_ser||' № '||p.pol_num) as varchar(100)) "
               "     end as pol_id, "
               "     p.enp, "
               "     e.fam, "
               "     e.im, "
               "     e.ot, "
               "     e.date_birth, "
               "     e.plc_birth, "
               "     d.doc_type, "
               "     case "
               "       when (d.doc_ser is NULL or d.doc_ser='') "
               "         then (d.doc_num) "
               "       else "
               "         (d.doc_ser||' № '||d.doc_num) "
               "     end as doc_sernum, "
               "     d.doc_date, "
               "     d.doc_org, "
               "     e.snils, "
               "     a.code_mo, "
               "     a.assig_type, "
               "     a.assig_way, "
               "     a.assig_date, "
               "     a.unsig_date, "
               "     a.code_mp, "
               "     a.code_ms, "
               "     a.snils_mt, "
               "     m.oid_mo, "
               "     t.prof_mt "
               "  from persons e "
               "  left join link_persons_asg la on(la.id_person=e.id) ";

        // наличие полиса
        if (ui->ch_polis->isChecked()) {
            sql += " join ";
        } else {
            sql += " left join ";
        };
        sql += "    polises p on ( p.id=e.id_polis ) ";

        // наличие УДЛ
        if (ui->ch_udl->isChecked()) {
            sql += " join ";
        } else {
            sql += " left join ";
        };
        sql += "    persons_docs d on ( d.id=e.id_udl ) ";

        // наличие прикрепления
        if (ui->ch_assig->isChecked()) {
            sql += " join ";
        } else {
            sql += " left join ";
        };
        sql += "    persons_assig a on ( a.id=la._id_asg and a.is_actual=0 ) ";

        // данные МО
        //"    left join spr_medorgs m on ( m.code_mo=a.code_mo and m.is_head=1 ) "

        sql += " join ";
        sql += "    ( select max(id) as id, code_mo "
               "      from spr_medorgs m "
               "      where m.is_head=1 "
               "      group by code_mo "
               "    ) qm on ( qm.code_mo=a.code_mo ) "
               "    left join spr_medorgs m on ( m.id=qm.id ) ";

        // данные MT
        sql += " left join ";
        sql += "    ( select max(id) as id, code_mo, snils_mt "
               "      from spr_medters t "
               "      group by code_mo, snils_mt "
               "    ) qt on ( qt.code_mo=a.code_mo and qt.snils_mt=a.snils_mt ) "
               "    left join spr_medters t on ( t.id=qt.id ) "

               " where (e.date_death is NULL or e.date_death>'#DATE_REP#') "
               "   and (select * from st_person(e.id, '#DATE_REP#'))>-100 "
               " order by e.fam, e.im, e.ot, e.date_birth ; ";


        sql = sql.replace("#DATE_REP#", date);


        QSqlQuery *query = new QSqlQuery(db);
        bool res = mySQL.exec(this, sql, QString("Выбор данных прикрепления на заданное число"), *query, true, db, data_app);
        if (!res) {
            QMessageBox::warning(this, "Данные не получены", "При выборе данных прикрепления произошла неожиданная ошибка.");
            delete query;
            return;
        }
        ui->lab_all->setText(QString::number(query->size()));
        ui->lab_cnt->setText("0");

        while (query->next()) {
            if (n>0)
                stream << "\n";

            stream << QString("\"Р\";");
            stream << "\"" + query->value(0).toString().trimmed().simplified() + "\";";
            stream << "\"" + query->value(1).toString().trimmed().simplified() + "\";";
            stream << "\"" + query->value(2).toString().trimmed().simplified() + "\";";
            stream << "\"" + query->value(3).toString().trimmed().simplified() + "\";";
            stream << "\"" + query->value(4).toString().trimmed().simplified() + "\";";
            stream << "\"" + query->value(5).toString().trimmed().simplified() + "\";";
            stream << "\"" + query->value(6).toDate().toString("yyyyMMdd") + "\";";
            stream << "\"" + query->value(7).toString().trimmed().simplified() + "\";";
            stream << "\"" + QString(query->value(8).isNull() ? "" : query->value(8).toString().trimmed().simplified()) + "\";";
            stream << "\"" + QString(query->value(9).isNull() ? "" : query->value(9).toString().trimmed().simplified()) + "\";";
            stream << "\"" + query->value(10).toDate().toString("yyyyMMdd") + "\";";
            stream << "\"" + query->value(11).toString().trimmed().simplified() + "\";";
            stream << "\"" + query->value(12).toString().trimmed().simplified() + "\";";
            stream << "\"" + query->value(13).toString().trimmed().simplified() + "\";";
            stream << "\"" + query->value(14).toString().trimmed().simplified() + "\";";
            stream << ";";
            stream << "\"" + query->value(16).toDate().toString("yyyyMMdd") + "\";";
            stream << "\"" + query->value(17).toDate().toString("yyyyMMdd") + "\";";
            stream << "\"" + query->value(21).toString().trimmed().simplified() + "\";";
            stream << "\"" + query->value(18).toString().trimmed().simplified() + "\";";
            stream << "\"" + query->value(19).toString().trimmed().simplified() + "\";";
            stream << "\"" + query->value(20).toString().trimmed().simplified() + "\";";
            stream << "\"" + QString::number(query->value(22).toInt()) + "\"";
            n++;
            ui->lab_all->setText(QString::number(n));
            QApplication::processEvents();
        }
        file.close();

        // ===========================
        // собственно открытие шаблона
        // ===========================
        // открытие полученного ODT
        long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_s.utf16()), 0, 0, SW_NORMAL);
    }

    // запомним, что все эти данные уже отсылались
    QString sql_upd;
    sql_upd += "update persons_assig "
               " set status=0 "
               " where status<0 ; ";

    QSqlQuery *query_upd = new QSqlQuery(db);
    bool res_upd = mySQL.exec(this, sql_upd, QString("Обновим статус отосланных прикреплений"), *query_upd, true, db, data_app);
    if (!res_upd) {
        QMessageBox::warning(this, "Ошибка обновления", "При обновлении статуса прикрепления произошла неожиданная ошибка.");
        delete query_upd;
        return;
    }
    delete query_upd;

    return;
}
