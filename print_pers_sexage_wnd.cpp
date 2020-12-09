#include "print_pers_sexage_wnd.h"
#include "ui_print_pers_sexage_wnd.h"

print_pers_sexage_wnd::print_pers_sexage_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    db(db),
    data_app(data_app),
    settings(settings),
    QDialog(parent),
    ui(new Ui::print_pers_sexage_wnd)
{
    ui->setupUi(this);

    on_bn_date_rep_clicked();
    ui->ln_smo_name->setText(data_app.smo_name);
    ui->ln_terr_name->setText(data_app.terr_name);
    ui->ln_terr_name_rp->setText(data_app.terr_name_rp);
    ui->ln_tfoms_director->setText(data_app.tfoms_director);
    ui->ln_tfoms_director_2->setText(data_app.tfoms_director_2);
    ui->ln_tfoms_chif->setText(data_app.tfoms_chif);
    ui->ln_filial_director->setText(data_app.filial_director);
    ui->ln_filial_director_2->setText(data_app.filial_director_2);
    ui->ln_filial_chif->setText(data_app.filial_chif);
}

print_pers_sexage_wnd::~print_pers_sexage_wnd() {
    delete ui;
}

void print_pers_sexage_wnd::on_bn_cansel_clicked() {
    reject();
}

void print_pers_sexage_wnd::on_bn_date_rep_clicked() {
    ui->date_rep->setDate(QDate::currentDate());
}

