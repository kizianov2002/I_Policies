#include "protocols_wnd.h"
#include "ui_protocols_wnd.h"
#include <QMessageBox>
#include <QDesktopWidget>
#include <QFileDialog>

protocols_wnd::protocols_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, persons_wnd *persons_w, QWidget *parent) :
    db(db),
    data_app(data_app),
    settings(settings),
    persons_w(persons_w),
    QDialog(parent),
    ui(new Ui::protocols_wnd)
{
    ui->setupUi(this);

    id_person = -1;
    id_polis = -1;
    id_event = -1;
    show_tab_w = NULL;

    on_bn_dnsize_clicked();

    ui->split_protocols_log->setStretchFactor(0, 8);
    ui->split_protocols_log->setStretchFactor(1, 1);
    ui->split_protocols->setStretchFactor(0, 2);
    ui->split_protocols->setStretchFactor(1, 5);

    QRect screen = QApplication::desktop()->screenGeometry();
    this->resize(1250, 1000);
    this->move(screen.width()/2-625, screen.height()/2-500 - 20);

    on_bn_today_clicked();

    refresh_packs();
    refresh_flk_tab();
    refresh_protocols_tab();
}

protocols_wnd::~protocols_wnd() {
    delete ui;
}

void protocols_wnd::on_bn_close_clicked() {
    //if (QMessageBox::question(this, "Закрыть окно протоколов?", "Закрыть окно протоколов?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes) {
        close();
    //}
}

void protocols_wnd::on_bn_hide_clicked() {
    QRect screen = QApplication::desktop()->screenGeometry();

    if (ui->bn_hide->text().trimmed()=="Свернуть окно") {
        //ui->lab_comment->setMaximumWidth(100);
        ui->pan_up->setVisible(false);
        ui->pan_flk->setVisible(false);
        ui->pan_protokols_up->setVisible(false);
        ui->pan_log->setVisible(false);

        this->resize(250,600);
        this->move(screen.width()-this->size().width(), (screen.height()-this->size().height())/2 - 100);
        this->resize(250,600);
        this->move(screen.width()-this->size().width(), (screen.height()-this->size().height())/2 - 100);

        ui->bn_hide->setText(" Развернуть окно ");

        //this->hide();
    } else {
        //ui->lab_comment->setMaximumWidth(500);
        ui->pan_up->setVisible(true);
        refresh_flk_tab();
        ui->pan_protokols_up->setVisible(true);
        ui->pan_log->setVisible(true);

        this->resize(1250, 1000);
        this->move(screen.width()/2-625, screen.height()/2-500 - 20);
        this->resize(1250, 1000);
        this->move(screen.width()/2-625, screen.height()/2-500 - 20);

        ui->bn_hide->setText(" Свернуть окно ");
    }
}

void protocols_wnd::refresh_packs() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка посылок
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select q.id, q.full_name, q.file_name, q.file_1, pt, nn, mm, yy "
                  "  from ( select id, '('||right('     '||id,5)||')  '||file_name as full_name, "
                  "                case when right(left(trim(pack_name),16),2)='_1' "
                  "                 then right(left(trim(pack_name),16),2) "
                  "                 else left(trim(pack_name),16) "
                  "                end as file_name, "
                  "                case "
                  "                 when character_length(trim(pack_name))>17 "
                  "                 then right(trim(pack_name), character_length(trim(pack_name))-16 ) "
                  "                else NULL end as file_1, "
                  "                trim(pack_name), "
                  "                left(trim(pack_name),16), "
                  "                right(left(trim(pack_name),16),10), "
                  "                left(right(left(trim(pack_name),16),10),3) as pt, "
                  "                left(right(left(trim(pack_name),16), 2),2) as nn, "
                  "                left(right(left(pack_name,16),6),2) as mm, "
                  "                left(right(left(trim(pack_name),16),4),2) as yy "
                  "             from public.files_in_p p "
                  "           ) q "
                  " order by yy desc, mm desc, nn desc, pt asc; ; ";
    mySQL.exec(this, sql, QString("список входных файлов протоколов"), *query, true, db, data_app);
    ui->combo_file->clear();
    while (query->next()) {
        int n = query->value(0).toInt();
        ui->combo_file->addItem(query->value(1).toString(), n);
    }
    ui->combo_file->setCurrentIndex(0);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void protocols_wnd::on_bn_refresh_clicked() {
    refresh_protocols_tab();
}

/*
void protocols_wnd::on_ch_only_polis_clicked() {
    if (ui->ch_only_polis->isChecked()) {
        // ничего не делаем
    } else {
        ui->ch_only_wrong->setChecked(false);
        ui->ch_only_wrong_active->setChecked(false);
    }
    refresh_protocols_tab();
}
void protocols_wnd::on_ch_only_wrong_clicked() {
    if (ui->ch_only_wrong->isChecked()) {
        ui->ch_only_polis->setChecked(true);
    } else {
        ui->ch_only_wrong_active->setChecked(false);
    }
    refresh_protocols_tab();
}
void protocols_wnd::on_ch_only_wrong_active_clicked() {
    if (ui->ch_only_wrong_active->isChecked()) {
        ui->ch_only_wrong->setChecked(true);
        ui->ch_only_polis->setChecked(true);
    } else {
        // ничего не делаем
    }
    refresh_protocols_tab();
}
*/

void protocols_wnd::refresh_protocols_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, "Нет доступа к базе данных",
                             "Нет открытого соединения к базе данных?\n "
                             "Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n "
                             "Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику.");
        return;
    }

    QString protocols_sql;
    protocols_sql += "select r.id, r.id_file_p, r.n_rec, r.ogrn_smo, r.id_tfoms, r.ocato, r.enp, r.pol_v, r.pol_ser, r.pol_num, r.date_begin, r.date_end, r.date_stop, "
                     "       r.in_erp, r.comment, r.status, p.id_person, r.id_polis, a.id as id_event, a.event_code, a.status as event_status,"
                     "       f.id as id_file_p, fp.id as id_pack, fp.id_file_i, fp.date_file_i, r.date_add, r.date_resend, r.date_ok, r.date_hide, r.id_operator, rop.oper_fio "
                     "  from frecs_in_p r "
                     "       left join polises p on(p.id=r.id_polis) "
                     "       left join events a on(a.id=p._id_last_event) "
                     "       left join files_in_p f on(f.id=r.id_file_p) "
                     "       left join files_packs fp on(fp.id_file_p=f.id) "
                     "       left join operators rop on(rop.id=r.id_operator) "
                     " where ";

    protocols_sql += " r.status in(999";
    if (ui->ch_req_polis_no->isChecked())
        protocols_sql += ", 0";
    if (ui->ch_req_polis_ok->isChecked())
        protocols_sql += ", 1";
    if (ui->ch_req_ready->isChecked())
        protocols_sql += ", 2";
    if (ui->ch_req_resended->isChecked())
        protocols_sql += ",-1";
    if (ui->ch_req_complete->isChecked())
        protocols_sql += ", 3";
    if (ui->ch_req_close_byhand->isChecked())
        protocols_sql += ",-2";
    protocols_sql += ") ";

    protocols_sql += " and r.enp like('%" + ui->line_filter_enp->text() + "%') ";
    protocols_sql += " and r.pol_ser like('%" + ui->line_filter_ser->text() + "%') ";
    protocols_sql += " and cast(r.pol_num as text) like('%" + ui->line_filter_num->text() + "%') ";
    protocols_sql += " and r.comment like('%" + ui->line_filter_comment->text() + "%') ";

    if (ui->rb_proto_day->isChecked())
        protocols_sql += " and f.date_get='" + ui->date_file->date().toString("yyyy-MM-dd") + "' ";
    if (ui->rb_proto_file->isChecked())
        protocols_sql += " and r.id_file_p=" + QString::number(ui->combo_file->currentData().toInt()) + " ";

    if (ui->ch_req_errors->isChecked())
        protocols_sql += " and ( (r.comment<>'полис нового образца включен в заявку на изготовление') and (r.comment<>'ОК!') ) ";

    protocols_sql += " order by r.id_file_p, r.n_rec ; ";

    model_protocols.setQuery(protocols_sql,db);
    QString err2 = model_protocols.lastError().driverText();

    // подключаем модель из БД
    ui->tab_protocols->setModel(&model_protocols);

    // обновляем таблицу
    ui->tab_protocols->reset();

    // задаём ширину колонок
    ui->tab_protocols->setColumnWidth( 0,  3);     // r.id
    ui->tab_protocols->setColumnWidth( 1,  1);     // r.id_file_p
    ui->tab_protocols->setColumnWidth( 2, 50);     // r.n_rec
    ui->tab_protocols->setColumnWidth( 3,  1);     // r.ogrn_smo
    ui->tab_protocols->setColumnWidth( 4,  1);     // r.id_tfoms
    ui->tab_protocols->setColumnWidth( 5,  1);     // r.ocato
    ui->tab_protocols->setColumnWidth( 6,110);     // r.enp
    ui->tab_protocols->setColumnWidth( 7, 50);     // r.pol_v
    ui->tab_protocols->setColumnWidth( 8, 50);     // r.pol_ser
    ui->tab_protocols->setColumnWidth( 9, 80);     // r.pol_num
    ui->tab_protocols->setColumnWidth(10, 70);     // r.date_begin
    ui->tab_protocols->setColumnWidth(11, 70);     // r.date_end
    ui->tab_protocols->setColumnWidth(12, 70);     // r.date_stop
    ui->tab_protocols->setColumnWidth(13, 50);     // r.in_erp
    ui->tab_protocols->setColumnWidth(14,350);     // r.comment
    ui->tab_protocols->setColumnWidth(15, 50);     // r.status
    ui->tab_protocols->setColumnWidth(16,  1);     // p.id_person
    ui->tab_protocols->setColumnWidth(17,  1);     // r.id_polis
    ui->tab_protocols->setColumnWidth(18,  1);     // a.id as id_event
    ui->tab_protocols->setColumnWidth(19, 50);     // a.event_code
    ui->tab_protocols->setColumnWidth(20, 50);     // a.status as event_status
    ui->tab_protocols->setColumnWidth(21, 50);     // f.id as id_file_p
    ui->tab_protocols->setColumnWidth(22, 50);     // fp.id as id_pack
    ui->tab_protocols->setColumnWidth(23, 50);     // fp.id_file_i
    ui->tab_protocols->setColumnWidth(24, 50);     // fp.date_file_i

    ui->tab_protocols->setColumnWidth(25, 70);     // r.date_add
    ui->tab_protocols->setColumnWidth(26, 70);     // r.date_resend
    ui->tab_protocols->setColumnWidth(27, 70);     // r.date_ok
    ui->tab_protocols->setColumnWidth(28, 70);     // r.date_hide
    ui->tab_protocols->setColumnWidth(29,  1);     // r.id_operator
    ui->tab_protocols->setColumnWidth(30,150);     // rop.oper_fio

    // правим заголовки
    model_protocols.setHeaderData( 0, Qt::Horizontal, ("ID"));
    model_protocols.setHeaderData( 3, Qt::Horizontal, ("N_REC"));
    model_protocols.setHeaderData( 3, Qt::Horizontal, ("ОГРН СМО"));
    model_protocols.setHeaderData( 5, Qt::Horizontal, ("ОКАТО"));
    model_protocols.setHeaderData( 6, Qt::Horizontal, ("ЕНП"));
    model_protocols.setHeaderData( 7, Qt::Horizontal, ("тип \nпоиса"));
    model_protocols.setHeaderData( 8, Qt::Horizontal, ("серия \nполиса"));
    model_protocols.setHeaderData( 9, Qt::Horizontal, ("номер \nполиса"));
    model_protocols.setHeaderData(10, Qt::Horizontal, ("дата \nвыдачи"));
    model_protocols.setHeaderData(11, Qt::Horizontal, ("полановое\nокончан."));
    model_protocols.setHeaderData(12, Qt::Horizontal, ("дата \nизъятия"));
    model_protocols.setHeaderData(13, Qt::Horizontal, ("в ЕРП?"));
    model_protocols.setHeaderData(14, Qt::Horizontal, ("комментарий"));
    model_protocols.setHeaderData(15, Qt::Horizontal, ("статус \nполиса"));
    model_protocols.setHeaderData(19, Qt::Horizontal, ("код \nсобыт."));
    model_protocols.setHeaderData(20, Qt::Horizontal, ("статус \nсобыт."));
    model_protocols.setHeaderData(21, Qt::Horizontal, ("ID \nP-файла"));
    model_protocols.setHeaderData(22, Qt::Horizontal, ("ID \nпосылки"));
    model_protocols.setHeaderData(23, Qt::Horizontal, ("ID \nI-файла"));
    model_protocols.setHeaderData(24, Qt::Horizontal, ("дата \nI-файла"));

    model_protocols.setHeaderData(25, Qt::Horizontal, ("дата \nпротокола"));    // r.date_add
    model_protocols.setHeaderData(26, Qt::Horizontal, ("дата \nпереотпр."));    // r.date_resend
    model_protocols.setHeaderData(27, Qt::Horizontal, ("дата \nОК"));           // r.date_ok
    model_protocols.setHeaderData(28, Qt::Horizontal, ("дата \nсокрытия"));     // r.date_hide
    model_protocols.setHeaderData(30, Qt::Horizontal, ("оператор"));            // rop.oper_fio

    ui->tab_protocols->repaint();

    ui->lab_comment->setText("-//-");

    this->setCursor(Qt::ArrowCursor);
}

void protocols_wnd::on_bn_filter_clear_clicked() {
    ui->line_filter_enp->clear();
    ui->line_filter_ser->clear();
    ui->line_filter_num->clear();
    ui->line_filter_comment->clear();
    refresh_protocols_tab();
}
void protocols_wnd::on_line_filter_enp_editingFinished() {
    refresh_protocols_tab();
}
void protocols_wnd::on_line_filter_ser_editingFinished() {
    refresh_protocols_tab();
}
void protocols_wnd::on_line_filter_num_editingFinished() {
    refresh_protocols_tab();
}
void protocols_wnd::on_line_filter_comment_editingFinished() {
    refresh_protocols_tab();
}


