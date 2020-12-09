#include "fiasFiles_wnd.h"
#include "ui_fiasFiles_wnd.h"
#include <QMessageBox>
#include <QDesktopWidget>
#include <QFileDialog>

fiasFiles_wnd::fiasFiles_wnd(QSqlDatabase &db, QSqlDatabase &db_ODBC, s_data_app &data_app, QSettings &settings, persons_wnd *persons_w, QWidget *parent) :
    db(db),
    db_ODBC(db_ODBC),
    data_app(data_app),
    settings(settings),
    persons_w(persons_w),
    QDialog(parent),
    ui(new Ui::fiasFiles_wnd)
{
    ui->setupUi(this);

    ui->ch_change_server->setChecked(false);
    ui->pan_connect->setVisible(false);
    ui->ch_clear->setChecked(false);
    ui->pan_wait->setVisible(false);

    FIAS_db_drvr = data_app.FIAS_db_drvr;
    FIAS_db_host = data_app.FIAS_db_host;
    FIAS_db_name = data_app.FIAS_db_name;
    FIAS_db_port = data_app.FIAS_db_port;
    FIAS_db_admn = data_app.FIAS_db_admn;
    FIAS_db_admp = data_app.FIAS_db_admp;

    ui->combo_db_driv->setCurrentIndex(ui->combo_db_driv->findText(data_app.FIAS_db_drvr));
    ui->line_db_host->setText(data_app.FIAS_db_host);
    ui->line_db_port->setText(data_app.FIAS_db_port);
    ui->line_db_name->setText(data_app.FIAS_db_name);
    ui->line_db_admn->setText(data_app.FIAS_db_admn);
    ui->line_db_admp->setText(data_app.FIAS_db_admp);

    try_connect_FIAS();

    ui->lab_filename-> setText("-//-");
    ui->lab_operation-> setText("-//-");
    ui->lab_cnt-> setText("-//-");
    ui->lab_result-> setText("-//-");
}

void fiasFiles_wnd::on_ch_change_server_clicked(bool checked) {
    ui->pan_connect->setVisible(checked);
}

bool fiasFiles_wnd::try_connect_FIAS() {
    date = QDate::currentDate();
    time = QTime::currentTime();
    ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  ПОДКЛЮЧЕНИЕ К БАЗЕ ФИАС:   driver=" + ui->combo_db_driv->currentText() + ";  host=" + ui->line_db_host->text() + ";  port=" + ui->line_db_port->text() + "  base=" + ui->line_db_name->text() + " ... ");
    QApplication::processEvents();
    // ---------------------------------------------------------- //
    // откроем новое подключение к базе ФИАС с админскими правами //
    // ---------------------------------------------------------- //
    db_FIAS2 = QSqlDatabase::addDatabase(ui->combo_db_driv->currentText(), "FIAS2 Database");

    db_FIAS2.setConnectOptions();
    // сервер базы ФИАС
    if (ui->line_db_host->text()!="polises_host")  db_FIAS2.setHostName(ui->line_db_host->text());
    else  db_FIAS2.setHostName(data_app.host);
    // порт базы ФИАС
    if (!ui->line_db_port->text().isEmpty())  db_FIAS2.setPort(ui->line_db_port->text().toInt());
    // имя базы ФИАС
    if (!ui->line_db_name->text().isEmpty())  db_FIAS2.setDatabaseName(ui->line_db_name->text());
    // имя администратора ФИАС
    if (!ui->line_db_admn->text().isEmpty())  db_FIAS2.setUserName(ui->line_db_admn->text());
    // пароль администратора ФИАС
    if (!ui->line_db_admp->text().isEmpty())  db_FIAS2.setPassword(ui->line_db_admp->text());

    if (!db_FIAS2.open()) {
        // подключение не удалось
        ui->bn_FIAS_load->setEnabled(false);
        ui->bn_FIAS_reindex->setEnabled(false);

        ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  ... !!! ОШИБКА ДОСТУПА !!!.\n-");

        QMessageBox::critical(this,"Ошибка подключения к базе ФИАС",
                              QString("ВНИМАНИЕ!!!\n\n") +
                              "При попытке подключиться к база ФИАС произошла ошибка.\n"
                              "Позвоните мне по телефону \n\n"
                              "8-920-449-54-88\n\n"
                              "Александр\n\n" +
                              "FIAS_db_drvr = " + ui->combo_db_driv->currentText() + "\n" +
                              "FIAS_db_host = " + ui->line_db_host->text() + "\n" +
                              "FIAS_db_port = " + ui->line_db_port->text() + "\n" +
                              "FIAS_db_name = " + ui->line_db_name->text() + "\n" +
                              "FIAS_db_admn = " + ui->line_db_admn->text() + "\n" +
                              "FIAS_db_admp = " + ui->line_db_admp->text() + "\n");
    } else {
        ui->bn_FIAS_load->setEnabled(true);
        ui->bn_FIAS_reindex->setEnabled(true);

        FIAS_db_drvr = ui->combo_db_driv->currentText();
        FIAS_db_host = ui->line_db_host->text();
        FIAS_db_name = ui->line_db_name->text();
        FIAS_db_port = ui->line_db_port->text();
        FIAS_db_admn = ui->line_db_admn->text();
        FIAS_db_admp = ui->line_db_admp->text();

        //count_FIAS_data();

        ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  ... готово.\n-");
        QApplication::processEvents();

        QMessageBox::information(this,"База ФИАС доступна",
                                 QString("Доступ к базе ФИАС в административном режиме получен.\n"));
    }

    //------------------------------//
    // отображение статуса на экран //
    //------------------------------//
    date = QDate::currentDate();
    time = QTime::currentTime();

    int sec = time0.secsTo(time) + date0.daysTo(date)*24*60*60;
    int sut = sec/86400;
    sec = sec - sut*86400;
    int hou = sec/3600;
    sec = sec - hou*3600;
    int min = sec/60;
    sec = sec - min*60;
    ui->lab_time_all->setText( (sut>0 ? (QString::number(sut)+"д ") : " ") +
                               QString("00" + QString::number(hou)).right(2) + ":" +
                               QString("00" + QString::number(min)).right(2) + ":" +
                               QString("00" + QString::number(sec)).right(2) );

    sec = time1.secsTo(time) + date1.daysTo(date)*24*60*60;
    sut = sec/86400;
    sec = sec - sut*86400;
    hou = sec/3600;
    sec = sec - hou*3600;
    min = sec/60;
    sec = sec - min*60;
    ui->lab_time->setText( (sut>0 ? (QString::number(sut)+"д ") : " ") +
                           QString("00" + QString::number(hou)).right(2) + ":" +
                           QString("00" + QString::number(min)).right(2) + ":" +
                           QString("00" + QString::number(sec)).right(2) );
    date1 = QDate::currentDate();
    time1 = QTime::currentTime();
    QApplication::processEvents();
    if (db_FIAS2.isOpen()) {
        // подключение готово
        return true;
    } else {
        return false;
    }
}

fiasFiles_wnd::~fiasFiles_wnd() {
    delete ui;
}

void fiasFiles_wnd::on_bn_close_clicked() {
    close();
}

void fiasFiles_wnd::on_push_connect_clicked() {
    if (try_connect_FIAS()) {
        ui->ch_change_server->setChecked(false);
        ui->pan_connect->setVisible(false);
    } else {
        ui->ch_change_server->setChecked(true);
        ui->pan_connect->setVisible(true);
    }
}

void fiasFiles_wnd::on_combo_db_driv_activated(int index) {
    if (ui->combo_db_driv->currentText()=="QIBASE") {
        ui->line_db_host->setText("172.20.250.19");
        ui->line_db_port->setText("");
        ui->line_db_name->setText("FIAS");
        ui->line_db_admn->setText("SYSDBA");
        ui->line_db_admp->setText("masterkey");
    } else if (ui->combo_db_driv->currentText()=="QPSQL") {
        ui->line_db_host->setText("172.20.250.15");
        ui->line_db_port->setText("5432");
        ui->line_db_name->setText("inko_fias");
        ui->line_db_admn->setText("belg");
        ui->line_db_admp->setText("belg");
    }
}

void fiasFiles_wnd::on_bn_FIAS_clear_clicked() {
    if (QMessageBox::question(this, "Удалить все данные ФИАС?",
                              "Вы действительно хотите удалить все данные ФИАС?\n\n"
                              "После этого надо будет загрузить свежую версию базы данных ФИАС, \nа это долгая операция \n"
                              "(лучше отложить на выходные)", QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel)==QMessageBox::Cancel) {
        QMessageBox::information(this, "Операция отменена", "Данные ФИАС остались без изменения.");
        return;
    }
    date = QDate::currentDate();
    time = QTime::currentTime();

    db_FIAS2.transaction();
    ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  Удаление данных ФИАС ...");
    QApplication::processEvents();

    // собственно удаление ФИАС

    QString sql_cl_ao = "truncate fias.addrobj restart idaentity ; ";
    QSqlQuery *query_cl_ao = new QSqlQuery(db_FIAS2);
    if (!query_cl_ao->exec(sql_cl_ao)) {
        db_FIAS2.rollback();
        ui->te_log->append("-");
        ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  ... операция отменена. Данные ФИАС остались без изменения.\n");
        QApplication::processEvents();
        QMessageBox::critical(this, "Непредвиденная ошибка", "При очистке данных ADDROBJ произошла неожиданная ошибка.\n\nОперация отменена.\nДанные ФИАС остались без изменения.");
        delete query_cl_ao;
        return;
    }
    delete query_cl_ao;


    QString sql_cl_hs = "truncate fias.house restart idaentity ; ";
    QSqlQuery *query_cl_hs = new QSqlQuery(db_FIAS2);
    if (!query_cl_hs->exec(sql_cl_hs)) {
        db_FIAS2.rollback();
        ui->te_log->append("-");
        ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  ... операция отменена. Данные ФИАС остались без изменения.\n");
        QApplication::processEvents();
        QMessageBox::critical(this, "Непредвиденная ошибка", "При очистке данных HOUSE произошла неожиданная ошибка.\n\nОперация отменена.\nДанные ФИАС остались без изменения.");
        delete query_cl_hs;
        return;
    }
    delete query_cl_hs;


    QString sql_cl_hi = "truncate fias.houseint restart idaentity ; ";
    QSqlQuery *query_cl_hi = new QSqlQuery(db_FIAS2);
    if (!query_cl_hi->exec(sql_cl_hi)) {
        db_FIAS2.rollback();
        ui->te_log->append("-");
        ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  ... операция отменена. Данные ФИАС остались без изменения.\n");
        QApplication::processEvents();
        QMessageBox::critical(this, "Непредвиденная ошибка", "При очистке данных HOUSEINT произошла неожиданная ошибка.\n\nОперация отменена.\nДанные ФИАС остались без изменения.");
        delete query_cl_hi;
        return;
    }
    delete query_cl_hi;


    QString sql_cl_fl = "truncate fias.load_files restart idaentity ; ";
    QSqlQuery *query_cl_fl = new QSqlQuery(db_FIAS2);
    if (!query_cl_fl->exec(sql_cl_fl)) {
        db_FIAS2.rollback();
        ui->te_log->append("-");
        ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  ... операция отменена. Данные ФИАС остались без изменения.\n");
        QApplication::processEvents();
        QMessageBox::critical(this, "Непредвиденная ошибка", "При очистке данных LOAD_FILES произошла неожиданная ошибка.\n\nОперация отменена.\nДанные ФИАС остались без изменения.");
        delete query_cl_fl;
        return;
    }
    delete query_cl_fl;

    /*
    // удаление индексов
    QString sql_cl_ind;
    if (ui->combo_db_driv->currentText()=="QPSQL") {
        sql_cl_ind= " DROP INDEX fias.FIAS_addrobj_areacode_idx; "
                   " DROP INDEX fias.FIAS_addrobj_citycode_idx; "
                   " DROP INDEX fias.FIAS_addrobj_placecode_idx; "
                   " DROP INDEX fias.FIAS_addrobj_regioncode_idx; "
                   " DROP INDEX fias.FIAS_addrobj_streetcode_idx; "
                   " DROP INDEX fias.spr_FIAS_addrobj_aoguid_idx; "
                   " DROP INDEX fias.spr_FIAS_addrobj_aoid_idx; "
                   " DROP INDEX fias.spr_FIAS_addrobj_code_idx; "
                   " DROP INDEX fias.spr_FIAS_addrobj_subj_dstr_city_nasp_strt_idx; ";
    }

    QSqlQuery *query_cl_ind = new QSqlQuery(db_FIAS2);
    if (!query_cl_ind->exec(sql_cl_ind)) {
        db_FIAS2.rollback();
        ui->te_log->append("-");
        ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  ... операция отменена. Данные ФИАС остались без изменения.\n");
        QApplication::processEvents();
        QMessageBox::critical(this, "Непредвиденная ошибка", "При удалении индексов ФИАС произошла неожиданная ошибка.\n\nОперация отменена.\nДанные ФИАС остались без изменения.");
        delete query_cl_ind;
        return;
    }
    delete query_cl_ind;
    */

    ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  ... данные ФИАС удалены.\n-");
    QApplication::processEvents();
    db_FIAS2.commit();

    //------------------------------//
    // отображение статуса на экран //
    //------------------------------//
    date = QDate::currentDate();
    time = QTime::currentTime();

    int sec = time0.secsTo(time) + date0.daysTo(date)*24*60*60;
    int sut = sec/86400;
    sec = sec - sut*86400;
    int hou = sec/3600;
    sec = sec - hou*3600;
    int min = sec/60;
    sec = sec - min*60;
    ui->lab_time_all->setText( (sut>0 ? (QString::number(sut)+"д ") : " ") +
                               QString("00" + QString::number(hou)).right(2) + ":" +
                               QString("00" + QString::number(min)).right(2) + ":" +
                               QString("00" + QString::number(sec)).right(2) );

    sec = time1.secsTo(time) + date1.daysTo(date)*24*60*60;
    sut = sec/86400;
    sec = sec - sut*86400;
    hou = sec/3600;
    sec = sec - hou*3600;
    min = sec/60;
    sec = sec - min*60;
    ui->lab_time->setText( (sut>0 ? (QString::number(sut)+"д ") : " ") +
                           QString("00" + QString::number(hou)).right(2) + ":" +
                           QString("00" + QString::number(min)).right(2) + ":" +
                           QString("00" + QString::number(sec)).right(2) );
    date1 = QDate::currentDate();
    time1 = QTime::currentTime();
    QApplication::processEvents();
    count_FIAS_data();
}

