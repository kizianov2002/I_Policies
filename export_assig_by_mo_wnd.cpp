#include "export_assig_by_mo_wnd.h"
#include "ui_export_assig_by_mo_wnd.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QTextStream>
#include <QTableWidgetItem>

export_assig_by_mo_wnd::export_assig_by_mo_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    QDialog(parent), db(db), data_app(data_app), settings(settings), ui(new Ui::export_assig_by_mo_wnd)
{
    ui->setupUi(this);
    //on_bn_fname_clicked();

    ui->ln_folder->setText(data_app.path_out + "_ASSIG_DATA_/");
}

export_assig_by_mo_wnd::~export_assig_by_mo_wnd() {
    delete ui;
}

void export_assig_by_mo_wnd::on_bn_close_clicked() {
    close();
}

void export_assig_by_mo_wnd::on_bn_fname_clicked() {
    QString folder_s = QFileDialog::getExistingDirectory(this, "Куда сохранить файлы прикреплений к ЛПУ?", ui->ln_folder->text());
    if (folder_s.isEmpty()) {
        QMessageBox::warning(this,
                             "Не выбрана целевая папка",
                             "Имя целевой папки не задано.\n\n"
                             "Операция отменена.");
        return;
    }
    /*if (QFile::exists(fname_s)) {
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
    }*/
    ui->ln_folder->setText(folder_s);
}

/*#include "qt_windows.h"
#include "qwindowdefs_win.h"
#include <shellapi.h>*/

