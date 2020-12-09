#include "print_vs_wnd.h"
#include <QSettings>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QSqlQuery>
#include <QProcess>
#include "ui_print_vs_wnd.h"

print_vs_wnd::print_vs_wnd(QSqlDatabase &db, s_data_app &data_app, s_data_pers &data_pers, s_data_polis &data_polis, QSettings &settings, QWidget *parent)
    : db(db), data_app(data_app), data_pers(data_pers), data_polis(data_polis), QDialog(parent), settings(settings), ui(new Ui::print_vs_wnd)
{
    ui->setupUi(this);

    ui->line_smo_name->setText(data_app.smo_name);
    ui->line_smo_address->setText(data_app.smo_address);
    ui->line_signer->setText(data_app.point_signer);

    ui->spin_blank_width->setValue(data_app.vs_blank_width_mm);
    ui->spin_blank_height->setValue(data_app.vs_blank_height_mm);

    ui->spin_field_top->setValue(data_app.vs_field_top_mm);
    ui->spin_field_bottom->setValue(data_app.vs_field_bottom_mm);
    ui->spin_field_left->setValue(data_app.vs_field_left_mm);
    ui->spin_field_right->setValue(data_app.vs_field_right_mm);
}

print_vs_wnd::~print_vs_wnd() {
    delete ui;
}

QString print_vs_wnd::date_to_str(QDate date) {
    QString res = QString::number(date.day()) + " ";
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
    res += QString::number(date.year()) + " г.";
    return res;
}

QString print_vs_wnd::month_to_str(QDate date) {
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

void print_vs_wnd::on_bn_edit_clicked(bool checked) {
    if (checked) {
        ui->bn_edit->setText("Сохранить");
        ui->lab_smo_name->setEnabled(true);
        ui->lab_smo_address->setEnabled(true);
        ui->lab_height->setEnabled(true);
        ui->lab_width->setEnabled(true);
        ui->lab_blank->setEnabled(true);
        ui->lab_fields->setEnabled(true);
        ui->lab_signer->setEnabled(true);
        ui->line_smo_name->setEnabled(true);
        ui->line_smo_address->setEnabled(true);
        ui->line_signer->setEnabled(true);
        ui->spin_blank_height->setEnabled(true);
        ui->spin_blank_width->setEnabled(true);
        ui->spin_field_top->setEnabled(true);
        ui->spin_field_bottom->setEnabled(true);
        ui->spin_field_left->setEnabled(true);
        ui->spin_field_right->setEnabled(true);
        ui->bn_print->setEnabled(false);
    } else {
        data_app.smo_name = ui->line_smo_name->text();
        data_app.smo_address = ui->line_smo_address->text();
        data_app.vs_blank_width_mm = ui->spin_blank_width->value();
        data_app.vs_blank_height_mm = ui->spin_blank_height->value();
        data_app.vs_field_top_mm = ui->spin_field_top->value();
        data_app.vs_field_bottom_mm = ui->spin_field_bottom->value();
        data_app.vs_field_left_mm = ui->spin_field_left->value();
        data_app.vs_field_right_mm = ui->spin_field_right->value();
        data_app.point_signer = ui->line_signer->text();
        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Сохранить сделанные изменения в настройках программы?\n\n"
                                  "В случае согласия новые данные будут сохранены и останутся доступны после перезагрузки программы.\n"
                                  "В случае отказа эти изменения будут доступны только до первого перезапуска программы.",
                                  QMessageBox::Yes|QMessageBox::No,
                                  QMessageBox::No)==QMessageBox::Yes)
        {
            // [VS]
            settings.beginGroup("VS");
            //settings.setValue("inko_name", data_app.vs_inko_name.toUtf8());
            //settings.setValue("inko_address", data_app.vs_inko_address.toUtf8());
            settings.setValue("blank_width_mm", data_app.vs_blank_width_mm);
            settings.setValue("blank_height_mm", data_app.vs_blank_height_mm);
            settings.setValue("field_top_mm", data_app.vs_field_top_mm);
            settings.setValue("field_bottom_mm", data_app.vs_field_bottom_mm);
            settings.setValue("field_left_mm", data_app.vs_field_left_mm);
            settings.setValue("field_right_mm", data_app.vs_field_right_mm);
            settings.endGroup();
            settings.beginGroup("smo");
            settings.setValue("smo_name", data_app.smo_name);
            settings.setValue("smo_address", data_app.smo_address);
            settings.setValue("point_signer", data_app.point_signer);
            settings.endGroup();
            settings.sync();
        }
        ui->bn_edit->setText("Изменить");
        ui->lab_smo_name->setEnabled(false);
        ui->lab_smo_address->setEnabled(false);
        ui->lab_height->setEnabled(false);
        ui->lab_width->setEnabled(false);
        ui->lab_blank->setEnabled(false);
        ui->lab_signer->setEnabled(false);
        ui->lab_fields->setEnabled(false);
        ui->line_smo_name->setEnabled(false);
        ui->line_smo_address->setEnabled(false);
        ui->line_signer->setEnabled(false);
        ui->spin_blank_height->setEnabled(false);
        ui->spin_blank_width->setEnabled(false);
        ui->spin_field_top->setEnabled(false);
        ui->spin_field_bottom->setEnabled(false);
        ui->spin_field_left->setEnabled(false);
        ui->spin_field_right->setEnabled(false);
        ui->bn_print->setEnabled(true);
    }
}

