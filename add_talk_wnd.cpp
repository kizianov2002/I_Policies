#include "add_talk_wnd.h"
#include "ui_add_talk_wnd.h"

add_talk_wnd::add_talk_wnd(QSqlDatabase &db, s_data_talk *data_talk, s_data_app &data_app, QWidget *parent) :
    db(db), data_talk(data_talk), data_app(data_app), QDialog(parent), ui(new Ui::add_talk_wnd)
{
    ui->setupUi(this);

    ui->lab_fio_pers->setText(data_talk->fio);
    ui->date_talk->setDate(data_talk->date_talk);
    ui->te_comment->setPlainText(data_talk->comment);

    ui->combo_type_talk->clear();
    ui->combo_type_talk->addItem(" - выберите способ связи - ", 0);
    ui->combo_type_talk->addItem("( 1)  плановое СМС-оповещение о получении полиса", 1);
    ui->combo_type_talk->addItem("( 2)  неплановое СМС-оповещение", 2);
    ui->combo_type_talk->addItem("( 3)  звонок из СМО застрахованному", 3);
    ui->combo_type_talk->addItem("( 4)  визит работника СМО к застрахованному", 4);
    ui->combo_type_talk->addItem("( 5)  звонок застрахованного в СМО", 5);
    ui->combo_type_talk->addItem("( 6)  письмо", 6);

    ui->combo_type_talk->setCurrentIndex(ui->combo_type_talk->findData(data_talk->type_talk));

    ui->combo_result->clear();
    ui->combo_result->addItem("(-4)  возврат письма", -4);
    ui->combo_result->addItem("(-3)  номер/адрес не существует", -3);
    ui->combo_result->addItem("(-2)  чужой номер/адрес", -2);
    ui->combo_result->addItem("(-1)  недозвон/нет ответа", -1);
    ui->combo_result->addItem(" - выберите результат - ", 0);
    ui->combo_result->addItem("( 1)  ответил лично", 1);
    ui->combo_result->addItem("( 2)  ответил роственик/знакомый", 2);
    ui->combo_result->addItem("( 3)  письмо отправлено", 3);
    ui->combo_result->addItem("( 4)  письмо получено", 4);

    if (data_talk->result==-999)
        ui->combo_result->setCurrentIndex(4);
    else
        ui->combo_result->setCurrentIndex(ui->combo_result->findData(data_talk->result));

    refresh_polises();

    if (data_talk->_id_polis>=0) {
        ui->combo_polis->setCurrentIndex(ui->combo_polis->findData(data_talk->_id_polis));
    } else {
        ui->combo_polis->setCurrentIndex(0);
    }
}

add_talk_wnd::~add_talk_wnd() {
    delete ui;
}

void add_talk_wnd::on_bn_close_clicked() {
    close();
}