void fiasFiles_wnd::on_bn_FIAS_vacuum_clicked() {
    vacuum_FIAS_tables();
}

void fiasFiles_wnd::on_bn_FIAS_count_clicked() {
    count_FIAS_data();
}

void fiasFiles_wnd::on_bn_FIAS_reindex_clicked() {
    if (QMessageBox::question(this, "Обновить индексы ФИАС?",
                              "Вы действительно хотите обновить все идексы по данным ФИАС?\n\n"
                              "Время выполнения полной перестройки индексов - от 15 минут до часа.\n"
                              "Всё это время база ФИАС может быть недоступна для использования.\n"
                              "(лучше запускать на ночь)", QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel)==QMessageBox::Cancel) {
        QMessageBox::information(this, "Операция отменена", "Индексы ФИАС остались без изменения.");
        return;
    }
    //vacuum_FIAS_tables();
    reindex_FIAS_tables();
}

void fiasFiles_wnd::on_bn_FIAS_recalc_clicked() {
    count_FIAS_data();
}

void fiasFiles_wnd::on_bn_FIAS_load_clicked() {
    // получим место размещения файлов ФИАС
    QString FIAS_dir_str = QFileDialog::getExistingDirectory(this, "Выберите папку размещения файлов ФИАС");

    QDir FIAS_dir(FIAS_dir_str);
    if (FIAS_dir_str.isEmpty() || !FIAS_dir.exists()) {
        QMessageBox::warning(this, "Папка не выбрана", "Данные ФИАС остались без изменения.");
        return;
    }
    QStringList filters;
    filters << "*.dbf";
    QStringList files_list_str = FIAS_dir.entryList(filters);

    QString FIAS_import_log = data_app.path_out + "FIAS_import_log.txt";

    ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  ПОЛНЫЙ ИМПОРТ ДАННЫХ ФИАС ИЗ *.DBF ...");
    ui->pan_wait->setVisible(true);
    QApplication::processEvents();




    if ( ui->ch_clear->isChecked()) {
        if ( QMessageBox::question(this, "Удалить старые данные?",
                                   "Вы действительно хотите удалить старые данные перед загрузкой?\n\nПолная перегрузка базы ФИАС занимает больше суток!\n",
                                   QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes) {
            // переберём список файлов - удалим старые данные
            ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  удаление старых данных");
            QApplication::processEvents();

            if (ui->combo_db_driv->currentText()=="QIBASE") {
                // закроем соединение с базой ФИАС
                db_FIAS2.close();
                QSqlDatabase::removeDatabase("FIAS2 Database");

                // попробуем заменить выбранную базу ФИАС пустой шаблонной базой
                if (QFile::exists(ui->line_db_name->text())) {
                    if (!QFile::remove(ui->line_db_name->text())) {
                        ui->pan_wait->setVisible(false);
                        QMessageBox::warning(this, "Ошибка удаления старой версии базы",
                                             "При попытке удалить старый экземпляр базы ФИАС произошла ошибка.\nОбновление базы ФИАС не удалось."
                                             "\n\nОперация отменена.");
                        return;
                    }
                }
                if (!QFile::copy(data_app.path_dbf + "INKO_FIAS_SRC.FDB", ui->line_db_name->text())) {
                    ui->pan_wait->setVisible(false);
                    QMessageBox::warning(this, "Ошибка копирования шаблонной базы",
                                         "При попытке скопировать шаблоную базу ФИАС произошла ошибка.\nОбновление базы ФИАС не удалось."
                                         "\n\nОперация отменена.");
                    return;
                }
                if (!try_connect_FIAS()) {
                    ui->pan_wait->setVisible(false);
                    QMessageBox::warning(this, "Ошибка подключения к новой базе ФИИАС",
                                         "При попытке подключиться к новой базе ФИАС произошла ошибка.\nОбновление базы ФИАС не удалось."
                                         "\n\nОперация отменена.");
                    return;
                }

            } else if (ui->combo_db_driv->currentText()=="QPSQL") {
                db_FIAS2.transaction();
                    QString sql_free = "truncate fias.addrobj restart identity ; "
                                       "truncate fias.house restart identity ; "
                                       "truncate fias.houseint restart identity ; "
                                       "truncate fias.load_files restart identity ; ";
                    QSqlQuery *query_free = new QSqlQuery(db_FIAS2);
                    if (!query_free->exec(sql_free)) {
                        db_FIAS2.rollback();
                        QMessageBox::warning(this, "Ошибка при удалении данных ФИАС",
                                             "При попытке удалить старые данные ФИАС произошла ошибка. \n\nСтарые данные не удалены.");
                    }
                    delete query_free;
                db_FIAS2.commit();
            }
        }
    }
    ui->ch_clear->setChecked(false);


    vacuum_FIAS_tables();


    date0 = QDate::currentDate();
    time0 = QTime::currentTime();
    date1 = QDate::currentDate();
    time1 = QTime::currentTime();

    // переберём список файлов
    for (int i=0; i<files_list_str.size(); i++) {
        QString filename_str = files_list_str.at(i);
        bool res = import_FIAS_file(FIAS_dir_str + "/" + filename_str, FIAS_import_log, false);
    }

    //------------------------------//
    // отображение статуса на экран //
    //------------------------------//
    date = QDate::currentDate();
    time = QTime::currentTime();

    int sec = time0.secsTo(time) + date0.daysTo(date)*24*60*60;
    int sut = sec/86400;
    sec = sec - sut*86400;
    int hou = sec/3600;
    sec = sec - hou*3600;
    int min = sec/60;
    sec = sec - min*60;
    ui->lab_time_all->setText( (sut>0 ? (QString::number(sut)+"д ") : " ") +
                               QString("00" + QString::number(hou)).right(2) + ":" +
                               QString("00" + QString::number(min)).right(2) + ":" +
                               QString("00" + QString::number(sec)).right(2) );

    sec = time1.secsTo(time) + date1.daysTo(date)*24*60*60;
    sut = sec/86400;
    sec = sec - sut*86400;
    hou = sec/3600;
    sec = sec - hou*3600;
    min = sec/60;
    sec = sec - min*60;
    ui->lab_time->setText( (sut>0 ? (QString::number(sut)+"д ") : " ") +
                           QString("00" + QString::number(hou)).right(2) + ":" +
                           QString("00" + QString::number(min)).right(2) + ":" +
                           QString("00" + QString::number(sec)).right(2) );
    date1 = QDate::currentDate();
    time1 = QTime::currentTime();
    QApplication::processEvents();


    vacuum_FIAS_tables();
    //reindex_FIAS_tables();


    ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  ... ПОЛНЫЙ ИМПОРТ ДАННЫХ ФИАС ИЗ *.DBF ЗАВЕРШЁН\n-");
    ui->pan_wait->setVisible(false);
    QApplication::processEvents();


    // сохраним журнал импорта данных
    QFile f_log(FIAS_import_log);
    f_log.open(QIODevice::WriteOnly);
    f_log.write(ui->te_log->toPlainText().toLocal8Bit());
    f_log.close();

    // открытие журнала
    long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(FIAS_import_log.utf16()), 0, 0, SW_NORMAL);
}

