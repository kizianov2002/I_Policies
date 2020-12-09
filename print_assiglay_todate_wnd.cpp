#include "print_assiglay_todate_wnd.h"
#include "ui_print_assiglay_todate_wnd.h"

print_assiglay_todate_wnd::print_assiglay_todate_wnd(bool f_today, QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    f_today(f_today),
    db(db),
    data_app(data_app),
    settings(settings),
    QDialog(parent),
    ui(new Ui::print_assiglay_todate_wnd)
{
    ui->setupUi(this);

    show_tab_w = NULL;

    if (f_today) {
        //ui->date_assiglay->setEnabled(false);
        //ui->bn_1st_day->setEnabled(false);
        //ui->bn_today->setEnabled(false);
        date_assiglay = QDate::currentDate();
        ui->date_assiglay->setDate(date_assiglay);
        ui->ch_bymo->setChecked(false);
    } else {
        //ui->date_assiglay->setEnabled(true);
        //ui->bn_1st_day->setEnabled(true);
        //ui->bn_today->setEnabled(true);
        date_assiglay = QDate(QDate::currentDate().year(), QDate::currentDate().month(), 1);
        ui->date_assiglay->setDate(date_assiglay);
        ui->ch_bymo->setChecked(true);
    }

    ui->date_assiglay->setDate(date_assiglay);
    ui->pan_progress->setVisible(false);
    ui->bn_save_acts->setVisible(false);

    /*if (data_app.ocato=="14000") {
        ui->bn_save_csv->setVisible(false);
    }*/
}

print_assiglay_todate_wnd::~print_assiglay_todate_wnd() {
    delete ui;
}

void print_assiglay_todate_wnd::on_bn_cansel_clicked() {
    close();
}



void print_assiglay_todate_wnd::on_bn_today_clicked() {
    date_assiglay = QDate::currentDate();
    ui->date_assiglay->setDate(date_assiglay);
    f_today = date_assiglay==QDate::currentDate();
}

void print_assiglay_todate_wnd::on_bn_1st_day_clicked() {
    date_assiglay = QDate(QDate::currentDate().year(), QDate::currentDate().month(), 1);
    ui->date_assiglay->setDate(date_assiglay);
    f_today = date_assiglay==QDate::currentDate();
}

void print_assiglay_todate_wnd::on_date_assiglay_editingFinished() {
    date_assiglay = ui->date_assiglay->date();
    f_today = date_assiglay==QDate::currentDate();
}



// теперь сформируем акты численности прикреплённых застрахованных в разрезе участков МО
void print_assiglay_todate_wnd::on_bn_save_acts_clicked() {
    ui->pan_progress->setVisible(true);
    QDate date_rep = ui->date_assiglay->date();

    // -------------------------------- //
    // подготовка к формированию отчёта //
    // -------------------------------- //

    // распаковка шаблона
    QString rep_folder = data_app.path_reports;
    QString tmp_folder = data_app.path_out + "_REPORTS_/temp/";
    QString rep_template = rep_folder + "act_assig4softrust.odt";

    if (!QFile::exists(rep_template)) {
        QMessageBox::warning(this,
                             "Шаблон не найден",
                             "Не найден шаблон печатной формы акта сверки прикрепления застрахованных: \n" + rep_template +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_save_acts->setEnabled(true);
        ui->pan_progress->setVisible(false);
        return;
    }
    QDir tmp_dir(tmp_folder);
    if (tmp_dir.exists(tmp_folder) && !tmp_dir.removeRecursively()) {
        QMessageBox::warning(this,
                             "Ошибка при очистке папки",
                             "Не удалось очистить папку временных файлов: \n" + tmp_folder +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_save_acts->setEnabled(true);
        ui->pan_progress->setVisible(false);
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
        ui->bn_save_acts->setEnabled(true);
        ui->pan_progress->setVisible(false);
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
        ui->bn_save_acts->setEnabled(true);
        ui->pan_progress->setVisible(false);
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
        ui->bn_save_acts->setEnabled(true);
        ui->pan_progress->setVisible(false);
        return;
    }
    // выбор шаблона
    int pos_start = 0;

    int pos_mo_start0 = s_content.indexOf("<office:text text:use-soft-page-breaks=\"true\">", pos_start) + 48;
    int pos_mo_start1 = s_content.indexOf("<table:table-row table:style-name=\"Таблица1.2\">", pos_mo_start0);

    int pos_mp_start0 = s_content.indexOf("<table:table-row table:style-name=\"Таблица1.2\">", pos_mo_start1);
    int pos_mp_start1 = s_content.indexOf("</table:table-row>", pos_mp_start0) + 18;

    int pos_ms_type_start0 = s_content.indexOf("<table:table-row table:style-name=\"Таблица1.3\">", pos_mp_start1);
    int pos_ms_type_start1 = s_content.indexOf("</table:table-row>", pos_ms_type_start0) + 18;

    int pos_ms0 = s_content.indexOf("<table:table-row table:style-name=\"Таблица1.4\">", pos_ms_type_start1);
    int pos_ms1 = s_content.indexOf("</table:table-row>", pos_ms0) + 18;

    int pos_ms_type_end0 = s_content.indexOf("<table:table-row table:style-name=\"Таблица1.5\">", pos_ms1);
    int pos_ms_type_end1 = s_content.indexOf("</table:table-row>", pos_ms_type_end0) + 18;

    int pos_mp_end0 = s_content.indexOf("<table:table-row table:style-name=\"Таблица1.6\">", pos_ms_type_end1);
    int pos_mp_end1 = s_content.indexOf("</table:table-row>", pos_mp_end0) + 18;

    int pos_mo_end0 = s_content.indexOf("<table:table-row table:style-name=\"Таблица1.7\">", pos_mp_end1);
    int pos_mo_end1 = s_content.indexOf("</office:text>", pos_mo_end0);

    int pos_end = s_content.size();

    QString s_start         = s_content.mid ( pos_start          , pos_mo_start0      - pos_start          );
    QString s_mo_start      = s_content.mid ( pos_mo_start0      , pos_mo_start1      - pos_mo_start0      );
    QString s_mp_start      = s_content.mid ( pos_mp_start0      , pos_mp_start1      - pos_mp_start0      );
    QString s_ms_type_start = s_content.mid ( pos_ms_type_start0 , pos_ms_type_start1 - pos_ms_type_start0 );
    QString s_ms            = s_content.mid ( pos_ms0            , pos_ms1            - pos_ms0            );
    QString s_ms_type_end   = s_content.mid ( pos_ms_type_end0   , pos_ms_type_end1   - pos_ms_type_end0   );
    QString s_mp_end        = s_content.mid ( pos_mp_end0        , pos_mp_end1        - pos_mp_end0        );
    QString s_mo_end        = s_content.mid ( pos_mo_end0        , pos_mo_end1        - pos_mo_end0        );
    QString s_end           = s_content.mid ( pos_mo_end1        , pos_end            - pos_mo_end1        );


    // ----------------------- //
    // сформируем начало файла //
    // ----------------------- //
    QString s_content_new = s_start;
    QString str;




    // список МО
    QString sql_mo = "select mo.id, mo.code_mo, mo.name_mo "
                     "  from spr_medorgs mo "
                     " where mo.is_head=1 "
                     " order by mo.code_mo ; ";
    QSqlQuery *query_mo = new QSqlQuery(db);
    if (mySQL.exec(this, sql_mo, "Получим список МО", query_mo, true, db, data_app)) {
        while (query_mo->next()) {
            QString code_mo_lst = query_mo->value(1).toString();

            // покажем код МО на экран
            ui->lab_code_mo->setText(code_mo_lst);
            QApplication::processEvents();


            QString sql_cnt;

            if (f_today) {
                // на сегодня - более быстрый алгоритм
                sql_cnt = "select a.code_mo, mo.name_mo, "
                          "       a.code_mp, mp.name_mp, "
                          "       a.code_ms, ms.name_ms, ms.type_ms, ms_t.text as type_ms_text, "
                          "       count(*) as cnt "
                          "  from persons e "
                          "  left join polises p on(p.id=e.id_polis) "
                          "  left join persons_assig a on(a.id_person=e.id and a.is_actual=0) "
                          "  left join spr_medorgs mo on(mo.code_mo=a.code_mo) "
                          "  left join spr_medparts mp on(mp.code_mo=a.code_mo and mp.code_mp=a.code_mp) "
                          "  left join spr_medsites ms on(ms.code_mo=a.code_mo and ms.code_mp=a.code_mp and ms.code_ms=a.code_ms) "
                          "  left join spr_medsites_types ms_t on(ms_t.code=ms.type_ms) "
                          " where e.status in(11,12,13,14,1,2,3,4) "
                          "   and a.code_mo='" + code_mo_lst + "' "
                          " group by a.code_mo, mo.name_mo, "
                          "          a.code_mp, mp.name_mp, "
                          "          a.code_ms, ms.name_ms, ms.type_ms, ms_t.text "
                          " order by a.code_mo, a.code_mp, ms.type_ms, a.code_ms; ";
            } else {
                // на дату - с учётом исторических данных
                sql_cnt = "select a.code_mo, mo.name_mo, "
                          "       a.code_mp, mp.name_mp, "
                          "       a.code_ms, ms.name_ms, ms.type_ms, ms_t.text as type_ms_text, "
                          "       count(*) as cnt "
                          "  from persons e "
                          "  left join polises p on(p.id=e.id_polis) "
                          "  left join persons_assig a on(a.id_person=e.id and a.assig_date<='" + date_assiglay.toString("yyyy-MM-dd") + "' and (a.unsig_date is NULL or a.unsig_date>'" + date_assiglay.toString("yyyy-MM-dd") + "')) "
                          "  left join spr_medorgs mo on(mo.code_mo=a.code_mo) "
                          "  left join spr_medparts mp on(mp.code_mo=a.code_mo and mp.code_mp=a.code_mp) "
                          "  left join spr_medsites ms on(ms.code_mo=a.code_mo and ms.code_mp=a.code_mp and ms.code_ms=a.code_ms) "
                          "  left join spr_medsites_types ms_t on(ms_t.code=ms.type_ms) "
                          " where st_person(e.id,'" + date_assiglay.toString("yyyy-MM-dd") + "') in(11,12,13,14,1,2,3,4) "
                          "   and a.code_mo='" + code_mo_lst + "' "
                          " group by a.code_mo, mo.name_mo, "
                          "          a.code_mp, mp.name_mp, "
                          "          a.code_ms, ms.name_ms, ms.type_ms, ms_t.text "
                          " order by a.code_mo, a.code_mp, ms.type_ms, a.code_ms; ";
            }
            QSqlQuery *query_cnt = new QSqlQuery(db);

            if (mySQL.exec(this, sql_cnt, "Получим данные прикреплённых к МО " + code_mo_lst, query_cnt, true, db, data_app)) {
                if (query_cnt->size()>0) {
                    bool is_first_line = true;

                    QString code_mo_block = "";
                    QString name_mo_block = "";
                    int     cnt_mo        = 0;
                    float   pay_mo        = 0.0;
                    QString code_mp_block = "";
                    QString name_mp_block = "";
                    int     cnt_mp        = 0;
                    float   pay_mp        = 0.0;
                    int     type_ms_block = -1;
                    QString txt_mst_block = "";
                    int     cnt_mst       = 0;
                    float   pay_mst       = 0.0;

                    while (query_cnt->next()) {
                        QString code_mo = query_cnt->value(0).toString();
                        QString name_mo = query_cnt->value(1).toString();
                        QString code_mp = query_cnt->value(2).toString();
                        QString name_mp = query_cnt->value(3).toString();
                        QString code_ms = query_cnt->value(4).toString();
                        QString name_ms = query_cnt->value(5).toString();
                        int     type_ms = query_cnt->value(6).toInt();
                        QString txt_mst = query_cnt->value(7).toString();
                        int     cnt     = query_cnt->value(8).toInt();

                        if (code_mo=="")  code_mo = " - без кода - ";
                        if (name_mo=="")  name_mo = " - медорганизация без названия - ";
                        if (code_mp=="")  code_mp = " - без кода - ";
                        if (name_mp=="")  name_mp = " - подразделение без названия - ";
                        if (code_ms=="")  code_ms = " - без кода - ";
                        if (name_ms=="")  name_ms = " - врачебный участок без названия - ";
                        if (type_ms==0 )  type_ms = -2;
                        if (txt_mst=="")  txt_mst = " - тип участка не известен - ";

                        // если изменился CODE_MO
                        if (code_mo!=code_mo_block) {
                            // если это не первая медорганизация
                            if (code_mo_block!="") {
                                // закроем последний тип участков
                                str = s_ms_type_end;
                                str = str.
                                        replace("#TYPE_MS#",     txt_mst_block).
                                        replace("#TYPE_MS_CNT#", QString::number(cnt_mst)).
                                        replace("#TYPE_MS_PAY#", "...");
                                s_content_new += str;

                                // закроем последнее подраздаеление
                                str = s_mp_end;
                                str = str.
                                        replace("#CODE_MP#", code_mp_block).
                                        replace("#NAME_MP#", name_mp_block).
                                        replace("#MP_CNT#",  QString::number(cnt_mp)).
                                        replace("#MP_PAY#",  "...");
                                s_content_new += str;

                                // закроем предыдущую медорганизацию
                                str = s_mo_end;
                                str = str.
                                        replace("#ALL_CNT#", QString::number(cnt_mo)).
                                        replace("#ALL_PAY#", "...");
                                s_content_new += str;
                            }
                            // откроем новую медорганизацию
                            str = s_mo_start;
                            str = str.
                                    replace("#CODE_MO#",  code_mo).
                                    replace("#NAME_MO#",  name_mo).
                                    replace("#DATE_REP#", date_rep.toString("dd.MM.yyyy"));
                            s_content_new += str;

                            // откроем новое подразделение
                            str = s_mp_start;
                            str = str.
                                    replace("#CODE_MP#", code_mp).
                                    replace("#NAME_MP#", name_mp);
                            s_content_new += str;

                            // откроем новый тип участков
                            str = s_ms_type_start;
                            str = str.
                                    replace("#TYPE_MS#", txt_mst);
                            s_content_new += str;

                            code_mo_block = code_mo;
                            name_mo_block = name_mo;
                            cnt_mo        = 0;
                            pay_mo        = 0.0;
                            code_mp_block = code_mp;
                            name_mp_block = name_mp;
                            cnt_mp        = 0;
                            pay_mp        = 0.0;
                            type_ms_block = type_ms;
                            txt_mst_block = txt_mst;
                            cnt_mst       = 0;
                            pay_mst       = 0.0;
                        } else

                        // если изменился CODE_MP
                        if (code_mp!=code_mp_block) {
                            // если это не первое подразделение
                            if (code_mo_block!="") {
                                // закроем последний тип участков
                                str = s_ms_type_end;
                                str = str.
                                        replace("#TYPE_MS#",     txt_mst_block).
                                        replace("#TYPE_MS_CNT#", QString::number(cnt_mst)).
                                        replace("#TYPE_MS_PAY#", "...");
                                s_content_new += str;

                                // закроем последнее подраздаеление
                                str = s_mp_end;
                                str = str.
                                        replace("#CODE_MP#", code_mp_block).
                                        replace("#NAME_MP#", name_mp_block).
                                        replace("#MP_CNT#",  QString::number(cnt_mp)).
                                        replace("#MP_PAY#",  "...");
                                s_content_new += str;
                            }
                            // откроем новое подразделение
                            str = s_mp_start;
                            str = str.
                                    replace("#CODE_MP#", code_mp).
                                    replace("#NAME_MP#", name_mp);
                            s_content_new += str;

                            // откроем новый тип участков
                            str = s_ms_type_start;
                            str = str.
                                    replace("#TYPE_MS#", txt_mst);
                            s_content_new += str;

                            code_mp_block = code_mp;
                            name_mp_block = name_mp;
                            cnt_mp        = 0;
                            pay_mp        = 0.0;
                            type_ms_block = type_ms;
                            txt_mst_block = txt_mst;
                            cnt_mst       = 0;
                            pay_mst       = 0.0;
                        } else

                        // если изменился TYPE_MS
                        if (type_ms!=type_ms_block) {
                            // если это не первый тип участков
                            if (type_ms_block!=-1) {
                                // закроем последний тип участков
                                str = s_ms_type_end;
                                str = str.
                                        replace("#TYPE_MS#", txt_mst_block).
                                        replace("#TYPE_MS_CNT#", QString::number(cnt_mst)).
                                        replace("#TYPE_MS_PAY#", "...");
                                s_content_new += str;
                            }
                            // откроем новый тип участков
                            str = s_ms_type_start;
                            str = str.
                                    replace("#TYPE_MS#", txt_mst);
                            s_content_new += str;

                            type_ms_block = type_ms;
                            txt_mst_block = txt_mst;
                            cnt_mst       = 0;
                            pay_mst       = 0.0;
                        }

                        // добавим данные участка
                        str = s_ms;
                        str = str.
                                replace("#CODE_MS#", code_ms).
                                replace("#NAME_MS#", name_ms).
                                replace("#MS_CNT#",  QString::number(cnt)).
                                replace("#MS_PAY#",  "...");
                        s_content_new += str;

                        // увеличим счётчики
                        cnt_mo  += cnt;
                        cnt_mp  += cnt;
                        cnt_mst += cnt;
                    }

                    // закроем последний тип участков
                    str = s_ms_type_end;
                    str = str.
                            replace("#TYPE_MS#",     txt_mst_block).
                            replace("#TYPE_MS_CNT#", QString::number(cnt_mst)).
                            replace("#TYPE_MS_PAY#", "...");
                    s_content_new += str;

                    // закроем последнее подраздаеление
                    str = s_mp_end;
                    str = str.
                            replace("#CODE_MP#", code_mp_block).
                            replace("#NAME_MP#", name_mp_block).
                            replace("#MP_CNT#",  QString::number(cnt_mp)).
                            replace("#MP_PAY#",  "...");
                    s_content_new += str;

                    // закроем предыдущую медорганизацию
                    str = s_mo_end;
                    str = str.
                            replace("#ALL_CNT#", QString::number(cnt_mo)).
                            replace("#ALL_PAY#", "...");
                    s_content_new += str;
                }
            }
            delete query_cnt;
        }
    }

    // закроем файл контента
    s_content_new += s_end;

    delete query_mo;

    // сохранение контента шаблона
    QFile f_content_save(tmp_folder + "/content.xml");
    f_content_save.open(QIODevice::WriteOnly);
    f_content_save.write(s_content_new.toUtf8());
    f_content_save.close();


    // ===========================
    // архивация шаблона
    // ===========================
    QString fname_res = data_app.path_out + QDate::currentDate().toString("yyyy-MM-dd") + "  -  " + data_app.smo_short + " - " + data_app.filial_name + "  -  Акт сверки численности прекреплённых к медорганизациям на дату " + date_rep.toString("dd.MM.yyyy") + ".odt";


    QFile::remove(fname_res);
    while (QFile::exists(fname_res)) {
        if (QMessageBox::warning(this,
                                 "Ошибка сохранения акта",
                                 "Не удалось сохранить сформированный акт: \n" + rep_template +
                                 "\n\nво временную папку\n" + fname_res +
                                 "\n\nПроверьте, не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                 QMessageBox::Retry|QMessageBox::Abort,
                                 QMessageBox::Retry)==QMessageBox::Abort) {
            this->setCursor(Qt::ArrowCursor);
            ui->bn_save_acts->setEnabled(true);            
            ui->pan_progress->setVisible(false);
            return;
        }
        QFile::remove(fname_res);
    }
    if (!QFile::copy(rep_template, fname_res) || !QFile::exists(fname_res)) {
        QMessageBox::warning(this, "Ошибка копирования шаблона",
                             "Не удалось скопировать сформированный акт: \n" + rep_template +
                             "\n\nво временную папку\n" + fname_res +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_save_acts->setEnabled(true);
        ui->pan_progress->setVisible(false);
        return;
    }
    com = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname_res + "\" \"" + tmp_folder + "/content.xml\"";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка обновления контента",
                             "При добавлении нового контента в шаблон печатной формы заявления произошла непредвиденная ошибка\n\n"
                             "Опреация прервана.");
    }


    // ===========================
    // собственно открытие шаблона
    // ===========================
    // открытие полученного ODT
    long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);
    this->setCursor(Qt::ArrowCursor);
    if (result<32) {
        QMessageBox::warning(this,
                             "Ошибка при открытии акта",
                             "Сформированный акт не найден:\n\n" + fname_res +
                             "\nКод ошибки: " + QString::number(result) +
                             "\n\nПопробуйте открыть этот файл вручную.\n\n"
                             "Если файл не создан или ошибка будет повторяться - обратитесь к разработчику.");
    }
    this->setCursor(Qt::ArrowCursor);
    ui->bn_save_acts->setEnabled(true);
    ui->pan_progress->setVisible(false);
}

void print_assiglay_todate_wnd::on_ch_bymo_clicked(bool checked) {
    //ui->bn_save_acts->setVisible(!checked);
}

void print_assiglay_todate_wnd::on_bn_save_csv_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    ui->pan_progress->setVisible(true);

    date_assiglay = ui->date_assiglay->date();

    if (!ui->ch_bymo->isChecked()) {
        // сфоримруем общий список и покажем в окне
        QString sql_assig = "select e.id, e.FAM, e.IM, e.OT, e.sex, e.date_birth, e.snils, "
                            //"       udl.doc_type, udl.doc_ser, udl.doc_num, drp.doc_type, drp.doc_ser, drp.doc_num, "
                            "       /*p.pol_v, p.pol_ser, p.pol_num, p.vs_num,*/ p.enp, "
                            "       a.code_mo, a.code_mp, a.code_ms, "
                            + QString( data_app.ocato=="14000" ? " case ms.type_ms when NULL then -666 when 1 then 2 when 2 then 1 when 3 then 4 when 4 then 4 when 5 then 3 end as type_ms, " : " ms.type_ms, " ) +
                            "       a.snils_mt, a.assig_date, a.unsig_date, "
                            "       a.is_actual, e.code_1st_aid "
                            "  from persons e "
                            "  left join persons_docs udl on (udl.id=e.id_udl) "
                            "  left join persons_docs drp on (drp.id=e.id_drp) "
                            "  left join polises p on (p.id=e.id_polis) "
                            "  left join (select aa.id_person, max(aa.id) as id from persons_assig aa where aa.assig_date<='" + date_assiglay.toString("yyyy-MM-dd") + "' and (aa.unsig_date is NULL or aa.unsig_date>'" + date_assiglay.toString("yyyy-MM-dd") + "') group by aa.id_person) qa on(qa.id_person=e.id) "
                            "  left join persons_assig a on(a.id=qa.id) "
                            "  left join spr_medorgs mo on(mo.code_mo=a.code_mo) "
                            "  left join spr_medsites ms on(ms.code_mo=a.code_mo and /*ms.code_mp=a.code_mp and*/ ms.code_ms=a.code_ms) "
                            " where st_person(e.id,'" + date_assiglay.toString("yyyy-MM-dd") + "') in(11,12,13,14,1,2,3,4) "
                            " order by e.fam, e.im, e.ot, e.date_birth ; ";

        delete show_tab_w;
        QString gen_dir = data_app.path_out + "_ASSIG_DATA_/";
        show_tab_w = new show_tab_wnd("Срез по прикреплению застрахованных к МО, на " + date_assiglay.toString("dd.MM.yyyy") + "г", sql_assig, db, data_app, this, gen_dir);
        show_tab_w->exec();

    } else {
        // сформируем файлы для каждой МО отдельно и сохраним в папку
        QString gen_dir = data_app.path_out + "_ASSIG_DATA_/" + ui->date_assiglay->date().toString("yyyy-MM-dd") + "/";
        QDir dir(gen_dir);
        bool res_mk = dir.mkpath(gen_dir);

        // выберем коды МО, для которых есть данные
        QString sql_codes = "select mo.code_head, count(*) "
                            "  from persons e "
                            "  left join polises p on ( p.id=e.id_polis ) "
                            "  left join (select aa.id_person, max(aa.id) as id from persons_assig aa where aa.assig_date<='" + date_assiglay.toString("yyyy-MM-dd") + "' and (aa.unsig_date is NULL or aa.unsig_date>'" + date_assiglay.toString("yyyy-MM-dd") + "') group by aa.id_person) qa on(qa.id_person=e.id) "
                            "  left join persons_assig a on(a.id=qa.id) "
                            "  left join spr_medorgs mo on(mo.code_mo=a.code_mo) "
                            " where (select st_person(e.id,'" + date_assiglay.toString("yyyy-MM-dd") + "')) in(11,12,13,14,1,2,3,4) "
                            " group by mo.code_head "
                            " order by mo.code_head ; ";
        QSqlQuery *query_codes = new QSqlQuery(db);
        bool res_codes = mySQL.exec(this, sql_codes, QString("выберем коды МО по которым есть данные"), *query_codes, true, db, data_app);
        if (!res_codes) {
            delete query_codes;
            this->setCursor(Qt::ArrowCursor);
            db.rollback();
            QMessageBox::warning(this, "Ошибка при проверке кодов МО по которым есть данные", "При проверке кодов МО по которым есть данные произошла неожиданная ошибка");
            ui->pan_progress->setVisible(false);
            return;
        }

        while (query_codes->next()) {
            QString code_head = query_codes->value(0).toString();
            int cnt = query_codes->value(1).toInt();
            if (cnt<1)
                continue;
            if (code_head.isEmpty())
                continue;

            // покажем код МО на экран
            ui->lab_code_mo->setText(code_head);
            QApplication::processEvents();

            // выберем данные прикрепления к этой МО
            QString sql_assig = "select e.id, e.FAM, e.IM, e.OT, e.sex, e.date_birth, e.snils, "
                                //"       udl.doc_type, udl.doc_ser, udl.doc_num, drp.doc_type, drp.doc_ser, drp.doc_num, "
                                "       /*p.pol_v, p.pol_ser, p.pol_num, p.vs_num,*/ p.enp, "
                                "       a.code_mo, a.code_mp, a.code_ms, "
                                + QString( data_app.ocato=="14000" ? " case ms.type_ms when NULL then -666 when 1 then 2 when 2 then 1 when 3 then 4 when 4 then 4 when 5 then 3 end as type_ms, " : " ms.type_ms, " ) +
                                "       a.snils_mt, a.assig_date, a.unsig_date, "
                                "       a.is_actual, e.code_1st_aid "
                                "  from persons e "
                                //"  left join persons_docs udl on (udl.id=e.id_udl) "
                                //"  left join persons_docs drp on (drp.id=e.id_drp) "
                                "  left join polises p on (p.id=e.id_polis) "
                                "  left join (select aa.id_person, max(aa.id) as id from persons_assig aa where aa.assig_date<='" + date_assiglay.toString("yyyy-MM-dd") + "' and (aa.unsig_date is NULL or aa.unsig_date>'" + date_assiglay.toString("yyyy-MM-dd") + "') group by aa.id_person) qa on(qa.id_person=e.id) "
                                "  left join persons_assig a on(a.id=qa.id) "
                                "  left join spr_medorgs mo on(mo.code_mo=a.code_mo) "
                                "  left join spr_medsites ms on(ms.code_mo=a.code_mo and ms.code_ms=a.code_ms) "
                                " where mo.code_head='" + code_head + "' "
                                "   and st_person(e.id,'" + date_assiglay.toString("yyyy-MM-dd") + "') in(11,12,13,14,1,2,3,4) "
                                " order by e.fam, e.im, e.ot, e.date_birth ; ";

            QSqlQuery *query_mo = new QSqlQuery(db);
            bool res_mo = mySQL.exec(this, sql_assig, QString("выберем данные прикрепления к этой МО"), *query_mo, true, db, data_app);
            if (!res_mo) {
                delete query_mo;
                this->setCursor(Qt::ArrowCursor);
                db.rollback();
                QMessageBox::warning(this, "Ошибка при выборке данных прикрепления к этой МО", "При выборке данных прикрепления к этой МО произошла неожиданная ошибка");
                ui->pan_progress->setVisible(false);
                return;
            }

            // сохраним данные в файл
            QString file_name = "PR" + data_app.smo_regnum + "_" + code_head + "_" + QDate::currentDate().toString("yyMM") + ".csv";
            QString file_name_full = gen_dir + file_name;
            QFile::remove(file_name_full);
            QFile file;
            file.setFileName(file_name_full);
            if (!file.open(QIODevice::WriteOnly)) {
                delete query_mo;
                this->setCursor(Qt::ArrowCursor);
                db.rollback();
                QMessageBox::warning(this, "Ошибка создания файла",
                                     "При создании целевого файла \n" + file_name_full + "\nпроизошла ошибка.\n\nОперация отменена.");
                ui->pan_progress->setVisible(false);
                return;
            }

            // подготовим поток для сохранения файла
            QTextStream stream(&file);
            QString codec_name = "Windows-1251";
            QTextCodec *codec = QTextCodec::codecForName(codec_name.toLocal8Bit());
            stream.setCodec(codec);

            // добавим шапку
            QString head = "id;fam;im;ot;sex;date_birth;snils;enp;code_mo;code_mp;code_ms;type_ms;snils_mt;assig_date;unsig_date;is_actual;code_1st_aid";
            stream << head << "\n";

            // добавим новые данные
            while (query_mo->next()) {
                int     id          = query_mo->value( 0).toInt();
                QString fam         = query_mo->value( 1).toString();
                QString im          = query_mo->value( 2).toString();
                QString ot          = query_mo->value( 3).toString();
                int     sex         = query_mo->value( 4).toInt();
                QDate   date_birth  = query_mo->value( 5).toDate();
                QString enp         = query_mo->value( 6).toString();
                QString snils       = query_mo->value( 7).toString();
                QString code_mo     = query_mo->value( 8).toString();
                QString code_mp     = query_mo->value( 9).toString();
                QString code_ms     = query_mo->value(10).toString();
                int     type_ms     = query_mo->value(11).toInt();
                QString snils_mt    = query_mo->value(12).toString();
                QDate   assig_date  = query_mo->value(13).toDate();
                QDate   unsig_date  = query_mo->value(14).toDate();
                int     is_actual   = query_mo->value(15).toInt();
                QString first_aid   = query_mo->value(16).toString();
                stream << QString::number(id)  << ";";
                stream << fam << ";";
                stream << im  << ";";
                stream << ot  << ";";
                stream << QString::number(sex) << ";";
                stream << date_birth.toString("yyyy-MM-dd") << ";";
                stream << enp << ";";
                stream << snils << ";";
                stream << code_mo << ";";
                stream << code_mp << ";";
                stream << code_ms << ";";
                stream << QString::number(type_ms) << ";";
                stream << snils_mt << ";";
                stream << assig_date.toString("yyyy-MM-dd") << ";";
                stream << unsig_date.toString("yyyy-MM-dd") << ";";
                stream << QString::number(is_actual) << ";";
                stream << first_aid <<"\n";
            }
            file.close();
            delete query_mo;
        }

        // ===========================
        // собственно открытие шаблона
        // ===========================
        /*long result = (long)*/ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(gen_dir.utf16()), 0, 0, SW_NORMAL);
    }

    ui->pan_progress->setVisible(false);
}

