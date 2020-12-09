#include "add_blanks_box_wnd.h"
#include "ui_add_blanks_box_wnd.h"

add_blanks_box_wnd::add_blanks_box_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent)
 : db(db), data_app(data_app), settings(settings), QDialog(parent), ui(new Ui::add_blanks_box_wnd)
{
    ui->setupUi(this);

    ui->line_head_act_num->setText("");
    ui->date_head_act->setDate(QDate::currentDate());
    //ui->line_point_act_num->setText("");
    //ui->date_point_act->setDate(QDate::currentDate());

    // ---------------------
    // выберем головной офис
    refresh_heads();
    // восстановим прежнее значение
    if (settings.value("polis_acts/head_regnum", "").toString()!="") {
        data_app.head_act_regnum = QString::fromUtf8(settings.value("polis_acts/head_regnum", " ").toByteArray());
        ui->combo_head->setCurrentIndex(head_regnums.indexOf(data_app.head_act_regnum));
    } else
        ui->combo_head->setCurrentIndex(0);

    // ----------------------------------
    // выберем сотрудника головного офиса
    refresh_head_operators(ui->combo_head->currentData().toInt());
    // восстановим прежнее значение
    if (settings.value("polis_acts/head_obtainer", "").toString()!="") {
        data_app.head_act_obtainer = QString::fromUtf8(settings.value("polis_acts/head_obtainer", " ").toByteArray());
        ui->combo_head_obtainer->setCurrentIndex(ui->combo_head_obtainer->findText(data_app.head_act_obtainer));
    } else
        ui->combo_head_obtainer->setCurrentIndex(0);

    // -----------
    // выберем ПВП
    refresh_points();
    // восстановим прежнее значение
    /*if (settings.value("polis_acts/point_regnum", "").toString()!="") {
        data_app.point_act_regnum = QString::fromUtf8(settings.value("polis_acts/point_regnum", " ").toByteArray());
        ui->combo_point->setCurrentIndex(ui->combo_point->findText(data_app.point_act_regnum));
    } else
        ui->combo_point->setCurrentIndex(0);*/

    // ----------------------
    // выберем сотрудника ПВП
    /*refresh_point_operators(ui->combo_point->currentData().toInt());
    // восстановим прежнее значение
    if (settings.value("polis_acts/point_obtainer", "").toString()!="") {
        data_app.point_act_obtainer = QString::fromUtf8(settings.value("polis_acts/point_obtainer", " ").toByteArray());
        ui->combo_point_obtainer->setCurrentIndex(ui->combo_point_obtainer->findText(data_app.point_act_obtainer));
    } else {
        ui->combo_point_obtainer->setCurrentIndex(0);
    }*/
}

add_blanks_box_wnd::~add_blanks_box_wnd() {
    delete ui;
}

void add_blanks_box_wnd::on_bn_cansel_clicked() {
    reject();
}

