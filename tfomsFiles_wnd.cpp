#include "tfomsFiles_wnd.h"
#include "ui_tfomsFiles_wnd.h"
#include <QMessageBox>
#include <QDesktopWidget>
#include <QFileDialog>

tfomsFiles_wnd::tfomsFiles_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, persons_wnd *persons_w, QWidget *parent) :
    db(db),
    data_app(data_app),
    settings(settings),
    persons_w(persons_w),
    QDialog(parent),
    ui(new Ui::tfomsFiles_wnd)
{
    ui->setupUi(this);

    ui->split_tfomsFiles_log->setStretchFactor(0, 3);
    ui->split_tfomsFiles_log->setStretchFactor(1, 2);

    QRect screen = QApplication::desktop()->screenGeometry();
    this->resize(1200, 1000);
    this->move(screen.width()/2-625, screen.height()/2-500 - 20);

    refresh_tfomsFiles_tab();
}

tfomsFiles_wnd::~tfomsFiles_wnd() {
    delete ui;
}

void tfomsFiles_wnd::on_bn_close_clicked() {
    //if (QMessageBox::question(this, "Закрыть окно протоколов?", "Закрыть окно протоколов?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes) {
        close();
    //}
}

void tfomsFiles_wnd::on_bn_refresh_clicked() {
    refresh_tfomsFiles_tab();
}

void tfomsFiles_wnd::refresh_tfomsFiles_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, "Нет доступа к базе данных",
                             "Нет открытого соединения к базе данных?\n "
                             "Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n "
                             "Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику.");
        return;
    }

    QString tfomsFiles_sql;
    tfomsFiles_sql += "select f.id, f.file_name, f.date_get, count(*) as cnt "
                    "  from files_in_tfoms f "
                    "       left join frecs_in_tfoms r on(r.id_file_tfoms=f.id) "
                    "       left join polises p on(p.id=r.id_polis) "
                    " group by f.id, f.file_name, f.date_get "
                    " order by f.file_name ; ";

    model_tfomsFiles.setQuery(tfomsFiles_sql,db);
    QString err2 = model_tfomsFiles.lastError().driverText();

    // подключаем модель из БД
    ui->tab_tfomsFiles->setModel(&model_tfomsFiles);

    // обновляем таблицу
    ui->tab_tfomsFiles->reset();

    // задаём ширину колонок
    ui->tab_tfomsFiles->setColumnWidth( 0, 50);     // r.id
    ui->tab_tfomsFiles->setColumnWidth( 1,200);     // r.file_name
    ui->tab_tfomsFiles->setColumnWidth( 2,120);     // r.date_get
    ui->tab_tfomsFiles->setColumnWidth( 3,120);     // cnt

    // правим заголовки
    model_tfomsFiles.setHeaderData( 0, Qt::Horizontal, ("ID"));
    model_tfomsFiles.setHeaderData( 1, Qt::Horizontal, ("имя файла"));
    model_tfomsFiles.setHeaderData( 2, Qt::Horizontal, ("дата получения"));
    model_tfomsFiles.setHeaderData( 3, Qt::Horizontal, ("число полисов"));
    ui->tab_tfomsFiles->repaint();

    this->setCursor(Qt::ArrowCursor);
}