void print_assiglay_todate_wnd::on_bn_save_dbf_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    ui->pan_progress->setVisible(true);

    db_ODBC.close();

    // подключимся к ODBC
    db_ODBC = QSqlDatabase::addDatabase("QODBC", "INKO_DBF");
    db_ODBC.setConnectOptions();
    // имя источника данных
    db_ODBC.setDatabaseName("INKO_DBF");
    while (!db_ODBC.open()) {
        if (QMessageBox::warning(this, "Не найден ODBC-источник данных INKO_DBF",
                                 QString("Источник данных должен использовать папку, указанную в параметре программы \"path_dbf\":\n"
                                 "При попытке подключиться к испточнику данных ODBC \"INKO_DBF\" операционная система вернула отказ. \n"
                                 "Этот источник данных необходим для генерации и чтения dbf-файлов обмена с ТФОМС. \n"
                                 "Если такого источника данных нет - создайте его.\n\n")
                                 + data_app.path_dbf + "\n\n"
                                 "Попробовать снова?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::No) {
            return;
        }
    }

    date_assiglay = ui->date_assiglay->date();

    if (!ui->ch_bymo->isChecked()) {
        QString gen_dir = data_app.path_out + "_ASSIG_DATA_/" + ui->date_assiglay->date().toString("yyyy-MM-dd") + "/";
        QDir dir(gen_dir);
        bool res_mk = dir.mkpath(gen_dir);

        // подготовим файл DBF
        QString f_name = data_app.path_dbf + "BelAssig.dbf";
        QFile::remove(f_name);
        QFile::copy(data_app.path_dbf + "BelAssig_src.dbf", f_name);

        // принудительно почистим файл DBF
        QString sql_del = "delete from BelAssig where 1=1";
        QSqlQuery *query_del = new QSqlQuery(db_ODBC);
        bool res_del = query_del->exec(sql_del);
        query_del->next();
        /*if (!res_del) {
            delete query_del;
            this->setCursor(Qt::ArrowCursor);
            QMessageBox::warning(this, "Ошибка при попытке добавить строку в DBF", "При попытке добавить строку в DBF произошла неожиданная ошибка");
            ui->pan_progress->setVisible(false);
            return;
        }*/
        delete query_del;

        // покажем "код МО" на экран
        ui->lab_code_mo->setText(" - ВСЕ МО - ");
        QApplication::processEvents();

        // сфоримруем общий список и покажем в окне
        QString sql_assig = "select smo.value as id_smo, "
                            "       e.FAM, e.IM, e.OT, e.date_birth as dr, "
                            "       udl.doc_type, udl.doc_ser, udl.doc_num,"
                            "       e.snils as ss, e.plc_birth as mr, e.enp, "
                            "       p.pol_v, p.pol_ser, p.pol_num, p.date_begin, p.date_end, "
                            "       a.code_mo, a.code_ms, "
                            //+ QString( data_app.ocato=="14000" ? " case ms.type_ms when NULL then -666 when 1 then 2 when 2 then 1 when 3 then 4 when 4 then 4 when 5 then 3 end as type_ms, " : " ms.type_ms, " ) +
                            "       a.snils_mt, "
                            "       coalesce(dsr.okato, dsl.okato) as rn, "
                            "       coalesce(ar.strt_name, al.strt_name) as ul, "
                            "       coalesce(ar.house, al.house) as dom, "
                            "       coalesce(ar.corp, al.corp) as kor, "
                            "       coalesce(ar.quart, al.quart) as kv, "
                            "       p.vs_num "
                            "  from persons e "
                            "  left join db_data smo on(smo.name='smo_regnum') "
                            "  left join persons_docs udl on (udl.id=e.id_udl) "
                            "  left join persons_docs drp on (drp.id=e.id_drp) "
                            "  left join polises p on (p.id=e.id_polis) "
                            "  left join (select aa.id_person, max(aa.id) as id from persons_assig aa where aa.assig_date<='" + date_assiglay.toString("yyyy-MM-dd") + "' and (aa.unsig_date is NULL or aa.unsig_date>'" + date_assiglay.toString("yyyy-MM-dd") + "') group by aa.id_person) qa on(qa.id_person=e.id) "
                            "  left join persons_assig a on(a.id=qa.id) "
                            "  left join spr_medorgs mo on(mo.code_mo=a.code_mo) "
                            "  left join spr_medsites ms on(ms.id=(select max(ms1.id) from spr_medsites ms1 where ms1.code_mo=a.code_mo and /*ms.code_mp=a.code_mp and*/ ms1.code_ms=a.code_ms) ) "
                            "  left join addresses ar on(ar.id=e.id_addr_reg) "
                            "  left join fias.dstr dsr on(dsr.subj=ar.subj and dsr.dstr=ar.dstr) "
                            "  left join addresses al on(al.id=e.id_addr_liv) "
                            "  left join fias.dstr dsl on(dsl.subj=al.subj and dsl.dstr=al.dstr) "
                            " where st_person(e.id,'" + date_assiglay.toString("yyyy-MM-dd") + "')>0 "
                            " order by e.fam, e.im, e.ot, e.date_birth ; ";
        QSqlQuery *query_assig = new QSqlQuery(db);
        bool res_assig = mySQL.exec(this, sql_assig, QString("выберем данные прикрепления - полный срез"), *query_assig, true, db, data_app);
        if (!res_assig) {
            delete query_assig;
            this->setCursor(Qt::ArrowCursor);
            QMessageBox::warning(this, "Ошибка при выыборке данных прикрепления - полный срез", "При выыборке данных прикрепления - полный срез, произошла неожиданная ошибка");
            ui->pan_progress->setVisible(false);
            return;
        }

        int n = 0;
        while (query_assig->next()) {
            QString id_smo    = query_assig->value( 0).toString();
            QString fam       = query_assig->value( 1).toString();
            QString im        = query_assig->value( 2).toString();
            QString ot        = query_assig->value( 3).toString();
            QDate   dr        = query_assig->value( 4).toDate();
            int     doctp     = query_assig->value( 5).toInt();
            QString docs      = query_assig->value( 6).toString();
            QString docn      = query_assig->value( 7).toString();
            QString ss        = query_assig->value( 8).toString();
            QString mr        = query_assig->value( 9).toString();
            QString enp       = query_assig->value(10).toString();
            int     poltp     = query_assig->value(11).toInt();
            QString spol      = query_assig->value(12).toString();
            QString npol      = query_assig->value(13).toString();
            QDate   dbeg      = query_assig->value(14).toDate();
            QDate   dend      = query_assig->value(15).toDate();
            QString lpu       = query_assig->value(16).toString();
            QString lpuuch    = query_assig->value(17).toString();
            QString ss_doctor = query_assig->value(18).toString();
            QString rn        = query_assig->value(19).toString();
            QString ul        = query_assig->value(20).toString();
            QString dom       = query_assig->value(21).toString();
            QString kor       = query_assig->value(22).toString();
            QString kv        = query_assig->value(23).toString();
            int     vs_num    = query_assig->value(24).toInt();

            ss = ss.replace("-","").replace(" ","");
            ss_doctor = ss_doctor.replace("-","").replace(" ","");
            if ( spol.isEmpty()
                 && npol.isEmpty()
                 && vs_num>10000 ) {
                QString vs_num_s = "000000000000" + QString::number(vs_num);
                vs_num_s = vs_num_s.right(11);
                spol  = vs_num_s.left(4);
                npol  = vs_num_s.right(7);
            }

            // добавим строку в DBF
            QString sql_add;
            sql_add +=  "insert into BelAssig ( ID_SMO, FAM, IM, OT, DR, DOCTP, DOCS, DOCN, SS, MR, ENP, POLTP, SPOL, NPOL, DBEG, DEND, LPU, LPUUCH, SS_DOCTOR, RN, UL, DOM, KOR, KV ) "
                        "values (  " + id_smo + " "
                        "       , '" + fam + "' "
                        "       , '" + im  + "' "
                        "       , '" + ot  + "' "
                        "       ,  " + QString("'"+ dr.toString("yyyy-MM-dd") +"'") + " "
                        "       , '" + QString::number(doctp) + "' "
                        "       , '" + docs + "' "
                        "       , '" + docn + "' "
                        "       , '" + ss + "' "
                        "       , '" + mr + "' "
                        "       , '" + enp + "' "
                        "       ,  " + QString::number(poltp) + " "
                        "       , '" + spol + "' "
                        "       , '" + npol + "' "
                        "       ,  " + (dbeg.isValid() ? QString("'"+ dbeg.toString("yyyy-MM-dd") +"'") : "NULL") + " "
                        "       ,  " + (dend.isValid() ? QString("'"+ dend.toString("yyyy-MM-dd") +"'") : "NULL") + " "
                        "       , '" + lpu + "' "
                        "       , '" + lpuuch + "' "
                        "       , '" + ss_doctor + "' "
                        "       , '" + rn + "' "
                        "       , '" + ul + "' "
                        "       , '" + dom + "' "
                        "       , '" + kor + "' "
                        "       , '" + kv + "' "
                        "       ) ";

            QSqlQuery *query_add = new QSqlQuery(db_ODBC);
            bool res_add = query_add->exec(sql_add);
            QString err = db_ODBC.lastError().databaseText();
            if (!res_add) {
                this->setCursor(Qt::ArrowCursor);
                QMessageBox::warning(this, "Ошибка при попытке добавить строку в DBF",
                                     "При попытке добавить строку в DBF произошла неожиданная ошибка\n\n"
                                     + query_add->lastError().databaseText() + "\n\n"
                                     + query_add->lastError().driverText() + "\n\n"
                                     " (Может быть вы открыли целевой DBF-файл в редакторе ?:) ");
                ui->pan_progress->setVisible(false);
                delete query_add;
                return;
            }
            delete query_add;
            n++;
            if (n%231==0) {
                ui->lab_code_mo->setText(" - ВСЕ МО - " + QString::number(n));
                QApplication::processEvents();
            }
        }
        n++;
        ui->lab_code_mo->setText(" - ВСЕ МО - " + QString::number(n));
        QApplication::processEvents();

        // переименуем целевой файл
        QString f_name_new = f_name;        
        f_name_new = gen_dir + "SVER31006_" + date_assiglay.toString("yyyy") + "_" + date_assiglay.toString("MM") + "_" + date_assiglay.toString("dd") + ".dbf";

        QFile::rename(f_name, f_name_new);

        // ===========================
        // собственно открытие шаблона
        // ===========================
        /*long result = (long)*/ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(f_name_new.utf16()), 0, 0, SW_NORMAL);

    } else {
        // сформируем файлы для каждой МО отдельно и сохраним в папку
        QString gen_dir = data_app.path_out + "_ASSIG_DATA_/" + ui->date_assiglay->date().toString("yyyy-MM-dd") + "/";
        QDir dir(gen_dir);
        bool res_mk = dir.mkpath(gen_dir);

        // выберем коды МО, для которых есть данные
        QString sql_codes = "select mo.code_head, count(*) "
                            "  from persons e "
                            "  left join polises p on ( p.id=e.id_polis ) "
                            "  left join (select aa.id_person, max(aa.id) as id from persons_assig aa where aa.assig_date<='" + date_assiglay.toString("yyyy-MM-dd") + "' and (aa.unsig_date is NULL or aa.unsig_date>'" + date_assiglay.toString("yyyy-MM-dd") + "') group by aa.id_person) qa on(qa.id_person=e.id) "
                            "  left join persons_assig a on(a.id=qa.id) "
                            "  left join spr_medorgs mo on(mo.code_mo=a.code_mo) "
                            " where (select st_person(e.id,'" + date_assiglay.toString("yyyy-MM-dd") + "')) in(11,12,13,14,1,2,3,4) "
                            " group by mo.code_head ; ";
        QSqlQuery *query_codes = new QSqlQuery(db);
        bool res_codes = mySQL.exec(this, sql_codes, QString("выберем коды МО по которым есть данные"), *query_codes, true, db, data_app);
        if (!res_codes) {
            delete query_codes;
            this->setCursor(Qt::ArrowCursor);
            QMessageBox::warning(this, "Ошибка при проверке кодов МО по которым есть данные", "При проверке кодов МО по которым есть данные произошла неожиданная ошибка");
            ui->pan_progress->setVisible(false);
            return;
        }

        while (query_codes->next()) {
            QString code_head = query_codes->value(0).toString();
            int cnt = query_codes->value(1).toInt();
            if (cnt<1)
                continue;
            if (code_head.isEmpty())
                continue;

            // подготовим файл DBF
            QFile::remove(data_app.path_dbf + "rep_ASG.dbf");
            QFile::copy(data_app.path_dbf + "rep_ASG_src.dbf", data_app.path_dbf + "rep_ASG.dbf");

            // принудительно почистим файл DBF
            QString sql_del = "delete from rep_ASG ; ";
            QSqlQuery *query_del = new QSqlQuery(db_ODBC);
            bool res_del = query_del->exec(sql_del);
            if (!res_del) {
                delete query_del;
                this->setCursor(Qt::ArrowCursor);
                QMessageBox::warning(this, "Ошибка при попытке добавить строку в DBF", "При попытке добавить строку в DBF произошла неожиданная ошибка");
                ui->pan_progress->setVisible(false);
                return;
            }
            delete query_del;

            // покажем код МО на экран
            ui->lab_code_mo->setText(code_head);
            QApplication::processEvents();

            // выберем данные прикрепления к этой МО
            QString sql_assig = "select e.FAM, e.IM, e.OT, e.sex, e.date_birth, e.snils, "
                                "       udl.doc_type, udl.doc_ser, udl.doc_num, drp.doc_type, drp.doc_ser, drp.doc_num, "
                                "       p.pol_v, p.pol_ser, p.pol_num, p.vs_num, "
                                "       a.assig_date, a.unsig_date, a.code_mo, a.code_ms, "
                                + QString( data_app.ocato=="14000" ? " case ms.type_ms when NULL then -666 when 1 then 2 when 2 then 1 when 3 then 4 when 4 then 4 when 5 then 3 end as type_ms, " : " ms.type_ms, " ) +
                                "       a.snils_mt "
                                "  from persons e "
                                "  left join persons_docs udl on (udl.id=e.id_udl) "
                                "  left join persons_docs drp on (drp.id=e.id_drp) "
                                "  left join polises p on (p.id=e.id_polis) "
                                "  left join (select aa.id_person, max(aa.id) as id from persons_assig aa where aa.assig_date<='" + date_assiglay.toString("yyyy-MM-dd") + "' and (aa.unsig_date is NULL or aa.unsig_date>'" + date_assiglay.toString("yyyy-MM-dd") + "') group by aa.id_person) qa on(qa.id_person=e.id) "
                                "  left join persons_assig a on(a.id=qa.id) "
                                "  left join spr_medorgs mo on(mo.code_mo=a.code_mo) "
                                "  left join spr_medsites ms on(ms.code_mo=a.code_mo and ms.code_ms=a.code_ms) "
                                " where mo.code_head='" + code_head + "' "
                                "   and st_person(e.id,'" + date_assiglay.toString("yyyy-MM-dd") + "') in(11,12,13,14,1,2,3,4) "
                                " order by e.fam, e.im, e.ot, e.date_birth ; ";
            QSqlQuery *query_assig = new QSqlQuery(db);
            bool res_assig = mySQL.exec(this, sql_assig, QString("выберем данные прикрепления - полный срез"), *query_assig, true, db, data_app);
            if (!res_assig) {
                delete query_assig;
                this->setCursor(Qt::ArrowCursor);
                QMessageBox::warning(this, "Ошибка при выыборке данных прикрепления - полный срез", "При выыборке данных прикрепления - полный срез, произошла неожиданная ошибка");
                ui->pan_progress->setVisible(false);
                return;
            }

            int n=0;
            while (query_assig->next()) {
                QString fam        = query_assig->value( 0).toString();
                QString im         = query_assig->value( 1).toString();
                QString ot         = query_assig->value( 2).toString();
                int     sex        = query_assig->value( 3).toInt();
                QDate   date_birth = query_assig->value( 4).toDate();
                QString snils      = query_assig->value( 5).toString();
                int     udl_type   = query_assig->value( 6).toInt();
                QString udl_ser    = query_assig->value( 7).toString();
                QString udl_num    = query_assig->value( 8).toString();
                int     drp_type   = query_assig->value( 9).toInt();
                QString drp_ser    = query_assig->value(10).toString();
                QString drp_num    = query_assig->value(11).toString();
                int     pol_v      = query_assig->value(12).toInt();
                QString pol_ser    = query_assig->value(13).toString();
                QString pol_num    = query_assig->value(14).toString();
                int     vs_num     = query_assig->value(15).toInt();
                QDate   assig_date = query_assig->value(16).toDate();
                QDate   unsig_date = query_assig->value(17).toDate();
                QString code_mo    = query_assig->value(18).toString();
                QString code_ms    = query_assig->value(19).toString();
                int     type_ms    = query_assig->value(20).toInt();
                QString snils_mt   = query_assig->value(21).toString();

                snils = snils.replace("-","").replace(" ","");
                snils_mt = snils_mt.replace("-","").replace(" ","");
                if ( pol_ser.isEmpty()
                     && pol_num.isEmpty()
                     && vs_num>0 ) {
                    QString vs_num_s = "000000000000" + QString::number(vs_num);
                    vs_num_s = vs_num_s.right(11);
                    pol_ser  = vs_num_s.left(4);
                    pol_num  = vs_num_s.right(7);
                }

                // добавим строку в DBF
                QString sql_add = "insert into rep_ASG ( FAM, IM, OT, W, DR, SS, DOCTP, DOCS, DOCN, RDOCTP, RDOCS, RDOCN, POLTP, SPOL, NPOL, DBEG, DEND, LPU, LPUUCH, TYP_UCH, SS_DOCTOR ) "
                                  "values ( '" + fam + "' "
                                  "       , '" + im  + "' "
                                  "       , '" + ot  + "' "
                                  "       ,  " + QString::number(sex) + " "
                                  "       ,  " + QString("'"+ date_birth.toString("yyyy-MM-dd") +"'") + " "
                                  "       , '" + snils + "' "
                                  "       , '" + QString::number(udl_type) + "' "
                                  "       , '" + udl_ser + "' "
                                  "       , '" + udl_num + "' "
                                  "       , '" + QString::number(drp_type) + "' "
                                  "       , '" + drp_ser + "' "
                                  "       , '" + drp_num + "' "
                                  "       ,  " + QString::number(pol_v) + " "
                                  "       , '" + pol_ser + "' "
                                  "       , '" + pol_num + "' "
                                  "       ,  " + (assig_date.isValid() ? QString("'"+ assig_date.toString("yyyy-MM-dd") +"'") : "NULL") + " "
                                  "       ,  " + (unsig_date.isValid() ? QString("'"+ unsig_date.toString("yyyy-MM-dd") +"'") : "NULL") + " "
                                  "       , '" + code_mo + "' "
                                  "       , '" + code_ms + "' "
                                  "       ,  " + QString::number(type_ms) + " "
                                  "       , '" + snils_mt + "' "
                                  "       ) ";

                QSqlQuery *query_add = new QSqlQuery(db_ODBC);
                bool res_add = query_add->exec(sql_add);
                if (!res_add) {
                    this->setCursor(Qt::ArrowCursor);
                    QMessageBox::warning(this, "Ошибка при попытке добавить строку в DBF",
                                         "При попытке добавить строку в DBF произошла неожиданная ошибка\n\n"
                                         + query_add->lastError().databaseText() + "\n\n"
                                         + query_add->lastError().driverText() + "\n\n"
                                         " (Может быть вы открыли целевой DBF-файл в редакторе ?:) ");
                    ui->pan_progress->setVisible(false);
                    delete query_add;
                    return;
                }
                delete query_add;
                n++;
                if (n%231==0) {
                    ui->lab_code_mo->setText(code_head + " - " + QString::number(n));
                    QApplication::processEvents();
                }
            }
            n++;
            ui->lab_code_mo->setText(code_head + " - " + QString::number(n));
            QApplication::processEvents();
            delete query_assig;
            this->setCursor(Qt::ArrowCursor);

            // скопируем готовый файл в целевую папку
            QString f_name = gen_dir + QDate::currentDate().toString("yyyy-MM-dd") + " - прикреплённые к МО " + code_head + " на дату " + date_assiglay.toString("yyyy-MM-dd") + ".dbf";
            QFile::remove(f_name);
            bool res_copy = QFile::copy(data_app.path_dbf + "rep_ASG.dbf", f_name);
            res_copy = res_copy;
        }

        // ===========================
        // собственно открытие шаблона
        // ===========================
        /*long result = (long)*/ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(gen_dir.utf16()), 0, 0, SW_NORMAL);
    }

    ui->pan_progress->setVisible(false);
}

