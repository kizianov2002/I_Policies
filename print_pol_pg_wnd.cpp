#include "print_pol_pg_wnd.h"
#include "ui_print_pol_pg_wnd.h"

print_pol_pg_wnd::print_pol_pg_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    db(db),
    data_app(data_app),
    settings(settings),
    QDialog(parent),
    ui(new Ui::print_pol_pg_wnd)
{
    ui->setupUi(this);

    on_bn_date_rep_clicked();
    ui->ln_smo_name->setText(data_app.smo_name);
    ui->ln_filial_name->setText(data_app.filial_name);
    ui->ln_filial_name_rp->setText(data_app.filial_name_rp);
    ui->ln_filial_director->setText(data_app.filial_director);
    ui->ln_filial_director_2->setText(data_app.filial_director_2);
    ui->ln_filial_director_fio->setText(data_app.filial_director_fio);

    set_state();
}

print_pol_pg_wnd::~print_pol_pg_wnd() {
    delete ui;
}

void print_pol_pg_wnd::set_state() {
    if (ui->checkBox->isChecked()) {
        ui->date_rep_2->setEnabled(true);
        ui->bn_date_rep_2->setEnabled(true);
        ui->bn_date_rep->setText("неделю назад");
    } else {
        ui->date_rep_2->setEnabled(false);
        ui->bn_date_rep_2->setEnabled(false);
        ui->bn_date_rep->setText("сегодня");
    }
}

void print_pol_pg_wnd::on_bn_cansel_clicked() {
    reject();
}

void print_pol_pg_wnd::on_bn_date_rep_clicked() {
    if (ui->checkBox->isChecked()) {
        ui->date_rep->setDate(QDate::currentDate().addDays(-8));
    } else {
        ui->date_rep->setDate(QDate::currentDate().addDays(-1));
    }
}

void print_pol_pg_wnd::on_bn_date_rep_mon_clicked() {
    ui->date_rep->setDate(QDate::currentDate().addMonths(-1).addDays(-1));
}

void print_pol_pg_wnd::on_bn_date_rep_2_clicked() {
    ui->date_rep_2->setDate(QDate::currentDate().addDays(-1));
}

