#include "print_monitoring_pers_by_date_wnd.h"
#include "ui_print_monitoring_pers_by_date_wnd.h"

print_monitoring_pers_by_date_wnd::print_monitoring_pers_by_date_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    db(db),
    data_app(data_app),
    settings(settings),
    QDialog(parent),
    ui(new Ui::print_monitoring_pers_by_date_wnd)
{
    ui->setupUi(this);

    on_bn_date_old_clicked();
    on_bn_date_new_clicked();
    ui->ln_smo_oms_chif->setText(data_app.smo_oms_chif);
}

print_monitoring_pers_by_date_wnd::~print_monitoring_pers_by_date_wnd() {
    delete ui;
}

void print_monitoring_pers_by_date_wnd::on_bn_cansel_clicked() {
    reject();
}

void print_monitoring_pers_by_date_wnd::on_bn_date_old_mon_clicked() {
    ui->date_old->setDate(QDate::currentDate().addMonths(-1).addDays(-1));
}

void print_monitoring_pers_by_date_wnd::on_bn_date_old_clicked() {
    ui->date_old->setDate(QDate::currentDate().addDays(-8));
}

void print_monitoring_pers_by_date_wnd::on_bn_date_new_clicked() {
    ui->date_new->setDate(QDate::currentDate().addDays(-1));
}

QString print_monitoring_pers_by_date_wnd::month_to_str(QDate date) {
    QString res;
    switch (date.month()) {
    case 1:
        res += "Январь ";
        break;
    case 2:
        res += "Февраль ";
        break;
    case 3:
        res += "Март ";
        break;
    case 4:
        res += "Апрель ";
        break;
    case 5:
        res += "Май ";
        break;
    case 6:
        res += "Июнь ";
        break;
    case 7:
        res += "Июль ";
        break;
    case 8:
        res += "Август ";
        break;
    case 9:
        res += "Сентябрь ";
        break;
    case 10:
        res += "Октябрь ";
        break;
    case 11:
        res += "Ноябрь ";
        break;
    case 12:
        res += "Декабрь ";
        break;
    default:
        break;
    }
    return res;
}

void print_monitoring_pers_by_date_wnd::on_bn_edit_clicked(bool checked) {
    if (checked) {
        ui->bn_edit->setText("Сохранить");

        /*ui->lab_date_old->setEnabled(true);
        ui->date_old->setEnabled(true);
        ui->bn_date_old->setEnabled(true);
        ui->lab_date_new->setEnabled(true);
        ui->date_new->setEnabled(true);
        ui->bn_date_new->setEnabled(true);*/
        ui->lab_smo_oms_chif->setEnabled(true);
        ui->ln_smo_oms_chif->setEnabled(true);

        ui->bn_print->setEnabled(false);
    } else {
        data_app.smo_director = ui->ln_smo_oms_chif->text();
        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Сохранить сделанные изменения в настройках программы?\n\n"
                                  "В случае согласия новые данные будут сохранены и останутся доступны после перезагрузки программы.\n"
                                  "В случае отказа эти изменения будут доступны только до первого перезапуска программы.",
                                  QMessageBox::Yes|QMessageBox::No,
                                  QMessageBox::No)==QMessageBox::Yes)
        {
            // [req_ch_smo]
            settings.beginGroup("smo");
            settings.setValue("smo_oms_chif", data_app.smo_oms_chif.toUtf8());
            settings.sync();
        }
        ui->bn_edit->setText("Изменить");

        /*ui->lab_date_old->setEnabled(false);
        ui->date_old->setEnabled(false);
        ui->bn_date_old->setEnabled(false);
        ui->lab_date_new->setEnabled(false);
        ui->date_new->setEnabled(false);
        ui->bn_date_new->setEnabled(false);*/
        ui->lab_smo_oms_chif->setEnabled(false);
        ui->ln_smo_oms_chif->setEnabled(false);

        ui->bn_print->setEnabled(true);
    }
}

#include "qt_windows.h"
#include "qwindowdefs_win.h"
#include <shellapi.h>

