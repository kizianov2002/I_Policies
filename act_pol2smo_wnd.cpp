#include "act_pol2smo_wnd.h"
#include "ui_act_pol2smo_wnd.h"

act_pol2smo_wnd::act_pol2smo_wnd(QSqlDatabase &db, s_data_app &data_app, s_data_act_pol2smo &data_act, QSettings &settings, QWidget *parent) :
    db(db),
    data_app(data_app),
    data_act(data_act),
    settings(settings),
    QDialog(parent),
    ui(new Ui::act_pol2smo_wnd)
{
    ui->setupUi(this);

    act_blanknums_w = NULL;

    refresh_combo_acts();
    refresh_combo_move_acts();
    ui->combo_act->setCurrentIndex(ui->combo_act->findData(data_act.id));
}

act_pol2smo_wnd::~act_pol2smo_wnd() {
    delete ui;
}

void act_pol2smo_wnd::on_bn_close_clicked() {
    close();
}

void act_pol2smo_wnd::refresh_combo_acts() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка актов
    QSqlQuery *query = new QSqlQuery(db);
    QString sql;
    sql = "select distinct id, coalesce(act_num,' - ') || '    (от ' || coalesce(cast(act_date as text),' - ') || ')', act_num, act_date "
          "  from blanks_boxes_acts a "
          " order by act_date asc, act_num asc ; ";
    mySQL.exec(this, sql, QString("список актов для ПВП"), *query, true, db, data_app);

    ui->combo_act->clear();
    ui->combo_act->addItem(" - нет акта - ", -1);

    while (query->next()) {
        QString num = query->value(2).toString();
        QDate date = query->value(3).toDate();
        num += "   (" + date.toString("dd.MM.yyyy") + ")";
        ui->combo_act->addItem(num, query->value(0).toInt());
    }
    ui->combo_act->setCurrentIndex(0);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void act_pol2smo_wnd::refresh_combo_move_acts() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка актов
    QSqlQuery *query = new QSqlQuery(db);
    QString sql;
    sql = "select distinct id, coalesce(act_num,' - ') || '    (от ' || coalesce(cast(act_date as text),' - ') || ')', act_num, act_date "
          "  from blanks_boxes_acts a "
          " order by act_date asc, act_num asc ; ";
    mySQL.exec(this, sql, QString("список актов для ПВП"), *query, true, db, data_app);

    ui->combo_move_act->clear();

    while (query->next()) {
        QString num = query->value(2).toString();
        QDate date = query->value(3).toDate();
        num += "  (" + date.toString("dd.MM.yyyy") + ")";
        ui->combo_move_act->addItem(num, query->value(0).toInt());
    }
    ui->combo_move_act->setCurrentIndex(0);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void act_pol2smo_wnd::refresh_tab_act_blanks() {
    QString sql_pol2smo = "select b.id as id_blank, b.pol_ser, b.pol_num, b.enp, b.date_scan_enp, "
                          "       s.code as st_code, '('||s.code||')  '||s.text as status, "
                          "       a.id, a.act_num, a.act_date, "
                          "       p.id as id_point, '('||p.point_regnum||')  '||p.point_name as point, "
                          "       o.id as id_obtainer, o.oper_fio as obtainer "
                          "  from blanks_pol b "
                          "  left join spr_blank_status s on(s.code=b.status) "
                          "  left join blanks_boxes_acts a on(b.id_blanks_box_act=a.id) "
                          "  /*left join blanks_boxes bb on(bb.box_num=a.box_num)*/ "
                          "  left join points p on(p.id=a.id_head_point) "
                          "  left join operators o on(o.id=a.id_obtainer) ";
    if ( ui->ch_act->isChecked() ) {
        sql_pol2smo  +=  " where ";

        if ( ui->combo_act->currentIndex()==0 ) {
            sql_pol2smo  +=  " a.id is NULL ";
        } else {
            sql_pol2smo  +=  " a.id=" + QString::number(ui->combo_act->currentData().toInt()) + " ";
        }
    }
    if (ui->combo_order->currentIndex()==0)
        sql_pol2smo += " order by b.pol_ser, b.pol_num ; ";
    else sql_pol2smo += " order by b.enp ; ";

    model_act_polises.setQuery(sql_pol2smo,db);
    QString err2 = model_act_polises.lastError().driverText();

    // подключаем модель из БД
    ui->tab_act_blanks->setModel(&model_act_polises);

    // обновляем таблицу
    ui->tab_act_blanks->reset();

    // задаём ширину колонок
    ui->tab_act_blanks->setColumnWidth( 0,  1);     // b.id,
    ui->tab_act_blanks->setColumnWidth( 1, 50);     // b.pol_ser,
    ui->tab_act_blanks->setColumnWidth( 2, 60);     // b.pol_num,
    ui->tab_act_blanks->setColumnWidth( 3,120);     // enp
    ui->tab_act_blanks->setColumnWidth( 4, 70);     // date_scan_enp
    ui->tab_act_blanks->setColumnWidth( 5,  1);     //
    ui->tab_act_blanks->setColumnWidth( 6,150);     // s.text as status,
    ui->tab_act_blanks->setColumnWidth( 7,  1);     // a.id,
    ui->tab_act_blanks->setColumnWidth( 8, 90);     // a.act_num,
    ui->tab_act_blanks->setColumnWidth( 9, 70);     // a.act_date,
    ui->tab_act_blanks->setColumnWidth(10,  1);     //
    ui->tab_act_blanks->setColumnWidth(11,150);     // '('||p.point_regnum||')  '||p.point_name as point,
    ui->tab_act_blanks->setColumnWidth(12,  1);     //
    ui->tab_act_blanks->setColumnWidth(13,120);     // o.oper_fio as obtainer

    // правим заголовки
    model_act_polises.setHeaderData( 1, Qt::Horizontal, ("серия\nбланка"));
    model_act_polises.setHeaderData( 2, Qt::Horizontal, ("номер\nбланка"));
    model_act_polises.setHeaderData( 3, Qt::Horizontal, ("ЕНП"));
    model_act_polises.setHeaderData( 4, Qt::Horizontal, ("дата\nскан. ЕНП"));
    model_act_polises.setHeaderData( 6, Qt::Horizontal, ("статус\nбланка"));
    model_act_polises.setHeaderData( 8, Qt::Horizontal, ("номер акта"));
    model_act_polises.setHeaderData( 9, Qt::Horizontal, ("дата \nакта"));
    model_act_polises.setHeaderData(11, Qt::Horizontal, ("получающая \nсторона"));
    model_act_polises.setHeaderData(13, Qt::Horizontal, ("ответственное \nлицо"));
    ui->tab_act_blanks->repaint();

    this->setCursor(Qt::ArrowCursor);

    ui->lab_cnt->setText(QString::number(model_act_polises.rowCount()) + " строк");
}

