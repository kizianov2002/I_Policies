#include "print_pol_monitoring_wnd.h"
#include "ui_print_pol_monitoring_wnd.h"

print_pol_monitoring_wnd::print_pol_monitoring_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    db(db),
    data_app(data_app),
    settings(settings),
    QDialog(parent),
    ui(new Ui::print_pol_monitoring_wnd)
{
    ui->setupUi(this);

    ui->date_rep->setDate(QDate(2015,1,1));
    ui->date_rep_2->setDate(QDate::currentDate());

    ui->ln_filial_by_city->setText(data_app.filial_by_city);
    ui->ln_filial_by_city_tp->setText(data_app.filial_by_city_tp);
    ui->ln_filial_director->setText(data_app.filial_director);
    ui->ln_filial_director_2->setText(data_app.filial_director_2);
    ui->ln_filial_director_fio->setText(data_app.filial_director_fio);

    set_state();
}

print_pol_monitoring_wnd::~print_pol_monitoring_wnd() {
    delete ui;
}

void print_pol_monitoring_wnd::set_state() {
    if (ui->ch_date2->isChecked()) {
        ui->date_rep_2->setEnabled(true);
        ui->bn_date_rep_2->setEnabled(true);
        ui->bn_date_rep->setText("неделю назад");
    } else {
        ui->date_rep_2->setEnabled(false);
        ui->bn_date_rep_2->setEnabled(false);
        ui->bn_date_rep->setText("сегодня");
    }
}

void print_pol_monitoring_wnd::on_bn_cansel_clicked() {
    reject();
}

void print_pol_monitoring_wnd::on_bn_date_rep_clicked() {
    if (ui->ch_date2->isChecked()) {
        ui->date_rep->setDate(QDate::currentDate().addDays(-8));
    } else {
        ui->date_rep->setDate(QDate::currentDate());
    }
}

void print_pol_monitoring_wnd::on_bn_date_rep_mon_clicked() {
    ui->date_rep->setDate(QDate::currentDate().addMonths(-1).addDays(-1));
}

void print_pol_monitoring_wnd::on_bn_date_rep_2_clicked() {
    ui->date_rep_2->setDate(QDate::currentDate().addDays(-1));
}

QString print_pol_monitoring_wnd::month_to_str(QDate date) {
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

void print_pol_monitoring_wnd::on_bn_edit_clicked(bool checked) {
    if (checked) {
        ui->bn_edit->setText("Сохранить");

        ui->lab_filial_by_city->setEnabled(true);
        ui->ln_filial_by_city->setEnabled(true);
        ui->lab_filial_by_city_tp->setEnabled(true);
        ui->ln_filial_by_city_tp->setEnabled(true);
        ui->lab_filial_director->setEnabled(true);
        ui->lab_filial_director_fio->setEnabled(true);
        ui->ln_filial_director->setEnabled(true);
        ui->ln_filial_director_2->setEnabled(true);
        ui->ln_filial_director_fio->setEnabled(true);

        ui->bn_print->setEnabled(false);
    } else {
        data_app.filial_by_city = ui->ln_filial_by_city->text();
        data_app.filial_by_city_tp = ui->ln_filial_by_city_tp->text();
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
            settings.setValue("filial_by_city", data_app.filial_by_city.toUtf8());
            settings.setValue("filial_by_city_tp", data_app.filial_by_city_tp.toUtf8());
            settings.setValue("filial_director", data_app.filial_director.toUtf8());
            settings.setValue("filial_director_2", data_app.filial_director_2.toUtf8());
            settings.setValue("filial_director_fio", data_app.filial_director_fio.toUtf8());
            settings.sync();
        }
        ui->bn_edit->setText("Изменить");

        ui->lab_filial_by_city->setEnabled(false);
        ui->ln_filial_by_city->setEnabled(false);
        ui->lab_filial_by_city_tp->setEnabled(false);
        ui->ln_filial_by_city_tp->setEnabled(false);
        ui->lab_filial_director->setEnabled(false);
        ui->lab_filial_director_fio->setEnabled(false);
        ui->ln_filial_director->setEnabled(false);
        ui->ln_filial_director_2->setEnabled(false);
        ui->ln_filial_director_fio->setEnabled(false);

        ui->bn_print->setEnabled(true);
    }
}

