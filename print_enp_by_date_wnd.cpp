#include "print_enp_by_date_wnd.h"
#include "ui_print_enp_by_date_wnd.h"

print_enp_by_date_wnd::print_enp_by_date_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    db(db),
    data_app(data_app),
    settings(settings),
    QDialog(parent),
    ui(new Ui::print_enp_by_date_wnd)
{
    ui->setupUi(this);

    on_bn_date_old_clicked();
    on_bn_date_new_clicked();
    ui->ln_smo_oms_chif->setText(data_app.smo_oms_chif);
}

print_enp_by_date_wnd::~print_enp_by_date_wnd() {
    delete ui;
}

void print_enp_by_date_wnd::on_bn_cansel_clicked() {
    reject();
}

void print_enp_by_date_wnd::on_bn_date_old_clicked() {
    ui->date_old->setDate(QDate::currentDate().addMonths(-1).addDays(-1));
}

void print_enp_by_date_wnd::on_bn_date_new_clicked() {
    ui->date_new->setDate(QDate::currentDate().addDays(-1));
}

QString print_enp_by_date_wnd::month_to_str(QDate date) {
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

void print_enp_by_date_wnd::on_bn_edit_clicked(bool checked) {
    if (checked) {
        ui->bn_edit->setText("Сохранить");

        ui->lab_date_old->setEnabled(true);
        ui->date_old->setEnabled(true);
        ui->bn_date_old->setEnabled(true);
        ui->lab_date_new->setEnabled(true);
        ui->date_new->setEnabled(true);
        ui->bn_date_new->setEnabled(true);
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

        ui->lab_date_old->setEnabled(false);
        ui->date_old->setEnabled(false);
        ui->bn_date_old->setEnabled(false);
        ui->lab_date_new->setEnabled(false);
        ui->date_new->setEnabled(false);
        ui->bn_date_new->setEnabled(false);
        ui->lab_smo_oms_chif->setEnabled(false);
        ui->ln_smo_oms_chif->setEnabled(false);

        ui->bn_print->setEnabled(true);
    }
}

#include "qt_windows.h"
#include "qwindowdefs_win.h"
#include <shellapi.h>

void print_enp_by_date_wnd::on_bn_print_clicked() {
    ui->bn_print->setEnabled(false);
    this->setCursor(Qt::WaitCursor);

    db.transaction();

    QDate date_old = ui->date_old->date();
    QDate date_new = ui->date_new->date();

    QString sql;
    sql = "select p.id, p.point_name, q_old.cnt_old, q_non.cnt_non, q_add.cnt_add, q_paper.cnt_paper, q_card.cnt_card, q_other.cnt_other, q_new.cnt_new "
          "  from  points p  "
          "    left join "
          "     ( select id_point, count(*) as cnt_old "
          "       from points p left join  blanks_pol b on(b.id_point=p.id)  "
          "       where b.date_add<='" + date_old.toString("dd.MM.yyyy") + "'  "
          "           and b.status not in(0,-1) "
          "       group by b.id_point  "
          "     ) q_old on (q_old.id_point=p.id) "
          "    left join "
          "     ( select id_point, count(*) as cnt_non "
          "       from points p left join  blanks_pol b on(b.id_point=p.id)  "
          "       where b.date_add>'" + date_old.toString("dd.MM.yyyy") + "' and b.date_add<='" + date_new.toString("dd.MM.yyyy") + "'  "
          "           and b.status=0  "
          "       group by b.id_point  "
          "     ) q_non on (q_non.id_point=p.id) "
          "    left join "
          "     ( select id_point, count(*) as cnt_add "
          "       from points p left join  blanks_pol b on(b.id_point=p.id) "
          "       where b.date_add>'" + date_old.toString("dd.MM.yyyy") + "' and b.date_add<='" + date_new.toString("dd.MM.yyyy") + "'  "
          "           and b.status=1 "
          "       group by b.id_point  "
          "     ) q_add on (q_add.id_point=p.id) "
          "    left join "
          "     ( select id_point, count(*) as cnt_paper "
          "       from points p left join  blanks_pol b on(b.id_point=p.id)  "
          "       where b.date_add>'" + date_old.toString("dd.MM.yyyy") + "' and b.date_add<='" + date_new.toString("dd.MM.yyyy") + "'  "
          "           and b.status=1 "
          "           and b.f_polis=1 "
          "       group by b.id_point  "
          "     ) q_paper on (q_paper.id_point=p.id) "
          "    left join "
          "     ( select id_point, count(*) as cnt_card "
          "       from points p left join  blanks_pol b on(b.id_point=p.id)  "
          "       where b.date_add>'" + date_old.toString("dd.MM.yyyy") + "' and b.date_add<='" + date_new.toString("dd.MM.yyyy") + "'  "
          "           and b.status=1 "
          "           and b.f_polis=2 "
          "       group by b.id_point  "
          "     ) q_card on (q_card.id_point=p.id) "
          "    left join "
          "     ( select id_point, count(*) as cnt_other "
          "       from points p left join  blanks_pol b on(b.id_point=p.id)  "
          "       where b.date_add>'" + date_old.toString("dd.MM.yyyy") + "' and b.date_add<='" + date_new.toString("dd.MM.yyyy") + "'  "
          "           and b.status=4 "
          "       group by b.id_point  "
          "     ) q_other on (q_other.id_point=p.id) "
          "    left join "
          "     ( select id_point, count(*) as cnt_new "
          "       from points p left join  blanks_pol b on(b.id_point=p.id)  "
          "       where b.date_add<='" + date_new.toString("dd.MM.yyyy") + "'  "
          //"           and b.status>0  "
          "           and b.status not in(0,-1)  "
          "       group by b.id_point  "
          "     ) q_new on (q_new.id_point=p.id) ; ";

    QSqlQuery *query = new QSqlQuery(db);
    if (!mySQL.exec(this, sql, "Отчёт по бланкам ВС", *query, true, db, data_app)) {
        delete query;
        db.rollback();
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    int q_n = query->size();

    // распаковка шаблона реестра
    QString fname_template = data_app.path_reports + "rep_enp_by_date.ods";
    QString temp_folder = data_app.path_temp + "rep_enp_by_date";
    if (!QFile::exists(fname_template)) {
        QMessageBox::warning(this,
                             "Шаблон не найден",
                             "Не найден шаблон печатной формы: \n" + fname_template +
                             "\n\nОперация прервана.");
        db.rollback();
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
        db.rollback();
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
        db.rollback();
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
        db.rollback();
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
        db.rollback();
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    // выбор шаблона
    int pos0 = s_content.indexOf("<table:table-row table:style-name=\"ro4\">", 0);
    int pos1 = s_content.indexOf("</table:table-row>", pos0) + 18;
    QString s_row = s_content.mid(pos0, pos1-pos0);

    // правка полей контента шаблона
    QString s_content_new = s_content.left(pos0);

    int n = 0;
    int sum_old = 0;
    int sum_non = 0;
    int sum_add = 0;
    int sum_paper = 0;
    int sum_card = 0;
    int sum_other = 0;
    int sum_new = 0;
    while (query->next()) {
        n++;

        int cnt_old   = query->value(2).toInt();
        int cnt_non   = query->value(3).toInt();
        int cnt_add   = query->value(4).toInt();
        int cnt_paper = query->value(5).toInt();
        int cnt_card  = query->value(6).toInt();
        int cnt_other = query->value(7).toInt();
        int cnt_new   = query->value(8).toInt();

        sum_old   += cnt_old;
        sum_non   += cnt_non;
        sum_add   += cnt_add;
        sum_paper += cnt_paper;
        sum_card  += cnt_card;
        sum_other += cnt_other;
        sum_new   += cnt_new;

        QString s_row_new = s_row;
        s_row_new = s_row_new.
                replace("#POINT_NAME#", query->value(1).toString()).
                replace("#CNT_OLD#",    QString::number(cnt_old)).
                replace("#CNT_NON#",    QString::number(cnt_non)).
                replace("#CNT_ADD#",    QString::number(cnt_add)).
                replace("#CNT_PAPER#",  QString::number(cnt_paper)).
                replace("#CNT_CARD#",   QString::number(cnt_card)).
                replace("#CNT_OTHER#",  QString::number(cnt_other)).
                replace("#CNT_NEW#",    QString::number(cnt_new));
        s_content_new += s_row_new;
    }
    s_content_new += s_content.right(s_content.size()-pos1);

    s_content_new = s_content_new.
            replace("#SUM_OLD#",  QString::number(sum_old)).
            replace("#SUM_NON#",  QString::number(sum_non)).
            replace("#SUM_ADD#",  QString::number(sum_add)).
            replace("#SUM_PAPER#",QString::number(sum_paper)).
            replace("#SUM_CARD#",QString::number(sum_card)).
            replace("#SUM_OTHER#",QString::number(sum_other)).
            replace("#SUM_NEW#",  QString::number(sum_new)).

            replace("#SMO_NAME#", data_app.smo_name).
            replace("#DATE_OLD#", date_old.toString("dd.MM.yyyy")).
            replace("#DATE_NEW#", date_new.toString("dd.MM.yyyy")).
            replace("#SMO_OMS_CHIF#", data_app.smo_oms_chif);

    delete query;

    // сохранение контента шаблона
    QFile f_content_save(temp_folder + "/content.xml");
    f_content_save.open(QIODevice::WriteOnly);
    f_content_save.write(s_content_new.toUtf8());
    f_content_save.close();


    // ===========================
    // архивация шаблона
    // ===========================
    QString fname_res = data_app.path_temp + "oms_by_date.ods";
    QFile::remove(fname_res);
    while (QFile::exists(fname_res)) {
        if (QMessageBox::warning(this,
                                 "Ошибка сохранения журнала ВС",
                                 "Не удалось сохранить отчёт по бланкам ВС: \n" + fname_template +
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
    if (!QFile::copy(fname_template, fname_res) || !QFile::exists(fname_res)) {
        QMessageBox::warning(this, "Ошибка копирования отчёта", "Не удалось скопировать отчёт по бланкам ВС: \n" + fname_template + "\n\nво временную папку\n" + fname_res + "\n\nОперация прервана.");
        db.rollback();
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    com = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname_res + "\" \"" + temp_folder + "/content.xml\"";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка обновления контента",
                             "При добавлении нового контента в отчёт по бланкам ВС произошла непредвиденная ошибка\n\nОпреация прервана.");
    }

    // ===========================
    // собственно открытие шаблона
    // ===========================
    // открытие полученного ODT
    long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);

    //close();
    db.rollback();
    this->setCursor(Qt::ArrowCursor);
    ui->bn_print->setEnabled(true);
}

void print_enp_by_date_wnd::on_ln_smo_oms_chif_textChanged(const QString &arg1) {
    data_app.smo_oms_chif = ui->ln_smo_oms_chif->text();
}