void print_vs_wnd::on_bn_cansel_clicked() {
    close();
}

#include "qt_windows.h"
#include "qwindowdefs_win.h"
#include <shellapi.h>

void print_vs_wnd::on_bn_print_clicked() {
    ui->bn_print->setEnabled(false);
    this->setCursor(Qt::WaitCursor);

    // получим данные УДЛ
    QSqlQuery *query = new QSqlQuery(db);
    QString sql;
    sql = "select d.doc_type, f11.text as doc_name, d.doc_ser, d.doc_num, d.doc_date, d.doc_exp, d.doc_org, d.id ";
    sql+= " from persons e ";
    sql+= "      left join persons_docs d on(d.id_person=e.id and d.status=1 and d.doc_type in(1,3,9,12,13,14,21,22,24,25,27,28) and (d.doc_exp is NULL or d.doc_exp>CURRENT_DATE)) ";
    sql+= "      left join spr_f011 f11 on(f11.code=d.doc_type) ";
    sql+= " where e.id=" + QString::number(data_pers.id) + " ; ";
    bool res = mySQL.exec(this, sql, QString("Получение данных УДЛ"), *query, true, db, data_app);
    if (!query->next()) {
        QMessageBox::warning(this,
                             "Ошибка получения данных УДЛ",
                             QString("Не удалось получить данные УДЛ застрахованного.") +
                             "\n\nПроверьте, введены ли данные и не истёк ли срок действия УДЛ застрахованного." +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    data_doc.doc_type = query->value(0).toInt();
    data_doc.doc_name = query->value(1).toString();
    data_doc.doc_ser = query->value(2).toString();
    data_doc.doc_num = query->value(3).toString();
    data_doc.doc_date = query->value(4).toDate();
    data_doc.has_doc_exp = !(query->value(5).isNull());
    data_doc.doc_exp = query->value(5).toDate();
    data_doc.doc_org = query->value(6).toString();
    data_doc.id = query->value(7).toInt();
    data_doc.status = true;
    query->finish();
    delete query;

    // распаковка шаблона
    QString rep_folder = data_app.path_reports;
    if (data_app.ocato=="14000") {
        rep_folder += "rep_vs_belg";
    } else if (data_app.ocato=="38000") {
        rep_folder += "rep_vs_kursk";
    } else {
        rep_folder += "rep_vs";
    }
    // распаковка шаблона
    QString tmp_folder = data_app.path_out + "_POL_VS_/temp/";
    QString rep_template = rep_folder + ".odt";

    if (!QFile::exists(rep_template)) {
        QMessageBox::warning(this,
                             "Шаблон не найден",
                             "Не найден шаблон печатной формы ВС: \n" + rep_template +
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
    // правка полей контента шаблона
    s_content = s_content.
            replace("#INKO_NAME#", data_app.smo_name).
            replace("#INKO_ADDRESS#", data_app.smo_address).
            replace("#D#", QString::number(data_polis.date_begin.day())).
            replace("#M#", month_to_str(data_polis.date_begin)).
            replace("#Y#", QString::number(data_polis.date_begin.year()%100)).
            replace("#FIO#", data_pers.fam + " " + data_pers.im + " " + data_pers.ot).
            replace("#DR_DOC#", date_to_str(data_pers.date_birth) + " рожд., " + data_doc.doc_name + " " + data_doc.doc_ser + " № " + data_doc.doc_num + ", выдан " + date_to_str(data_doc.doc_date) + ", " ).
            replace("#DOC_ORG#", data_doc.doc_org).
            replace("#B_PLACE#", data_pers.plc_birth).
            //replace("#S1#", ((data_pers.sex<2) ? QString("V") : QString(" "))).
            //replace("#S2#", ((data_pers.sex>1) ? QString(" ") : QString("V"))).
            replace("#D2#", QString::number(data_polis.date_end.day())).
            replace("#M2#", month_to_str(data_polis.date_end)).
            replace("#Y2#", QString::number(data_polis.date_end.year()%100)).
            replace("#AUTHOR#", data_app.point_signer);
    if (data_pers.sex==1) {
        s_content = s_content.replace("#S1#", "V").replace("#S2#", " ");
    } else {
        s_content = s_content.replace("#S1#", " ").replace("#S2#", "V");
    }
    // сохранение контента шаблона
    QFile f_content_save(tmp_folder + "/content.xml");
    f_content_save.open(QIODevice::WriteOnly);
    f_content_save.write(s_content.toUtf8());
    f_content_save.close();


    // ===========================
    // правка стилей шаблона
    // ===========================
    // открытие стилей шаблона
    QFile f_styles(tmp_folder + "/styles.xml");
    f_styles.open(QIODevice::ReadOnly);
    QString s_styles = f_styles.readAll();
    f_styles.close();
    if (s_styles.isEmpty()) {
        QMessageBox::warning(this,
                             "Файл стилей шаблона пустой",
                             "Файл стилей шаблона пустой. Возможно шаблон был испорчен.\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    int st_pos_from = s_styles.indexOf("<style:page-layout-properties");
    int st_pos_to = s_styles.indexOf("</style:page-layout-properties>");
    // правка полей стилей шаблона
    QString subs_styles = "<style:page-layout-properties "
                          "fo:page-width=\"" + QString::number(data_app.vs_blank_width_mm/10.0) + "cm\" "
                          "fo:page-height=\"" + QString::number(data_app.vs_blank_height_mm/10.0) + "cm\" "
                          "style:num-format=\"1\" "
                          "style:print-orientation=\"portrait\" "
                          "fo:margin-top=\"" + QString::number(data_app.vs_field_top_mm/10.0) + "cm\" "
                          "fo:margin-bottom=\"" + QString::number(data_app.vs_field_bottom_mm/10.0) + "cm\" "
                          "fo:margin-left=\"" + QString::number(data_app.vs_field_left_mm/10.0) + "cm\" "
                          "fo:margin-right=\"" + QString::number(data_app.vs_field_right_mm/10.0) + "cm\" "
                          "style:writing-mode=\"lr-tb\" "
                          "style:footnote-max-height=\"0cm\">"
                          "<style:footnote-sep style:width=\"0.018cm\" "
                          "style:distance-before-sep=\"0.101cm\" "
                          "style:distance-after-sep=\"0.101cm\" "
                          "style:line-style=\"solid\" "
                          "style:adjustment=\"left\" "
                          "style:rel-width=\"25%\" "
                          "style:color=\"#000000\"/> ";
    s_styles = s_styles.mid(0, st_pos_from) + subs_styles + s_styles.mid(st_pos_to);
    // сохранение стилей шаблона
    QFile f_styles_save(tmp_folder + "/styles.xml");
    f_styles_save.open(QIODevice::WriteOnly);
    f_styles_save.write(s_styles.toUtf8());
    f_styles_save.close();


    // ===========================
    // архивация шаблона
    // ===========================
    QString fname_res = data_app.path_out + "_POL_VS_/pol_vs - " + data_pers.fam + " " + data_pers.im + " " + data_pers.ot + " - " + date_to_str(data_pers.date_birth) + "odt";

    QFile::remove(fname_res);
    while (QFile::exists(fname_res)) {
        if (QMessageBox::warning(this,
                                 "Ошибка сохранения печатной формы ВС",
                                 "Не удалось сохранить шаблон печатной формы ВС: \n" + rep_template +
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
        QMessageBox::warning(this, "Ошибка копирования шаблона", "Не удалось скопировать шаблон печатной формы ВС: \n" + rep_template + "\n\nво временную папку\n" + fname_res + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    com = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname_res + "\" \"" + tmp_folder + "/content.xml\"";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка обновления контента",
                             "При добавлении нового контента в шаблон печатной формы ВС произошла непредвиденная ошибка\n\nОпреация прервана.");
    }
    com = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname_res + "\" \"" + tmp_folder + "/styles.xml\"";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка обновления стилей",
                             "При добавлении новых стилей в шаблон печатной формы ВС произошла непредвиденная ошибка\n\nОпреация прервана.");
    }



    // ===========================
    // собственно открытие шаблона
    // ===========================
    // открытие полученного ODT
    long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);
    this->setCursor(Qt::ArrowCursor);
    if (result<32) {
        QMessageBox::warning(this,
                             "Ошибка при открытии файла печатной формы ВС",
                             "Файл печатной формы ВС не найден:\n\n" + fname_res +
                             "\nКод ошибки: " + QString::number(result) +
                             "\n\nПопробуйте открыть этот файл вручную.\n\nЕсли файл не создан или ошибка будет повторяться - обратитесь к разработчику.");
    }
    ui->bn_print->setEnabled(true);
    close();
}
