#include "quest2fond_wnd.h"
#include "ui_quest2fond_wnd.h"
#include "persons_wnd.h"

quest2fond_wnd::quest2fond_wnd(QSqlDatabase &db, s_data_quest2fond &data_quest, s_data_app &data_app, QSettings &settings, persons_wnd *persons_w, QWidget *parent) :
    db(db),
    data_quest(data_quest),
    data_app(data_app),
    settings(settings),
    persons_w(persons_w),
    QDialog(parent),
    ui(new Ui::quest2fond_wnd)
{
    ui->setupUi(this);

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

    refresh_combo_doc_type();
    refresh_combo_pol_v();

    // вкладка "ЗАПРОСЫ"
    // заполним поля данными по умолчанию
    if (!(data_quest.fam.trimmed().isEmpty())) {
        ui->ln_fam->setText(data_quest.fam.trimmed());
        ui->ch_fam->setChecked(true);
    }
    if (!(data_quest.im.trimmed().isEmpty())) {
        ui->ln_im->setText(data_quest.im.trimmed());
        ui->ch_im->setChecked(true);
    }
    if (!(data_quest.ot.trimmed().isEmpty())) {
        ui->ln_ot->setText(data_quest.ot.trimmed());
        ui->ch_ot->setChecked(true);
    }
    if (data_quest.w==1 || data_quest.w==2) {
        ui->combo_sex->setCurrentIndex(data_quest.w);
        ui->ch_sex->setChecked(true);
    }
    if (data_quest.date_birth>QDate(1900,1,1) && data_quest.date_birth<=QDate::currentDate()) {
        ui->date_birth->setDate(data_quest.date_birth);
        ui->ch_date_birth->setChecked(true);
    }
    if (!(data_quest.snils.trimmed().simplified().replace(" ","").replace("-","").isEmpty())) {
        ui->ln_snils->setText(data_quest.snils.trimmed().simplified().replace(" ","").replace("-",""));
        ui->ch_snils->setChecked(true);
    }
    if (data_quest.doc_type>=0) {
        ui->combo_doc_type->setCurrentIndex(ui->combo_doc_type->findData(data_quest.doc_type));
        ui->ch_doc_type->setChecked(true);
    }
    if (!(data_quest.doc_ser.trimmed().replace("-"," ").simplified().isEmpty())) {
        ui->ln_doc_ser->setText(data_quest.doc_ser.trimmed().replace("-"," ").simplified());
        ui->ch_doc_ser->setChecked(true);
    }
    if (!(data_quest.doc_num.trimmed().replace(" ","").replace("-","").simplified().isEmpty())) {
        ui->ln_doc_num->setText(data_quest.doc_num.trimmed().replace("-","").replace(" ","").simplified());
        ui->ch_doc_num->setChecked(true);
    }
    if (data_quest.pol_v>=0) {
        ui->combo_pol_v->setCurrentIndex(ui->combo_pol_v->findData(data_quest.pol_v));
        ui->ch_pol_v->setChecked(true);
    }
    if (!(data_quest.pol_ser.trimmed().replace("-"," ").simplified().isEmpty())) {
        ui->ln_pol_ser->setText(data_quest.pol_ser.trimmed().replace("-"," ").simplified());
        ui->ch_pol_ser->setChecked(true);
    }
    if (!(data_quest.pol_num.trimmed().replace(" ","").replace("-","").simplified().isEmpty())) {
        ui->ln_pol_num->setText(data_quest.pol_num.trimmed().replace("-","").replace(" ","").simplified());
        ui->ch_pol_num->setChecked(true);
    }
    if (data_quest.date_begin>QDate(1900,1,1) && data_quest.date_begin<=QDate::currentDate()) {
        ui->date_begin->setDate(data_quest.date_begin);
        ui->ch_date_begin->setChecked(true);
    }
    if (data_quest.date_end>QDate(1900,1,1) && data_quest.date_end<=QDate::currentDate()) {
        ui->date_end->setDate(data_quest.date_end);
        ui->ch_date_end->setChecked(true);
    }
    if (!(data_quest.OGRNSMO.trimmed().replace(" ","").replace("-","").simplified().isEmpty())) {
        ui->ln_OGRNSMO->setText(data_quest.OGRNSMO.trimmed().replace("-","").replace(" ","").simplified());
        ui->ch_OGRNSMO->setChecked(true);
    }
    if (!(data_quest.QOGRN.trimmed().replace(" ","").replace("-","").simplified().isEmpty())) {
        ui->ln_QOGRN->setText(data_quest.QOGRN.trimmed().replace("-","").replace(" ","").simplified());
        ui->ch_QOGRN->setChecked(true);
    }
    if (data_quest.SUO>=0) {
        ui->double_SUO->setValue(data_quest.SUO);
        ui->ch_SUO->setChecked(true);
    }

    ui->ch_PERSON->setChecked(ui->ch_fam->isChecked() || ui->ch_im->isChecked() || ui->ch_ot->isChecked() || ui->ch_date_birth->isChecked() || ui->ch_sex->isChecked() || ui->ch_snils->isChecked());
    ui->ch_UDL->setChecked(ui->ch_doc_type->isChecked() || ui->ch_doc_ser->isChecked() || ui->ch_doc_num->isChecked());
    ui->ch_POLIS->setChecked(ui->ch_pol_v->isChecked() || ui->ch_pol_ser->isChecked() || ui->ch_pol_num->isChecked() || ui->ch_date_begin->isChecked() || ui->ch_date_end->isChecked());
    //ui->ch_SMO->setChecked(ui->ch_QOGRN->isChecked() || ui->ch_SUO->isChecked() || ui->ch_OGRNSMO->isChecked());
    ui->ch_SMO->setCheckState(Qt::Unchecked);

    on_ch_PERSON_clicked (ui->ch_PERSON->isChecked());
    on_ch_UDL_clicked (ui->ch_UDL->isChecked());
    on_ch_POLIS_clicked (ui->ch_POLIS->isChecked());
    on_ch_SMO_clicked (ui->ch_SMO->isChecked());

    // вкладка "ОТВЕТЫ"
    ui->date_now->setDate(QDate::currentDate());
    refresh_reply_tab();

    // переключим вкладку
    switch (data_quest.mode) {
    case 1:
        ui->pages_TFOMS->setCurrentIndex(0);
        break;
    case 2:
        ui->pages_TFOMS->setCurrentIndex(1);
        break;
    default:
        ui->pages_TFOMS->setCurrentIndex(1);
        break;
    }

    refresh_pack_tab();
}