void fiasFiles_wnd::on_bn_FIAS_load4files_clicked() {
    QStringList files_list_str = QFileDialog::getOpenFileNames(this, "Выберите файлы ФИАС для обновления");
    if (files_list_str.isEmpty()) {
        QMessageBox::warning(this, "Ничего не выбрано", "Действие отменено");
        return;
    }

    QString FIAS_import_log = data_app.path_out + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm") + " - FIAS_Full_import_log" + ".txt";

    ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  Обновление части данных ФИАС из *.DBF ...");
    ui->pan_wait->setVisible(true);
    QApplication::processEvents();

    // переберём список файлов - удалим старые данные
    ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  удаление старых данных");
    QApplication::processEvents();

    for (int i=0; i<files_list_str.size(); i++) {
        QString fullname_str = files_list_str.at(i);
        QFileInfo fi(fullname_str);
        QString basename_str = fi.baseName();
        QString sql_file = "select id from fias.load_files where upper(file_name)=upper('" + basename_str + "') ; ";
        QSqlQuery *query_file = new QSqlQuery(db_FIAS2);
        if (!query_file->exec(sql_file)) {
            QMessageBox::warning(this, "Ошибка при получении ID файла",
                                 "При попытке получить из базы данных ФИАС ID файла произошла ошибка. \n\n" + basename_str + "\n\nФайл пропущен.");
            continue;
        }
        while (query_file->next()) {
            int id_file = query_file->value(0).toInt();

            QString sql_free;
            QSqlQuery *query_free = new QSqlQuery(db_FIAS2);


            // удаление старых данных из HOUSEINT
            sql_free = "delete from fias.houseint where id_file=" + QString::number(id_file) + " ; ";
            if (!query_free->exec(sql_free)) {
                delete query_free;
                db_FIAS2.rollback();
                QMessageBox::warning(this, "Ошибка при удалении данных ФИАС",
                                     "При попытке удалить старые данные ФИАС из таблицы HOUSEINT произошла ошибка. \n\n" +
                                     basename_str + "\n\nСтарые данные файла не удалены.");
                continue;
            }
            // удаление старых данных из ADDROBJ
            sql_free = "delete from fias.addrobj where id_file=" + QString::number(id_file) + " ; ";
            if (!query_free->exec(sql_free)) {
                delete query_free;
                db_FIAS2.rollback();
                QMessageBox::warning(this, "Ошибка при удалении данных ФИАС",
                                     "При попытке удалить старые данные ФИАС из таблицы ADDROBJ произошла ошибка. \n\n" +
                                     basename_str + "\n\nСтарые данные файла не удалены.");
                continue;
            }
            // удаление старых данных из HOUSE
            sql_free = "delete from fias.house where id_file=" + QString::number(id_file) + " ; ";
            if (!query_free->exec(sql_free)) {
                delete query_free;
                db_FIAS2.rollback();
                QMessageBox::warning(this, "Ошибка при удалении данных ФИАС",
                                     "При попытке удалить старые данные ФИАС из таблицы HOUSE произошла ошибка. \n\n" +
                                     basename_str + "\n\nСтарые данные файла не удалены.");
                continue;
            }
            // удаление старых данных из LOAD_FILES
            sql_free = "delete from fias.load_files where id=" + QString::number(id_file) + " ; ";
            if (!query_free->exec(sql_free)) {
                delete query_free;
                db_FIAS2.rollback();
                QMessageBox::warning(this, "Ошибка при удалении данных ФИАС",
                                     "При попытке удалить старые данные ФИАС из таблицы LOAD_FILES произошла ошибка. \n\n" +
                                     basename_str + "\n\nСтарые данные файла не удалены.");
                continue;
            }

            delete query_free;
        }
        delete query_file;
    }

    vacuum_FIAS_tables();

    date0 = QDate::currentDate();
    time0 = QTime::currentTime();
    date1 = QDate::currentDate();
    time1 = QTime::currentTime();

    // переберём список файлов
    for (int i=0; i<files_list_str.size(); i++) {
        QString fullname_str = files_list_str.at(i);
        bool res = import_FIAS_file(fullname_str, FIAS_import_log, true);
    }

    //------------------------------//
    // отображение статуса на экран //
    //------------------------------//
    date = QDate::currentDate();
    time = QTime::currentTime();

    int sec = time0.secsTo(time) + date0.daysTo(date)*24*60*60;
    int sut = sec/86400;
    sec = sec - sut*86400;
    int hou = sec/3600;
    sec = sec - hou*3600;
    int min = sec/60;
    sec = sec - min*60;
    ui->lab_time_all->setText( (sut>0 ? (QString::number(sut)+"д ") : " ") +
                               QString("00" + QString::number(hou)).right(2) + ":" +
                               QString("00" + QString::number(min)).right(2) + ":" +
                               QString("00" + QString::number(sec)).right(2) );

    sec = time1.secsTo(time) + date1.daysTo(date)*24*60*60;
    sut = sec/86400;
    sec = sec - sut*86400;
    hou = sec/3600;
    sec = sec - hou*3600;
    min = sec/60;
    sec = sec - min*60;
    ui->lab_time->setText( (sut>0 ? (QString::number(sut)+"д ") : " ") +
                           QString("00" + QString::number(hou)).right(2) + ":" +
                           QString("00" + QString::number(min)).right(2) + ":" +
                           QString("00" + QString::number(sec)).right(2) );
    date1 = QDate::currentDate();
    time1 = QTime::currentTime();
    QApplication::processEvents();


    ui->lab_filename-> setText("-//-");
    ui->lab_operation-> setText("-//-");
    ui->lab_cnt-> setText("-//-");
    ui->lab_result-> setText("-//-");


    vacuum_FIAS_tables();
    //reindex_FIAS_tables();


    ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  ... ОБНОВЛЕНИЕ ЧАСТИ ДАННЫХ ФИАС ИЗ *.DBF ЗАВЕРШЕНО\n-");
    QApplication::processEvents();


    // сохраним журнал импорта данных
    QFile f_log(FIAS_import_log);
    f_log.open(QIODevice::WriteOnly);
    f_log.write(ui->te_log->toPlainText().toLocal8Bit());
    f_log.close();

    ui->pan_wait->setVisible(false);
    QMessageBox::warning(this, "Готово", "Полная загрузка базы ФИАС закончена.");

    // открытие журнала
    long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(FIAS_import_log.utf16()), 0, 0, SW_NORMAL);
}

void fiasFiles_wnd::on_bn_FIAS_delta_clicked() {
    // получим место размещения файлов ФИАС
    QString FIAS_dir_str = QFileDialog::getExistingDirectory(this, "Выберите папку размещения файлов дельты для ФИАС");

    QDir FIAS_dir(FIAS_dir_str);
    if (!FIAS_dir.exists()) {
        QMessageBox::warning(this, "Папка не выбрана", "Данные ФИАС остались без изменения.");
        return;
    }
    QStringList filters;
    filters << "*.dbf";
    QStringList files_list_str = FIAS_dir.entryList(filters);

    QString FIAS_import_log = data_app.path_out + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm") + " - FIAS_Delta_import_log" + ".txt";

    ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  Импорт -дельты- ФИАС из *.DBF ...");
    ui->pan_wait->setVisible(true);
    QApplication::processEvents();

    // переберём список файлов
    for (int i=0; i<files_list_str.size(); i++) {
        QString filename_str = files_list_str.at(i);
        bool res = import_FIAS_delta(FIAS_dir_str + "/" + filename_str, FIAS_import_log, false);
    }

    // переберём список файлов
    for (int i=0; i<files_list_str.size(); i++) {
        QString filename_str = files_list_str.at(i);
        bool res = remove_FIAS_delta(FIAS_dir_str + "/" + filename_str, FIAS_import_log, false);
    }

    ui->lab_filename-> setText("-//-");
    ui->lab_operation-> setText("-//-");
    ui->lab_cnt-> setText("-//-");
    ui->lab_result-> setText("-//-");

    ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  ... готово\n-");
    QApplication::processEvents();


    // сохраним журнал импорта данных
    QFile f_log(FIAS_import_log);
    f_log.open(QIODevice::WriteOnly);
    f_log.write(ui->te_log->toPlainText().toLocal8Bit());
    f_log.close();

    ui->pan_wait->setVisible(false);
    QMessageBox::warning(this, "Готово", "Импорт -ДЕЛЬТЫ- ФИАС закончен.");

    // открытие журнала
    long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(FIAS_import_log.utf16()), 0, 0, SW_NORMAL);
}

void fiasFiles_wnd::vacuum_FIAS_tables() {
    date = QDate::currentDate();
    time = QTime::currentTime();

    ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  VACUUM ...");
    QApplication::processEvents();

    if (FIAS_db_drvr=="QPSQL") {
        // VACUUM
        ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  VACUUM таблицы ADDROBJ ");
        QApplication::processEvents();
        QString sql_vacuum1 = "VACUUM (ANALYZE) fias.addrobj ; ";
        QSqlQuery *query_vacuum1 = new QSqlQuery(db_FIAS2);
        if (!query_vacuum1->exec(sql_vacuum1))
            QMessageBox::warning(this, "Ошибка при попытке вакуума данных ФИАС",
                                 "При попытке вакуума таблицы addrobj ФИАС произошла ошибка. \n\nОперация пропущена.");
        delete query_vacuum1;

        ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  VACUUM таблицы HOUSE ");
        QApplication::processEvents();
        QString sql_vacuum2 = "VACUUM (ANALYZE) fias.house ; ";
        QSqlQuery *query_vacuum2 = new QSqlQuery(db_FIAS2);
        if (!query_vacuum2->exec(sql_vacuum2))
            QMessageBox::warning(this, "Ошибка при попытке вакуума данных ФИАС",
                                 "При попытке вакуума таблицы house ФИАС произошла ошибка. \n\nОперация пропущена.");
        delete query_vacuum2;

        ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  VACUUM таблицы HOUSEINT ");
        QApplication::processEvents();
        QString sql_vacuum3 = "VACUUM (ANALYZE) fias.houseint ; ";
        QSqlQuery *query_vacuum3 = new QSqlQuery(db_FIAS2);
        if (!query_vacuum3->exec(sql_vacuum3))
            QMessageBox::warning(this, "Ошибка при попытке вакуума данных ФИАС",
                                 "При попытке вакуума таблицы houseint ФИАС произошла ошибка. \n\nОперация пропущена.");
        delete query_vacuum3;

        ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  VACUUM таблицы LOAD_FILES ");
        QApplication::processEvents();
        QString sql_vacuum4 = "VACUUM (ANALYZE) fias.load_files ; ";
        QSqlQuery *query_vacuum4 = new QSqlQuery(db_FIAS2);
        if (!query_vacuum4->exec(sql_vacuum4))
            QMessageBox::warning(this, "Ошибка при попытке вакуума данных ФИАС",
                                 "При попытке вакуума таблицы load_files ФИАС произошла ошибка. \n\nОперация пропущена.");
        delete query_vacuum4;
    } else if (FIAS_db_drvr=="QIBASE") {
    }


    //------------------------------//
    // отображение статуса на экран //
    //------------------------------//
    date = QDate::currentDate();
    time = QTime::currentTime();

    int sec = time0.secsTo(time) + date0.daysTo(date)*24*60*60;
    int sut = sec/86400;
    sec = sec - sut*86400;
    int hou = sec/3600;
    sec = sec - hou*3600;
    int min = sec/60;
    sec = sec - min*60;
    ui->lab_time_all->setText( (sut>0 ? (QString::number(sut)+"д ") : " ") +
                               QString("00" + QString::number(hou)).right(2) + ":" +
                               QString("00" + QString::number(min)).right(2) + ":" +
                               QString("00" + QString::number(sec)).right(2) );

    sec = time1.secsTo(time) + date1.daysTo(date)*24*60*60;
    sut = sec/86400;
    sec = sec - sut*86400;
    hou = sec/3600;
    sec = sec - hou*3600;
    min = sec/60;
    sec = sec - min*60;
    ui->lab_time->setText( (sut>0 ? (QString::number(sut)+"д ") : " ") +
                           QString("00" + QString::number(hou)).right(2) + ":" +
                           QString("00" + QString::number(min)).right(2) + ":" +
                           QString("00" + QString::number(sec)).right(2) );
    date1 = QDate::currentDate();
    time1 = QTime::currentTime();
    QApplication::processEvents();


    ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  ... готово\n-");
    QApplication::processEvents();
    return;
}

