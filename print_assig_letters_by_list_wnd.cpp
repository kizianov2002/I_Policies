#include "print_assig_letters_by_list_wnd.h"
#include "ui_print_assig_letters_by_list_wnd.h"

int mod(int n) {
    if (n<0)
        return -1 * n;
    else return n;
}

print_assig_letters_by_list_wnd::print_assig_letters_by_list_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    db(db),
    data_app(data_app),
    settings(settings),
    QDialog(parent),
    ui(new Ui::print_assig_letters_by_list_wnd)
{
    ui->setupUi(this);

    refresh_medorgs();

    on_bn_date_assig_clicked();
    on_bn_date_cur_clicked();

    ui->ln_filial_director->setText(data_app.filial_director);
    ui->ln_filial_director_2->setText(data_app.filial_director_2);
    ui->ln_filial_chif->setText(data_app.filial_chif);
    ui->ln_rep_doer->setText(data_app.rep_doer);
    ui->ln_filial_phone->setText(data_app.filial_phone);
    ui->ln_filial_phone_2->setText(data_app.filial_phone_2);
    ui->ln_filial_email->setText(data_app.filial_email);
    ui->ln_filial_email_2->setText(data_app.filial_email_2);
}

print_assig_letters_by_list_wnd::~print_assig_letters_by_list_wnd() {
    delete ui;
}

void print_assig_letters_by_list_wnd::on_bn_cansel_clicked() {
    reject();
}

void print_assig_letters_by_list_wnd::on_bn_date_assig_clicked() {
    ui->date_assig->setDate(QDate(QDate::currentDate().year(), QDate::currentDate().month(), 1));
}

void print_assig_letters_by_list_wnd::on_bn_date_cur_clicked() {
    ui->date_cur->setDate(QDate::currentDate().addDays(-1));
}

