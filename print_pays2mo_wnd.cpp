#include "print_pays2mo_wnd.h"
#include "ui_print_pays2mo_wnd.h"

print_pays2mo_wnd::print_pays2mo_wnd(QSqlDatabase &db, s_data_app &data_app, s_data_req_pays2mo &data_req_pays2mo, s_data_pers &data_pers, s_data_doc &data_doc, QSettings &settings, QWidget *parent)
 : db(db), data_app(data_app), data_req_pays2mo(data_req_pays2mo), data_pers(data_pers), data_doc(data_doc), QDialog(parent), settings(settings), ui(new Ui::print_pays2mo_wnd)
{
    ui->setupUi(this);

    if (data_req_pays2mo.date_birth>QDate::currentDate().addYears(-14)) {
        ui->combo_pers_category->setCurrentIndex(0);
    } else if (data_req_pays2mo.date_birth>QDate::currentDate().addYears(-15)) {
        ui->combo_pers_category->setCurrentIndex(1);
    } else {
        ui->combo_pers_category->setCurrentIndex(2);
    }
    ui->combo_pers_category->setEnabled(false);

    ui->rb_by_self->setChecked( true );

    //if (ui->combo_pers_category->currentIndex()>0) {
        ui->ln_FIO_actor->setText(QString(data_req_pays2mo.fam + " " + data_req_pays2mo.im + " " + data_req_pays2mo.ot).trimmed().simplified());
    //}

    ui->date_from->setDate(QDate::currentDate().addMonths(-1));
    ui->date_to->setDate(QDate::currentDate());

    ui->ln_signer->setText(data_app.point_signer);

    ui->pan_scoresDB->setVisible( false );
    ui->pan_event->setVisible( false );
    ui->pan_buttons->setVisible( false );
    ui->line_buttons->setVisible( false );

    ui->bn_print_statement->setEnabled( false );
    ui->bn_print_reference->setEnabled( false );
    ui->bn_reference_ok->setEnabled( false );
}

print_pays2mo_wnd::~print_pays2mo_wnd() {
    delete ui;
}

void print_pays2mo_wnd::on_date_from_userDateChanged(const QDate &date) {
    if (ui->date_from->date()>QDate::currentDate()) {
        ui->date_from->setDate(QDate::currentDate());
    }
    if (ui->date_from->date()<QDate(2000,1,1)) {
        ui->date_from->setDate(QDate(2000,1,1));
    }
    if (ui->date_to->date()<ui->date_from->date()) {
        ui->date_to->setDate(ui->date_from->date());
    }
    ui->lab_dates->setText(QString::number(ui->date_from->date().daysTo(ui->date_to->date()) +1) + " дней");

    ui->bn_print_statement->setEnabled((ui->date_to->date()>=QDate(2000,1,1) && ui->date_to->date()<=QDate::currentDate()) &&
                                       (ui->date_from->date()>=QDate(2000,1,1) && ui->date_from->date()<=QDate::currentDate()));
    ui->bn_print_reference->setEnabled((ui->date_to->date()>=QDate(2000,1,1) && ui->date_to->date()<=QDate::currentDate()) &&
                                       (ui->date_from->date()>=QDate(2000,1,1) && ui->date_from->date()<=QDate::currentDate()));
}

void print_pays2mo_wnd::on_date_to_userDateChanged(const QDate &date) {
    if (ui->date_to->date()>QDate::currentDate()) {
        ui->date_to->setDate(QDate::currentDate());
    }
    if (ui->date_to->date()<QDate(2000,1,1)) {
        ui->date_to->setDate(QDate(2000,1,1));
    }
    if (ui->date_from->date()>ui->date_to->date()) {
        ui->date_from->setDate(ui->date_to->date());
    }
    ui->lab_dates->setText(QString::number(ui->date_from->date().daysTo(ui->date_to->date()) +1) + " дней");

    ui->bn_print_statement->setEnabled((ui->date_to->date()>=QDate(2000,1,1) && ui->date_to->date()<=QDate::currentDate()) &&
                                       (ui->date_from->date()>=QDate(2000,1,1) && ui->date_from->date()<=QDate::currentDate()));
    ui->bn_print_reference->setEnabled((ui->date_to->date()>=QDate(2000,1,1) && ui->date_to->date()<=QDate::currentDate()) &&
                                       (ui->date_from->date()>=QDate(2000,1,1) && ui->date_from->date()<=QDate::currentDate()));
}