void act_pol2smo_wnd::on_bn_refresh_clicked() {
    refresh_tab_act_blanks();
}
void act_pol2smo_wnd::on_combo_act_currentIndexChanged(int index) {
    refresh_tab_act_blanks();
}

void act_pol2smo_wnd::on_bn_move_blanks_clicked() {
    // обработаем только выделенные
    QModelIndexList list = ui->tab_act_blanks->selectionModel()->selectedIndexes();

    int n = list.count()/13;
    if (n==0) {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано.");
        return;
    } else if (n>0) {
        if (QMessageBox::question(this, "Нужно подтверждение",
                                  "Вы действительно хотите включить \n" +
                                  QString::number(n) + " бланков полисов единого образца \n\n"
                                  "в акт передачи на ПВП \n" +
                                  ui->combo_move_act->currentText() + "\n?",
                                  QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel)==QMessageBox::Cancel) {
            return;
        }
    }
    int id_act_cur = ui->combo_act->currentData().toInt();
    int id_act_new = ui->combo_move_act->currentData().toInt();
    if ( ui->ch_act->isChecked()
         && id_act_new==id_act_cur) {
        QMessageBox::warning(this, "Неверные данные",
                              "Текущий и целевой акты совпадают.\n"
                              "Бланки нельзя переносить из акта в тот же самый акт.\n\n"
                              "Операция отменена.");
        return;
    }
    db.transaction();

    for (int i=0; i<list.count(); i++) {
        QModelIndex index = list.at(i);
        if (index.column()!=0) continue;

        int id_blank_pol = model_act_polises.data(model_act_polises.index(index.row(), 0), Qt::EditRole).toInt();

        QString sql = "update blanks_pol "
                      "   set id_blanks_box_act=" + QString::number(id_act_new) + " "
                      " where id=" + QString::number(id_blank_pol) + " ; ";

        QSqlQuery *query = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Перенос бланка полиса в другой акт", *query, true, db, data_app) ) {
            db.rollback();
            QMessageBox::warning(this, "Не удалось перенести бланк полиса в другой акт",
                                 "При попытке перенести бланк полиса в другой акт произошла неожиданная ошибка.\n\n"
                                 "Операция отменена.");
            delete query;
            return;
        }
        delete query;
    }
    db.commit();
    refresh_tab_act_blanks();
}