void print_monitoring_pers_by_date_wnd::on_bn_print_clicked() {
    ui->bn_print->setEnabled(false);
    this->setCursor(Qt::WaitCursor);

    db.transaction();

    QDate date_old = ui->date_old->date();
    QDate date_new = ui->date_new->date();

    QString period = "";
    if (date_old==date_new) {
        period += " " + date_old.toString("dd.MM.yyyy");
    } else if ( date_old.day()==1 && date_new.day()==1 &&
                date_old.daysTo(date_new)>27 &&
                date_old.daysTo(date_new)<33 ) {
        period += " " + month_to_str(date_old) + " " + date_old.toString("yyyy") + " г.";
    } else {
        period += " " + date_old.toString("dd.MM.yyyy") + " - " + date_new.toString("dd.MM.yyyy");
    }

    QString sql;

    QSqlQuery *query = new QSqlQuery(db);
    if (!mySQL.exec(this, sql, "Реестр ВС за период", *query, true, db, data_app)) {
        delete query;
        db.rollback();
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    int q_n = query->size();

    // распаковка шаблона
    QString rep_folder = data_app.path_reports;
    // распаковка шаблона
    QString tmp_folder = data_app.path_out + "_REPORTS_/temp/";
    QString rep_template = rep_folder + "rep_vs_reester_by_date.ods";

    if (!QFile::exists(rep_template)) {
        QMessageBox::warning(this,
                             "Шаблон не найден",
                             "Не найден шаблон реестра ВС: \n" + rep_template +
                             "\n\nОперация прервана.");
        db.rollback();
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
        db.rollback();
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
        db.rollback();
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
        db.rollback();
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
        db.rollback();
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    // выбор шаблона
    int pos0 = s_content.indexOf("<table:table-row table:style-name=\"ro7\">", 0);
    int pos1 = s_content.indexOf("</table:table-row>", pos0) + 18;
    QString s_row = s_content.mid(pos0, pos1-pos0);

    // правка полей контента шаблона
    QString s_content_new = s_content.left(pos0);



    int n = 0;
    while (query->next()) {
        n++;

        QDate d_date     = query->value( 2).toDate();
        QString s_fio    = query->value( 3).toString() + ", " + query->value(4).toDate().toString("dd.MM.yyyy");
        QString s_vs_num = query->value( 5).toString();

        QString s_row_new = s_row;
        s_row_new = s_row_new.
                replace("#N#",      QString::number(n)).
                replace("#DATE#",   d_date.toString("dd.MM.yyyy")).
                replace("#FIO#",    s_fio).
                replace("#VS_NUM#", s_vs_num);
        s_content_new += s_row_new;
    }
    s_content_new += s_content.right(s_content.size()-pos1);

    delete query;

    // сохранение контента шаблона
    QFile f_content_save(tmp_folder + "/content.xml");
    f_content_save.open(QIODevice::WriteOnly);
    f_content_save.write(s_content_new.toUtf8());
    f_content_save.close();


    // ===========================
    // архивация шаблона
    // ===========================
    QString fname_res = data_app.path_out + "_REPORTS_/" +
            QDate::currentDate().toString("yyyy-MM-dd") + "  -  " + data_app.smo_short + " - " + data_app.filial_name + "  -  rep_vs_reester_by_date - " + /*ui->combo_pol_v->currentText() + " - " +*/ date_old.toString("yyyy-MM-dd") + (date_old<date_new ? (" - " + date_new.toString("yyyy-MM-dd")) : "") + ".ods";

    QFile::remove(fname_res);
    while (QFile::exists(fname_res)) {
        if (QMessageBox::warning(this,
                                 "Ошибка сохранения реестра ВС ",
                                 "Не удалось сохранить реестр ВС: \n" + rep_template +
                                 "\n\nво временную папку\n" + fname_res +
                                 "\n\nПроверьте, не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                 QMessageBox::Retry|QMessageBox::Abort,
                                 QMessageBox::Retry)==QMessageBox::Abort) {
            db.rollback();
            this->setCursor(Qt::ArrowCursor);
            ui->bn_print->setEnabled(true);
            return;
        }
        QFile::remove(fname_res);
    }
    if (!QFile::copy(rep_template, fname_res) || !QFile::exists(fname_res)) {
        QMessageBox::warning(this, "Ошибка копирования отчёта", "Не удалось скопировать реестр ВС: \n" + rep_template + "\n\nво временную папку\n" + fname_res + "\n\nОперация прервана.");
        db.rollback();
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    com = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname_res + "\" \"" + tmp_folder + "/content.xml\"";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка обновления контента",
                             "При добавлении нового контента в реестр ВС произошла непредвиденная ошибка\n\nОпреация прервана.");
    }

    // ===========================
    // собственно открытие шаблона
    // ===========================
    // открытие полученного ODT
    long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);

    db.rollback();
    this->setCursor(Qt::ArrowCursor);
    ui->bn_print->setEnabled(true);
    //close();
}

void print_monitoring_pers_by_date_wnd::on_ln_smo_oms_chif_textChanged(const QString &arg1) {
    data_app.smo_oms_chif = ui->ln_smo_oms_chif->text();
}