void fiasFiles_wnd::reindex_FIAS_tables() {
    QApplication::processEvents();

    ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  REINDEX ...");

    if (FIAS_db_drvr=="QPSQL") {
        // REINDEX
        ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  REINDEX таблицы ADDROBJ ");
        QApplication::processEvents();
        QString sql_reindex1 = "REINDEX table fias.addrobj ; ";
        QSqlQuery *query_reindex1 = new QSqlQuery(db_FIAS2);
        if (!query_reindex1->exec(sql_reindex1))
            QMessageBox::warning(this, "Ошибка при переиндексации ФИАС",
                                 "При переиндексации таблицы house ФИАС произошла ошибка. \n\nОперация пропущена.");
        delete query_reindex1;

        ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  REINDEX таблицы HOUSE ");
        QApplication::processEvents();
        QString sql_reindex2 = "REINDEX table fias.house ; ";
        QSqlQuery *query_reindex2 = new QSqlQuery(db_FIAS2);
        if (!query_reindex2->exec(sql_reindex2))
            QMessageBox::warning(this, "Ошибка при переиндексации ФИАС",
                                 "При переиндексации таблицы house ФИАС произошла ошибка. \n\nОперация пропущена.");
        delete query_reindex2;

        ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  REINDEX таблицы HOUSEINT ");
        QApplication::processEvents();
        QString sql_reindex3 = "REINDEX table fias.houseint ; ";
        QSqlQuery *query_reindex3 = new QSqlQuery(db_FIAS2);
        if (!query_reindex3->exec(sql_reindex3))
            QMessageBox::warning(this, "Ошибка при переиндексации ФИАС",
                                 "При переиндексации таблицы house ФИАС произошла ошибка. \n\nОперация пропущена.");
        delete query_reindex3;

        ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  REINDEX таблицы LOAD_FILES ");
        QApplication::processEvents();
        QString sql_reindex4 = "REINDEX table fias.load_files ; ";
        QSqlQuery *query_reindex4 = new QSqlQuery(db_FIAS2);
        if (!query_reindex4->exec(sql_reindex4))
            QMessageBox::warning(this, "Ошибка при переиндексации ФИАС",
                                 "При переиндексации таблицы house ФИАС произошла ошибка. \n\nОперация пропущена.");
        delete query_reindex4;
    } else if (FIAS_db_drvr=="QIBASE") {
        // ADDROBJ
        ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  REINDEX таблицы ADDROBJ ");
        QApplication::processEvents();
        QString sql_reindex1 = "SET statistics INDEX addrobj_idx1 ; ";
        QSqlQuery *query_reindex1 = new QSqlQuery(db_FIAS2);
        if (!query_reindex1->exec(sql_reindex1))
            QMessageBox::warning(this, "Ошибка при переиндексации ФИАС",
                                 "При переиндексации addrobj_idx1 ФИАС произошла ошибка. \n\nОперация пропущена.");
        delete query_reindex1;
        QString sql_reindex2 = "SET statistics INDEX addrobj_idx2 ; ";
        QSqlQuery *query_reindex2 = new QSqlQuery(db_FIAS2);
        if (!query_reindex2->exec(sql_reindex2))
            QMessageBox::warning(this, "Ошибка при переиндексации ФИАС",
                                 "При переиндексации addrobj_idx2 ФИАС произошла ошибка. \n\nОперация пропущена.");
        delete query_reindex2;
        QString sql_reindex3 = "SET statistics INDEX addrobj_idx3 ; ";
        QSqlQuery *query_reindex3 = new QSqlQuery(db_FIAS2);
        if (!query_reindex3->exec(sql_reindex3))
            QMessageBox::warning(this, "Ошибка при переиндексации ФИАС",
                                 "При переиндексации addrobj_idx3 ФИАС произошла ошибка. \n\nОперация пропущена.");
        delete query_reindex3;
        QString sql_reindex4 = "SET statistics INDEX addrobj_idx4 ; ";
        QSqlQuery *query_reindex4 = new QSqlQuery(db_FIAS2);
        if (!query_reindex4->exec(sql_reindex4))
            QMessageBox::warning(this, "Ошибка при переиндексации ФИАС",
                                 "При переиндексации addrobj_idx4 ФИАС произошла ошибка. \n\nОперация пропущена.");
        delete query_reindex4;
        QString sql_reindex5 = "SET statistics INDEX addrobj_idx5 ; ";
        QSqlQuery *query_reindex5 = new QSqlQuery(db_FIAS2);
        if (!query_reindex5->exec(sql_reindex5))
            QMessageBox::warning(this, "Ошибка при переиндексации ФИАС",
                                 "При переиндексации addrobj_idx5 ФИАС произошла ошибка. \n\nОперация пропущена.");
        delete query_reindex5;
        QString sql_reindex6 = "SET statistics INDEX addrobj_idx6 ; ";
        QSqlQuery *query_reindex6 = new QSqlQuery(db_FIAS2);
        if (!query_reindex6->exec(sql_reindex6))
            QMessageBox::warning(this, "Ошибка при переиндексации ФИАС",
                                 "При переиндексации addrobj_idx6 ФИАС произошла ошибка. \n\nОперация пропущена.");
        delete query_reindex6;

        // HOUSE
        ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  REINDEX таблицы HOUSE ");
        QApplication::processEvents();
        QString sql_reindx21 = "SET statistics INDEX house_idx1 ; ";
        QSqlQuery *query_reindx21 = new QSqlQuery(db_FIAS2);
        if (!query_reindx21->exec(sql_reindx21))
            QMessageBox::warning(this, "Ошибка при переиндексации ФИАС",
                                 "При переиндексации house_idx1 ФИАС произошла ошибка. \n\nОперация пропущена.");
        delete query_reindx21;
        QString sql_reindx22 = "SET statistics INDEX house_idx2 ; ";
        QSqlQuery *query_reindx22 = new QSqlQuery(db_FIAS2);
        if (!query_reindx22->exec(sql_reindx22))
            QMessageBox::warning(this, "Ошибка при переиндексации ФИАС",
                                 "При переиндексации house_idx2 ФИАС произошла ошибка. \n\nОперация пропущена.");
        delete query_reindx22;
        QString sql_reindx23 = "SET statistics INDEX house_idx3 ; ";
        QSqlQuery *query_reindx23 = new QSqlQuery(db_FIAS2);
        if (!query_reindx23->exec(sql_reindx23))
            QMessageBox::warning(this, "Ошибка при переиндексации ФИАС",
                                 "При переиндексации house_idx3 ФИАС произошла ошибка. \n\nОперация пропущена.");
        delete query_reindx23;
        QString sql_reindx24 = "SET statistics INDEX house_idx4 ; ";
        QSqlQuery *query_reindx24 = new QSqlQuery(db_FIAS2);
        if (!query_reindx24->exec(sql_reindx24))
            QMessageBox::warning(this, "Ошибка при переиндексации ФИАС",
                                 "При переиндексации house_idx4 ФИАС произошла ошибка. \n\nОперация пропущена.");
        delete query_reindx24;
        QString sql_reindx25 = "SET statistics INDEX house_idx5 ; ";
        QSqlQuery *query_reindx25 = new QSqlQuery(db_FIAS2);
        if (!query_reindx25->exec(sql_reindx25))
            QMessageBox::warning(this, "Ошибка при переиндексации ФИАС",
                                 "При переиндексации house_idx5 ФИАС произошла ошибка. \n\nОперация пропущена.");
        delete query_reindx25;
        QString sql_reindx26 = "SET statistics INDEX house_idx6 ; ";
        QSqlQuery *query_reindx26 = new QSqlQuery(db_FIAS2);
        if (!query_reindx26->exec(sql_reindx26))
            QMessageBox::warning(this, "Ошибка при переиндексации ФИАС",
                                 "При переиндексации house_idx6 ФИАС произошла ошибка. \n\nОперация пропущена.");
        delete query_reindx26;

        // HOUSEINT
        ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  REINDEX таблицы HOUSEINT ");
        QApplication::processEvents();
        QString sql_reindx31 = "SET statistics INDEX houseint_idx1 ; ";
        QSqlQuery *query_reindx31 = new QSqlQuery(db_FIAS2);
        if (!query_reindx31->exec(sql_reindx31))
            QMessageBox::warning(this, "Ошибка при переиндексации ФИАС",
                                 "При переиндексации houseint_idx1 ФИАС произошла ошибка. \n\nОперация пропущена.");
        delete query_reindx31;
        QString sql_reindx32 = "SET statistics INDEX houseint_idx2 ; ";
        QSqlQuery *query_reindx32 = new QSqlQuery(db_FIAS2);
        if (!query_reindx32->exec(sql_reindx32))
            QMessageBox::warning(this, "Ошибка при переиндексации ФИАС",
                                 "При переиндексации houseint_idx2 ФИАС произошла ошибка. \n\nОперация пропущена.");
        delete query_reindx32;
        QString sql_reindx33 = "SET statistics INDEX houseint_idx3 ; ";
        QSqlQuery *query_reindx33 = new QSqlQuery(db_FIAS2);
        if (!query_reindx33->exec(sql_reindx33))
            QMessageBox::warning(this, "Ошибка при переиндексации ФИАС",
                                 "При переиндексации houseint_idx3 ФИАС произошла ошибка. \n\nОперация пропущена.");
        delete query_reindx33;
        QString sql_reindx34 = "SET statistics INDEX houseint_idx4 ; ";
        QSqlQuery *query_reindx34 = new QSqlQuery(db_FIAS2);
        if (!query_reindx34->exec(sql_reindx34))
            QMessageBox::warning(this, "Ошибка при переиндексации ФИАС",
                                 "При переиндексации houseint_idx4 ФИАС произошла ошибка. \n\nОперация пропущена.");
        delete query_reindx34;
    }

    //------------------------------//
    // отображение статуса на экран //
    //------------------------------//
    date = QDate::currentDate();
    time = QTime::currentTime();

    int sec = time0.secsTo(time) + date0.daysTo(date)*24*60*60;
    int sut = sec/86400;
    sec = sec - sut*86400;
    int hou = sec/3600;
    sec = sec - hou*3600;
    int min = sec/60;
    sec = sec - min*60;
    ui->lab_time_all->setText( (sut>0 ? (QString::number(sut)+"д ") : " ") +
                               QString("00" + QString::number(hou)).right(2) + ":" +
                               QString("00" + QString::number(min)).right(2) + ":" +
                               QString("00" + QString::number(sec)).right(2) );

    sec = time1.secsTo(time) + date1.daysTo(date)*24*60*60;
    sut = sec/86400;
    sec = sec - sut*86400;
    hou = sec/3600;
    sec = sec - hou*3600;
    min = sec/60;
    sec = sec - min*60;
    ui->lab_time->setText( (sut>0 ? (QString::number(sut)+"д ") : " ") +
                           QString("00" + QString::number(hou)).right(2) + ":" +
                           QString("00" + QString::number(min)).right(2) + ":" +
                           QString("00" + QString::number(sec)).right(2) );
    date1 = QDate::currentDate();
    time1 = QTime::currentTime();
    QApplication::processEvents();


    ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  ... готово\n-");
    QApplication::processEvents();
    return;
}