void protocols_wnd::on_bn_load_new_protokols_clicked() {
    on_bn_upsize_clicked();

    QStringList files_str_list = QFileDialog::getOpenFileNames(this, "Выберите файлы из фонда для загрузки",
                                                               data_app.path_in, "файлы XML или ZIP-архивы (*.xml *.zip *.7z *.csv)",
                                                               0, QFileDialog::ReadOnly);
    ui->te_log->append("\nЗапуск обработки входных файлов ...");
    db.transaction();

    f_yes2all = false;

    // переберём список файлов
    for (int i=0; i<files_str_list.size(); i++) {
        QString f_str = files_str_list.at(i);
        QFile f(f_str);
        QFileInfo fi(f_str);
        QString fname = fi.fileName().toUpper();
        QString ext = fi.suffix().toUpper();

        if (ext=="ZIP" || ext=="7Z") {
            // проверим возможность распаковки архива и поищем в нём файл протокола для нужного ПВП
            QString arch_fname = fname;
            QString arch_ext = ext;

            // подготовим папку для распаковки архива
            QDir dir(data_app.path_temp);
            QDir dir_tmp(data_app.path_temp + arch_fname);
            bool is_ok = true;
            if (dir_tmp.exists()) {
                is_ok = false;
                while (!(is_ok=dir_tmp.removeRecursively())) {
                    if (QMessageBox::warning(this, "Ошибка удаления папки",
                                             "Не удалось очистить и удалить временную папку\n\n" + (data_app.path_temp + arch_fname) + "\n\nПовторить?\n",
                                             QMessageBox::Retry|QMessageBox::Cancel)==QMessageBox::Cancel) {
                        // отказ от обработки архива
                        break;
                    }
                }
                if (!is_ok) {
                    // пропустим этот файл
                    continue;
                }
            }
            while (!(is_ok=dir.mkdir(arch_fname))) {
                if (QMessageBox::warning(this, "Ошибка создания папки",
                                         "Не пересоздать временную папку\n\n" + (data_app.path_temp + arch_fname) + "\n\nПовторить?\n",
                                         QMessageBox::Retry|QMessageBox::Cancel)==QMessageBox::Cancel) {
                    // отказ от обработки архива
                    break;
                }
                if (!is_ok) {
                    // пропустим этот файл
                    continue;
                }
            }

            // достанем протокол из архива
            QProcess myProcess;
            QString com = "\"" + data_app.path_arch + "7z.exe\" e \"" + f_str + "\" -o\"" + (data_app.path_temp + arch_fname) + "\" * -r";
            while (myProcess.execute(com)!=0) {
                if (QMessageBox::warning(this, "Ошибка распаковки архива протокола",
                                         "Не удалось распаковать файл архива протокола\n" + f_str +
                                         "\nво временную папку\n" + (data_app.path_temp + arch_fname) +
                                         "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки.\n\n"
                                         "Повторить?\n",
                                         QMessageBox::Retry|QMessageBox::Cancel)==QMessageBox::Cancel) {
                    // отказ от обработки архива
                    break;
                }
                if (!is_ok) {
                    // пропустим этот файл
                    continue;
                }
            }

            // выберем из временной папки файл протокола без "_1"
            bool has_file = false;
            QStringList files2 = dir_tmp.entryList(QStringList("*.xml"));
            for (int i=0; i<files2.size(); i++) {
                QString f_str2 = files2.at(i);
                QFileInfo fi2(f_str2);
                QString fname2 = fi2.fileName().toUpper();
                QString s1 = fname2.left(1);
                QString s2 = fname2.mid(1,5);
                QString s3 = fname2.mid(6,1);
                int pos = fname2.indexOf("_",7);
                QString s4 = fname2.mid(7,pos-7);
                QString s5 = fname2.right(6);
                if ( // s1=="P"
                     // && s4==data_app.point_regnum
                     // && s2==data_app.smo_regnum
                     s3=="_"
                     // && s5!="_1.XML"
                       ) {
                    // обработаем файл
                    f_str = (data_app.path_temp + arch_fname) + "/" + fname2;
                    f.setFileName(f_str);
                    fi.setFile(f_str);
                    fname = fi.fileName().toUpper();
                    ext = fi.suffix().toUpper();

                    has_file = true;

                    if (ext=="XML") {
                        // проверим тип входного файла
                        QString s1 = fname.left(1);
                        QString s2 = fname.mid(1,5);
                        QString s3 = fname.mid(6,1);
                        int pos = fname.indexOf("_",7);
                        QString s4 = fname.mid(7,pos-7);
                        QString s5 = fname.right(6);
                        if ( //s2==data_app.smo_regnum &&
                             s3=="_"
                             // && s4==data_app.point_regnum
                             // && s5!="_1.XML"
                             // && s5!="_0.XML"
                               ) {

                            // попробуем обработать файл в зависимости от его типа
                            if (s1=="P") {
                                ui->te_log->append("\n=> файл  " + f_str + "  -  файл протокола ...");
                                if (!process_P_file(f_str)) {
                                    db.rollback();
                                    ui->te_log->append("\n!!! ПРИ ОБРАБОТКЕ ФАЙЛА  " + f_str + "  ПРОИЗОШЛИ ОШИБКИ !!! ");
                                    ui->te_log->append("!!! ФАЙЛ ПРОПУЩЕН, НИЧЕГО НЕ ИЗМЕНИЛОСЬ !!! ");
                                    continue;
                                }
                            } else if (s1=="S") {
                                ui->te_log->append("\n=> файл  " + f_str + "  -  стоп-файл ...");
                                if (!process_S_file(f_str)) {
                                    db.rollback();
                                    ui->te_log->append("\n!!! ПРИ ОБРАБОТКЕ ФАЙЛА  " + f_str + "  ПРОИЗОШЛИ ОШИБКИ !!! ");
                                    ui->te_log->append("!!! ФАЙЛ ПРОПУЩЕН, НИЧЕГО НЕ ИЗМЕНИЛОСЬ !!! ");
                                    continue;
                                }
                            } else if (s1=="K") {
                                ui->te_log->append("\n=> файл  " + f_str + "  -  файл корректировки ...");
                                if (!process_K_file(f_str)) {
                                    db.rollback();
                                    ui->te_log->append("\n!!! ПРИ ОБРАБОТКЕ ФАЙЛА  " + f_str + "  ПРОИЗОШЛИ ОШИБКИ !!! ");
                                    ui->te_log->append("!!! ФАЙЛ ПРОПУЩЕН, НИЧЕГО НЕ ИЗМЕНИЛОСЬ !!! ");
                                    continue;
                                }
                            } else if (s1=="F") {
                                ui->te_log->append("\n=> файл  " + f_str + "  -  протокол ошибок ФЛК ...");
                                if (!process_F_file(f_str)) {
                                    db.rollback();
                                    ui->te_log->append("\n!!! ПРИ ОБРАБОТКЕ ФАЙЛА  " + f_str + "  ПРОИЗОШЛИ ОШИБКИ !!! ");
                                    ui->te_log->append("!!! ФАЙЛ ПРОПУЩЕН, НИЧЕГО НЕ ИЗМЕНИЛОСЬ !!! ");
                                    continue;
                                }
                            } else if (s1=="U") {
                                ui->te_log->append("\n=> файл  " + f_str + "  -  файл полисов в составе УЭК ...");
                                if (!process_U_file(f_str)) {
                                    db.rollback();
                                    ui->te_log->append("\n!!! ПРИ ОБРАБОТКЕ ФАЙЛА  " + f_str + "  ПРОИЗОШЛИ ОШИБКИ !!! ");
                                    ui->te_log->append("!!! ФАЙЛ ПРОПУЩЕН, НИЧЕГО НЕ ИЗМЕНИЛОСЬ !!! ");
                                    continue;
                                }
                            } else {
                                ui->te_log->append("\n!!! ТИП ФАЙЛА  " + f_str + "  НЕ ОПОЗНАН !!! ");
                                ui->te_log->append("!!! ФАЙЛ ПРОПУЩЕН, НИЧЕГО НЕ ИЗМЕНИЛОСЬ !!! ");
                            }
                        } else {
                            ui->te_log->append("\n!!! НАЗВАНИЕ ФАЙЛА  " + f_str + "  НЕ СООТВЕТСТВУЕТ ДАННЫМ СМО !!! ");
                            ui->te_log->append("!!! ФАЙЛ ПРОПУЩЕН, НИЧЕГО НЕ ИЗМЕНИЛОСЬ !!! ");
                        }
                        QApplication::processEvents();
                    }
                }
            }
        } else if (ext=="XML") {
            // проверим тип входного файла
            QString s1 = fname.left(1);
            QString s2 = fname.mid(1,5);
            QString s3 = fname.mid(6,1);
            int pos = fname.indexOf("_",7);
            QString s4 = fname.mid(7,pos-7);
            QString s5 = fname.right(6);
            if ( //s2==data_app.smo_regnum &&
                 s3=="_"
                 /* && s4==data_app.point_regnum */
                 /* && s5!="_1.XML" && s5!="_0.XML" */) {

                // попробуем обработать файл в зависимости от его типа
                if (s1=="P") {
                    ui->te_log->append("\n=> файл  " + f_str + "  -  файл протокола ...");
                    if (!process_P_file(f_str)) {
                        db.rollback();
                        ui->te_log->append("\n!!! ПРИ ОБРАБОТКЕ ФАЙЛА  " + f_str + "  ПРОИЗОШЛИ ОШИБКИ !!! ");
                        ui->te_log->append("!!! ФАЙЛ ПРОПУЩЕН, НИЧЕГО НЕ ИЗМЕНИЛОСЬ !!! ");
                        continue;
                    }
                } else if (s1=="S") {
                    ui->te_log->append("\n=> файл  " + f_str + "  -  стоп-файл ...");
                    if (!process_S_file(f_str)) {
                        db.rollback();
                        ui->te_log->append("\n!!! ПРИ ОБРАБОТКЕ ФАЙЛА  " + f_str + "  ПРОИЗОШЛИ ОШИБКИ !!! ");
                        ui->te_log->append("!!! ФАЙЛ ПРОПУЩЕН, НИЧЕГО НЕ ИЗМЕНИЛОСЬ !!! ");
                        continue;
                    }
                } else if (s1=="K") {
                    ui->te_log->append("\n=> файл  " + f_str + "  -  файл корректировки ...");
                    if (!process_K_file(f_str)) {
                        db.rollback();
                        ui->te_log->append("\n!!! ПРИ ОБРАБОТКЕ ФАЙЛА  " + f_str + "  ПРОИЗОШЛИ ОШИБКИ !!! ");
                        ui->te_log->append("!!! ФАЙЛ ПРОПУЩЕН, НИЧЕГО НЕ ИЗМЕНИЛОСЬ !!! ");
                        continue;
                    }
                } else if (s1=="F") {
                    ui->te_log->append("\n=> файл  " + f_str + "  -  протокол ошибок ФЛК ...");
                    if (!process_F_file(f_str)) {
                        db.rollback();
                        ui->te_log->append("\n!!! ПРИ ОБРАБОТКЕ ФАЙЛА  " + f_str + "  ПРОИЗОШЛИ ОШИБКИ !!! ");
                        ui->te_log->append("!!! ФАЙЛ ПРОПУЩЕН, НИЧЕГО НЕ ИЗМЕНИЛОСЬ !!! ");
                        continue;
                    }
                } else if (s1=="U") {
                    ui->te_log->append("\n=> файл  " + f_str + "  -  файл полисов в составе УЭК ...");
                    if (!process_U_file(f_str)) {
                        db.rollback();
                        ui->te_log->append("\n!!! ПРИ ОБРАБОТКЕ ФАЙЛА  " + f_str + "  ПРОИЗОШЛИ ОШИБКИ !!! ");
                        ui->te_log->append("!!! ФАЙЛ ПРОПУЩЕН, НИЧЕГО НЕ ИЗМЕНИЛОСЬ !!! ");
                        continue;
                    }
                } else {
                    ui->te_log->append("\n!!! ТИП ФАЙЛА  " + f_str + "  НЕ ОПОЗНАН !!! ");
                    ui->te_log->append("!!! ФАЙЛ ПРОПУЩЕН, НИЧЕГО НЕ ИЗМЕНИЛОСЬ !!! ");
                }
            } else {
                ui->te_log->append("\n!!! НАЗВАНИЕ ФАЙЛА  " + f_str + "  НЕ СООТВЕТСТВУЕТ ДАННЫМ СМО !!! ");
                ui->te_log->append("!!! ФАЙЛ ПРОПУЩЕН, НИЧЕГО НЕ ИЗМЕНИЛОСЬ !!! ");
            }
        } else if (ext=="CSV") {
            // проверим тип входного файла
            QString s1 = fname.left(2);
            QString s2 = fname.mid(2,14);
            int dot_pos = fname.indexOf(".", 2);
            if ( (s1=="EO" || s1=="LO")
                 && dot_pos>=14
                 && s2.indexOf("(?![0-9])",2)==-1 ) {

                // попробуем обработать файл в зависимости от его типа
                if ( (s1=="EO" || s1=="LO") ) {
                    ui->te_log->append("\n=> файл  " + f_str + "  -  протокол по прикреплениям ...");
                    if (!process_EO_file(f_str)) {
                        db.rollback();
                        continue;
                    }
                } else {
                    ui->te_log->append("\n!!! тип файла  " + f_str + "  не опознан !!! ");
                }
            } else {
                ui->te_log->append("\n!!! название файла  " + f_str + "  не соответствует данным СМО !!! ");
            }
        }
    }

    ui->te_log->append("\n ОБРАБОТКА ЗАВЕРШЕНА \n-");

    db.commit();

    refresh_flk_tab();
    refresh_packs();;
    refresh_protocols_tab();
}


