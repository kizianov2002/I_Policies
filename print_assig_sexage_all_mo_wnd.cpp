#include "print_assig_sexage_all_mo_wnd.h"
#include "ui_print_assig_sexage_all_mo_wnd.h"

print_assig_sexage_all_mo_wnd::print_assig_sexage_all_mo_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    db(db),
    data_app(data_app),
    settings(settings),
    QDialog(parent),
    ui(new Ui::print_assig_sexage_all_mo_wnd)
{
    ui->setupUi(this);

    on_bn_date_assig_clicked();
    ui->ln_smo_name->setText(data_app.smo_name);
    ui->ln_terr_name->setText(data_app.terr_name);
    ui->ln_terr_name_rp->setText(data_app.terr_name_rp);
    ui->ln_filial_director->setText(data_app.filial_director);
    ui->ln_filial_director_2->setText(data_app.filial_director_2);
    ui->ln_filial_director_fio->setText(data_app.filial_director_fio);
}

print_assig_sexage_all_mo_wnd::~print_assig_sexage_all_mo_wnd() {
    delete ui;
}

void print_assig_sexage_all_mo_wnd::on_bn_cansel_clicked() {
    reject();
}

void print_assig_sexage_all_mo_wnd::on_bn_date_assig_clicked() {
    ui->date_assig->setDate(QDate::currentDate());
}

QString print_assig_sexage_all_mo_wnd::month_to_str(QDate date) {
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

void print_assig_sexage_all_mo_wnd::on_bn_edit_clicked(bool checked) {
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
        ui->lab_filial_director_fio->setEnabled(true);
        ui->ln_filial_director_2->setEnabled(true);
        ui->ln_filial_director_fio->setEnabled(true);

        ui->bn_print->setEnabled(false);
    } else {
        data_app.smo_name = ui->ln_smo_name->text();
        data_app.terr_name = ui->ln_terr_name->text();
        data_app.terr_name_rp = ui->ln_terr_name_rp->text();
        data_app.filial_director = ui->ln_filial_director->text();
        data_app.filial_director_2 = ui->ln_filial_director_2->text();
        data_app.filial_director_fio = ui->ln_filial_director_fio->text();
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
            settings.setValue("filial_director_fio", data_app.filial_director_fio.toUtf8());
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
        ui->lab_filial_director_fio->setEnabled(false);
        ui->ln_filial_director_fio->setEnabled(false);

        ui->bn_print->setEnabled(true);
    }
}

#include "qt_windows.h"
#include "qwindowdefs_win.h"
#include <shellapi.h>