void fiasFiles_wnd::count_FIAS_data() {
    // -------------------------------- //
    // посчитаем число строк в таблицах //
    // -------------------------------- //
    ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  Подсчёт числа записей в таблицах ФИАС ...");
    QApplication::processEvents();

    QString sql_cnt_ao = "select count(*)  from fias.addrobj ; ";
    QSqlQuery *query_cnt_ao = new QSqlQuery(db_FIAS2);
    if (!query_cnt_ao->exec(sql_cnt_ao)) {
        ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  ADDROBJ - ошибка");
        QApplication::processEvents();
        QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке посчитать чисто строк в таблице ADDROBJ произошла неожиданная ошибка.");
        delete query_cnt_ao;
    } else {
        query_cnt_ao->next();
        int cnt = query_cnt_ao->value(0).toInt();
        QString s = QString::number(cnt);
        if (cnt>999)  s = s.left(s.length()-3) + "'" + s.right(3);
        if (cnt>999999)  s = s.left(s.length()-7) + "'" + s.right(7);
        if (cnt>999999999)  s = s.left(s.length()-11) + "'" + s.right(11);
        if (cnt>999999999999)  s = s.left(s.length()-15) + "'" + s.right(15);
        ui->lab_addrobj_cnt->setText("  "+s);
        ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  ADDROBJ :  " + QString::number(cnt) + "");
        QApplication::processEvents();
    }
    delete query_cnt_ao;

    QString sql_cnt_hs = "select count(*)  from fias.house ; ";
    QSqlQuery *query_cnt_hs = new QSqlQuery(db_FIAS2);
    if (!query_cnt_hs->exec(sql_cnt_hs)) {
        ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  HOUSE - ошибка");
        QApplication::processEvents();
        QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке посчитать чисто строк в таблице HOUSE произошла неожиданная ошибка.");
        delete query_cnt_hs;
    } else {
        query_cnt_hs->next();
        int cnt = query_cnt_hs->value(0).toInt();
        QString s = QString::number(cnt);
        if (cnt>999)  s = s.left(s.length()-3) + "'" + s.right(3);
        if (cnt>999999)  s = s.left(s.length()-7) + "'" + s.right(7);
        if (cnt>999999999)  s = s.left(s.length()-11) + "'" + s.right(11);
        if (cnt>999999999999)  s = s.left(s.length()-15) + "'" + s.right(15);
        ui->lab_house_cnt->setText("  "+s);
        ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  HOUSE :  " + QString::number(cnt) + "");
        QApplication::processEvents();
    }
    delete query_cnt_hs;


    QString sql_cnt_hi = "select count(*)  from fias.houseint ; ";
    QSqlQuery *query_cnt_hi = new QSqlQuery(db_FIAS2);
    if (!query_cnt_hi->exec(sql_cnt_hi)) {
        ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  HOUSEINT - ошибка");
        QApplication::processEvents();
        QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке посчитать чисто строк в таблице HOUSEINT произошла неожиданная ошибка.");
        delete query_cnt_hi;
    } else {
        query_cnt_hi->next();
        int cnt = query_cnt_hi->value(0).toInt();
        QString s = QString::number(cnt);
        if (cnt>999)  s = s.left(s.length()-3) + "'" + s.right(3);
        if (cnt>999999)  s = s.left(s.length()-7) + "'" + s.right(7);
        if (cnt>999999999)  s = s.left(s.length()-11) + "'" + s.right(11);
        if (cnt>999999999999)  s = s.left(s.length()-15) + "'" + s.right(15);
        ui->lab_houseint_cnt->setText("  "+s);
        ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  HOUSEINT :  " + QString::number(cnt) + "");
        QApplication::processEvents();
    }
    delete query_cnt_hi;

    ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  ... готово.\n-");
    QApplication::processEvents();
}

bool fiasFiles_wnd::import_FIAS_file(QString fullname_str, QString &FIAS_import_log, bool f_reload) {
    QFile f(fullname_str);
    QFileInfo fi(fullname_str);
    QString fname = fi.fileName().toUpper();
    QString fshrt = fi.baseName().toUpper();
    QString table = fi.baseName().toUpper();
    QString f_ext = fi.suffix().toUpper();

    if ( table.left(5)=="HOUSE"
         && table.length()==7 )
        table = "HOUSE";

    if ( table.left(6)=="ADDROB"
         && table.length()==8 )
        table = "ADDROBJ";

    ui->lab_filename->setText(fname);
    ui->lab_operation-> setText("-//-");
    ui->lab_cnt-> setText("-//-");
    ui->lab_result-> setText("-//-");

    if (f_ext=="DBF") {

        if ( fshrt=="ADDROBJ" ||
             fshrt=="HOUSEINT" ||
             ( fshrt.left(6)=="ADDROB" && fshrt.length()==8 ) ||
             ( fshrt.left(5)=="HOUSE" && fshrt.length()==7 ) ) {

            db_FIAS2.transaction();

            ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -   " + fname);
            int n = 0;

            // проверим, не загружен ли этот файл
            QString sql_test = "select id from fias.load_files where file_name='" + fshrt + "' ; ";
            QSqlQuery *query_test = new QSqlQuery(db_FIAS2);
            if (!query_test->exec(sql_test)) {
                ui->te_log->setText(ui->te_log->toPlainText() + "  ->  !!! ошибка загрузки !!!  ОСТАНОВКА");
                QMessageBox::critical(this, "Ошибка проверки, загружен ли файл",
                                      "При попытке проверить, загружен ли файл " + fshrt + " произошла ошибка!\n\n"
                                      + sql_test + "\n\n");
                delete query_test;
                return false;
            }
            if ( query_test->next() ) {
                int old_id_file = query_test->value(0).toInt();
                if (old_id_file>0) {
                    if (f_reload) {
                        // удалим старые данные
                        ui->te_log->setText(ui->te_log->toPlainText() + "  ->  уже загружен, удалим старые данные");
                        QApplication::processEvents();

                        QString sql_del_old = "delete from fias." + table + " where id_file=" + QString::number(old_id_file) + " ; ";
                        QSqlQuery *query_del_old = new QSqlQuery(db_FIAS2);
                        if (!query_del_old->exec(sql_del_old)) {
                            ui->te_log->setText(ui->te_log->toPlainText() + "  ->  !!! ошибка удаления старых данных !!!  ОСТАНОВКА");
                            QMessageBox::critical(this, "Ошибка удаления старых данных",
                                                  "При попытке удалить старые данные произошла ошибка!\n\n"
                                                  + sql_del_old + "\n\n");
                            delete query_del_old;
                            return false;
                        }
                        // удалим запись файла
                        QString sql_delfile = "delete from fias.load_files where id=" + QString::number(old_id_file) + " ; ";
                        QSqlQuery *query_delfile = new QSqlQuery(db_FIAS2);
                        if (!query_delfile->exec(sql_delfile)) {
                            ui->te_log->setText(ui->te_log->toPlainText() + "  ->  !!! ошибка удаления старой записи файла !!!  ОСТАНОВКА");
                            QMessageBox::critical(this, "Ошибка удаления старой записи файла",
                                                  "При попытке удалить старую запись файла произошла ошибка!\n\n"
                                                  + sql_delfile + "\n\n");
                            delete query_delfile;
                            return false;
                        }
                    } else {
                        // этот файл уже загружен - пропустим
                        ui->te_log->setText(ui->te_log->toPlainText() + "  ->  уже загружен, пропускаем");
                        QApplication::processEvents();
                        return true;
                    }
                }
            }
            delete query_test;

            m_jump:
            // запомним, что файл уже загружен
            QString sql_file = "INSERT INTO fias.load_files(file_name, dt_load, count_recs) "
                               " values ('" + fshrt + "', CURRENT_TIMESTAMP, NULL) "
                               " returning id ; ";
            QSqlQuery *query_file = new QSqlQuery(db_FIAS2);
            if (!query_file->exec(sql_file)) {
                ui->te_log->setText(ui->te_log->toPlainText() + "  ->  !!! ошибка добавления отметки о загрузке файла !!!  ОСТАНОВКА");
                QMessageBox::critical(this, "Ошибка добавления отметки о загрузке файла",
                                      "При попытке добавить запись о загрузке файла ФИАС " + fshrt + " произошла ошибка!\n\n"
                                      + sql_file + "\n\n");
                delete query_file;
                db_FIAS2.rollback();
                return false;
            }
            query_file->next();
            int id_file = query_file->value(0).toInt();
            delete query_file;


            ui->lab_operation->setText("подготовка");
            ui->lab_cnt-> setText("-//-");
            ui->lab_result-> setText("-//-");
            ui->te_log->setText(ui->te_log->toPlainText() + "  ->  подготовка");
            QApplication::processEvents();
m_ret1:
            // перенесём файл в папку _DBF_
            bool res_rem = QFile::remove(data_app.path_dbf + fname);
            if ( !QFile::copy(fullname_str, data_app.path_dbf + fname) ) {
                ui->te_log->setText(ui->te_log->toPlainText() + "  ->  !!! ошибка доступа к файлу !!!  пропускаем");
                if (QMessageBox::warning(this, "Ошибка доступа к файлу",
                                         "При попытке скопировать загружаемый файл в папку _DBF_ произошла ошибка. Целевой файл уже есть и занят другим процессом?\n\n"
                                         "Повторить попытку", QMessageBox::Retry|QMessageBox::Cancel, QMessageBox::Cancel)==QMessageBox::Retry) {
                    goto m_ret1;
                }
                ui->te_log->setText(ui->te_log->toPlainText() + "  ->  ! ошибка доступа к файлу - файл пропущен");
                QApplication::processEvents();
                return true;
            }

            ui->lab_operation->setText("чтение данных");
            ui->lab_cnt->setText("-//-");
            ui->lab_result->setText("-//-");
            ui->te_log->setText(ui->te_log->toPlainText() + "  ->  чтение данных");
            QApplication::processEvents();
        m_ret2:
            // чтение данных *.DBF
            QString sql = "select * from " + fshrt + " ; ";
            QSqlQuery *query = new QSqlQuery(db_ODBC);
            if (!query->exec(sql)) {
                if (QMessageBox::warning(this, "Ошибка чтения DBF-файла",
                                         "При попытке прочитать DBF-файл " + fshrt + ".dbf произошла ошибка. Файл открыт и редактируется другим процессом?\n\n"
                                         "Обновить соединение с ODBC и повторить попытку?",
                                         QMessageBox::Retry|QMessageBox::Cancel, QMessageBox::Retry)==QMessageBox::Retry) {
                    delete query;
                    db_ODBC.close();

                    // обновим подключение к ODBC
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
                            ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  ... При попытке переподключения к ODBC произошла ошибка! \nЗагрузка остановлена.\n-");
                            QApplication::processEvents();
                            return false;
                        }
                        ui->te_log->setText(ui->te_log->toPlainText() + "  ->  повторное подключение к ODBC");
                    }

                    goto m_ret2;
                }
                ui->te_log->setText(ui->te_log->toPlainText() + "  ->  ! ошибка чтения данных - файл пропущен");
                QApplication::processEvents();
                delete query;
                return true;
            }

            ui->lab_operation->setText("запись данных");
            ui->lab_cnt->setText("0");
            ui->lab_result->setText("-//-");
            ui->te_log->setText(ui->te_log->toPlainText() + "  ->  запись данных");
            QApplication::processEvents();

            // прочитаем названия колонок
            QStringList fields;
            QSqlRecord rec = query->record();
            QString sql_ins = "insert into fias." + table + "(id_file, ";
            for (int i=0; i<rec.count() && i<100; i++) {
                fields.append(rec.fieldName(i));
                if (i>0) sql_ins += ", ";
                sql_ins += rec.fieldName(i);
            }
            sql_ins += ") ";

            sql_ins += " values(" + QString::number(id_file) + ", ";
            while(query->next()) {
                // прочитаем данные строки
                QString sql_ins_data = sql_ins;

                QString value;
                for (int i=0; i<rec.count() && i<100; i++) {
                    value = query->value(i).toString().trimmed().replace("\'","\"");
                    if (i>0) sql_ins_data += ",";
                    if (value.isEmpty())
                        sql_ins_data += " NULL ";
                    else
                        sql_ins_data += " '" + value + "' ";
                }
                sql_ins_data += ") ; ";

                // копирование данных *.DBF в основную базу данных
                QSqlQuery *query_ins = new QSqlQuery(db_FIAS2);
                bool res_ins = query_ins->exec(sql_ins_data);
                if (!res_ins) {
                    ui->te_log->setText(ui->te_log->toPlainText() + "  ->  !!! ошибка записи данных ФИАС в базу !!!  ОСТАНОВКА");
                    QMessageBox::critical(this, "Ошибка записи данных",
                                          "При попытке добавить строку в таблицу " + table + " произошла ошибка!\n\n"
                                          + sql_ins_data + "\n"
                                          + "\ndrv: " + query_ins->lastError().driverText()
                                          + "\nDB: " + query_ins->lastError().databaseText() );
                    delete query_ins;
                    return false;
                }
                delete query_ins;

                n++;
                if (n%2341==0) {
                    //------------------------------//
                    // отображение статуса на экран //
                    //------------------------------//
                    date = QDate::currentDate();
                    time = QTime::currentTime();

                    int sec = time0.secsTo(time) + date0.daysTo(date)*24*60*60;
                    int sut = sec/86400;
                    sec = sec - sut*86400;
                    int hou = sec/3600;
                    sec = sec - hou*3600;
                    int min = sec/60;
                    sec = sec - min*60;
                    ui->lab_time_all->setText( (sut>0 ? (QString::number(sut)+"д ") : " ") +
                                               QString("00" + QString::number(hou)).right(2) + ":" +
                                               QString("00" + QString::number(min)).right(2) + ":" +
                                               QString("00" + QString::number(sec)).right(2) );

                    sec = time1.secsTo(time) + date1.daysTo(date)*24*60*60;
                    sut = sec/86400;
                    sec = sec - sut*86400;
                    hou = sec/3600;
                    sec = sec - hou*3600;
                    min = sec/60;
                    sec = sec - min*60;
                    ui->lab_time->setText( (sut>0 ? (QString::number(sut)+"д ") : " ") +
                                           QString("00" + QString::number(hou)).right(2) + ":" +
                                           QString("00" + QString::number(min)).right(2) + ":" +
                                           QString("00" + QString::number(sec)).right(2) );
                    date1 = QDate::currentDate();
                    time1 = QTime::currentTime();
                    QApplication::processEvents();

                    ui->lab_cnt->setText(QString::number(n));
                    QApplication::processEvents();
                }
            }

            ui->lab_cnt->setText("-//-");
            ui->lab_result->setText("Готово");
            ui->te_log->setText(ui->te_log->toPlainText() + "  ->  " + QString::number(n));
            ui->te_log->setText(ui->te_log->toPlainText() + "  ->  готово");
            QApplication::processEvents();


            // обновим подключение к ODBC
            db_ODBC.close();
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
                    ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  ... При попытке переподключения к ODBC произошла ошибка! \nЗагрузка остановлена.\n-");
                    QApplication::processEvents();
                    return false;
                }
                ui->te_log->setText(ui->te_log->toPlainText() + "  ->  повторное подключение к ODBC");
            }

            // запомним число загруженных строк
            QString sql_count = "UPDATE fias.load_files "
                                "   set count_recs=" + QString::number(n) + " "
                                " where id=" + QString::number(id_file) + " ; ";
            QSqlQuery *query_count = new QSqlQuery(db_FIAS2);
            if (!query_count->exec(sql_count)) {
                QMessageBox::critical(this, "Ошибка сохранения числа строк в файле",
                                      "При попытке сохранить число записей загруженного файла ФИАС " + fshrt + " произошла ошибка!\n\n"
                                      + sql_count + "\n\n");
                delete query_count;
                db_FIAS2.rollback();
                return false;
            }
            delete query_count;

            db_FIAS2.commit();

            // сохраним журнал импорта данных
            QFile f_log(FIAS_import_log);
            f_log.open(QIODevice::WriteOnly);
            f_log.write(ui->te_log->toPlainText().toLocal8Bit());
            f_log.close();

            return true;
        } else

        // Этот файл ФИАС грузить не надо - просто пропустим
        {   QApplication::processEvents();
            return true;
        }
    } else

    // Этот файл грузить не надо - просто пропустим
    {   QApplication::processEvents();
        return true;
    }
}