QString print_pers_sexage_wnd::month_to_str(QDate date) {
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

void print_pers_sexage_wnd::on_bn_edit_clicked(bool checked) {
    if (checked) {
        ui->bn_edit->setText("Сохранить");

        ui->lab_smo_name->setEnabled(true);
        ui->ln_smo_name->setEnabled(true);
        ui->lab_terr_name->setEnabled(true);
        ui->ln_terr_name->setEnabled(true);
        ui->lab_terr_name_rp->setEnabled(true);
        ui->ln_terr_name_rp->setEnabled(true);
        ui->lab_filial_director->setEnabled(true);
        ui->lab_filial_chif->setEnabled(true);
        ui->lab_tfoms_director->setEnabled(true);
        ui->lab_tfoms_chif->setEnabled(true);
        ui->ln_filial_director->setEnabled(true);
        ui->ln_filial_director_2->setEnabled(true);
        ui->ln_filial_chif->setEnabled(true);
        ui->ln_tfoms_director->setEnabled(true);
        ui->ln_tfoms_director_2->setEnabled(true);
        ui->ln_tfoms_chif->setEnabled(true);

        ui->bn_print->setEnabled(false);
    } else {
        data_app.smo_name = ui->ln_smo_name->text();
        data_app.terr_name = ui->ln_terr_name->text();
        data_app.terr_name_rp = ui->ln_terr_name_rp->text();
        data_app.tfoms_director = ui->ln_tfoms_director->text();
        data_app.tfoms_director_2 = ui->ln_tfoms_director_2->text();
        data_app.tfoms_chif = ui->ln_tfoms_chif->text();
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
            settings.setValue("terr_name_rp", data_app.terr_name_rp.toUtf8());
            settings.setValue("tfoms_director", data_app.tfoms_director.toUtf8());
            settings.setValue("tfoms_director_2", data_app.tfoms_director_2.toUtf8());
            settings.setValue("tfoms_chif", data_app.tfoms_chif.toUtf8());
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
        ui->lab_filial_chif->setEnabled(false);
        ui->lab_tfoms_director->setEnabled(false);
        ui->lab_tfoms_chif->setEnabled(false);
        ui->ln_filial_director->setEnabled(false);
        ui->ln_filial_director_2->setEnabled(false);
        ui->ln_filial_chif->setEnabled(false);
        ui->ln_tfoms_director->setEnabled(false);
        ui->ln_tfoms_director_2->setEnabled(false);
        ui->ln_tfoms_chif->setEnabled(false);

        ui->bn_print->setEnabled(true);
    }
}

#include "qt_windows.h"
#include "qwindowdefs_win.h"
#include <shellapi.h>


void print_pers_sexage_wnd::on_bn_print_clicked() {
    ui->bn_print->setEnabled(false);
    this->setCursor(Qt::WaitCursor);

    // распаковка шаблона
    QString rep_folder = data_app.path_reports;
    // распаковка шаблона
    QString tmp_folder = data_app.path_out + "_REPORTS_/temp/";
    QString rep_template = rep_folder + "rep_pers_sexage_tab.ods";

    if (!QFile::exists(rep_template)) {
        QMessageBox::warning(this,
                             "Шаблон не найден",
                             "Не найден шаблон печатной формы половозрастной таблицы застрахованных: \n" + rep_template +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    QDir tmp_dir(tmp_folder);
    if (tmp_dir.exists(tmp_folder) && !tmp_dir.removeRecursively()) {
        QMessageBox::warning(this,
                             "Ошибка при очистке папки",
                             "Не удалось очистить папку временных файлов: \n" + tmp_folder +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    tmp_dir.mkpath(tmp_folder);
    QProcess myProcess;
    QString com = "\"" + data_app.path_arch + "7z.exe\" e \"" + rep_template + "\" -o\"" + tmp_folder + "\" content.xml";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка распаковки шаблона",
                             "Не удалось распаковать файл контента шаблона\n" + rep_template + " -> content.xml"
                             "\nпечатной формы во временную папку\n" + tmp_folder +
                             "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    com = "\"" + data_app.path_arch + "7z.exe\" e \"" + rep_template + "\" -o\"" + tmp_folder + "\" styles.xml";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка распаковки шаблона",
                             "Не удалось распаковать файл стилей шаблона \n" + rep_template + " -> styles.xml"
                             "\nпечатной формы во временную папку\n" + tmp_folder +
                             "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }

    // ===========================
    // правка контента шаблона
    // ===========================
    // открытие контента шаблона
    QFile f_content(tmp_folder + "/content.xml");
    f_content.open(QIODevice::ReadOnly);
    QString s_content = f_content.readAll();
    f_content.close();
    if (s_content.isEmpty()) {
        QMessageBox::warning(this,
                             "Файл контента шаблона пустой",
                             "Файл контента шаблона пустой. Возможно шаблон был испорчен.\n\nОперация прервана.");
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
    QDate date_rep= ui->date_rep->date();
    QDate date_01 = date_rep.addYears( -1);
    QDate date_05 = date_rep.addYears( -5);
    QDate date_18 = date_rep.addYears(-18);
    QDate date_55 = date_rep.addYears(-55);
    QDate date_60 = date_rep.addYears(-60);
    QString date_str = date_rep.toString("yy-MM-yyyy");

    /*// Обновление статусов застрахованных
    QString sql = "select * from update_pers_links('" + date_rep.toString("yyyy-MM-dd") + "') ; ";
    QSqlQuery *query = new QSqlQuery(db);
    if (!mySQL.exec(this, sql, "Обновление статусов застрахованных", *query, true, db, data_app))
        QMessageBox::warning(this, "Операция отменена", "При обновлении статусов застрахованных произошла непредвиденная ошибка.\n\nОперация пропущена.");
    delete query;*/

    // получим список районов
    QString sql_dstrs;
    sql_dstrs += "select s.id, s.name, s.kladr from address_struct s where s.level=2 order by s.id asc ; ";

    sql_dstrs = sql_dstrs.replace("#DATE_REP#", date_rep.toString("yyyy-MM-dd"));

    QSqlQuery *query_dstrs = new QSqlQuery(db);
    if (!mySQL.exec(this, sql_dstrs, "Запросим список районов области", *query_dstrs, true, db, data_app)) {
        delete query_dstrs;
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    };
    int n_dstrs = query_dstrs->size();
    ui->lab_all->setText(QString::number(n_dstrs));
    ui->lab_cnt->setText("0");



    // суммирование по колонкам
    int sm_0 =   0;
    int sw_0 =   0;
    int sm14 =   0;
    int sw14 =   0;
    int sm517 =  0;
    int sw517 =  0;
    int sm1859 = 0;
    int sw1854 = 0;
    int sm60_ =  0;
    int sw55_ =  0;
    int ssum =   0;

    int n = 0;

    // переберём районы области
    while (query_dstrs->next()) {

        int i_dstr = query_dstrs->value(0).toInt();
        QString name = query_dstrs->value(1).toString();
        QString kladr = query_dstrs->value(2).toString();

        int m_0 =   0;
        int w_0 =   0;
        int m14 =   0;
        int w14 =   0;
        int m517 =  0;
        int w517 =  0;
        int m1859 = 0;
        int w1854 = 0;
        int m60_ =  0;
        int w55_ =  0;
        int sum =   0;

        // выберем данные прикреплённых к данному району
        QString sql;
        sql += "select  e.id, e.fam, e.im, e.ot, e.sex, e.date_birth, s.kladr "
               " from persons e ";

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

        sql += "    left join addresses ar on(ar.id=e.id_addr_reg) "
               "    left join addresses al on(al.id=e.id_addr_liv) "
               "    left join address_struct s "
               "           on ( (ar.kladr is not NULL) and "
               "                (TRIM(ar.kladr)<>'') and "
               "                (s.level>1) and "
               "                ( ( ( ( substring ( ar.kladr from 1 for 11 ) || '00' ) = s.kladr ) or "
               "                    ( ( substring ( ar.kladr from 1 for  8 ) || '00000' ) = s.kladr ) or "
               "                    ( ( substring ( ar.kladr from 1 for  5 ) || '00000000' ) = s.kladr ) ) "
               "                  or "
               "                  ( ar.kladr is NULL and al.kladr is NULL ) "
               "              ) ) "
               " where (e.date_death is NULL or e.date_death>'#DATE_REP#') "
               "   and (select * from st_person(e.id,'#DATE_REP#')) in(1,2,3,4,11,12,13,14) "
               //"   and p.date_begin<='#DATE_REP#' ";
               //"   and e._date_insure_begin<='#DATE_REP#' "
               //"   and ((e._date_insure_end is NULL) or (e._date_insure_end>'#DATE_REP#')) "
               "   and s.kladr='" + kladr + "' ; ";

        if (ui->ch_tfoms->isChecked()) {
            sql += "   and p.tfoms_date is not NULL ";
        }

        sql = sql.replace("#DATE_REP#", date_rep.toString("yyyy-MM-dd"));




        QSqlQuery *query = new QSqlQuery(db);
        bool res = mySQL.exec(this, sql, "Запросим застрахованных жителей района", *query, true, db, data_app);

        while (query->next()) {
            // получим данные застрахованного
            int sex = query->value(4).toInt();
            QDate date_birth = query->value(5).toDate();

            // добавим запись к счётчикам
            if (sex==1) {
                if (date_birth>date_01) {
                    m_0++;
                    sm_0++;
                } else if (date_birth>date_05) {
                    m14++;
                    sm14++;
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
                if (date_birth>date_01) {
                    w_0++;
                    sw_0++;
                } else if (date_birth>date_05) {
                    w14++;
                    sw14++;
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
        delete query;

        n++;
        QString s_row_new = s_row;
        s_row_new = s_row_new.
                replace("#N#",    QString::number(n)).
                replace("#NAME#", name).
                replace("#M_0#",  (m_0>0  ? QString::number(m_0)  : " ")).
                replace("#W_0#",  (w_0>0  ? QString::number(w_0)  : " ")).
                replace("#M14#",  (m14>0  ? QString::number(m14)  : " ")).
                replace("#W14#",  (w14>0  ? QString::number(w14)  : " ")).
                replace("#M517#", (m517>0 ? QString::number(m517) : " ")).
                replace("#W517#", (w517>0 ? QString::number(w517) : " ")).
                replace("#M1859#",(m1859>0? QString::number(m1859): " ")).
                replace("#W1854#",(w1854>0? QString::number(w1854): " ")).
                replace("#M60_#", (m60_>0 ? QString::number(m60_) : " ")).
                replace("#W55_#", (w55_>0 ? QString::number(w55_) : " ")).
                replace("#SUM#",  QString::number(sum));
        s_content_new += s_row_new;

        ui->lab_cnt->setText(QString::number(n));
        QApplication::processEvents();
    }

    delete query_dstrs;

    // строка "другие регионы"
    QString name = "другие регионы";

    int m_0 =   0;
    int w_0 =   0;
    int m14 =   0;
    int w14 =   0;
    int m517 =  0;
    int w517 =  0;
    int m1859 = 0;
    int w1854 = 0;
    int m60_ =  0;
    int w55_ =  0;
    int sum =   0;

    // выберем данные жителей других регионов
    QString sql = "select  e.id, e.fam, e.im, e.ot, e.sex, e.date_birth, s.kladr "
           " from persons e ";

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

    sql += "    left join addresses ar on(ar.id=e.id_addr_reg) "
           "    left join addresses al on(al.id=e.id_addr_liv) "
           "    left join address_struct s "
           "           on ( (ar.kladr is not NULL) and "
           "                (TRIM(ar.kladr)<>'') and "
           "                (s.level>1) and "
           "                ( ( ( ( substring ( ar.kladr from 1 for 11 ) || '00' ) = s.kladr ) or "
           "                    ( ( substring ( ar.kladr from 1 for  8 ) || '00000' ) = s.kladr ) or "
           "                    ( ( substring ( ar.kladr from 1 for  5 ) || '00000000' ) = s.kladr ) ) "
           "                  /*or "
           "                  ( ( ( substring ( al.kladr from 1 for 11 ) || '00' ) = s.kladr ) or "
           "                    ( ( substring ( al.kladr from 1 for  8 ) || '00000' ) = s.kladr ) or "
           "                    ( ( substring ( al.kladr from 1 for  5 ) || '00000000' ) = s.kladr ) )*/ "
           "                  or "
           "                  ( ar.kladr is NULL and al.kladr is NULL ) "
           "              ) ) "
           " where (e.date_death is NULL or e.date_death>'#DATE_REP#') "
           "   and p.date_begin<='#DATE_REP#' ";
    if (ui->ch_status->isChecked())
        sql+= "and (select * from st_person(e.id,'#DATE_REP#')) in(1,2,3,4,11,12,13,14) ";
    sql += "   and s.kladr is NULL ; ";

    sql = sql.replace("#DATE_REP#", date_rep.toString("yyyy-MM-dd"));



    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, "Запросим застрахованных жителей другого региона", *query, true, db, data_app);

    while (query->next() ) {
        // получим данные застрахованного
        int sex = query->value(4).toInt();
        QDate date_birth = query->value(5).toDate();

        // добавим запись к счётчикам
        if (sex==1) {
            if (date_birth>date_01) {
                m_0++;
                sm_0++;
            } else if (date_birth>date_05) {
                m14++;
                sm14++;
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
            if (date_birth>date_01) {
                w_0++;
                sw_0++;
            } else if (date_birth>date_05) {
                w14++;
                sw14++;
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

    n++;
    QString s_row_new = s_row;
    s_row_new = s_row_new.
            replace("#N#",    QString::number(n)).
            replace("#NAME#", name).
            replace("#M_0#",  (m_0>0  ? QString::number(m_0)  : " ")).
            replace("#W_0#",  (w_0>0  ? QString::number(w_0)  : " ")).
            replace("#M14#",  (m14>0  ? QString::number(m14)  : " ")).
            replace("#W14#",  (w14>0  ? QString::number(w14)  : " ")).
            replace("#M517#", (m517>0 ? QString::number(m517) : " ")).
            replace("#W517#", (w517>0 ? QString::number(w517) : " ")).
            replace("#M1859#",(m1859>0? QString::number(m1859): " ")).
            replace("#W1854#",(w1854>0? QString::number(w1854): " ")).
            replace("#M60_#", (m60_>0 ? QString::number(m60_) : " ")).
            replace("#W55_#", (w55_>0 ? QString::number(w55_) : " ")).
            replace("#SUM#",  QString::number(sum));
    if (sum>0) {
        s_content_new += s_row_new;
    }

    s_content_new += s_content.right(s_content.size()-pos1);

    s_content_new = s_content_new.
            replace("#SM_0#",  QString::number(sm_0)).
            replace("#SW_0#",  QString::number(sw_0)).
            replace("#SM14#",  QString::number(sm14)).
            replace("#SW14#",  QString::number(sw14)).
            replace("#SM517#", QString::number(sm517)).
            replace("#SW517#", QString::number(sw517)).
            replace("#SM1859#",QString::number(sm1859)).
            replace("#SW1854#",QString::number(sw1854)).
            replace("#SM60_#", QString::number(sm60_)).
            replace("#SW55_#", QString::number(sw55_)).
            replace("#SSUM#",  QString::number(ssum)).

            replace("#SMO_NAME#", data_app.smo_name).
            replace("#DATE_REP#", date_rep.toString("dd.MM.yyyy")).
            replace("#TERR_NAME_RP#",data_app.terr_name_rp).

            replace("#TFOMS_DIRECTOR#",data_app.tfoms_director).
            replace("#TFOMS_DIRECTOR_2#",data_app.tfoms_director_2).
            replace("#TFOMS_DIRECTOR_FIO#",data_app.tfoms_chif).
            replace("#FILIAL_DIRECTOR#",data_app.filial_director).
            replace("#FILIAL_DIRECTOR_2#",data_app.filial_director_2).
            replace("#FILIAL_DIRECTOR_FIO#",data_app.filial_chif);

    delete query_dstrs;

    // сохранение контента шаблона
    QFile f_content_save(tmp_folder + "/content.xml");
    f_content_save.open(QIODevice::WriteOnly);
    f_content_save.write(s_content_new.toUtf8());
    f_content_save.close();


    // ===========================
    // архивация шаблона
    // ===========================
    QString fname_res = data_app.path_out + "_REPORTS_/" +
            QDate::currentDate().toString("yyyy-MM-dd") + "  -  " + data_app.smo_short + " - " + data_app.filial_name + "  -  pers_sexage_tab - " + date_rep.toString("yyyy-MM-dd") + ".ods";

    QFile::remove(fname_res);
    while (QFile::exists(fname_res)) {
        if (QMessageBox::warning(this,
                                 "Ошибка сохранения журнала ВС",
                                 "Не удалось сохранить форму половозрастной таблицы застрахованных: \n" + rep_template +
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
    if (!QFile::copy(rep_template, fname_res) || !QFile::exists(fname_res)) {
        QMessageBox::warning(this, "Ошибка копирования отчёта", "Не удалось скопировать форму половозрастной таблицы застрахованных: \n" + rep_template + "\n\nво временную папку\n" + fname_res + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    com = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname_res + "\" \"" + tmp_folder + "/content.xml\"";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка обновления контента",
                             "При добавлении нового контента в форму половозрастной таблицы застрахованных произошла непредвиденная ошибка\n\nОпреация прервана.");
    }

    // ===========================
    // собственно открытие шаблона
    // ===========================
    // открытие полученного ODT
    long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);

    ui->bn_print->setEnabled(true);
    QMessageBox::information(this, "Готово", "Готово");
    //close();
    this->setCursor(Qt::ArrowCursor);
    ui->bn_print->setEnabled(true);
}

void print_pers_sexage_wnd::on_ln_smo_name_textChanged(const QString &arg1) {
    data_app.smo_name = ui->ln_smo_name->text();
}

void print_pers_sexage_wnd::on_ln_tfoms_director_textChanged(const QString &arg1) {
    data_app.tfoms_director = ui->ln_tfoms_director->text();
}

void print_pers_sexage_wnd::on_ln_tfoms_director_2_textChanged(const QString &arg1) {
    data_app.tfoms_director_2 = ui->ln_tfoms_director_2->text();
}

void print_pers_sexage_wnd::on_ln_tfoms_chif_textChanged(const QString &arg1) {
    data_app.tfoms_chif = ui->ln_tfoms_chif->text();
}

void print_pers_sexage_wnd::on_ln_filial_director_textChanged(const QString &arg1) {
    data_app.filial_director = ui->ln_filial_director->text();
}

void print_pers_sexage_wnd::on_ln_filial_director_2_textChanged(const QString &arg1) {
    data_app.filial_director_2 = ui->ln_filial_director_2->text();
}

void print_pers_sexage_wnd::on_ln_filial_chif_textChanged(const QString &arg1) {
    data_app.filial_chif = ui->ln_filial_chif->text();
}

void print_pers_sexage_wnd::on_bn_date_cur_clicked() {
    ui->date_rep->setDate(QDate::currentDate().addDays(-1));
}
