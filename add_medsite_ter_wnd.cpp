#include "add_medsite_ter_wnd.h"
#include "ui_add_medsite_ter_wnd.h"

add_medsite_ter_wnd::add_medsite_ter_wnd(QSqlDatabase &db, s_data_app &data_app, s_data_medsite_ter &data, QSettings &settings, QWidget *parent) :
    db(db), data_app(data_app), data(data), settings(settings),
    QDialog(parent),
    ui(new Ui::add_medsite_ter_wnd)
{
    ui->setupUi(this);

    //if (data.id_mt>=0) {
    //    this->setWindowTitle("Участковый врач (правка записи)");
    //} else {
        this->setWindowTitle("+ Участковый врач (новая запись) +");
    //}

    refresh_medsites();
    refresh_medters();
    ui->lab_mo->setText(data.name_mo);
    ui->lab_mp->setText(data.name_mp);
    ui->combo_ms->setCurrentIndex(ui->combo_ms->findData(data.id_ms));
    ui->combo_mt->setCurrentIndex(ui->combo_mt->findData(data.id_mt));
}

add_medsite_ter_wnd::~add_medsite_ter_wnd() {
    delete ui;
}

void add_medsite_ter_wnd::on_bn_cansel_clicked() {
    close();
}

void add_medsite_ter_wnd::refresh_medsites() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка участков
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code_ms, '(' || code_ms || ') ' || t.text "
                  " from public.spr_medsites s "
                  " left join spr_medsites_types t on(t.code=s.type_ms) "
                  " where ms.code_mo='" + data.code_mo + "' "
                  "   and ms.code_mp='" + data.code_mp + "' "
                  " order by code_ms ; ";
    mySQL.exec(this, sql, QString("Справочник участков для медорганизации и подразделения"), *query, true, db, data_app);
    ui->combo_ms->clear();
    while (query->next()) {
        ui->combo_ms->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    //ui->combo_medsite_mp->setCurrentIndex(-1);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}


void add_medsite_ter_wnd::refresh_medters() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка врачей
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select id, '(' || snils_mt || ') ' || fio_mt "
                  " from public.spr_medters mt "
                  " where mt.code_mo='" + data.code_mo + "' "
                  " order by fio_mt ; ";
    mySQL.exec(this, sql, QString("Справочник врачей для медорганизации"), *query, true, db, data_app);
    ui->combo_mt->clear();
    while (query->next()) {
        ui->combo_mt->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    //ui->combo_medsite_mp->setCurrentIndex(-1);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void add_medsite_ter_wnd::on_bn_save_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"),
                             ("Нет открытого соединения к базе данных?\n "
                              "Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n "
                              "Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    // проверка
    if (ui->combo_ms->currentIndex()<0) {
        QMessageBox::warning(this, "Недостаточно данных",
                             "Выберите участок, к которому относится врач.\n\nСохранение отменено.");
        return;
    }
    if (ui->combo_mt->currentIndex()<0) {
        QMessageBox::warning(this, "Недостаточно данных",
                             "Выберите врача, обслуживающего выбранный участок.\n\nСохранение отменено.");
        return;
    }
    if (ui->combo_mt_prof->currentIndex()<1) {
        QMessageBox::warning(this, "Недостаточно данных",
                             "Выберите специальность врача.\n\nСохранение отменено.");
        return;
    }

    this->setCursor(Qt::WaitCursor);
    db.transaction();

    {   // проверим дублирующие записи
        QString id_ms = QString::number(data.id_ms);
        QString id_mt = QString::number(data.id_mt);

        QString sql2 = "select count(*) from spr_ms_r_mt "
                       " where id_ms=" + id_ms + " "
                       "   and id_mt=" + id_mt + " "
                       "   and (date_end is NULL or date_end>='" + QDate::currentDate().toString("yyyy-MM-dd") + "'); ";
        QSqlQuery *query2 = new QSqlQuery(db);
        if ( !mySQL.exec(this, sql2, "Проверка на дубли записи участкового врача", *query2, true, db, data_app) ||
             !query2->next() ) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При проверке на дубли записи участкового врача произошла неожиданная ошибка.\n\nУдаление отменено.");
            delete query2;
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        if (query2->value(0).toInt()>0) {
            QMessageBox::warning(this, "Такая запись уже есть",
                                 "Выбранный врач уже является действующим участковым врачом по этому участку.\n\n"
                                 "Операция отменена.");
            db.rollback();
            this->setCursor(Qt::ArrowCursor);
            return;
        }

        delete query2;

        // добавим участкового врача
        QString sql = "insert into spr_ms_r_mt (id_ms, id_mt, prof_mt, date_beg, date_end) "
                      " values ("
                      "   " + QString::number(ui->combo_ms->currentData().toInt()) + ", "
                      "   " + QString::number(ui->combo_mt->currentData().toInt()) + ", "
                      "   " + QString::number(ui->combo_mt_prof->currentIndex()) + ", "
                      "   '" + QDate::currentDate().toString("yyyy-MM-dd") + "', "
                      "   NULL) ; ";

        QSqlQuery *query = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Добавление участкового врача", *query, true, db, data_app) ) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка",
                                  "При попытке добавить участкового врача произошла неожиданная ошибка.\n\n"
                                  "Сохранение отменено.");
            delete query;
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        delete query;
    }

    db.commit();
    this->setCursor(Qt::ArrowCursor);

    close();
}