bool fiasFiles_wnd::import_FIAS_delta(QString fullname_str, QString &FIAS_import_log, bool f_reload) {
    QFile f(fullname_str);
    QFileInfo fi(fullname_str);
    QString fname = fi.fileName().toUpper();
    QString fshrt = fi.baseName().toUpper();
    QString table = fi.baseName().toUpper();
    QString f_ext = fi.suffix().toUpper();

    if ( table.left(5)=="HOUSE"
         && table.length()==7 )
        table = "HOUSE";

    if ( table.left(1)=="D" )
        table = table.right(table.length()-1);

    ui->lab_filename->setText(fname);
    ui->lab_operation-> setText("-//-");
    ui->lab_cnt-> setText("-//-");
    ui->lab_result-> setText("-//-");

    if (f_ext=="DBF") {

        // загрузка и обновление данных
        if ( fshrt=="HOUSEINT" ||
             ( fshrt.left(6)=="ADDROB" && fshrt.length()==8 ) ||
             ( fshrt.left(5)=="HOUSE"  && fshrt.length()==7 ) ) {

            db_FIAS2.transaction();

            ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -   " + fname);
            int n = 0;

            ui->lab_operation->setText("подготовка");
            ui->lab_cnt-> setText("-//-");
            ui->lab_result-> setText("-//-");
            ui->te_log->setText(ui->te_log->toPlainText() + "  ->  подготовка");
            QApplication::processEvents();
m_ret1:
            // перенесём файл в папку _DBF_
            bool res_rem = QFile::remove(data_app.path_dbf + fname);
            if ( !QFile::copy(fullname_str, data_app.path_dbf + fname) ) {
                ui->te_log->setText(ui->te_log->toPlainText() + "  ->  !!! ошибка доступа к файлу !!!  пропускаем");
                if (QMessageBox::warning(this, "Ошибка доступа к файлу",
                                         "При попытке скопировать загружаемый файл в папку _DBF_ произошла ошибка. Целевой файл уже есть и занят другим процессом?\n\n"
                                         "Повторить попытку", QMessageBox::Retry|QMessageBox::Cancel, QMessageBox::Retry)==QMessageBox::Retry) {
                    goto m_ret1;
                }
                ui->te_log->setText(ui->te_log->toPlainText() + "  ->  ! ошибка доступа к файлу - файл пропущен");
                QApplication::processEvents();
                return true;
            }

            // найдём id записи файла, в состав которого входит эта дельта
            QString sql_id = "";
            sql_id += "select id "
                      "  from fias.load_files "
                      " where file_name='" + table + "' "
                      "    or file_name='" + fshrt + "' ; ";

            QSqlQuery *query_id = new QSqlQuery(db);
            bool res_id = mySQL.exec(this, sql_id, QString("Поиск ID файла, в который входит дельта"), *query_id, true, db, data_app);
            if (!res_id /*|| query_id->size()<=0*/) {
                QMessageBox::warning(this, "ID файла, в который входит дельта не получен", "При ID файла, в который входит дельта, произошла неожиданная ошибка.");
                delete query_id;
                return false;
            }
            query_id->next();
            int id_file = query_id->value(0).toInt();
            delete query_id;

            ui->lab_operation->setText("чтение данных");
            ui->lab_cnt->setText("-//-");
            ui->lab_result->setText("-//-");
            ui->te_log->setText(ui->te_log->toPlainText() + "  ->  чтение данных");
            QApplication::processEvents();
        m_ret2:
            // чтение данных *.DBF
            QString sql = "select * from " + fshrt + " ; ";
            QSqlQuery *query = new QSqlQuery(db_ODBC);
            if (!query->exec(sql)) {
                if (QMessageBox::warning(this, "Ошибка чтения DBF-файла",
                                         "При попытке прочитать DBF-файл " + fshrt + ".dbf произошла ошибка. Файл открыт и редактируется другим процессом?\n\n"
                                         "Обновить соединение с ODBC и повторить попытку?",
                                         QMessageBox::Retry|QMessageBox::Cancel, QMessageBox::Retry)==QMessageBox::Retry) {
                    delete query;
                    db_ODBC.close();

                    // обновим подключение к ODBC
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
                            ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  ... При попытке переподключения к ODBC произошла ошибка! \nЗагрузка остановлена.\n-");
                            QApplication::processEvents();
                            return false;
                        }
                        ui->te_log->setText(ui->te_log->toPlainText() + "  ->  повторное подключение к ODBC");
                    }

                    goto m_ret2;
                }
                ui->te_log->setText(ui->te_log->toPlainText() + "  ->  ! ошибка чтения данных - файл пропущен");
                QApplication::processEvents();
                delete query;
                return true;
            }

            ui->lab_operation->setText("обновление данных");
            ui->lab_cnt->setText("0");
            ui->lab_result->setText("-//-");
            ui->te_log->setText(ui->te_log->toPlainText() + "  ->  обновление данных");
            QApplication::processEvents();

            // подготовим запрос на удаление старой версии записи
            QString sql_clear = "delete from fias." + table + " where #FIELD1#='#GUID1#' or #FIELD1#='#GUID2#' ; ";
            if (table=="ADDROBJ") {
                sql_clear = sql_clear.replace("#FIELD1#", "AOID").replace("#FIELD2#", "PREVID");
            } else if (table=="HOUSE") {
                sql_clear = sql_clear.replace("#FIELD1#", "HOUSEID").replace("#FIELD2#", "HOUSEGUID");
            } else if (table=="HOUSEINT") {
                sql_clear = sql_clear.replace("#FIELD1#", "HOUSEINTID").replace("#FIELD1#", "INTGUID");
            } else {
                QMessageBox::warning(this, "Попытка загрузить дельту неожиданной таблицы",
                                     "При попытке загрузить дельту неожиданной таблицы ФИАС получен отказ системы.\n\n"
                                     + table +
                                     "\n\nОбновление файла отменено.");
                db_FIAS2.rollback();
                return false;
            }

            // прочитаем названия колонок
            int n_field = 0;
            int n_GUID1_field = -1;
            QString GUID1_str = "";
            int n_GUID2_field = -1;
            QString GUID2_str = "";
            QStringList fields;
            QSqlRecord rec = query->record();
            QString sql_ins = "insert into fias." + table + "(id_file, ";
            for (int i=0; i<rec.count(); i++) {
                fields.append(rec.fieldName(i));
                if (i>0) sql_ins += ", ";
                sql_ins += rec.fieldName(i);

                // запомним порядковый номер поля с GUID1 записи
                if ( (table=="ADDROBJ" && rec.fieldName(i)=="AOID")
                   ||(table=="HOUSE" && rec.fieldName(i)=="HOUSEID")
                   ||(table=="HOUSEINT" && rec.fieldName(i)=="HOUSEINTID") ) {
                    n_GUID1_field = n_field;
                }

                // запомним порядковый номер поля с GUID2 записи
                if ( (table=="ADDROBJ" && rec.fieldName(i)=="PREVID")
                   ||(table=="HOUSE" && rec.fieldName(i)=="HOUSEGUID")
                   ||(table=="HOUSEINT" && rec.fieldName(i)=="INTGUID") ) {
                    n_GUID2_field = n_field;
                }
                n_field++;
            }
            sql_ins += ") ";

            sql_ins += " values(" + QString::number(id_file) + ", ";
            while(query->next()) {
                // прочитаем данные строки
                QString sql_ins_data = sql_ins;

                QString value;
                for (int i=0; i<rec.count(); i++) {
                    value = query->value(i).toString().trimmed().replace("\'","\"");
                    if (i>0) sql_ins_data += ",";
                    if (value.isEmpty())
                        sql_ins_data += " NULL ";
                    else
                        sql_ins_data += " '" + value + "' ";

                    // запомним GUID1 записи
                    if (i==n_GUID1_field) {
                        GUID1_str = value;
                    }

                    // запомним GUID2 записи
                    if (i==n_GUID2_field) {
                        GUID2_str = value;
                    }
                }
                sql_ins_data += ") ; ";

                // копирование данных *.DBF в основную базу данных
                QString sql_clear_data = sql_clear;
                sql_clear_data = sql_clear_data.replace("#GUID1#", GUID1_str).replace("#GUID2#", GUID2_str).replace("=''", " is NULL ");
                QSqlQuery *query_clear_data = new QSqlQuery(db_FIAS2);
                if (!query_clear_data->exec(sql_clear_data)) {
                    ui->te_log->setText(ui->te_log->toPlainText() + "  ->  !!! ошибка удаления старых данных ФИАС !!!  ОСТАНОВКА");
                    QMessageBox::critical(this, "Ошибка удаления данных",
                                          "При попытке удалить старые данные в таблице " + table + " произошла ошибка!\n\n"
                                          + sql_clear_data + "\n"
                                          + "\ndrv: " + query_clear_data->lastError().driverText()
                                          + "\nDB: " + query_clear_data->lastError().databaseText() );
                    delete query_clear_data;
                    return false;
                }
                delete query_clear_data;


                QSqlQuery *query_ins = new QSqlQuery(db_FIAS2);
                if (!query_ins->exec(sql_ins_data)) {
                    ui->te_log->setText(ui->te_log->toPlainText() + "  ->  !!! ошибка записи данных ФИАС в базу !!!  ОСТАНОВКА");
                    QMessageBox::critical(this, "Ошибка записи данных",
                                          "При попытке добавить строку в таблицу " + table + " произошла ошибка!\n\n"
                                          + sql_ins_data + "\n"
                                          + "\ndrv: " + query_ins->lastError().driverText()
                                          + "\nDB: " + query_ins->lastError().databaseText() );
                    delete query_ins;
                    return false;
                }
                delete query_ins;


                n++;
                if (n%231==0) {
                    //------------------------------//
                    // отображение статуса на экран //
                    //------------------------------//
                    date = QDate::currentDate();
                    time = QTime::currentTime();

                    int sec = time0.secsTo(time) + date0.daysTo(date)*24*60*60;
                    int sut = sec/86400;
                    sec = sec - sut*86400;
                    int hou = sec/3600;
                    sec = sec - hou*3600;
                    int min = sec/60;
                    sec = sec - min*60;
                    ui->lab_time_all->setText( (sut>0 ? (QString::number(sut)+"д ") : " ") +
                                               QString("00" + QString::number(hou)).right(2) + ":" +
                                               QString("00" + QString::number(min)).right(2) + ":" +
                                               QString("00" + QString::number(sec)).right(2) );

                    sec = time1.secsTo(time) + date1.daysTo(date)*24*60*60;
                    sut = sec/86400;
                    sec = sec - sut*86400;
                    hou = sec/3600;
                    sec = sec - hou*3600;
                    min = sec/60;
                    sec = sec - min*60;
                    ui->lab_time->setText( (sut>0 ? (QString::number(sut)+"д ") : " ") +
                                           QString("00" + QString::number(hou)).right(2) + ":" +
                                           QString("00" + QString::number(min)).right(2) + ":" +
                                           QString("00" + QString::number(sec)).right(2) );
                    date1 = QDate::currentDate();
                    time1 = QTime::currentTime();
                    QApplication::processEvents();

                    ui->lab_cnt->setText(QString::number(n));
                    QApplication::processEvents();
                }
            }

            ui->lab_cnt->setText("-//-");
            ui->lab_result->setText("Готово");
            ui->te_log->setText(ui->te_log->toPlainText() + "  ->  " + QString::number(n));
            ui->te_log->setText(ui->te_log->toPlainText() + "  ->  готово");
            QApplication::processEvents();


            // обновим подключение к ODBC
            db_ODBC.close();
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
                    ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  ... При попытке переподключения к ODBC произошла ошибка! \nЗагрузка остановлена.\n-");
                    QApplication::processEvents();
                    return false;
                }
                ui->te_log->setText(ui->te_log->toPlainText() + "  ->  повторное подключение к ODBC");
            }

            // запомним число загруженных строк
            QString sql_count = "UPDATE fias.load_files "
                                "   set count_recs=" + QString::number(n) + " "
                                " where id=" + QString::number(id_file) + " ; ";
            QSqlQuery *query_count = new QSqlQuery(db_FIAS2);
            if (!query_count->exec(sql_count)) {
                QMessageBox::critical(this, "Ошибка сохранения числа строк в файле",
                                      "При попытке сохранить число записей загруженного файла ФИАС " + fshrt + " произошла ошибка!\n\n"
                                      + sql_count + "\n\n");
                delete query_count;
                db_FIAS2.rollback();
                return false;
            }
            delete query_count;

            db_FIAS2.commit();

            // сохраним журнал импорта данных
            QFile f_log(FIAS_import_log);
            f_log.open(QIODevice::WriteOnly);
            f_log.write(ui->te_log->toPlainText().toLocal8Bit());
            f_log.close();

            return true;

        } else

        // Этот файл ФИАС грузить не надо - просто пропустим
        {   QApplication::processEvents();
            return true;
        }
    } else

    // Этот файл грузить не надо - просто пропустим
    {   QApplication::processEvents();
        return true;
    }
}