void export_assig_by_mo_wnd::on_bn_save_to_csv_clicked() {
    QString fname_s = ui->ln_fname->text();

    if (fname_s.isEmpty()) {
        QMessageBox::warning(this,
                             "Не выбрана целевая папка",
                             "Имя целевой папки не задано.\n\n"
                             "Операция отменена.");
        return;
    }

    QString folder = ui->ln_folder->text();
    QString fname = ui->ln_fname->text();

    // выберем данные для сохранения
    QString date = QDate::currentDate().toString("yyyy-MM-dd");

    // выберем данные для сохранения
    QString sql_medorgs;
    sql_medorgs += "select * from update_pers_links('#DATE_REP#') ; "
                   "select distinct m.id, m.code_mo, m.name_mo "
                   "  from persons e "
                   "  left join link_persons_asg la on(la.id_person=e.id) "
                   "  left join polises p on(p.id=e.id_polis) "
                   "  left join persons_assig asg on(asg.id=la._id_asg) "
                   "  left join spr_medorgs m on( m.code_mo=asg.code_mo and m.is_head=1 ) "
                   " where (e.date_death is NULL or e.date_death>'#DATE_REP#') "
                   "   and m.code_mo is not NULL "
                   "   and (select * from st_person(e.id, '#DATE_REP#'))>-100 ; ";
    sql_medorgs = sql_medorgs.replace("#DATE_REP#", date);

    QSqlQuery *query_medorgs = new QSqlQuery(db);
    bool res_dstr = mySQL.exec(this, sql_medorgs, "Запросим данные для сохранения", *query_medorgs, true, db, data_app);
    if (!res_dstr || query_medorgs->size()<=0) {
        QMessageBox::warning(this, "Данные для сохранения не получены", "При выборе полного списка медорганизаций произошла неожиданная ошибка.");
        delete query_medorgs;
        return;
    };

    int n_mo = 0;
    int fcnt = 0;
    ui->lab_all->setText(QString::number(query_medorgs->size()));
    ui->lab_cnt->setText("0");

    // переберём медорганизации
    while (query_medorgs->next()) {
        n_mo++;

        int id_mo = query_medorgs->value(0).toInt();
        QString code_mo = query_medorgs->value(1).toString();
        QString text = query_medorgs->value(2).toString();

        QString fname_s = folder + "/" + QDate::currentDate().toString("yyyyMMdd") + "_" + code_mo + "_" + fname + ".csv";

        QFile file;
        file.setFileName(fname_s);
        if (file.exists()) {
            /*if (QMessageBox::question(this,
                                      "Удалить старый файл?",
                                      "Файл уже существует.\n"
                                      "Удалить старый файл для того чтобы сохранить новый?",
                                      QMessageBox::Yes|QMessageBox::Cancel)==QMessageBox::Yes) {*/
                if (!QFile::remove(fname_s)) {
                    QMessageBox::warning(this,
                                         "Ошибка при удалении старого файла",
                                         "При удалении старого файла произошла непредвиденная ошибка.\n\n"
                                         "Операция отменена.");
                    return;
                }
            //}
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
        QString head = "\"Код_типа_ДПФС\";\"ИД_полиса\";\"ЕНП\";\"Фамилия\";\"Имя\";\"Отчество\";\"Дата_рождения\";\"Место_рождения\";"
                       "\"Тип_УДЛ\";\"УДЛ\";\"Дата_УДЛ\";\"Орган_УДЛ\";\"СНИЛС\";"
                       "\"Идентификатор_МО\";\"Подразделение\";\"Участок\";\"Тип_участка\";\"СНИЛС_врача\";"
                       "\"Способ_прикрепления\";\"Тип_прикрепления\";\"Дата_прикрепления\";\"Дата_открепления\";"
                       "\"Регион\";\"Район\";\"Город\";\"Нас.пункт\";\"Улица\";\"Дом\";\"Корпус\";\"Квартира\"";
        if (data_app.assig_csv_headers) {
            stream << head;
            n++;
        }

        // переберём полученные данные и сохраним в файл

        QString date = QDate::currentDate().toString("yyyy-MM-dd");
        QString sql;
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
               "     a.code_mo, a.code_mp, a.code_ms, ms.type_ms, a.snils_mt, "
               "     a.assig_type, a.assig_way, a.assig_date, a.unsig_date, "
               "     addr.subj_name, addr.dstr_name, addr.city_name, addr.nasp_name, addr.strt_name, addr.house, addr.corp, addr.quart "
               "  from persons e "
               "  left join link_persons_asg la on(la.id_person=e.id) "
               "  left join addresses addr on(addr.id=e.id_addr_reg) ";

        // наличие полиса
        if (ui->ch_polis->isChecked())
             sql += " join ";
        else sql += " left join ";
        sql +=                 " polises p on ( p.id=e.id_polis ) ";

        // наличие УДЛ
        if (ui->ch_udl->isChecked())
             sql += " join ";
        else sql += " left join ";
        sql +=                 " persons_docs d on ( d.id=e.id_udl ) ";

        // наличие прикрепления
        sql += "       join persons_assig a on ( a.id=la._id_asg and a.is_actual=0 ) "
               "  left join spr_medorgs m on ( m.code_mo=a.code_mo and m.is_head=1 ) "
               "  left join spr_medsites ms on ( ms.code_mo=a.code_mo and ms.code_mp=a.code_mp and ms.code_ms=a.code_ms and m.is_head=1 ) "
               " where (e.date_death is NULL or e.date_death>'#DATE_REP#') "
               "   and a.code_mo='" + code_mo + "' "
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

        int pcnt = 0;

        while (query->next()) {
            if (n>0) stream << "\n";

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
            stream << "\"" + query->value(15).toString().trimmed().simplified() + "\";";
            // для Белгорода тип участка кодируется по белгородскому справочнику
            int ms_type = query->value(16).toInt();
            if (data_app.ocato=="14000") {
                int ms_type_belg = 0;
                switch (ms_type) {
                    case 1:
                        ms_type_belg = 2;
                        break;
                    case 2:
                        ms_type_belg = 1;
                        break;
                    case 3:
                        ms_type_belg = 4;
                        break;
                    case 4:
                        ms_type_belg = 4;
                        break;
                    case 5:
                        ms_type_belg = 3;
                        break;
                    default:
                            ms_type_belg = 3;
                        break;
                }
                stream << "\"" + QString::number(ms_type_belg) + "\";";
            } else {
                stream << "\"" + QString::number(ms_type) + "\";";
            }
            stream << "\"" + query->value(17).toString().trimmed().simplified() + "\";";
            stream << "\"" + query->value(18).toString().trimmed().simplified() + "\";";
            stream << "\"\";";
            stream << "\"" + query->value(20).toDate().toString("yyyyMMdd") + "\";";
            stream << "\"" + query->value(21).toDate().toString("yyyyMMdd") + "\";";

            stream << "\"" + query->value(22).toString().trimmed().simplified() + "\";";
            stream << "\"" + query->value(23).toString().trimmed().simplified() + "\";";
            stream << "\"" + query->value(24).toString().trimmed().simplified() + "\";";
            stream << "\"" + query->value(25).toString().trimmed().simplified() + "\";";
            stream << "\"" + query->value(26).toString().trimmed().simplified() + "\";";
            stream << "\"" + QString::number(query->value(27).toInt()) + "\";";
            stream << "\"" + query->value(28).toString().trimmed().simplified() + "\";";
            stream << "\"" + query->value(29).toString().trimmed().simplified() + "\";";
            n++;
            pcnt++;
        }
        file.close();

        if (pcnt==0) {
            // ничего не сохранено - удалим файл
            file.remove();
        } else {
            fcnt++;
        }

        ui->lab_cnt->setText(QString::number(n_mo));
        QApplication::processEvents();
    }

    // ===========================
    // собственно открытие шаблона
    // ===========================
    long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(folder.utf16()), 0, 0, SW_NORMAL);
}

