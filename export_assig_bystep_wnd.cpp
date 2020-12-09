#include "export_assig_bystep_wnd.h"
#include "ui_export_assig_bystep_wnd.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QTextStream>
#include <QTableWidgetItem>

export_assig_bystep_wnd::export_assig_bystep_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    QDialog(parent), db(db), data_app(data_app), settings(settings), ui(new Ui::export_assig_bystep_wnd)
{
    ui->setupUi(this);
    ui->bn_save_to_csv->setEnabled(false);

    ui->ln_fname->setText(data_app.path_out/* + "_ASSIG_DATA_/"*/);

    if(this->data_app.ocato=="38000") {
        ui->ch_assig_medter->setChecked(false);
    } else {
        ui->ch_assig_medter->setChecked(true);
    }

    on_date_assig_editingFinished();
}

export_assig_bystep_wnd::~export_assig_bystep_wnd() {
    delete ui;
}

void export_assig_bystep_wnd::on_bn_close_clicked() {
    close();
}

void export_assig_bystep_wnd::on_date_assig_editingFinished() {
    QString fname_s = ui->ln_fname->text();

    // имя файла
    QDir d(fname_s);
    if (fname_s.toLower().contains(".csv")) {
        d.cdUp();
        fname_s = d.path();
    }
    fname_s += "/MO1" + data_app.smo_regnum + QDate::currentDate().toString("yyyyMMdd") + ".csv";
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

void export_assig_bystep_wnd::on_bn_fname_clicked() {

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
        fname_s += "/MO1" + data_app.smo_regnum + QDate::currentDate().toString("yyyyMMdd") + ".csv";

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

void export_assig_bystep_wnd::on_bn_save_to_csv_clicked() {

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

    db.transaction();

    // переберём полученные данные и сохраним в файл
    {   // выберем данные для сохранения
        QString date = QDate::currentDate().toString("yyyy-MM-dd");
        QString sql;
        //sql += "select * from update_pers_links('#DATE_REP#') ; ";    // нет смысла для текущей даты
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
               "     t.prof_mt, "
               "     case a.status "
               "       when -1 then 'У' "
               "       when 2 then 'Р' "
               "       when 1 then 'И' "
               "       else 'Р' "
               "     end as action_event, "
               "     e.id as id_person,"
               "     a.id "
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

        // наличие прикрепления к МО
        if (ui->ch_assig->isChecked()) {
            sql += " join ";
        } else {
            sql += " left join ";
        };
        sql += "    persons_assig a on ( a.id=la._id_asg and a.is_actual=0 ) ";

        // данные МО
        //"    left join spr_medorgs m on ( m.code_mo=a.code_mo and m.is_head=1 ) "
        sql += " left join ";
        sql += "    ( select max(id) as id, code_mo "
               "      from spr_medorgs m "
               "      where m.is_head=1 "
               "      group by code_mo "
               "    ) qm on ( qm.code_mo=a.code_mo ) "
               "    left join spr_medorgs m on ( m.id=qm.id ) ";

        // наличие прикрепления к врачу
        if (ui->ch_assig_medter->isChecked()) {
            sql += " join ";
        } else {
            sql += " left join ";
        };
        sql += "    ( select max(id) as id, code_mo, snils_mt "
               "      from spr_medters t "
               "      group by code_mo, snils_mt "
               "    ) qt on ( qt.code_mo=a.code_mo and qt.snils_mt=a.snils_mt ) "
               "    left join spr_medters t on ( t.id=qt.id ) ";

        // только застрахованные :) и живые :]]_
        sql += " where (e.date_death is NULL or e.date_death>'#DATE_REP#') "
               "   and (a.status is NULL or a.status<>0)  "
               "   and e.status>-100 "
               " order by e.fam, e.im, e.ot, e.date_birth ; ";


        sql = sql.replace("#DATE_REP#", date);


        QSqlQuery *query = new QSqlQuery(db);
        bool res = mySQL.exec(this, sql, QString("Выбор данных прикрепления на заданное число"), *query, true, db, data_app);
        if (!res) {
            QMessageBox::warning(this, "Данные не получены", "При выборе данных прикрепления произошла неожиданная ошибка.");
            delete query;
            db.rollback();
            return;
        }
        query->next();
        int cnt = query->size();

        if (cnt>0) {
            ui->lab_all->setText(QString::number(cnt));
            ui->lab_cnt->setText("0");

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
                        db.rollback();
                        return;
                    }
                }
            }

            // подготовим поток для сохранения файла
            QTextStream stream(&file);
            QString codec_name = ui->combo_encoding->currentText();
            QTextCodec *codec = QTextCodec::codecForName(codec_name.toLocal8Bit());
            stream.setCodec(codec);

            if (!file.open(QIODevice::WriteOnly)) {
                QMessageBox::warning(this,
                                     "Ошибка создания файла",
                                     "При создании целевого файла произошла ошибка.\n\n"
                                     "Операция отменена.");
                db.rollback();
                return;
            }

            // добавим запись файла в таблицу файлов по прикреплению
            QFileInfo fi = QFileInfo(fname_s);
            QString sql_file = "insert into files_out_assig(file_name, date_send, n_recs, bin) "
                               " values ('" + fi.baseName() + "', CURRENT_DATE, NULL, NULL) "
                               " returning id ; ";

            QSqlQuery *query_file = new QSqlQuery(db);
            bool res_file = mySQL.exec(this, sql_file, QString("Получим ID записи файла с прикреплениями"), *query_file, true, db, data_app);
            if (!res_file) {
                QMessageBox::warning(this, "Ошибка добавления файла прикреплений", "При попытке добавить файл с прикреплениями произошла неожиданная ошибка.");
                delete query_file;
                db.rollback();
                return;
            }
            query_file->next();
            int id_file_assig = query_file->value(0).toInt();
            delete query_file;

            do {
                if (n>0)
                    stream << "\n";

                // добавление строки FREC
                QString sql_line = "insert into frecs_out_assig( "
                                       " id_file_assig, "
                                       " n, "
                                       " pol_type, "
                                       " pol_id, "
                                       " enp, "
                                       " fam, "
                                       " im, "
                                       " ot, "
                                       " date_birth, "
                                       " plc_birth, "
                                       " doc_type, "
                                       " doc_sernum, "
                                       " doc_date, "
                                       " doc_org, "
                                       " snils, "
                                       " code_mo, "
                                       " assig_type, "
                                       " assig_way, "
                                       " assig_date, "
                                       " unsig_date, "
                                       " code_mp, "
                                       " code_ms, "
                                       " snils_mt, "
                                       " oid_mo, "
                                       " prof_mt, "
                                       " action_event, "
                                       " id_person) "
                                   " values ("
                                       " " + QString::number(id_file_assig) + ", "
                                       " " + QString::number(n+1) + ", "
                                       " " + (query->value( 0).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query->value(0).toString().trimmed()  + "'")) + ", "
                                       " " + (query->value( 1).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query->value(1).toString().trimmed()  + "'")) + ", "
                                       " " + (query->value( 2).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query->value(2).toString().trimmed()  + "'")) + ", "
                                       " " + (query->value( 3).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query->value(3).toString().trimmed()  + "'")) + ", "
                                       " " + (query->value( 4).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query->value(4).toString().trimmed()  + "'")) + ", "
                                       " " + (query->value( 5).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query->value(5).toString().trimmed()  + "'")) + ", "
                                       " " + (query->value( 6).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query->value(6).toDate().toString("yyyy-MM-dd")    + "'")) + ", "
                                       " " + (query->value( 7).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query->value(7).toString().trimmed()  + "'")) + ", "
                                       " " + (query->value( 8).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query->value(8).toString().trimmed()  + "'")) + ", "
                                       " " + (query->value( 9).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query->value(9).toString().trimmed()  + "'")) + ", "
                                       " " + (query->value(10).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query->value(10).toDate().toString("yyyy-MM-dd")   + "'")) + ", "
                                       " " + (query->value(11).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query->value(11).toString().trimmed() + "'")) + ", "
                                       " " + (query->value(12).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query->value(12).toString().trimmed() + "'")) + ", "
                                       " " + (query->value(13).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query->value(13).toString().trimmed() + "'")) + ", "
                                       " " + (query->value(14).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query->value(14).toString().trimmed() + "'")) + ", "
                                       " NULL, "
                                       " " + (query->value(16).isNull()                                    ? "NULL" : QString("'" + query->value(16).toDate().toString("yyyy-MM-dd")   + "'")) + ", "
                                       " " + (query->value(17).isNull()                                    ? "NULL" : QString("'" + query->value(17).toDate().toString("yyyy-MM-dd")   + "'")) + ", "
                                       " " + (query->value(18).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query->value(18).toString().trimmed() + "'")) + ", "
                                       " " + (query->value(19).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query->value(19).toString().trimmed() + "'")) + ", "
                                       " " + (query->value(20).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query->value(20).toString().trimmed() + "'")) + ", "
                                       " " + (query->value(21).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query->value(21).toString().trimmed() + "'")) + ", "
                                       " " + (query->value(22).isNull()                                    ? "NULL" : QString("'" + QString::number(query->value(22).toInt()))         + "'")  + ", "
                                       " " + (query->value(23).toString().trimmed().isEmpty() ? "NULL" : QString("'" + query->value(23).toString().trimmed() + "'")) + ", "
                                       " " + (query->value(24).isNull()                                    ? "NULL" : QString("'" + QString::number(query->value(24).toInt()))         + "'")  + ") "
                                   " returning id ; ";

                QSqlQuery *query_line = new QSqlQuery(db);
                bool res_line = mySQL.exec(this, sql_line, QString("Запомним строку файла с прикреплениями"), *query_line, true, db, data_app);
                if (!res_line) {
                    QMessageBox::warning(this, "Ошибка добавления строки файла прикреплений", "При попытке добавить строку файла с прикреплениями произошла неожиданная ошибка.");
                    delete query_line;
                    db.rollback();
                    return;
                }

                int id_assig = query->value(25).toInt();
                // запомним, что все эти данные уже отсылались
                QString sql_upd;
                sql_upd += "update persons_assig a "
                           "   set status=0 "
                           " where a.id=" + QString::number(id_assig) + " ; ";
                QSqlQuery *query_upd = new QSqlQuery(db);
                bool res_upd = mySQL.exec(this, sql_upd, QString("Обновим статус отосланных прикреплений"), *query_upd, true, db, data_app);
                if (!res_upd) {
                    QMessageBox::warning(this, "Ошибка обновления", "При обновлении статуса прикрепления произошла неожиданная ошибка.");
                    delete query_upd;
                    db.rollback();
                    return;
                }
                delete query_upd;


                query_line->next();

                stream << (query->value(23).toString().trimmed().isEmpty() ? "" : QString("\"" + query->value(23).toString().trimmed() + "\"")) + ";";
                stream << (query->value( 0).toString().trimmed().isEmpty() ? "" : QString("\"" + query->value(0).toString().trimmed()  + "\"")) + ";";
                stream << (query->value( 1).toString().trimmed().isEmpty() ? "" : QString("\"" + query->value(1).toString().trimmed()  + "\"")) + ";";
                stream << (query->value( 2).toString().trimmed().isEmpty() ? "" : QString("\"" + query->value(2).toString().trimmed()  + "\"")) + ";";
                stream << (query->value( 3).toString().trimmed().isEmpty() ? "" : QString("\"" + query->value(3).toString().trimmed()  + "\"")) + ";";
                stream << (query->value( 4).toString().trimmed().isEmpty() ? "" : QString("\"" + query->value(4).toString().trimmed()  + "\"")) + ";";
                stream << (query->value( 5).toString().trimmed().isEmpty() ? "" : QString("\"" + query->value(5).toString().trimmed()  + "\"")) + ";";
                stream << (query->value( 6).toString().trimmed().isEmpty() ? "" : QString("\"" + query->value(6).toDate().toString("yyyyMMdd")      + "\"")) + ";";
                stream << (query->value( 7).toString().trimmed().isEmpty() ? "" : QString("\"" + query->value(7).toString().trimmed()  + "\"")) + ";";
                stream << (query->value( 8).toString().trimmed().isEmpty() ? "" : QString("\"" + query->value(8).toString().trimmed()  + "\"")) + ";";
                stream << (query->value( 9).toString().trimmed().isEmpty() ? "" : QString("\"" + query->value(9).toString().trimmed()  + "\"")) + ";";
                stream << (query->value(10).toString().trimmed().isEmpty() ? "" : QString("\"" + query->value(10).toDate().toString("yyyyMMdd")     + "\"")) + ";";
                stream << (query->value(11).toString().trimmed().isEmpty() ? "" : QString("\"" + query->value(11).toString().trimmed() + "\"")) + ";";
                stream << (query->value(12).toString().trimmed().isEmpty() ? "" : QString("\"" + query->value(12).toString().trimmed() + "\"")) + ";";
                stream << (query->value(13).toString().trimmed().isEmpty() ? "" : QString("\"" + query->value(13).toString().trimmed() + "\"")) + ";";
                stream << (query->value(14).toString().trimmed().isEmpty() ? "" : QString("\"" + query->value(14).toString().trimmed() + "\"")) + ";";
                stream << ";";
                stream << (query->value(16).isNull()                                    ? "" : QString("\"" + query->value(16).toDate().toString("yyyyMMdd")     + "\"")) + ";";
                stream << (query->value(17).isNull()                                    ? "" : QString("\"" + query->value(17).toDate().toString("yyyyMMdd")     + "\"")) + ";";
                stream << (query->value(21).toString().trimmed().isEmpty() ? "" : QString("\"" + query->value(21).toString().trimmed() + "\"")) + ";";
                stream << (query->value(18).toString().trimmed().isEmpty() ? "" : QString("\"" + query->value(18).toString().trimmed() + "\"")) + ";";
                stream << (query->value(19).toString().trimmed().isEmpty() ? "" : QString("\"" + query->value(19).toString().trimmed() + "\"")) + ";";
                stream << (query->value(20).toString().trimmed().isEmpty() ? "" : QString("\"" + query->value(20).toString().trimmed() + "\"")) + ";";
                stream << (query->value(22).isNull()                                    ? "" : QString("\"" + QString::number(query->value(22).toInt()))         + "\"");

                n++;
                ui->lab_cnt->setText(QString::number(n));
                QApplication::processEvents();
            } while (query->next());
            file.close();

            // Запомним число строк в сохранённом файле по прикреплению
            QString bin;
            mySQL.myBYTEA(fname_s, bin);
            QString sql_nrecs = "update files_out_assig "
                                "   set n_recs=" + QString::number(n) + ", "
                                "       bin='" + bin + "' "
                                " where id=" + QString::number(id_file_assig) + " ; ";

            QSqlQuery *query_nrecs = new QSqlQuery(db);
            bool res_nrecs = mySQL.exec(this, sql_nrecs, QString("Запомним число строк в сохранённом файле с прикреплениями"), *query_nrecs, true, db, data_app);
            if (!res_nrecs) {
                QMessageBox::warning(this, "Ошибка сохранении размера файла прикреплений", "При попытке запомнить число строк в сохранённом файле с прикреплениями произошла неожиданная ошибка.");
                delete query_nrecs;
                db.rollback();
                return;
            }
            delete query_nrecs;

            // ===========================
            // собственно открытие шаблона
            // ===========================
            // открытие полученного ODT
            long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_s.utf16()), 0, 0, SW_NORMAL);

        } else {
            QMessageBox::information(this, "Нечего выгружать", "Нет новых данных прикрепления.");
        }
    }
    db.commit();
    QMessageBox::information(this, "Готово", "Готово.");
    close();
}
