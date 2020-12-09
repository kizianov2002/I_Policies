#include "add_blank_scan_wnd.h"
#include "ui_add_blank_scan_wnd.h"

add_blank_scan_wnd::add_blank_scan_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent)
    : db(db), data_app(data_app), settings(settings), QDialog(parent), ui(new Ui::add_blank_scan_wnd)
{
    ui->setupUi(this);

    add_blanks_box_w = NULL;
    add_blank_w = NULL;

    refresh_blanks_boxes();
    refresh_blanks_tab();
    refresh_comports();
}

add_blank_scan_wnd::~add_blank_scan_wnd() {
    delete ui;
}

void add_blank_scan_wnd::on_combo_comport_activated(const QString &arg1) {
    data_app.barcodes_portname = ui->combo_comport->currentData().toString();
}

void add_blank_scan_wnd::on_combo_boxes_currentIndexChanged(const QString &arg1) {
    refresh_blanks_tab();
}


void add_blank_scan_wnd::refresh_blanks_boxes() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка загруженных коробов
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select ba.id, ba.box_num /*||'  ('||count(*)||')'*/ "
                  "  from blanks_boxes_acts ba "
                  "  left join blanks_boxes bb on(ba.box_num=bb.box_num) "
                  " group by ba.id, ba.box_num, bb.box_num "
                  " order by ba.box_num ; ";
    mySQL.exec(this, sql, QString("короба бланков"), *query, true, db, data_app);
    ui->combo_boxes->clear();
    while (query->next()) {
        int id = query->value(0).toInt();
        int box_num = query->value(1).toInt();
        ui->combo_boxes->addItem("короб № " + QString::number(box_num), box_num);
    }
    ui->combo_boxes->setCurrentIndex(ui->combo_boxes->count()-1);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void add_blank_scan_wnd::refresh_blanks_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    // данные персоны

    QString blanks_sql =
            "select id, enp, pol_ser, pol_num, date_order, date_product, date_add, date_scan "
            " from blanks_boxes "
            " where box_num=" + QString::number(ui->combo_boxes->currentData().toInt()) + " "
            " order by pol_ser, pol_num ; ";

    model_blanks.setQuery(blanks_sql,db);
    QString err = model_blanks.lastError().driverText();

    // подключаем модель из БД
    ui->blanks_tab->setModel(&model_blanks);

    // обновляем таблицу
    ui->blanks_tab->reset();

    // задаём ширину колонок
    ui->blanks_tab->setColumnWidth( 0,  1);    // id
    ui->blanks_tab->setColumnWidth( 1,130);    // enp
    ui->blanks_tab->setColumnWidth( 2, 45);    // pol_ser
    ui->blanks_tab->setColumnWidth( 3, 75);    // pol_num
    ui->blanks_tab->setColumnWidth( 4, 85);    // date_order
    ui->blanks_tab->setColumnWidth( 5, 85);    // date_product
    ui->blanks_tab->setColumnWidth( 6, 85);    // date_add
    ui->blanks_tab->setColumnWidth( 7, 85);    // date_scan

    // правим заголовки
    model_blanks.setHeaderData( 1, Qt::Horizontal, ("ЕНП"));
    model_blanks.setHeaderData( 2, Qt::Horizontal, ("сер."));
    model_blanks.setHeaderData( 3, Qt::Horizontal, ("номер"));
    model_blanks.setHeaderData( 4, Qt::Horizontal, ("дата \nзаказа"));
    model_blanks.setHeaderData( 5, Qt::Horizontal, ("дата \nизготовления"));
    model_blanks.setHeaderData( 6, Qt::Horizontal, ("дата \nдобавления"));
    model_blanks.setHeaderData( 7, Qt::Horizontal, ("дата \nсканирования"));
    ui->blanks_tab->repaint();

    this->setCursor(Qt::ArrowCursor);
}

void add_blank_scan_wnd::on_combo_boxes_currentIndexChanged(int index) {
    refresh_blanks_tab();
}