void print_pays2mo_wnd::on_bn_close_clicked() {
    close();
}

void print_pays2mo_wnd::on_bn_edit_clicked(bool checked) {
    if (checked) {
        ui->lab_signer->setEnabled(true);
        ui->ln_signer->setEnabled(true);

        ui->bn_print_reference->setEnabled(false);
        ui->bn_print_statement->setEnabled(false);

        ui->bn_edit->setText("Сохранить");
    } else {
        data_app.point_signer = ui->ln_signer->text();
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
            settings.setValue("point_signer", data_app.point_signer.toUtf8());
            settings.endGroup();
            settings.sync();
        }
        ui->bn_edit->setText("Изменить");

        ui->lab_signer->setEnabled(false);
        ui->ln_signer->setEnabled(false);

        ui->bn_print_statement->setEnabled(true);
        ui->bn_print_reference->setEnabled(true);
    }
}

QString print_pays2mo_wnd::date_to_str(QDate date) {
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

#include "qt_windows.h"
#include "qwindowdefs_win.h"
#include <shellapi.h>

bool print_pays2mo_wnd::test_for_docs() {
    bool res = false;

    switch (ui->combo_pers_category->currentIndex()) {
    case 0:
        res = ( ui->rb_legal_rep->isChecked() &&
                ( ui->ch_self_birth_cert->isChecked() &&
                  ui->ch_legal_UDL->isChecked() )
              ) ||
              ( ui->rb_notary_rep->isChecked() &&
                ( ui->ch_notary_cert->isChecked() &&
                  ui->ch_notary_UDL->isChecked()
                )
              );
        break;
    case 1:
        res = ( ui->rb_legal_rep->isChecked() &&
                ( ui->ch_self_UDL->isChecked() &&
                  ui->ch_legal_UDL->isChecked() )
              ) ||
              ( ui->rb_notary_rep->isChecked() &&
                ( ui->ch_notary_cert->isChecked() &&
                  ui->ch_notary_UDL->isChecked()
                )
              );
        break;
    case 2:
        res = ( ui->rb_by_self->isChecked() &&
                ( ui->ch_self_UDL->isChecked() )
              ) ||
              ( ui->rb_notary_rep->isChecked() &&
                ( ui->ch_notary_cert->isChecked() &&
                  ui->ch_notary_UDL->isChecked()
                )
              );
        break;
    default:
        res = false;
        break;
    }

    return res;
}

void print_pays2mo_wnd::on_ch_self_UDL_clicked() {
    bool f = test_for_docs();
    ui->pan_event->setVisible(f);
    ui->line_buttons->setVisible(f);
    ui->pan_buttons->setVisible(f);
}
void print_pays2mo_wnd::on_ch_self_birth_cert_clicked() {
    bool f = test_for_docs();
    ui->pan_event->setVisible(f);
    ui->line_buttons->setVisible(f);
    ui->pan_buttons->setVisible(f);
}
void print_pays2mo_wnd::on_ch_legal_UDL_clicked() {
    bool f = test_for_docs();
    ui->pan_event->setVisible(f);
    ui->line_buttons->setVisible(f);
    ui->pan_buttons->setVisible(f);
}
void print_pays2mo_wnd::on_ch_notary_cert_clicked() {
    bool f = test_for_docs();
    ui->pan_event->setVisible(f);
    ui->line_buttons->setVisible(f);
    ui->pan_buttons->setVisible(f);
}
void print_pays2mo_wnd::on_ch_notary_UDL_clicked() {
    bool f = test_for_docs();
    ui->pan_event->setVisible(f);
    ui->line_buttons->setVisible(f);
    ui->pan_buttons->setVisible(f);
}
void print_pays2mo_wnd::on_rb_by_self_clicked() {
    bool f = test_for_docs();
    ui->pan_event->setVisible(f);
    ui->line_buttons->setVisible(f);
    ui->pan_buttons->setVisible(f);
}
void print_pays2mo_wnd::on_rb_legal_rep_clicked() {
    bool f = test_for_docs();
    ui->pan_event->setVisible(f);
    ui->line_buttons->setVisible(f);
    ui->pan_buttons->setVisible(f);
}
void print_pays2mo_wnd::on_rb_notary_rep_clicked() {
    bool f = test_for_docs();
    ui->pan_event->setVisible(f);
    ui->line_buttons->setVisible(f);
    ui->pan_buttons->setVisible(f);
}

void print_pays2mo_wnd::on_ch_scoresDB_clicked(bool checked) {
    if (checked) {
        if (QMessageBox::question(this, "Нужно подтверждение",
                                  "Вы действительно хотите изменить параметры подключения к базе счетов?",
                                  QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel)==QMessageBox::Cancel) {
            ui->ch_scoresDB->setChecked(false);
            return;
        }

        // покажем параметры подключения к базе счетов
        ui->pan_person->setEnabled(false);
        ui->pan_buttons->setVisible(false);
        ui->pan_event->setVisible(false);
        ui->line_buttons->setVisible(false);

        ui->pan_scoresDB->setVisible(true);

        ui->ln_scores_host->setText(data_app.scores_host);
        ui->ln_scores_base->setText(data_app.scores_base);
        ui->ln_scores_user->setText(data_app.scores_user);
        ui->ln_scores_pass->setText(data_app.scores_pass);
    }
    else {
        ui->pan_person->setEnabled(true);
        bool f = test_for_docs();
        ui->pan_event->setVisible(f);
        ui->line_buttons->setVisible(f);
        ui->pan_buttons->setVisible(f);

        ui->pan_scoresDB->setVisible(false);
    }
}

void print_pays2mo_wnd::on_bn_save_scoresDB_clicked() {
    data_app.scores_host = ui->ln_scores_host->text();
    data_app.scores_port = 0;
    data_app.scores_base = ui->ln_scores_base->text();
    data_app.scores_user = ui->ln_scores_user->text();
    data_app.scores_pass = ui->ln_scores_pass->text();
    // [scoresDB]
    settings.beginGroup("scoresDB");
    settings.setValue("scores_host", data_app.scores_host);
    settings.setValue("scores_port", data_app.scores_port);
    settings.setValue("scores_base", data_app.scores_base);
    settings.setValue("scores_user", data_app.scores_user);
    settings.setValue("scores_pass", data_app.scores_pass);
    settings.endGroup();
    settings.sync();

    QMessageBox::information(this, "Изменения сохранены",
                             "Новые параметры подключения к базе счетов сохранены.");
    ui->pan_person->setEnabled(true);
    bool f = test_for_docs();
    ui->pan_event->setVisible(f);
    ui->line_buttons->setVisible(f);
    ui->pan_buttons->setVisible(f);

    ui->ch_scoresDB->setChecked(false);
    ui->pan_scoresDB->setVisible(false);
}

// печать заявления о выдаче справке о стоимости оказанных мед-услуг
void print_pays2mo_wnd::on_bn_print_statement_clicked(bool checked) {
    if ( !(this->test_for_docs()) ) {
        QMessageBox::warning(this, "Уточните перечень предоставленных документов",
                             "Для получения справки о стоимости оказанных услуг, в зависимости от категории застрахованного или его представителя надо предоставить следуюлщие документы:\n\n"
                             "Для гдаждан до 14 лет :\n"
                             "  -  свидетельство о рождении;\n"
                             "  -  документ, удостоверяющий личность законного представителя.\n\n"
                             "Для граждан от 14 до 15 лет :\n"
                             "  -  документ, удостоверяющий личность гражданина;\n"
                             "  -  документ, удостоверяющий личность законного представителя.\n\n"
                             "Для граждан достигших 15 лет :\n"
                             "  -  документ, удостоверяющий личность гражданина.\n\n"
                             "Для представителей по доверенности :\n"
                             "  -  ноториальная доверенность;\n"
                             "  -  документ, удостоверяющий личность представителя по доверенности.\n\n");
        return;
    }

    //if (checked) {
        ui->bn_print_statement->setEnabled(false);
    //}

    QDate date_from = ui->date_from->date();
    QString date_from_s = date_to_str(date_from);
    QDate date_to = ui->date_to->date();
    QString date_to_s = date_to_str(date_to);
    QDate date_birth = data_req_pays2mo.date_birth;
    QString date_birth_s = date_to_str(date_birth);
    QDate date_udl = data_req_pays2mo.udl_date;
    QString date_udl_s = date_to_str(date_udl);
    QDate date_cur = QDate::currentDate();
    QString date_cur_s = date_to_str(date_cur);


    // ------------------------------
    // собственно генерация заявления
    // ------------------------------

    this->setCursor(Qt::WaitCursor);

    // распаковка шаблона
    QString rep_folder = data_app.path_reports;
    // распаковка шаблона
    QString tmp_folder = data_app.path_out + "_REPORTS_/temp/";
    QString rep_template = rep_folder + "req_pays2mo.odt";


    if (!QFile::exists(rep_template)) {
        QMessageBox::warning(this,
                             "Шаблон не найден",
                             "Не найден шаблон печатной формы заявления: \n" + rep_template +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    QDir tmp_dir(tmp_folder);
    if (tmp_dir.exists(tmp_folder) && !tmp_dir.removeRecursively()) {
        QMessageBox::warning(this,
                             "Ошибка при очистке папки",
                             "Не удалось очистить папку временных файлов: \n" + tmp_folder +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
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
        return;
    }
    // правка полей контента шаблона
    s_content = s_content.
            replace("#FAM#", data_req_pays2mo.fam).
            replace("#IM#",  data_req_pays2mo.im).
            replace("#OT#",  data_req_pays2mo.ot).

            replace("#DATE1#",  date_from_s).
            replace("#DATE2#",  date_to_s).
            replace("#DATE_BIRTH#",  date_birth_s).

            replace("#UDL_TYPE#", data_req_pays2mo.udl_name).
            replace("#UDL_SER#",  data_req_pays2mo.udl_ser).
            replace("#UDL_NUM#",  data_req_pays2mo.udl_num).
            replace("#UDL_CODE#", data_req_pays2mo.udl_code).
            replace("#UDL_ORG#",  data_req_pays2mo.udl_org).
            replace("#UDL_DATE#", date_udl_s).

            replace("#PHONE_CELL#",  data_req_pays2mo.phone_cell).
            replace("#PHONE_HOME#",  data_req_pays2mo.phone_home).

            replace("#DATE_CUR#",  date_cur_s);


    if (data_req_pays2mo.sex==1) {
        s_content = s_content.replace("#S1#", "V").replace("#S2#", " ");
    } else {
        s_content = s_content.replace("#S1#", " ").replace("#S2#", "V");
    };

    // сохранение контента шаблона
    QFile f_content_save(tmp_folder + "/content.xml");
    f_content_save.open(QIODevice::WriteOnly);
    f_content_save.write(s_content.toUtf8());
    f_content_save.close();


    // ===========================
    // архивация шаблона
    // ===========================
    QString fname_res = data_app.path_out + "_REPORTS_/" +
            QDate::currentDate().toString("yyyy-MM-dd") + "  -  " + data_app.smo_short + " - " + data_app.filial_name + "  -  " +
            "pays2mo_req - " + data_req_pays2mo.fam + " " + data_req_pays2mo.im + " " + data_req_pays2mo.ot + " - " + date_to_str(data_req_pays2mo.date_birth) + " -- " +
            date_from.toString("yyyy-MM-dd") + " - " + date_to.toString("yyyy-MM-dd") + ".odt";

    //QString fname_res = data_app.path_out + "_REPORTS_/"
    //        "pays2mo - " + data_pers.fam + " " + data_pers.im + " " + data_pers.ot + " - " + date_to_str(data_req_pays2mo.date_birth) + "odt";

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
            return;
        }
        QFile::remove(fname_res);
    }
    if (!QFile::copy(rep_template, fname_res) || !QFile::exists(fname_res)) {
        QMessageBox::warning(this, "Ошибка копирования шаблона", "Не удалось скопировать шаблон печатной формы уведомления: \n" + rep_template + "\n\nво временную папку\n" + fname_res + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
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
                             "Ошибка при открытии файла печатной формы заявления",
                             "Файл печатной формы заявления не найден:\n\n" + fname_res +
                             "\nКод ошибки: " + QString::number(result) +
                             "\n\nПопробуйте открыть этот файл вручную.\n\nЕсли файл не создан или ошибка будет повторяться - обратитесь к разработчику.");
    }
}


// печать справки о стоимости оказанных мед-услуг
void print_pays2mo_wnd::on_bn_print_reference_clicked(bool checked) {
    if ( !(this->test_for_docs()) ) {
        QMessageBox::warning(this, "Уточните перечень предоставленных документов",
                             "Для получения справки о стоимости оказанных услуг, в зависимости от категории застрахованного или его представителя надо предоставить следуюлщие документы:\n\n"
                             "Для гдаждан до 14 лет :\n"
                             "  -  свидетельство о рождении;\n"
                             "  -  документ, удостоверяющий личность законного представителя.\n\n"
                             "Для граждан от 14 до 15 лет :\n"
                             "  -  документ, удостоверяющий личность гражданина;\n"
                             "  -  документ, удостоверяющий личность законного представителя.\n\n"
                             "Для граждан достигших 15 лет :\n"
                             "  -  документ, удостоверяющий личность гражданина.\n\n"
                             "Для представителей по доверенности :\n"
                             "  -  ноториальная доверенность;\n"
                             "  -  документ, удостоверяющий личность представителя по доверенности.\n\n");
        return;
    }

    ui->bn_print_reference->setEnabled(false);
    ui->bn_reference_ok->setEnabled(true);


    // ----------------------------------------
    // получение данных справки - пока заглушка
    // ----------------------------------------
    data_pays2mo.id         = -1;
    data_pays2mo.id_person  = data_pers.id;
    data_pays2mo.id_point   = data_app.id_point;
    data_pays2mo.id_operator= data_app.id_operator;

    data_pays2mo.n_ref      = 258;
    data_pays2mo.fam        = data_req_pays2mo.fam;
    data_pays2mo.im         = data_req_pays2mo.im;
    data_pays2mo.ot         = data_req_pays2mo.ot;
    data_pays2mo.date_birth = data_req_pays2mo.date_birth;
    data_pays2mo.sex        = data_req_pays2mo.sex;

    data_pays2mo.date_from  = ui->date_from->date();
    data_pays2mo.date_to    = ui->date_to->date();
    data_pays2mo.date_req   = QDate::currentDate();
    data_pays2mo.date_take  = QDate::currentDate();
    data_pays2mo.fio_oper   = data_app.operator_fio;
    data_pays2mo.status     = 0;


    // если подключения ещё нет - создадим новое подключение
    if (!db_scores.isOpen()) {
        // создадим подключение к базе данных
        db_scores = QSqlDatabase::addDatabase("QIBASE", "ScoreDB");
        db_scores.setHostName(data_app.scores_host);
        db_scores.setDatabaseName(data_app.scores_base);
        db_scores.setUserName(data_app.scores_user);
        db_scores.setPassword(data_app.scores_pass);

        if (!db_scores.open()) {
            // подключение не удалось - выведем сообщение об ошибке
            QMessageBox::critical(this, "База счетов не доступна",
                                  "При открытии базы счетов произошла ошибка\n" +
                                  db_scores.lastError().driverText() + "\n" +
                                  db_scores.lastError().databaseText());
            return;
        }
    }

    // прочитаем данные из базы счетов
    QSqlQuery *query_ref = new QSqlQuery(db_scores);
    QString sql_ref = "select * "
                      //" from REF_VIEW_REFERENCE(" + QString::number(data_pays2mo.id_person) + ", "
                      " from REF_VIEW_REFERENCE_2(" + QString::number(data_pays2mo.id_person) + ", "
                                                    + "'" + ui->date_from->date().toString("yyyy-MM-dd") + "', " +
                                                    + "'" + ui->date_to->date().toString("yyyy-MM-dd") + "') ; ";
    bool res_ref = query_ref->exec(sql_ref);
    if (!res_ref) {
        delete query_ref;
        this->setCursor(Qt::ArrowCursor);
        QMessageBox::warning(this, "Ошибка при чтении из базы счетов", "При чтении данных оказанных медуслуг из базы счетов произошла неожиданная ошибка");
        delete query_ref;
        db_scores.close();
        ui->bn_print_reference->setChecked(false);
        ui->bn_print_reference->setEnabled(true);
        return;
    }
    while (query_ref->next()) {
        s_data_pays2mo_ln line1;

        line1.mo_name  = query_ref->value("NAME_MO").toString();
        line1.usl_ok   = query_ref->value("USL_OK").toString();
        line1.usl_dates= query_ref->value("DATES").toString();
        line1.usl_name = query_ref->value("NAME_USL").toString();
        line1.pay      = query_ref->value("SUMP").toFloat();

        data_pays2mo.lines.append(line1);
    }


    // ---------------------------------------
    // получим номер последней справки из базы
    // ---------------------------------------
    QString sql_nref;
    sql_nref = "select max(n_ref) from refs_pays2mo ; ";

    QSqlQuery *query_nref = new QSqlQuery(db);
    if (!mySQL.exec(this, sql_nref, "Получение номера последней сгенерированной справки", *query_nref, true, db, data_app) || !query_nref->next()) {
        delete query_nref;
        QMessageBox::warning(this, "Операция отменена",
                             "При получении номера последней сгенерированной справки произошла неожиданная ошибка.\n\n"
                             "Операция генерации справки отменена.");
        //return;
    }
    data_pays2mo.n_ref = query_nref->value(0).toInt() +1;
    delete query_nref;


    // -----------------------------
    // собственно генерация справки
    // -----------------------------
    this->setCursor(Qt::WaitCursor);

    // распаковка шаблона
    QString rep_folder = data_app.path_reports;
    // распаковка шаблона
    QString tmp_folder = data_app.path_out + "_REPORTS_/temp/";
    QString rep_template = rep_folder + "rep_pays2mo.odt";


    if (!QFile::exists(rep_template)) {
        QMessageBox::warning(this,
                             "Шаблон не найден",
                             "Не найден шаблон печатной формы заявления: \n" + rep_template +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    QDir tmp_dir(tmp_folder);
    if (tmp_dir.exists(tmp_folder) && !tmp_dir.removeRecursively()) {
        QMessageBox::warning(this,
                             "Ошибка при очистке папки",
                             "Не удалось очистить папку временных файлов: \n" + tmp_folder +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
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
        return;
    }

    // -----------------------
    // выделение строк справки
    // -----------------------
    int pos0 = 0;
    int pos1 = s_content.indexOf("<table:table-row table:style-name=\"Таблица2.2\">", pos0);
    int pos2 = s_content.indexOf("<table:table-row table:style-name=\"Таблица2.3\">", pos1);
    int pos3 = s_content.indexOf("</table:table-row>", pos2) + 18;
    int pos4 = s_content.length();
    QString s_top   = s_content.mid(pos0, pos1-pos0);
    QString s_line1 = s_content.mid(pos1, pos2-pos1);
    QString s_line2 = s_content.mid(pos2, pos3-pos2);
    QString s_down  = s_content.mid(pos3, pos4-pos3);

    // -----------------------------
    // правка полей контента шаблона
    // -----------------------------
    QString s_line = s_top;
    s_content = s_top.
            replace("#SMO_LOGO1#", data_app.smo_logo1).
            replace("#SMO_LOGO2#", data_app.smo_logo2).
            replace("#SMO_LOGO3#", data_app.smo_logo3).
            replace("#N#",         QString::number(data_pays2mo.n_ref)).
            replace("#DATE_CUR#",  date_to_str(QDate::currentDate())).
            replace("#FAM#",       data_pays2mo.fam).
            replace("#IM#",        data_pays2mo.im).
            replace("#OT#",        data_pays2mo.ot).
            replace("#DATE1#",     date_to_str(data_pays2mo.date_from)).
            replace("#DATE2#",     date_to_str(data_pays2mo.date_to));

    // переберём строки справки с одинаковыми значениями mo_name
    QString mo_name = "";
    if (data_pays2mo.lines.count()>0)
        mo_name = data_pays2mo.lines.at(0).mo_name;
    int n = 0;
    float pay = 0;
    data_pays2mo.pay_sum = 0;
    for (int i=0; i<data_pays2mo.lines.count(); i++) {
        if ( mo_name!=data_pays2mo.lines.at(i).mo_name ) {
            mo_name = data_pays2mo.lines.at(i).mo_name;
            n = 0;
            int pay_r = pay;
            int pay_k = (pay-pay_r)*100;
            pay = 0;
            s_line = s_line2;
            s_content += s_line.
                    replace("#PAY_R#", QString::number(pay_r)).
                    replace("#PAY_K#", QString::number(pay_k));
        }
        n++;
        pay += data_pays2mo.lines.at(i).pay;
        data_pays2mo.pay_sum += data_pays2mo.lines.at(i).pay;
        s_line = s_line1;
        s_content += s_line.
                replace("#MO#",       data_pays2mo.lines.at(i).mo_name).
                replace("#N#",        QString::number(n)).
                replace("#USL_OK#",   data_pays2mo.lines.at(i).usl_ok).
                replace("#USL_DATE#", data_pays2mo.lines.at(i).usl_dates).
                replace("#USL_NAME#", data_pays2mo.lines.at(i).usl_name);

    }
    if ( n>0 ) {
        int pay_r = pay;
        int pay_k = (pay-pay_r)*100;
        s_line = s_line2;
        s_content += s_line.
                replace("#PAY_R#", QString::number(pay_r)).
                replace("#PAY_K#", QString::number(pay_k));
    }
    s_line = s_down;
    s_content += s_down.
            replace("#FIO_OPER#", data_pays2mo.fio_oper);

    // сохранение контента шаблона
    QFile f_content_save(tmp_folder + "/content.xml");
    f_content_save.open(QIODevice::WriteOnly);
    f_content_save.write(s_content.toUtf8());
    f_content_save.close();


    // ===========================
    // архивация шаблона
    // ===========================
    QString fname_res = data_app.path_out + "_REPORTS_/" + QDate::currentDate().toString("yyyy-MM-dd") + "  -  " + data_app.smo_short + " - " + data_app.filial_name + "  -  " +
            "pays2mo - " + data_pays2mo.fam + " " + data_pays2mo.im + " " + data_pays2mo.ot + " - " + date_to_str(data_req_pays2mo.date_birth) + " -- " +
            data_pays2mo.date_from.toString("yyyy-MM-dd") + " - " + data_pays2mo.date_to.toString("yyyy-MM-dd") + ".odt";

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
            return;
        }
        QFile::remove(fname_res);
    }
    if (!QFile::copy(rep_template, fname_res) || !QFile::exists(fname_res)) {
        QMessageBox::warning(this, "Ошибка копирования шаблона", "Не удалось скопировать шаблон печатной формы уведомления: \n" + rep_template + "\n\nво временную папку\n" + fname_res + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    com = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname_res + "\" \"" + tmp_folder + "/content.xml\"";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка обновления контента",
                             "При добавлении нового контента в шаблон печатной формы заявления произошла непредвиденная ошибка\n\nОпреация прервана.");
    }


    // -----------------------------
    // сохраним запись в базу данных
    // -----------------------------
    QString bin;
    mySQL.myBYTEA(fname_res, bin);
    QString sql_file;
    sql_file = "insert into refs_pays2mo (n_ref, id_person, id_point, id_oper, date_from, date_to, date_req, date_take, pay_sum, status, pers_fio, pers_date_birth, pers_sex, bin) ";
    sql_file+= " values("
               "'" + QString::number(data_pays2mo.n_ref) + "', "
               "'" + QString::number(data_pays2mo.id_person) + "', "
               "'" + QString::number(data_pays2mo.id_point) + "', "
               "'" + QString::number(data_pays2mo.id_operator) + "', "
               "'" + data_pays2mo.date_from.toString("yyyy-MM-dd") + "', "
               "'" + data_pays2mo.date_to.toString("yyyy-MM-dd") + "', "
               "'" + data_pays2mo.date_req.toString("yyyy-MM-dd") + "', "
               "NULL, "
               "'" + QString::number(data_pays2mo.pay_sum) + "', "
               "'" + QString::number(data_pays2mo.status) + "', "
               "'" + QString(data_pays2mo.fam + " " + data_pays2mo.im + " " + data_pays2mo.ot).trimmed().simplified() + "', "
               "'" + data_pays2mo.date_birth.toString("yyyy-MM-dd") + "', "
               "'" + QString::number(data_pays2mo.sex) + "', "
               "'" + bin + "') ";
    sql_file+= " returning id ; ";

    QSqlQuery *query_file = new QSqlQuery(db);
    if (!mySQL.exec(this, sql_file, "Запись резервной копии файла справки в базу полисов", *query_file, true, db, data_app) || !query_file->next()) {
        delete query_file;
        QMessageBox::warning(this, "Операция отменена", "При записи резервной копии файла справки в базу полисов произошла неожиданная ошибка.\n\nОперация резервирования файла справки отменена.\nРаботу можно продолжить.");
    }
    data_pays2mo.id = query_file->value(0).toInt();
    delete query_file;




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
}


void print_pays2mo_wnd::on_bn_reference_ok_clicked(bool checked) {
    //if (checked) {
        ui->bn_reference_ok->setEnabled(false);
    //}

    // сделаем запись, что справка выдана
    QString sql_satus;
    sql_satus = "update refs_pays2mo "
                " set status=1, "
                "     date_take='" + QDate::currentDate().toString("yyyy-MM-dd") + "' "
                " where id=" + QString::number(data_pays2mo.id) + " ; ";

    QSqlQuery *query_satus = new QSqlQuery(db);
    mySQL.exec(this, sql_satus, "Правка статуса выданной справки о стоимости услуг МО", *query_satus, true, db, data_app) || !query_satus->next();
    delete query_satus;
}
