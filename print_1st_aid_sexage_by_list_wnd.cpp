#include "print_1st_aid_sexage_by_list_wnd.h"
#include "ui_print_1st_aid_sexage_by_list_wnd.h"

print_1st_aid_sexage_by_list_wnd::print_1st_aid_sexage_by_list_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    db(db),
    data_app(data_app),
    settings(settings),
    QDialog(parent),
    ui(new Ui::print_1st_aid_sexage_by_list_wnd)
{
    ui->setupUi(this);

    on_bn_date_assig_clicked();
    ui->ln_smo_name->setText(data_app.smo_name);
    ui->ln_terr_name->setText(data_app.terr_name);
    ui->ln_terr_name_rp->setText(data_app.terr_name_rp);
    ui->ln_filial_director->setText(data_app.filial_director);
    ui->ln_filial_director_2->setText(data_app.filial_director_2);
    ui->ln_filial_chif->setText(data_app.filial_chif);

    on_bn_first_day_clicked();
}

print_1st_aid_sexage_by_list_wnd::~print_1st_aid_sexage_by_list_wnd() {
    delete ui;
}

void print_1st_aid_sexage_by_list_wnd::on_bn_cansel_clicked() {
    reject();
}

void print_1st_aid_sexage_by_list_wnd::on_bn_date_assig_clicked() {
    ui->date_assig->setDate(QDate::currentDate());
}

QString print_1st_aid_sexage_by_list_wnd::month_to_str(QDate date) {
    QString res;
    switch (date.month()) {
    case 1:
        res += "января ";
        break;
    case 2:
        res += "февраля ";
        break;
    case 3:
        res += "марта ";
        break;
    case 4:
        res += "апреля ";
        break;
    case 5:
        res += "мая ";
        break;
    case 6:
        res += "июня ";
        break;
    case 7:
        res += "июля ";
        break;
    case 8:
        res += "августа ";
        break;
    case 9:
        res += "сентября ";
        break;
    case 10:
        res += "октября ";
        break;
    case 11:
        res += "ноября ";
        break;
    case 12:
        res += "декабря ";
        break;
    default:
        break;
    }
    return res;
}

void print_1st_aid_sexage_by_list_wnd::on_bn_edit_clicked(bool checked) {
    if (checked) {
        ui->bn_edit->setText("Сохранить");

        ui->lab_smo_name->setEnabled(true);
        ui->ln_smo_name->setEnabled(true);
        ui->lab_terr_name->setEnabled(true);
        ui->ln_terr_name->setEnabled(true);
        ui->lab_terr_name_rp->setEnabled(true);
        ui->ln_terr_name_rp->setEnabled(true);
        ui->lab_filial_director->setEnabled(true);
        ui->ln_filial_director->setEnabled(true);
        ui->lab_filial_chif->setEnabled(true);
        ui->ln_filial_director_2->setEnabled(true);
        ui->ln_filial_chif->setEnabled(true);

        ui->bn_print->setEnabled(false);
    } else {
        data_app.smo_name = ui->ln_smo_name->text();
        data_app.terr_name = ui->ln_terr_name->text();
        data_app.terr_name_rp = ui->ln_terr_name_rp->text();
        data_app.filial_director = ui->ln_filial_director->text();
        data_app.filial_director_2 = ui->ln_filial_director_2->text();
        data_app.filial_chif = ui->ln_filial_chif->text();
        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Сохранить сделанные изменения в настройках программы?\n\n"
                                  "В случае согласия новые данные будут сохранены и останутся доступны после перезагрузки программы.\n"
                                  "В случае отказа эти изменения будут доступны только до первого перезапуска программы.",
                                  QMessageBox::Yes|QMessageBox::No,
                                  QMessageBox::No)==QMessageBox::Yes)
        {
            // [filial]
            settings.beginGroup("filial");
            settings.setValue("smo_name", data_app.smo_name.toUtf8());
            settings.setValue("terr_name", data_app.terr_name.toUtf8());
            settings.setValue("terr_name_2", data_app.terr_name_rp.toUtf8());
            settings.setValue("filial_director", data_app.filial_director.toUtf8());
            settings.setValue("filial_director_2", data_app.filial_director_2.toUtf8());
            settings.setValue("filial_chif", data_app.filial_chif.toUtf8());
            settings.sync();
        }
        ui->bn_edit->setText("Изменить");

        ui->lab_smo_name->setEnabled(false);
        ui->ln_smo_name->setEnabled(false);
        ui->lab_terr_name->setEnabled(false);
        ui->ln_terr_name->setEnabled(false);
        ui->lab_terr_name_rp->setEnabled(false);
        ui->ln_terr_name_rp->setEnabled(false);
        ui->lab_filial_director->setEnabled(false);
        ui->ln_filial_director->setEnabled(false);
        ui->ln_filial_director_2->setEnabled(false);
        ui->lab_filial_chif->setEnabled(false);
        ui->ln_filial_chif->setEnabled(false);

        ui->bn_print->setEnabled(true);
    }
}