bool protocols_wnd::process_P_file (QString file_fullname) {
    // очистка данных файла
    data_file_P.vers.clear();
    data_file_P.filename.clear();
    data_file_P.packname.clear();
    data_file_P.smo_regnum.clear();
    data_file_P.point_regnum.clear();
    data_file_P.n_recs = 0;
    data_file_P.n_errs = 0;
    data_file_P.rep_list.clear();

    // загрузка протокола
    QFile* file = new QFile(file_fullname);
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text))  {
        QMessageBox::warning(this,"Невозможно открыть XML-файл","Невозможно открыть XML-файл");
        return false;
    }

    QFileInfo* finfo = new QFileInfo(file_fullname);
    QString pack_name = finfo->baseName();
    bool is_P1 = false;
    if (pack_name.right(2)=="_1") {
        is_P1 = true;
        pack_name = pack_name.left(pack_name.length()-2);
    } else {
        is_P1 = false;
    }
    delete finfo;
    pack_name = pack_name.replace("p", "").replace("P", "");
    // получим ID I-файла этого пакета
    QString sql_getI = "select fp.id_file_i "
                       "  from files_packs fp "
                       " where fp.pack_name='" + pack_name + "' ; ";
    QSqlQuery *query_getI = new QSqlQuery(db);
    bool res_getI = mySQL.exec(this, sql_getI, "Найдём I-файл в составе этого пакета", query_getI, true, db, data_app);

    int id_file_i = -1;
    if (res_getI && query_getI->next())
        id_file_i = query_getI->value(0).toInt();
    delete query_getI;


    QXmlStreamReader xml(file);

    QXmlStreamReader::TokenType token;
    QString token_name;
    token = xml.readNext();
    token_name = xml.name().toString();
    if (!xml.atEnd() && !xml.hasError()) {

        // REPLIST
        token = xml.readNext();
        token_name = xml.name().toString();
        while (!xml.atEnd() && !xml.hasError()) {
            if (token==QXmlStreamReader::EndElement) {
                // закрытие узла
                token = xml.readNext();
                token_name = xml.name().toString();

                if (token==QXmlStreamReader::EndDocument && token_name=="") break;
                else continue;
            }
            if (!xml.atEnd() && !xml.hasError() && token_name!="REPLIST") {
                // пропустим ненужный узел
                token = xml.readNext();
                token_name = xml.name().toString();
                continue;
            }
            data_file_P.filename = xml.attributes().value("FILENAME").toString().trimmed();
            data_file_P.packname = data_file_P.filename.mid(1,data_file_P.filename.size()-5);
            if (data_file_P.packname.right(2)=="_1")
                data_file_P.packname = data_file_P.packname.left(data_file_P.packname.length()-2);
            data_file_P.smo_regnum = xml.attributes().value("SMOCOD").toString().trimmed();
            data_file_P.point_regnum = xml.attributes().value("PRZCOD").toString().trimmed();
            data_file_P.vers = xml.attributes().value("VERS").toString().trimmed();
            data_file_P.n_recs = xml.attributes().value("NRECORDS").toString().toInt();
            data_file_P.n_errs = xml.attributes().value("NERR").toString().toInt();

            //----------------------------------------------------------------
            // найдём запись file_in_p
            QString sql_test_file =
                    "select id from files_in_p "
                    " where pack_name='" + pack_name + "' "
                    "   and file_name='" + data_file_P.filename.toLower().replace(".xml", "") + "' ; ";
            QSqlQuery *query_test_file = new QSqlQuery(db);
            bool res_test_file = mySQL.exec(this, sql_test_file, "Получим ID записи файла в базу", query_test_file, true, db, data_app);
            if (!res_test_file) {
                delete query_test_file;
                QMessageBox::warning(this,"Ошибка при поиске готовой записи файла в базе", "повоклорапфрпафорвалофыврофывпалывПАШГЫВПАРЦУПОПЛРЁЁпоиске готовой записи файла в базе произошла непредвиденная ошибка.");
                return false;
            }
            int id_file_p = -1;
            if (query_test_file->next()) {
                id_file_p = query_test_file->value(0).toInt();
            }
            delete query_test_file;

            int bn_res = QMessageBox::YesToAll;

            if ( f_yes2all ||
                 id_file_p<0 ||
                 (bn_res = QMessageBox::question(this,
                                                 "Внимание !!!", "Протокол с таким именем уже загружен.\n\n"
                                                 + pack_name + "\n\n"
                                                 "Повторить загрузку?",
                                                 QMessageBox::Yes|QMessageBox::No|QMessageBox::YesToAll,
                                                 QMessageBox::No))!=QMessageBox::No ) {
                f_yes2all = (bn_res==QMessageBox::YesToAll);
                //----------------------------------------------------------------
                // добавим файл file_in_p
                QString bin;
                mySQL.myBYTEA(file_fullname, bin);
                QString sql_add =
                        "insert into files_in_p (pack_name, vers, file_name, date_get, smo_regnum, point_regnum, n_recs, n_errs, bin) "
                        " values('" + data_file_P.packname.toLower().replace(".xml", "") + "', " +
                        "        '" + data_file_P.vers + "', " +
                        "        '" + data_file_P.filename.toLower().replace(".xml", "") + "', " +
                        "        '" + QDate::currentDate().toString("yyyy-MM-dd") + "', " +
                        "        '" + data_file_P.smo_regnum + "', " +
                        "        '" + data_file_P.point_regnum + "', " +
                        "        '" + QString::number(data_file_P.n_recs) + "', " +
                        "        '" + QString::number(data_file_P.n_errs) + "', " +
                        "        '" + bin + "') "
                        " returning id; ";
                QSqlQuery *query_add = new QSqlQuery(db);
                bool res_add = mySQL.exec(this, sql_add, "Запись полученного протокола в базу", query_add, true, db, data_app);
                if (!res_add) {
                    delete query_add;
                    QMessageBox::warning(this,"Ошибка при добавлении протокола в базу", "При добавлении протокола в базу произошла непредвиденная ошибка.");
                    return false;
                }
                if (query_add->next()) {
                    id_file_p = query_add->value(0).toInt();
                } else {
                    delete query_add;
                    QMessageBox::warning(this,"Ошибка при добавлении протокола в базу", "При добавлении в базу файла протокола не получен ID новой записи.\n\nОперация отменена.");
                    return false;
                }
                delete query_add;


                //----------------------------------------------------------------
                // добавим ссылку на протокол в записи посылки
                QString sql_link = "";
                if (is_P1) {
                    sql_link =
                            "update files_packs "
                            " set id_file_p_1=" + QString::number(id_file_p) + ", "
                            "     date_file_p_1=CURRENT_DATE "
                            " where pack_name=\'" + pack_name + "\' ; " ;
                } else {
                    sql_link =
                            "update files_packs "
                            " set id_file_p=" + QString::number(id_file_p) + ", "
                            "     date_file_p=CURRENT_DATE "
                            " where pack_name=\'" + pack_name + "\' ; " ;
                }
                QSqlQuery *query_link = new QSqlQuery(db);
                bool res_link = mySQL.exec(this, sql_link, "Добавление ссылки на протокол в запись посылки", query_link, true, db, data_app);
                if (!res_link) {
                    delete query_link;
                    QMessageBox::warning(this,"Ошибка при добавлении ссылки на протокол в запись посылки",
                                         "При добавлении ссылки на протокол в запись посылки произошла непредвиденная ошибка.");
                    return false;
                }
                delete query_link;
            }


            // REP
            token = xml.readNext();
            token_name = xml.name().toString();
            int cnt_recs = 0;
            while (!xml.atEnd() && !xml.hasError()) {
                if (token==QXmlStreamReader::EndElement) {
                    // закрытие узла
                    token = xml.readNext();
                    token_name = xml.name().toString();

                    if (token_name=="REPLIST") break;
                    else continue;
                }
                if (!xml.atEnd() && !xml.hasError() && token_name!="REP") {
                    // пропустим ненужный узел
                    token = xml.readNext();
                    token_name = xml.name().toString();

                    if (token_name=="REPLIST") break;
                    else continue;
                }

                int rep_status = 0;

                s_data_file_P_rep rep;
                rep.n_rec = -1;
                rep.id_tfoms = -1;
                rep.code_erp = -1;
                rep.comment = "";
                rep.insurance.ocato = "";
                rep.insurance.enp = "";
                rep.insurance.ogrn_smo = "";
                rep.insurance.in_erp = -999;
                rep.insurance.polis.pol_v = -1;
                rep.insurance.polis.pol_ser = "";
                rep.insurance.polis.pol_num = "";
                rep.insurance.polis.has_date_begin = false;
                rep.insurance.polis.has_date_end = false;
                rep.insurance.polis.has_date_stop = false;
                rep.insurance.polis.date_begin = QDate(1800,1,1);
                rep.insurance.polis.date_end = QDate(1800,1,1);
                rep.insurance.polis.date_stop = QDate(1800,1,1);

                rep.code_erp = xml.attributes().value("CODE_ERP").toString().toInt();
                rep.comment = xml.attributes().value("COMMENT").toString().trimmed().replace("'", "|");;
                rep.id_tfoms = xml.attributes().value("ID").toString().toInt();
                rep.n_rec = xml.attributes().value("N_REC").toString().toInt();

                // INSURANCE
                token = xml.readNext();
                token_name = xml.name().toString();
                while (!xml.atEnd() && !xml.hasError()) {
                    if (token==QXmlStreamReader::EndElement) {
                        // закрытие узла
                        token = xml.readNext();
                        token_name = xml.name().toString();

                        if (token_name=="REP") break;
                        else continue;
                    }
                    if (!xml.atEnd() && !xml.hasError() && token_name!="INSURANCE") {
                        // пропустим ненужный узел
                        token = xml.readNext();
                        token_name = xml.name().toString();

                        if (token_name=="REP") break;
                        else continue;
                    }

                    rep.insurance.enp = xml.attributes().value("ENP").toString().trimmed();
                    rep.insurance.in_erp = xml.attributes().value("ERP").toString().toInt();
                    rep.insurance.ogrn_smo = xml.attributes().value("OGRNSMO").toString().trimmed();
                    rep.insurance.ocato = xml.attributes().value("TER_ST").toString().trimmed();

                    // POLIS
                    token = xml.readNext();
                    token_name = xml.name().toString();
                    while (!xml.atEnd() && !xml.hasError()) {
                        if (token==QXmlStreamReader::EndElement) {
                            // закрытие узла
                            token = xml.readNext();
                            token_name = xml.name().toString();

                            if (token_name=="INSURANCE") break;
                            else continue;
                        }
                        if (!xml.atEnd() && !xml.hasError() && token_name!="POLIS") {
                            // пропустим ненужный узел
                            token = xml.readNext();
                            token_name = xml.name().toString();

                            if (token_name=="INSURANCE") break;
                            else continue;
                        }
                        rep.insurance.polis.pol_v = xml.attributes().value("VPOLIS").toString().toInt();
                        QString s;
                        s= xml.attributes().value("SPOLIS").toString().trimmed();
                        if (s.isEmpty()) {
                            rep.insurance.polis.pol_ser = "";
                        } else {
                            rep.insurance.polis.pol_ser = s;
                        }
                        rep.insurance.polis.pol_num = xml.attributes().value("NPOLIS").toString().trimmed();
                        s = xml.attributes().value("DBEG").toString().trimmed();
                        if (s.isEmpty()) {
                            rep.insurance.polis.has_date_begin = false;
                        } else {
                            rep.insurance.polis.has_date_begin = true;
                            rep.insurance.polis.date_begin = QDate::fromString(s, "yyyy-MM-dd");
                        }

                        // для полисов единого образца выделим серию из номера
                        if (rep.insurance.polis.pol_v==3 && rep.insurance.polis.pol_ser.isEmpty()) {
                            rep.insurance.polis.pol_ser = rep.insurance.polis.pol_num.left(4);
                            rep.insurance.polis.pol_num = rep.insurance.polis.pol_num.right(rep.insurance.polis.pol_num.length()-4);
                        }

                        s = xml.attributes().value("DEND").toString().trimmed();
                        if (s.isEmpty()) {
                            rep.insurance.polis.has_date_end = false;
                        } else {
                            rep.insurance.polis.has_date_end = true;
                            rep.insurance.polis.date_end = QDate::fromString(s, "yyyy-MM-dd");
                        }
                        s = xml.attributes().value("DSTOP").toString().trimmed();
                        if (s.isEmpty()) {
                            rep.insurance.polis.has_date_stop = false;
                        } else {
                            rep.insurance.polis.has_date_stop = true;
                            rep.insurance.polis.date_stop = QDate::fromString(s, "yyyy-MM-dd");
                        }

                        token = xml.readNext();
                        token_name = xml.name().toString();
                    }
                }
                data_file_P.rep_list.append(rep);

                cnt_recs++;

                // ----------------------------------------------------
                // поиск готовой строки протокола
                QString sql_test_frec =
                        "select id from frecs_in_p "
                        " where id_file_p=" + QString::number(id_file_p) + " "
                        "   and n_rec=" + QString::number(rep.n_rec) + " "
                        "   and ogrn_smo='" + rep.insurance.ogrn_smo + "' ; ";
                QSqlQuery *query_test_frec = new QSqlQuery(db);
                bool res_test_frec = mySQL.exec(this, sql_test_frec, "Получим ID записи файла в базу", query_test_frec, true, db, data_app);
                if (!res_test_frec) {
                    delete query_test_frec;
                    QMessageBox::warning(this,"Ошибка при поиске готовой строки файла в базе", "При поиске готовой строки файла в базе произошла непредвиденная ошибка.");
                    return false;
                }
                int id_frec_p = -1;
                if (query_test_frec->size()>0) {
                    // такая запись в базе уже есть - пропустим
                    delete query_test_frec;
                    ui->te_log->append( " = уже есть = " );
                    continue;
                }
                delete query_test_frec;

                // ----------------------------------------------------
                // найдём полис, к которому относится эта строка протокола
                int id_polis = -1;
                if (rep.insurance.polis.pol_ser.isEmpty())
                    rep.insurance.polis.pol_ser = "-";
                if (rep.insurance.polis.pol_num.isEmpty())
                    rep.insurance.polis.pol_num = "-1";
                QString sql_polis = "select ri.id_polis "
                                    "  from frecs_out_i ri "
                                    " where ri.id_file_i=" + QString::number(id_file_i) + " "
                                    "   and ri.n_rec=" + QString::number(rep.n_rec) + "; ";
                QSqlQuery *query_polis = new QSqlQuery(db);
                bool res_polis = mySQL.exec(this, sql_polis, "Найдём в базе полис для этой строки протокола", query_polis, true, db, data_app);

                if (res_polis && query_polis->next()) {
                    id_polis = query_polis->value(0).toInt();
                    delete query_polis;

                } else {
                    delete query_polis;
                    QString sql_polis = "select id "
                                        "  from polises "
                                        " where pol_v=" + QString::number(rep.insurance.polis.pol_v) + " and "
                                        "       ( ( pol_v=3 and pol_ser='" + rep.insurance.polis.pol_ser + "' and pol_num='" + rep.insurance.polis.pol_num + "' )"
                                        "         or"
                                        "         ( pol_v=2 and vs_num='" + rep.insurance.polis.pol_num + "' )"
                                        "       ) ; ";
                    query_polis = new QSqlQuery(db);
                    res_polis = mySQL.exec(this, sql_polis, "Найдём в базе полис для этой строки протокола", query_polis, true, db, data_app);

                    if (res_polis && query_polis->next()) {
                        id_polis = query_polis->value(0).toInt();
                    }
                    delete query_polis;
                }

                if (id_polis==-1) {
                    //QMessageBox::warning(this,"Ошибка при выборе полиса для строки протокола",
                    //                     "При поиске полиса, связанного со строкой протокола ничего не получено.\n\n"
                    //                     "id_file_i=" + QString::number(id_file_i) + ";  n_rec=" + QString::number(rep.n_rec) + "\n\n"
                    //                     "pol_v=" + QString::number(rep.insurance.polis.pol_v) + ";  pol_ser=" + rep.insurance.polis.pol_ser + ";  pol_num=" + rep.insurance.polis.pol_num + " \n");

                    ui->te_log->append("  ! полис не найден !  ");
                } else {
                    rep_status = 1;

                    // обновим значение in_erp по данным протокола
                    int in_erp = rep.insurance.in_erp;
                    if (in_erp!=1)  in_erp = 0;
                    QString sql_erp = "update polises "
                                      "   set in_erp=" + QString::number(in_erp) + ","
                                      "       tfoms_id=" + QString::number(rep.id_tfoms) + ", "
                                      "       tfoms_date=CURRENT_DATE "
                                      " where id=" + QString::number(id_polis) + " ; ";
                    QSqlQuery *query_erp = new QSqlQuery(db);
                    bool res_erp = mySQL.exec(this, sql_erp, "Обновим признак регистрации полиса в ЕРП по данным из протокола", query_erp, true, db, data_app);
                    if (!res_erp) {
                        QMessageBox::warning(this,"Ошибка при обновлении статуса ЕРП",
                                             "При попытке обновить признак регистрации полиса в ЕРП по данным из протокола произошла непредвиденная ошибка.");
                    }
                    delete query_erp;

                    // обновим значение ЕНП по данным протокола
                    QString enp = rep.insurance.enp.trimmed();
                    if (!enp.isEmpty()) {
                        QString sql_enp = "update polises "
                                          "   set enp='" + enp + "' "
                                          " where id=" + QString::number(id_polis) + " "
                                          "   and (enp is NULL or char_length(enp)<16) ; ";
                        QSqlQuery *query_enp = new QSqlQuery(db);
                        bool res_enp = mySQL.exec(this, sql_enp, "Обновим ЕНП по данным из протокола", query_enp, true, db, data_app);
                        if (!res_enp) {
                            QMessageBox::warning(this,"Ошибка при обновлении ЕНП",
                                                 "При попытке обновить ЕНП по данным из протокола произошла непредвиденная ошибка.");
                        }
                        delete query_enp;
                    }

                    // если строка сообщает об успехе ...
                    /*protocols_sql += " and r.comment<>'полис нового образца включен в заявку на изготовление' and r.comment<>'ОК!' ";
                    if (rep.comment)
                    // статусы прежних записей для этого полиса
                    QString enp = rep.insurance.enp.trimmed();
                    if (!enp.isEmpty()) {
                        QString sql_enp = "update polises "
                                          " set enp='" + enp + "' "
                                          " where id=" + QString::number(id_polis) + " "
                                          "   and enp is NULL ; ";
                        QSqlQuery *query_enp = new QSqlQuery(db);
                        bool res_enp = mySQL.exec(this, sql_enp, "Обновим ЕНП по данным из протокола", query_enp, true, db, data_app);
                        if (!res_enp) {
                            QMessageBox::warning(this,"Ошибка при обновлении ЕНП",
                                                 "При попытке обновить ЕНП по данным из протокола произошла непредвиденная ошибка.");
                        }
                        delete query_enp;
                    }*/
                }

                ui->te_log->append("  " + QString(
                                   (rep.insurance.polis.pol_ser + " № " + rep.insurance.polis.pol_num) +
                                   (rep.insurance.polis.has_date_begin ? (",  дата выдачи " + rep.insurance.polis.date_begin.toString("dd.MM.yyyy")) : "") +
                                   (rep.insurance.polis.has_date_end   ? (",  дата окончания " + rep.insurance.polis.date_end.toString("dd.MM.yyyy"))   : "") +
                                   (rep.insurance.polis.has_date_stop  ? (",  дата изъятия " + rep.insurance.polis.date_begin.toString("dd.MM.yyyy")) : "") +
                                   "  -->>  " + rep.comment ).trimmed() );

                // ----------------------------------------------- //
                // если это файл P.. и на него был файл P.._1  ... //
                QString sql_pp1 = "select f1.id as id_file_p1, r1.id as id_frec_p1 "
                                  "  from files_in_p f1 "
                                  "  left join frecs_in_p r1 on(r1.id_file_p=f1.id "
                                  "        and r1.pol_num='" + rep.insurance.polis.pol_num + "' "
                                  "        and r1.pol_ser='" + rep.insurance.polis.pol_ser + "' "
                                  "        and r1.comment='" + rep.comment + "' "
                                  "        and r1.pol_v=" + QString::number(rep.insurance.polis.pol_v) + " ) "
                                  " where f1.file_name='" + data_file_P.filename.toLower().replace(".xml", "") + "_1' "
                                  "   and r1.id is not NULL ; ";
                QSqlQuery *query_pp1 = new QSqlQuery(db);
                bool res_pp1 = mySQL.exec(this, sql_pp1, "Поиск предыдущей версии строки протокола", query_pp1, true, db, data_app);
                if (!res_pp1) {
                    QMessageBox::warning(this,"Ошибка при поиске старой копии протокола",
                                         "При попытке найти старую версию протокола произошла непредвиденная ошибка.");
                }
                if (query_pp1->next()) {
                    int id_file_p_1 = query_pp1->value(0).toInt();
                    int id_frec_p_1 = query_pp1->value(1).toInt();

                    QString sql_upd_frec = "update frecs_in_p "
                                           "   set id_file_p=" + QString::number(id_file_p_1) + " "
                                           " where id=" + QString::number(id_frec_p_1) + " ; ";
                    QSqlQuery *query_upd_frec = new QSqlQuery(db);
                    bool res_upd_frec = mySQL.exec(this, sql_upd_frec, "Перенос строки протокола из файла Р.._1 в файл Р..", query_upd_frec, true, db, data_app);
                    if (!res_upd_frec) {
                        QMessageBox::warning(this,"Ошибка при переносе строки протокола из файла Р.._1 в файл Р..",
                                             "При переносе строки протокола из файла Р.._1 в файл Р.. произошла непредвиденная ошибка.");
                    }
                    delete query_upd_frec;                    
                    ui->te_log->append( " + ошибка уже исправлена при обрабртке предварительного протокола + " );
                    id_frec_p = id_frec_p_1;
                } else {
                    // добавим в базу строку протокола
                    QString sql_add_frec =
                            "insert into frecs_in_p (id_file_p, n_rec, ogrn_smo, id_tfoms, ocato, enp, pol_v, pol_ser, pol_num, "
                            "                        date_begin, date_end, date_stop, in_erp, comment, "
                            "                        id_polis, status, date_add, date_resend, date_ok, date_hide, id_operator) "
                            " values(" + QString::number(id_file_p) + ", " +
                            "        " + QString::number(rep.n_rec) + ", " +
                            "       '" + rep.insurance.ogrn_smo + "', " +
                            "        " + QString::number(rep.id_tfoms) + ", " +
                            "       '" + rep.insurance.ocato + "', " +
                            "       '" + rep.insurance.enp + "', " +
                            "        " + QString::number(rep.insurance.polis.pol_v) + ", " +
                            "       '" + rep.insurance.polis.pol_ser + "', " +
                            "       '" + rep.insurance.polis.pol_num + "', " +
                            "        " + QString(rep.insurance.polis.has_date_begin? ("'" + rep.insurance.polis.date_begin.toString("yyyy-MM-dd")+ "'") : "NULL") + ", " +
                            "        " + QString(rep.insurance.polis.has_date_end  ? ("'" + rep.insurance.polis.date_end.toString("yyyy-MM-dd")  + "'") : "NULL") + ", " +
                            "        " + QString(rep.insurance.polis.has_date_stop ? ("'" + rep.insurance.polis.date_stop.toString("yyyy-MM-dd") + "'") : "NULL") + ", " +
                            "        " + QString::number(rep.insurance.in_erp) + ", " +
                            "       '" + rep.comment + "', " +
                            "        " + QString(id_polis>0 ? QString::number(id_polis) : "NULL") + ", " +
                            "        " + QString::number(rep_status) + ", " +
                            "        CURRENT_DATE, " +
                            "        NULL, " +
                            "        NULL, " +
                            "        NULL, " +
                            "        NULL) "
                            " returning id; ";
                    QSqlQuery *query_add_frec = new QSqlQuery(db);
                    bool res_add_frec = mySQL.exec(this, sql_add_frec, "Запись полученного протокола в базу", query_add_frec, true, db, data_app);
                    if (!res_add_frec) {
                        delete query_add_frec;
                        QMessageBox::warning(this,"Ошибка при добавлении строки протокола в базу", "При добавлении строки протокола в базу произошла непредвиденная ошибка.");
                        return false;
                    }
                    if (query_add_frec->next()) {
                        id_frec_p = query_add_frec->value(0).toInt();
                    } else {
                        delete query_add_frec;
                        QMessageBox::warning(this,"Ошибка при добавлении строки протокола в базу", "При добавлении в базу строки протокола не получен ID новой записи.");
                        continue;
                    }
                    delete query_add_frec;
                }
                delete query_pp1;

                // ----------------------------------------------------
                // если это OK - пометим эту и прошлые записи как подтверждённые
                if ( rep.comment=="полис нового образца включен в заявку на изготовление" ) {

                    QString sql_ok =
                            "update polises "                   // пометка что заявка на изготовление полиса отправлена в ГОЗНАК
                            "   set tfoms_date=CURRENT_DATE "
                            " where id=" + QString::number(id_polis) + " "
                            "   and tfoms_date is NULL; "

                            "update frecs_in_p "                // запомним дату ОК
                            "   set status=3, "
                            "       date_ok=CURRENT_DATE "
                            " where ogrn_smo='" + rep.insurance.ogrn_smo + "' "
                            "   and pol_v=" + QString::number(rep.insurance.polis.pol_v) + " "
                            "   and ( (enp is not NULL and enp<>'' and enp='" + rep.insurance.enp + "') "
                            "      or (pol_ser is not NULL and pol_ser<>'-' and pol_ser='" + rep.insurance.polis.pol_ser + "' and pol_num is not NULL and pol_num<>-1 and pol_num=" + rep.insurance.polis.pol_num + ") "
                            "      or (pol_ser is not NULL and pol_ser ='-' and pol_num is not NULL and pol_num<>-1 and pol_num=" + rep.insurance.polis.pol_num + ") "
                            "      or (id_polis=" + QString::number(id_polis) + ") "
                            "       ) ; ";
                    QSqlQuery *query_ok = new QSqlQuery(db);
                    bool res_ok = mySQL.exec(this, sql_ok, "Отметка подтверждённых записей", query_ok, true, db, data_app);
                    if (!res_ok) {
                        delete query_ok;
                        QMessageBox::warning(this,"Ошибка при попытке отметить подтверждённые записи", "При попытке отметить подтверждённые записи произошла непредвиденная ошибка.");
                        return false;
                    }
                    delete query_ok;
                }

                // ----------------------------------------------------
                // если это OK - пометим эту и прошлые записи как подтверждённые
                if ( rep.comment=="ОК!" ) {

                    QString sql_ok =
                            "update frecs_in_p "                // запомним дату ОК
                            "   set status=3, "
                            "       date_ok=CURRENT_DATE "
                            " where ogrn_smo='" + rep.insurance.ogrn_smo + "' "
                            "   and pol_v=" + QString::number(rep.insurance.polis.pol_v) + " "
                            "   and ( (enp is not NULL and enp<>'' and enp='" + rep.insurance.enp + "') "
                            "      or (pol_ser is not NULL and pol_ser<>'-' and pol_ser='" + rep.insurance.polis.pol_ser + "' and pol_num is not NULL and pol_num<>-1 and pol_num=" + rep.insurance.polis.pol_num + ") "
                            "      or (pol_ser is not NULL and pol_ser ='-' and pol_num is not NULL and pol_num<>-1 and pol_num=" + rep.insurance.polis.pol_num + ") "
                            "      or (id_polis=" + QString::number(id_polis) + ") "
                            "       ) ; ";
                    QSqlQuery *query_ok = new QSqlQuery(db);
                    bool res_ok = mySQL.exec(this, sql_ok, "Отметка подтверждённых записей", query_ok, true, db, data_app);
                    if (!res_ok) {
                        delete query_ok;
                        QMessageBox::warning(this,"Ошибка при попытке отметить подтверждённые записи", "При попытке отметить подтверждённые записи произошла непредвиденная ошибка.");
                        return false;
                    }
                    delete query_ok;
                }
                QApplication::processEvents();
            }
            if (cnt_recs==0) {
                ui->te_log->append("  в файле нет ни одной записи" );
            }
        }
    }
    file->close();

    return true;
}


