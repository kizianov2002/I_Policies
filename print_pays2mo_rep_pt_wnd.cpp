#include "print_pays2mo_rep_pt_wnd.h"
#include "ui_print_pays2mo_rep_pt_wnd.h"

print_pays2mo_rep_pt_wnd::print_pays2mo_rep_pt_wnd(QSqlDatabase &db, bool f_2point, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    db(db),
    f_2point(f_2point),
    data_app(data_app),
    settings(settings),
    QDialog(parent),
    ui(new Ui::print_pays2mo_rep_pt_wnd)
{
    ui->setupUi(this);

    ui->lab_point->setVisible(f_2point);
    ui->combo_point->setVisible(f_2point);

    refresh_points();

    ui->date_from->setDate(QDate::currentDate().addMonths(-1));
    ui->date_to->setDate(QDate::currentDate());
}

print_pays2mo_rep_pt_wnd::~print_pays2mo_rep_pt_wnd() {
    delete ui;
}

void print_pays2mo_rep_pt_wnd::on_bn_close_clicked() {
    close();
}

QString print_pays2mo_rep_pt_wnd::date_to_str(QDate date) {
    QString res = QString::number(date.day()) + "  ";
    switch (date.month()) {
    case 1:
        res += "января  ";
        break;
    case 2:
        res += "февраля  ";
        break;
    case 3:
        res += "марта  ";
        break;
    case 4:
        res += "апреля  ";
        break;
    case 5:
        res += "мая  ";
        break;
    case 6:
        res += "июня  ";
        break;
    case 7:
        res += "июля  ";
        break;
    case 8:
        res += "августа  ";
        break;
    case 9:
        res += "сентября ";
        break;
    case 10:
        res += "октября  ";
        break;
    case 11:
        res += "ноября ";
        break;
    case 12:
        res += "декабря  ";
        break;
    default:
        break;
    }
    res += QString::number(date.year()) + " г.";
    return res;
}

void print_pays2mo_rep_pt_wnd::refresh_points() {
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "";
    sql += QString("select p.id, '('||p.point_regnum||') '||p.point_name as name"
                   " from points p "
                   " order by p.point_regnum, id ; ");
    query->exec( sql );
    ui->combo_point->clear();

    // заполнение списка пунктов выдачи
    while (query->next()) {
        ui->combo_point->addItem( query->value(1).toString(),
                                  query->value(0).toInt() );
    }
    query->finish();
    delete query;

    ui->combo_point->setCurrentIndex(0);
}

void print_pays2mo_rep_pt_wnd::on_date_from_editingFinished() {
    if (ui->date_from->date()>ui->date_to->date()) {
        ui->date_to->setDate(ui->date_from->date());
    }
}

void print_pays2mo_rep_pt_wnd::on_date_to_editingFinished() {
    if (ui->date_to->date()>ui->date_from->date()) {
        ui->date_from->setDate(ui->date_to->date());
    }
}

void print_pays2mo_rep_pt_wnd::on_bn_print_clicked() {
    ui->bn_print->setEnabled(false);
    this->setCursor(Qt::WaitCursor);

    // выберем данные для отчёта
    QString sql_rep = "select pers_fio, pers_date_birth, date_req, date_take, case when pay_sum=0 then '0' else ' ' end "
                      " from refs_pays2mo r "
                      " where date_req>='" + ui->date_from->date().toString("yyyy-MM-dd") + "' "
                      "   and date_req<='" + ui->date_to->date().toString("yyyy-MM-dd") + "' ";
    if (f_2point)
        sql_rep   +=  "   and r.id_point=" + QString::number(ui->combo_point->currentData().toInt()) + " ; ";

    QSqlQuery *query_rep = new QSqlQuery(db);
    bool res_rep = mySQL.exec(this, sql_rep, "Получение списка справок о стоимости оказанных медуслуг за период", *query_rep, true, db, data_app);
    if (!res_rep) {
        QMessageBox::warning(this, "Список справок о стоимости оказанных медуслуг за период не получен", "При выборе полного списка справок о стоимости оказанных медуслуг за период произошла неожиданная ошибка.");
        delete query_rep;
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    };
    int n_rep = query_rep->size();

    // -----------------------------
    // собственно генерация справки
    // -----------------------------
    this->setCursor(Qt::WaitCursor);

    // распаковка шаблона
    QString rep_folder = data_app.path_reports;
    // распаковка шаблона
    QString tmp_folder = data_app.path_out + "_REPORTS_/temp/";
    QString rep_template = rep_folder + "list_pays2mo_pt.odt";


    if (!QFile::exists(rep_template)) {
        QMessageBox::warning(this,
                             "Шаблон не найден",
                             "Не найден шаблон печатной формы заявления: \n" + rep_template +
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

    // -----------------------
    // выделение строк справки
    // -----------------------
    int pos0 = 0;
    int pos1 = s_content.indexOf("<table:table-row table:style-name=\"Таблица5.2\">", pos0);
    int pos2 = s_content.indexOf("</table:table-row>", pos1) + 18;
    int pos3 = s_content.length();
    QString s_top   = s_content.mid(pos0, pos1-pos0);
    QString s_line1 = s_content.mid(pos1, pos2-pos1);
    QString s_down  = s_content.mid(pos2, pos3-pos2);

    // -----------------------------
    // правка полей контента шаблона
    // -----------------------------
    QString s_line = s_top;
    s_content = s_line.
            replace("#POINT_NAME#", f_2point ? ui->combo_point->currentText() : " - все ПВП - ").
            replace("#DATE_FROM#",  date_to_str(ui->date_from->date())).
            replace("#DATE_TO#",    date_to_str(ui->date_to->date()));

    int n = 0;
    int n_take = 0;
    int n_zero = 0;
    while (query_rep->next()) {
        n++;
        QString fio = query_rep->value(0).toString();
        QDate date_birth = query_rep->value(1).toDate();
        QDate date_req = query_rep->value(2).toDate();
        QDate date_take = query_rep->value(3).toDate();
        bool  is_zero = (query_rep->value(4).toString()=="0");

        if (!query_rep->value(3).isNull())
            n_take++;
        if (is_zero)
            n_zero++;

        s_line = s_line1;
        s_content += s_line.
                replace("#N#",       QString::number(n)).
                replace("#FIO#",     fio).
                replace("#D_BIRTH#", (query_rep->value(1).isNull() ? " " : date_birth.toString("dd.MM-yyyy"))).
                replace("#D_REQ#",   (query_rep->value(2).isNull() ? " " : date_req.toString("dd.MM-yyyy"))).
                replace("#D_TAKE#",  (query_rep->value(3).isNull() ? " " : date_take.toString("dd.MM-yyyy"))).
                replace("#IS_ZERO#", (is_zero ? "0" : " "));
    }
    s_line = s_down;
    s_content += s_line.
            replace("#N_TAKE#", QString::number(n_take)).
            replace("#N_ZERO#", QString::number(n_zero));

    // сохранение контента шаблона
    QFile f_content_save(tmp_folder + "/content.xml");
    f_content_save.open(QIODevice::WriteOnly);
    f_content_save.write(s_content.toUtf8());
    f_content_save.close();


    // ===========================
    // архивация шаблона
    // ===========================
    QString fname_res = data_app.path_out + "_REPORTS_/" +
            QDate::currentDate().toString("yyyy-MM-dd") + "  -  " + data_app.smo_short + " - " + data_app.filial_name +
            "  -  list_pays2mo_pt - " + QString(f_2point ? ui->combo_point->currentText() : " ВСЕ ПВП ") + " -- " +
            ui->date_from->date().toString("yyyy-MM-dd") + " - " + ui->date_to->date().toString("yyyy-MM-dd") + ".odt";

    QFile::remove(fname_res);
    while (QFile::exists(fname_res)) {
        if (QMessageBox::warning(this,
                                 "Ошибка сохранения печатной формы",
                                 "Не удалось сохранить шаблон печатной формы: \n" + rep_template +
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
        QMessageBox::warning(this, "Ошибка копирования шаблона", "Не удалось скопировать шаблон печатной формы: \n" + rep_template + "\n\nво временную папку\n" + fname_res + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    com = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname_res + "\" \"" + tmp_folder + "/content.xml\"";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка обновления контента",
                             "При добавлении нового контента в шаблон печатной формы заявления произошла непредвиденная ошибка\n\nОпреация прервана.");
    }



    // ===========================
    // собственно открытие шаблона
    // ===========================
    // открытие полученного ODT
    long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);
    this->setCursor(Qt::ArrowCursor);
    if (result<32) {
        QMessageBox::warning(this,
                             "Ошибка при открытии файла печатной формы справки",
                             "Файл печатной формы справки не найден:\n\n" + fname_res +
                             "\nКод ошибки: " + QString::number(result) +
                             "\n\nПопробуйте открыть этот файл вручную.\n\nЕсли файл не создан или ошибка будет повторяться - обратитесь к разработчику.");
    }
    this->setCursor(Qt::ArrowCursor);
    ui->bn_print->setEnabled(true);
}