void tfomsFiles_wnd::on_bn_load_new_tfomsFiles_clicked() {
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

    // запросим tfoms-файлы для загрузки
    QStringList tfomsFiles = QFileDialog::getOpenFileNames(this, "Выберите tfoms-файлы для загрузки",
                                                          data_app.path_in, tr("файлы уведомления от ТФОМС (*tfoms.dbf)"));
    if (tfomsFiles.count()==0) {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано.");

        // закроем соединение с ODBC
        db_ODBC.close();
        QSqlDatabase::removeDatabase("INKO_DBF");

        return;
    }

    db.transaction();

    ui->te_log->append("\nЗагрузка " + QString::number(tfomsFiles.count()) + " файлов-заявок от ТФОМС:");
    QApplication::processEvents();

    foreach (QString tfomsFile, tfomsFiles) {
        // разместим файл в папке источника данных dbf
        QFileInfo fi(tfomsFile);
        QString fname = fi.fileName();
        QString bname = fi.baseName();
        QString new_fname = fname;
        new_fname = new_fname.replace(" ", "").replace("\t", "").replace("и", "_");
        QString new_bname = bname;
        new_bname = new_bname.replace(" ", "").replace("\t", "").replace("и", "_");
        QString new_tfomsFile = data_app.path_dbf + new_fname;

        ui->te_log->append(" " + fname);
        QApplication::processEvents();

        tfomsFile = tfomsFile.replace("/", "\\");
        new_tfomsFile = new_tfomsFile.replace("/", "\\");
        if (new_tfomsFile!=tfomsFile) {
            QFile::remove(new_tfomsFile);
            QFile::copy(tfomsFile, new_tfomsFile);
        }

        // проверим, нет ли в базе такого же файла tfoms
        QString sql_test  = "select count(*) from files_in_tfoms "
                            " where file_name='" + fname + "' ; ";
        QSqlQuery *query_test = new QSqlQuery(db);
        mySQL.exec(this, sql_test, "Проверка отсутствия в базе готового файла tfoms", *query_test, true, db, data_app);
        if (!query_test->next()) {
            QMessageBox::warning(this, "Ошибка чтения данных",
                                 "При попытке проверить наличие в базе данных готовой записи файла tfoms произошла неожиданная ошибка.\n\n"
                                 "Загрузка файлов отменена.");
            // закроем соединение с ODBC
            db_ODBC.close();
            QSqlDatabase::removeDatabase("INKO_DBF");

            ui->te_log->append("!!! Ошибка чтения базы данных - операция отменена !!!");
            QApplication::processEvents();

            db.rollback();
            return;
        }
        int cnt = query_test->value(0).toInt();
        if (cnt>0) {
            ui->te_log->append("  -  уже есть в базе");
            QApplication::processEvents();
            continue;
        }

        // заранее прочитаем данные из файла - чтобы убедиться что это правильный файл
        QString sql_read = "select dedit, enp, fam, im, ot, dr, w, mr, cn, npol, ss "
                           "  from " + new_bname + " ";
        QSqlQuery *query_read = new QSqlQuery(db_ODBC);
        if (!query_read->exec(sql_read)) {
            delete query_read;

            ui->te_log->append("  -  неверный формат или ошибка доступа к файлу");
            QApplication::processEvents();
            continue;
        }

        // добавление файла заявки в базу
        QString bin;
        mySQL.myBYTEA(tfomsFile, bin);
        QString sql_ins = "insert into files_in_tfoms (file_name, date_get, bin)  "
                          " values ('" + fname + "', CURRENT_DATE, '" + bin + "') "
                          " returning id ; ";
        QSqlQuery *query_ins = new QSqlQuery(db);
        mySQL.exec(this, sql_ins, "Добавление нового файла tfoms", *query_ins, true, db, data_app);
        if (!query_ins->next()) {
            QMessageBox::warning(this, "Ошибка записи данных",
                                 "При попытке записать данные нового файла tfoms произошла неожиданная ошибка.\n\n"
                                 "Загрузка файлов отменена.");
            // закроем соединение с ODBC
            delete query_read;
            db_ODBC.close();
            QSqlDatabase::removeDatabase("INKO_DBF");

            ui->te_log->append("!!! Ошибка записи данных заявки в базу данных - операция отменена !!!");
            QApplication::processEvents();

            delete query_ins;
            db.rollback();
            return;
        }
        int id_file_tfoms = query_ins->value(0).toInt();
        delete query_ins;

        int cnt_read = 0;
        int cnt_add  = 0;
        while (query_read->next()) {
            cnt_read++;

            // выбор данных одной заявки
            QDate   tfoms_date   = query_read->value( 0).toDate();
            QString enp        = query_read->value( 1).toString();
            QString fam        = query_read->value( 2).toString();
            QString im         = query_read->value( 3).toString();
            QString ot         = query_read->value( 4).toString();
            QDate   date_birth = query_read->value( 5).toDate();
            int     sex        = query_read->value( 6).toInt();
            QString plc_birth  = query_read->value( 7).toString();
            QString oksm_c     = query_read->value( 8).toString();
            QString vs_num     = query_read->value( 9).toString();
            QString snils      = query_read->value(10).toString();

            // найдём ВС
            QString sql_VS = "select id "
                             "  from polises p "
                             " where p.vs_num='" + vs_num + "' "
                             "   and p.pol_v=2 ; ";
            QSqlQuery *query_VS = new QSqlQuery(db);
            if (!query_VS->exec(sql_VS)) {
                QMessageBox::warning(this, "Ошибка при поиске ВС",
                                     "При попытке найти ВС по данным из tfoms-файла произошла неожиданная ошибка.\n\n"
                                     "Загрузка файлов отменена.");
                // закроем соединение с ODBC
                delete query_read;
                delete query_VS;
                delete query_ins;

                db_ODBC.close();
                QSqlDatabase::removeDatabase("INKO_DBF");

                ui->te_log->append("!!! Ошибка поиска по базе данных - операция отменена !!!");
                QApplication::processEvents();

                db.rollback();
                return;
            }
            query_VS->next();
            int id_VS = query_VS->value(0).toInt();

            if (!query_VS->value(0).isNull()) {
                cnt_add++;

                // добавим в полис данные заявки
                QString sql_upd = "update polises "
                                  "   set tfoms_date='" + tfoms_date.toString("yyyy-MM-dd") + "', "
                                  "       enp='" + enp + "' "
                                  " where id=" + QString::number(id_VS) + " ; "

                                  "insert into frecs_in_tfoms(id_file_tfoms, id_polis, fam, im, ot, sex, date_birth, plc_birth, enp, vs_num) "
                                  "values (" + QString::number(id_file_tfoms) + ", "
                                           " " + QString::number(id_VS) + ", "
                                           "'" + fam + "', "
                                           "'" + im  + "', "
                                           "'" + ot  + "', "
                                           " " + QString::number(sex) + ", "
                                           "'" + date_birth.toString("yyyy-MM-dd") + "', "
                                           "'" + plc_birth + "', "
                                           "'" + enp + "', "
                                           "'" + vs_num + "' )";
                QSqlQuery *query_upd = new QSqlQuery(db);
                if (!mySQL.exec(this, sql_upd, "Добавление строки файла tfoms", *query_upd, true, db, data_app)) {
                    QMessageBox::warning(this, "Ошибка записи данных",
                                         "При попытке записать строку нового файла tfoms произошла неожиданная ошибка.\n\n"
                                         "Загрузка файлов отменена.");
                    // закроем соединение с ODBC
                    delete query_read;
                    db_ODBC.close();
                    QSqlDatabase::removeDatabase("INKO_DBF");

                    ui->te_log->append("!!! Ошибка записи строки заявки в базу данных - операция отменена !!!");
                    QApplication::processEvents();

                    delete query_upd;
                    db.rollback();
                    return;
                }
            }
        }
        ui->te_log->append("  +  добавлено " + QString::number(cnt_add) + " строк из " + QString::number(cnt_read));
        QApplication::processEvents();
    }

    ui->te_log->append("Готово");
    QApplication::processEvents();

    // закроем соединение с ODBC
    db_ODBC.close();
    QSqlDatabase::removeDatabase("INKO_DBF");

    db.commit();

    refresh_tfomsFiles_tab();
}
