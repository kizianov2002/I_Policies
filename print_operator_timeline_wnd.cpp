#include "print_operator_timeline_wnd.h"
#include "ui_print_operator_timeline_wnd.h"

print_operator_timeline_wnd::print_operator_timeline_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    db(db),
    data_app(data_app),
    settings(settings),
    QDialog(parent),
    ui(new Ui::print_operator_timeline_wnd)
{
    ui->setupUi(this);

    refresh_combo_operator();
    refresh_combo_month();
}


void print_operator_timeline_wnd::refresh_combo_operator() {
    this->setCursor(Qt::WaitCursor);
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select o.id, o.oper_fio "
                  "  from operators o "
                  "       left join points p on(o.id_point=p.id) "
                  " where o.status=1 "
                  " order by o.oper_fio ; ";
    mySQL.exec(this, sql, QString("Список инспекторов"), *query, true, db, data_app);
    ui->combo_operator->clear();
    while (query->next()) {
        ui->combo_operator->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_operator->setCurrentIndex(0);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}


void print_operator_timeline_wnd::refresh_combo_month() {
    this->setCursor(Qt::WaitCursor);
    // обновление списка папок
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select min(dt_ins) as date_min, max(dt_ins) as date_max "
                  "  from public.persons ; ";
    mySQL.exec(this, sql, QString("Диапазон дат страхования персон"), *query, true, db, data_app);

    ui->combo_month->clear();
    query->next();
    QDate date_min = query->value(0).toDate();
    QDate date_max = query->value(1).toDate();
    for (int i=date_max.year(); i>=date_min.year(); i--){

        // месяцы
        if ( (i==date_min.year() && date_min.month()<=12) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=12) ) {
            ui->combo_month->addItem(QString::number(i) + ", декабрь", QDate(i,12,1));
        }
        if ( (i==date_min.year() && date_min.month()<=11) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=11) ) {
            ui->combo_month->addItem(QString::number(i) + ", ноябрь", QDate(i,11,1));
        }
        if ( (i==date_min.year() && date_min.month()<=10) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=10) ) {
            ui->combo_month->addItem(QString::number(i) + ", октябрь", QDate(i,10,1));
        }
        if ( (i==date_min.year() && date_min.month()<=9) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=9) ) {
            ui->combo_month->addItem(QString::number(i) + ", сентябрь", QDate(i,9,1));
        }
        if ( (i==date_min.year() && date_min.month()<=8) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=8) ) {
            ui->combo_month->addItem(QString::number(i) + ", август", QDate(i,8,1));
        }
        if ( (i==date_min.year() && date_min.month()<=7) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=7) ) {
            ui->combo_month->addItem(QString::number(i) + ", июль", QDate(i,7,1));
        }
        if ( (i==date_min.year() && date_min.month()<=6) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=6) ) {
            ui->combo_month->addItem(QString::number(i) + ", июнь", QDate(i,6,1));
        }
        if ( (i==date_min.year() && date_min.month()<=5) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=5) ) {
            ui->combo_month->addItem(QString::number(i) + ", май", QDate(i,5,1));
        }
        if ( (i==date_min.year() && date_min.month()<=4) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=4) ) {
            ui->combo_month->addItem(QString::number(i) + ", апрель", QDate(i,4,1));
        }
        if ( (i==date_min.year() && date_min.month()<=3) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=3) ) {
            ui->combo_month->addItem(QString::number(i) + ", март", QDate(i,3,1));
        }
        if ( (i==date_min.year() && date_min.month()<=2) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=2) ) {
            ui->combo_month->addItem(QString::number(i) + ", февраль", QDate(i,2,1));
        }
        if ( (i==date_min.year() && date_min.month()<=1) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=1) ) {
            ui->combo_month->addItem(QString::number(i) + ", январь", QDate(i,1,1));
        }
    }

    delete query;
    ui->combo_month->setCurrentIndex(0);
    this->setCursor(Qt::ArrowCursor);
}

print_operator_timeline_wnd::~print_operator_timeline_wnd() {
    delete ui;
}