void print_assig_letters_by_list_wnd::refresh_medorgs() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка подразделений
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code_mo, '(' || code_mo || ') ' || name_mo "
                  "  from public.spr_medorgs m "
                  " order by code_mo ; ";
    mySQL.exec(this, sql, QString("Список медорганизаций"), *query, true, db, data_app);
    ui->combo_1lpu->clear();
    while (query->next()) {
        ui->combo_1lpu->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_1lpu->setCurrentIndex(-1);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

QString print_assig_letters_by_list_wnd::month_to_str(QDate date) {
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

void print_assig_letters_by_list_wnd::on_bn_edit_clicked(bool checked) {
    if (checked) {
        ui->bn_edit->setText("Сохранить");

        ui->lab_filial_director->setEnabled(true);
        ui->ln_filial_director->setEnabled(true);
        ui->lab_filial_chif->setEnabled(true);
        ui->ln_filial_director_2->setEnabled(true);
        ui->ln_filial_chif->setEnabled(true);
        ui->lab_filial_phones->setEnabled(true);
        ui->ln_rep_doer->setEnabled(true);
        ui->lab_rep_doer->setEnabled(true);
        ui->lab_filial_phones->setEnabled(true);
        ui->ln_filial_phone->setEnabled(true);
        ui->ln_filial_phone_2->setEnabled(true);
        ui->lab_filial_email->setEnabled(true);
        ui->ln_filial_email->setEnabled(true);
        ui->ln_filial_email_2->setEnabled(true);

        ui->bn_print->setEnabled(false);
    } else {
        data_app.filial_director = ui->ln_filial_director->text();
        data_app.filial_director_2 = ui->ln_filial_director_2->text();
        data_app.filial_chif = ui->ln_filial_chif->text();
        data_app.rep_doer = ui->ln_rep_doer->text();
        data_app.filial_phone = ui->ln_filial_phone->text();
        data_app.filial_phone_2 = ui->ln_filial_phone_2->text();
        data_app.filial_email = ui->ln_filial_email->text();
        data_app.filial_email_2 = ui->ln_filial_email_2->text();
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
            settings.setValue("rep_doer", data_app.rep_doer.toUtf8());
            settings.setValue("filial_phone", data_app.filial_phone.toUtf8());
            settings.setValue("filial_phone_2", data_app.filial_phone_2.toUtf8());
            settings.setValue("filial_email", data_app.filial_email.toUtf8());
            settings.setValue("filial_email_2", data_app.filial_email_2.toUtf8());
            settings.sync();
        }
        ui->bn_edit->setText("Изменить");

        ui->lab_rep_doer->setEnabled(false);
        ui->ln_rep_doer->setEnabled(false);
        ui->lab_filial_director->setEnabled(false);
        ui->ln_filial_director->setEnabled(false);
        ui->ln_filial_director_2->setEnabled(false);
        ui->lab_filial_chif->setEnabled(false);
        ui->ln_filial_chif->setEnabled(false);
        ui->lab_rep_doer->setEnabled(false);
        ui->ln_rep_doer->setEnabled(false);
        ui->lab_filial_phones->setEnabled(false);
        ui->ln_filial_phone->setEnabled(false);
        ui->ln_filial_phone_2->setEnabled(false);
        ui->lab_filial_email->setEnabled(false);
        ui->ln_filial_email->setEnabled(false);
        ui->ln_filial_email_2->setEnabled(false);

        ui->bn_print->setEnabled(true);
    }
}

#include "qt_windows.h"
#include "qwindowdefs_win.h"
#include <shellapi.h>

void print_assig_letters_by_list_wnd::on_bn_print_clicked() {
    ui->bn_print->setEnabled(false);
    this->setCursor(Qt::WaitCursor);

    QString fname_tmp;
    QString fname_res;

    // пороговые даты
    QDate date_assig = ui->date_assig->date();
    QDate date_cur = ui->date_cur->date();
    QDate date_01 = date_assig.addYears( -1);
    QDate date_05 = date_assig.addYears( -5);
    QDate date_18 = date_assig.addYears(-18);
    QDate date_55 = date_assig.addYears(-55);
    QDate date_60 = date_assig.addYears(-60);
    QString date_str = date_assig.toString("yyyy-MM-dd");

    // подготовим папку для сохранения
    QString folder_res = data_app.path_out + "_ASSIG_DATA_/" + date_str + "/";
    QDir dir_res(folder_res);
    if (!dir_res.exists())
        dir_res.mkpath(folder_res);

    // переберём ЛПУ в массиве
    QString sql_medorgs;
    sql_medorgs = "select m.id, m.code_mo, m.name_mo, m.mo_chif_state, m.mo_chif_famio, m.mo_chif_greetings, m.mo_contract_num, m.mo_contract_date, m.name_mo_full, m.mo_chif_state_dp, m.mo_chif_famio_dp "
                  "  from spr_medorgs m "
                  " where m.mo_assig>0 ";

    if (ui->ch_1lpu->isChecked()) sql_medorgs += "   and m.code_mo='" + ui->combo_1lpu->currentData().toString() + "' ";

    sql_medorgs += " order by m.id asc ; ";

    QSqlQuery *query_medorgs = new QSqlQuery(db);
    bool res_medorgs = mySQL.exec(this, sql_medorgs, "Запросим список медорганизаций, к которым может прикрепляться население", *query_medorgs, true, db, data_app);
    if (!res_medorgs || query_medorgs->size()<=0) {
        QMessageBox::warning(this, "Список медорганизаций не получен", "При выборе полного списка медорганизаций произошла неожиданная ошибка.");
        delete query_medorgs;
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    };
    int n_mo = query_medorgs->size();
    ui->lab_all->setText(QString::number(n_mo));
    ui->lab_cnt->setText("0");

    // Обновление статусов застрахованных
    /*QString sql = "select * from update_pers_links('" + date_assig.toString("yyyy-MM-dd") + "') ; ";
    QSqlQuery *query = new QSqlQuery(db);
    if (!mySQL.exec(this, sql, "Обновление статусов застрахованных", *query, true, db, data_app))
        QMessageBox::warning(this, "Операция отменена", "При обновлении статусов застрахованных произошла непредвиденная ошибка.\n\nОперация пропущена.");
    delete query;*/

    int n = 0;
    while (query_medorgs->next()) {
        n++;

        QString code_mo = query_medorgs->value(1).toString().trimmed();
        QString mo_name = query_medorgs->value(2).toString().trimmed();
        QString mo_chif_state = query_medorgs->value(3).toString().trimmed();
        QString mo_chif_famio = query_medorgs->value(4).toString().trimmed();
        QString mo_chif_greetings = query_medorgs->value(5).toString().trimmed();
        QString mo_contract_num = query_medorgs->value(6).toString().trimmed();
        QDate   mo_contract_date = query_medorgs->value(7).toDate();
        QString mo_name_full = query_medorgs->value(8).toString().trimmed();
        QString mo_chif_state_dp = query_medorgs->value(9).toString().trimmed();
        QString mo_chif_famio_dp = query_medorgs->value(10).toString().trimmed();

        // распаковка шаблона реестра
        QString fname_template = data_app.path_reports + "rep_assig_letter.odt";
        QString temp_folder = data_app.path_temp + "rep_assig_letter";
        if (!QFile::exists(fname_template)) {
            QMessageBox::warning(this,
                                 "Шаблон не найден",
                                 "Не найден шаблон печатной формы половозрастной таблицы прикреплённых к МО: \n" + fname_template +
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
                                 "Не удалось распаковать файл стилей шаблона \n" + fname_template + " -> styles.xml"
                                 "\nпечатной формы во временную папку\n" + temp_folder +
                                 "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
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
                                 "Файл контента шаблона пустой. Возможно шаблон был испорчен.\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            ui->bn_print->setEnabled(true);
            return;
        }

        // правка полей контента шаблона
        QString s_content_new = s_content;



        bool back_side = mod(date_assig.daysTo(QDate::currentDate()))>15;

        // выберем список персон
        QString sql_pers = "";
        sql_pers += "select e.id, e.sex, e.date_birth, e.fam, e.im, e.ot "
                    "  from persons e "
                    //"  left join link_persons_asg la on(la.id_person=e.id) "
                    //"  left join polises p on ( p.id=e.id_polis ) "
                    //"  left join persons_assig a on ( a.id=" + QString(/*back_side ? "(select * from id_person_asg(e.id,'#DATE_REP#')) " :*/ "la._id_asg") + " ) "
                    //"  left join (select aa.id_person, max(aa.id) as id from persons_assig aa where aa.assig_date<='" + date_assig.toString("yyyy-MM-dd") + "' and (aa.unsig_date is NULL or aa.unsig_date>'" + date_assig.toString("yyyy-MM-dd") + "') group by aa.id_person) qa on(qa.id_person=e.id) "
                    //"  left join persons_assig a on(a.id=qa.id) "
                    " where (select st_person(e.id, '#DATE_REP#')) in(1,2,3,4,11,12,13,14) ";

        sql_pers = sql_pers.replace("#DATE_REP#", date_assig.toString("yyyy-MM-dd"));

        QSqlQuery *query_pers = new QSqlQuery(db);
        bool res_pers = mySQL.exec(this, sql_pers, QString("Выбор полного списка застрахованных"), *query_pers, true, db, data_app);
        if (!res_pers /*|| query_pers->size()<=0*/) {
            QMessageBox::warning(this, "Список персон не получен", "При выборе полного списка застрахованных произошла неожиданная ошибка.");
            delete query_pers;
            this->setCursor(Qt::ArrowCursor);
            ui->bn_print->setEnabled(true);
            return;
        }

        //int n = 0;

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

        // переберём персоны
        query_pers->first();
        int m = 0;
        while (query_pers->next()) {
            if (m==0) {
                query_pers->first();
            }
            m++;

            // получим данные застрахованного
            QString id_person = QString::number(query_pers->value(0).toInt());
            int sex = query_pers->value(1).toInt();
            QDate date_birth = query_pers->value(2).toDate();

            // выберем данные этого человека, как прикреплённого к данной МО
                QString sql = "select count(*) "
                              "  from persons e "
                              //"  left join link_persons_asg la on(la.id_person=e.id) "

                              "  left join (select aa.id_person, max(aa.id) as id "
                              "               from persons_assig aa "
                              "              where aa.assig_date<='" + date_assig.toString("yyyy-MM-dd") + "' "
                              "                and ( aa.unsig_date is NULL "
                              "                   or aa.unsig_date>'" + date_assig.toString("yyyy-MM-dd") + "') "
                              "              group by aa.id_person) qa "
                              "       on(qa.id_person=e.id) "
                              "  left join persons_assig asg on(asg.id=qa.id) "

                              //"  left join persons_assig asg on ( asg.id=la._id_asg ) "
                              "  left join spr_medorgs m on ( m.code_mo=asg.code_mo and m.is_head=1 ) "
                              " where e.id=" + id_person + " "
                              "   and st_person(e.id,'" + date_assig.toString("yyyy-MM-dd") + "') in(11,12,13,14,1,2,3,4) "
                              "   and asg.code_mo='" + code_mo + "' ; ";

            QSqlQuery *query = new QSqlQuery(db);
            bool res = mySQL.exec(this, sql, "Запросим персону, прикреплённую к данной МО", *query, true, db, data_app);

            if (res && query->size()>0 && query->next() && query->value(0).toInt()>0) {

                // добавим запись к счётчикам
                if (sex==1) {
                    if (date_birth>date_01) {
                        m_0++;
                    } else if (date_birth>date_05) {
                        m14++;
                    } else if (date_birth>date_18) {
                        m517++;
                    } else if (date_birth>date_60) {
                        m1859++;
                    } else {
                        m60_++;
                    }
                } else {
                    if (date_birth>date_01) {
                        w_0++;
                    } else if (date_birth>date_05) {
                        w14++;
                    } else if (date_birth>date_18) {
                        w517++;
                    } else if (date_birth>date_55) {
                        w1854++;
                    } else {
                        w55_++;
                    }
                }
                sum++;
            }
        };

        if (sum==0 && ui->ch_gen_all->isChecked()) {
            // не будем сохранять это письмо
            continue;
        }

        s_content_new = s_content_new.
                replace("#CODE_MO#", code_mo).
                replace("#MO_NAME#", mo_name).
                replace("#MO_NAME_FULL#", mo_name_full).
                replace("#MO_CHIF_STATE#", mo_chif_state).
                replace("#MO_CHIF_FamIO#", mo_chif_famio).
                replace("#MO_CHIF_STATE_DP#", mo_chif_state_dp).
                replace("#MO_CHIF_FamIO_DP#", mo_chif_famio_dp).
                replace("#MO_CHIF_GREETINGS#", mo_chif_greetings).
                replace("#MO_CONTRACT_NUM#", mo_contract_num).
                replace("#MO_CONTRACT_DATE#", mo_contract_date.toString("dd.MM.yyyy")).
                replace("#REP_DATE#", date_assig.toString("dd.MM.yyyy")).
                replace("#CUR_DATE#", QDate::currentDate().toString("dd.MM.yyyy")).
                replace("#SMO_ADDRESS#", data_app.smo_logo1).

                replace("#MO#",   code_mo).
                replace("#m_0#",  (m_0>0  ? QString::number(m_0)  : " ")).
                replace("#w_0#",  (w_0>0  ? QString::number(w_0)  : " ")).
                replace("#m14#",  (m14>0  ? QString::number(m14)  : " ")).
                replace("#w14#",  (w14>0  ? QString::number(w14)  : " ")).
                replace("#m517#", (m517>0 ? QString::number(m517) : " ")).
                replace("#w517#", (w517>0 ? QString::number(w517) : " ")).
                replace("#m1859#",(m1859>0? QString::number(m1859): " ")).
                replace("#w1854#",(w1854>0? QString::number(w1854): " ")).
                replace("#m60_#", (m60_>0 ? QString::number(m60_) : " ")).
                replace("#w55_#", (w55_>0 ? QString::number(w55_) : " ")).
                replace("#SUMM#", QString::number(sum));

        s_content_new = s_content_new.
                replace("#SMO_NAME#", data_app.smo_name).
                replace("#DATE_REP#", date_assig.toString("dd.MM.yyyy")).
                replace("#DATE_REP_STR#", QString::number(date_cur.day()) + " " + month_to_str(date_cur) + " " + QString::number(date_cur.year()) ).
                replace("#DATE_CUR#", date_cur.toString("dd.MM.yyyy")).
                replace("#TERR_NAME_RP#", data_app.terr_name_rp).
                replace("#FILIAL_DIRECTOR#",data_app.filial_director).
                replace("#FILIAL_DIRECTOR_2#",data_app.filial_director_2).
                replace("#FILIAL_DIRECTOR_FamIO#",data_app.filial_chif).

                replace("#DOER_FamIO#",data_app.rep_doer).
                replace("#FILIAL_PHONE#",data_app.filial_phone).
                replace("#FILIAL_PHONE2#",data_app.filial_phone_2).
                replace("#FILIAL_EMAIL#",data_app.filial_email).
                replace("#FILIAL_EMAIL2#",data_app.filial_email_2);

        delete query_pers;





        // сохранение контента шаблона
        QFile f_content_save(temp_folder + "/content.xml");
        f_content_save.open(QIODevice::WriteOnly);
        f_content_save.write(s_content_new.toUtf8());
        f_content_save.close();

        // ===========================
        // архивация шаблона
        // ===========================
        fname_res = folder_res + date_assig.toString("yyyy-MM-dd") + " - assig_letter - " + code_mo + ".odt";
        QFile::remove(fname_res);
        while (QFile::exists(fname_res)) {
            if (QMessageBox::warning(this,
                                     "Ошибка сохранения письма",
                                     "Не удалось сохранить письмо о прикреплённых к МО: \n" + fname_template +
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
            QMessageBox::warning(this, "Ошибка копирования отчёта", "Не удалось скопировать письмо о прикреплённых к МО: \n" + fname_template + "\n\nв целевую папку\n" + fname_res + "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            ui->bn_print->setEnabled(true);
            return;
        }

        com = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname_res + "\" \"" + temp_folder + "/content.xml\"";
        if (myProcess.execute(com)!=0) {
            QMessageBox::warning(this,
                                 "Ошибка обновления контента",
                                 "При добавлении нового контента в письмо о прикреплённых к МО произошла непредвиденная ошибка\n\nОпреация прервана.");
        }

        ui->lab_cnt->setText(QString::number(n));
        QApplication::processEvents();
    }

    this->setCursor(Qt::ArrowCursor);
    ui->bn_print->setEnabled(true);

    // ===========================
    // собственно открытие шаблона
    // ===========================
    long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(folder_res.utf16()), 0, 0, SW_NORMAL);

    QMessageBox::information(this, "Готово", "Готово");
}

void print_assig_letters_by_list_wnd::on_ln_filial_director_textChanged(const QString &arg1) {
    data_app.filial_director = ui->ln_filial_director->text();
}

void print_assig_letters_by_list_wnd::on_ln_filial_director_2_textChanged(const QString &arg1) {
    data_app.filial_director_2 = ui->ln_filial_director_2->text();
}

void print_assig_letters_by_list_wnd::on_ln_rep_doer_textChanged(const QString &arg1) {
    data_app.rep_doer = ui->ln_rep_doer->text();
}

void print_assig_letters_by_list_wnd::on_ln_filial_chif_textChanged(const QString &arg1) {
    data_app.filial_chif = ui->ln_filial_chif->text();
}

void print_assig_letters_by_list_wnd::on_ln_filial_phone_textChanged(const QString &arg1) {
    data_app.filial_phone = ui->ln_filial_phone->text();
}

void print_assig_letters_by_list_wnd::on_ln_filial_phone_2_textChanged(const QString &arg1) {
    data_app.filial_phone_2 = ui->ln_filial_phone_2->text();
}

void print_assig_letters_by_list_wnd::on_ln_filial_email_textChanged(const QString &arg1) {
    data_app.filial_email = ui->ln_filial_email->text();
}

void print_assig_letters_by_list_wnd::on_ln_filial_email_2_textChanged(const QString &arg1) {
    data_app.filial_email_2 = ui->ln_filial_email_2->text();
}

void print_assig_letters_by_list_wnd::on_ch_1lpu_clicked(bool checked) {
    ui->combo_1lpu->setEnabled(checked);
}
