#include "print_vs_by_date_wnd.h"
#include "ui_print_vs_by_date_wnd.h"

print_vs_by_date_wnd::print_vs_by_date_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    db(db),
    data_app(data_app),
    settings(settings),
    QDialog(parent),
    ui(new Ui::print_vs_by_date_wnd)
{
    ui->setupUi(this);

    on_bn_date_old_clicked();
    on_bn_date_new_clicked();
    ui->ln_smo_oms_chif->setText(data_app.smo_oms_chif);
}

print_vs_by_date_wnd::~print_vs_by_date_wnd() {
    delete ui;
}

void print_vs_by_date_wnd::on_bn_cansel_clicked() {
    reject();
}

void print_vs_by_date_wnd::on_bn_date_old_mon_clicked() {
    ui->date_old->setDate(QDate::currentDate().addMonths(-1).addDays(-1));
}

void print_vs_by_date_wnd::on_bn_date_old_clicked() {
    ui->date_old->setDate(QDate::currentDate().addDays(-8));
}

void print_vs_by_date_wnd::on_bn_date_new_clicked() {
    ui->date_new->setDate(QDate::currentDate().addDays(-1));
}

QString print_vs_by_date_wnd::month_to_str(QDate date) {
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

void print_vs_by_date_wnd::on_bn_edit_clicked(bool checked) {
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

void print_vs_by_date_wnd::on_bn_print_clicked() {
    ui->bn_print->setEnabled(false);
    this->setCursor(Qt::WaitCursor);

    db.transaction();

    QDate date_old = ui->date_old->date();
    QDate date_new = ui->date_new->date();

    QString sql;
    sql= "select p.id, p.point_name, "
         "       qb_old.cntb_free as cntb_old, qb_add.cntb_int as cntb_add, qb_add.cntb_start as cntb_used, qb_add.cntb_spoil as cntb_spoiled, qb_new.cntb_free as cntb_new, "
         "       q_vs.cnt, q_vs2.cnt, q_pol.cnt, q_other.cnt, (q_vs.cnt + q_pol.cnt - q_vs2.cnt + q_other.cnt) as cnt_sum "

         "  from  points p  "
         "    left join "
         "     ( select p.id as id_point, coalesce(q_int.cnt_int, 0)-coalesce(q_start.cnt_start, 0)-coalesce(q_spoil.cnt_spoil, 0) as cntb_free "
         "       from  points p  "
         "       left join  "
         "        ( select id_point, sum(i.vs_count) as cnt_int "
         "          from points p left join vs_intervals_pt i on(i.id_point=p.id) "
         "          where i.date_open<'" + date_old.toString("yyyy-MM-dd") + "' "
         "          group by i.id_point "
         "        ) q_int on(p.id=q_int.id_point) "
         "       left join  "
         "        ( select id_point, count(*) as cnt_start "
         "          from points p left join  blanks_vs b on(b.id_point=p.id) "
         "          where b.date_add<'" + date_old.toString("yyyy-MM-dd") + "' "
         "                and b.status>0 "
         "          group by b.id_point "
         "        ) q_start on(p.id=q_start.id_point) "
         "       left join  "
         "        ( select id_point, count(*) as cnt_spoil "
         "          from points p left join  blanks_vs b on(b.id_point=p.id) "
         "          where b.date_add<'" + date_old.toString("yyyy-MM-dd") + "' "
         "            and b.status<0 "
         "          group by b.id_point "
         "        ) q_spoil on(p.id=q_spoil.id_point) "
         "     ) qb_old on (p.id=qb_old.id_point) "
         "    left join "
         "     ( select p.id as id_point, q_int.cntb_int, q_start.cntb_start, q_spoil.cntb_spoil "
         "       from  points p  "
         "       left join  "
         "        ( select id_point, sum(i.vs_count) as cntb_int "
         "          from points p left join vs_intervals_pt i on(i.id_point=p.id) "
         "          where i.date_open>='" + date_old.toString("yyyy-MM-dd") + "' and i.date_open<='" + date_new.toString("yyyy-MM-dd") + "' "
         "          group by i.id_point "
         "        ) q_int on(p.id=q_int.id_point) "
         "       left join  "
         "        ( select id_point, count(*) as cntb_start "
         "          from points p left join  blanks_vs b on(b.id_point=p.id) "
         "          where b.date_add>='" + date_old.toString("yyyy-MM-dd") + "' and b.date_add<='" + date_new.toString("yyyy-MM-dd") + "' "
         "            and b.status>0 "
         "          group by b.id_point "
         "        ) q_start on(p.id=q_start.id_point) "
         "       left join  "
         "        ( select id_point, count(*) as cntb_spoil "
         "          from points p left join  blanks_vs b on(b.id_point=p.id) "
         "          where b.date_add>='" + date_old.toString("yyyy-MM-dd") + "' and b.date_add<='" + date_new.toString("yyyy-MM-dd") + "' "
         "            and b.status<0 "
         "          group by b.id_point "
         "        ) q_spoil on(p.id=q_spoil.id_point) "
         "     ) qb_add on (p.id=qb_add.id_point) "
         "    left join "
         "     ( select p.id as id_point, coalesce(q_int.cnt_int, 0)-coalesce(q_start.cnt_start, 0)-coalesce(q_spoil.cnt_spoil, 0) as cntb_free "
         "       from  points p  "
         "       left join  "
         "        ( select id_point, sum(i.vs_count) as cnt_int "
         "          from points p left join vs_intervals_pt i on(i.id_point=p.id) "
         "          where i.date_open<='" + date_new.toString("yyyy-MM-dd") + "' "
         "          group by i.id_point "
         "        ) q_int on(p.id=q_int.id_point) "
         "       left join  "
         "        ( select id_point, count(*) as cnt_start "
         "          from points p left join  blanks_vs b on(b.id_point=p.id) "
         "          where b.date_add<='" + date_new.toString("yyyy-MM-dd") + "' "
         "            and b.status>0 "
         "          group by b.id_point "
         "         ) q_start on(p.id=q_start.id_point) "
         "       left join  "
         "        ( select id_point, count(*) as cnt_spoil "
         "          from points p left join  blanks_vs b on(b.id_point=p.id) "
         "          where b.date_add<='" + date_new.toString("yyyy-MM-dd") + "' "
         "            and b.status<0 "
         "          group by b.id_point "
         "        ) q_spoil on(p.id=q_spoil.id_point) "
         "     ) qb_new on (p.id=qb_new.id_point) "

      // ВС выдано
         "   left join "
         "    ( select n.id as id_point, /*n.point_name, bv.status,*/ count(*) as cnt "
         "        from persons e "
         "             left join "
         "              ( select max(p.id) as id_polis, p.id_person "
         "                from polises p "
         "                     join blanks_vs  bv on(p.vs_num=bv.vs_num) "
         "                where date_begin>='" + date_old.toString("yyyy-MM-dd") + "' "
         "                      and date_begin<='" + date_new.toString("yyyy-MM-dd") + "' "
         "                group by p.id_person "
         "              ) qp on(e.id=qp.id_person) "
         "             join polises p on (p.id=qp.id_polis) "
         "             join points n on(n.id=p._id_last_point) "
         "        where (select * from st_person(e.id,'" + date_old.toString("yyyy-MM-dd") + "'))>-100 "
         "      group by n.id/*, n.point_name, bv.status*/ "
         "    ) q_vs on(q_vs.id_point=p.id) "

      // ВС выдано и уже изьято
         "   left join "
         "    ( select n.id as id_point, /*n.point_name, bv.status,*/ count(*) as cnt "
         "        from persons e "
         "             join "
         "              ( select max(p.id) as id_polis, p.id_person "
         "                  from polises p "
         "                       join blanks_vs  bv on(p.vs_num=bv.vs_num) "
         "                 where date_begin>='" + date_old.toString("yyyy-MM-dd") + "' "
         "                       and date_begin<='" + date_new.toString("yyyy-MM-dd") + "' "
         "                 group by p.id_person "
         "              ) q_vs on(e.id=q_vs.id_person) "
         "             join "
         "              ( select max(p.id) as id_polis, p.id_person "
         "                  from polises p "
         "                       join blanks_pol bp on(p.pol_num=bp.pol_num and ((p.pol_ser is null and bp.pol_ser is null) or p.pol_ser=bp.pol_ser) ) "
         "                 where p.date_begin>='" + date_old.toString("yyyy-MM-dd") + "' "
         "                       and p.date_begin<='" + date_new.toString("yyyy-MM-dd") + "' "
         "                       and (p.date_end  is NULL or p.date_end >'" + date_old.toString("yyyy-MM-dd") + "') "
         "                       and (p.date_stop is NULL or p.date_stop>'" + date_new.toString("yyyy-MM-dd") + "') "
         "                 group by p.id_person "
         "               ) q_pol on(q_vs.id_person=q_pol.id_person) "
         "             left join polises p on (p.id=q_vs.id_polis) "
         "             left join points n on(n.id=p._id_last_point) "
         "       where (select * from st_person(e.id,'" + date_old.toString("yyyy-MM-dd") + "'))>-100 "
         "       group by n.id/*, n.point_name, bv.status*/ "
         //"       order by n.id, n.point_name, bv.status "
         "    ) q_vs2 on(q_vs2.id_point=p.id) "

      // полисов выдано
         "   left join "
         "    ( select n.id as id_point, /*n.point_name, bp.status,*/ count(*) as cnt "
         "        from persons e "
         "             left join "
         "              ( select max(p.id) as id_polis, p.id_person "
         "                  from polises p "
         "                 where p.date_begin>='" + date_old.toString("yyyy-MM-dd") + "' "
         "                       and p.date_begin<='" + date_new.toString("yyyy-MM-dd") + "' "
         "                       and (p.date_end  is NULL or p.date_end >'" + date_new.toString("yyyy-MM-dd") + "') "
         "                       and (p.date_stop is NULL or p.date_stop>'" + date_new.toString("yyyy-MM-dd") + "') "
         "                 group by p.id_person "
         "              ) qp on(e.id=qp.id_person) "
         "             left join polises p on (p.id=qp.id_polis) "
         "             left join blanks_pol bp on(p.pol_ser=bp.pol_ser and bp.pol_num=p.pol_num) "
         "             left join points n on(n.id=p._id_last_point) "
         "       where p.id is not NULL "
         "         and bp.status=1 "
         "         and (select * from st_person(e.id,'" + date_old.toString("yyyy-MM-dd") + "'))>-100 "
         "       group by n.id/*, n.point_name, bp.status*/ "
         //"      order by n.id, n.point_name, bp.status "
         "    ) q_pol on(q_pol.id_point=p.id) "

      // перестраховано
         "   left join "
         "    ( select n.id as id_point, /*n.point_name, bp.status,*/ count(*) as cnt "
         "        from persons e "
         "             left join "
         "              ( select max(p.id) as id_polis, p.id_person "
         "                  from polises p "
         "                 where p.date_begin>='" + date_old.toString("yyyy-MM-dd") + "' "
         "                   and p.date_begin<='" + date_new.toString("yyyy-MM-dd") + "' "
         "                   and (p.date_end  is NULL or p.date_end >'" + date_new.toString("yyyy-MM-dd") + "') "
         "                   and (p.date_stop is NULL or p.date_stop>'" + date_new.toString("yyyy-MM-dd") + "') "
         "                 group by p.id_person "
         "              ) qp on(e.id=qp.id_person) "
         "             left join polises p on (p.id=qp.id_polis) "
         "             left join blanks_pol bp on(p.pol_ser=bp.pol_ser and bp.pol_num=p.pol_num) "
         "             left join points n on(n.id=p._id_last_point) "
         "       where p.id is not NULL "
         "         and bp.status=4 "
         "         and (select * from st_person(e.id,'" + date_old.toString("yyyy-MM-dd") + "'))>-100 "
         "       group by n.id/*, n.point_name, bp.status*/ "
         "    ) q_other on(q_other.id_point=p.id) ; ";

    QSqlQuery *query = new QSqlQuery(db);
    if (!mySQL.exec(this, sql, "Отчёт по бланкам ВС", *query, true, db, data_app)) {
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
    QString rep_template = rep_folder + "rep_vs_by_date.ods";

    if (!QFile::exists(rep_template)) {
        QMessageBox::warning(this,
                             "Шаблон не найден",
                             "Не найден шаблон печатной формы реестра выданных ВС: \n" + rep_template +
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
    int pos0 = s_content.indexOf("<table:table-row table:style-name=\"ro3\">", 0);
    int pos1 = s_content.indexOf("</table:table-row>", pos0) + 18;
    QString s_row = s_content.mid(pos0, pos1-pos0);

    // правка полей контента шаблона
    QString s_content_new = s_content.left(pos0);

    int n = 0;
    int sumb_old = 0;
    int sumb_add = 0;
    int sumb_used = 0;
    int sumb_spoiled = 0;
    int sumb_new = 0;
    //int sump_old = 0;
    int sump_other = 0;
    int sump_vs = 0;
    int sump_vs2 = 0;
    int sump_enp = 0;
    int sump_new = 0;
    while (query->next()) {
        n++;

        int cntb_old    = query->value(2).toInt();
        int cntb_add    = query->value(3).toInt();
        int cntb_used   = query->value(4).toInt();
        int cntb_spoiled= query->value(5).toInt();
        int cntb_new    = query->value(6).toInt();

        //int cntp_old    = query->value(7).toInt();

        int cntp_other  = query->value(10).toInt();
        int cntp_vs     = query->value(7).toInt();
        int cntp_vs2    = query->value(8).toInt();
        int cntp_enp    = query->value(9).toInt();
        int cntp_new    = query->value(11).toInt();

        sumb_old    += cntb_old;
        sumb_add    += cntb_add;
        sumb_used   += cntb_used;
        sumb_spoiled+= cntb_spoiled;
        sumb_new    += cntb_new;

        //sump_old    += cntp_old;
        sump_other  += cntp_other;
        sump_vs     += cntp_vs;
        sump_vs2    += cntp_vs2;
        sump_enp    += cntp_enp;
        sump_new    += cntp_new;

        QString s_row_new = s_row;
        s_row_new = s_row_new.
                replace("#POINT_NAME#", query->value(1).toString()).
                replace("#CNTB_OLD#",    QString::number(cntb_old)).
                replace("#CNTB_ADD#",    QString::number(cntb_add)).
                replace("#CNTB_USED#",   QString::number(cntb_used)).
                replace("#CNTB_SPOILED#",QString::number(cntb_spoiled)).
                replace("#CNTB_NEW#",    QString::number(cntb_new)).

                //replace("#CNTP_OLD#",  QString::number(cntp_old)).
                replace("#CNTP_OTHER#",  QString::number(cntp_other)).
                replace("#CNTP_VS#",     QString::number(cntp_vs)).
                replace("#CNTP_VS2#",    QString::number(cntp_vs2)).
                replace("#CNTP_ENP#",    QString::number(cntp_enp)).
                replace("#CNTP_NEW#",    QString::number(cntp_new));
        s_content_new += s_row_new;
    }
    s_content_new += s_content.right(s_content.size()-pos1);

    s_content_new = s_content_new.
            replace("#SUMB_OLD#",    QString::number(sumb_old)).
            replace("#SUMB_ADD#",    QString::number(sumb_add)).
            replace("#SUMB_USED#",   QString::number(sumb_used)).
            replace("#SUMB_SPOILED#",QString::number(sumb_spoiled)).
            replace("#SUMB_NEW#",    QString::number(sumb_new)).

            //replace("#SUMP_OLD#",  QString::number(sump_old)).
            replace("#SUMP_OTHER#",  QString::number(sump_other)).
            replace("#SUMP_VS#",     QString::number(sump_vs)).
            replace("#SUMP_VS2#",    QString::number(sump_vs2)).
            replace("#SUMP_ENP#",    QString::number(sump_enp)).
            replace("#SUMP_NEW#",    QString::number(sump_new)).

            replace("#SMO_NAME#",    data_app.smo_name).
            replace("#DATE_OLD#",    date_old.toString("dd.MM.yyyy")).
            replace("#DATE_NEW#",    date_new.toString("dd.MM.yyyy")).
            replace("#SMO_OMS_CHIF#",data_app.smo_oms_chif);

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
            QDate::currentDate().toString("yyyy-MM-dd") + "  -  " + data_app.smo_short + " - " + data_app.filial_name + "  -  vs_by_date - " + date_old.toString("yyyy-MM-dd") + (date_old<date_new ? ("-" + date_new.toString("yyyy-MM-dd")) : "") + ".ods";

    QFile::remove(fname_res);
    while (QFile::exists(fname_res)) {
        if (QMessageBox::warning(this,
                                 "Ошибка сохранения журнала ВС",
                                 "Не удалось сохранить отчёт по бланкам ВС: \n" + rep_template +
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
        QMessageBox::warning(this, "Ошибка копирования отчёта", "Не удалось скопировать отчёт по бланкам ВС: \n" + rep_template + "\n\nво временную папку\n" + fname_res + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    com = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname_res + "\" \"" + tmp_folder + "/content.xml\"";
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
    this->setCursor(Qt::ArrowCursor);
    ui->bn_print->setEnabled(true);
}

void print_vs_by_date_wnd::on_ln_smo_oms_chif_textChanged(const QString &arg1) {
    data_app.smo_oms_chif = ui->ln_smo_oms_chif->text();
}