void add_blanks_box_wnd::refresh_heads() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка ПВП для выбора головного офиса
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select id, '('||point_regnum||')  '||point_name as name, point_regnum "
                  "  from public.points "
                  " where status=1 and point_regnum<>'000' "
                  "   and point_rights=2 "
                  " order by point_regnum ; ";
    mySQL.exec(this, sql, QString("Список ПВП"), *query, true, db, data_app);
    ui->combo_head->clear();
    head_regnums.clear();
    ui->combo_head->addItem(" - выберите головной офис - ", -1);
    head_regnums.append("000");
    while (query->next()) {
        ui->combo_head->addItem(query->value(1).toString(), query->value(0).toString());
        head_regnums.append(query->value(2).toString());
    }
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void add_blanks_box_wnd::refresh_head_operators(int id_head) {
    this->setCursor(Qt::WaitCursor);

    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "";
    sql += "select u.id, u.oper_fio "
           " from operators u "
           " where u.status>0 ";
    sql += (id_head==-1) ? (QString(" ")) : (QString(" and u.id_point=") + QString::number(id_head));
    sql += " order by u.oper_fio ; ";

    query->exec( sql );
    ui->combo_head_obtainer->clear();
    ui->combo_head_obtainer->addItem(" - выберите работника - ", -1);

    // заполнение списка операторов
    while (query->next()) {
        ui->combo_head_obtainer->addItem( query->value(1).toString(),
                                     query->value(0).toInt() );
    }
    query->finish();
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void add_blanks_box_wnd::refresh_points() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка ПВП для выбора головного офиса
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select id, '('||point_regnum||')  '||point_name as name, point_regnum "
                  " from public.points "
                  " where status=1 and point_regnum<>'000' "
                  " order by point_regnum ; ";
    mySQL.exec(this, sql, QString("Список ПВП"), *query, true, db, data_app);
    //ui->combo_point->clear();
    point_regnums.clear();
    //ui->combo_point->addItem(" - выберите ПВП - ", -1);
    point_regnums.append("000");
    /*while (query->next()) {
        ui->combo_point->addItem(query->value(1).toString(), query->value(0).toString());
        point_regnums.append(query->value(2).toString());
    }
    ui->combo_point->setCurrentIndex(ui->combo_point->findData(data_app.point_regnum));
    refresh_point_operators(ui->combo_point->currentData().toInt());*/
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void add_blanks_box_wnd::refresh_point_operators(int id_point) {
    this->setCursor(Qt::WaitCursor);

    /*QSqlQuery *query = new QSqlQuery(db);
    QString sql = "";
    sql += "select u.id, u.oper_fio "
           " from operators u "
           " where u.status>0 ";
    sql += (id_point==-1) ? (QString(" ")) : (QString(" and u.id_point=") + QString::number(id_point));
    sql += " order by u.oper_fio ; ";

    query->exec( sql );
    ui->combo_point_obtainer->clear();
    ui->combo_point_obtainer->addItem(" - выберите работника - ", -1);

    // заполнение списка операторов
    while (query->next()) {
        ui->combo_point_obtainer->addItem( query->value(1).toString(),
                                     query->value(0).toInt() );
    }
    query->finish();
    delete query;*/

    this->setCursor(Qt::ArrowCursor);
}

void add_blanks_box_wnd::on_combo_head_currentIndexChanged(int index) {
    int i = ui->combo_head->currentIndex();
    if (i>=0 && i<head_regnums.size()) {
        data_app.head_act_regnum = head_regnums.at(i);
        refresh_head_operators(ui->combo_head->currentData().toInt());
    } else {
        refresh_head_operators(-1);
    }
}

void add_blanks_box_wnd::on_combo_point_activated(int index) {
    data_app.head_act_obtainer = ui->combo_head_obtainer->currentText();
    //refresh_point_operators(ui->combo_point->currentData().toInt());
}

void add_blanks_box_wnd::on_bn_process_clicked() {
    this->setCursor(Qt::WaitCursor);

    // проверим заполнение полей
    if (ui->spin_box->value()<=10000) {
        QMessageBox::critical(this, "Введите номер короба", "Номер короба не может быть меньше 10001.\n");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    if (ui->combo_head->currentIndex()<1) {
        QMessageBox::critical(this, "Выберите головной офис", "Выберите головной офис филиала.\n");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    if (ui->combo_head_obtainer->currentIndex()<1) {
        QMessageBox::critical(this, "Выберите ответственного работника головного офиса", "Выберите ответственного работника головного офиса.\n");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    if (ui->line_head_act_num->text().isEmpty()) {
        QMessageBox::critical(this, "Введите номер акта на получение короба", "Введите номер акта, по которому получен этот короб.\n");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    if ( ui->date_head_act->date()<QDate(2014,1,1) ||
         ui->date_head_act->date()>QDate::currentDate() ) {
        QMessageBox::critical(this, "Введите дату акта на получение короба", "Дата акта на получение короба не может быть раньше 01.01.2014 или позжае текущего дня.\n");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    /*if (ui->combo_point->currentIndex()<1) {
        QMessageBox::critical(this, "Выберите целевой ПВП", "Выберите ПВП, для которого предназначены сканируемые полисы.\n\nЕсли в коробе содержатся полисы для нескольких ПВП - \nони сканируются по отельности.\n");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    if (ui->combo_point_obtainer->currentIndex()<1) {
        QMessageBox::critical(this, "Выберите ответственного работника ПВП", "Выберите ответственного работника ПВП, который должен расписаться за получение отсканированных бланков.\n");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    if (ui->line_point_act_num->text().isEmpty()) {
        QMessageBox::critical(this, "Введите номер акта для ПВП", "Введите номер акта, по которому полисы из этого короба будут переданы на ПВП.\n");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    if ( ui->date_point_act->date()<QDate::currentDate() ||
         ui->date_point_act->date()>QDate::currentDate().addDays(7) ) {
        QMessageBox::critical(this, "Введите дату акта для ПВП", "Дата акта передачи короба на ПВП не может быть раньше текущего дня или позжае текущего дня +7 дней.\n");
        this->setCursor(Qt::ArrowCursor);
        return;
    }*/


    db.transaction();

    // запомним настройки в INI-файле
    settings.beginGroup("polis_acts");
    settings.setValue("head_regnum", data_app.head_act_regnum);
    settings.setValue("head_obtainer", data_app.head_act_obtainer);
    /*settings.setValue("point_regnum", data_app.point_act_regnum);
    settings.setValue("point_obtainer", data_app.point_act_obtainer);*/
    settings.endGroup();
    settings.sync();

    // ДОБАВЛИМ ЗАПИСЬ АКТА НА ПОЛУЧЕНИЕ КОРОБА
    int id_blanks_box_act = -1;

    // проверим, есть ли в базе такой акт
    QSqlQuery *query_act = new QSqlQuery(db);
    QString sql_act = "select id "
                      " from blanks_boxes_acts a "
                      " where act_num='" + ui->line_head_act_num->text() + "' "
                      "   and id_head_point=" + QString::number(ui->combo_head->currentData().toInt()) + "; ";
    if (mySQL.exec(this, sql_act, QString("проверим наличие акта на получение короба в СМО"), *query_act, true, db, data_app)) {
        if (query_act->next()) {
            id_blanks_box_act = query_act->value(0).toInt();

        } else {
            // подготовим новую запись акта приёмки
            QSqlQuery *query_boxx = new QSqlQuery(db);
            QString sql_boxx = "insert into blanks_boxes_acts(box_num, id_head_point, date_add, act_num, act_date, id_obtainer) "
                               " values (" + QString::number(ui->spin_box->value()) + ","
                               "         " + QString::number(ui->combo_head->currentData().toInt()) + ","
                               "         CURRENT_DATE,"
                               "         '" + ui->line_head_act_num->text() + "',"
                               "         '" + ui->date_head_act->date().toString("yyyy-MM-dd") + "',"
                               "         " + QString::number(ui->combo_head_obtainer->currentData().toInt()) + ") "
                               " returning id ; ";

            if (!mySQL.exec(this, sql_boxx, QString("добавим новый акт на получение бланков полисов в СМО"), *query_boxx, true, db, data_app)) {
                QMessageBox::critical(this, "Ошибка добавления акта приёмки",
                                      "При попытке добавить акт приёмки бланков полисов произошла непредвиденная ошибка.\n"
                                      "Операция отменена.\n");
                db.rollback();
                this->setCursor(Qt::ArrowCursor);
                return;
            }
            query_boxx->next();
            id_blanks_box_act = query_boxx->value(0).toInt();
        }

    } else {
        db.rollback();
        this->setCursor(Qt::ArrowCursor);
        QMessageBox::critical(this, "Ошибка проверки наличия акта приёмки",
                              "При попытке проверить существование ранее введённых данных нового акта приёмки произошла непредвиденная ошибка.\n"
                              "Операция отменена.\n");
        return;
    }

    /*// ДОБАВЛИМ ЗАПИСЬ АКТА НА передачу КОРОБА на ПВП

    // проверим, есть ли в базе такой акт
    QSqlQuery *query_act2 = new QSqlQuery(db);
    QString sql_act2 = "select count(*) "
                       "  from blanks_pol_acts a "
                       "       join blanks_pol bp on(bp.id_blanks_pol_act=a.id) "
                       " where a.act_num='" + ui->line_point_act_num->text() + "' "
                       "   and a.id_point=" + QString::number(ui->combo_point->currentData().toInt()) + "; ";
    if (mySQL.exec(this, sql_act2, QString("проверим наличие акта на передачу бланков на ПВП"), *query_act2, true, db, data_app) && query_act2->next()) {
        int n = query_act2->value(0).toInt();

        if (n>0) {
            QMessageBox::warning(this, "Такой акт уже есть",
                                 "В базе данных уже есть данные передаточного акта с таким номером для этого ПВП.\n"
                                 "По акту оформлена передача " + QString::number(n) + " бланков полисов.\n\n"
                                 "Операция отменена.\n");
            db.rollback();
            this->setCursor(Qt::ArrowCursor);
            return;
        }
    } else {
        db.rollback();
        this->setCursor(Qt::ArrowCursor);
        QMessageBox::critical(this, "Ошибка проверки наличия передаточного акта",
                              "При попытке проверить существование ранее введённых данных передаточного акта произошла непредвиденная ошибка.\n"
                              "Операция отменена.\n");
        return;
    }

    // подготовим новую запись акта передачи на ПВП
    QSqlQuery *query_pols = new QSqlQuery(db);
    QString sql_pols = "delete from blanks_pol_acts "
                       " where act_num='" + ui->line_point_act_num->text() + "' ; "
                       "insert into blanks_pol_acts(id_point, date_add, act_num, act_date, id_obtainer) "
                       " values (" + QString::number(ui->combo_point->currentData().toInt()) + ","
                       "         CURRENT_DATE,"
                       "         '" + ui->line_point_act_num->text() + "',"
                       "         '" + ui->date_point_act->date().toString("yyyy-MM-dd") + "',"
                       "         " + QString::number(ui->combo_point_obtainer->currentData().toInt()) + ") "
                       " returning id ; ";

    if (!mySQL.exec(this, sql_pols, QString("добавим новый акт передачи бланков полисов на ПВП"), *query_pols, true, db, data_app)) {
        QMessageBox::critical(this, "Ошибка добавления акта передачи бланков полисов на ПВП",
                              "При попытке добавить акт передачи бланков полисов на ПВП произошла непредвиденная ошибка.\n"
                              "Операция отменена.\n");
        db.rollback();
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    query_pols->next();
    int id_blanks_pol_act = query_pols->value(0).toInt();
    */

    // разбор строк данных
    int n_read=0, n_add=0;
    QString s_data = ui->plainTextEdit->toPlainText();
    QString s_enp = "";
    QString s_pol_ser = "";
    QString s_pol_num = "";
    QString s_date_order = "";
    QString s_date_product = "";
    QDate date_order;
    QDate date_product;
    int pos = 0;
    int state = 0;  // 0 - читаю ЕНП
                    // 1 - пропуск
                    // 2 - серия/номер полиса
                    // 3 - пропуск
                    // 4 - дата заказа
                    // 5 - пропуск
                    // 6 - дата выпуска
                    // 7 - пропуск
    while (pos<s_data.length()) {
        switch (state) {
        case 0:
            if (s_data.at(pos)>=QChar('0') && s_data.at(pos)<=QChar('9')) {
                s_enp += s_data.at(pos);
            } else {
                n_read++;
                state = 1;
                continue;
            }
            break;
        case 1:
            if (s_data.at(pos)>=QChar('0') && s_data.at(pos)<=QChar('9')) {
                state = 2;
                continue;
            }
            break;
        case 2:
            if (s_data.at(pos)>=QChar('0') && s_data.at(pos)<=QChar('9')) {
                if ( s_pol_ser.length()<4 )
                     s_pol_ser += s_data.at(pos);
                else s_pol_num += s_data.at(pos);
            } else {
                state = 3;
                continue;
            }
            break;
        case 3:
            if (s_data.at(pos)>=QChar('0') && s_data.at(pos)<=QChar('9')) {
                state = 4;
                continue;
            }
            break;
        case 4:
            if ( (s_data.at(pos)>=QChar('0') && s_data.at(pos)<=QChar('9')) || s_data.at(pos)==QChar('.') ) {
                s_date_order += s_data.at(pos);
            } else {
                state = 5;
                continue;
            }
            break;
        case 5:
            if (s_data.at(pos)>=QChar('0') && s_data.at(pos)<=QChar('9')) {
                state = 6;
                continue;
            }
            break;
        case 6:
            if ( (s_data.at(pos)>=QChar('0') && s_data.at(pos)<=QChar('9')) || s_data.at(pos)==QChar('.') ) {
                s_date_product += s_data.at(pos);
            } else {
                state = 7;
                continue;
            }
            break;
        case 7:
            if (s_data.at(pos)>=QChar('0') && s_data.at(pos)<=QChar('9')) {
                state = 0;
                continue;
            }
            break;
        }

        if (state==7) {
            // проверим корректность полученных данных
            if (s_enp.length()!=16) {
                QMessageBox::critical(this, "Ошибка в ЕНП",
                                      "ЕНП должен состоять из 16 цифр.\nОбнаружено:\n"
                                      "  ЕНП   = " + s_enp + "\n"
                                      "  серия = " + s_pol_ser + "\n"
                                      "  номер = " + s_pol_num + "\n"
                                      "  дата  = " + s_date_order + "\n\n"
                                      "Операция отменена.\n");
                db.rollback();
                this->setCursor(Qt::ArrowCursor);
                return;
            }
            if (s_pol_ser.length()!=4) {
                QMessageBox::critical(this, "Ошибка в серии полиса",
                                      "Серия полиса должна состоять из 4 цифр.\nОбнаружено:\n"
                                      "  ЕНП   = " + s_enp + "\n"
                                      "  серия = " + s_pol_ser + "\n"
                                      "  номер = " + s_pol_num + "\n"
                                      "  дата  = " + s_date_order + "\n\n"
                                      "Операция отменена.\n");
                db.rollback();
                this->setCursor(Qt::ArrowCursor);
                return;
            }
            if (s_pol_num.length()<4) {
                QMessageBox::critical(this, "Ошибка в номере полиса",
                                      "Номер полиса должен содержать не менее 4 цифр.\nОбнаружено:\n"
                                      "  ЕНП   = " + s_enp + "\n"
                                      "  серия = " + s_pol_ser + "\n"
                                      "  номер = " + s_pol_num + "\n"
                                      "  дата  = " + s_date_order + "\n\n"
                                      "Операция отменена.\n");
                db.rollback();
                this->setCursor(Qt::ArrowCursor);
                return;
            }
            if (s_date_order.length()!=10) {
                QMessageBox::critical(this, "Ошибка в дате заявки на выпуск полиса",
                                      "Дата заявки на выпуск полиса должна состоять из 10 символов.\nОбнаружено:\n"
                                      "  ЕНП   = " + s_enp + "\n"
                                      "  серия = " + s_pol_ser + "\n"
                                      "  номер = " + s_pol_num + "\n"
                                      "  дата  = " + s_date_order + "\n\n"
                                      "Операция отменена.\n");
                db.rollback();
                this->setCursor(Qt::ArrowCursor);
                return;
            }
            if (s_date_order.length()!=10) {
                QMessageBox::critical(this, "Ошибка в дате заявки на выпуск полиса",
                                      "Дата заявки на выпуск полиса должна состоять из 10 символов.\nОбнаружено:\n"
                                      "  ЕНП   = " + s_enp + "\n"
                                      "  серия = " + s_pol_ser + "\n"
                                      "  номер = " + s_pol_num + "\n"
                                      "  дата  = " + s_date_order + "\n\n"
                                      "Операция отменена.\n");
                db.rollback();
                this->setCursor(Qt::ArrowCursor);
                return;
            }

            // обработаем полученные данные
            date_order   = QDate::fromString(s_date_order,   "dd.MM.yyyy");
            date_product = QDate::fromString(s_date_product, "dd.MM.yyyy");

            // проверим, есть ли уже такая строка
            QSqlQuery *query_blank = new QSqlQuery(db);
            QString sql_blank = "select count(*) "
                                "  from blanks_boxes s "
                                " where box_num=" + QString::number(ui->spin_box->value()) + " "
                                "   and enp='" + s_enp + "' "
                                "   and pol_ser='" + s_pol_ser + "' "
                                "   and pol_num='" + s_pol_num + "' " +
                                (s_date_order.isEmpty()   ? " " : (" and date_order='"   + date_order.toString("yyyy-MM-dd") + "' ")) +
                                (s_date_product.isEmpty() ? " " : (" and date_product='" + date_product.toString("yyyy-MM-dd") + "' ")) + " ; ";
            if (mySQL.exec(this, sql_blank, QString("проверим наличие бланка"), *query_blank, true, db, data_app) && query_blank->next()) {
                int n = query_blank->value(0).toInt();

                if (n==0) {
                    // добавим новый бланк
                    QSqlQuery *query_add = new QSqlQuery(db);
                    QString sql_add = "insert into blanks_boxes (box_num, enp, pol_ser, pol_num, date_order, date_product, date_add, date_scan, id_blanks_box_act) "
                                      " values(" + QString::number(ui->spin_box->value()) + ", "
                                          "   '" + s_enp + "', "
                                          "   '" + s_pol_ser + "', "
                                          "   '" + s_pol_num + "', "
                                          "   '" + date_order.toString("yyyy-MM-dd")   + "', "
                                          "   '" + date_product.toString("yyyy-MM-dd") + "', "
                                          "   '" + QDate::currentDate().toString("yyyy-MM-dd") + "', "
                                          "   NULL,"
                                          "   "  + QString::number(id_blanks_box_act) + ") ; ";
                    if (!mySQL.exec(this, sql_add, QString("добавим банк"), *query_add, true, db, data_app)) {
                        QMessageBox::critical(this, "Ошибка добавления бланка",
                                              "При попытке добавить новй бланк произошла непредвиденная ошибка.\n"
                                              "Операция отменена.\n");
                        db.rollback();
                        this->setCursor(Qt::ArrowCursor);
                        return;
                    }
                    n_add++;
                }
            } else {
                QMessageBox::critical(this, "Ошибка добавления записи бланка",
                                      "При попытке добавить запись бланка произошла непредвиденная ошибка.\n"
                                      "Операция отменена.\n");
                db.rollback();
                this->setCursor(Qt::ArrowCursor);
                return;
            }

            // найдём следующую строку
            while ( pos<s_data.length() && (s_data.at(pos)>=QChar('0') && s_data.at(pos)<=QChar('9')) ) {
                pos++;
            }
            // если данные кончились - выход из цикла
            if (pos>=s_data.length()) {
                break;
            }

            // почистим строки
            s_enp = "";
            s_pol_ser = "";
            s_pol_num = "";
            s_date_order = "";
            s_date_product = "";
        }
        pos++;
    }
    db.commit();
    this->setCursor(Qt::ArrowCursor);

    QMessageBox::information(this, "Обработка завершена", "Прочитано " + QString::number(n_read) + " записей.\n\nДобавлено " + QString::number(n_add) + " новых бланков.");
    accept();
}

void add_blanks_box_wnd::on_date_head_act_dateChanged(const QDate &date) {
    if (date>QDate::currentDate()) {
        ui->date_head_act->setDate(QDate::currentDate());
    }
}

void add_blanks_box_wnd::on_combo_point_currentIndexChanged(int index) {
    /*int i = ui->combo_point->currentIndex();
    if (i>0) {
        QString point_regnum = point_regnums.at(i);
        ui->line_point_act_num->setText(ui->line_head_act_num->text() + " / " + point_regnum);
    }*/
}

void add_blanks_box_wnd::on_bn_today_clicked() {
    //ui->date_point_act->setDate(QDate::currentDate());
}

void add_blanks_box_wnd::on_date_point_act_dateChanged(const QDate &date) {
    /*if (date<QDate::currentDate()) {
        ui->date_point_act->setDate(QDate::currentDate());
    }*/
}

void add_blanks_box_wnd::on_date_head_act_editingFinished() {
    //ui->date_point_act->setDate(ui->date_head_act->date());
}