void print_assiglay_todate_wnd::on_bn_save_csv_softrust_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    ui->pan_progress->setVisible(true);

    date_assiglay = ui->date_assiglay->date();

    if (!ui->ch_bymo->isChecked()) {
        QString gen_dir = data_app.path_out + "_ASSIG_DATA_/" + ui->date_assiglay->date().toString("yyyy-MM-dd") + "/";
        QDir dir(gen_dir);
        bool res_mk = dir.mkpath(gen_dir);

        // сфоримруем общий список и покажем в окне
        QString sql_assig = "select distinct "
                            "       e.id, e.FAM, e.IM, e.OT, e.date_birth, e.sex, "
                            "       coalesce(adr.subj_name,'')||', '||coalesce(adr.dstr_name,'')||', '||coalesce(adr.city_name,'')||', '||coalesce(adr.nasp_name,'')||', '||coalesce(adr.strt_name,'')||', '||coalesce(cast(adr.house as text),'')||' '||coalesce(adr.corp,'')||', '||coalesce(adr.quart,'') as address, "
                            + QString( data_app.ocato=="14000" ? " case ms.type_ms when NULL then -666 when 1 then 2 when 2 then 1 when 3 then 4 when 4 then 4 when 5 then 3 end as type_ms, " : " ms.type_ms, " ) +
                            "       cast(right(a.code_mo,4) as smallint), a.code_ms, ms.name_ms, "
                            "       case p.pol_v when 1 then 'С' when 2 then 'В' when 3 then 'П' else '?' end as pol_type, "
                            "       coalesce(e.enp, p.enp, p.pol_ser||' '||p.pol_num, p.pol_num) as pol_id "
                            "  from persons e "
                            "  left join addresses adr on(adr.id=coalesce(e.id_addr_liv, e.id_addr_reg)) "
                            "  left join polises p on (p.id=e.id_polis) "

                            "  left join (select aa.id_person, max(aa.id) as id "
                            "               from persons_assig aa "
                            "              where aa.assig_date<='" + date_assiglay.toString("yyyy-MM-dd") + "' "
                            "                and ( aa.unsig_date is NULL "
                            "                   or aa.unsig_date>'" + date_assiglay.toString("yyyy-MM-dd") + "') "
                            "              group by aa.id_person) qa "
                            "       on(qa.id_person=e.id) "
                            "  left join persons_assig a on(a.id=qa.id) "

                            "  left join spr_medorgs mo on(mo.code_mo=a.code_mo) "
                            "  left join spr_medsites ms on(ms.code_mo=a.code_mo and /*ms.code_mp=a.code_mp and*/ ms.code_ms=a.code_ms) "
                            " where st_person(e.id,'" + date_assiglay.toString("yyyy-MM-dd") + "') in(11,12,13,14,1,2,3,4) "
                            " order by e.fam, e.im, e.ot, e.date_birth ; ";

        /*delete show_tab_w;
        QString gen_dir = data_app.path_out + "_ASSIG_DATA_/";
        show_tab_w = new show_tab_wnd("Срез по прикреплению застрахованных к МО, на " + date_assiglay.toString("dd.MM.yyyy") + "г", sql_assig, db, data_app, this, gen_dir);
        show_tab_w->exec();*/

        QSqlQuery *query_assig = new QSqlQuery(db);
        bool res_mo = mySQL.exec(this, sql_assig, QString("выберем данные прикрепления ко всем МО"), *query_assig, true, db, data_app);
        if (!res_mo) {
            delete query_assig;
            this->setCursor(Qt::ArrowCursor);
            db.rollback();
            QMessageBox::warning(this, "Ошибка при выборке данных прикрепления ко всем МО", "При выборке данных прикрепления ко всем МО произошла неожиданная ошибка");
            ui->pan_progress->setVisible(false);
            return;
        }

        // сохраним данные в файл
        QString file_name = "fzp(" + data_app.smo_regnum + "_ALL_MO_" + QDate::currentDate().toString("yyMM") + ").csv";
        QString file_name_full = gen_dir + file_name;
        QFile::remove(file_name_full);
        QFile file;
        file.setFileName(file_name_full);
        if (!file.open(QIODevice::WriteOnly)) {
            delete query_assig;
            this->setCursor(Qt::ArrowCursor);
            db.rollback();
            QMessageBox::warning(this, "Ошибка создания файла",
                                 "При создании целевого файла \n" + file_name_full + "\nпроизошла ошибка.\n\nОперация отменена.");
            ui->pan_progress->setVisible(false);
            return;
        }

        // подготовим поток для сохранения файла
        QTextStream stream(&file);
        QString codec_name = "Windows-1251";
        QTextCodec *codec = QTextCodec::codecForName(codec_name.toLocal8Bit());
        stream.setCodec(codec);

        // добавим шапку
        //QString head = "id;fam;im;ot;sex;date_birth;snils;enp;code_mo;code_mp;code_ms;type_ms;snils_mt;assig_date;unsig_date;is_actual;code_1st_aid";
        //stream << head << "\n";

        // добавим новые данные
        while (query_assig->next()) {
            QString fam         = query_assig->value( 1).toString();
            QString im          = query_assig->value( 2).toString();
            QString ot          = query_assig->value( 3).toString();
            QDate   date_birth  = query_assig->value( 4).toDate();
            int     sex         = query_assig->value( 5).toInt();
            QString address     = query_assig->value( 6).toString();
            int     type_ms     = query_assig->value( 7).toInt();
            int     code_lpu    = query_assig->value( 8).toInt();
            QString code_ms     = query_assig->value( 9).toString();
            QString name_ms     = query_assig->value(10).toString();
            QString pol_type    = query_assig->value(11).toString();
            QString id_polis    = query_assig->value(12).toString();
            stream << "\"" << fam << "\";";
            stream << "\"" << im  << "\";";
            stream << "\"" << ot  << "\";";
            stream << "\"" << date_birth.toString("yyyyMMdd") << "\";";
            stream << "\"" << QString::number(sex) << "\";";
            stream << "\"" << address << "\";";
            stream << "\"" << QString::number(type_ms) << "\";";
            stream << "\"" << QString::number(code_lpu) << "\";";
            stream << "\"" << code_ms << "\";";
            stream << "\"" << name_ms << "\";";
            stream << "\"" << pol_type << "\";";
            stream << "\"" << id_polis << "\"\r\n";
        }
        file.close();
        delete query_assig;

        // ===========================
        // собственно открытие шаблона
        // ===========================
        /*long result = (long)*/ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(file_name_full.utf16()), 0, 0, SW_NORMAL);

    } else {
        // сформируем файлы для каждой МО отдельно и сохраним в папку
        QString gen_dir = data_app.path_out + "_ASSIG_DATA_/" + ui->date_assiglay->date().toString("yyyy-MM-dd") + "/";
        QDir dir(gen_dir);
        bool res_mk = dir.mkpath(gen_dir);

        // выберем коды МО, для которых есть данные
        QString sql_codes = "select mo.code_head, count(*) "
                            "  from persons e "
                            "  left join addresses adr on(adr.id=coalesce(e.id_addr_liv, e.id_addr_reg)) "
                            "  left join polises p on (p.id=e.id_polis) "
                            "  left join (select aa.id_person, max(aa.id) as id from persons_assig aa where aa.assig_date<='" + date_assiglay.toString("yyyy-MM-dd") + "' and (aa.unsig_date is NULL or aa.unsig_date>'" + date_assiglay.toString("yyyy-MM-dd") + "') group by aa.id_person) qa on(qa.id_person=e.id) "
                            "  left join persons_assig a on(a.id=qa.id) "
                            "  left join spr_medorgs mo on(mo.code_mo=a.code_mo) "
                            "  left join spr_medsites ms on(ms.code_mo=a.code_mo and /*ms.code_mp=a.code_mp and*/ ms.code_ms=a.code_ms) "
                            " where st_person(e.id,'" + date_assiglay.toString("yyyy-MM-dd") + "') in(11,12,13,14,1,2,3,4) "
                            " group by mo.code_head "
                            " order by mo.code_head ; ";
        QSqlQuery *query_codes = new QSqlQuery(db);
        bool res_codes = mySQL.exec(this, sql_codes, QString("выберем коды МО по которым есть данные"), *query_codes, true, db, data_app);
        if (!res_codes) {
            delete query_codes;
            this->setCursor(Qt::ArrowCursor);
            db.rollback();
            QMessageBox::warning(this, "Ошибка при проверке кодов МО по которым есть данные", "При проверке кодов МО по которым есть данные произошла неожиданная ошибка");
            ui->pan_progress->setVisible(false);
            return;
        }

        while (query_codes->next()) {
            QString code_head = query_codes->value(0).toString();
            int cnt = query_codes->value(1).toInt();
            if (cnt<1)
                continue;
            if (code_head.isEmpty())
                continue;

            // покажем код МО на экран
            ui->lab_code_mo->setText(code_head);
            QApplication::processEvents();

            // выберем данные прикрепления к этой МО
            QString sql_assig = "select e.id, e.FAM, e.IM, e.OT, e.date_birth, e.sex, "
                                "       coalesce(adr.subj_name,'')||', '||coalesce(adr.dstr_name,'')||', '||coalesce(adr.city_name,'')||', '||coalesce(adr.nasp_name,'')||', '||coalesce(adr.strt_name,'')||', '||coalesce(cast(adr.house as text),'')||' '||coalesce(adr.corp,'')||', '||coalesce(adr.quart,'') as address, "
                                + QString( data_app.ocato=="14000" ? " case ms.type_ms when NULL then -666 when 1 then 2 when 2 then 1 when 3 then 4 when 4 then 4 when 5 then 3 end as type_ms, " : " ms.type_ms, " ) +
                                "       cast(right(a.code_mo,4) as smallint), a.code_ms, ms.name_ms, "
                                "       case p.pol_v when 1 then 'С' when 2 then 'В' when 3 then 'П' else '?' end as pol_type, "
                                "       coalesce(e.enp, p.enp, p.pol_ser||' '||p.pol_num, p.pol_num) as pol_id "
                                "  from persons e "
                                "  left join addresses adr on(adr.id=coalesce(e.id_addr_liv, e.id_addr_reg)) "
                                "  left join polises p on (p.id=e.id_polis) "
                                "  left join (select aa.id_person, max(aa.id) as id from persons_assig aa where aa.assig_date<='" + date_assiglay.toString("yyyy-MM-dd") + "' and (aa.unsig_date is NULL or aa.unsig_date>'" + date_assiglay.toString("yyyy-MM-dd") + "') group by aa.id_person) qa on(qa.id_person=e.id) "
                                "  left join persons_assig a on(a.id=qa.id) "
                                "  left join spr_medorgs mo on(mo.code_mo=a.code_mo) "
                                "  left join spr_medsites ms on(ms.code_mo=a.code_mo and /*ms.code_mp=a.code_mp and*/ ms.code_ms=a.code_ms) "
                                " where mo.code_head='" + code_head + "' "
                                "   and st_person(e.id,'" + date_assiglay.toString("yyyy-MM-dd") + "') in(11,12,13,14,1,2,3,4) "
                                " order by e.fam, e.im, e.ot, e.date_birth ; ";

            QSqlQuery *query_mo = new QSqlQuery(db);
            bool res_mo = mySQL.exec(this, sql_assig, QString("выберем данные прикрепления к этой МО"), *query_mo, true, db, data_app);
            if (!res_mo) {
                delete query_mo;
                this->setCursor(Qt::ArrowCursor);
                db.rollback();
                QMessageBox::warning(this, "Ошибка при выборке данных прикрепления к этой МО", "При выборке данных прикрепления к этой МО произошла неожиданная ошибка");
                ui->pan_progress->setVisible(false);
                return;
            }

            // сохраним данные в файл
            QString file_name = "fzp(" + data_app.smo_regnum + "_" + code_head + "_"+ QDate::currentDate().toString("yyMM") + ").csv";
            QString file_name_full = gen_dir + file_name;
            QFile::remove(file_name_full);
            QFile file;
            file.setFileName(file_name_full);
            if (!file.open(QIODevice::WriteOnly)) {
                delete query_mo;
                this->setCursor(Qt::ArrowCursor);
                db.rollback();
                QMessageBox::warning(this, "Ошибка создания файла",
                                     "При создании целевого файла \n" + file_name_full + "\nпроизошла ошибка.\n\nОперация отменена.");
                ui->pan_progress->setVisible(false);
                return;
            }

            // подготовим поток для сохранения файла
            QTextStream stream(&file);
            QString codec_name = "Windows-1251";
            QTextCodec *codec = QTextCodec::codecForName(codec_name.toLocal8Bit());
            stream.setCodec(codec);

            // добавим шапку
            //QString head = "id;fam;im;ot;sex;date_birth;snils;enp;code_mo;code_mp;code_ms;type_ms;snils_mt;assig_date;unsig_date;is_actual;code_1st_aid";
            //stream << head << "\n";

            // добавим новые данные
            while (query_mo->next()) {
                QString fam         = query_mo->value( 1).toString();
                QString im          = query_mo->value( 2).toString();
                QString ot          = query_mo->value( 3).toString();
                QDate   date_birth  = query_mo->value( 4).toDate();
                int     sex         = query_mo->value( 5).toInt();
                QString address     = query_mo->value( 6).toString();
                int     type_ms     = query_mo->value( 7).toInt();
                int     code_lpu    = query_mo->value( 8).toInt();
                QString code_ms     = query_mo->value( 9).toString();
                QString name_ms     = query_mo->value(10).toString();
                QString pol_type    = query_mo->value(11).toString();
                QString id_polis    = query_mo->value(12).toString();
                stream << "\"" << fam << "\";";
                stream << "\"" << im  << "\";";
                stream << "\"" << ot  << "\";";
                stream << "\"" << date_birth.toString("yyyyMMdd") << "\";";
                stream << "\"" << QString::number(sex) << "\";";
                stream << "\"" << address << "\";";
                stream << "\"" << QString::number(type_ms) << "\";";
                stream << "\"" << QString::number(code_lpu) << "\";";
                stream << "\"" << code_ms << "\";";
                stream << "\"" << name_ms << "\";";
                stream << "\"" << pol_type << "\";";
                stream << "\"" << id_polis << "\"\r\n";
            }
            file.close();
            delete query_mo;
        }

        // ===========================
        // собственно открытие шаблона
        // ===========================
        /*long result = (long)*/ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(gen_dir.utf16()), 0, 0, SW_NORMAL);
    }

    ui->pan_progress->setVisible(false);
}
