#include "print_notic2_wnd.h"
#include "ui_print_notic2_wnd.h"

print_notic2_wnd::print_notic2_wnd(QSqlDatabase &db, s_data_app &data_app, s_data_pers &data_pers, s_data_vizit &data_vizit, s_data_polis &data_polis, QSettings &settings, QWidget *parent)
 : db(db), data_app(data_app), data_pers(data_pers), data_vizit(data_vizit), data_polis(data_polis), QDialog(parent), settings(settings), ui(new Ui::print_notic2_wnd)
{
    ui->setupUi(this);

    ui->ln_smo_logo1->setText(data_app.smo_logo1);
    ui->ln_smo_logo2->setText(data_app.smo_logo2);
    ui->ln_smo_logo3->setText(data_app.smo_logo3);
    ui->ln_smo_contact_info->setText(data_app.smo_contact_info);
}

print_notic2_wnd::~print_notic2_wnd() {
    delete ui;
}

void print_notic2_wnd::on_bn_close_clicked() {
    reject();
}

QString print_notic2_wnd::date_to_str(QDate date) {
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


#include "qt_windows.h"
#include "qwindowdefs_win.h"
#include <shellapi.h>

void print_notic2_wnd::on_bn_print_clicked() {
    ui->bn_print->setEnabled(false);
    this->setCursor(Qt::WaitCursor);

    // распаковка шаблона
    QString rep_folder = data_app.path_reports;
    // распаковка шаблона
    QString tmp_folder = data_app.path_out + "_POL_NOTICES_/temp/";
    QString rep_template = rep_folder + "pol_notic2.odt";

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
            replace("#FAM#", data_pers.fam).
            replace("#IM#", data_pers.im).
            replace("#OT#", data_pers.ot).
            replace("#SUBJ_NAME#", data_app.terr_name_rp);


    // сохранение контента шаблона
    QFile f_content_save(tmp_folder + "/content.xml");
    f_content_save.open(QIODevice::WriteOnly);
    f_content_save.write(s_content.toUtf8());
    f_content_save.close();


    // ===========================
    // архивация шаблона
    // ===========================
    QString fname_res = data_app.path_out + "_POL_NOTICES_/pol_notic2 - " + data_pers.fam + " " + data_pers.im + " " + data_pers.ot + " - " + date_to_str(data_pers.date_birth) + "odt";


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
                             "Ошибка при открытии файла печатной формы информирования",
                             "Файл печатной формы информирования не найден:\n\n" + fname_res +
                             "\nКод ошибки: " + QString::number(result) +
                             "\n\nПопробуйте открыть этот файл вручную.\n\nЕсли файл не создан или ошибка будет повторяться - обратитесь к разработчику.");
    }
    this->setCursor(Qt::ArrowCursor);
    ui->bn_print->setEnabled(true);
    close();
}

void print_notic2_wnd::on_bn_edit_clicked(bool checked) {
    if (checked) {
        ui->bn_edit->setText("Сохранить");
        ui->lab_smo_contact_info->setEnabled(true);

        ui->ln_smo_logo1->setEnabled(true);
        ui->ln_smo_logo2->setEnabled(true);
        ui->ln_smo_logo3->setEnabled(true);
        ui->ln_smo_contact_info->setEnabled(true);

        ui->bn_print->setEnabled(false);
    } else {
        data_app.smo_logo1 = ui->ln_smo_logo1->text();
        data_app.smo_logo2 = ui->ln_smo_logo2->text();
        data_app.smo_logo3 = ui->ln_smo_logo3->text();
        data_app.smo_contact_info = ui->ln_smo_contact_info->text();
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
            settings.setValue("smo_contact_info", data_app.smo_contact_info.toUtf8());
            settings.endGroup();
            settings.sync();
        }
        ui->bn_edit->setText("Изменить");

        ui->lab_smo_contact_info->setEnabled(false);

        ui->ln_smo_logo1->setEnabled(false);
        ui->ln_smo_logo2->setEnabled(false);
        ui->ln_smo_logo3->setEnabled(false);
        ui->ln_smo_contact_info->setEnabled(false);

        ui->bn_print->setEnabled(true);
    }
}

void print_notic2_wnd::on_bn_edit_clicked()
{

}