#include "qt_windows.h"
#include "qwindowdefs_win.h"
#include <shellapi.h>

void print_pol_monitoring_wnd::on_bn_print_clicked() {
    if ( ui->ch_date2->isChecked() &&
         ui->date_rep->date()>ui->date_rep_2->date() ) {
        QMessageBox::warning(this,
                             "Неверные данные",
                             "Начальная дата интервала не может позже раньше конечной ! \n"
                             "\n\nОперация прервана.");
        return;
    }

    ui->bn_print->setEnabled(false);
    this->setCursor(Qt::WaitCursor);

    // распаковка шаблона
    QString rep_folder = data_app.path_reports;
    // распаковка шаблона ВС
    QString tmp_folder = data_app.path_out + "_REPORTS_/temp/";
    QString rep_template = rep_folder + "rep_polises_monitoring.odt";



    if (!QFile::exists(rep_template)) {
        QMessageBox::warning(this,
                             "Шаблон не найден",
                             "Не найден шаблон печатной формы уведомления: \n" + rep_template +
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


    // получение данных
    QDate date_old = ui->date_rep->date();
    QString date_rep = ui->date_rep->date().toString("yyyy-MM-dd");

    QDate date_new = QDate::currentDate();  //ui->date_rep_2->date();
    if (ui->ch_date2->isChecked())
        date_new = ui->date_rep_2->date();
    QString date_rep2 = date_new.toString("yyyy-MM-dd");


    int n_vs  =0, all_vs  =0;
    int n_smo =0, all_smo =0;
    int n_vs2 =0, all_vs2 =0;
    int n_vb  =0, all_vb  =0;
    int n_fond=0, all_fond=0;
    int n_fact=0, all_fact=0;
    int n_ret =0, n_ret_papr =0, n_ret_elec =0, all_ret =0, all_ret_papr =0, all_ret_elec =0;
    int n_re2 =0, n_re2_papr =0, n_re2_elec =0, all_re2 =0, all_re2_papr =0, all_re2_elec =0;
    //int n_elec=0, all_elec=0;
    int n_get =0, n_get_papr =0, n_get_elec =0, all_get =0, all_get_papr =0, all_get_elec =0;
    int n_bls =0, n_bls_papr =0, n_bls_elec =0, all_bls =0, all_bls_papr =0, all_bls_elec =0;

    // -----------------------------
    // выдано временных свидетельств
    // -----------------------------
    QString sql_n_vs;
    sql_n_vs = "select count(*) "
               "  from blanks_vs bv "
               "       join persons e on(e.id=bv.id_person) "
               " where bv.date_spent>='" + date_rep  + "' "
               "   and bv.date_spent<='" + date_rep2 + "' "
               "   and bv.status<>-1 ; ";
    QSqlQuery *query_n_vs = new QSqlQuery(db);
    bool res_n_vs = mySQL.exec(this, sql_n_vs, QString("Выбор числа ВС, выданных за диапазон дат"), *query_n_vs, true, db, data_app);
    if (!res_n_vs) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (query_n_vs->next()) {
        n_vs  = query_n_vs->value(0).toInt();
        n_smo = query_n_vs->value(0).toInt();
    }


    QString sql_all_vs;
    sql_all_vs = "select count(*) "
                 "  from blanks_vs bv "
                 "       join persons e on(e.id=bv.id_person) "
                 " where bv.date_spent<=CURRENT_DATE "
                 "   and bv.status<>-1 ; ";
    QSqlQuery *query_all_vs = new QSqlQuery(db);
    bool res_all_vs = mySQL.exec(this, sql_all_vs, QString("Выбор числа ВС, выданных - ВСЕГО"), *query_all_vs, true, db, data_app);
    if (!res_all_vs) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (query_all_vs->next()) {
        all_vs  = query_all_vs->value(0).toInt();
        all_smo = query_all_vs->value(0).toInt();
    }


    QString sql_n_vs2;
    sql_n_vs2 = "select count(*) "
                "  from events a "
                " where a.event_dt>='" + date_rep + "' and a.event_dt<='" + date_rep2 + "' "
                "   and a.event_code in('П061','П062','П063') ; ";
    QSqlQuery *query_n_vs2 = new QSqlQuery(db);
    bool res_n_vs2 = mySQL.exec(this, sql_n_vs2, QString("Выбор числа повторных ВС, выданных за диапазон дат"), *query_n_vs2, true, db, data_app);
    if (!res_n_vs2) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (query_n_vs2->next()) {
        n_vs2 = query_n_vs2->value(0).toInt();
    }


    QString sql_all_vs2;
    sql_all_vs2 = "select count(*) "
                  "  from events a "
                  " where a.event_dt<=CURRENT_DATE and a.event_code in('П061','П062','П063') ; ";
    QSqlQuery *query_all_vs2 = new QSqlQuery(db);
    bool res_all_vs2 = mySQL.exec(this, sql_all_vs2, QString("Выбор числа повторных ВС, выданных - ВСЕГО"), *query_all_vs2, true, db, data_app);
    if (!res_all_vs2) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (query_all_vs2->next()) {
        all_vs2 = query_all_vs2->value(0).toInt();
    }


    // -----------------------------------
    // передано СМО временных свидетельств
    // -----------------------------------
    QString sql_n_vb;
    sql_n_vb = "select sum(i.vs_count) "
               "  from vs_intervals i "
               " where i.date_get>='" + date_rep + "' and i.date_get<='" + date_rep2 + "' ; ";
    QSqlQuery *query_n_vb = new QSqlQuery(db);
    bool res_n_vb = mySQL.exec(this, sql_n_vb, QString("Выбор числа ВС, переданных в СМО за диапазон дат"), *query_n_vb, true, db, data_app);
    if (!res_n_vb) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (query_n_vb->next()) {
        n_vb = query_n_vb->value(0).toInt();
    }


    QString sql_all_vb;
    sql_all_vb = "select sum(i.vs_count) "
                 "  from vs_intervals i "
                 " where i.date_get<=CURRENT_DATE ; ";
    QSqlQuery *query_all_vb = new QSqlQuery(db);
    bool res_all_vb = mySQL.exec(this, sql_all_vb, QString("Выбор числа ВС, переданных в СМО - ВСЕГО"), *query_all_vb, true, db, data_app);
    if (!res_all_vb) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (query_all_vb->next()) {
        all_vb = query_all_vb->value(0).toInt();
    }


    // --------------------------
    // получено полисов ОМС в СМО
    // --------------------------
    QString sql_n_ret;
    sql_n_ret = "select count(*) "
                "  from blanks_pol bp "
                " where bp.date_add>='" + date_rep + "' and bp.date_add<='" + date_rep2 + "' "
                "   and (select * from st_blank_pol(bp.id, '" + date_rep2 + "')) in (0,1,-2,-3,-5) ; ";
    QSqlQuery *query_n_ret = new QSqlQuery(db);
    bool res_n_ret = mySQL.exec(this, sql_n_ret, QString("Выбор числа ЕНП, выпущенных ГОСЗНАК за диапазон дат"), *query_n_ret, true, db, data_app);
    if (!res_n_ret) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (query_n_ret->next()) {
        n_ret = query_n_ret->value(0).toInt();
        n_re2 = query_n_ret->value(0).toInt();
    }


    QString sql_all_ret;
    sql_all_ret = "select count(*) "
                  "  from blanks_pol bp "
                  " where bp.date_add<=CURRENT_DATE and bp.status in (0,1,-2,-3,-5) ; ";
    QSqlQuery *query_all_ret = new QSqlQuery(db);
    bool res_all_ret = mySQL.exec(this, sql_all_ret, QString("Выбор числа ЕНП, выпущенных ГОСЗНАК - ВСЕГО"), *query_all_ret, true, db, data_app);
    if (!res_all_ret) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (query_all_ret->next()) {
        all_ret = query_all_ret->value(0).toInt();
        all_re2 = query_all_ret->value(0).toInt();
    }


    QString sql_n_re2_papr;
    sql_n_re2_papr = "select count(*) "
                     "  from blanks_pol bp "
                     " where bp.date_add>='" + date_rep + "' and bp.date_add<='" + date_rep2 + "' "
                     "   and (select * from st_blank_pol(bp.id, '" + date_rep2 + "')) in (0,1,-2,-3,-5) "
                     "   and bp.f_polis<>2 ; ";
    QSqlQuery *query_n_re2_papr = new QSqlQuery(db);
    bool res_n_re2_papr = mySQL.exec(this, sql_n_re2_papr, QString("Выбор числа бумажных ЕНП, переданных в СМО за диапазон дат"), *query_n_re2_papr, true, db, data_app);
    if (!res_n_re2_papr) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (query_n_re2_papr->next()) {
        n_ret_papr = query_n_re2_papr->value(0).toInt();
        n_re2_papr = query_n_re2_papr->value(0).toInt();
    }


    QString sql_all_re2_papr;
    sql_all_re2_papr = "select count(*) "
                       "  from blanks_pol bp "
                       " where bp.date_add<=CURRENT_DATE and bp.status in (0,1,-2,-3,-5) "
                       "   and bp.f_polis<>2 ; ";
    QSqlQuery *query_all_re2_papr = new QSqlQuery(db);
    bool res_all_re2_papr = mySQL.exec(this, sql_all_re2_papr, QString("Выбор числа бумажных ЕНП, переданных в СМО - ВСЕГО"), *query_all_re2_papr, true, db, data_app);
    if (!res_all_re2_papr) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (query_all_re2_papr->next()) {
        all_ret_papr = query_all_re2_papr->value(0).toInt();
        all_re2_papr = query_all_re2_papr->value(0).toInt();
    }



    QString sql_n_re2_elec;
    sql_n_re2_elec = "select count(*) "
                     "  from blanks_pol bp "
                     " where bp.date_add>='" + date_rep + "' and bp.date_add<='" + date_rep2 + "' "
                     "   and (select * from st_blank_pol(bp.id, '" + date_rep2 + "')) in (0,1,-2,-3,-5)"
                     "   and bp.f_polis=2 ; ";
    QSqlQuery *query_n_re2_elec = new QSqlQuery(db);
    bool res_n_re2_elec = mySQL.exec(this, sql_n_re2_elec, QString("Выбор числа пластиковых ЕНП, переданных в СМО за диапазон дат"), *query_n_re2_elec, true, db, data_app);
    if (!res_n_re2_elec) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (query_n_re2_elec->next()) {
        n_ret_elec = query_n_re2_elec->value(0).toInt();
        n_re2_elec = query_n_re2_elec->value(0).toInt();
    }


    QString sql_all_re2_elec;
    sql_all_re2_elec = "select count(*) "
                       "  from blanks_pol bp "
                       " where bp.date_add<=CURRENT_DATE and bp.status in (0,1,-2,-3,-5) "
                       "   and bp.f_polis=2 ; ";
    QSqlQuery *query_all_re2_elec = new QSqlQuery(db);
    bool res_all_re2_elec = mySQL.exec(this, sql_all_re2_elec, QString("Выбор числа пластиковых ЕНП, переданных в СМО - ВСЕГО"), *query_all_re2_elec, true, db, data_app);
    if (!res_all_re2_elec) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (query_all_re2_elec->next()) {
        all_ret_elec = query_all_re2_elec->value(0).toInt();
        all_re2_elec = query_all_re2_elec->value(0).toInt();
    }



    // ----------------------
    // выдано СМО полисов ОМС
    // ----------------------
    QString sql_n_get;
    sql_n_get = "select count(*) "
                "  from blanks_pol bp "
                " where bp.date_add>='" + date_rep + "' and bp.date_add<='" + date_rep2 + "' "
                "   and (select * from st_blank_pol(bp.id, '" + date_rep2 + "')) in (1,-2,-3,-5) ; ";
    QSqlQuery *query_n_get = new QSqlQuery(db);
    bool res_n_get = mySQL.exec(this, sql_n_get, QString("Выбор числа ЕНП, выданных застрахованным за диапазон дат"), *query_n_get, true, db, data_app);
    if (!res_n_get) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (query_n_get->next()) {
        n_get = query_n_get->value(0).toInt();
    }


    QString sql_all_get;
    sql_all_get = "select count(*) "
                  "  from blanks_pol bp "
                  " where bp.date_add<=CURRENT_DATE and bp.status in (1,-2,-3,-5) ; ";
    QSqlQuery *query_all_get = new QSqlQuery(db);
    bool res_all_get = mySQL.exec(this, sql_all_get, QString("Выбор числа ЕНП, выданных застрахованным - ВСЕГО"), *query_all_get, true, db, data_app);
    if (!res_all_get) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (query_all_get->next()) {
        all_get = query_all_get->value(0).toInt();
    }



    QString sql_n_get_papr;
    sql_n_get_papr = "select count(*) "
                     "  from blanks_pol bp "
                     " where bp.date_add>='" + date_rep + "' and bp.date_add<='" + date_rep2 + "' "
                     "   and (select * from st_blank_pol(bp.id, '" + date_rep2 + "')) in (1,-2,-3,-5) "
                     "   and bp.f_polis<>2 ; ";
    QSqlQuery *query_n_get_papr = new QSqlQuery(db);
    bool res_n_get_papr = mySQL.exec(this, sql_n_get_papr, QString("Выбор числа бумажных ЕНП, выданных застрахованным за диапазон дат"), *query_n_get_papr, true, db, data_app);
    if (!res_n_get_papr) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (query_n_get_papr->next()) {
        n_get_papr = query_n_get_papr->value(0).toInt();
    }


    QString sql_all_get_papr;
    sql_all_get_papr = "select count(*) "
                       "  from blanks_pol bp "
                       " where bp.date_add<=CURRENT_DATE and bp.status in (1,-2,-3,-5) "
                       "   and bp.f_polis<>2 ; ";
    QSqlQuery *query_all_get_papr = new QSqlQuery(db);
    bool res_all_get_papr = mySQL.exec(this, sql_all_get_papr, QString("Выбор числа бумажных ЕНП, выданных застрахованным - ВСЕГО"), *query_all_get_papr, true, db, data_app);
    if (!res_all_get_papr) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (query_all_get_papr->next()) {
        all_get_papr = query_all_get_papr->value(0).toInt();
    }



    QString sql_n_get_elec;
    sql_n_get_elec = "select count(*) "
                     "  from blanks_pol bp "
                     " where bp.date_add>='" + date_rep + "' and bp.date_add<='" + date_rep2 + "' "
                     "   and (select * from st_blank_pol(bp.id, '" + date_rep2 + "')) in (1,-2,-3,-5) "
                     "   and bp.f_polis=2 ; ";
    QSqlQuery *query_n_get_elec = new QSqlQuery(db);
    bool res_n_get_elec = mySQL.exec(this, sql_n_get_elec, QString("Выбор числа пластиковых ЕНП, выданных застрахованным за диапазон дат"), *query_n_get_elec, true, db, data_app);
    if (!res_n_get_elec) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (query_n_get_elec->next()) {
        n_get_elec = query_n_get_elec->value(0).toInt();
    }


    QString sql_all_get_elec;
    sql_all_get_elec = "select count(*) "
                       "  from blanks_pol bp "
                       " where bp.date_add<=CURRENT_DATE and bp.status in (1,-2,-3,-5) "
                       "   and bp.f_polis=2 ; ";
    QSqlQuery *query_all_get_elec = new QSqlQuery(db);
    bool res_all_get_elec = mySQL.exec(this, sql_all_get_elec, QString("Выбор числа пластиковых ЕНП, выданных застрахованным - ВСЕГО"), *query_all_get_elec, true, db, data_app);
    if (!res_all_get_elec) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (query_all_get_elec->next()) {
        all_get_elec = query_all_get_elec->value(0).toInt();
    }



    // --------------------------------------
    // число невостребованных бланков полисов
    // --------------------------------------
    QString sql_n_bls;
    sql_n_bls = "select count(*) "
                "  from blanks_pol bp "
                "       join persons e on(e.id=bp.id_person) "
                "       join polises vs on(vs.pol_v=2 and vs.id=(select * from id_person_pol(e.id, '" + date_rep2 + "'))) "
                " where /*(select * from st_person(e.id, '" + date_rep2 + "')) in(1,2,3,4,11,12,13,14)"
                "   and*/ bp.date_add>='" + date_rep + "' and bp.date_add<='" + date_rep2 + "' and (select * from st_blank_pol(bp.id, '" + date_rep2 + "'))=0 "
                "   and vs.date_end>='" + date_rep  + "' "
                "   and vs.date_end<'" + date_rep2 + "' ; ";
    QSqlQuery *query_n_bls = new QSqlQuery(db);
    bool res_n_bls = mySQL.exec(this, sql_n_bls, QString("Выбор числа невострабованных бланков ЕНП, за диапазон дат"), *query_n_bls, true, db, data_app);
    if (!res_n_bls) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (query_n_bls->next()) {
        n_bls = query_n_bls->value(0).toInt();
    }


    QString sql_all_bls;
    sql_all_bls = "select count(*) "
                   "  from blanks_pol bp "
                   "       join persons e on(e.id=bp.id_person) "
                   "       join polises vs on(vs.pol_v=2 and vs.id=e.id_polis) "
                   " where /*e.status in(1,2,3,4,11,12,13,14)"
                   "   and*/ bp.status=0 "
                   "   and vs.date_end<CURRENT_DATE; ";
    QSqlQuery *query_all_bls = new QSqlQuery(db);
    bool res_all_bls = mySQL.exec(this, sql_all_bls, QString("Выбор числа невостребованных бланков ЕНП - ВСЕГО"), *query_all_bls, true, db, data_app);
    if (!res_all_bls) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (query_all_bls->next()) {
        all_bls = query_all_bls->value(0).toInt();
    }



    QString sql_n_bls_papr;
    sql_n_bls_papr = "select count(*) "
                     "  from blanks_pol bp "
                     "       join persons e on(e.id=bp.id_person) "
                     "       join polises vs on(vs.pol_v=2 and vs.id=(select * from id_person_pol(e.id, '" + date_rep2 + "'))) "
                     " where /*(select * from st_person(e.id, '" + date_rep2 + "')) in(1,2,3,4,11,12,13,14)"
                     "   and*/ bp.date_add>='" + date_rep + "' and bp.date_add<='" + date_rep2 + "' and (select * from st_blank_pol(bp.id, '" + date_rep2 + "'))=0 "
                     "   and vs.date_end>='" + date_rep  + "' "
                     "   and vs.date_end<'" + date_rep2 + "' "
                     "   and bp.f_polis<>2 ; ";
    QSqlQuery *query_n_bls_papr = new QSqlQuery(db);
    bool res_n_bls_papr = mySQL.exec(this, sql_n_bls_papr, QString("Выбор числа невостребованных бумажных ЕНП, недовыданных за диапазон дат"), *query_n_bls_papr, true, db, data_app);
    if (!res_n_bls_papr) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (query_n_bls_papr->next()) {
        n_bls_papr = query_n_bls_papr->value(0).toInt();
    }


    QString sql_all_bls_papr;
    sql_all_bls_papr = "select count(*) "
                       "  from blanks_pol bp "
                       "       join persons e on(e.id=bp.id_person) "
                       "       join polises vs on(vs.pol_v=2 and vs.id=e.id_polis) "
                       " where /*e.status in(1,2,3,4,11,12,13,14)"
                       "   and*/ bp.status=0 "
                       "   and vs.date_end<CURRENT_DATE "
                       "   and bp.f_polis<>2 ; ";
    QSqlQuery *query_all_bls_papr = new QSqlQuery(db);
    bool res_all_bls_papr = mySQL.exec(this, sql_all_bls_papr, QString("Выбор числа невостребованных бумажных ЕНП, недовыданных - ВСЕГО"), *query_all_bls_papr, true, db, data_app);
    if (!res_all_bls_papr) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (query_all_bls_papr->next()) {
        all_bls_papr = query_all_bls_papr->value(0).toInt();
    }



    QString sql_n_bls_elec;
    sql_n_bls_elec = "select count(*) "
                     "  from blanks_pol bp "
                     "       join persons e on(e.id=bp.id_person) "
                     "       join polises vs on(vs.pol_v=2 and vs.id=(select * from id_person_pol(e.id, '" + date_rep2 + "'))) "
                     " where /*(select * from st_person(e.id, '" + date_rep2 + "')) in(1,2,3,4,11,12,13,14)"
                     "   and*/ bp.date_add>='" + date_rep + "' and bp.date_add<='" + date_rep2 + "' and (select * from st_blank_pol(bp.id, '" + date_rep2 + "'))=0 "
                     "   and vs.date_end>='" + date_rep  + "' "
                     "   and vs.date_end<'" + date_rep2 + "' "
                     "   and bp.f_polis=2 ; ";
    QSqlQuery *query_n_bls_elec = new QSqlQuery(db);
    bool res_n_bls_elec = mySQL.exec(this, sql_n_bls_elec, QString("Выбор числа невостребованных пластиковых ЕНП, недовыданных за диапазон дат"), *query_n_bls_elec, true, db, data_app);
    if (!res_n_bls_elec) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (query_n_bls_elec->next()) {
        n_bls_elec = query_n_bls_elec->value(0).toInt();
    }


    QString sql_all_bls_elec;
    sql_all_bls_elec = "select count(*) "
                       "  from blanks_pol bp "
                       "       join persons e on(e.id=bp.id_person) "
                       "       join polises vs on(vs.pol_v=2 and vs.id=e.id_polis) "
                       " where /*e.status in(1,2,3,4,11,12,13,14)"
                       "   and*/ bp.status=0 "
                       "   and vs.date_end<CURRENT_DATE "
                       "   and bp.f_polis=2 ; ";
    QSqlQuery *query_all_bls_elec = new QSqlQuery(db);
    bool res_all_bls_elec = mySQL.exec(this, sql_all_bls_elec, QString("Выбор числа невостребованных пластиковых ЕНП, недовыданных - ВСЕГО"), *query_all_bls_elec, true, db, data_app);
    if (!res_all_bls_elec) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (query_all_bls_elec->next()) {
        all_bls_elec = query_all_bls_elec->value(0).toInt();
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
                             "Файл контента пустой",
                             "Файл контента пустой. Возможно шаблон был испорчен.\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    // правка полей контента шаблона
    s_content = s_content.
            replace("#N_VS#",    QString::number(n_vs)    ).
            replace("#ALL_VS#",  QString::number(all_vs)  ).
            replace("#N_VS2#",   QString::number(n_vs2)   ).
            replace("#ALL_VS2#", QString::number(all_vs2) ).
            replace("#N_VB#",    QString::number(n_vb)    ).
            replace("#ALL_VB#",  QString::number(all_vb)  ).
            replace("#N_SMO#",   QString::number(n_smo)   ).
            replace("#ALL_SMO#", QString::number(all_smo) ).
            replace("#N_FOND#",  QString::number(n_vs)    ). //QString::number(n_fond)  ).
            replace("#ALL_FOND#",QString::number(all_vs)  ). //QString::number(all_fond)).
            replace("#N_FACT#",  QString::number(n_vs)    ). //QString::number(n_fact)  ).
            replace("#ALL_FACT#",QString::number(all_vs)  ). //QString::number(all_fact)).

            replace("#N_RET#",   QString::number(n_ret)   ).
            replace("#ALL_RET#", QString::number(all_ret) ).
            replace("#N_RET_PAPR#",   QString::number(n_ret_papr)   ).
            replace("#ALL_RET_PAPR#", QString::number(all_ret_papr) ).
            replace("#N_RET_ELEC#",   QString::number(n_ret_elec)   ).
            replace("#ALL_RET_ELEC#", QString::number(all_ret_elec) ).

            replace("#N_RE2#",  QString::number(n_re2)  ).
            replace("#ALL_RE2#",QString::number(all_re2)).
            replace("#N_RE2_PAPR#",  QString::number(n_re2_papr)  ).
            replace("#ALL_RE2_PAPR#",QString::number(all_re2_papr)).
            replace("#N_RE2_ELEC#",  QString::number(n_re2_elec)  ).
            replace("#ALL_RE2_ELEC#",QString::number(all_re2_elec)).

            replace("#N_GET#",   QString::number(n_get)   ).
            replace("#ALL_GET#", QString::number(all_get) ).
            replace("#N_GET_PAPR#",   QString::number(n_get_papr)   ).
            replace("#ALL_GET_PAPR#", QString::number(all_get_papr) ).
            replace("#N_GET_ELEC#",   QString::number(n_get_elec)   ).
            replace("#ALL_GET_ELEC#", QString::number(all_get_elec) ).

            replace("#N_BLS#",   QString::number(n_bls)   ).
            replace("#ALL_BLS#", QString::number(all_bls) ).
            replace("#N_BLS_PAPR#",   QString::number(n_bls_papr)   ).
            replace("#ALL_BLS_PAPR#", QString::number(all_bls_papr) ).
            replace("#N_BLS_ELEC#",   QString::number(n_bls_elec)   ).
            replace("#ALL_BLS_ELEC#", QString::number(all_bls_elec) ).

            replace("#FILIAL_BY_CITY#", data_app.filial_by_city_tp).
            replace("#DATE_CUR#",       QDate::currentDate().toString("dd.MM.yyyy")).
            replace("#FILIAL_DIRECTOR#",     data_app.filial_director).
            replace("#FILIAL_DIRECTOR_2#",   data_app.filial_director_2).
            replace("#FILIAL_DIRECTOR_FIO#", data_app.filial_director_fio).
            replace("Отчетный день",QString("На отчётный день ") + date_new.toString("dd.MM.yyyy")).
            replace("Всего (нарастающим итогом с 2015 г.)",QString("Всего (нарастающим итогом с 2015 г.)   на ") + QDate::currentDate().toString("dd.MM.yyyy"));

    s_content = s_content.
                replace("#DATE_REP#", date_old.toString("dd.MM.yyyy") + " г. – " + date_new.toString("dd.MM.yyyy") + " г.");

    // сохранение контента шаблона
    QFile f_content_save(tmp_folder + "/content.xml");
    f_content_save.open(QIODevice::WriteOnly);
    f_content_save.write(s_content.toUtf8());
    f_content_save.close();


    // ===========================
    // архивация шаблона
    // ===========================
    QString fname_res = data_app.path_out + "_REPORTS_/" +
            QDate::currentDate().toString("yyyy-MM-dd") + "  -  " + data_app.smo_short + " - " + data_app.filial_name + "  -  rep_polises_monitoring - " + date_old.toString("yyyy-MM-dd") + (date_new!=date_old ? (" - " + date_new.toString("yyyy-MM-dd")) : "") + ".odt";

    QFile::remove(fname_res);
    while (QFile::exists(fname_res)) {
        if (QMessageBox::warning(this,
                                 "Ошибка сохранения печатной формы уведомления",
                                 "Не удалось сохранить шаблон печатной формы уведомления: \n" + rep_template +
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
        QMessageBox::warning(this, "Ошибка копирования шаблона", "Не удалось скопировать шаблон печатной формы уведомления: \n" + rep_template + "\n\nво временную папку\n" + fname_res + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    com = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname_res + "\" \"" + tmp_folder + "/content.xml\"";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка обновления контента",
                             "При добавлении нового контента в шаблон печатной формы уведомления произошла непредвиденная ошибка\n\nОпреация прервана.");
    }


    // ===========================
    // собственно открытие шаблона
    // ===========================
    // открытие полученного ODT
    long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);
    this->setCursor(Qt::ArrowCursor);
    if (result<32) {
        QMessageBox::warning(this,
                             "Ошибка при открытии файла печатной формы уведомления",
                             "Файл печатной формы уведомления не найден:\n\n" + fname_res +
                             "\nКод ошибки: " + QString::number(result) +
                             "\n\nПопробуйте открыть этот файл вручную.\n\nЕсли файл не создан или ошибка будет повторяться - обратитесь к разработчику.");
    }
    this->setCursor(Qt::ArrowCursor);
    ui->bn_print->setEnabled(true);
    close();
}

void print_pol_monitoring_wnd::on_ln_filial_director_textChanged(const QString &arg1) {
    data_app.filial_director = ui->ln_filial_director->text();
}

void print_pol_monitoring_wnd::on_ln_filial_director_2_textChanged(const QString &arg1) {
    data_app.filial_director_2 = ui->ln_filial_director_2->text();
}

void print_pol_monitoring_wnd::on_ln_filial_director_fio_textChanged(const QString &arg1) {
    data_app.filial_director_fio = ui->ln_filial_director_fio->text();
}

void print_pol_monitoring_wnd::on_ch_date2_clicked(bool checked) {
    set_state();
    on_bn_date_rep_2_clicked();
}