void print_assig_sexage_all_mo_wnd::on_bn_print_clicked() {

    // распаковка шаблона реестра
    QString fname_template = data_app.path_reports + "rep_assig_sexage_tab.ods";
    QString temp_folder = data_app.path_temp + "rep_assig_sexage_tab";
    if (!QFile::exists(fname_template)) {
        QMessageBox::warning(this,
                             "Шаблон не найден",
                             "Не найден шаблон печатной формы половозрастной таблицы прикреплённых к МО: \n" + fname_template +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    QDir tmp_dir(temp_folder);
    if (tmp_dir.exists(temp_folder) && !tmp_dir.removeRecursively()) {
        QMessageBox::warning(this,
                             "Ошибка при очистке папки",
                             "Не удалось очистить папку временных файлов: \n" + temp_folder +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
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
        return;
    }
    com = "\"" + data_app.path_arch + "7z.exe\" e \"" + fname_template + "\" -o\"" + temp_folder + "\" styles.xml";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка распаковки шаблона",
                             "Не удалось распаковать файл стилей шаблона \n" + fname_template + " -> styles.xml"
                             "\nпечатной формы во временную папку\n" + temp_folder +
                             "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
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
                             "Файл контента шаблона пустой. Возможно шаблон был испорчен.\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
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
    QDate date_05 = date_assig.addYears( -5);
    QDate date_18 = date_assig.addYears(-18);
    QDate date_55 = date_assig.addYears(-55);
    QDate date_60 = date_assig.addYears(-60);
    QString date_str = date_assig.toString("yy-MM-yyyy");

    // получим список персон на заданную дату
    QString sql_dstrs;
    //sql_dstrs = "select m.id, m.code_mo, m.text from assig_medorgs m  order by m.id asc ; ";

    sql_dstrs += "select distinct m.id, m.code_mo, m.name_mo ";
    sql_dstrs += " from persons e ";
    sql_dstrs += "      left join polises p on ( p.id_person=e.id and p.date_begin<='" + date_str + "' and (p.date_end is NULL or p.date_end>='" + date_str + "') and (p.date_stop is NULL or p.date_stop>='" + date_str + "') ) ";
    sql_dstrs += "      left join persons_assig asg on ( asg.id_person=e.id and asg.is_actual=0 ) ";
    sql_dstrs += "      left join spr_medorgs m on ( m.code_mo=asg.code_mo and m.is_head=1 ) ";
    sql_dstrs += " where (e.date_death is NULL or e.date_death>'" + date_str + "') and "
                 "       m.code_mo is not NULL and p.id is not NULL and "
                 "       p.date_begin<='" + date_str + "' and  "
                 "       (p.date_stop is NULL or p.date_stop>'" + date_str + "')  ; ";

    QSqlQuery *query_dstrs = new QSqlQuery(db);
    bool res_dstr = mySQL.exec(this, sql_dstrs, "Запросим список медорганизаций, к которым может прикрепляться население", *query_dstrs, true, db, data_app);
    if (!res_dstr || query_dstrs->size()<=0) {
        QMessageBox::warning(this, "Список медорганизаций не получен", "При выборе полного списка медорганизаций произошла неожиданная ошибка.");
        delete query_dstrs;
        return;
    };

    // выберем список персон
    QString sql_pers = "select id from persons p ; ";
    QSqlQuery *query_pers = new QSqlQuery(db);
    bool res_pers = mySQL.exec(this, sql_pers, QString("Выбор полного списка застрахованных"), *query_pers, true, db, data_app);
    if (!res_pers || query_pers->size()<=0) {
        QMessageBox::warning(this, "Список персон не получен", "При выборе полного списка застрахованных произошла неожиданная ошибка.");
        delete query_pers;
        return;
    }



    // суммирование по колонкам
    int sm_4 =   0;
    int sw_4 =   0;
    int sm517 =  0;
    int sw517 =  0;
    int sm1859 = 0;
    int sw1854 = 0;
    int sm60_ =  0;
    int sw55_ =  0;
    int ssum =   0;

    int n = 0;

    // переберём медорганизации
    while (query_dstrs->next()) {

        int i_mo = query_dstrs->value(0).toInt();
        QString code_mo = query_dstrs->value(1).toString();
        QString text = query_dstrs->value(2).toString();

        int m_4 =   0;
        int w_4 =   0;
        int m517 =  0;
        int w517 =  0;
        int m1859 = 0;
        int w1854 = 0;
        int m60_ =  0;
        int w55_ =  0;
        int sum =   0;

        // переберём персоны
        query_pers->first();
        int m = 0;
        while (query_pers->next()) {
            if (m==0) {
                query_pers->first();
            }
            m++;

            QString id_person = QString::number(query_pers->value(0).toInt());

            // выберем данные этого человека, как прикреплённого к данной МО
            QString sql = "";
            sql += "select e.id, "
                         " e.sex,"
                         " e.date_birth ";
            sql += " from  persons e ";
            //             " left join polises p on ( p.id_person=e.id and p.date_begin<='#DATE_REP#' and (p.date_end is NULL or p.date_end>='#DATE_REP#') and (p.date_stop is NULL or p.date_stop>='#DATE_REP#') ) "
            //             " left join persons_assig asg on ( asg.id_person=e.id and asg.is_actual=0 ) "
            //             " left join spr_medorgs m on ( m.code_mo=asg.code_mo and m.is_head=1 ) ";
            sql += " where "
            //             " asg.code_mo='" + code_mo + "' and "
                         " e.id=" + id_person; //+ " and "
            //             " p.id is not NULL and "
            //             " p.date_begin<='#DATE_REP#' and "
            //             " (p.date_stop is NULL or p.date_stop>'#DATE_REP#') ; ";

            sql_pers = sql_pers.replace("#DATE_REP#", date_str);

            QSqlQuery *query = new QSqlQuery(db);
            bool res = mySQL.exec(this, sql, "Запросим персону, прикреплённую к данной МО", *query, true, db, data_app);

            if (res && query->size()>0) {
                query->next();
                // получим данне застрахованного
                int sex = query->value(1).toInt();
                QDate date_birth = query->value(2).toDate();

                // добавим запись к счётчикам
                if (sex==1) {
                    if (date_birth>date_05) {
                        m_4++;
                        sm_4++;
                    } else if (date_birth>date_18) {
                        m517++;
                        sm517++;
                    } else if (date_birth>date_60) {
                        m1859++;
                        sm1859++;
                    } else {
                        m60_++;
                        sm60_++;
                    }
                } else {
                    if (date_birth>date_05) {
                        w_4++;
                        sw_4++;
                    } else if (date_birth>date_18) {
                        w517++;
                        sw517++;
                    } else if (date_birth>date_55) {
                        w1854++;
                        sw1854++;
                    } else {
                        w55_++;
                        sw55_++;
                    }
                }
                sum++;
                ssum++;
            }
        };

        n++;
        QString s_row_new = s_row;
        s_row_new = s_row_new.
                replace("#N#",    QString::number(n)).
                replace("#TEXT#", text).
                replace("#MO#",   code_mo).
                replace("#M_4#",  (m_4>0  ? QString::number(m_4)  : " ")).
                replace("#W_4#",  (w_4>0  ? QString::number(w_4)  : " ")).
                replace("#M517#", (m517>0 ? QString::number(m517) : " ")).
                replace("#W517#", (w517>0 ? QString::number(w517) : " ")).
                replace("#M1859#",(m1859>0? QString::number(m1859): " ")).
                replace("#W1854#",(w1854>0? QString::number(w1854): " ")).
                replace("#M60_#", (m60_>0 ? QString::number(m60_) : " ")).
                replace("#W55_#", (w55_>0 ? QString::number(w55_) : " ")).
                replace("#SUM#",  QString::number(sum));
        s_content_new += s_row_new;
    }
    s_content_new += s_content.right(s_content.size()-pos1);

    s_content_new = s_content_new.
            replace("#SM_4#",  QString::number(sm_4)).
            replace("#SW_4#",  QString::number(sw_4)).
            replace("#SM517#", QString::number(sm517)).
            replace("#SW517#", QString::number(sw517)).
            replace("#SM1859#",QString::number(sm1859)).
            replace("#SW1854#",QString::number(sw1854)).
            replace("#SM60_#", QString::number(sm60_)).
            replace("#SW55_#", QString::number(sw55_)).
            replace("#SSUM#",  QString::number(ssum)).

            replace("#SMO_NAME#", data_app.smo_name).
            replace("#DATE_REP#", date_assig.toString("dd.MM.yyyy")).
            replace("#TERR_NAME_RP#", data_app.terr_name_rp).

            replace("#FILIAL_DIRECTOR#",data_app.filial_director).
            replace("#FILIAL_DIRECTOR_2#",data_app.filial_director_2).
            replace("#FILIAL_DIRECTOR_FIO#",data_app.filial_director_fio);

    delete query_dstrs;

    // сохранение контента шаблона
    QFile f_content_save(temp_folder + "/content.xml");
    f_content_save.open(QIODevice::WriteOnly);
    f_content_save.write(s_content_new.toUtf8());
    f_content_save.close();


    // ===========================
    // архивация шаблона
    // ===========================
    QString fname_res = data_app.path_out + "_REPORTS_/" +
            QDate::currentDate().toString("yyyy-MM-dd") + " - assig_sexage_tab - " + date_assig.toString("yyyy-MM-dd") + ".ods";

    QFile::remove(fname_res);
    while (QFile::exists(fname_res)) {
        if (QMessageBox::warning(this,
                                 "Ошибка сохранения журнала ВС",
                                 "Не удалось сохранить форму половозрастной таблицы прикреплённых к МО: \n" + fname_template +
                                 "\n\nво временную папку\n" + fname_res +
                                 "\n\nПроверьте, не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                 QMessageBox::Retry|QMessageBox::Abort,
                                 QMessageBox::Retry)==QMessageBox::Abort) {
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        QFile::remove(fname_res);
    }
    if (!QFile::copy(fname_template, fname_res) || !QFile::exists(fname_res)) {
        QMessageBox::warning(this, "Ошибка копирования отчёта", "Не удалось скопировать форму половозрастной таблицы прикреплённых к МО: \n" + fname_template + "\n\nво временную папку\n" + fname_res + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    com = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname_res + "\" \"" + temp_folder + "/content.xml\"";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка обновления контента",
                             "При добавлении нового контента в форму половозрастной таблицы прикреплённых к МО произошла непредвиденная ошибка\n\nОпреация прервана.");
    }

    // ===========================
    // собственно открытие шаблона
    // ===========================
    // открытие полученного ODT
    long result = (long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);

    //close();
}

void print_assig_sexage_all_mo_wnd::on_ln_smo_name_textChanged(const QString &arg1) {
    data_app.smo_name = ui->ln_smo_name->text();
}

void print_assig_sexage_all_mo_wnd::on_ln_filial_director_textChanged(const QString &arg1) {
    data_app.filial_director = ui->ln_filial_director->text();
}

void print_assig_sexage_all_mo_wnd::on_ln_filial_director_2_textChanged(const QString &arg1) {
    data_app.filial_director_2 = ui->ln_filial_director_2->text();
}

void print_assig_sexage_all_mo_wnd::on_ln_filial_director_fio_textChanged(const QString &arg1) {
    data_app.filial_director_fio = ui->ln_filial_director_fio->text();
}

void print_assig_sexage_all_mo_wnd::on_ln_terr_name_textChanged(const QString &arg1) {
    data_app.terr_name = ui->ln_terr_name->text();
}

void print_assig_sexage_all_mo_wnd::on_ln_terr_name_rp_textChanged(const QString &arg1) {
    data_app.terr_name_rp = ui->ln_terr_name_rp->text();
}