bool protocols_wnd::process_S_file (QString file_fullname) {
    // очистка данных файла
    data_file_S.vers.clear();
    data_file_S.filename.clear();
    data_file_S.smo_regnum.clear();
    data_file_S.point_regnum.clear();
    data_file_S.n_recs = 0;
    data_file_S.stop_list.clear();


    // загрузка стоп-листа
    QFile* file = new QFile(file_fullname);
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text))  {
        QMessageBox::warning(this,"Невозможно открыть XML-файл","Невозможно открыть XML-файл");
        return false;
    }
    QXmlStreamReader xml(file);

    QXmlStreamReader::TokenType token;
    QString token_name;
    token = xml.readNext();
    token_name = xml.name().toString();
    if (!xml.atEnd() && !xml.hasError()) {

        db.transaction();

        // STOPLIST
        token = xml.readNext();
        token_name = xml.name().toString();
        while (!xml.atEnd() && !xml.hasError()) {
            if (token==QXmlStreamReader::EndElement) {
                // закрытие узла
                token = xml.readNext();
                token_name = xml.name().toString();

                if (token==QXmlStreamReader::EndDocument && token_name=="") break;
                else continue;
            }
            if (!xml.atEnd() && !xml.hasError() && token_name!="STOPLIST") {
                // пропустим ненужный узел
                token = xml.readNext();
                token_name = xml.name().toString();
                continue;
            }
            data_file_S.filename = xml.attributes().value("FILENAME").toString().trimmed();
            data_file_S.n_recs = xml.attributes().value("NRECORDS").toString().toInt();
            data_file_S.smo_regnum = xml.attributes().value("SMOCOD").toString().trimmed();
            data_file_S.point_regnum = xml.attributes().value("PRZCOD").toString().trimmed();
            data_file_S.vers = xml.attributes().value("VERS").toString().trimmed();

            // STOP
            do {
                token = xml.readNext();
                token_name = xml.name().toString();
            } while (!xml.atEnd() && !xml.hasError() && token_name != "STOP");

            QString s;

            while (!xml.atEnd() && !xml.hasError()) {
                if (token==QXmlStreamReader::EndElement) {
                    // закрытие узла
                    token = xml.readNext();
                    token_name = xml.name().toString();

                    if (token_name=="STOPLIST") break;
                    else continue;
                }
                if (!xml.atEnd() && !xml.hasError() && token_name!="STOP") {
                    // пропустим ненужный узел
                    token = xml.readNext();
                    token_name = xml.name().toString();

                    if (token_name=="STOPLIST") break;
                    else continue;
                }

                s_data_file_S_stop stop;
                stop.id_tfoms = -1;
                stop.polis.pol_v = -1;
                stop.polis.pol_ser = "";
                stop.polis.pol_num = "";
                stop.polis.has_date_begin = false;
                stop.polis.has_date_end = false;
                stop.polis.has_date_stop = false;
                stop.polis.date_begin = QDate(1800,1,1);
                stop.polis.date_end = QDate(1800,1,1);
                stop.polis.date_stop = QDate(1800,1,1);

                stop.id_tfoms = xml.attributes().value("ID").toString().toInt();
                stop.stop_reason = xml.attributes().value("REASON").toString().toInt();
                stop.enp = xml.attributes().value("ENP").toString();

                s = xml.attributes().value("DDEATH").toString().trimmed();
                if (s.isEmpty()) {
                    stop.has_date_death = false;
                } else {
                    stop.has_date_death = true;
                    stop.date_death = QDate::fromString(s, "yyyy-MM-dd");
                }

                // POLIS
                do {
                    token = xml.readNext();
                    token_name = xml.name().toString();
                } while (!xml.atEnd() && !xml.hasError() && token_name != "POLIS");

                while (!xml.atEnd() && !xml.hasError()) {
                    if (token==QXmlStreamReader::EndElement) {
                        // закрытие узла
                        token = xml.readNext();
                        token_name = xml.name().toString();

                        if (token_name=="STOP") break;
                        else continue;
                    }
                    if (!xml.atEnd() && !xml.hasError() && token_name!="POLIS") {
                        // пропустим ненужный узел
                        token = xml.readNext();
                        token_name = xml.name().toString();

                        if (token_name=="STOP") break;
                        else continue;
                    }
                    stop.polis.pol_v = xml.attributes().value("VPOLIS").toString().toInt();
                    s= xml.attributes().value("SPOLIS").toString().trimmed();
                    if (s.isEmpty()) {
                        stop.polis.pol_ser = "";
                    } else {
                        stop.polis.pol_ser = s;
                    }
                    stop.polis.pol_num = xml.attributes().value("NPOLIS").toString().trimmed();
                    s = xml.attributes().value("DBEG").toString().trimmed();
                    if (s.isEmpty()) {
                        stop.polis.has_date_begin = false;
                    } else {
                        stop.polis.has_date_begin = true;
                        stop.polis.date_begin = QDate::fromString(s, "yyyy-MM-dd");
                    }

                    // для полисов единого образца выделим серию из номера
                    if (stop.polis.pol_v==3 && stop.polis.pol_ser.isEmpty()) {
                        stop.polis.pol_ser = stop.polis.pol_num.left(4);
                        stop.polis.pol_num = stop.polis.pol_num.right(stop.polis.pol_num.length()-4);
                    }

                    s = xml.attributes().value("DEND").toString().trimmed();
                    if (s.isEmpty()) {
                        stop.polis.has_date_end = false;
                    } else {
                        stop.polis.has_date_end = true;
                        stop.polis.date_end = QDate::fromString(s, "yyyy-MM-dd");
                    }
                    s = xml.attributes().value("DSTOP").toString().trimmed();
                    if (s.isEmpty()) {
                        stop.polis.has_date_stop = false;
                    } else {
                        stop.polis.has_date_stop = true;
                        stop.polis.date_stop = QDate::fromString(s, "yyyy-MM-dd");
                    }

                    token = xml.readNext();
                    token_name = xml.name().toString();
                }
                data_file_S.stop_list.append(stop);

            }
        }


        //----------------------------------------------------------------
        // собственно обработка данных Stop-файла
        //----------------------------------------------------------------
        // найдём запись file_in_s
        QString sql_test_file =
                "select id from files_in_s "
                " where file_name='" + data_file_S.filename.toLower().replace(".xml", "") + "' ; ";
        QSqlQuery *query_test_file = new QSqlQuery(db);
        bool res_test_file = mySQL.exec(this, sql_test_file, "Получим ID записи файла в базу", query_test_file, true, db, data_app);
        if (!res_test_file) {
            delete query_test_file;
            QMessageBox::warning(this,"Ошибка при поиске готовой записи файла в базе", "При поиске готовой записи файла в базе произошла непредвиденная ошибка.");
            return false;
        }
        int id_file_s = -1;
        if (query_test_file->next()) {
            id_file_s = query_test_file->value(0).toInt();
        }
        delete query_test_file;


        //----------------------------------------------------------------
        // найдём или добавим запись файла
        if (id_file_s<0) {
            // добавим файл file_in_s
            QString bin;
            mySQL.myBYTEA(file_fullname, bin);
            QString sql_add =
                    "insert into files_in_s (vers, file_name, date_get, smo_regnum, point_regnum, n_recs, bin) "
                    " values('" + data_file_S.vers + "', " +
                    "        '" + data_file_S.filename + "', " +
                    "        '" + QDate::currentDate().toString("yyyy-MM-dd") + "', " +
                    "        '" + data_file_S.smo_regnum + "', " +
                    "        '" + data_file_S.point_regnum + "', " +
                    "        '" + QString::number(data_file_S.n_recs) + "', " +
                    "        '" + bin + "') "
                    " returning id; ";
            QSqlQuery *query_add = new QSqlQuery(db);
            bool res_add = mySQL.exec(this, sql_add, "Запись полученного стоп-листа в базу", query_add, true, db, data_app);
            if (!res_add) {
                delete query_add;
                QMessageBox::warning(this,"Ошибка при добавлении стоп-листа в базу", "При добавлении стоп-листа в базу произошла непредвиденная ошибка.");
                return false;
            }
            if (query_add->next()) {
                id_file_s = query_add->value(0).toInt();
            } else {
                delete query_add;
                QMessageBox::warning(this,"Ошибка при добавлении стоп-листа в базу", "При добавлении в базу файла стоп-листа не получен ID новой записи.\n\nОперация отменена.");
                return false;
            }
            delete query_add;
        }


        // ----------------------------------------------------
        // переберём данные файла
        foreach (s_data_file_S_stop stop, data_file_S.stop_list) {
            // поиск готовой строки стоп-листа
            QString sql_test_frec = "";
            if (stop.polis.pol_v==2) {
                sql_test_frec += "select id "
                                 "  from frecs_in_s "
                                 " where status=2 "     // запись найдена, полис уже закрыт
                                 "   /* and id_file_s=" + QString::number(id_file_s) + " */ "
                                 "   and pol_v=" + QString::number(stop.polis.pol_v) + " "
                                 "   and pol_num='" + stop.polis.pol_num + "' ; ";
            } else {
                sql_test_frec += "select id "
                                 "  from frecs_in_s "
                                 " where status=2 "     // запись найдена, полис уже закрыт
                                 "   /* and id_file_s=" + QString::number(id_file_s) + " */ "
                                 "   and pol_v=" + QString::number(stop.polis.pol_v) + " "
                                 "   and pol_ser='" + stop.polis.pol_ser + "' "
                                 "   and pol_num='" + stop.polis.pol_num + "' ; ";
            }
            QSqlQuery *query_test_frec = new QSqlQuery(db);
            bool res_test_frec = mySQL.exec(this, sql_test_frec, "Получим ID записи файла в базу", query_test_frec, true, db, data_app);
            if (!res_test_frec) {
                delete query_test_frec;
                QMessageBox::warning(this,"Ошибка при поиске готовой строки файла в базе", "При поиске готовой строки файла в базе произошла непредвиденная ошибка.");
                return false;
            }
            int id_frec_s = -1;
            if (query_test_frec->size()>0) {
                delete query_test_frec;
                // такая запись в базе уже есть - пропустим
                ui->te_log->append( QString("  Получен стоп-файл на полис  ") +
                                    stop.polis.pol_ser + " № " + stop.polis.pol_num + "  ->  - уже есть - " );
                continue;
            }
            delete query_test_frec;

            ui->te_log->append( QString("  Получен стоп-файл на полис  ") +
                                (stop.polis.pol_ser + " № " + stop.polis.pol_num) +
                                (stop.polis.has_date_begin ? (",  дата выдачи " + stop.polis.date_begin.toString("dd.MM.yyyy")) : "") +
                                (stop.polis.has_date_end   ? (",  дата окончания " + stop.polis.date_end.toString("dd.MM.yyyy"))   : "") +
                                (stop.polis.has_date_stop  ? (",  дата изъятия " + stop.polis.date_stop.toString("dd.MM.yyyy")) : "") );

            // -------------------------------------------------------
            // найдём полис, к которому относится эта строка стоп-листа
            int id_polis = -1;
            int id_person = -1;
            int stop_status = 0;
            if (stop.polis.pol_ser.isEmpty())
                stop.polis.pol_ser = "-";
            if (stop.polis.pol_num.isEmpty())
                stop.polis.pol_num = "-1";
            QString sql_polis = "";
            if (stop.polis.pol_v==2) {
                sql_polis = "select id, id_person "
                            " from polises "
                            " where pol_v=2 "
                            "   and vs_num='" + stop.polis.pol_num+ "' ; ";
            } else {
                sql_polis = "select id, id_person "
                            " from polises "
                            " where pol_v=" + QString::number(stop.polis.pol_v) + " "
                            "   and pol_ser='" + stop.polis.pol_ser + "' "
                            "   and pol_num='" + stop.polis.pol_num + "' ; ";
            }
            QSqlQuery *query_polis = new QSqlQuery(db);
            bool res_polis = mySQL.exec(this, sql_polis, "Найдём в базе полис для этой строки стоп-листа", query_polis, true, db, data_app);
            if (!res_polis) {
                QMessageBox::warning(this,"Ошибка при выборе полиса для строки стоп-листа",
                                     "При поиске полиса, связанного со строкой стоп-листа произошла непредвиденная ошибка.");
            } else {
                if (query_polis->next()) {
                    id_polis  = query_polis->value(0).toInt();
                    id_person = query_polis->value(1).toInt();
                    stop_status = 1;
                    ui->te_log->append("    ->  полис найден  ");
                } else {
                    stop_status = -1;
                    ui->te_log->append("    !!! ПОЛИС НЕ НАЙДЕН !!!  ");
                }
            }
            delete query_polis;


            if (id_polis>0) {
                // получим расшифровку причины закрытия полиса
                QString sql_reason = "";
                sql_reason = "select text "
                             "  from spr_stop_reasons s "
                             " where code=" + QString::number(stop.stop_reason) + " ; ";
                QSqlQuery *query_reason = new QSqlQuery(db);
                bool res_reason = mySQL.exec(this, sql_reason, "получим расшифровку причины закрытия полиса", query_reason, true, db, data_app);
                if (!res_reason) {
                    QMessageBox::warning(this,"Ошибка при выборе причины закрытия полиса",
                                         "При поиске причины закрытия полиса произошла непредвиденная ошибка.");
                } else {
                    if (query_reason->next()) {
                        QString s_reason = query_reason->value(0).toString();
                        ui->te_log->append("    ->  " + s_reason);

                        QString event_code = "";
                        QString event_comment = "";
                        int pers_status = 1;
                        switch (stop.stop_reason) {
                        case 1:
                            event_code = "П022";
                            event_comment = "стоп-файл  -  смерть застрахованного";
                            pers_status = -10;
                            break;
                        case 2:
                            event_code = "П021";
                            event_comment = "стоп-файл  -  ежегодная замена страховой";
                            pers_status = -2;
                            break;
                        case 3:
                            event_code = "П021";
                            event_comment = "стоп-файл  -  замена страховой по причине смены места жительства";
                            pers_status = -2;
                            break;
                        case 4:
                            event_code = "П021";
                            event_comment = "стоп-файл  -  выдача ВС в другой СМО";
                            pers_status = -2;
                            break;
                        case 5:
                            event_code = "П023";
                            event_comment = "стоп-файл  -  выявление дубликата";
                            pers_status = 999;
                            break;
                        case 7:
                            event_code = "П025";
                            event_comment = "стоп-файл  -  аннулирование страховки";
                            pers_status = -1;
                            break;
                        case 8:
                            // ничего не делаем - эта ситуация уже отражена в нашей базе
                            event_code = " ?? ";
                            event_comment = "стоп-файл  -  замена ДПФС в той же СМО";
                            pers_status = 999;
                            break;
                        case 6:
                        default:
                            event_code = "П021";
                            event_comment = "стоп-файл  -  прочее";
                            pers_status = -1;
                            break;
                        };


                        // ------------------------------------------------------------- //
                        // выберем все действующие полисы персоны выданные до даты стопа //
                        // ------------------------------------------------------------- //
                        QString sql_pol;
                        sql_pol = "select id "
                                  "  from polises p "
                                  " where id_person=" + QString::number(id_person) + " "
                                  "   and p.date_begin<'" + stop.polis.date_stop.toString("yyyy-MM-dd") + "' "
                                  "   and (date_end is NULL or date_stop is NULL or date_stop<>'" + stop.polis.date_stop.toString("yyyy-MM-dd") + "') ; ";
                        QSqlQuery *query_pol= new QSqlQuery(db);
                        bool res_pol= mySQL.exec(this, sql_pol, QString("Пометим бланк полиса как недействующий"), *query_pol, true, db, data_app);

                        while (query_pol->next()) {
                            int id_polis = query_pol->value(0).toInt();
                            // ----------------------- //
                            // попробуем закрыть полис //
                            // ----------------------- //
                            QString sql_close_pol;

                            // date_end - просто берём из файла или используем текущую дату
                            if (stop.polis.has_date_end)
                                sql_close_pol = "update polises "
                                                "   set date_end='" + stop.polis.date_end.toString("yyyy-MM-dd") + "', "
                                                "       _id_last_point=" + QString::number(data_app.id_point) + ", "
                                                "       _id_last_operator=" + QString::number(data_app.id_operator) + " "
                                                " where id=" + QString::number(id_polis) + " ; ";
                            else
                                sql_close_pol = "update polises "
                                                "   set date_end='" + QDate::currentDate().toString("yyyy-MM-dd") + "', "
                                                "       _id_last_point=" + QString::number(data_app.id_point) + ", "
                                                "       _id_last_operator=" + QString::number(data_app.id_operator) + " "
                                                " where id=" + QString::number(id_polis) + " ; ";

                            /*
                            // date_stop - просто берём из файла или используем текущую дату
                            if (stop.polis.has_date_stop)
                                sql_close_pol = "update polises "
                                                "   set date_stop='" + stop.polis.date_stop.toString("yyyy-MM-dd") + "' "
                                                " where id=" + QString::number(id_polis) + " ; ";
                            else
                                sql_close_pol = "update polises "
                                                "   set date_stop='" + QDate::currentDate().toString("yyyy-MM-dd") + "' "
                                                " where id=" + QString::number(id_polis) + " ; ";
                                                */

                            // date_stop должна быть в любом случае - иначе запись полиса останется действующей
                            if (stop.polis.has_date_stop)
                                sql_close_pol += "update polises "
                                                 "   set date_stop='" + stop.polis.date_stop.toString("yyyy-MM-dd") + "' "
                                                 " where id=" + QString::number(id_polis) + " "
                                                 "   /*and ( date_stop is NULL "
                                                 "      or date_stop>'" + stop.polis.date_stop.toString("yyyy-MM-dd") + "' )*/ ; ";
                            else
                                sql_close_pol += "update polises "
                                                 "   set date_stop=CURRENT_DATE "
                                                 " where id=" + QString::number(id_polis) + " "
                                                 "   /*and ( date_stop is NULL "
                                                 "      or date_stop>CURRENT_DATE )*/ ; ";

                            QSqlQuery *query_close_pol = new QSqlQuery(db);
                            bool res_close_pol = mySQL.exec(this, sql_close_pol, QString("Закрытие полиса по стоп-файлу"), *query_close_pol, true, db, data_app);
                            if (res_close_pol) {
                                delete query_close_pol;
                                stop_status = 2;
                                ui->te_log->append("        ->  полис закрыт - OK  ");
                            } else {
                                delete query_close_pol;
                                stop_status = -1;
                                ui->te_log->append("        !!! ПРИ ПОПЫТКЕ ЗАКРЫТЬ ПОЛИС ПРОИЗОШЛА ОШИБКА !!!  ");
                            }

                            // ------------------------------------ //
                            // если есть событие на закрытие полиса //
                            // ------------------------------------ //
                            if (event_code!=" ?? ") {
                                // сформируем событие на закрытие полиса
                                QString sql_s = "";
                                sql_s = "insert into events (id_point, id_operator, id_polis, id_vizit, event_code, event_dt, event_time, comment, status, id_person, id_person_old, id_udl, id_drp) "
                                        "select " + QString::number(data_app.id_point) + ", "
                                        "       " + QString::number(data_app.id_operator) + ", "
                                        "       p.id, "
                                        "       -1, "
                                        "       '" + event_code + "', "
                                        "       '" + QDate::currentDate().toString("yyyy-MM-dd") + "', "
                                        "       '" + QTime::currentTime().toString("hh:mm:ss") + "', "
                                        "       '" + event_comment + "', "
                                        "       -1, "
                                        "       e.id, "
                                        "       o.id, "
                                        "       e.id_udl, "
                                        "       e.id_drp "
                                        "  from polises p"
                                        "  left join persons e on(e.id=p.id_person) "
                                        "  left join persons_old o on(o.id_person=e.id) "
                                        " where p.id=" + QString::number(id_polis) + " ; ";

                                QSqlQuery *query_s= new QSqlQuery(db);
                                bool res_s= mySQL.exec(this, sql_s, QString("Сформируем событие закрытия полиса"), *query_s, true, db, data_app);

                                if (!res_s) {
                                    delete query_s;
                                    db.rollback();
                                    this->setCursor(Qt::ArrowCursor);
                                    QMessageBox::warning(this, "Ошибка при генерации события закрытия полиса", "При генерации события закрытия полиса в связи с расстрахованием персоны произошла неожиданная ошибка");
                                    return false;
                                }
                                delete query_s;
                            }
                        }
                        delete query_pol;


                        // -------------------------------------- //
                        // Пометим бланк полиса как недействующий //
                        // -------------------------------------- //
                        QString sql_bs = "";
                        //if (stop.polis.pol_v==2) {
                            sql_bs+= "update blanks_vs b"
                                     "   set status=-5, "
                                     "       date_spent = coalesce(date_spent, '" + stop.polis.date_stop.toString("yyyy-MM-dd") + "'), "
                                     "       date_waste = coalesce(date_waste, '" + stop.polis.date_stop.toString("yyyy-MM-dd") + "') "
                                     " where b.id_person=" + QString::number(id_person) + " "
                                     "   and b.status in(1,4) ; ";
                        //} else {
                            sql_bs+= "update blanks_pol b"
                                     "   set status=-5, "
                                     "       date_spent = coalesce(date_spent, '" + stop.polis.date_stop.toString("yyyy-MM-dd") + "'), "
                                     "       date_waste = coalesce(date_waste, '" + stop.polis.date_stop.toString("yyyy-MM-dd") + "') "
                                     " where b.id_person=" + QString::number(id_person) + " "
                                     "   and b.status in(1,4) ; ";
                        //}

                        QSqlQuery *query_bs= new QSqlQuery(db);
                        bool res_bs= mySQL.exec(this, sql_bs, QString("Пометим бланк полиса как недействующий"), *query_bs, true, db, data_app);
                        delete query_bs;


                        // ------------------------------------- //
                        // если надо задать новый статус персоны //
                        // ------------------------------------- //
                        if (pers_status!=999) {
                            // сохраним новый статус
                            QSqlQuery *query = new QSqlQuery(db);
                            QString sql = "";
                            sql += "update persons "
                                   " set status=" + QString::number(pers_status) + ", "
                                       " id_polis=NULL, "
                                       " _id_last_point=" + QString::number(data_app.id_point) + ", "
                                       " _id_last_operator=" + QString::number(data_app.id_operator) + ", "
                                       " _date_insure_end=min_date(_date_insure_end, CURRENT_DATE) ";
                            if (stop.has_date_death)
                                sql += ", date_death='" + stop.date_death.toString("yyyy-MM-dd") + "' ";
                            sql += " where id=" + QString::number(id_person) + " ; ";
                            if (!(mySQL.exec(this, sql, QString("Список статусов персоны"), *query, true, db, data_app))) {
                                delete query;
                                QMessageBox::warning(this, "Ошибка при обновлении записи",
                                                     "При обновлении записи произошла неожиданная ошибка.\n" + sql + "\n\nОперация отменена.");
                                //data_pers.status = model_persons.data(model_persons.index(index_pers.row(), 21), Qt::EditRole).toInt();  // persons.status
                                //ui->combo_pers_status->setCurrentIndex(ui->combo_pers_status->findData(data_pers.status));
                                db.rollback();
                                this->setCursor(Qt::ArrowCursor);
                                return false;
                            }
                            delete query;
                        }


                    } else {
                        stop_status = -1;
                        ui->te_log->append("    !!! КОД ПРИЧИНЫ ЗАКРЫТИЯ  " + QString::number(stop.stop_reason) + "  надо обработать вручную !!!");
                    }
                }
                delete query_reason;

                // при необходимости - перенесём запись персоны в архив
                QSqlQuery *query_arch = new QSqlQuery(db);
                QString sql_arch = "";
                sql_arch += "select * from update_pers_archive(" + QString::number(id_person) + ") ; ";
                if (!(mySQL.exec(this, sql_arch, QString("Перенос персоны в архив"), *query_arch, true, db, data_app))) {
                    delete query_arch;
                    QMessageBox::warning(this, "Ошибка при переносе персоны в архив",
                                         "При переносе персоны в архив произошла неожиданная ошибка.\n" + sql_arch + "\n\nОперация отменена.");
                    db.rollback();
                    this->setCursor(Qt::ArrowCursor);
                    return false;
                }
                while (query_arch->next()) {
                    int res_arch = query_arch->value(0).toInt();
                    switch (res_arch) {
                    case 1:
                        ui->te_log->append("        ->  данные персоны перенесены в архив  ");
                        break;
                    case 0:
                        ui->te_log->append("        !!  данные персоны не могут быть перенесены в архив, так как используются в других записях  ");
                        break;
                    case-1:
                        ui->te_log->append("        ??  данные персоны уже удалены ??  ");
                        break;
                    default:
                        ui->te_log->append("        ???  ");
                        break;
                    }
                }
                delete query_arch;
            }


            // ----------------------------------------------------
            // добавим в базу строку стопа
            QString sql_add_frec =
                    "insert into frecs_in_s (id_file_s, id_tfoms, s_reason, enp, pol_v, pol_ser, pol_num, date_begin, date_end, date_stop, id_polis, status) "
                    " values( " + QString::number(id_file_s) + ", " +
                    "         " + QString::number(stop.id_tfoms) + ", " +
                    "         " + QString::number(stop.stop_reason) + ", " +
                    "        '" + stop.enp + "', " +
                    "         " + QString::number(stop.polis.pol_v) + ", " +
                    "        '" + stop.polis.pol_ser + "', " +
                    "        '" + stop.polis.pol_num + "', " +
                    "         " + QString(stop.polis.has_date_begin? ("'" + stop.polis.date_begin.toString("yyyy-MM-dd")+ "'") : "NULL") + ", " +
                    "         " + QString(stop.polis.has_date_end  ? ("'" + stop.polis.date_end.toString("yyyy-MM-dd")  + "'") : "NULL") + ", " +
                    "         " + QString(stop.polis.has_date_stop ? ("'" + stop.polis.date_stop.toString("yyyy-MM-dd") + "'") : "NULL") + ", " +
                    "         " + QString(id_polis>0 ? QString::number(id_polis) : "NULL") + ", " +
                    "         " + QString::number(stop_status) +") "
                    " returning id; ";
            QSqlQuery *query_add_frec = new QSqlQuery(db);
            bool res_add_frec = mySQL.exec(this, sql_add_frec, "Запись полученного стопа в базу", query_add_frec, true, db, data_app);
            if (!res_add_frec) {
                delete query_add_frec;
                QMessageBox::warning(this,"Ошибка при добавлении строки стопа в базу", "При добавлении строки стопа в базу произошла непредвиденная ошибка.");
                return false;
            }
            if (query_add_frec->next()) {
                id_frec_s = query_add_frec->value(0).toInt();
            } else {
                delete query_add_frec;
                QMessageBox::warning(this,"Ошибка при добавлении строки стопа в базу", "При добавлении в базу строки стопа не получен ID новой записи.");
                continue;
            }
            delete query_add_frec;
        }

        db.commit();
    }
    file->close();


    return test_S_file(data_file_S);
}

