#include "print_subconto_wnd.h"
#include "ui_print_subconto_wnd.h"

print_subconto_wnd::print_subconto_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    db(db),
    data_app(data_app),
    settings(settings),
    QDialog(parent),
    ui(new Ui::print_subconto_wnd)
{
    ui->setupUi(this);

    refresh_combo_pol_v();
    on_bn_date_old_clicked();
    on_bn_date_new_clicked();
    ui->ln_smo_oms_chif->setText(data_app.smo_oms_chif);    
}

void print_subconto_wnd::refresh_combo_pol_v() {
    ui->combo_pol_v->clear();
    //ui->combo_pol_v->addItem(" - все - ", 0);
    //ui->combo_pol_v->addItem("старого образца", 1);
    ui->combo_pol_v->addItem("временные свидетельства", 2);
    ui->combo_pol_v->addItem("бумажные единого образца", 3);
    ui->combo_pol_v->addItem("пластиковые единого образца", 4);

    ui->combo_pol_v->setCurrentIndex(3);
}

print_subconto_wnd::~print_subconto_wnd() {
    delete ui;
}

void print_subconto_wnd::on_bn_cansel_clicked() {
    reject();
}

void print_subconto_wnd::on_bn_date_old_mon_clicked() {
    ui->date_old->setDate(QDate::currentDate().addMonths(-1).addDays(-1));
}

void print_subconto_wnd::on_bn_date_old_clicked() {
    ui->date_old->setDate(QDate::currentDate().addDays(-8));
}

void print_subconto_wnd::on_bn_date_new_clicked() {
    ui->date_new->setDate(QDate::currentDate().addDays(-1));
}