void print_operator_timeline_wnd::on_bn_cansel_clicked() {
    reject();
}

void print_operator_timeline_wnd::on_bn_print_clicked() {
    int id_operator = ui->combo_operator->currentData().toInt();
    int year = ui->combo_month->currentData().toDate().year();
    int mnth = ui->combo_month->currentData().toDate().month();
    QString operator_FIO = ui->combo_operator->currentText();
    QString month_year = ui->combo_month->currentText();

    // получим данные из таблицы OPERATORS_TIMELINE
    QDate date0 = QDate(year,mnth,1);
    QDate date1 = date0.addMonths(1);
    QString sql_timeline = "select id_operator, oper_date, time_step, state "
                           "  from operators_timeline tl "
                           " where tl.id_operator=" + QString::number(id_operator) + " "
                           "   and tl.oper_date>='" + date0.toString("yyyy-MM-dd") + "' "
                           "   and tl.oper_date< '" + date1.toString("yyyy-MM-dd") + "' "
                           "   and state>0 "
                           " order by time_step, state ; ";

    QSqlQuery *query_timeline = new QSqlQuery(db);
    bool res_timeline = mySQL.exec(this, sql_timeline, "Запросим данные по работе оператора", *query_timeline, true, db, data_app);

    if (!res_timeline) {
        QMessageBox::warning(this, "Список не получен", "При выборе данных по работе оператора произошла неожиданная ошибка.");
        delete query_timeline;
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    };




    // распаковка шаблона
    QString fname_template = data_app.path_reports + "rep_oper_timeline.odg";
    QString temp_folder = data_app.path_temp + "rep_oper_timeline";
    if (!QFile::exists(fname_template)) {
        QMessageBox::warning(this,
                             "Шаблон не найден",
                             "Не найден шаблон печатной формы к МО: \n" + fname_template +
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


    // выбор шаблонов маркеров
    int pos1_0 = s_content.indexOf("<draw:custom-shape draw:style-name=\"gr12\" draw:text-style-name=\"P15\" draw:layer=\"layout\" svg:width=\"0.3cm\" svg:height=\"0.3cm\" svg:x=\"0.601cm\" svg:y=\"0.601cm\">", 0);
    int pos1_1 = s_content.indexOf("</draw:custom-shape>", pos1_0) + 20;
    QString s_cyc1 = s_content.mid(pos1_0, pos1_1-pos1_0);


    int pos2_0 = s_content.indexOf("<draw:custom-shape draw:style-name=\"gr13\" draw:text-style-name=\"P16\" draw:layer=\"layout\" svg:width=\"0.3cm\" svg:height=\"0.3cm\" svg:x=\"0.602cm\" svg:y=\"1.202cm\">", 0);
    int pos2_1 = s_content.indexOf("</draw:custom-shape>", pos2_0) + 20;
    QString s_cyc2 = s_content.mid(pos2_0, pos2_1-pos2_0);


    int pos3_0 = s_content.indexOf("<draw:custom-shape draw:style-name=\"gr14\" draw:text-style-name=\"P17\" draw:layer=\"layout\" svg:width=\"0.3cm\" svg:height=\"0.3cm\" svg:x=\"0.603cm\" svg:y=\"1.803cm\">", 0);
    int pos3_1 = s_content.indexOf("</draw:custom-shape>", pos3_0) + 20;
    QString s_cyc3 = s_content.mid(pos3_0, pos3_1-pos3_0);

    QString s_content_new = s_content.left(pos3_1);
    s_content_new = s_content_new.
            replace("#OPER_FIO#",  QString::number(id_operator) + ")  " + operator_FIO).
            replace("#MONTH#",  month_year);

    // правка контента шаблона
    while (query_timeline->next()) {
        QDate oper_date = query_timeline->value(1).toDate();
        int day = oper_date.day();
        int time_step = query_timeline->value(2).toInt();
        if (time_step<1)  continue;  //time_step = 1;
        if (time_step>48) continue;  //time_step = 48;
        int state = query_timeline->value(3).toInt();

        float x = 3.0 + (time_step-1)/2.0;
        QString s_x = "svg:x=\"" + QString::number(x) + "cm\"";

        float y = 3.0 + (day-1)/2.0;
        QString s_y = "svg:y=\"" + QString::number(y) + "cm\"";

        QString s_cyc_new;
        switch (state) {
        case 1:
            s_cyc_new = s_cyc1;
            s_cyc_new = s_cyc_new.
                    replace("svg:x=\"0.601cm\"",s_x).
                    replace("svg:y=\"0.601cm\"",s_y);
            break;
        case 2:
            s_cyc_new = s_cyc2;
            s_cyc_new = s_cyc_new.
                    replace("svg:x=\"0.602cm\"",s_x).
                    replace("svg:y=\"1.202cm\"",s_y);
            break;
        default:
            s_cyc_new = s_cyc3;
            s_cyc_new = s_cyc_new.
                    replace("svg:x=\"0.603cm\"",s_x).
                    replace("svg:y=\"1.803cm\"",s_y);
            break;
        };
        s_content_new += s_cyc_new;
    }

    s_content_new += s_content.right(s_content.size()-pos3_1);

    s_content_new = s_content_new.
            replace("#OPER_FIO#",  QString::number(id_operator) + ")  " + operator_FIO).
            replace("#MONTH#",  month_year);

    // проверим число дней в месяце
    int n1 = 1;
    QDate dat(year, mnth, 1);
    dat = dat.addMonths(1);
    dat = dat.addDays(-1);
    int n2 = dat.day();

    for (int i=1; i<=32; i++) {
        if (i<=n2) s_content_new = s_content_new.replace("#" + QString::number(i) + "#", QString::number(i));
        else s_content_new = s_content_new.replace("#" + QString::number(i) + "#", "-");
    }


    // сохранение контента шаблона
    QFile f_content_save(temp_folder + "/content.xml");
    f_content_save.open(QIODevice::WriteOnly);
    f_content_save.write(s_content_new.toUtf8());
    f_content_save.close();


    // ===========================
    // архивация шаблона
    // ===========================
    QString fname_res = data_app.path_out + "_REPORTS_/" +
            QDate::currentDate().toString("yyyy-MM-dd") + "  -  " + data_app.smo_short + " - " + data_app.filial_name + "  -  Отчёт по работе инспектора - " + operator_FIO + "  за  " + month_year + ".odg";

    QFile::remove(fname_res);
    while (QFile::exists(fname_res)) {
        if (QMessageBox::warning(this,
                                 "Ошибка сохранения отчёта",
                                 "Не удалось сохранить отчёт по работе инспектора за месяц: \n" + fname_template +
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
        QMessageBox::warning(this, "Ошибка копирования отчёта",
                             "Не удалось скопировать отчёт по работе инспектора за месяц: \n" + fname_template +
                             "\n\nво временную папку\n" + fname_res +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    QString fname_zip = fname_res;
    fname_zip = fname_zip.replace(".odg", ".zip");
    QFile::remove(fname_zip);
    if (!QFile::rename(fname_res, fname_zip)) {
        QMessageBox::warning(this,
                             "Ошибка переименования шаблона в архив",
                             "При переименовании шаблона в архив произошла непредвиденная ошибка\n\n"
                             "Опреация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    com = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname_zip + "\" \"" + temp_folder + "/content.xml\"";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка обновления контента",
                             "При добавлении нового контента в отчёт по работе инспектора за месяц произошла непредвиденная ошибка\n\n"
                             "Опреация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    if (!QFile::rename(fname_zip, fname_res)) {
        QMessageBox::warning(this,
                             "Ошибка переименования архива в шаблон",
                             "При переименовании архива в шаблон произошла непредвиденная ошибка\n\n"
                             "Опреация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    this->setCursor(Qt::ArrowCursor);
    ui->bn_print->setEnabled(true);

    // ===========================
    // собственно открытие шаблона
    // ===========================
    // открытие полученного ODT
    long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);

    QMessageBox::information(this, "Готово", "Готово");
}