bool protocols_wnd::test_S_file (s_data_file_S &data_file_S) {
    ui->te_log->append( "\n"
                        "  => КОНТРОЛЬ ИСПОЛНЕНИЯ СТОП-ФАЙЛА  " + data_file_S.filename);
    bool is_ok = true;

    // переберём данные
    foreach (s_data_file_S_stop stop, data_file_S.stop_list) {
        // поиск строки стоп-листа
        QString sql_test_file = "";
        if (stop.polis.pol_v==2) {
            ui->te_log->append("    ->  строка ВС: " + stop.polis.pol_num);
            sql_test_file += "select id "
                             "  from frecs_in_s "
                             " where status=2 "     // запись найдена, полис уже закрыт
                             "   and pol_v=" + QString::number(stop.polis.pol_v) + " "
                             "   and pol_num='" + stop.polis.pol_num + "' ; ";
        } else {
            ui->te_log->append("    ->  строка ЕНП: " + stop.polis.pol_ser + " " + stop.polis.pol_num);
            sql_test_file += "select id "
                             "  from frecs_in_s "
                             " where status=2 "     // запись найдена, полис уже закрыт
                             "   and pol_v=" + QString::number(stop.polis.pol_v) + " "
                             "   and pol_ser='" + stop.polis.pol_ser + "' "
                             "   and pol_num='" + stop.polis.pol_num + "' ; ";
        }
        QSqlQuery *query_test_file = new QSqlQuery(db);
        int id_file_s = -1;
        bool res_test_file = mySQL.exec(this, sql_test_file, "Получим ID записи файла ", query_test_file, true, db, data_app);
        if (!res_test_file) {
            delete query_test_file;
            QMessageBox::warning(this,"Ошибка при поиске строки файла в базе", "При поиске строки файла в базе произошла непредвиденная ошибка.");
            return false;
        }
        if (query_test_file->size()<=0 || !query_test_file->next()) {
            // файл не найден !!!
            //ui->te_log->append("    ->  !!! СТРОКА СТОПА ПОЛИСА НЕ НАЙДЕНА !!!");
            //return false;
        }
        id_file_s = query_test_file->value(0).toInt();
        delete query_test_file;

        int id_frec_s = -1;
        // -------------------------------------------------------
        // найдём полис, к которому относится эта строка стоп-листа
        int id_polis = -1;
        int id_person = -1;
        if (stop.polis.pol_ser.isEmpty())
            stop.polis.pol_ser = "-";
        if (stop.polis.pol_num.isEmpty())
            stop.polis.pol_num = "-1";
        QString sql_polis = "";
        if (stop.polis.pol_v==2) {
            sql_polis = "select id, id_person "
                        " from polises "
                        " where pol_v=2 "
                        "   and vs_num='" + stop.polis.pol_num+ "' ; ";
        } else {
            sql_polis = "select id, id_person "
                        " from polises "
                        " where pol_v=" + QString::number(stop.polis.pol_v) + " "
                        "   and pol_ser='" + stop.polis.pol_ser + "' "
                        "   and pol_num='" + stop.polis.pol_num + "' ; ";
        }
        QSqlQuery *query_polis = new QSqlQuery(db);
        bool res_polis = mySQL.exec(this, sql_polis, "Найдём в базе полис для этой строки стоп-листа", query_polis, true, db, data_app);
        if (!res_polis) {
            QMessageBox::warning(this,"Ошибка при выборе полиса для строки стоп-листа",
                                 "При поиске полиса, связанного со строкой стоп-листа произошла непредвиденная ошибка.");
        } else {
            if (query_polis->next()) {
                id_polis  = query_polis->value(0).toInt();
                id_person = query_polis->value(1).toInt();
            } else {
                ui->te_log->append("    ->  !!! ПОЛИС НЕ НАЙДЕН !!!  ");
            }
        }
        delete query_polis;


        if (id_polis>0) {
            // сравним дату выдачи, запланированную дату закрытия, датиу изъятия и статус полиса со стопом
            QString sql_test = "";
            sql_test = "select p.pol_v, coalesce(p.date_begin,'1900-01-01'), coalesce(p.date_end,'1900-01-01'), coalesce(p.date_stop,'1900-01-01'), e.event_code "
                       "  from polises p "
                       "  left join events e on e.id=p.id_evt_end "
                       " where p.id=" + QString::number(id_polis) + " ; ";
            QSqlQuery *query_test = new QSqlQuery(db);
            bool res_test = mySQL.exec(this, sql_test, "Сравним статус полиса с данными стопа", query_test, true, db, data_app);
            if (!res_test) {
                QMessageBox::warning(this,"Ошибка при сравнении статуса полиса с данными стопа",
                                     "При сравнении статуса полиса с данными стопа произошла непредвиденная ошибка.");
            } else {
                if (query_test->next()) {
                    int pol_v = query_test->value(0).toInt();
                    QDate date_begin = query_test->value(1).toDate();
                    QDate date_end   = query_test->value(2).toDate();
                    QDate date_stop  = query_test->value(3).toDate();
                    QString event_code = query_test->value(4).toString();

                    if (date_begin>QDate(1920,1,1) && date_begin<stop.polis.date_begin) {
                        is_ok = false;
                        ui->te_log->append("        !!!  РАЗНАЯ ДАТА НАЧАЛА  !!!  " + stop.polis.date_begin.toString("yyyy-MM-dd") + " (стоп-файл) != " + date_begin.toString("yyyy-MM-dd") + " (полис)");
                    } else
                        ui->te_log->append("    +   дата начала   " + stop.polis.date_begin.toString("yyyy-MM-dd") + " (стоп-файл) <= " + date_begin.toString("yyyy-MM-dd") + " (полис)");
                    if (date_end>QDate(1920,1,1) && date_end!=stop.polis.date_end) {
                        is_ok = false;
                        ui->te_log->append("        !!!  РАЗНАЯ ПЛАНОВАЯ ДАТА КОНЦА  !!!  " + stop.polis.date_end.toString("yyyy-MM-dd") + " (стоп-файл) != " + date_end.toString("yyyy-MM-dd") + " (полис)");
                    } else
                        ui->te_log->append("    +   плановая дата конца   " + stop.polis.date_end.toString("yyyy-MM-dd") + " (стоп-файл) == " + date_end.toString("yyyy-MM-dd") + " (полис)");
                    if (pol_v!=2 && date_stop>QDate(1920,1,1) && date_stop!=stop.polis.date_stop) {
                        is_ok = false;
                        ui->te_log->append("        !!!  РАЗНАЯ ДАТА ИЗЪЯТИЯ  !!!  " + stop.polis.date_stop.toString("yyyy-MM-dd") + " (стоп-файл) != " + date_stop.toString("yyyy-MM-dd") + " (полис)");
                    } else
                        ui->te_log->append("    +   дата изьятия   " + stop.polis.date_stop.toString("yyyy-MM-dd") + " (стоп-файл) == " + date_stop.toString("yyyy-MM-dd") + " (полис)");
                    if (is_ok)
                        ui->te_log->append("        ok");
                }
            }


            if (id_person>0 && stop.has_date_death) {
                // сравним дату смерти со стопом
                QString sql_test = "";
                sql_test = "select e.date_death "
                           "  from persons e "
                           " where e.id=" + QString::number(id_person) + " ; ";
                QSqlQuery *query_test = new QSqlQuery(db);
                bool res_test = mySQL.exec(this, sql_test, "Сравним статус персоны с данными стопа", query_test, true, db, data_app);
                if (!res_test) {
                    QMessageBox::warning(this,"Ошибка при сравнении статуса персоны с данными стопа",
                                         "При сравнении статуса персоны с данными стопа произошла непредвиденная ошибка.");
                } else {
                    if (query_test->next()) {
                        QDate date_death = query_test->value(0).toDate();
                        bool has_date_death = !query_test->value(0).isNull();

                        if (stop.has_date_death && !has_date_death) {
                            is_ok = false;
                            ui->te_log->append("        !!!  ОТСУТСТВУЕТ ДАТА СМЕРТИ  !!!  " + stop.date_death.toString("yyyy-MM-dd") + " (стоп-файл) != НЕТ (персона)");
                        } else if (stop.has_date_death && has_date_death && date_death!=stop.date_death) {
                            is_ok = false;
                            ui->te_log->append("        !!!  РАЗНАЯ ДАТА СМЕРТИ  !!!  " + stop.date_death.toString("yyyy-MM-dd") + " (стоп-файл) != " + date_death.toString("yyyy-MM-dd") + " (персона)");
                        } else
                            ui->te_log->append("    +   дата смерти   " + date_death.toString("yyyy-MM-dd") + " (стоп-файл) == " + stop.date_death.toString("yyyy-MM-dd") + " (персона)");
                        if (is_ok)
                            ui->te_log->append("        ok");
                    }
                }
            }
        }
    }
    return is_ok;
}