QString print_subconto_wnd::month_to_str(QDate date) {
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

void print_subconto_wnd::on_bn_edit_clicked(bool checked) {
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

void print_subconto_wnd::on_bn_print_clicked() {
    ui->bn_print->setEnabled(false);
    this->setCursor(Qt::WaitCursor);

    db.transaction();

    QDate date_old = ui->date_old->date();
    QDate date_new = ui->date_new->date();

    QString period = ui->combo_pol_v->currentText() + " - ";
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

    if (ui->combo_pol_v->currentData().toInt()==2) {
        sql= "select pt.id, pt.point_regnum, pt.point_name, "
             "        coalesce(base0,0) as base0, coalesce(sub0,0) as sub0, coalesce(grb0,0) as grb0, coalesce(base0,0)-coalesce(sub0,0)-coalesce(grb0,0) as CNT0, "
             "        coalesce(base,0)  as base,  coalesce(sub,0)  as sub,  coalesce(grb,0)  as grb,  coalesce(base0,0)-coalesce(sub0,0)-coalesce(grb0,0) + coalesce(base,0) -coalesce(sub,0) -coalesce(grb,0)  as CNT "
             "   from  points pt "
             "     left join "
             "     ( /* число бланков ВС, переданных на ПВП до #DATE0# */ "
             "         select pt.id as id_pt, pt.point_name, sum(i.vs_count) as base0 "
             "           from vs_intervals_pt i "
             "                join points pt on(i.id_point=pt.id  "
             "                       and i.date_open<'#DATE0#') "
             "          group by pt.id, pt.point_name "
             "          order by pt.id "
             "         ) q_base0 on(pt.id=q_base0.id_pt) "
             "     left join "
             "     ( /* число бланков ВС, использованных на ПВП до #DATE0# */ "
             "         select pt.id as id_pt, pt.point_name, count(*) as sub0 "
             "           from polises p "
             "                join points pt on(p._id_last_point=pt.id and p.pol_v=2 "
             "                       and p.date_get2hand<'#DATE0#') "
             "          group by pt.id, pt.point_name "
             "          order by pt.id "
             "     ) q_sub0 on(pt.id=q_sub0.id_pt) "
             "     left join "
             "     ( /* число бланков ВС, испорченных на ПВП до #DATE0# */ "
             "         select pt.id as id_pt, pt.point_name, count(*) as grb0 "
             "           from points pt "
             "                join blanks_vs bv on(pt.id=bv.id_point and bv.status=-1 "
             "                       and bv.date_add<'#DATE0#') "
             "          group by pt.id, pt.point_name "
             "          order by pt.id "
             "     ) q_grb0 on(pt.id=q_grb0.id_pt) "

             "     left join "
             "     ( /* число бланков ВС, переданных на ПВП с #DATE0# по #DATE1# */ "
             "         select pt.id as id_pt, pt.point_name, sum(i.vs_count) as base "
             "           from vs_intervals_pt i "
             "                join points pt on(i.id_point=pt.id "
             "                       and i.date_open>='#DATE0#' and i.date_open<='#DATE1#') "
             "          group by pt.id, pt.point_name "
             "          order by pt.id "
             "     ) q_base on(pt.id=q_base.id_pt) "
             "     left join "
             "     ( /* число бланков ВС, использованных на ПВП с #DATE0# по #DATE1# */ "
             "         select pt.id as id_pt, pt.point_name, count(*) as sub "
             "           from polises p "
             "                join points pt on(p._id_last_point=pt.id and p.pol_v=2 "
             "                       and p.date_get2hand>='#DATE0#' and p.date_get2hand<='#DATE1#') "
             "          group by pt.id, pt.point_name "
             "          order by pt.id "
             "     ) q_sub on(pt.id=q_sub.id_pt) "
             "     left join "
             "     ( /* число бланков ВС, испорченных на ПВП с #DATE0# по #DATE1# */ "
             "         select pt.id as id_pt, pt.point_name, count(*) as grb "
             "           from points pt "
             "                join blanks_vs bv on(pt.id=bv.id_point and bv.status=-1 "
             "                       and bv.date_add>='#DATE0#' and bv.date_add<='#DATE1#') "
             "          group by pt.id, pt.point_name "
             "          order by pt.id "
             "     ) q_grb on(pt.id=q_grb.id_pt) "

             " where pt.point_regnum<>'000' "
             " order by pt.point_regnum "
             " ; ";
        sql = sql.replace("#DATE0#", date_old.toString("yyyy-MM-dd"));
        sql = sql.replace("#DATE1#", date_new.toString("yyyy-MM-dd"));
    }
    else if (ui->combo_pol_v->currentData().toInt()==3) {
        sql= "select pt.id, pt.point_regnum, pt.point_name, "
             "        q_deb0.deb0, q_cred0.cred0, 0 as grb0, coalesce(q_deb0.deb0, 0) - coalesce(q_cred0.cred0, 0) as cnt0, "
             "        q_deb.deb,   q_cred.cred, 0 as grb, coalesce(q_deb0.deb0, 0) - coalesce(q_cred0.cred0, 0) + coalesce(q_deb.deb, 0) - coalesce(q_cred.cred, 0) as cnt "
             "   from  points pt "
             "     left join  "
             "     ( /* число бланков ЕНП, отсканированных для ПВП до #DATE0# */ "
             "         select pt.id as id_pt, pt.point_name, count(b.id) as deb0 "
             "           from blanks_pol b "
             "                join polises p on(p.id=b.id_polis) "
             "                join points pt on(p._id_last_point=pt.id  "
             "                       and b.date_add<'#DATE0#') "
             "          where b.status<>4 "
             "          group by pt.id, pt.point_name "
             "          order by pt.id "
             "     ) q_deb0 on(pt.id=q_deb0.id_pt) "
             "     left join "
             "     ( /* число бланков ЕНП, выданных на ПВП до #DATE0# */ "
             "         select pt.id as id_pt, pt.point_name, count(*) as cred0 "
             "           from blanks_pol b "
             "                left join polises p on(p.id=b.id_polis) "
             "                join points pt on(p._id_last_point=pt.id and p.pol_v=3 "
             "                       and p.date_get2hand<'#DATE0#') "
             "          where b.status<>4 "
             "          group by pt.id, pt.point_name "
             "          order by pt.id "
             "     ) q_cred0 on(pt.id=q_cred0.id_pt) "

             "     left join "
             "     ( /* число бланков ЕНП, отсканированных для ПВП с #DATE0# по #DATE1# */ "
             "          select pt.id as id_pt, pt.point_name, count(b.id) as deb "
             "           from blanks_pol b "
             "                join polises p on(p.id=b.id_polis) "
             "                join points pt on(p._id_last_point=pt.id  "
             "                       and b.date_add>='#DATE0#' and b.date_add<='#DATE1#') "
             "          where b.status<>4 "
             "          group by pt.id, pt.point_name "
             "          order by pt.id "
             "     ) q_deb on(pt.id=q_deb.id_pt) "
             "     left join "
             "     ( /* число бланков ЕНП, выданных на ПВП с #DATE0# по #DATE1# */ "
             "         select pt.id as id_pt, pt.point_name, count(*) as cred "
             "           from blanks_pol b "
             "                left join polises p on(p.id=b.id_polis) "
             "                join points pt on(p._id_last_point=pt.id and p.pol_v=3 "
             "                       and p.date_get2hand>='#DATE0#' and p.date_get2hand<='#DATE1#') "
             "          where b.status<>4 "
             "          group by pt.id, pt.point_name "
             "          order by pt.id "
             "     ) q_cred on(pt.id=q_cred.id_pt) "

             " where pt.point_regnum<>'000' "
             " order by pt.point_regnum "
             " ; ";
        sql = sql.replace("#DATE0#", date_old.toString("yyyy-MM-dd"));
        sql = sql.replace("#DATE1#", date_new.toString("yyyy-MM-dd"));
    } else {
        QMessageBox::warning(this, "Неизвестный тип полиса!",
                             "Тип полиса <" + ui->combo_pol_v->currentText() + "> не поддерживается!");
    }




    QSqlQuery *query = new QSqlQuery(db);
    if (!mySQL.exec(this, sql, "Оборотно-сальдовая ведемость", *query, true, db, data_app)) {
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
    QString rep_template = rep_folder + "rep_subconto.ods";

    if (!QFile::exists(rep_template)) {
        QMessageBox::warning(this,
                             "Шаблон не найден",
                             "Не найден шаблон оборотно-сальдовой ведомости: \n" + rep_template +
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
    s_content_new = s_content_new.replace("#COUNT#", ui->line_count->text()).replace("#PERIOD#", period);



    int n = 0;
    int sum_base0= 0;
    int sum_sub0 = 0;
    int sum_grb0 = 0;
    int sum_cnt0 = 0;
    int sum_base = 0;
    int sum_sub  = 0;
    int sum_grb  = 0;
    int sum_cnt  = 0;

    while (query->next()) {
        n++;

        QString name = query->value( 2).toString();
        int base0= query->value( 3).toInt();
        int sub0 = query->value( 4).toInt();
        int grb0 = query->value( 5).toInt();
        int cnt0 = query->value( 6).toInt();
        int base = query->value( 7).toInt();
        int sub  = query->value( 8).toInt();
        int grb  = query->value( 9).toInt();
        int cnt  = query->value(10).toInt();

        sum_base0+= base0;
        sum_sub0 += sub0;
        sum_grb0 += grb0;
        sum_cnt0 += cnt0;
        sum_base += base;
        sum_sub  += sub;
        sum_grb  += grb;
        sum_cnt  += cnt;

        QString s_row_new = s_row;
        s_row_new = s_row_new.
                replace("#N#",      QString::number(n)).
                replace("#NAME#",   name).
                replace("#DEB0#",   QString::number(cnt0)).
                replace("#CRED0#",  "0").
                replace("#DEB#",    QString::number(base)).
                replace("#CRED#",   QString::number(sub+grb)).
                replace("#DEB1#",   QString::number(cnt)).
                replace("#CRED1#",  "0");
        s_content_new += s_row_new;
    }
    QString s_end = s_content.right(s_content.size()-pos1);
    s_content_new += s_end.
            replace("#S_DEB0#",   QString::number(sum_cnt0)).
            replace("#S_CRED0#",  "0").
            replace("#S_DEB#",    QString::number(sum_base)).
            replace("#S_CRED#",   QString::number(sum_sub+sum_grb)).
            replace("#S_DEB1#",   QString::number(sum_cnt)).
            replace("#S_CRED1#",  "0");

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
            QDate::currentDate().toString("yyyy-MM-dd") + "  -  " + data_app.smo_short + " - " + data_app.filial_name + "  -  rep_subconto(" + QString::number(ui->combo_pol_v->currentData().toInt()) + ") - " + ui->combo_pol_v->currentText() + " - " + date_old.toString("yyyy-MM-dd") + (date_old<date_new ? (" - " + date_new.toString("yyyy-MM-dd")) : "") + ".ods";

    QFile::remove(fname_res);
    while (QFile::exists(fname_res)) {
        if (QMessageBox::warning(this,
                                 "Ошибка сохранения оборотно-сальдовой ведомости по счёту " + ui->line_count->text(),
                                 "Не удалось сохранить оборотно-сальдовую ведомость: \n" + rep_template +
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
        QMessageBox::warning(this, "Ошибка копирования отчёта", "Не удалось скопировать оборотно-сальдовую ведомость: \n" + rep_template + "\n\nво временную папку\n" + fname_res + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    com = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname_res + "\" \"" + tmp_folder + "/content.xml\"";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка обновления контента",
                             "При добавлении нового контента в оборотно-сальдовую ведомость произошла непредвиденная ошибка\n\nОпреация прервана.");
    }

    // ===========================
    // собственно открытие шаблона
    // ===========================
    // открытие полученного ODT
    long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);

    //close();
    this->setCursor(Qt::ArrowCursor);
    ui->bn_print->setEnabled(true);
}

void print_subconto_wnd::on_ln_smo_oms_chif_textChanged(const QString &arg1) {
    data_app.smo_oms_chif = ui->ln_smo_oms_chif->text();
}