quest2fond_wnd::~quest2fond_wnd() {
    delete ui;
}

void quest2fond_wnd::on_bn_close_clicked() {
    accept();
}

void quest2fond_wnd::refresh_combo_doc_type() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка типов документов
    QString sql = "select code, '('||code||') '||text, mask_ser, mask_num "
                  " from public.spr_f011 s "
                  " order by code ; ";
    // ==== //
    ui->te_log->clear();
    ui->te_log->append(sql);
    // ==== //

    QSqlQuery *query = new QSqlQuery(db);
    mySQL.exec(this, sql, QString("Справочник типов документов"), *query, true, db, data_app);
    ui->combo_doc_type->clear();
    ui->combo_doc_type->addItem(" - выберите тип документа - ", 0);
    while (query->next()) {
        ui->combo_doc_type->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_doc_type->setCurrentIndex(0);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void quest2fond_wnd::refresh_combo_pol_v() {
    this->setCursor(Qt::WaitCursor);
    // обновление списка форм изготовления полиса
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, '(' || code || ') ' || text "
                  " from public.spr_f008 s "
                  " order by code ; ";
    // ==== //
    ui->te_log->clear();
    ui->te_log->append(sql);
    // ==== //

    mySQL.exec(this, sql, QString("Обновление справочника типов полисов"), *query, true, db, data_app);
    ui->combo_pol_v->clear();
    ui->combo_pol_v->addItem(" - выберите тип полиса - ", 0);
    while (query->next()) {
        ui->combo_pol_v->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_pol_v->setCurrentIndex(0);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void quest2fond_wnd::on_ln_fam_editingFinished() {
    QString text = ui->ln_fam->text().trimmed().simplified();
    ui->ch_fam->setChecked(!(text.isEmpty()));
    ui->ln_fam->setText(text);
}
void quest2fond_wnd::on_ln_im_editingFinished() {
    QString text = ui->ln_im->text().trimmed().simplified();
    ui->ch_im->setChecked(!(text.isEmpty()));
    ui->ln_im->setText(text);
}
void quest2fond_wnd::on_ln_ot_editingFinished() {
    QString text = ui->ln_ot->text().trimmed().simplified();
    ui->ch_ot->setChecked(!(text.isEmpty()));
    ui->ln_ot->setText(text);
}
void quest2fond_wnd::on_combo_sex_activated(int index) {
    ui->ch_sex->setChecked(index>0);
}
void quest2fond_wnd::on_date_birth_dateChanged(const QDate &date) {
    ui->ch_date_birth->setChecked(date>QDate(1900,1,1));
    ui->date_birth->setEnabled(date>QDate(1900,1,1));
}
void quest2fond_wnd::on_ln_snils_editingFinished() {
    QString text = ui->ln_snils->text().trimmed().simplified().replace(" ","").replace("-","");
    ui->ch_snils->setChecked(!(text.length()>5));
    ui->ln_snils->setText(text);
}
void quest2fond_wnd::on_combo_doc_type_activated(int index) {
    ui->ch_doc_type->setChecked(index>0);
}
void quest2fond_wnd::on_ln_doc_ser_editingFinished() {
    QString text = ui->ln_doc_ser->text().trimmed().simplified();
    ui->ch_doc_ser->setChecked(!(text.isEmpty()));
    ui->ln_doc_ser->setText(text);
}
void quest2fond_wnd::on_ln_doc_num_editingFinished() {
    QString text = ui->ln_doc_num->text().trimmed().simplified().replace(" ","").replace("-","");
    ui->ch_doc_num->setChecked(!(text.isEmpty()));
    ui->ln_doc_num->setText(text);
}
void quest2fond_wnd::on_date_begin_dateChanged(const QDate &date) {
    ui->ch_date_begin->setChecked(date>QDate(1900,1,1));
    ui->date_begin->setEnabled(date>QDate(1900,1,1));
}
void quest2fond_wnd::on_date_end_dateChanged(const QDate &date) {
    ui->ch_date_end->setChecked(date>QDate(1900,1,1));
    ui->date_end->setEnabled(date>QDate(1900,1,1));
}
void quest2fond_wnd::on_ln_QOGRN_editingFinished() {
    QString text = ui->ln_QOGRN->text().trimmed().simplified().replace(" ","").replace("-","");
    ui->ch_QOGRN->setChecked(!(text.length()>5));
    ui->ln_QOGRN->setText(text);
}
void quest2fond_wnd::on_ln_OGRNSMO_editingFinished() {
    QString text = ui->ln_OGRNSMO->text().trimmed().simplified().replace(" ","").replace("-","");
    ui->ch_OGRNSMO->setChecked(!(text.length()>5));
    ui->ln_OGRNSMO->setText(text);
}
void quest2fond_wnd::on_ch_date_birth_clicked(bool checked) {
    ui->date_birth->setEnabled(checked);
}
void quest2fond_wnd::on_ch_date_begin_clicked(bool checked) {
    ui->date_begin->setEnabled(checked);
}
void quest2fond_wnd::on_ch_date_end_clicked(bool checked) {
    ui->date_end->setEnabled(checked);
}
void quest2fond_wnd::on_ch_SUO_clicked(bool checked) {
    ui->double_SUO->setEnabled(checked);
}

void quest2fond_wnd::on_ch_PERSON_clicked(bool checked) {
    ui->ch_fam->setEnabled(checked);
    ui->ch_im->setEnabled(checked);
    ui->ch_ot->setEnabled(checked);
    ui->ch_date_birth->setEnabled(checked);
    ui->ch_sex->setEnabled(checked);
    ui->ch_snils->setEnabled(checked);

    ui->ln_fam->setEnabled(checked);
    ui->ln_im->setEnabled(checked);
    ui->ln_ot->setEnabled(checked);
    ui->date_birth->setEnabled(checked && ui->ch_date_birth->isChecked());
    ui->combo_sex->setEnabled(checked);
    ui->ln_snils->setEnabled(checked);
}
void quest2fond_wnd::on_ch_UDL_clicked(bool checked) {
    ui->ch_doc_type->setEnabled(checked);
    ui->ch_doc_ser->setEnabled(checked);
    ui->ch_doc_num->setEnabled(checked);

    ui->combo_doc_type->setEnabled(checked);
    ui->ln_doc_ser->setEnabled(checked);
    ui->ln_doc_num->setEnabled(checked);
}
void quest2fond_wnd::on_ch_POLIS_clicked(bool checked) {
    ui->ch_pol_v->setEnabled(checked);
    ui->ch_pol_ser->setEnabled(checked);
    ui->ch_pol_num->setEnabled(checked);
    ui->ch_date_begin->setEnabled(checked);
    ui->ch_date_end->setEnabled(checked);

    ui->combo_pol_v->setEnabled(checked);
    ui->ln_pol_ser->setEnabled(checked);
    ui->ln_pol_num->setEnabled(checked);
    ui->date_begin->setEnabled(checked && ui->ch_date_begin->isChecked());
    ui->date_end->setEnabled(checked && ui->ch_date_end->isChecked());
}
void quest2fond_wnd::on_ch_SMO_clicked(bool checked) {
    ui->ch_QOGRN->setEnabled(checked);
    ui->ch_OGRNSMO->setEnabled(checked);
    ui->ch_SUO->setEnabled(checked);

    ui->ln_QOGRN->setEnabled(checked);
    ui->ln_OGRNSMO->setEnabled(checked);
    ui->double_SUO->setEnabled(checked && ui->ch_SUO->isChecked());
}



void quest2fond_wnd::refresh_pack_tab() {
    this->setCursor(Qt::WaitCursor);

    // проверим, есть ли файл посылки
    if (!QFile::exists(data_app.path_dbf + "q2fond.dbf"))
        QFile::copy(data_app.path_dbf + "q2fond_src.dbf",data_app.path_dbf + "q2fond.dbf");

    if (!db_ODBC.isOpen()) {
        QMessageBox::warning(this, "Нет доступа к базе данных",
                             "Нет открытого соединения к базе данных?\n "
                             "Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n "
                             "Закройте и снова откройте это окно. Если это сообщение будет повторяться - сообщите разработчику.");
        return;
    }

    QString pack_sql = "select * "
                        " from q2fond "
                        " order by fam, im, ot, dr ; ";

    model_pack.setQuery(pack_sql,db_ODBC);
    QString err2 = model_pack.lastError().driverText();

    // подключаем модель из БД
    ui->tab_pack->setModel(&model_pack);

    // обновляем таблицу
    ui->tab_pack->reset();

    // задаём ширину колонок
    ui->tab_pack->setColumnWidth( 3, 30);     // ,
    ui->tab_pack->setColumnWidth( 4, 80);     // ,
    // правим заголовки
    /* model_pack.setHeaderData( 5, Qt::Horizontal, ("фамилия"));
    model_pack.setHeaderData( 6, Qt::Horizontal, ("имя"));
    model_pack.setHeaderData( 7, Qt::Horizontal, ("отчество"));
    model_pack.setHeaderData( 8, Qt::Horizontal, ("пол"));
    model_pack.setHeaderData( 9, Qt::Horizontal, ("дата рожд."));
    model_pack.setHeaderData(10, Qt::Horizontal, ("код СМО"));
    model_pack.setHeaderData(11, Qt::Horizontal, ("СМО"));
    model_pack.setHeaderData(12, Qt::Horizontal, ("тип п"));
    model_pack.setHeaderData(13, Qt::Horizontal, ("сер пол"));
    model_pack.setHeaderData(14, Qt::Horizontal, ("номер полиса"));
    model_pack.setHeaderData(15, Qt::Horizontal, ("дата нач."));
    model_pack.setHeaderData(16, Qt::Horizontal, ("дата оконч."));
    model_pack.setHeaderData(17, Qt::Horizontal, ("ОКАТО")); */
    ui->tab_pack->repaint();

    this->setCursor(Qt::ArrowCursor);

    int cnt = model_pack.rowCount();
    ui->lab_cnt->setText(QString::number(cnt));
}


void quest2fond_wnd::on_bn_gen_dbf_clicked() {
    // проверка заполнения полей
    if (!(ui->ch_fam->isChecked())) {
        QMessageBox::warning(this, "Не хватает данных","Не заполнено или не выбрано обязательное поле \"фамилия\"");
        return;
    }
    if (!(ui->ch_im->isChecked())) {
        QMessageBox::warning(this, "Не хватает данных","Не заполнено или не выбрано обязательное поле \"имя\"");
        return;
    }
    if (!(ui->ch_sex->isChecked())) {
        QMessageBox::warning(this, "Не хватает данных","Не заполнено или не выбрано обязательное поле \"пол\"");
        return;
    }
    if (!(ui->ch_date_birth->isChecked())) {
        QMessageBox::warning(this, "Не хватает данных","Не заполнено или не выбрано обязательное поле \"дата рождения\"");
        return;
    }

    // это делается при открытии окна
    /*// подключимся к ODBC
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
    }*/

    /*// очистим посылку
    QString sql_clear = "delete from q2fond ";
    QSqlQuery *query_clear = new QSqlQuery(db_ODBC);
    bool res_clear = query_clear->exec(sql_clear);
    QString err = db_ODBC.lastError().databaseText();
    if (!res_clear) {
        delete query_clear;
        QMessageBox::warning(this, "Ошибка подготовки посылки",
                             "При попытке очистить посылку для заприса данных застрахованных произошла ошибка!\n\n"
                             "Операция отменена");
        db_ODBC.close();
        QSqlDatabase::removeDatabase("INKO_DBF");
        return;
    }
    delete query_clear;*/

    // добавим строку с данными запрашиваемого застрахованного
    QString sql_add = "insert into q2fond(fam, im, ot, w, dr, ss, doctype, docser, docnum, tpol, spolic, npolic, din, dout, qogrn, suo, ogrnsmo) "
                      " values (" +
                        ( (ui->ch_PERSON->isChecked() && ui->ch_fam->isChecked()) ? QString("'"+ ui->ln_fam->text().trimmed().simplified() +"'") : "NULL" ) + ", " +
                        ( (ui->ch_PERSON->isChecked() && ui->ch_im->isChecked()) ? QString("'"+ ui->ln_im->text().trimmed().simplified() +"'") : "NULL" ) + ", " +
                        ( (ui->ch_PERSON->isChecked() && ui->ch_ot->isChecked()) ? QString("'"+ ui->ln_ot->text().trimmed().simplified() +"'") : "NULL" ) + ", " +
                        ( (ui->ch_PERSON->isChecked() && ui->ch_sex->isChecked() && ui->combo_sex->currentIndex()>0) ? QString::number(ui->combo_sex->currentIndex()) : "NULL" ) + ", " +
                        ( (ui->ch_PERSON->isChecked() && ui->ch_date_birth->isChecked()) ? QString("'"+ ui->date_birth->date().toString("dd.MM.yyyy") +"'") : "NULL" ) + ", " +
                        ( (ui->ch_PERSON->isChecked() && ui->ch_snils->isChecked()) ? QString("'"+ ui->ln_snils->text().trimmed().simplified() +"'") : "NULL" ) + ", " +
                        ( (ui->ch_UDL->isChecked() && ui->ch_doc_type->isChecked() && ui->combo_doc_type->currentIndex()>0) ? QString::number(ui->combo_doc_type->currentData().toInt()) : "NULL" ) + ", " +
                        ( (ui->ch_UDL->isChecked() && ui->ch_doc_ser->isChecked()) ? QString("'"+ ui->ln_doc_ser->text().trimmed().simplified() +"'") : "NULL" ) + ", " +
                        ( (ui->ch_UDL->isChecked() && ui->ch_doc_num->isChecked()) ? QString("'"+ ui->ln_doc_num->text().trimmed().simplified() +"'") : "NULL" ) + ", " +
                        ( (ui->ch_POLIS->isChecked() && ui->ch_pol_v->isChecked() && ui->combo_pol_v->currentIndex()>0) ? QString::number(ui->combo_pol_v->currentData().toInt()) : "NULL" ) + ", " +
                        ( (ui->ch_POLIS->isChecked() && ui->ch_pol_ser->isChecked()) ? QString("'"+ ui->ln_pol_ser->text().trimmed().simplified() +"'") : "NULL" ) + ", " +
                        ( (ui->ch_POLIS->isChecked() && ui->ch_pol_num->isChecked()) ? QString("'"+ ui->ln_pol_num->text().trimmed().simplified() +"'") : "NULL" ) + ", " +
                        ( (ui->ch_POLIS->isChecked() && ui->ch_date_begin->isChecked()) ? QString("'"+ ui->date_begin->date().toString("dd.MM.yyyy") +"'") : "NULL" ) + ", " +
                        ( (ui->ch_POLIS->isChecked() && ui->ch_date_end->isChecked()) ? QString("'"+ ui->date_end->date().toString("dd.MM.yyyy") +"'") : "NULL" ) + ", " +
                        ( (ui->ch_SMO->isChecked() && ui->ch_QOGRN->isChecked()) ? QString("'"+ ui->ln_QOGRN->text().trimmed().simplified() +"'") : "NULL" ) + ", " +
                        ( (ui->ch_SMO->isChecked() && ui->ch_SUO->isChecked()) ? QString::number(ui->double_SUO->value()) : "NULL" ) + ", " +
                        ( (ui->ch_SMO->isChecked() && ui->ch_OGRNSMO->isChecked()) ? QString("'"+ ui->ln_OGRNSMO->text().trimmed().simplified() +"'") : "NULL" ) +
                      " ) ";
    // ==== //
    ui->te_log->clear();
    ui->te_log->append(sql_add);
    // ==== //

    QSqlQuery *query_add = new QSqlQuery(db_ODBC);
    bool res_add = query_add->exec(sql_add);
    QString err = db_ODBC.lastError().databaseText();
    if (!res_add) {
        delete query_add;
        QMessageBox::warning(this, "Ошибка добавления строки",
                             "При попытке добавить строку с данными запрашиваемого застрахованного произошла ошибка!\n\n"
                             "Операция отменена");
        refresh_pack_tab();
        return;
    }
    delete query_add;

    refresh_pack_tab();
    return;
}


void quest2fond_wnd::on_bn_clear_dbf_clicked() {
    // проверим число строк в посылке
    QString sql_test = "select count(*) as cnt from q2fond ";
    // ==== //
    ui->te_log->clear();
    ui->te_log->append(sql_test);
    // ==== //

    QSqlQuery *query_test = new QSqlQuery(db_ODBC);
    bool res_test = query_test->exec(sql_test);
    if (!res_test) {
        delete query_test;
        QMessageBox::warning(this, "Ошибка при подсчёте числа строк посылки",
                             "При подсчёте числа строк посылки произошла ошибка!\n\n"
                             "Операция отменена");
        refresh_pack_tab();
        return;
    }
    query_test->next();
    int n = query_test->value(0).toInt();
    delete query_test;
    if (n>0) {
        if (!QMessageBox::question(this, "Нужно подтверждение",
                                   "Вы действительно хотите удалить " + QString::number(n) + " строк послылки ?",
                                   QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel)==QMessageBox::Cancel) {
            QMessageBox::warning(this, "Отмена операции",
                                 "В посылке для ТФОМС осталось " + QString::number(n) + " строк.");
            refresh_pack_tab();
            return;

        } else {
            db_ODBC.close();
            QSqlDatabase::removeDatabase("INKO_DBF");
            QFile::remove(data_app.path_dbf + "q2fond.dbf");
            QFile::copy(data_app.path_dbf + "q2fond_src.dbf", data_app.path_dbf + "q2fond.dbf");

            accept();

            QMessageBox::warning(this, "Откройте окно ещё раз",
                                 "Для завершения операции откройте окно ещё раз.");
        }

    } else {
        db_ODBC.close();
        QSqlDatabase::removeDatabase("INKO_DBF");
        QFile::remove(data_app.path_dbf + "q2fond.dbf");
        QFile::copy(data_app.path_dbf + "q2fond_src.dbf", data_app.path_dbf + "q2fond.dbf");

        accept();

        QMessageBox::warning(this, "Откройте окно ещё раз",
                             "Для завершения операции откройте окно ещё раз.");
    }
}

void quest2fond_wnd::on_bn_send_to_TFOMS_clicked() {
    // скопируем файл DBF
    QString dbf_name = "SP" + QDate::currentDate().toString("yyMMdd");
    int n=1;
    while (QFile::exists(data_app.path_to_TFOMS + dbf_name  + QString::number(n) + ".dbf"))
        n++;
    QString dbf_file_name = data_app.path_to_TFOMS + dbf_name + QString::number(n) + ".dbf";

    // если надо - удалим старый файл
    QFile::remove(dbf_file_name);

    // скопируем новый файл
    QFile f(data_app.path_dbf + "q2fond.dbf");
    bool res_copy = f.copy(dbf_file_name);


    // почистим данные отосланной посылки
    if (res_copy) {
        // это делается при открытии окна
        /*// подключимся к ODBC
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
        }*/

        // удалим из посылки все данные
        QString sql_clesr = "delete from q2fond ";
        // ==== //
        ui->te_log->clear();
        ui->te_log->append(sql_clesr);
        // ==== //

        QSqlQuery *query_clear = new QSqlQuery(db_ODBC);
        bool res_clear = query_clear->exec(sql_clesr);
        if (!res_clear) {
            delete query_clear;
            QMessageBox::warning(this, "Ошибка очистки посылки",
                                 "При попытке очистить посылку для ТФОМС произошла ошибка!\n\n"
                                 "Операция отменена");
            refresh_pack_tab();
            return;
        }
        delete query_clear;

        QMessageBox::warning(this, "Посылка отправлена", "Послылка отправлена. \n\nШаблон посылки очищен.");
    }
    refresh_pack_tab();
}




void quest2fond_wnd::refresh_reply_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    QString reply_sql = "select id, dt_get, fam, im, ot, w, dr, rcod, rsmo, opdoc, spol, npol, din, dout, rokato, rqogrn, renp, ropdoc, rspol, rnpol, rdbeg, rdend, doctp, docs, docn, ss, qogrn, logrn, sm, eerp, repl, extid, rq, rprz, okato, enp, schet, addr, lpuhist, lpu, dstop, rstop, _id_person "
                        " from files_in_tfoms "
                        " where 1=1 ";
    if (ui->ch_filter_fio->isChecked()) {
        reply_sql   +=  "   and ((fam is NULL) or (fam like('" + ui->line_fam->text().trimmed().toUpper().simplified() + "%') )) ";
        reply_sql   +=  "   and ((im  is NULL) or (im  like('" + ui->line_im->text().trimmed().toUpper().simplified()  + "%') )) ";
        reply_sql   +=  "   and ((ot  is NULL) or (ot  like('" + ui->line_ot->text().trimmed().toUpper().simplified()  + "%') )) ";
    }
    if (ui->ch_filter_date->isChecked()) {
        reply_sql   +=  "   and cast(dt_get as date)='" + ui->date_now->date().toString("yyyy-MM-dd") + "' ";
    }
    reply_sql  +=  " order by fam, im, ot, dr ; ";

    model_reply.setQuery(reply_sql,db);
    QString err2 = model_reply.lastError().driverText();

    // подключаем модель из БД
    ui->tab_reply->setModel(&model_reply);

    // обновляем таблицу
    ui->tab_reply->reset();

    // задаём ширину колонок
    ui->tab_reply->setColumnWidth( 0,  1);     // id,
    ui->tab_reply->setColumnWidth( 1,  1);     // ,
    ui->tab_reply->setColumnWidth( 2,100);     // ,
    ui->tab_reply->setColumnWidth( 3, 80);     // ,
    ui->tab_reply->setColumnWidth( 4,100);     // ,
    ui->tab_reply->setColumnWidth( 5, 40);     // ,
    ui->tab_reply->setColumnWidth( 6, 75);     // ,
    ui->tab_reply->setColumnWidth( 7,  1);     // ,
    ui->tab_reply->setColumnWidth( 8,200);     // ,
    ui->tab_reply->setColumnWidth( 9, 40);     // ,
    ui->tab_reply->setColumnWidth(10, 60);     // ,
    ui->tab_reply->setColumnWidth(11,110);     // ,
    ui->tab_reply->setColumnWidth(12, 80);     // ,
    ui->tab_reply->setColumnWidth(13, 80);     // ,
    ui->tab_reply->setColumnWidth(14, 60);     // ,

    // правим заголовки
    model_reply.setHeaderData( 2, Qt::Horizontal, ("фамилия"));
    model_reply.setHeaderData( 3, Qt::Horizontal, ("имя"));
    model_reply.setHeaderData( 4, Qt::Horizontal, ("отчество"));
    model_reply.setHeaderData( 5, Qt::Horizontal, ("пол"));
    model_reply.setHeaderData( 6, Qt::Horizontal, ("дата рожд."));
    model_reply.setHeaderData( 7, Qt::Horizontal, ("код СМО"));
    model_reply.setHeaderData( 8, Qt::Horizontal, ("СМО"));
    model_reply.setHeaderData( 9, Qt::Horizontal, ("тип п"));
    model_reply.setHeaderData(10, Qt::Horizontal, ("сер пол"));
    model_reply.setHeaderData(11, Qt::Horizontal, ("номер полиса"));
    model_reply.setHeaderData(12, Qt::Horizontal, ("дата нач."));
    model_reply.setHeaderData(13, Qt::Horizontal, ("дата оконч."));
    model_reply.setHeaderData(14, Qt::Horizontal, ("ОКАТО"));
    ui->tab_reply->repaint();

    this->setCursor(Qt::ArrowCursor);
}

void quest2fond_wnd::on_bn_get_from_TFOMS_clicked() {
    // переберём файлы во входной папке с подходящим названием
    QDir dir_TFOMS(data_app.path_from_TFOMS);
    QStringList filters;
    filters << "out_SP*.dbf";
    
    QStringList fileList = dir_TFOMS.entryList(QDir::Files|QDir::Readable); //dir.entryList( QDir::Files );
    int fCnt = 0;
    for ( QStringList::Iterator fit = fileList.begin(); fit != fileList.end(); ++fit ) {
        QString TFOMS_dir = data_app.path_from_TFOMS;
        QString fname = QString(*fit).toLower().trimmed().simplified();

        // проверим имя файла
        if (fname.left(6)=="out_sp" && fname.right(4)==".dbf") {
            fCnt++;
            ui->lab_fCnt->setText(QString::number(fCnt));
            ui->lab_fName->setText(fname);

            // перенесём файл в папку _DBF_
            QString path_from = TFOMS_dir + fname;
            QString path_to = data_app.path_dbf + "ret4fond.dbf";
            QString path_backup = TFOMS_dir + "backup/" + fname;
            QFile f_rem(path_to);
            bool res_rem  = f_rem.remove();
            bool res_copy = QFile::copy(path_from, path_to);

            if (res_copy) {
                db.transaction();

                // прочитаем полученные данные
                QString sql_get = "select FAM, IM, OT, DR, W, OPDOC, SPOL, NPOL, DIN, DOUT, ROKATO, RENP, RQOGRN, ROPDOC, "
                                  "       RSPOL, RNPOL, RDBEG, RDEND, DOCTP, DOCS, DOCN, SS, QOGRN, LOGRN, SM, EERP, REPL, "
                                  "       EXTID, RQ, RPRZ, OKATO, ENP, SCHET, ADDR, LPUHIST, LPU, DSTOP, RSTOP, RSMO, RCOD "
                                  "from ret4fond ; ";
                QSqlQuery *query_get = new QSqlQuery(db_ODBC);
                // ==== //
                ui->te_log->clear();
                ui->te_log->append(sql_get);
                // ==== //

                bool res_get = query_get->exec(sql_get);
                QString err = db_ODBC.lastError().databaseText();
                if (!res_get) {
                    delete query_get;
                    QMessageBox::warning(this, "Ошибка чтения полученных из ТФОМС данных",
                                         "При попытке прочитать полученные из ТФОМС данные произошла ошибка!\n\n"
                                         "Операция отменена");
                    db.rollback();
                    return;
                }


                int num = 0;
                while (query_get->next()) {
                    num++;
                    ui->lab_lnNum->setText(QString::number(num));
                    QApplication::processEvents();


                    // поищем персону в таблице персон
                    QString sql_pers = "select id "
                                       "  from persons "
                                       " where fam" + QString(query_get->value( 0).isNull() ? " is NULL" : "='" + query_get->value( 0).toString() + "'") + " "
                                       "   and im"  + QString(query_get->value( 1).isNull() ? " is NULL" : "='" + query_get->value( 1).toString() + "'") + " "
                                       "   and ot"  + QString(query_get->value( 2).isNull() ? " is NULL" : "='" + query_get->value( 2).toString() + "'") + " "
                                       "   and date_birth='"+ query_get->value( 3).toDate().toString("yyyy-MM-dd") + "' ; ";
                    QSqlQuery *query_pers = new QSqlQuery(db);
                    // ==== //
                    ui->te_log->clear();
                    ui->te_log->append(sql_pers);
                    // ==== //

                    bool res_pers = query_pers->exec(sql_pers);
                    QString err2 = db.lastError().databaseText();
                    /*if (!res_pers) {
                        delete query_pers;
                        QMessageBox::warning(this, "Ошибка поиска персоны в таблице персон",
                                             "При попытке найти персону в таблице персон произошла ошибка!\n\n"
                                             "Операция отменена");
                        db.rollback();
                        return;
                    }*/
                    int id_pers = -1;
                    if (query_pers->next())
                        id_pers = query_pers->value(0).toInt();


                    QDate rdbeg = query_get->value(16).toDate();
                    QDate rdend = query_get->value(17).toDate();
                    QString sql_add = "insert into files_in_tfoms"
                                      "            ( dt_get, "
                                      "              /*zid, pid, recid,*/ fam, im, ot, dr, w, opdoc, spol, npol, din, dout, "
                                      "              rokato, renp, rqogrn, ropdoc, rspol, rnpol, rdbeg, rdend, "
                                      "              doctp, docs, docn, ss, qogrn, logrn, sm, eerp, repl, extid, rq, rprz, "
                                      "              okato, enp, schet, addr, lpuhist, lpu, dstop, rstop, rsmo, rcod, _id_person) "
                                      " values (CURRENT_TIMESTAMP, " +                                                                             // date_get,
                                                //QString(query_get->value( 0).isNull() ? "NULL" : QString::number(query_get->value( 0).toInt())) + ", " +                   // zid
                                                //QString(query_get->value( 1).isNull() ? "NULL" : QString::number(query_get->value( 1).toInt())) + ", " +                   // pid
                                                //QString(query_get->value( 2).isNull() ? "NULL" : "'" + query_get->value( 2).toString() + "'") + ", " +                     // recid
                                                QString(query_get->value( 0).isNull() ? "NULL" : "'" + query_get->value( 0).toString() + "'") + ", " +                     // fam
                                                QString(query_get->value( 1).isNull() ? "NULL" : "'" + query_get->value( 1).toString() + "'") + ", " +                     // im
                                                QString(query_get->value( 2).isNull() ? "NULL" : "'" + query_get->value( 2).toString() + "'") + ", " +                     // ot
                                                QString(query_get->value( 3).isNull() ? "NULL" : "'" + query_get->value( 3).toDate().toString("yyyy-MM-dd") + "'") + ", " +// dr
                                                QString(query_get->value( 4).isNull() ? "NULL" : QString::number(query_get->value( 4).toInt())) + ", " +                   // w
                                                QString(query_get->value( 5).isNull() ? "NULL" : "'" + query_get->value( 5).toString() + "'") + ", " +                     // opdoc
                                                QString(query_get->value( 6).isNull() ? "NULL" : "'" + query_get->value( 6).toString() + "'") + ", " +                     // spol
                                                QString(query_get->value( 7).isNull() ? "NULL" : "'" + query_get->value( 7).toString() + "'") + ", " +                     // npol
                                                QString(query_get->value( 8).isNull() ? "NULL" : "'" + query_get->value( 8).toDate().toString("yyyy-MM-dd") + "'") + ", " +// din
                                                QString(query_get->value( 9).isNull() ? "NULL" : "'" + query_get->value( 9).toDate().toString("yyyy-MM-dd") + "'") + ", " +// dout
                                                QString(query_get->value(10).isNull() ? "NULL" : "'" + query_get->value(10).toString() + "'") + ", " +                     // rokato
                                                QString(query_get->value(11).isNull() ? "NULL" : "'" + query_get->value(11).toString() + "'") + ", " +                     // renp
                                                QString(query_get->value(12).isNull() ? "NULL" : "'" + query_get->value(12).toString() + "'") + ", " +                     // rqogrn
                                                QString(query_get->value(13).isNull() ? "NULL" : "'" + query_get->value(13).toString() + "'") + ", " +                     // ropdoc
                                                QString(query_get->value(14).isNull() ? "NULL" : "'" + query_get->value(14).toString() + "'") + ", " +                     // rspol
                                                QString(query_get->value(15).isNull() ? "NULL" : "'" + query_get->value(15).toString() + "'") + ", " +                     // rnpol
                                                QString(query_get->value(16).isNull() ? "NULL" : "'" + query_get->value(16).toDate().toString("yyyy-MM-dd") + "'") + ", " +// rdbeg
                                                QString(query_get->value(17).isNull() ? "NULL" : "'" + query_get->value(17).toDate().toString("yyyy-MM-dd") + "'") + ", " +// rdend
                                                QString(query_get->value(18).isNull() ? "NULL" : "'" + query_get->value(18).toString() + "'") + ", " +                     // doctp
                                                QString(query_get->value(19).isNull() ? "NULL" : "'" + query_get->value(19).toString() + "'") + ", " +                     // docs
                                                QString(query_get->value(20).isNull() ? "NULL" : "'" + query_get->value(20).toString() + "'") + ", " +                     // docn
                                                QString(query_get->value(21).isNull() ? "NULL" : "'" + query_get->value(21).toString() + "'") + ", " +                     // ss
                                                QString(query_get->value(22).isNull() ? "NULL" : "'" + query_get->value(22).toString() + "'") + ", " +                     // qogrn
                                                QString(query_get->value(23).isNull() ? "NULL" : "'" + query_get->value(23).toString() + "'") + ", " +                     // logrn
                                                QString(query_get->value(24).isNull() ? "NULL" : QString::number(query_get->value(24).toInt())) + ", " +                   // sm
                                                QString(query_get->value(25).isNull() ? "NULL" : "'" + query_get->value(25).toString() + "'") + ", " +                     // eerp
                                                QString(query_get->value(26).isNull() ? "NULL" : "'" + query_get->value(26).toString() + "'") + ", " +                     // repl
                                                QString(query_get->value(27).isNull() ? "NULL" : "'" + query_get->value(27).toString() + "'") + ", " +                     // extid
                                                QString(query_get->value(28).isNull() ? "NULL" : "'" + query_get->value(28).toString() + "'") + ", " +                     // rq
                                                QString(query_get->value(29).isNull() ? "NULL" : "'" + query_get->value(29).toString() + "'") + ", " +                     // rprz
                                                QString(query_get->value(30).isNull() ? "NULL" : "'" + query_get->value(30).toString() + "'") + ", " +                     // okato
                                                QString(query_get->value(31).isNull() ? "NULL" : "'" + query_get->value(31).toString() + "'") + ", " +                     // enp
                                                QString(query_get->value(32).isNull() ? "NULL" : "'" + query_get->value(32).toString() + "'") + ", " +                     // schet
                                                QString(query_get->value(33).isNull() ? "NULL" : "'" + query_get->value(33).toString() + "'") + ", " +                     // addr
                                                QString(query_get->value(34).isNull() ? "NULL" : "'" + query_get->value(34).toString() + "'") + ", " +                     // lpuhist
                                                QString(query_get->value(35).isNull() ? "NULL" : "'" + query_get->value(35).toString() + "'") + ", " +                     // lpu
                                                QString(query_get->value(36).isNull() ? "NULL" : "'" + query_get->value(36).toDate().toString("yyyy-MM-dd") + "'") + ", " +//dstop
                                                QString(query_get->value(37).isNull() ? "NULL" : QString::number(query_get->value(37).toInt())) + ", " +                   // rstop
                                                QString(query_get->value(38).isNull() ? "NULL" : "'" + query_get->value(38).toString() + "'") + ", " +                     // rsmo
                                                QString(query_get->value(39).isNull() ? "NULL" : "'" + query_get->value(39).toString() + "'") + ", " +                     // rcod
                                                QString(id_pers<0 ? "NULL" : QString::number(id_pers)) + ") ; ";                                                           // _id_person
                    // ==== //
                    ui->te_log->clear();
                    ui->te_log->append(sql_add);
                    // ==== //

                    QSqlQuery *query_add = new QSqlQuery(db);
                    bool res_add = query_add->exec(sql_add);
                    QString err = db.lastError().databaseText();
                    if (!res_add) {
                        delete query_add;
                        delete query_get;
                        QMessageBox::warning(this, "Ошибка сохранения данных, полученных из ТФОМС",
                                             "При попытке прочитать данные, полученные из ТФОМС, произошла ошибка!\n\n"
                                             "Операция отменена");
                        db.rollback();
                        return;
                    }
                }
                delete query_get;
                db.commit();

                // перенесём этот файл в папку резервного копирования
                QFile f_rem2(path_backup);
                bool res_rem2  = f_rem2.remove();
                bool res_copy2 = QFile::copy(path_from, path_backup);
                QFile::remove(path_from);
            }
        }
    }

    refresh_reply_tab();
    QMessageBox::information(this, "Готово", "Все данные загружены.");
}

void quest2fond_wnd::on_date_now_dateChanged(const QDate &date) {
    refresh_reply_tab();
}

void quest2fond_wnd::on_bn_today_clicked() {
    ui->date_now->setDate(QDate::currentDate());
}

void quest2fond_wnd::on_quest2fond_wnd_accepted() {
    db_ODBC.close();
    QSqlDatabase::removeDatabase("INKO_DBF");
}
/*void quest2fond_wnd::on_quest2fond_wnd_finished(int result) {
    db_ODBC.close();
    QSqlDatabase::removeDatabase("INKO_DBF");
}
void quest2fond_wnd::on_quest2fond_wnd_rejected() {
    db_ODBC.close();
    QSqlDatabase::removeDatabase("INKO_DBF");
}
void quest2fond_wnd::on_quest2fond_wnd_destroyed() {
    db_ODBC.close();
    QSqlDatabase::removeDatabase("INKO_DBF");
}*/

void quest2fond_wnd::on_ch_filter_fio_clicked(bool checked) {
    ui->line_fam->setEnabled(checked);
    ui->line_im->setEnabled(checked);
    ui->line_ot->setEnabled(checked);
    refresh_reply_tab();
}
void quest2fond_wnd::on_ch_filter_date_clicked(bool checked) {
    ui->date_now->setEnabled(checked);
    ui->bn_today->setEnabled(checked);
    refresh_reply_tab();
}

void quest2fond_wnd::on_line_fam_editingFinished() {
    ui->line_fam->setText(ui->line_fam->text().trimmed().toUpper().simplified());
    refresh_reply_tab();
}
void quest2fond_wnd::on_line_im_editingFinished() {
    ui->line_im->setText(ui->line_im->text().trimmed().toUpper().simplified());
    refresh_reply_tab();
}
void quest2fond_wnd::on_line_ot_editingFinished() {
    ui->line_ot->setText(ui->line_ot->text().trimmed().toUpper().simplified());
    refresh_reply_tab();
}

void quest2fond_wnd::on_bn_clear_tab_clicked() {
    if (QMessageBox::question(this, "Нужно подтверждение",
                              "Вы действительно хотите очистить всю историю сообщений от ТФОМС о статусе застрахованных?",
                              QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel)==QMessageBox::Cancel) {
        return;
    }
    // добавим строку с данными запрашиваемого застрахованного
    QString sql_clear = "delete from files_in_tfoms ; ";
    // ==== //
    ui->te_log->clear();
    ui->te_log->append(sql_clear);
    // ==== //

    QSqlQuery *query_clear = new QSqlQuery(db);
    bool res_clear = query_clear->exec(sql_clear);
    QString err = db.lastError().databaseText();
    if (!res_clear) {
        delete query_clear;
        QMessageBox::warning(this, "Ошибка очистки таблицы сообщений от ТФОМС",
                             "При попытке очистить таблицу сообщений от ТФОМС произошла ошибка!\n\n"
                             "Операция отменена");
        refresh_reply_tab();
        return;
    }
    delete query_clear;

    QMessageBox::information(this, "Таблица очищена", "Все данные удалены.");
    refresh_reply_tab();
    return;
}

void quest2fond_wnd::on_bn_to_pers_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_reply|| !ui->tab_reply->selectionModel()) {
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_reply->selectionModel()->selection().indexes();

    int id_person = -1;

    if (indexes.size()>0) {
        QModelIndex index = indexes.front();

        if (!model_reply.data(model_reply.index(index.row(), 42), Qt::EditRole).isNull()) {
            id_person = model_reply.data(model_reply.index(index.row(), 42), Qt::EditRole).toInt();
            persons_w->select_person_in_tab(id_person);

            accept();
            return;
        } else {
            QMessageBox::warning(this, "Запись не найдена",
                                 "Запись выбранной персоны не найдена в нашей базе данных.");
            return;
        }

    } else {
        QMessageBox::warning(this, "Ничего не выбрано",
                             "Выберите строку персоны, к записи которой хотите перейти.");
        return;
    }
}

void quest2fond_wnd::on_tab_reply_activated(const QModelIndex &index) {
    on_bn_to_pers_clicked();
}
