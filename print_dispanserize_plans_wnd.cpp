#include "print_dispanserize_plans_wnd.h"
#include "ui_print_dispanserize_plans_wnd.h"

print_dispanserize_plans_wnd::print_dispanserize_plans_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    db(db),
    data_app(data_app),
    settings(settings),
    QDialog(parent),
    ui(new Ui::print_dispanserize_plans_wnd)
{
    ui->setupUi(this);

    ui->combo_age->setCurrentIndex(0);
    ui->spin_year->setValue(QDate::currentDate().year());
}

print_dispanserize_plans_wnd::~print_dispanserize_plans_wnd() {
    delete ui;
}

void print_dispanserize_plans_wnd::on_bn_close_clicked() {
    close();
}

QString print_dispanserize_plans_wnd::date_to_str(QDate date) {
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

void print_dispanserize_plans_wnd::on_bn_print_clicked() {
    ui->bn_print->setEnabled(false);
    this->setCursor(Qt::WaitCursor);

    // проверка ввода данных
    if (ui->combo_age->currentIndex()<1) {
        QMessageBox::warning(this, "Выберите возрастную категорию",
                             "Выберите возрастную категорию застрахованных, "
                             "\nдля которых формируется этот отчёт.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }

    // подсчитаем МО, к которым сделоно прикрепление
    QString sql_mo = "select distinct mo.id, mo.code_mo, mo.name_mo "
                     "  from spr_medorgs mo "
                     "       join persons_assig a on( a.code_mo=mo.code_mo "
                                                    " and a.assig_date<='" + QString::number(ui->spin_year->value()) + "-12-31' "
                                                    " and ( a.unsig_date is NULL "
                                                    "       or a.unsig_date>'" + QString::number(ui->spin_year->value()) + "-12-31') ) ";
    QSqlQuery *query_mo = new QSqlQuery(db);
    bool res_mo = mySQL.exec(this, sql_mo, "Получение списка медорганизаций, по которым проводится диспансеризация за год", *query_mo, true, db, data_app);
    if (!res_mo) {
        QMessageBox::warning(this, "Ошибка при получении списка медорганизаций для диспансеризациии за год", "При получении списка медорганизаций для диспансеризациии за год произошла неожиданная ошибка.");
        delete query_mo;
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    };

    int year = ui->spin_year->value();
    int age = ui->combo_age->currentIndex();

    while (query_mo->next()) {
        QString code_mo = query_mo->value(1).toString();
        QString name_mo = query_mo->value(2).toString();

        // выберем данные для отчёта
        QString sql_rep;
        QString s_years;
        sql_rep = "select e.id, coalesce(e.fam,'')||' '||coalesce(e.im,'')||' '||coalesce(e.ot,''), e.date_birth, "
                  "       coalesce(p.enp, e.enp) as enp, "
                  "       coalesce(e.phone_cell,'')||'  '||coalesce(e.email,''), "
                  "       '('||a.code_mo||') '||mo.name_mo as name_mo, "
                  "       'адрес регистрации: '||coalesce(ar.subj_name,'')||', '||coalesce(ar.dstr_name,'')||', '||coalesce(ar.city_name,'')||', '||coalesce(ar.nasp_name,'')||', '||coalesce(ar.strt_name,'')||', '||ar.house||' '||coalesce(ar.corp,'')||' '||coalesce(', кв.'||ar.quart,'') as addr_reg, "
                  "       'адрес проживания: ' ||al.subj_name||', '||coalesce(al.dstr_name,'')||', '||coalesce(al.city_name,'')||', '||coalesce(al.nasp_name,'')||', '||coalesce(al.strt_name,'')||', '||al.house||' '||coalesce(al.corp,'')||' '||coalesce(', кв.'||al.quart,'') as addr_liv,"
                  "       coalesce(a.code_mo, '0'), coalesce(a.code_mp, '0'), coalesce(a.code_ms, '0') "
                  "  from persons e "
                  "  left join polises p on(p.id=e.id_polis) "
                  "  left join persons_assig a on(a.id_person=e.id and a.is_actual=0) "
                  "  left join spr_medorgs mo on(mo.code_mo=a.code_mo and  mo.is_head=1) "
                  "  left join addresses ar on(ar.id=e.id_addr_reg) "
                  "  left join addresses al on(al.id=e.id_addr_liv) "
                  " where st_person(e.id, '" + QString::number(year) + "-12-31') in(1,2,3,4,11,12,13,14) "
                  "   and extract(year from e.date_birth) in(#YEARS#) "
                  "   and mo.code_mo='#CODE_MO#' "
                  " order by e.fam, e.im, e.ot, e.date_birth ; ";
        if (age==1) {
            for (int i=0; i<18; i++) {
                if (i>0)  s_years += ",";
                s_years += QString::number(year - 17 + 1*i);
            }
        } else if (age==2) {
            // выберем годы
            for (int i=0; i<50; i++) {
                if (i>0)  s_years += ",";
                s_years += QString::number(year - 21 - 3*i);
            }
        } else {
            QMessageBox::warning(this, "Выберите возрастную категорию",
                                 "Выберите возрастную категорию застрахованных, "
                                 "\nдля которых формируюется этот отчёт.");
        }
        sql_rep = sql_rep.replace("#YEARS#", s_years);
        sql_rep = sql_rep.replace("#CODE_MO#", code_mo);

        QSqlQuery *query_rep = new QSqlQuery(db);
        bool res_rep = mySQL.exec(this, sql_rep, "Формирование плана диспансеризации по МО за год", *query_rep, true, db, data_app);
        if (!res_rep) {
            QMessageBox::warning(this, "Не удалось построить план диспансеризации по МО за год", "При попытке построить план диспансеризации по МО за год произошла неожиданная ошибка.");
            delete query_rep;
            this->setCursor(Qt::ArrowCursor);
            ui->bn_print->setEnabled(true);
            return;
        };
        int n_rep = query_rep->size();        
        if (n_rep==0) {
            delete query_rep;
            continue;
        }

        // -------------------------- //
        // собственно генерация плана //
        // -------------------------- //
        this->setCursor(Qt::WaitCursor);

        // распаковка шаблона
        QString rep_folder = data_app.path_reports;
        // распаковка шаблона
        QString tmp_folder = data_app.path_out + "_REPORTS_/temp/";
        QString rep_template = rep_folder + "rep_disp_list4mo.odt";


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
        int pos1 = s_content.indexOf("<table:table-row table:style-name=\"Таблица1.2\">", pos0);
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
                replace("#YEAR#", QString::number(year)).
                replace("#CODE_MO#", code_mo).
                replace("#NAME_MO#", name_mo).
                replace("#AGE#", (age==1 ? "дети (до 18 лет)" : "взрослые (старше 18 лет)")).
                replace("#CNT#", QString::number(n_rep));

        int n = 0;
        while (query_rep->next()) {
            n++;
            QString fio = query_rep->value(1).toString() + ", " + query_rep->value(2).toDate().toString("dd.MM.yyyy");
            QString enp = query_rep->value(3).toString();
            QString phone = query_rep->value(4).toString();
            QString medorg = query_rep->value(5).toString();
            QString addr_reg = query_rep->value(6).toString();
            QString addr_liv = query_rep->value(7).toString();
            QString code_mo = query_rep->value( 8).toString();
            QString code_mp = query_rep->value( 9).toString();
            if (code_mp.trimmed().isEmpty())  code_mp = "0";
            QString code_ms = query_rep->value(10).toString();
            if (code_ms.trimmed().isEmpty())  code_ms = "0";

            s_line = s_line1;
            s_content += s_line.
                    replace("#N#",       QString::number(n)).
                    replace("#FIO#",     fio).
                    replace("#ENP#",     enp).
                    replace("#PHONE#",   phone).
                    replace("#ADDRESS#", addr_reg + ";   " + addr_liv).
                    replace("#CODE_MP#", code_mp).
                    replace("#CODE_MS#", code_ms);
        }
        s_line = s_down;
        s_content += s_line/*.
                replace("#N_TAKE#", QString::number(n_take)).
                replace("#N_ZERO#", QString::number(n_zero))*/;

        // сохранение контента шаблона
        QFile f_content_save(tmp_folder + "/content.xml");
        f_content_save.open(QIODevice::WriteOnly);
        f_content_save.write(s_content.toUtf8());
        f_content_save.close();


        // ===========================
        // архивация шаблона
        // ===========================
        QString fname_res = data_app.path_out + "_REPORTS_/" +
                QDate::currentDate().toString("yyyy-MM-dd") + "  -  " + data_app.smo_short + " - " + data_app.filial_name + "  -  " + code_mo + " - план по диспансеризации на " + QString::number(year) + " год - " + (age==1 ? "дети (до 18 лет)" : "взрослые (старше 18 лет)") + ".odt";

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
            QMessageBox::warning(this, "Ошибка копирования шаблона", "Не удалось скопировать шаблон печатной формы: \n" + rep_template + "\n\nв целевую папку\n" + fname_res + "\n\nОперация прервана.");
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



        //--------------------------------- //
        // сформируем поквартальные таблицы //
        //--------------------------------- //
        QString sql_rep2;
        sql_rep2 = "select coalesce(a.code_mo, '0'), coalesce(a.code_mp, '0'), coalesce(a.code_ms, '0'), count(*) "
                   "  from persons e "
                   "  join persons_assig a on(a.id_person=e.id and a.is_actual=0) "
                   " where st_person(e.id, '" + QString::number(year) + "-12-31') in(1,2,3,4,11,12,13,14) "
                   "   and extract(year from e.date_birth) in(#YEARS#) "
                   "   and a.code_mo='#CODE_MO#' "
                   " group by coalesce(a.code_mo, '0'), coalesce(a.code_mp, '0'), coalesce(a.code_ms, '0') "
                   " order by coalesce(a.code_mo, '0'), coalesce(a.code_mp, '0'), coalesce(a.code_ms, '0') ; ";

        sql_rep2 = sql_rep2.replace("#YEARS#", s_years);
        sql_rep2 = sql_rep2.replace("#CODE_MO#", code_mo);

        QSqlQuery *query_rep2 = new QSqlQuery(db);
        bool res_rep2 = mySQL.exec(this, sql_rep2, "Формирование плана диспансеризации по МО, поквартального", *query_rep2, true, db, data_app);
        if (!res_rep2) {
            QMessageBox::warning(this, "Не удалось построить план диспансеризации по МО, поквартального", "При попытке построить план диспансеризации по МО, поквартального, произошла неожиданная ошибка.");
            delete query_rep2;
            this->setCursor(Qt::ArrowCursor);
            ui->bn_print->setEnabled(true);
            return;
        };
        int n_rep2 = query_rep2->size();
        if (n_rep2==0) {
            delete query_rep2;
            continue;
        }




        // распаковка шаблона
        QString rep_folder2 = data_app.path_reports;
        // распаковка шаблона
        QString tmp_folder2 = data_app.path_out + "_REPORTS_/temp/";
        QString rep_template2 = rep_folder2 + "rep_disp_list4mo.odt";


        if (!QFile::exists(rep_template2)) {
            QMessageBox::warning(this,
                                 "Шаблон не найден",
                                 "Не найден шаблон печатной формы заявления: \n" + rep_template2 +
                                 "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            ui->bn_print->setEnabled(true);
            return;
        }
        QDir tmp_dir2(tmp_folder2);
        if (tmp_dir2.exists(tmp_folder2) && !tmp_dir2.removeRecursively()) {
            QMessageBox::warning(this,
                                 "Ошибка при очистке папки",
                                 "Не удалось очистить папку временных файлов: \n" + tmp_folder2 +
                                 "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            ui->bn_print->setEnabled(true);
            return;
        }
        tmp_dir2.mkpath(tmp_folder2);
        QProcess myProcess2;
        QString com2 = "\"" + data_app.path_arch + "7z.exe\" e \"" + rep_template2 + "\" -o\"" + tmp_folder2 + "\" content.xml";
        if (myProcess2.execute(com2)!=0) {
            QMessageBox::warning(this,
                                 "Ошибка распаковки шаблона",
                                 "Не удалось распаковать файл контента шаблона\n" + rep_template2 + " -> content.xml"
                                 "\nпечатной формы во временную папку\n" + tmp_folder2 +
                                 "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            ui->bn_print->setEnabled(true);
            return;
        }

        // ===========================
        // правка контента2 шаблона
        // ===========================
        // открытие контента2 шаблона
        QFile f_content2(tmp_folder2 + "/content.xml");
        f_content2.open(QIODevice::ReadOnly);
        QString s_content2 = f_content2.readAll();
        f_content2.close();
        if (s_content2.isEmpty()) {
            QMessageBox::warning(this,
                                 "Файл контента пустой",
                                 "Файл контента пустой. Возможно шаблон был испорчен.\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            ui->bn_print->setEnabled(true);
            return;
        }

        // -----------------------------
        // правка полей контента2 шаблона
        // -----------------------------
        s_content2 = s_content2.
                     replace("#YEAR#", QString::number(year)).
                     replace("#AGE#", (age==1 ? "дети (до 18 лет)" : "взрослые (старше 18 лет)"));

        // ---------------------------- //
        // разберём контент2 по строкам //
        // ---------------------------- //
        int pos00 = 0;
        int pos11 = s_content2.indexOf("<table:table-row table:style-name=\"Таблица2.2\">", pos00);
        int pos12 = s_content2.indexOf("</table:table-row>", pos11) + 18;
        int pos21 = s_content2.indexOf("<table:table-row table:style-name=\"Таблица4.2\">", pos12);
        int pos22 = s_content2.indexOf("</table:table-row>", pos21) + 18;
        int pos31 = s_content2.indexOf("<table:table-row table:style-name=\"Таблица6.2\">", pos22);
        int pos32 = s_content2.indexOf("</table:table-row>", pos31) + 18;
        int pos41 = s_content2.indexOf("<table:table-row table:style-name=\"Таблица8.2\">", pos32);
        int pos42 = s_content2.indexOf("</table:table-row>", pos41) + 18;
        int pos50 = s_content2.length();
        QString s2_top   = s_content2.mid(pos00, pos11-pos00);
        QString s2_line1 = s_content2.mid(pos11, pos12-pos11);
        QString s2_mid1  = s_content2.mid(pos12, pos21-pos12);
        QString s2_line2 = s_content2.mid(pos21, pos22-pos21);
        QString s2_mid2  = s_content2.mid(pos22, pos31-pos22);
        QString s2_line3 = s_content2.mid(pos31, pos32-pos31);
        QString s2_mid3  = s_content2.mid(pos32, pos41-pos32);
        QString s2_line4 = s_content2.mid(pos41, pos42-pos41);
        QString s2_down  = s_content2.mid(pos42, pos50-pos42);
        QString s2_line;


        // --------------------------------------------- //
        // рассчитаем план по диспансеризации на квартал //
        // --------------------------------------------- //
        int n_all = n;
        int n_qrt = n_all/4;
        if (n_qrt>=1000) {
            n_qrt = (n_qrt/100 +1) * 100;
        } else if (n_qrt>=100) {
            n_qrt = (n_qrt/10 +1) * 10;
        }

        int quart = 1;
        int quart_cnt = 0;
        s_content2 = s2_top;
        int cnt = 0;

        while (query_rep2->next()) {
            // получим данные нового участка
            QString code_mo = query_rep2->value(0).toString();
            QString code_mp = query_rep2->value(1).toString();
            if (code_mp.trimmed().isEmpty())  code_mp = "0";
            QString code_ms = query_rep2->value(2).toString();
            if (code_ms.trimmed().isEmpty())  code_ms = "0";
            cnt = query_rep2->value(3).toInt();
m_quart:
            if (cnt<1)  continue;

            // выберем строку шаблоны
            switch (quart) {
            case 1 : s2_line = s2_line1; break;
            case 2 : s2_line = s2_line2; break;
            case 3 : s2_line = s2_line3; break;
            default: s2_line = s2_line4; break;
            }

            if (quart<4) {
                // численность по плану доводится до n_qrt
                if (quart_cnt+cnt<=n_qrt) {
                    // добавим строку
                    s2_line = s2_line
                            .replace("#CODE_MP#",code_mp)
                            .replace("#CODE_MS#",code_ms)
                            .replace("#N_PERS#", QString::number(cnt));
                    s_content2 += s2_line;
                    // вычтем потраченное
                    quart_cnt += cnt;
                    cnt = 0;
                } else {
                    // добавим строку
                    s2_line = s2_line
                            .replace("#CODE_MP#",code_mp)
                            .replace("#CODE_MS#",code_ms)
                            .replace("#N_PERS#", QString::number(n_qrt-quart_cnt));
                    s_content2 += s2_line;
                    // вычтем потраченное
                    cnt -= n_qrt-quart_cnt;
                    quart_cnt = n_qrt;
                }
            } else {
                // к последнему кварталу строчу добавляю без проверки
                s2_line = s2_line
                        .replace("#CODE_MP#",code_mp)
                        .replace("#CODE_MS#",code_ms)
                        .replace("#N_PERS#", QString::number(cnt));
                s_content2 += s2_line;
                // вычтем потраченное
                quart_cnt += cnt;
                cnt = 0;
            }

            if ( quart_cnt==n_qrt && quart<4 ) {
                // этот квартал завершён
                //s_content2 += s2_line;
                switch (quart) {
                case 1 : s_content2 += s2_mid1; break;
                case 2 : s_content2 += s2_mid2; break;
                case 3 : s_content2 += s2_mid3; break;
                }
                quart++;
                quart_cnt = 0;
                goto m_quart;
            }
        }
        // закроем последний квартал
        //s_content2 += s2_line;
        s_content2 += s2_down;
        //quart++;
        //quart_cnt = 0;


        // сохранение контента шаблона
        QFile f_content2_save(tmp_folder2 + "/content.xml");
        f_content2_save.open(QIODevice::WriteOnly);
        f_content2_save.write(s_content2.toUtf8());
        f_content2_save.close();


        // ===========================
        // архивация шаблона
        // ===========================
        QString fname2_res = data_app.path_out + "_ДИСПАНСЕРИЗАЦИЯ_/" +
                QDate::currentDate().toString("yyyy-MM-dd") + "  -  " + data_app.smo_short + " - " + data_app.filial_name + "  -  " +
                code_mo + " - план по диспансеризации на " + QString::number(year) + " год - " + (age==1 ? "дети (до 18 лет)" : "взрослые (старше 18 лет)") + " - работа с населением.odt";

        QFile::remove(fname2_res);
        while (QFile::exists(fname2_res)) {
            if (QMessageBox::warning(this,
                                     "Ошибка сохранения печатной формы",
                                     "Не удалось сохранить шаблон печатной формы: \n" + rep_template2 +
                                     "\n\nво временную папку\n" + fname2_res +
                                     "\n\nПроверьте, не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                     QMessageBox::Retry|QMessageBox::Abort,
                                     QMessageBox::Retry)==QMessageBox::Abort) {
                this->setCursor(Qt::ArrowCursor);
                ui->bn_print->setEnabled(true);
                return;
            }
            QFile::remove(fname2_res);
        }
        if (!QFile::copy(rep_template2, fname2_res) || !QFile::exists(fname2_res)) {
            QMessageBox::warning(this, "Ошибка копирования шаблона", "Не удалось скопировать шаблон печатной формы: \n" + rep_template2 + "\n\nв целевую папку\n" + fname2_res + "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            ui->bn_print->setEnabled(true);
            return;
        }
        com2 = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname2_res + "\" \"" + tmp_folder2 + "/content.xml\"";
        if (myProcess.execute(com2)!=0) {
            QMessageBox::warning(this,
                                 "Ошибка обновления контента",
                                 "При добавлении нового контента в шаблон печатной формы заявления произошла непредвиденная ошибка\n\nОпреация прервана.");
        }





    }


    // ========================= //
    // собственно открытие папки //
    // ========================= //
    // открытие папки отчётов
    QString folder_res = data_app.path_out + "_REPORTS_";
    long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(folder_res.utf16()), 0, 0, SW_NORMAL);
    this->setCursor(Qt::ArrowCursor);
    if (result<32) {
        QMessageBox::warning(this,
                             "Ошибка при открытии папки",
                             "При попытке открыть папку сохранения отчётов произошла ошибка:\n\n" + folder_res +
                             "\nКод ошибки: " + QString::number(result) +
                             "\n\nПопробуйте открыть эту папку вручную.");
    }
    this->setCursor(Qt::ArrowCursor);
    ui->bn_print->setEnabled(true);
}