bool protocols_wnd::process_F_file (QString file_fullname) {

    // очистка данных файла
    data_file_F.vers.clear();
    data_file_F.packname.clear();
    data_file_F.filename.clear();
    data_file_F.err_list.clear();


    // загрузка ФЛК-листа
    QFile* file = new QFile(file_fullname);
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text))  {
        QMessageBox::warning(this,"Невозможно открыть XML-файл","Невозможно открыть XML-файл");
        return false;
    }
    QXmlStreamReader xml(file);

    QXmlStreamReader::TokenType token;
    QString token_name;
    token = xml.readNext();
    token_name = xml.name().toString();
    if (!xml.atEnd() && !xml.hasError()) {

        // FLK_P
        token = xml.readNext();
        token_name = xml.name().toString();
        while (!xml.atEnd() && !xml.hasError()) {
            if (token==QXmlStreamReader::EndElement) {
                // закрытие узла
                token = xml.readNext();
                token_name = xml.name().toString();

                if (token==QXmlStreamReader::EndDocument && token_name=="") break;
                else continue;
            }
            if (!xml.atEnd() && !xml.hasError() && token_name!="FLK_P") {
                // пропустим ненужный узел
                token = xml.readNext();
                token_name = xml.name().toString();
                continue;
            }
            data_file_F.filename = xml.attributes().value("FNAME").toString().trimmed();
            data_file_F.packname = data_file_F.filename.mid(1,data_file_F.filename.size()-5);
            data_file_F.filename_i = xml.attributes().value("FNAME_I").toString().trimmed();
            data_file_F.vers = xml.attributes().value("VERS").toString().trimmed();

            //----------------------------------------------------------------
            // найдём запись file_in_f
            QString sql_test_file =
                    "select id from files_in_f "
                    " where pack_name='" + data_file_F.packname.toLower().replace(".xml", "") + "' "
                    "       and file_name='" + data_file_F.filename.toLower().replace(".xml", "") + "' "
                    "       and file_name_i='" + data_file_F.filename_i.toLower().replace(".xml", "") + "' ; ";
            QSqlQuery *query_test_file = new QSqlQuery(db);
            bool res_test_file = mySQL.exec(this, sql_test_file, "Получим ID записи файла в базу", query_test_file, true, db, data_app);
            if (!res_test_file) {
                delete query_test_file;
                QMessageBox::warning(this,"Ошибка при поиске готовой записи файла в базе", "При поиске готовой записи файла в базе произошла непредвиденная ошибка.");
                return false;
            }
            int id_file_f = -1;
            if (query_test_file->next()) {
                id_file_f = query_test_file->value(0).toInt();
            }
            delete query_test_file;

            if (id_file_f<0) {
                //----------------------------------------------------------------
                // добавим файл file_in_f
                QString bin;
                mySQL.myBYTEA(file_fullname, bin);
                QString sql_add =
                        "insert into files_in_f (vers, pack_name, file_name, file_name_i, date_get, n_recs, status, bin) "
                        " values('" + data_file_F.vers + "', " +
                        "        '" + data_file_F.packname.toLower().replace(".xml", "") + "', " +
                        "        '" + data_file_F.filename.toLower().replace(".xml", "") + "', " +
                        "        '" + data_file_F.filename_i.toLower().replace(".xml", "") + "', " +
                        "        '" + QDate::currentDate().toString("yyyy-MM-dd") + "', " +
                        "        0, " +
                        "        0, " +
                        "        '" + bin + "') "
                        " returning id; ";
                QSqlQuery *query_add = new QSqlQuery(db);
                bool res_add = mySQL.exec(this, sql_add, "Запись полученного ФЛК-листа в базу", query_add, true, db, data_app);
                if (!res_add) {
                    delete query_add;
                    QMessageBox::warning(this,"Ошибка при добавлении ФЛК-листа в базу", "При добавлении ФЛК-листа в базу произошла непредвиденная ошибка.");
                    return false;
                }
                if (query_add->next()) {
                    id_file_f = query_add->value(0).toInt();
                } else {
                    delete query_add;
                    QMessageBox::warning(this,"Ошибка при добавлении ФЛК-листа в базу", "При добавлении в базу файла ФЛК-листа не получен ID новой записи.\n\nОперация отменена.");
                    return false;
                }
                delete query_add;



                //----------------------------------------------------------------
                // добавим ссылку на ФЛК в записи посылки
                QString pack_name = data_file_F.packname.toLower().trimmed();
                pack_name = pack_name.replace("p", "");
                QString sql_link =
                        "update files_packs "
                        " set id_file_f=" + QString::number(id_file_f) + ", "
                        "     date_file_f=CURRENT_DATE "
                        " where pack_name=\'" + pack_name + "\' ; " ;
                QSqlQuery *query_link = new QSqlQuery(db);
                bool res_link = mySQL.exec(this, sql_link, "Добавление ссылки на ФЛК в запись посылки", query_link, true, db, data_app);
                if (!res_link) {
                    delete query_link;
                    QMessageBox::warning(this,"Ошибка при добавлении ссылки на ФЛК в запись посылки",
                                         "При добавлении ссылки на ФЛК в запись посылки произошла непредвиденная ошибка.");
                    return false;
                }
                delete query_link;
            } else {
                // такая запись в базе уже есть - пропустим
                ui->te_log->append( " = уже есть = " );
                token = xml.readNext();
                break;
            }

            ui->te_log->append( QString(" !!! ПОЛУЧЕН ПРОТОКОЛ ОШИБОК ФЛК !!!  \n") +
                                "     фонд не принял файл  \'" + data_file_F.filename_i + "\'");


            // PR
            do {
                token = xml.readNext();
                token_name = xml.name().toString();
            } while (!xml.atEnd() && !xml.hasError() && token_name != "PR");

            while (!xml.atEnd() && !xml.hasError()) {
                if (token==QXmlStreamReader::EndElement) {
                    // закрытие узла
                    token = xml.readNext();
                    token_name = xml.name().toString();

                    if (token_name=="FLK_P") break;
                    else continue;
                }
                if (!xml.atEnd() && !xml.hasError() && token_name!="PR") {
                    // пропустим ненужный узел
                    token = xml.readNext();
                    token_name = xml.name().toString();

                    if (token_name=="FLK_P") break;
                    else continue;
                }

                //int stop_status = 0;

                s_data_file_F_err err;
                err.code_err = -1;
                err.field_name = "";
                err.base_name = "";
                err.n_rec = -1;
                err.comment = "";

                err.code_err = xml.attributes().value("OSHIB").toString().toInt();
                err.field_name = xml.attributes().value("IM_POL").toString();
                err.base_name = xml.attributes().value("BAS_EL").toString();
                err.n_rec = xml.attributes().value("N_REC").toString().toInt();
                err.comment = xml.attributes().value("COMMENT").toString().trimmed().replace("'", "|");
                data_file_F.err_list.append(err);

                // ----------------------------------------------------
                // поиск готовой строки ФЛК-листа
                QString sql_test_frec =
                        "select id from frecs_in_f "
                        " where id_file_f=" + QString::number(id_file_f) + " "
                        "       and code_err=" + QString::number(err.code_err) + " "
                        "       and n_rec=" + QString::number(err.n_rec) + " "
                        "       and field_name='" + err.field_name + "' "
                        "       and base_name='" + err.base_name + "' ; ";
                QSqlQuery *query_test_frec = new QSqlQuery(db);
                bool res_test_frec = mySQL.exec(this, sql_test_frec, "Получим ID записи файла в базу", query_test_frec, true, db, data_app);
                if (!res_test_frec) {
                    delete query_test_frec;
                    QMessageBox::warning(this,"Ошибка при поиске готовой строки файла в базе", "При поиске готовой строки файла в базе произошла непредвиденная ошибка.");
                    return false;
                }
                int id_frec_s = -1;
                if (query_test_frec->size()>0) {
                    // такая запись в базе уже есть - пропустим
                    delete query_test_frec;
                    ui->te_log->append( " = уже есть = " );
                    continue;
                }
                delete query_test_frec;

                ui->te_log->append( "     ->>  строка № " + QString::number(err.n_rec) + ","
                                    "  поле \'" + err.field_name + "\',"
                                    "  база \'" + err.base_name + "\',"
                                    "  код ошибки " + QString::number(err.code_err) + ","
                                    "\n" + err.comment);


                // ----------------------------------------------------
                // добавим в базу строку ФЛК
                QString sql_add_frec =
                        "insert into frecs_in_f (id_file_f, n_rec, code_err, field_name, base_name, comment, status) "
                        " values(" + QString::number(id_file_f) + ", " +
                        "        " + QString::number(err.n_rec) + ", " +
                        "        " + QString::number(err.code_err) + ", " +
                        "        '" + err.field_name + "', " +
                        "        '" + err.base_name + "', " +
                        "        '" + err.comment + "', " +
                        "        0) "
                        " returning id; ";
                QSqlQuery *query_add_frec = new QSqlQuery(db);
                bool res_add_frec = mySQL.exec(this, sql_add_frec, "Запись строки протокола ФЛК в базу", query_add_frec, true, db, data_app);
                if (!res_add_frec) {
                    delete query_add_frec;
                    QMessageBox::warning(this,"Ошибка при добавлении строки протокола ФЛК в базу", "При добавлении строки протокола ФЛК в базу произошла непредвиденная ошибка.");
                    return false;
                }
                if (query_add_frec->next()) {
                    id_frec_s = query_add_frec->value(0).toInt();
                } else {
                    delete query_add_frec;
                    QMessageBox::warning(this,"Ошибка при добавлении строки протокола ФЛК в базу", "При добавлении в базу строки протокола ФЛК не получен ID новой записи.");
                    continue;
                }
                delete query_add_frec;

                token = xml.readNext();
            }
        }
    }
    file->close();
    return true;
}