void add_talk_wnd::refresh_polises() {
    //выберем список полисов персоны
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка " + QString::number(data_talk->_id_person) + " ПВП
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select * "
                  " from ( "
                  "    select id_polis, NULL as pol_v, NULL as vs_num, bp.pol_ser, bp.pol_num, bp.enp, bp.date_add as date_pol, bp.id as id_blank_pol "
                  "      from blanks_pol bp "
                  "     where bp.id_person=" + QString::number(data_talk->_id_person) + " "
                  "       and bp.status=0 "
                  "   union "
                  "    select p.id, p.pol_v, p.vs_num, p.pol_ser, p.pol_num, p.enp, cast(p.dt_ins as date), NULL "
                  "      from polises p "
                  "         left join blanks_pol bp on(bp.id_polis=p.id) "
                  "         left join blanks_vs  bv on(bv.id_polis=p.id) "
                  "     where p.id_person=" + QString::number(data_talk->_id_person) + " "
                  "  ) q "
                  " order by q.date_pol desc ; ";

    mySQL.exec(this, sql, QString("статусы бланков"), *query, true, db, data_app);
    pol_vers.clear();
    pol_ids.clear();
    blank_ids.clear();
    ui->combo_polis->clear();
    while (query->next()) {
        int pol_type = query->value(1).toInt();
        switch (pol_type) {
        case 0:
            ui->combo_polis->addItem("не выданный полис: " + query->value(3).toString() + " №" + query->value(4).toString() + " ", query->value(0).toInt());
            break;
        case 2:
            ui->combo_polis->addItem("ВС: №" + query->value(2).toString() + " ", query->value(0).toInt());
            break;
        case 3:
            ui->combo_polis->addItem("полис: " + query->value(3).toString() + " №" + query->value(4).toString() + " ", query->value(0).toInt());
            break;
        default:
            ui->combo_polis->addItem(" - ??? - ", query->value(0).toInt());
            break;
        }
        pol_vers.append(query->value(1).toInt());
        pol_ids.append(query->value(0).isNull() ? -1 : query->value(0).toInt());
        blank_ids.append(query->value(7).isNull() ? -1 : query->value(7).toInt());
    }
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void add_talk_wnd::on_bn_save_clicked() {
    // проверка введённых данных
    if (ui->combo_type_talk->currentIndex()==0) {
        QMessageBox::warning(this, "Недостаточно данных", "Укажите способ связи с застрахованным.");
        return;
    }
    if (ui->date_talk->date()>QDate::currentDate()) {
        QMessageBox::warning(this, "Неверные данные", "Выбранная дата звонка/визита ещё не наступила.");
        return;
    }
    if (ui->combo_result->currentIndex()==4) {
        QMessageBox::warning(this, "Недостаточно данных", "Укажите результат звонка/визита.");
        return;
    }

    if (data_talk->id<0) {
        // записи нет - создадим её
        QString sql_add = "insert into talks(pol_v, _id_person, _id_polis, _id_blank_pol, type_talk, date_talk, result, comment) "
                          " values( " + QString::number(pol_vers.at(ui->combo_polis->currentIndex())) + ", "
                                  " " + QString::number(data_talk->_id_person) + ", "
                                  " " + QString::number(pol_ids.at(ui->combo_polis->currentIndex())) + ", "
                                  " " + QString::number(blank_ids.at(ui->combo_polis->currentIndex())) + ", "
                                  " " + QString::number(ui->combo_type_talk->currentIndex()) + ", "
                                  "'" + ui->date_talk->date().toString("yyyy-MM-dd") + "', "
                                  " " + QString::number(ui->combo_result->currentData().toInt()) + ", "
                                  "'" + ui->te_comment->toPlainText() + "' ) "
                          " returning id ; ";

        QSqlQuery *query = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_add, "Добавление записи звонка/визита", *query, true, db, data_app) ) {
            QMessageBox::warning(this, "Не удалось добавить запись звонка/визита",
                                 "Новую запись звонка/визита добавить в базу данных не удалось\n\n"
                                 "Проверьте правильность введённых данных и повторите попытку.");
        }
        query->next();
        data_talk->id = query->value(0).toInt();
        delete query;
    } else {
        QString sql_edit = "update talks "
                           " set type_talk=" + QString::number(ui->combo_type_talk->currentIndex()) + ", "
                           "     date_talk='" + ui->date_talk->date().toString("yyyy-MM-dd") + "', "
                           "     result=" + QString::number(ui->combo_result->currentData().toInt()) + ", "
                           "     comment='" + ui->te_comment->toPlainText() + "' "
                           " where id=" + QString::number(data_talk->id) + " ; ";

        QSqlQuery *query = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_edit, "Обновление записи звонка/визита", *query, true, db, data_app) ) {
            QMessageBox::warning(this, "Не удалось обновить запись звонка/визита",
                                 "Обновить запись звонка/визита не удалось\n\n"
                                 "Проверьте правильность введённых данных и повторите попытку.");
        }
        delete query;
    }
    close();
}

void add_talk_wnd::on_date_talk_dateChanged(const QDate &date) {
    ui->lab_now->setVisible(date==QDate::currentDate());
}

void add_talk_wnd::on_combo_type_talk_currentIndexChanged(int index) {
    if (index==6) {
        ui->label_6->setText("номер .\nпочтового .\nотправления :");
    } else {
        ui->label_6->setText("комментарий :");
    }
}