void act_pol2smo_wnd::on_bn_remove_blanks_clicked() {
    // обработаем только выделенные
    QModelIndexList list = ui->tab_act_blanks->selectionModel()->selectedIndexes();

    int n = list.count()/13;
    if (n==0) {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано.");
        return;
    } else if (n>0) {
        if (QMessageBox::question(this, "Нужно подтверждение",
                                  "Вы действительно хотите изъять \n" +
                                  QString::number(n) + " бланков полисов единого образца \n\n"
                                  "из акта (актов) передачи на ПВП ?",
                                  QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel)==QMessageBox::Cancel) {
            return;
        }
    }
    db.transaction();

    for (int i=0; i<list.count(); i++) {
        QModelIndex index = list.at(i);
        if (index.column()!=0) continue;

        int id_blank_pol = model_act_polises.data(model_act_polises.index(index.row(), 0), Qt::EditRole).toInt();

        QString sql = "update blanks_pol "
                      "   set id_blanks_box_act=NULL "
                      " where id=" + QString::number(id_blank_pol) + " ; ";

        QSqlQuery *query = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Изъятие бланка полиса из акта", *query, true, db, data_app) ) {
            db.rollback();
            QMessageBox::warning(this, "Не удалось изъять бланк полиса из акта",
                                 "При попытке изъять бланк полиса из акта произошла неожиданная ошибка.\n\n"
                                 "Операция отменена.");
            delete query;
            return;
        }
        delete query;
    }
    db.commit();
    refresh_tab_act_blanks();
}

void act_pol2smo_wnd::on_ch_act_clicked(bool checked) {
    ui->combo_act->setEnabled(checked);
    refresh_tab_act_blanks();
}

void act_pol2smo_wnd::on_combo_order_activated(const QString &arg1) {
    refresh_tab_act_blanks();
}

void act_pol2smo_wnd::on_bn_add_blanks_clicked()  {
    QStringList blanknums;

    delete act_blanknums_w;
    act_blanknums_w = new act_blanknums_wnd(blanknums, this);

    if (act_blanknums_w->exec()) {

        db.transaction();

        for (int i=0; i<blanknums.size(); i++) {
            QString ser = blanknums.at(i).left(4);
            QString num = blanknums.at(i).right(blanknums.at(i).size() - 4);

            QString sql = "update blanks_pol "
                          "   set id_blanks_box_act=" + QString::number(data_act.id) + " "
                          " where pol_ser='" + ser + "' "
                          "   and pol_num='" + num + "' ; ";

            QSqlQuery *query = new QSqlQuery(db);
            if (!mySQL.exec(this, sql, "Перенос бланка полиса в другой акт", *query, true, db, data_app) ) {
                db.rollback();
                QMessageBox::warning(this, "Не удалось включить бланк полиса в этот акт",
                                     "При попытке включить бланк полиса в этот акт (для СМО) произошла неожиданная ошибка.\n\n"
                                     "Операция отменена.");
                delete query;
                return;
            }
            delete query;
        }

        db.commit();
        refresh_tab_act_blanks();
        QMessageBox::information(this, "Готово", "Опеавция завершена");

    } else {
        QMessageBox::information(this, "Действие отменено пользователем", "Действие отменено пользователем");
    }
}