bool fiasFiles_wnd::remove_FIAS_delta(QString fullname_str, QString &FIAS_import_log, bool f_reload) {
    QFile f(fullname_str);
    QFileInfo fi(fullname_str);
    QString fname = fi.fileName().toUpper();
    QString fshrt = fi.baseName().toUpper();
    QString table = fi.baseName().toUpper();
    QString f_ext = fi.suffix().toUpper();

    if ( table.left(5)=="HOUSE"
         && table.length()==7 )
        table = "HOUSE";

    if ( table.left(1)=="D" )
        table = table.right(table.length()-1);

    ui->lab_filename->setText(fname);
    ui->lab_operation-> setText("-//-");
    ui->lab_cnt-> setText("-//-");
    ui->lab_result-> setText("-//-");

    if (f_ext=="DBF") {

        // удаление технологически удалённых данных
        if ( fshrt=="DADDROBJ" ||
             fshrt=="DHOUSEINT"||
             fshrt=="DHOUSE" ) {

            db_FIAS2.transaction();

            ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -   " + fname);
            int n = 0;

            ui->lab_operation->setText("удаление");
            ui->lab_cnt-> setText("-//-");
            ui->lab_result-> setText("-//-");
            ui->te_log->setText(ui->te_log->toPlainText() + "  ->  удаление");
            QApplication::processEvents();
m_ret1:
            // перенесём файл в папку _DBF_
            bool res_rem = QFile::remove(data_app.path_dbf + fname);
            if ( !QFile::copy(fullname_str, data_app.path_dbf + fname) ) {
                ui->te_log->setText(ui->te_log->toPlainText() + "  ->  !!! ошибка доступа к файлу !!!  пропускаем");
                if (QMessageBox::warning(this, "Ошибка доступа к файлу",
                                         "При попытке скопировать загружаемый файл в папку _DBF_ произошла ошибка. Целевой файл уже есть и занят другим процессом?\n\n"
                                         "Повторить попытку", QMessageBox::Retry|QMessageBox::Cancel, QMessageBox::Retry)==QMessageBox::Retry) {
                    goto m_ret1;
                }
                ui->te_log->setText(ui->te_log->toPlainText() + "  ->  ! ошибка доступа к файлу - файл пропущен");
                QApplication::processEvents();
                return true;
            }

            ui->lab_operation->setText("чтение мусора");
            ui->lab_cnt->setText("-//-");
            ui->lab_result->setText("-//-");
            ui->te_log->setText(ui->te_log->toPlainText() + "  ->  чтение мусора");
            QApplication::processEvents();
        m_ret2:
            // чтение данных *.DBF
            QString sql = "select * from " + fshrt + " ; ";
            QSqlQuery *query = new QSqlQuery(db_ODBC);
            if (!query->exec(sql)) {
                if (QMessageBox::warning(this, "Ошибка чтения DBF-файла",
                                         "При попытке прочитать DBF-файл " + fshrt + ".dbf произошла ошибка. Файл открыт и редактируется другим процессом?\n\n"
                                         "Обновить соединение с ODBC и повторить попытку?",
                                         QMessageBox::Retry|QMessageBox::Cancel, QMessageBox::Retry)==QMessageBox::Retry) {
                    delete query;
                    db_ODBC.close();

                    // обновим подключение к ODBC
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
                            ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  ... При попытке переподключения к ODBC произошла ошибка! \nЗагрузка остановлена.\n-");
                            QApplication::processEvents();
                            return false;
                        }
                        ui->te_log->setText(ui->te_log->toPlainText() + "  ->  повторное подключение к ODBC");
                    }

                    goto m_ret2;
                }
                ui->te_log->setText(ui->te_log->toPlainText() + "  ->  ! ошибка чтения данных - файл пропущен");
                QApplication::processEvents();
                delete query;
                return true;
            }

            ui->lab_operation->setText("удаление мусора");
            ui->lab_cnt->setText("0");
            ui->lab_result->setText("-//-");
            ui->te_log->setText(ui->te_log->toPlainText() + "  ->  удаление мусора");
            QApplication::processEvents();

            // подготовим запрос на удаление старой версии записи
            QString sql_clear = "delete from fias." + table + " where #FIELD1#='#GUID1#' or #FIELD1#='#GUID2#' ; ";
            if (table=="ADDROBJ") {
                sql_clear = sql_clear.replace("#FIELD1#", "AOID").replace("#FIELD2#", "PREVID");
            } else if (table=="HOUSE") {
                sql_clear = sql_clear.replace("#FIELD1#", "HOUSEID").replace("#FIELD2#", "HOUSEGUID");
            } else if (table=="HOUSEINT") {
                sql_clear = sql_clear.replace("#FIELD1#", "HOUSEINTID").replace("#FIELD1#", "INTGUID");
            } else {
                QMessageBox::warning(this, "Попытка загрузить дельту неожиданной таблицы",
                                     "При попытке загрузить дельту неожиданной таблицы ФИАС получен отказ системы.\n\n"
                                     + table +
                                     "\n\nОбновление файла отменено.");
                db_FIAS2.rollback();
                return false;
            }

            // прочитаем названия колонок
            int n_field = 0;
            int n_GUID1_field = -1;
            QString GUID1_str = "";
            int n_GUID2_field = -1;
            QString GUID2_str = "";
            QStringList fields;
            QSqlRecord rec = query->record();
            for (int i=0; i<rec.count(); i++) {
                fields.append(rec.fieldName(i));

                // запомним порядковый номер поля с GUID1 записи
                if ( (table=="ADDROBJ" && rec.fieldName(i)=="AOID")
                   ||(table=="HOUSE" && rec.fieldName(i)=="HOUSEID")
                   ||(table=="HOUSEINT" && rec.fieldName(i)=="HOUSEINTID") ) {
                    n_GUID1_field = n_field;
                }

                // запомним порядковый номер поля с GUID2 записи
                if ( (table=="ADDROBJ" && rec.fieldName(i)=="PREVID")
                   ||(table=="HOUSE" && rec.fieldName(i)=="HOUSEGUID")
                   ||(table=="HOUSEINT" && rec.fieldName(i)=="INTGUID") ) {
                    n_GUID2_field = n_field;
                }
                n_field++;
            }

            while(query->next()) {
                // прочитаем данные строки
                QString value;
                for (int i=0; i<rec.count(); i++) {
                    value = query->value(i).toString().trimmed().replace("\'","\"");

                    // запомним GUID1 записи
                    if (i==n_GUID1_field)  GUID1_str = value;

                    // запомним GUID2 записи
                    if (i==n_GUID2_field)  GUID2_str = value;
                }

                // копирование данных *.DBF в основную базу данных
                QString sql_clear_data = sql_clear;
                sql_clear_data = sql_clear_data.replace("#GUID1#", GUID1_str).replace("#GUID2#", GUID2_str).replace("=''", " is NULL ");
                QSqlQuery *query_clear_data = new QSqlQuery(db_FIAS2);
                if (!query_clear_data->exec(sql_clear_data)) {
                    ui->te_log->setText(ui->te_log->toPlainText() + "  ->  !!! ошибка удаления старых данных ФИАС !!!  ОСТАНОВКА");
                    QMessageBox::critical(this, "Ошибка удаления данных",
                                          "При попытке ужадить старые данные в таблице " + table + " произошла ошибка!\n\n"
                                          + sql_clear_data + "\n"
                                          + "\ndrv: " + query_clear_data->lastError().driverText()
                                          + "\nDB: " + query_clear_data->lastError().databaseText() );
                    delete query_clear_data;
                    return false;
                }
                delete query_clear_data;

                n++;
                if (n%231==0) {
                    ui->lab_cnt->setText(QString::number(n));
                    QApplication::processEvents();
                }
            }

            ui->lab_cnt->setText("-//-");
            ui->lab_result->setText("Готово");
            ui->te_log->setText(ui->te_log->toPlainText() + "  ->  " + QString::number(n));
            ui->te_log->setText(ui->te_log->toPlainText() + "  ->  готово");
            QApplication::processEvents();


            // обновим подключение к ODBC
            db_ODBC.close();
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
                    ui->te_log->append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "  -  ... При попытке переподключения к ODBC произошла ошибка! \nЗагрузка остановлена.\n-");
                    QApplication::processEvents();
                    return false;
                }
                ui->te_log->setText(ui->te_log->toPlainText() + "  ->  повторное подключение к ODBC");
            }

            db_FIAS2.commit();

            // сохраним журнал импорта данных
            QFile f_log(FIAS_import_log);
            f_log.open(QIODevice::WriteOnly);
            f_log.write(ui->te_log->toPlainText().toLocal8Bit());
            f_log.close();

            return true;
        } else

        // Этот файл ФИАС грузить не надо - просто пропустим
        {   QApplication::processEvents();
            return true;
        }
    } else

    // Этот файл грузить не надо - просто пропустим
    {   QApplication::processEvents();
        return true;
    }
}