bool protocols_wnd::process_K_file (QString fileName) {
    return true;
}

bool protocols_wnd::process_U_file (QString fileName) {
    return true;
}

bool protocols_wnd::process_EO_file (QString fileName) {
    QFile file(fileName);
    QFileInfo fi(fileName);
    QString base_name = fi.baseName().replace("EO", "MO").replace("LO", "MO");

    // получим ID выходного файла, на который пришёл этот протокол
    QString sql_get_outfileID =
            "select id from files_out_assig "
            " where file_name='" + base_name + "' ; ";
    QSqlQuery *query_get_outfileID = new QSqlQuery(db);
    bool res_get_outfileID = mySQL.exec(this, sql_get_outfileID, "Получим ID исходящего файла прикрелений", query_get_outfileID, true, db, data_app);
    if (!res_get_outfileID) {
        delete query_get_outfileID;
        QMessageBox::warning(this,"Ошибка при поиске ID исходящего файла прикрелений", "При поиске ID исходящего файла прикрелений произошла непредвиденная ошибка.");
        return false;
    }
    if (!query_get_outfileID->next()) {
        // исходящий файл не наден
        ui->te_log->append( "  исходящий файл не наден, пропускаем" );
        return true;
    }
    int id_MO_file = query_get_outfileID->value(0).toInt();
    delete query_get_outfileID;

    if (file.open(QIODevice::ReadOnly)) {
        QString line;

        QApplication::processEvents();

m_new_line:
        line = file.readLine();
        if (line.isEmpty())
            return true;

        int     lenght    = line.length();
        int     pos0      = line.indexOf(";", 0);
        int     ret_n     = line.mid(0, pos0).toInt();
        int     pos1      = line.indexOf(";", pos0+1);
        QString ret_enp   = line.mid(pos0+1, pos1 - pos0);
        int     ret_error = abs(line.right(lenght - pos1 -1).trimmed().toInt()) * (-1);

        // получим данные отосланного прикрепления
        QString sql_get_assig_data =
                "select id, n, id_person, code_mo, code_mp, code_ms, snils_mt "
                "  from frecs_out_assig "
                " where id_file_assig=" + QString::number(id_MO_file) + " "
                "   and n=" + QString::number(ret_n) + " ; ";
        QSqlQuery *query_get_assig_data = new QSqlQuery(db);
        bool res_get_assig_data = mySQL.exec(this, sql_get_assig_data, "Получим данные прикрепления", query_get_assig_data, true, db, data_app);
        if (!res_get_assig_data) {
            delete query_get_assig_data;
            QMessageBox::warning(this,"Ошибка при попытке получить данные прикрепления", "При попытке получить данные прикрепления произошла непредвиденная ошибка.");
            return false;
        }
        if (!query_get_assig_data->next()) {
            delete query_get_assig_data;
            // исходящий файл не наден
            ui->te_log->append( "  - данные прикрепления не найдены, пропускаем -  " );
            goto m_new_line;
        }
        int     assig_id_rec   = query_get_assig_data->value(0).toInt();
        int     assig_n        = query_get_assig_data->value(1).toInt();
        int     assig_id_person= query_get_assig_data->value(2).toInt();
        QString assig_code_mo  = query_get_assig_data->value(3).toString();
        QString assig_code_mp  = query_get_assig_data->value(4).toString();
        QString assig_code_ms  = query_get_assig_data->value(5).toString();
        QString assig_snils_mt = query_get_assig_data->value(6).toString();
        delete query_get_assig_data;

        // проверим наличие действующего прикрепления
        QString sql_test_assig_error =
                "select count(*) as cnt "
                "  from persons_assig "
                " where id_person=" + QString::number(assig_id_person) + " "
                "   and code_mo='" + assig_code_mo + "' "
                "   and code_mp='" + assig_code_mp + "' "
                "   and code_ms='" + assig_code_ms + "' "
                "   and snils_mt='" + assig_snils_mt + "' "
                "   and assig_date<=CURRENT_DATE "
                "   and ( unsig_date is NULL or unsig_date>CURRENT_DATE ) "
                "; ";
        QSqlQuery *query_test_assig_error = new QSqlQuery(db);
        bool res_test_assig_error = mySQL.exec(this, sql_test_assig_error, "Поиск действующего прикрепления", query_test_assig_error, true, db, data_app);
        if (!res_test_assig_error) {
            delete query_test_assig_error;
            QMessageBox::warning(this,"Ошибка при поиске действующего прикрепления", "При поиске действующего прикрепления произошла непредвиденная ошибка.");
            return false;
        }
        query_test_assig_error->next();
        int cnt = query_test_assig_error->value(0).toInt();
        delete query_test_assig_error;
        if (cnt==0) {
            ui->te_log->append( "  - такое прикрепление не найдено, пропускаем -  " );
            goto m_new_line;
        }

        // сохраним код ошибки прикрепления
        QString sql_save_assig_error =
                "update persons_assig "
                "   set status=" + QString::number(ret_error) + " "
                " where id_person=" + QString::number(assig_id_person) + " "
                "   and code_mo='" + assig_code_mo + "' "
                "   and code_mp='" + assig_code_mp + "' "
                "   and code_ms='" + assig_code_ms + "' "
                "   and snils_mt='" + assig_snils_mt + "' "
                "   and assig_date<=CURRENT_DATE "
                "   and ( unsig_date is NULL or unsig_date>CURRENT_DATE ) "
                "; ";
        QSqlQuery *query_save_assig_error = new QSqlQuery(db);
        bool res_save_assig_error = mySQL.exec(this, sql_save_assig_error, "Запомним код ошибки", query_save_assig_error, true, db, data_app);
        if (!res_save_assig_error) {
            delete query_save_assig_error;
            QMessageBox::warning(this,"Ошибка при попытке запомнить код ошибки", "При попытке запомнить код ошибки произошла непредвиденная ошибка.");
            return false;
        }
        delete query_save_assig_error;
        ui->te_log->append( "  ! " + QString::number(cnt) + " ошибочных прикреплений ! " );

        goto m_new_line;

    } else {
        // исходящий файл не наден
        ui->te_log->append( "  - ошибка чтения файла -" );
        return false;
    }

    QApplication::processEvents();
    return true;
}

void protocols_wnd::on_tab_protocols_clicked(const QModelIndex &index) {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_protocols || !ui->tab_protocols->selectionModel()) {
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_protocols->selectionModel()->selection().indexes();

    id_pfile  = -1;
    id_pack   = -1;
    id_ifile  = -1;
    id_person = -1;
    id_polis  = -1;
    id_event = -1;

    if (indexes.size()>0) {
        QModelIndex index = indexes.front();

        id_pfile = model_protocols.data(model_protocols.index(index.row(), 21), Qt::EditRole).toInt();
        id_pack  = model_protocols.data(model_protocols.index(index.row(), 22), Qt::EditRole).toInt();
        id_ifile = model_protocols.data(model_protocols.index(index.row(), 23), Qt::EditRole).toInt();

        if (!model_protocols.data(model_protocols.index(index.row(), 16), Qt::EditRole).isNull())
            id_person = model_protocols.data(model_protocols.index(index.row(), 16), Qt::EditRole).toInt();

        if (!model_protocols.data(model_protocols.index(index.row(), 17), Qt::EditRole).isNull())
            id_polis  = model_protocols.data(model_protocols.index(index.row(), 17), Qt::EditRole).toInt();

        if (!model_protocols.data(model_protocols.index(index.row(), 18), Qt::EditRole).isNull())
            id_event = model_protocols.data(model_protocols.index(index.row(), 18), Qt::EditRole).toInt();

        ui->combo_file->setCurrentIndex(ui->combo_file->findData(id_pfile));
        QString comment = model_protocols.data(model_protocols.index(index.row(), 14), Qt::EditRole).toString().replace("|","\"");
        ui->lab_comment->setText(comment);
        ui->bn_to_polis->setEnabled(true);

    } else {
        ui->lab_comment->setText("-//-");
        ui->bn_to_polis->setEnabled(false);
    }
}

