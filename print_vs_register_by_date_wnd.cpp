#include "print_vs_register_by_date_wnd.h"
#include "ui_print_vs_register_by_date_wnd.h"

print_vs_register_by_date_wnd::print_vs_register_by_date_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    db(db),
    data_app(data_app),
    settings(settings),
    QDialog(parent),
    ui(new Ui::print_vs_register_by_date_wnd)
{
    ui->setupUi(this);

    on_bn_date_old_clicked();
    on_bn_date_new_clicked();
    ui->ln_smo_oms_chif->setText(data_app.smo_oms_chif);    
    refresh_points();
    refresh_operators();
}

print_vs_register_by_date_wnd::~print_vs_register_by_date_wnd() {
    delete ui;
}

void print_vs_register_by_date_wnd::refresh_points() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка медорганизаций
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select id, point_regnum, '('||point_regnum||')  '||point_name as name "
                  " from public.points "
                  " where status=1 and point_regnum<>'000' "
                  " order by point_regnum ; ";
    mySQL.exec(this, sql, QString("Список ПВП"), *query, true, db, data_app);
    ui->combo_point->clear();
    point_regnums.clear();
    ui->combo_point->addItem(" - все ПВП - ", "нет");
    point_regnums.append("???");
    while (query->next()) {
        ui->combo_point->addItem(query->value(2).toString(), query->value(0).toInt());
        point_regnums.append(query->value(1).toString());
    }
    ui->combo_point->setCurrentIndex(0);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void print_vs_register_by_date_wnd::refresh_operators() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка операторов
    QSqlQuery *query = new QSqlQuery(db);
    QString sql;
    sql = "select o.id, o.oper_fio "
          " from operators o "
          "      left join points p on(o.id_point=p.id) "
          " where o.status=1 ";
    if (ui->combo_point->currentIndex()>0)
        sql += " and p.status=1 and p.id=" + QString::number(ui->combo_point->currentData().toInt()) + " ";
    sql += " order by o.oper_fio ; ";
    mySQL.exec(this, sql, QString("Список инспекторов"), *query, true, db, data_app);
    ui->combo_operator->clear();
    ui->combo_operator->addItem(" - все операторы - ", -1);
    while (query->next()) {
        ui->combo_operator->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_operator->setCurrentIndex(0);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void print_vs_register_by_date_wnd::on_bn_cansel_clicked() {
    reject();
}

void print_vs_register_by_date_wnd::on_bn_date_old_mon_clicked() {
    ui->date_old->setDate(QDate::currentDate().addMonths(-1).addDays(-1));
}

void print_vs_register_by_date_wnd::on_bn_date_old_clicked() {
    ui->date_old->setDate(QDate::currentDate().addDays(-8));
}

void print_vs_register_by_date_wnd::on_bn_date_new_clicked() {
    ui->date_new->setDate(QDate::currentDate().addDays(-1));
}

QString print_vs_register_by_date_wnd::month_to_str(QDate date) {
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

void print_vs_register_by_date_wnd::on_bn_edit_clicked(bool checked) {
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

void print_vs_register_by_date_wnd::on_bn_print_clicked() {
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

        sql= "select p.id, e.id, p.date_begin, coalesce(e.fam, '') || ' ' || coalesce(e.im, '') || ' ' || coalesce(e.ot, '') as fio,  coalesce(e.date_birth, '2000-01-01'), vs.vs_num, pt.point_name, op.oper_fio "
             "  from polises p "
             "       join persons e on(e.id=p.id_person) "
             "       join points pt on(pt.id=p._id_first_point) "
             "       join operators op on(op.id=p._id_first_operator) "
             "       join blanks_vs vs on(vs.id_polis=p.id) "
             " where p.pol_v=2 "
             "   and vs.status<>-1 "
             "   and p.date_begin>='#DATE0#' "
             "   and p.date_begin<='#DATE1#' ";
        if (ui->combo_point->currentIndex()>0)
             sql += "   and pt.point_regnum='" + point_regnums.at(ui->combo_point->currentIndex()) + "' ";
        if (ui->combo_operator->currentIndex()>0)
             sql += "   and p._id_first_operator=" + QString::number(ui->combo_operator->currentData().toInt()) + " ";
        sql+=" order by p.vs_num, e.fam, e.im, e.ot, e.date_birth ; ";

        sql = sql.replace("#DATE0#", date_old.toString("yyyy-MM-dd"));
        sql = sql.replace("#DATE1#", date_new.toString("yyyy-MM-dd"));


    QSqlQuery *query = new QSqlQuery(db);
    if (!mySQL.exec(this, sql, "Реестр ВС за период", *query, true, db, data_app)) {
        delete query;
        db.rollback();
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    int q_n = query->size();

    QString sq2;

        sq2= "select vs.vs_num, -1, vs.date_spent, ' - ИСПОРЧЕН - ', vs.vs_num, pt.point_name, op.oper_fio "
             "  from blanks_vs vs "
             "  left join points pt on(pt.id=vs.id_point) "
             "       join operators op on(op.id=vs.id_operator) "
             " where vs.status=-1 "
             "   and vs.date_spent>='#DATE0#' "
             "   and vs.date_spent<='#DATE1#' ";
        if (ui->combo_point->currentIndex()>0)
             sq2 += "   and pt.point_regnum='" + point_regnums.at(ui->combo_point->currentIndex()) + "' ";
        if (ui->combo_operator->currentIndex()>0)
             sq2 += "   and vs.id_operator=" + QString::number(ui->combo_operator->currentData().toInt()) + " ";
        sq2+=" order by vs.vs_num ; ";

        sq2 = sq2.replace("#DATE0#", date_old.toString("yyyy-MM-dd"));
        sq2 = sq2.replace("#DATE1#", date_new.toString("yyyy-MM-dd"));


    QSqlQuery *quer2 = new QSqlQuery(db);
    if (!mySQL.exec(this, sq2, "Реестр испорченных ВС за период", *quer2, true, db, data_app)) {
        delete quer2;
        db.rollback();
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    int q2n = quer2->size();

    // распаковка шаблона
    QString rep_folder = data_app.path_reports;
    QString tmp_folder = data_app.path_out + "_REPORTS_/temp/";
    QString rep_template = rep_folder + "rep_vs_reester_by_date.ods";

    if (!QFile::exists(rep_template)) {
        QMessageBox::warning(this,
                             "Шаблон не найден",
                             "Не найден шаблон реестра ВС: \n" + rep_template +
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
    s_content_new = s_content_new.replace("#PERIOD#", period).replace("#POINT#", ui->combo_point->currentText() + " ,  " + ui->combo_operator->currentText());



    int n = 0;
    while (query->next()) {
        n++;

        QDate d_date     = query->value( 2).toDate();
        QString s_fio    = query->value( 3).toString() + ", " + query->value(4).toDate().toString("dd.MM.yyyy");
        QString s_vs_num = query->value( 5).toString();
        QString s_point_name = query->value( 6).toString();
        QString s_oper_fio   = query->value( 7).toString();

        QString s_row_new = s_row;
        s_row_new = s_row_new.
                replace("#N#",      QString::number(n)).
                replace("#DATE#",   d_date.toString("dd.MM.yyyy")).
                replace("#FIO#",    s_fio).
                replace("#VS_NUM#", s_vs_num).
                replace("#POINT_NAME#", s_point_name).
                replace("#OPER_FIO#", s_oper_fio);
        s_content_new += s_row_new;
    }
    while (quer2->next()) {
        n++;

        QDate d_date     = quer2->value( 2).toDate();
        QString s_fio    = quer2->value( 3).toString();
        QString s_vs_num = quer2->value( 4).toString();
        QString s_point_name = query->value( 5).toString();
        QString s_oper_fio   = query->value( 6).toString();

        QString s_row_new = s_row;
        s_row_new = s_row_new.
                replace("#N#",      QString::number(n)).
                replace("#DATE#",   d_date.toString("dd.MM.yyyy")).
                replace("#FIO#",    s_fio).
                replace("#VS_NUM#", s_vs_num).
                replace("#POINT_NAME#", s_point_name).
                replace("#OPER_FIO#", s_oper_fio);
        s_content_new += s_row_new;
    }
    s_content_new += s_content.right(s_content.size()-pos1);

    delete query;
    delete quer2;

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
            this->setCursor(Qt::ArrowCursor);
            ui->bn_print->setEnabled(true);
            return;
        }
        QFile::remove(fname_res);
    }
    if (!QFile::copy(rep_template, fname_res) || !QFile::exists(fname_res)) {
        QMessageBox::warning(this, "Ошибка копирования отчёта", "Не удалось скопировать реестр ВС: \n" + rep_template + "\n\nво временную папку\n" + fname_res + "\n\nОперация прервана.");
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

    //close();
    this->setCursor(Qt::ArrowCursor);
    ui->bn_print->setEnabled(true);
}

void print_vs_register_by_date_wnd::on_ln_smo_oms_chif_textChanged(const QString &arg1) {
    data_app.smo_oms_chif = ui->ln_smo_oms_chif->text();
}

void print_vs_register_by_date_wnd::on_combo_point_currentIndexChanged(const QString &arg1) {
    refresh_operators();
}