#include "qt_windows.h"
#include "qwindowdefs_win.h"
#include <shellapi.h>

typedef struct {
    QString code_mo;
    QString name_mo;
    int m_0;
    int w_0;
    int m14;
    int w14;
    int m517;
    int w517;
    int m1859;
    int w1854;
    int m60_;
    int w55_;
    int sum;
} s_sexage_rec;

void print_1st_aid_sexage_by_list_wnd::on_bn_print_clicked() {
    this->setCursor(Qt::WaitCursor);
    ui->bn_print->setEnabled(false);

    // распаковка шаблона реестра
    QString fname_template = data_app.path_reports + "rep_1st_aid_sexage_tab.ods";
    QString temp_folder = data_app.path_temp + "rep_1st_aid_sexage_tab";
    if (!QFile::exists(fname_template)) {
        QMessageBox::warning(this,
                             "Шаблон не найден",
                             "Не найден шаблон печатной формы половозрастной таблицы прикреплённых к станции скорой помощи: \n" + fname_template +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    QDir tmp_dir(temp_folder);
    if (tmp_dir.exists(temp_folder) && !tmp_dir.removeRecursively()) {
        QMessageBox::warning(this,
                             "Ошибка при очистке папки",
                             "Не удалось очистить папку временных файлов: \n" + temp_folder +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    tmp_dir.mkpath(temp_folder);
    QProcess myProcess;
    QString com = "\"" + data_app.path_arch + "7z.exe\" e \"" + fname_template + "\" -o\"" + temp_folder + "\" content.xml";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка распаковки шаблона",
                             "Не удалось распаковать файл контента шаблона\n" + fname_template + " -> content.xml"
                             "\nпечатной формы во временную папку\n" + temp_folder +
                             "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    com = "\"" + data_app.path_arch + "7z.exe\" e \"" + fname_template + "\" -o\"" + temp_folder + "\" styles.xml";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка распаковки шаблона",
                             "Не удалось распаковать файл стилей шаблона \n" + fname_template + " -> styles.xml\n"
                             "печатной формы во временную папку\n" + temp_folder +
                             "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки.\n\n"
                             "Операция прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }

    // ===========================
    // правка контента шаблона
    // ===========================
    // открытие контента шаблона
    QFile f_content(temp_folder + "/content.xml");
    f_content.open(QIODevice::ReadOnly);
    QString s_content = f_content.readAll();
    f_content.close();
    if (s_content.isEmpty()) {
        QMessageBox::warning(this,
                             "Файл контента шаблона пустой",
                             "Файл контента шаблона пустой. Возможно шаблон был испорчен.\n\n"
                             "Операция прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    // выбор шаблона
    int pos0 = s_content.indexOf("<table:table-row table:style-name=\"ro6\">", 0);
    int pos1 = s_content.indexOf("</table:table-row>", pos0) + 18;
    QString s_row = s_content.mid(pos0, pos1-pos0);

    // правка полей контента шаблона
    QString s_content_new = s_content.left(pos0);

    // пороговые даты
    QDate date_assig = ui->date_assig->date();
    QDate date_01 = date_assig.addYears( -1);
    QDate date_05 = date_assig.addYears( -5);
    QDate date_18 = date_assig.addYears(-18);
    QDate date_55 = date_assig.addYears(-55);
    QDate date_60 = date_assig.addYears(-60);
    QString date_str = date_assig.toString("yy-MM-yyyy");

    // получим список медорганизаций
    QString sql_medorgs;
    sql_medorgs += "select m.id, m.code_mo, m.name_mo, m.mo_letter_text "
                   "  from spr_medorgs m "
                   " where m.mo_1st_aid=1 "
                   " order by m.id ; ";

    QSqlQuery *query_medorgs = new QSqlQuery(db);
    bool res_medorgs = mySQL.exec(this, sql_medorgs, "Запросим персону, прикреплённую к данной станции скорой помощи", *query_medorgs, true, db, data_app);

    if (!res_medorgs || query_medorgs->size()<=0) {
        QMessageBox::warning(this, "Список не получен", "При выборе списка станций скорой помощи произошла неожиданная ошибка.");
        delete query_medorgs;
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    };

    // матрица данных
    s_sexage_rec *rec = NULL;
    QList<s_sexage_rec*> recs;
    while (query_medorgs->next()) {
        s_sexage_rec *rec = new s_sexage_rec;
        rec->code_mo = query_medorgs->value(1).toString();
        rec->name_mo = query_medorgs->value(3).toString().trimmed().simplified();
        rec->m_0   = 0;
        rec->w_0   = 0;
        rec->m14   = 0;
        rec->w14   = 0;
        rec->m517  = 0;
        rec->w517  = 0;
        rec->m1859 = 0;
        rec->w1854 = 0;
        rec->m60_  = 0;
        rec->w55_  = 0;
        rec->sum   = 0;
        recs.append(rec);
    }
    QString code_mo = " - ";

    // суммирование по колонкам
    s_sexage_rec *rec_sum = new s_sexage_rec;
    rec_sum->code_mo = "";
    rec_sum->name_mo = "Сумма : ";
    rec_sum->m_0   = 0;
    rec_sum->w_0   = 0;
    rec_sum->m14   = 0;
    rec_sum->w14   = 0;
    rec_sum->m517  = 0;
    rec_sum->w517  = 0;
    rec_sum->m1859 = 0;
    rec_sum->w1854 = 0;
    rec_sum->m60_  = 0;
    rec_sum->w55_  = 0;
    rec_sum->sum   = 0;


    // получим список прикреплённых застрахованных
    QString date = ui->date_assig->date().toString("yyyy-MM-dd");
    QString sql;
    sql+="select distinct m.code_mo, m.name_mo, e.id, e.fam, e.im, e.ot, e.sex, e.date_birth, m.id "
         "  from persons e ";

    // наличие полиса
    if (ui->ch_polis->isChecked())
         sql+=" join ";
    else sql+=" left join ";
    sql += "    polises p on ( p.id=e.id_polis /*and p.date_begin<='#DATE_REP#' and (p.date_stop is NULL or p.date_stop>'#DATE_REP#')*/ ) ";

    // наличие УДЛ
    if (ui->ch_udl->isChecked())
         sql+=" join ";
    else sql+=" left join ";
    sql += "    persons_docs d on ( d.id=e.id_udl /*and d.doc_date<='#DATE_REP#' and (d.doc_exp is NULL or d.doc_exp>'#DATE_REP#')*/ ) ";

    // наличие прикрепления
    sql += " left join ";
    sql += "    persons_assig a on(a.id=(select * from id_person_asg(e.id, '#DATE_REP#')))  "
           "    left join spr_medorgs m on ( m.is_head=1 and m.mo_1st_aid=1 and ((e.code_1st_aid is not NULL and m.code_mo=e.code_1st_aid) or (e.code_1st_aid is NULL and m.code_mo=a.code_mo) ) ) "
           " where (select * from st_person(e.id,'#DATE_REP#')) in(1,2,3,4,11,12,13,14) ";

    if (ui->ch_tfoms->isChecked())
        sql+="   and p.tfoms_date is not NULL ";
    sql += " order by m.id ; ";

    sql = sql.replace("#DATE_REP#", date);

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, "Запросим персону, прикреплённую к данной станции скорой помощи", *query, true, db, data_app);

    if (!res || query->size()<=0) {
        QMessageBox::warning(this, "Список не получен", "При выборе полного списка прикреплённых к станции скорой помощи произошла неожиданная ошибка.");
        delete query;
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    };
    ui->lab_all->setText("0");
    ui->lab_cnt->setText("0");

    // счётчик
    int n = 0;

    // переберём районы и станции скорой помощи области
    while (query->next()) {
        if (code_mo!=query->value(0).toString()) {
            QString code_mo_new = query->value(0).toString();
            code_mo = " - ";
            // найдём нужную медорганизацию
            for (int i=0; i<recs.count(); i++) {
                if (code_mo_new==recs.at(i)->code_mo) {
                    rec = recs.at(i);
                    code_mo = code_mo_new;
                    break;
                }
            }
            if (code_mo==" - ") {
                // неизвестный код медорганизации - пропустим
                continue;
            }
        }
        // получим данные застрахованного
        int sex = query->value(6).toInt();
        QDate date_birth = query->value(7).toDate();

        // добавим запись к счётчикам
        if (sex==1) {
            if (date_birth>date_01) {
                rec->m_0++;
                rec_sum->m_0++;
            } else if (date_birth>date_05) {
                rec->m14++;
                rec_sum->m14++;
            } else if (date_birth>date_18) {
                rec->m517++;
                rec_sum->m517++;
            } else if (date_birth>date_60) {
                rec->m1859++;
                rec_sum->m1859++;
            } else {
                rec->m60_++;
                rec_sum->m60_++;
            }
        } else {
            if (date_birth>date_01) {
                rec->w_0++;
                rec_sum->w_0++;
            } else if (date_birth>date_05) {
                rec->w14++;
                rec_sum->w14++;
            } else if (date_birth>date_18) {
                rec->w517++;
                rec_sum->w517++;
            } else if (date_birth>date_55) {
                rec->w1854++;
                rec_sum->w1854++;
            } else {
                rec->w55_++;
                rec_sum->w55_++;
            }
        }
        rec->sum++;
        rec_sum->sum++;
    }
    delete query;

    for (int i=0; i<recs.count(); i++) {
        s_sexage_rec *rec = recs.at(i);

        n++;
        QString s_row_new = s_row;
        s_row_new = s_row_new.
                    replace("#N#",    QString::number(n)).
                    replace("#TEXT#", rec->name_mo).
                    replace("#MO#",   rec->code_mo).
                    replace("#M_0#",  (rec->m_0>0  ? QString::number(rec->m_0)  : " ")).
                    replace("#W_0#",  (rec->w_0>0  ? QString::number(rec->w_0)  : " ")).
                    replace("#M14#",  (rec->m14>0  ? QString::number(rec->m14)  : " ")).
                    replace("#W14#",  (rec->w14>0  ? QString::number(rec->w14)  : " ")).
                    replace("#M517#", (rec->m517>0 ? QString::number(rec->m517) : " ")).
                    replace("#W517#", (rec->w517>0 ? QString::number(rec->w517) : " ")).
                    replace("#M1859#",(rec->m1859>0? QString::number(rec->m1859): " ")).
                    replace("#W1854#",(rec->w1854>0? QString::number(rec->w1854): " ")).
                    replace("#M60_#", (rec->m60_>0 ? QString::number(rec->m60_) : " ")).
                    replace("#W55_#", (rec->w55_>0 ? QString::number(rec->w55_) : " ")).
                    replace("#SUM#",  QString::number(rec->sum));
        s_content_new += s_row_new;

        ui->lab_cnt->setText(QString::number(n));
        QApplication::processEvents();
    }

    s_content_new += s_content.right(s_content.size()-pos1);

    s_content_new = s_content_new.
                    replace("#SM_0#",  QString::number(rec_sum->m_0)).
                    replace("#SW_0#",  QString::number(rec_sum->w_0)).
                    replace("#SM14#",  QString::number(rec_sum->m14)).
                    replace("#SW14#",  QString::number(rec_sum->w14)).
                    replace("#SM517#", QString::number(rec_sum->m517)).
                    replace("#SW517#", QString::number(rec_sum->w517)).
                    replace("#SM1859#",QString::number(rec_sum->m1859)).
                    replace("#SW1854#",QString::number(rec_sum->w1854)).
                    replace("#SM60_#", QString::number(rec_sum->m60_)).
                    replace("#SW55_#", QString::number(rec_sum->w55_)).
                    replace("#SSUM#",  QString::number(rec_sum->sum)).

                    replace("#SMO_NAME#", data_app.smo_name).
                    replace("#DATE_REP#", date_assig.toString("dd.MM.yyyy")).
                    replace("#TERR_NAME_RP#", data_app.terr_name_rp).

                    replace("#FILIAL_DIRECTOR#",data_app.filial_director).
                    replace("#FILIAL_DIRECTOR_2#",data_app.filial_director_2).
                    replace("#FILIAL_DIRECTOR_FIO#",data_app.filial_chif);

    for (int i=0; i<recs.count(); i++)
        delete recs.at(i);
    delete rec_sum;

    // сохранение контента шаблона
    QFile f_content_save(temp_folder + "/content.xml");
    f_content_save.open(QIODevice::WriteOnly);
    f_content_save.write(s_content_new.toUtf8());
    f_content_save.close();


    // ===========================
    // архивация шаблона
    // ===========================
    QString fname_res = data_app.path_out + "_REPORTS_/" +
            QDate::currentDate().toString("yyyy-MM-dd") + "  -  " + data_app.smo_short + " - " + data_app.filial_name + "  -  1st_aid_sexage_tab - " + date_assig.toString("yyyy-MM-dd") + ".ods";

    QFile::remove(fname_res);
    while (QFile::exists(fname_res)) {
        if (QMessageBox::warning(this,
                                 "Ошибка сохранения журнала ВС",
                                 "Не удалось сохранить форму половозрастной таблицы прикреплённых к станции скорой помощи: \n" + fname_template +
                                 "\n\nво временную папку\n" + fname_res +
                                 "\n\nПроверьте, не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                 QMessageBox::Retry|QMessageBox::Abort,
                                 QMessageBox::Retry)==QMessageBox::Abort) {
            this->setCursor(Qt::ArrowCursor);
            ui->bn_print->setEnabled(true);
            return;
        }
        QFile::remove(fname_res);
    }
    if (!QFile::copy(fname_template, fname_res) || !QFile::exists(fname_res)) {
        QMessageBox::warning(this, "Ошибка копирования отчёта", "Не удалось скопировать форму половозрастной таблицы прикреплённых к станции скорой помощи: \n" + fname_template + "\n\nво временную папку\n" + fname_res + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    com = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname_res + "\" \"" + temp_folder + "/content.xml\"";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка обновления контента",
                             "При добавлении нового контента в форму половозрастной таблицы прикреплённых к станции скорой помощи произошла непредвиденная ошибка\n\nОпреация прервана.");
    }
    this->setCursor(Qt::ArrowCursor);
    ui->bn_print->setEnabled(true);

    // ===========================
    // собственно открытие шаблона
    // ===========================
    // открытие полученного ODT
    long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);

    QMessageBox::information(this, "Готово", "Готово");
    //close();
}

void print_1st_aid_sexage_by_list_wnd::on_ln_smo_name_textChanged(const QString &arg1) {
    data_app.smo_name = ui->ln_smo_name->text();
}

void print_1st_aid_sexage_by_list_wnd::on_ln_filial_director_textChanged(const QString &arg1) {
    data_app.filial_director = ui->ln_filial_director->text();
}

void print_1st_aid_sexage_by_list_wnd::on_ln_filial_director_2_textChanged(const QString &arg1) {
    data_app.filial_director_2 = ui->ln_filial_director_2->text();
}

void print_1st_aid_sexage_by_list_wnd::on_ln_filial_chif_textChanged(const QString &arg1) {
    data_app.filial_chif = ui->ln_filial_chif->text();
}

void print_1st_aid_sexage_by_list_wnd::on_ln_terr_name_textChanged(const QString &arg1) {
    data_app.terr_name = ui->ln_terr_name->text();
}

void print_1st_aid_sexage_by_list_wnd::on_ln_terr_name_rp_textChanged(const QString &arg1) {
    data_app.terr_name_rp = ui->ln_terr_name_rp->text();
}

void print_1st_aid_sexage_by_list_wnd::on_bn_date_cur_clicked() {
    ui->date_assig->setDate(QDate::currentDate().addDays(-1));
}

void print_1st_aid_sexage_by_list_wnd::on_bn_first_day_clicked() {
    ui->date_assig->setDate(QDate(QDate::currentDate().year(), QDate::currentDate().month(), 1).addDays(-1));
}