void protocols_wnd::on_bn_to_polis_clicked() {
    QString message;

    if (id_person<1) {
        message += "\nЗапись человека не найдена !\n";
    } else {
        persons_w->select_person_in_tab(id_person);
    }
    if (id_polis<1) {
        message += "\nЗапись полиса не найдена !\n";
    } else {
        persons_w->select_polis_in_tab(id_polis);
    }
    if (id_event<1) {
        message += "\nЗапись события не найдена !\n";
    } else {
        persons_w->select_event_in_tab(id_event);
    }

    if (id_person>0 || id_polis>0 || id_event>0) {
        //ui->lab_comment->setMaximumWidth(100);
        ui->pan_protokols_up->setVisible(false);
        QRect screen = QApplication::desktop()->screenGeometry();
        this->resize(250,600);
        this->move(screen.width()-this->size().width(), (screen.height()-this->size().height())/2 - 100);
        this->resize(250,600);
        this->move(screen.width()-this->size().width(), (screen.height()-this->size().height())/2 - 100);
        ui->bn_hide->setText("Развернуть");

        persons_w->setFocus();
    } else {
        QMessageBox::warning(this, "Неполные данные", message);
    }
}

void protocols_wnd::on_tab_protocols_activated(const QModelIndex &index) {
    //if (QMessageBox::question(this, "Перейти к полису?", "Свернуть это окно и найти соответствующий полис в окне \"Застрахованные\"?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes) {
        on_bn_to_polis_clicked();
    //}
}

void protocols_wnd::on_bn_refresh_flk_clicked() {
    refresh_flk_tab();
}

void protocols_wnd::refresh_flk_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, "Нет доступа к базе данных",
                             "Нет открытого соединения к базе данных?\n "
                             "Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n "
                             "Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику.");
        return;
    }

    QString flk_sql;
    flk_sql += "select f.id, f.file_name, f.file_name_i, f.date_get, f.n_recs, f.status, r.id, r.n_rec, r.code_err, r.field_name, r.base_name, r.comment, r.status "
               " from files_in_f f "
               "      left join frecs_in_f r on(f.id=r.id_file_f and r.status=0) "
               " where f.status=0";
    flk_sql += " order by f.file_name_i, r.n_rec ; ";

    model_flk.setQuery(flk_sql,db);
    QString err2 = model_flk.lastError().driverText();

    // подключаем модель из БД
    ui->tab_flk->setModel(&model_flk);

    // обновляем таблицу
    ui->tab_flk->reset();

    // задаём ширину колонок
    ui->tab_flk->setColumnWidth( 0,  3);     // f.id
    ui->tab_flk->setColumnWidth( 1,  1);     // f.file_name
    ui->tab_flk->setColumnWidth( 2,150);     // f.file_name_i
    ui->tab_flk->setColumnWidth( 3, 70);     // f_date_get
    ui->tab_flk->setColumnWidth( 4,  1);     // f.n_recs
    ui->tab_flk->setColumnWidth( 5, 50);     // f.status
    ui->tab_flk->setColumnWidth( 6,  1);     // r.id
    ui->tab_flk->setColumnWidth( 7, 50);     // r.n_rec
    ui->tab_flk->setColumnWidth( 8, 60);     // r.code_err
    ui->tab_flk->setColumnWidth( 9,120);     // r.field_name
    ui->tab_flk->setColumnWidth(10,140);     // r.base_name
    ui->tab_flk->setColumnWidth(11,300);     // r.comment
    ui->tab_flk->setColumnWidth(12, 50);     // r.status

    // правим заголовки
    model_flk.setHeaderData( 0, Qt::Horizontal, ("ID"));
    model_flk.setHeaderData( 1, Qt::Horizontal, ("файл ФЛК"));
    model_flk.setHeaderData( 2, Qt::Horizontal, ("повреждённая\nпосылка"));
    model_flk.setHeaderData( 3, Qt::Horizontal, ("дата\nполучения"));
    model_flk.setHeaderData( 4, Qt::Horizontal, ("число\nзаписей"));
    model_flk.setHeaderData( 5, Qt::Horizontal, ("статут\nфайла"));
    model_flk.setHeaderData( 6, Qt::Horizontal, ("ID rec"));
    model_flk.setHeaderData( 7, Qt::Horizontal, ("N rec"));
    model_flk.setHeaderData( 8, Qt::Horizontal, ("код \nошибки"));
    model_flk.setHeaderData( 9, Qt::Horizontal, ("имя поля"));
    model_flk.setHeaderData(10, Qt::Horizontal, ("имя базового элемента"));
    model_flk.setHeaderData(11, Qt::Horizontal, ("комментарий"));
    model_flk.setHeaderData(12, Qt::Horizontal, ("статус\nстроки"));
    ui->tab_flk->repaint();

    ui->lab_comment->setText("-//-");

    this->setCursor(Qt::ArrowCursor);

    if (model_flk.rowCount()<1) {
        ui->pan_flk->setVisible(false);
    } else {
        ui->pan_flk->setVisible(true);
    }
}


void protocols_wnd::on_bn_hide_rec_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n"
                                                                   " Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n"
                                                                   " Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_protocols || !ui->tab_protocols->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] над таблицей протоколов.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_protocols->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this, "Нужно подтверждение",
                                  "Вы действительно хотите скрыть выбранную строку с сообщением об ошибке в данных полиса?\n\n"
                                  "(cоответствующая ошибка в данных должна быть уже исправлена?)\n",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        QModelIndex index = indexes.front();
        int id_protokol = model_protocols.data(model_protocols.index(index.row(), 0), Qt::EditRole).toInt();

        db.transaction();
        QString sql_upd = "update frecs_in_p "
                          " set status=-2, "
                          "     date_hide=CURRENT_DATE,"
                          "     id_operator=" + QString::number(data_app.id_operator) + " "
                          " where id=" + QString::number(id_protokol) + " ; ";

        QSqlQuery *query_upd = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_upd, "Спрячем строку протокола", *query_upd, true, db, data_app)) {
            delete query_upd;
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке спрятать строку протокола произошла неожиданная ошибка.\n\n"
                                                                 "Сохранение отменено.");
            return;
        }
        delete query_upd;
        db.commit();

        refresh_protocols_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void protocols_wnd::on_bn_hide_flk_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, "Нет доступа к базе данных",
                             "Нет открытого соединения к базе данных?\n"
                             " Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n"
                             " Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику.");
        return;
    }
    if (!ui->tab_flk || !ui->tab_flk->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] над таблицей ФЛК.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_flk->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this, "Нужно подтверждение",
                                  "Вы действительно хотите скрыть выбранную строку с соообщением об ошибке при контроле ФЛК посылки?\n"
                                  "Будут скрыты все строки выбранного протокола ФЛК.\n\n"
                                  "(cоответствующая ошибка в данных должна быть уже исправлена?)\n",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        QModelIndex index = indexes.front();
        int id_flk = model_flk.data(model_flk.index(index.row(), 0), Qt::EditRole).toInt();

        db.transaction();
        // добавим фирму
        QString sql_upd = "update files_in_f "
                          " set status=-2 "
                          " where id=" + QString::number(id_flk) + " ; ";

        QSqlQuery *query_upd = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_upd, "Спрячем строку протокола", *query_upd, true, db, data_app)) {
            delete query_upd;
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка",
                                  "При попытке спрятать строку протокола произошла неожиданная ошибка.\n\n"
                                  "Сохранение отменено.");
            return;
        }
        delete query_upd;
        db.commit();

        refresh_flk_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void protocols_wnd::on_tab_flk_clicked(const QModelIndex &index) {
    if (!db.isOpen()) {
        QMessageBox::warning(this, "Нет доступа к базе данных",
                             "Нет открытого соединения к базе данных?\n"
                             " Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n"
                             " Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику.");
        return;
    }
    if (!ui->tab_flk || !ui->tab_flk->selectionModel()) {
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_flk->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        QModelIndex index = indexes.front();

        QString comment = model_flk.data(model_flk.index(index.row(), 11), Qt::EditRole).toString();
        ui->lab_comment_flk->setText(comment);

    } else {
        ui->lab_comment_flk->setText("-//-");
    }
}

void protocols_wnd::on_bn_upsize_clicked() {
    ui->split_protocols->hide();
    ui->bn_upsize->setVisible(false);
    ui->bn_dnsize->setVisible(true);
}

void protocols_wnd::on_bn_dnsize_clicked() {
    ui->split_protocols->show();
    ui->bn_upsize->setVisible(true);
    ui->bn_dnsize->setVisible(false);
}

void protocols_wnd::on_combo_file_currentIndexChanged(int index) {
    ui->bn_save_Pfile->setEnabled(true/*index>09*/);
}

#include "qt_windows.h"
#include "qwindowdefs_win.h"
#include <shellapi.h>

void protocols_wnd::on_bn_save_Pfile_clicked() {
    int id_prc = ui->combo_file->currentData().toInt();
    QString name_prc = ui->combo_file->currentText();
    name_prc = name_prc.right(name_prc.length() - name_prc.indexOf("p"));
    QString sql_sel = "select bin from files_in_p "
                      " where id=" + QString::number(id_prc) + " ; ";

    QSqlQuery *query_sel = new QSqlQuery(db);
    if (!mySQL.exec(this, sql_sel, "Получим файл протокола", *query_sel, true, db, data_app)) {
        QMessageBox::critical(this, "Непредвиденная ошибка",
                              "При попытке получить файл протокола произошла неожиданная ошибка.\n\n"
                              "Действие отменено.");
        delete query_sel;
        return;
    }
    if (query_sel->next()) {
        QString str = QString::fromLocal8Bit(query_sel->value(0).toByteArray());
        QString fname_res = data_app.path_temp + name_prc + ".xml";
        //mySQL.myUnBYTEA(BYTEA_str, fname_res);
        QFile f(fname_res);

        f.remove();
        if (!f.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this,
                                 "Ошибка создания файла",
                                 "При создании целевого файла произошла ошибка.\n\n"
                                 "Операция отменена.");
            return;
        }
        f.write(str.toLocal8Bit());
        f.close();
        long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);
    } else {
        QMessageBox::warning(this, "Ничего не получено", "При попытке получить данныые файла протоколов ничего не получено.");
    }
    delete query_sel;
}

void protocols_wnd::on_combo_file_activated(const QString &arg1) {
    refresh_protocols_tab();  }
void protocols_wnd::on_ch_req_polis_no_clicked(bool checked) {
    refresh_protocols_tab();  }
void protocols_wnd::on_ch_req_polis_ok_clicked(bool checked) {
    refresh_protocols_tab();  }
void protocols_wnd::on_ch_req_errors_clicked(bool checked) {
    refresh_protocols_tab();  }
void protocols_wnd::on_ch_req_ready_clicked(bool checked) {
    refresh_protocols_tab();  }
void protocols_wnd::on_ch_req_resended_clicked(bool checked) {
    refresh_protocols_tab();  }
void protocols_wnd::on_ch_req_complete_clicked(bool checked) {
    refresh_protocols_tab();  }
void protocols_wnd::on_ch_req_close_byhand_clicked(bool checked) {
    refresh_protocols_tab();  }



void protocols_wnd::on_protocols_wnd_finished(int result) {
    /*
    ui->te_log->append("\n\n ДАННЫЕ ЗАГРУЖЕНЫ "
                       "\n Идёт обновление статуса застрахованных с учётом новых данных. ");
    QApplication::processEvents();

    // обновим статусы персоны
    QString sql_status = "select * from update_pers_links(NULL) ; ";

    QSqlQuery *query_status = new QSqlQuery(db);
    bool res_status = mySQL.exec(this, sql_status, QString("Обновление статусов всех персон"), *query_status, true, db, data_app);
    if (res_status) {
        delete query_status;
    } else {
        delete query_status;
        ui->te_log->append("        !!! ПРИ ПОПЫТКЕ ОБНОВИТЬ СТАТУСЫ ВСЕХ ПЕРСОН ПРОИЗОШЛА ОШИБКА !!!  ");
    }
    */
}

void protocols_wnd::on_rb_proto_all_clicked(bool checked) {
    ui->date_file->setEnabled(!checked);
    ui->bn_today->setEnabled(!checked);
    ui->combo_file->setEnabled(!checked);
    ui->bn_save_Pfile->setEnabled(true/*!checked*/);
    refresh_protocols_tab();
}
void protocols_wnd::on_rb_proto_day_clicked(bool checked) {
    ui->date_file->setEnabled(checked);
    ui->bn_today->setEnabled(checked);
    ui->combo_file->setEnabled(!checked);
    ui->bn_save_Pfile->setEnabled(true/*!checked*/);
    refresh_protocols_tab();
}
void protocols_wnd::on_rb_proto_file_clicked(bool checked) {
    ui->date_file->setEnabled(!checked);
    ui->bn_today->setEnabled(!checked);
    ui->combo_file->setEnabled(checked);
    ui->bn_save_Pfile->setEnabled(true/*checked*/);
    refresh_protocols_tab();
}
void protocols_wnd::on_date_file_dateChanged(const QDate &date) {
    refresh_protocols_tab();
}
void protocols_wnd::on_bn_today_clicked() {
    ui->date_file->setDate(QDate::currentDate());
    refresh_protocols_tab();
}

void protocols_wnd::on_bn_save_Ifile_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, "Нет доступа к базе данных!",
                             "Нет открытого соединения к базе данных?\n "
                             "Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n "
                             "Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику.");
        return;
    }
    if (!ui->tab_protocols || !ui->tab_protocols->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Сначала выберите строку протокола, для которой хотите увидеть данные, направленные в ТФОМС.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes_proto = ui->tab_protocols->selectionModel()->selection().indexes();

    if (indexes_proto.size()>0) {
        QModelIndex index_proto = indexes_proto.front();

        // данные застрахованного
        if (model_protocols.data(model_protocols.index(index_proto.row(), 22), Qt::EditRole).isNull()) {
            QMessageBox::warning(this, "Нет данных",
                                 "Данной строке не сопоставлена ни одна строка данных посылки в ТФОМС.");
        } else {
            int id_proto = model_protocols.data(model_protocols.index(index_proto.row(), 22), Qt::EditRole).toInt();
            QString sql_sel = "select file_name "
                              "  from files_out_i i "
                              " where i.id=" + QString::number(id_proto) + " ; ";

            QSqlQuery *query_sel = new QSqlQuery(db);
            if (!mySQL.exec(this, sql_sel, "Чтение текста посылки", *query_sel, true, db, data_app))
                QMessageBox::critical(this, "Непредвиденная ошибка",
                                      "При попытке прочитать текст посылки произошла непредвиденная ошибка.\n\n"
                                      "Операция отменена.");

            if (query_sel->next()) {
                QString fname = query_sel->value(0).toString();
                QByteArray arr = query_sel->value(1).toByteArray();
                QString fname_res = data_app.path_temp + fname + ".zip";
                QFile f(fname_res);

                f.remove();
                if (!f.open(QIODevice::WriteOnly)) {
                    delete query_sel;
                    QMessageBox::warning(this,
                                         "Ошибка сохранения на диск выгружаемого файла архива",
                                         "При сохранения на диск выгружаемого файла архива произошла ошибка.\n\n"
                                         "Операция отменена.");
                    return;
                }
                f.write(arr);
                f.close();
                long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);
            }
            delete query_sel;
        }
    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано.");
    }
}