void fiasFiles_wnd::on_bn_FIAS_IDs_clicked() {
    db.transaction();
    QString sql_adr = "select a.id, a.fias_aoid, a.fias_houseid, a.kladr, a.kladr_street, a.house, a.corp "
                      "  from addresses a "
                      " where (FIAS_AOID is NULL or FIAS_HOUSEID is NULL) "
                      "   and (KLADR is not NULL or KLADR_STREET is not NULL) ; ";
    QSqlQuery *query_adr = new QSqlQuery(db);
    if (!query_adr->exec(sql_adr)) {
        QMessageBox::critical(this, "Ошибка при получении адресов без кодов ФИАС",
                              "При получении адресов без кодов ФИАС произошла ошибка!\n\n"
                              + sql_adr + "\n\n");
        delete query_adr;
        db.rollback();
        return;
    }

    int n = 0;
    while (query_adr->next()) {
        int id                 = query_adr->value(0).toInt();
        QString fias_aoguid    = query_adr->value(1).toString();
        QString fias_houseguid = query_adr->value(2).toString();
        QString kladr          = query_adr->value(3).toString();
        QString kladr_street   = query_adr->value(4).toString();
        int house              = query_adr->value(5).toInt();
        QString corp           = query_adr->value(6).toString().toUpper().trimmed();

        bool f_save = false;

        if ( fias_aoguid.isEmpty()
             && !kladr_street.isEmpty() ) {
            // найдём код ФИАС AOGUID
            QString sql_aoguid = "select aoid, aoguid "
                                 "  from fias.addrobj ao "
                                 " where code='" + kladr_street + "'"
                                 "   and (ao.enddate is NULL or ao.enddate>CURRENT_DATE) "
                                 " limit 1 ; ";
            QSqlQuery *query_aoguid = new QSqlQuery(db_FIAS2);
            if (!query_aoguid->exec(sql_aoguid)) {
                QMessageBox::critical(this, "Ошибка проверки кода ФИАС для КЛАДР ",
                                      "При проверке кода ФИАС для КЛАДР произошла ошибка!\n\n"
                                      + sql_aoguid + "\n\n");
                delete query_aoguid;
                db.rollback();
                return;
            }
            if (query_aoguid->next()) {
                fias_aoguid = query_aoguid->value(1).toString();
                f_save = true;
            }
            delete query_aoguid;
        }


        if ( fias_aoguid.isEmpty()
             && !kladr.isEmpty() ) {
            // найдём код ФИАС AOGUID
            QString sql_aoguid = "select aoid, aoguid "
                                 "  from fias.addrobj ao "
                                 " where code='" + kladr + "'"
                                 "   and (ao.enddate is NULL or ao.enddate>CURRENT_DATE) "
                                 " limit 1 ; ";
            QSqlQuery *query_aoguid = new QSqlQuery(db_FIAS2);
            if (!query_aoguid->exec(sql_aoguid)) {
                QMessageBox::critical(this, "Ошибка проверки кода ФИАС для КЛАДР ",
                                      "При проверке кода ФИАС для КЛАДР произошла ошибка!\n\n"
                                      + sql_aoguid + "\n\n");
                delete query_aoguid;
                db.rollback();
                return;
            }
            if (query_aoguid->next()) {
                fias_aoguid = query_aoguid->value(1).toString();
                f_save = true;
            }
            delete query_aoguid;
        }


        if ( fias_houseguid.isEmpty()
             && !kladr_street.isEmpty() ) {
            // найдём код ФИАС HOUSEGUID
            QString sql_houseguid = "select h.houseid, h.houseguid "
                                    "  from fias.addrobj ao "
                                    "  left join fias.house h on(h.aoguid=ao.aoguid and (h.enddate is NULL or h.enddate>CURRENT_DATE) ) "
                                    " where ao.code='" + kladr_street + "' "
                                    "   and h.housenum in('" + QString::number(house) + corp + "', '" +
                                                               QString::number(house) + " " + corp + "', '" +
                                                               QString::number(house) + "/" + corp + "', '" +
                                                               QString::number(house) + "') "
                                    "   and (ao.enddate is NULL or ao.enddate>CURRENT_DATE) "
                                    "   and (h.enddate  is NULL or h.enddate>CURRENT_DATE) "
                                    " limit 1 ; ";
            QSqlQuery *query_houseguid = new QSqlQuery(db_FIAS2);
            if (!query_houseguid->exec(sql_houseguid)) {
                QMessageBox::critical(this, "Ошибка проверки кода ФИАС для КЛАДР ",
                                      "При проверке кода ФИАС для КЛАДР произошла ошибка!\n\n"
                                      + sql_houseguid + "\n\n");
                delete query_houseguid;
                db.rollback();
                return;
            }
            if (query_houseguid->next()) {
                fias_houseguid = query_houseguid->value(1).toString();
                f_save = true;
            }
            delete query_houseguid;
        }


        if ( fias_houseguid.isEmpty()
             && !kladr.isEmpty() ) {
            // найдём код ФИАС HOUSEGUID
            QString sql_houseguid = "select h.houseid, h.houseguid "
                                    "  from fias.addrobj ao "
                                    "  left join fias.house h on(h.aoguid=ao.aoguid and (h.enddate is NULL or h.enddate>CURRENT_DATE) ) "
                                    " where ao.code='" + kladr + "' "
                                    "   and h.buildnum in('" + QString::number(house) + corp + "', '" +
                                                               QString::number(house) + " " + corp + "', '" +
                                                               QString::number(house) + "/" + corp + "', '" +
                                                               QString::number(house) + "') "
                                    "   and (ao.enddate is NULL or ao.enddate>CURRENT_DATE) "
                                    "   and (h.enddate  is NULL or h.enddate>CURRENT_DATE) "
                                    " limit 1 ; ";
            QSqlQuery *query_houseguid = new QSqlQuery(db_FIAS2);
            if (!query_houseguid->exec(sql_houseguid)) {
                QMessageBox::critical(this, "Ошибка проверки кода ФИАС для КЛАДР ",
                                      "При проверке кода ФИАС для КЛАДР произошла ошибка!\n\n"
                                      + sql_houseguid + "\n\n");
                delete query_houseguid;
                db.rollback();
                return;
            }
            if (query_houseguid->next()) {
                fias_houseguid = query_houseguid->value(1).toString();
                f_save = true;
            }
            delete query_houseguid;
        }

        if ( f_save
             && !fias_aoguid.isEmpty()
             && !fias_houseguid.isEmpty()) {
            // сохраним найденные коды
            QString sql_save = "update addresses "
                               "   set fias_aoid='" + fias_aoguid + "', "
                               "       fias_houseid='" + fias_houseguid + "' "
                               " where id=" + QString::number(id) + " ; ";
            QSqlQuery *query_save = new QSqlQuery(db);
            if (!query_save->exec(sql_save)) {
                QMessageBox::critical(this, "Ошибка при сохранении кодов ФИАС ",
                                      "При сохранении кодов ФИАС произошла ошибка!\n\n"
                                      + sql_save + "\n\n");
                delete query_save;
                db.rollback();
                return;
            }
            delete query_save;
            n++;
        } else
        if ( f_save
             && !fias_aoguid.isEmpty()
             && fias_houseguid.isEmpty()) {
            // сохраним найденные коды
            QString sql_save = "update addresses "
                               "   set fias_aoid='" + fias_aoguid + "' "
                               " where id=" + QString::number(id) + " ; ";
            QSqlQuery *query_save = new QSqlQuery(db);
            if (!query_save->exec(sql_save)) {
                QMessageBox::critical(this, "Ошибка при сохранении кода fias_aoguid ",
                                      "При сохранении кода fias_aoguid произошла ошибка!\n\n"
                                      + sql_save + "\n\n");
                delete query_save;
                db.rollback();
                return;
            }
            delete query_save;
            n++;
        } else
        if ( f_save
             && fias_aoguid.isEmpty()
             && !fias_houseguid.isEmpty()) {
            // сохраним найденные коды
            QString sql_save = "update addresses "
                               "   set fias_houseid='" + fias_houseguid + "' "
                               " where id=" + QString::number(id) + " ; ";
            QSqlQuery *query_save = new QSqlQuery(db);
            if (!query_save->exec(sql_save)) {
                QMessageBox::critical(this, "Ошибка при сохранении кодоа fias_houseguid ",
                                      "При сохранении кодоа fias_houseguid произошла ошибка!\n\n"
                                      + sql_save + "\n\n");
                delete query_save;
                db.rollback();
                return;
            }
            delete query_save;
            n++;
        }
        if (n%21==0) {
            ui->lab_cnt_IDs->setText(QString::number(n));
            db.commit();
            db.transaction();
            QApplication::processEvents();
        }
    }
    delete query_adr;
    db.commit();
}