QString print_pol_pg_wnd::month_to_str(QDate date) {
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

void print_pol_pg_wnd::on_bn_edit_clicked(bool checked) {
    if (checked) {
        ui->bn_edit->setText("Сохранить");

        ui->lab_smo_name->setEnabled(true);
        ui->ln_smo_name->setEnabled(true);
        ui->lab_filial_name->setEnabled(true);
        ui->ln_filial_name->setEnabled(true);
        ui->lab_filial_name_rp->setEnabled(true);
        ui->ln_filial_name_rp->setEnabled(true);
        ui->lab_filial_director->setEnabled(true);
        ui->lab_filial_director_fio->setEnabled(true);
        ui->ln_filial_director->setEnabled(true);
        ui->ln_filial_director_2->setEnabled(true);
        ui->ln_filial_director_fio->setEnabled(true);

        ui->bn_print->setEnabled(false);
    } else {
        data_app.smo_name = ui->ln_smo_name->text();
        data_app.filial_name = ui->ln_filial_name->text();
        data_app.filial_name_rp = ui->ln_filial_name_rp->text();
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
            settings.setValue("filial_name", data_app.filial_name.toUtf8());
            settings.setValue("filial_name_rp", data_app.filial_name_rp.toUtf8());
            settings.setValue("tfoms_director", data_app.tfoms_director.toUtf8());
            settings.setValue("tfoms_director_2", data_app.tfoms_director_2.toUtf8());
            settings.setValue("tfoms_director_fio", data_app.tfoms_director_fio.toUtf8());
            settings.setValue("filial_director", data_app.filial_director.toUtf8());
            settings.setValue("filial_director_2", data_app.filial_director_2.toUtf8());
            settings.setValue("filial_director_fio", data_app.filial_director_fio.toUtf8());
            settings.sync();
        }
        ui->bn_edit->setText("Изменить");

        ui->lab_smo_name->setEnabled(false);
        ui->ln_smo_name->setEnabled(false);
        ui->lab_filial_name->setEnabled(false);
        ui->ln_filial_name->setEnabled(false);
        ui->lab_filial_name_rp->setEnabled(false);
        ui->ln_filial_name_rp->setEnabled(false);
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

void print_pol_pg_wnd::on_bn_print_clicked() {
    if ( ui->checkBox->isChecked() &&
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
    // распаковка шаблона
    QString tmp_folder = data_app.path_out + "_REPORTS_/temp/";
    QString rep_template = rep_folder + "rep_polises_pg.odt";

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
    QDate date_new = ui->date_rep_2->date();
    QString date_rep = ui->date_rep->date().toString("yyyy-MM-dd");
    QString date_rep2;
    if (ui->checkBox->isChecked()) {
        date_rep2 = ui->date_rep_2->date().toString("yyyy-MM-dd");
        date_new = ui->date_rep_2->date();
    } else {
        date_rep2 = ui->date_rep->date().toString("yyyy-MM-dd");
        date_new = ui->date_rep->date();
    }

    int n_c=0, n_s=0, n_p=0, n_d=0;

    QString sql_c;
    sql_c = "select count(*) "
            " from polises p "
            "      join ( "
            "        select max(id) as id, id_polis "
            "        from events a "
            "        where a.event_code in('П010') "
            "        group by a.id_polis "
            "      ) qa on(qa.id_polis=p.id) "
            "      join events a on(a.id=qa.id) "
            " where a.event_dt>='" + date_rep + "' and a.event_dt<='" + date_rep2 + "' ; ";
    QSqlQuery *query_c = new QSqlQuery(db);
    bool res_c = mySQL.exec(this, sql_c, QString("Выбор всех событий П010 за диапазон дат"), *query_c, true, db, data_app);
    if (!res_c) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (query_c->next()) {
        n_c = query_c->value(0).toInt();
    }

    QString sql_s;
    sql_s = "select count(*) "
            " from polises p "
            "      join ( "
            "        select max(id) as id, id_polis "
            "        from events a "
            "        where a.event_code in('П031', 'П032', 'П033', 'П034', 'П035', 'П036') "
            "        group by a.id_polis "
            "      ) qa on(qa.id_polis=p.id) "
            "      join events a on(a.id=qa.id) "
            " where a.event_dt>='" + date_rep + "' and a.event_dt<='" + date_rep2 + "' ; ";
    QSqlQuery *query_s = new QSqlQuery(db);
    bool res_s = mySQL.exec(this, sql_s, QString("Выбор всех событий П010 за диапазон дат"), *query_s, true, db, data_app);
    if (!res_s) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (query_s->next()) {
        n_s = query_s->value(0).toInt();
    }

    QString sql_p;
    sql_p = "select count(*) "
            " from polises p "
            "      join ( "
            "        select max(id) as id, id_polis "
            "        from events a "
            "        where a.event_code in('П061', 'П063') "
            "        group by a.id_polis "
            "      ) qa on(qa.id_polis=p.id) "
            "      join events a on(a.id=qa.id) "
            " where a.event_dt>='" + date_rep + "' and a.event_dt<='" + date_rep2 + "' ; ";
    QSqlQuery *query_p = new QSqlQuery(db);
    bool res_p = mySQL.exec(this, sql_p, QString("Выбор всех событий П010 за диапазон дат"), *query_p, true, db, data_app);
    if (!res_p) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (query_p->next()) {
        n_p = query_p->value(0).toInt();
    }

    QString sql_d;
    sql_d = "select count(*) "
            " from polises p "
            "      join ( "
            "        select max(id) as id, id_polis "
            "        from events a "
            "        where a.event_code in('П062') "
            "        group by a.id_polis "
            "      ) qa on(qa.id_polis=p.id) "
            "      join events a on(a.id=qa.id) "
            " where a.event_dt>='" + date_rep + "' and a.event_dt<='" + date_rep2 + "' ; ";
    QSqlQuery *query_d = new QSqlQuery(db);
    bool res_d = mySQL.exec(this, sql_d, QString("Выбор всех событий П010 за диапазон дат"), *query_d, true, db, data_app);
    if (!res_d) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (query_d->next()) {
        n_d = query_d->value(0).toInt();
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
            replace("#NUM_CSPD#",QString::number(n_c+n_s + n_p+n_d)).
            replace("#NUM_CS#",  QString::number(n_c+n_s)).
            replace("#NUM_C#",   QString::number(n_c)).
            replace("#NUM_S#",   QString::number(n_s)).
            replace("#NUM_PD#",  QString::number(n_p+n_d)).
            replace("#NUM_P#",   QString::number(n_p)).
            replace("#NUM_D#",   QString::number(n_d)).

            replace("#FILIAL_NAME#", data_app.filial_name_rp).
            replace("#SMO_NAME#",    data_app.smo_name).
            replace("#DATE_CUR#",    QDate::currentDate().toString("dd.MM.yyyy") + " г.").
            replace("#FILIAL_DIRECTOR#",     data_app.filial_director).
            replace("#FILIAL_DIRECTOR_2#",    data_app.filial_director_2).
            replace("#FILIAL_DIRECTOR_FIO#", data_app.filial_director_fio);
    if (ui->checkBox->isChecked()) {
        s_content = s_content.
                replace("#DATE_REP#", ui->date_rep->date().toString("dd.MM.yyyy") + " г. – " + ui->date_rep_2->date().toString("dd.MM.yyyy") + " г.");
    } else {
        s_content = s_content.
                replace("#DATE_REP#",  ui->date_rep->date().toString("dd.MM.yyyy") + " г.");
    }

    // сохранение контента шаблона
    QFile f_content_save(tmp_folder + "/content.xml");
    f_content_save.open(QIODevice::WriteOnly);
    f_content_save.write(s_content.toUtf8());
    f_content_save.close();


    // ===========================
    // архивация шаблона
    // ===========================
    QString fname_res = data_app.path_out + "_REPORTS_/" +
            QDate::currentDate().toString("yyyy-MM-dd") + "  -  " + data_app.smo_short + " - " + data_app.filial_name + "  -  rep_polises_pg - " + date_old.toString("yyyy-MM-dd") + (date_new!=date_old ? (" - " + date_new.toString("yyyy-MM-dd")) : "") + ".odt";

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

void print_pol_pg_wnd::on_ln_smo_name_textChanged(const QString &arg1) {
    data_app.smo_name = ui->ln_smo_name->text();
}

void print_pol_pg_wnd::on_ln_filial_director_textChanged(const QString &arg1) {
    data_app.filial_director = ui->ln_filial_director->text();
}

void print_pol_pg_wnd::on_ln_filial_director_2_textChanged(const QString &arg1) {
    data_app.filial_director_2 = ui->ln_filial_director_2->text();
}

void print_pol_pg_wnd::on_ln_filial_director_fio_textChanged(const QString &arg1) {
    data_app.filial_director_fio = ui->ln_filial_director_fio->text();
}

void print_pol_pg_wnd::on_checkBox_clicked() {
    set_state();
}