#include "COM/qextserialenumerator.h"
void add_blank_scan_wnd::refresh_comports() {

    ui->combo_comport->clear();
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
    qDebug() << "List of ports:";
    foreach (QextPortInfo info, ports) {
        qDebug() << "port name:"       << info.portName;
        qDebug() << "friendly name:"   << info.friendName;
        qDebug() << "physical name:"   << info.physName;
        qDebug() << "enumerator name:" << info.enumName;
        qDebug() << "vendor ID:"       << info.vendorID;
        qDebug() << "product ID:"      << info.productID;

        qDebug() << "===================================";

        ui->combo_comport->addItem(info.friendName, info.portName);
    }

    ui->combo_comport->setCurrentIndex(ui->combo_comport->findData(data_app.barcodes_portname));
}

#include "qt_windows.h"
#include "qwindowdefs_win.h"
#include <shellapi.h>

QString nums_str = "0123456789";
QString numsh_str = "0123456789abcdefghABCDEFGH";
QString abcd_str = "_-'абвгдеёжзийклмнопрстуфхцчшщъыьэюяАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯabcehkmoptxyABCEHKMOPTXY";

void add_blank_scan_wnd::on_bn_process_scaner_clicked() {
    // запомним порт, с которого проводится чтение
    settings.beginGroup("barcode");
    settings.setValue("portname", data_app.barcodes_portname);
    data_app.barcodes_file = ui->line_file->text();
    settings.setValue("transport_file", data_app.barcodes_file);
    settings.endGroup();
    settings.sync();

    // прочитаем данные из обменного файла
    QFile f(data_app.barcodes_file);
    if (!f.open(QFile::ReadOnly)) {
        QMessageBox::warning(this,
                             "Обменный файл не найден",
                             "Файл с данными сканирования штрих-кодов не найден.\n"
                             "Может быть ничего небыло отсканировано?\n\n"
                             "Поскольку данные не получены, опреация была отменена.");
        return;
    }

    //int id_blanks_pol_act = ui->combo_blanks_pol_acts->currentData().toInt();

    do {
        s_data_new_blank data_new_blank;

        // прочитаем данные из обменного файла
        char buf[1024];
        for (int i=0; i<1024; i++)
            buf[i] = 0;
        int line_size = f.readLine(buf,1024);
        QString s = QString::fromLocal8Bit(buf);

        int pos = 0, pos1;

        pos1 = s.indexOf("\t", pos);
        data_new_blank.bc_type = s.mid(pos, pos1-pos).simplified().trimmed().toInt();
        pos = pos1+1;

        data_new_blank.pol_num = "";
        data_new_blank.pol_ser = "";

        pos1 = s.indexOf("\t", pos);
        data_new_blank.enp = s.mid(pos, pos1-pos).simplified().trimmed();
        pos = pos1+1;

        pos1 = s.indexOf("\t", pos);
        data_new_blank.fam = s.mid(pos, pos1-pos).simplified().trimmed();
        pos = pos1+1;

        pos1 = s.indexOf("\t", pos);
        data_new_blank.im = s.mid(pos, pos1-pos).simplified().trimmed();
        pos = pos1+1;

        pos1 = s.indexOf("\t", pos);
        data_new_blank.ot = s.mid(pos, pos1-pos).simplified().trimmed();
        pos = pos1+1;

        pos1 = s.indexOf("\t", pos);
        data_new_blank.sex = s.mid(pos, pos1-pos).simplified().trimmed().toInt();
        pos = pos1+1;

        pos1 = s.indexOf("\t", pos);
        data_new_blank.date_birth = QDate::fromString(s.mid(pos, pos1-pos).simplified().trimmed(), "yyyy-MM-dd");
        pos = pos1+1;

        pos1 = s.indexOf("\t", pos);
        data_new_blank.date_exp = QDate::fromString(s.mid(pos, pos1-pos).simplified().trimmed(), "yyyy-MM-dd");
        pos = pos1+1;

        pos1 = s.indexOf("\t", pos);
        data_new_blank.smo_ogrn = s.mid(pos, pos1-pos).simplified().trimmed();
        pos = pos1+1;

        pos1 = s.indexOf("\t", pos);
        data_new_blank.ocato = s.mid(pos, pos1-pos).simplified().trimmed();
        pos = pos1+1;

        pos1 = s.indexOf("\t", pos);
        data_new_blank.eds = s.mid(pos, pos1-pos).simplified().trimmed();
        pos = pos1+1;

        // ------------------------------- //
        // проверка данных на корректность //
        // ------------------------------- //
        bool is_ok = true;
        QString error_msg = "";

        // bc_type;
        error_msg += "Тип штрих-кода:  " + QString::number(data_new_blank.bc_type) + "\n";
        if ( data_new_blank.bc_type<1
             || data_new_blank.bc_type>4 ) {
            is_ok = false;
            error_msg += "                          -- недопустимое значение  \n";
        }
        // enp;
        error_msg += "ЕНП:  " + data_new_blank.enp + "\n";
        for (int i=0; i<data_new_blank.enp.length(); i++) {
            QString cs = data_new_blank.enp.at(i);
            if ( nums_str.indexOf(cs)<0 ) {
                is_ok = false;
                error_msg += "                          -- недопустимое значение  \n";
                break;
            }
        }
        // fam,
        error_msg +="Фамилия:  " +  data_new_blank.fam + "\n";
        for (int i=0; i<data_new_blank.fam.length(); i++) {
            QString cs = data_new_blank.fam.at(i);
            if ( abcd_str.indexOf(cs)<0 ) {
                is_ok = false;
                error_msg += "                          -- недопустимое значение  \n";
                break;
            }
        }
        // im,
        error_msg += "Имя:  " + data_new_blank.im + "\n";
        for (int i=0; i<data_new_blank.im.length(); i++) {
            QString cs = data_new_blank.im.at(i);
            if ( abcd_str.indexOf(cs)<0 ) {
                is_ok = false;
                error_msg += "                          -- недопустимое значение  \n";
                break;
            }
        }
        // ot;
        error_msg += "Отчество:  " + data_new_blank.ot + "\n";
        for (int i=0; i<data_new_blank.ot.length(); i++) {
            QString cs = data_new_blank.ot.at(i);
            if ( abcd_str.indexOf(cs)<0 ) {
                is_ok = false;
                error_msg += "                          -- недопустимое значение  \n";
                break;
            }
        }
        // sex;
        error_msg += "Пол:  " + QString::number(data_new_blank.sex) + "\n";
        if ( data_new_blank.sex<1
             || data_new_blank.sex>2 ) {
            is_ok = false;
            error_msg += "                          -- недопустимое значение  \n";
        }
        /*// date_birth;
        error_msg += "Дата рождения:  " + data_new_blank.date_birth.toString("dd.MM.yyyy") + "\n";
        if ( data_new_blank.date_birth<QDate(1920,1,1)
             || data_new_blank.date_birth>QDate::currentDate() ) {
            is_ok = false;
            error_msg += "                          -- недопустимое значение  \n";
        }
        // date_exp;
        if ( data_new_blank.date_exp!=QDate(1900,1,1) ) {
            error_msg += "Срок действия полиса:  " + data_new_blank.date_exp.toString("dd.MM.yyyy") + "\n";
            if ( data_new_blank.date_exp<=QDate::currentDate() ) {
                is_ok = false;
                error_msg += "                          -- недопустимое значение  \n";
            }
        }
        // smo_ogrn;
        error_msg += "ОГРН СМО:  " + data_new_blank.smo_ogrn + "\n";
        for (int i=0; i<data_new_blank.smo_ogrn.length(); i++) {
            QString cs = data_new_blank.smo_ogrn.at(i);
            if ( nums_str.indexOf(cs)<0 ) {
                is_ok = false;
                error_msg += "                          -- недопустимое значение  \n";
                break;
            }
        }
        // ocato;
        error_msg += "ОКАТО региона:  " + data_new_blank.ocato + "\n";
        for (int i=0; i<data_new_blank.ocato.length(); i++) {
            QString cs = data_new_blank.ocato.at(i);
            if ( nums_str.indexOf(cs)<0 ) {
                is_ok = false;
                error_msg += "                          -- недопустимое значение  \n";
                break;
            }
        }
        // eds;
        error_msg += "Цифровая подпись:  " + data_new_blank.eds + "\n";
        for (int i=0; i<data_new_blank.eds.length(); i++) {
            QString cs = data_new_blank.eds.at(i);
            if ( numsh_str.indexOf(cs)<0 ) {
                is_ok = false;
                error_msg += "                          -- недопустимое значение  \n";
                break;
            }
        }*/

        // f_polis;
        // pol_ser,
        // pol_num;
        // date_scan_enp;

        if (!is_ok) {
            QMessageBox::warning (this, "Данные отсканированы с ошибкой - пересканируйте этот бланк !!!",
                                  "Данные отсканированы с ошибкой - пересканируйте этот бланк !!!\n\n"
                                  + error_msg);
        } else {
            // проверим, сканировался ли уже этот бланк
            QSqlQuery *query_blank0 = new QSqlQuery(db);
            QString sql_blank0 = "select id, pol_ser, pol_num "
                                 " from blanks_boxes s "
                                 " where enp='" + data_new_blank.enp + "'"
                                 "   and date_scan is not NULL "
                                 "   and box_num in (select box_num from blanks_boxes where id=(select max(id) as id_box from blanks_boxes  where enp='" + data_new_blank.enp + "') ) ; ";
            if ( mySQL.exec(this, sql_blank0, QString("проверим, сканировался ли уже этот бланк"), *query_blank0, true, db, data_app) &&
                 query_blank0->next() &&
                 !query_blank0->value(0).isNull() ) {
                QMessageBox::warning (this, "Этот бланк уже отсканирован",
                                      "ЕНП " + data_new_blank.enp + "\n\n" +
                                      data_new_blank.fam + " " + data_new_blank.im + " " + data_new_blank.ot + ", д.р. " + data_new_blank.date_birth.toString("dd.MM.yyyy") + "\n\n"
                                      "полис " + query_blank0->value(1).toString() + " " + QString::number(query_blank0->value(2).toInt()) + (data_new_blank.date_exp.isValid() ? QString(", срок действия " + data_new_blank.date_exp.toString("dd.MM.yyyy") ) : "") + "\n\n"
                                      "Этот бланк уже отсканирован.\n");
            } else {

                // поищем серию и номер бланка в таблице коробов бланков
                QSqlQuery *query_blank = new QSqlQuery(db);
                QString sql_blank = "select id, pol_ser, pol_num "
                                    "  from blanks_boxes s "
                                    " where enp='" + data_new_blank.enp + "'"
                                    "   and date_scan is NULL "
                                    "   and box_num in (select max(box_num) as box_num from blanks_boxes  where enp='" + data_new_blank.enp + "') "
                                    " order by pol_ser desc, pol_num desc ; ";
                data_new_blank.id_blanks_boxes = -1;
                data_new_blank.pol_ser = "";
                data_new_blank.pol_num = "";

                if ( mySQL.exec(this, sql_blank, QString("поищем серию и номер бланка в таблице коробов бланков"), *query_blank, true, db, data_app)
                     && query_blank->next() ) {
                    int id = query_blank->value(0).toInt();
                    QString pol_ser = query_blank->value(1).toString();
                    int pol_num = query_blank->value(2).toInt();

                    data_new_blank.id_blanks_boxes = id;
                    data_new_blank.pol_ser = pol_ser;
                    data_new_blank.pol_num = QString::number(pol_num);
                }

                // откроем окно добавления бланка
                delete add_blank_w;
                add_blank_w = new add_blank_wnd(db, data_app, data_new_blank, settings, this);
                add_blank_w->exec();

                // Запишем дату сканирования бланка
                QSqlQuery *query_scan = new QSqlQuery(db);
                QString sql_scan = "update blanks_boxes "
                                   "   set date_scan='" + QDate::currentDate().toString("yyyy-MM-dd") + "' "
                                   " where id = " + QString::number(data_new_blank.id_blanks_boxes) + " ; "

                                   "update blanks_pol "
                                   "   set id_blanks_box_act = (select id from blanks_boxes_acts where box_num=" + QString::number(ui->combo_boxes->currentData().toInt()) + "), "
                                   "       id_blanks_pol_act = NULL, "
                                   "       date_spent = NULL, "
                                   "       date_sms = NULL, "
                                   "       status=0,"
                                   "       date_scan_enp = coalesce(date_scan_enp, CURRENT_DATE) "
                                   " where pol_ser = '" + data_new_blank.pol_ser + "' "
                                   "   and pol_num = '" + QString("0000000" + data_new_blank.pol_num).right(7) + "' "
                                   "   and ((id_blanks_pol_act is NULL) or (id_blanks_pol_act<=0)) ; ";

                if (!mySQL.exec(this, sql_scan, QString("Запишем дату сканирования бланка"), *query_scan, true, db, data_app)) {
                    QMessageBox::critical(this, "Ошибка записи даты сканирования бланка",
                                          "При попытке записать дату сканирования бланка произошла непредвиденная ошибка.\n"
                                          "Операция отменена.\n");
                    continue;
                }
            }
        }
    } while (!f.atEnd());

    f.close();

    if (QMessageBox::question(this, "Добавление бланков завершилось",
                              "Все отсканированные бланки были обработаны либо добавлены в базу данных либо пропущены.\n\n"
                              "Очистить файл обмена?\n",
                              QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes) {
        QFile f(data_app.barcodes_file);
        bool res_rem = f.remove();
        if (!res_rem) {
            QMessageBox::warning(this, "Ошибка удаления обменного файла",
                                 "При удалении обменного файла произошла неожиданная ошибка\n\n"
                                 "Внимание, содержащиеся в этом файле данные будут повторно обрабатываться при следующем сканировании бланков полисов.\n"
                                 "Это не приведёт к ошибках в данных, поскольку такие записи будут отсеяты в связи с повторением серии/номера бланка, но это затруднит операторскую работу.\n\n"
                                 "Рекомендуется удалить или очистить обменный файл вручную\n" + data_app.barcodes_file);
        }
    }
    on_bn_refresh_clicked();
}


void add_blank_scan_wnd::on_bn_start_scaner_clicked() {
    if (ui->combo_comport->currentText().isEmpty()) {
        QMessageBox::warning(this, "Выберите СОМ-порт сканера",
                             "Выберите СОМ-порт, к которому подключён сканер штрих-кодов!\n\n"
                             "Запуск утилиты сканирования отменён.");
        return;
    }
    if (ui->line_file->text().isEmpty()) {
        QMessageBox::warning(this, "Не найден обменный файл",
                             "Не задан обменный файл для передачи данных сканирования в основную базу данных!\n"
                             "Задайте полное имя существующего ТХТ-файла.\n\n"
                             "Запуск утилиты сканирования отменён.");
        return;
    }
    QFile f(ui->line_file->text());
    if (f.open(QIODevice::Append)) {
        f.write("");
        f.close();
    }
    if (!QFile(ui->line_file->text()).exists()) {
        QMessageBox::warning(this, "Не найден обменный файл",
                             "Не найден обменный файл для передачи данных сканирования в основную базу данных!\n\n"
                             "Запуск утилиты сканирования отменён.");
        return;
    }

    // запомним порт, с которого проводится чтение
    settings.beginGroup("barcode");
    settings.setValue("portname", data_app.barcodes_portname);
    data_app.barcodes_file = ui->line_file->text();
    settings.setValue("transport_file", data_app.barcodes_file);
    settings.endGroup();
    settings.sync();

    // поправим батник для запуска программы перехвата данных со сканера
    QString bat_s = data_app.path_install + "test_barc.bat";
    QFile file(bat_s);
    file.open(QIODevice::WriteOnly);
    QString comm_s = "\"" + data_app.path_install + "_INKO_POLISES/_INKO_BARCODER.exe\" " + ui->combo_comport->currentData().toString() + " \"" + ui->line_file->text() + "\"";
    file.write(comm_s.replace("/", "\\").toLocal8Bit());
    file.close();

    // запустим программу для перехвата данных со сканера
    long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(bat_s.replace("/","\\").utf16()), 0, 0, SW_NORMAL);

    if ( result!=0 &&
         result!=42 ) {
        // работа сканировщика завершилась с ошибкой  - сообщим об этом`
        QMessageBox::warning(this,
                             "Ошибка запуска модуля сканирования",
                             "При запуске модуля сканирования штрих-кодов произошла ошибка.\n\n"
                             "Команда \n" + bat_s + "\n\n"
                             "Код ошибки - " + QString::number(result) +
                             "\n\nОпреация прервана.");
    }
}

void add_blank_scan_wnd::on_bn_cansel_clicked() {
    accept();
}

void add_blank_scan_wnd::on_bn_add_box_clicked() {
    // откроем окно сканера штрихкодов
    delete add_blanks_box_w;
    add_blanks_box_w = new add_blanks_box_wnd(db, data_app, settings, this);
    add_blanks_box_w->exec();

    refresh_blanks_boxes();
    refresh_blanks_tab();
    //refresh_blanks_pol_acts();
}

void add_blank_scan_wnd::on_bn_refresh_clicked() {
    refresh_blanks_tab();
}

void add_blank_scan_wnd::on_ch_today_clicked() {
    //refresh_blanks_pol_acts();
}
