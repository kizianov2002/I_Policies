#include "act_vs2point_wnd.h"
#include "ui_act_vs2point_wnd.h"

act_vs2point_wnd::act_vs2point_wnd(QSqlDatabase &db, s_data_app &data_app, s_data_act_vs2point &data_act, QSettings &settings, QWidget *parent) :
    db(db),
    data_app(data_app),
    data_act(data_act),
    settings(settings),
    QDialog(parent),
    ui(new Ui::act_vs2point_wnd)
{
    ui->setupUi(this);

    act_blanknums_w = NULL;

    refresh_combo_acts();
    refresh_combo_move_acts();
    ui->combo_act->setCurrentIndex(ui->combo_act->findData(data_act.id));
}

act_vs2point_wnd::~act_vs2point_wnd() {
    delete ui;
}

void act_vs2point_wnd::on_bn_close_clicked() {
    close();
}

void act_vs2point_wnd::refresh_combo_acts() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка актов
    QSqlQuery *query = new QSqlQuery(db);
    QString sql;
    sql = "select distinct id, coalesce(act_num,' - ') || '    (от ' || coalesce(cast(act_date as text),' - ') || ')', act_num, act_date "
          "  from public.blanks_vs_acts a "
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

void act_vs2point_wnd::refresh_combo_move_acts() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка актов
    QSqlQuery *query = new QSqlQuery(db);
    QString sql;
    sql = "select distinct id, coalesce(act_num,' - ') || '    (от ' || coalesce(cast(act_date as text),' - ') || ')', act_num, act_date "
          "  from public.blanks_vs_acts a "
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

void act_vs2point_wnd::refresh_tab_act_blanks() {

    /*
    QString sql = "select b.vs_num, s.text as status, '('||p0.point_regnum||')  '||p0.point_name as sender, '('||p1.point_regnum||')  '||p1.point_name as point, o.oper_fio as obtainer "
                  "  from blanks_vs b "
                  "  left join spr_blank_status s on(s.code=b.status) "
                  "  left join blanks_vs_acts a on(a.id=b.id_blanks_vs_act) "
                  "  left join points p0 on(p0.id=a.id_sender) "
                  "  left join points p1 on(p1.id=a.id_point) "
                  "  left join operators o on(o.id=a.id_obtainer) "
                  " where b.id_blanks_vs_act=" + QString::number(id) + " "
                  " order by b.vs_num ; ";
    */


    QString sql_vs2point = "select b.vs_num, "
                           "       s.code as st_code, '('||s.code||')  '||s.text as status, "
                           "       a.id, a.act_num, a.act_date, "
                           "       p0.id as id_sender, case when p0.id is NULL then ' - головной офис - ' else '('||p0.point_regnum||')  '||p0.point_name end as sender, "
                           "       p.id as id_point, '('||p.point_regnum||')  '||p.point_name as point, "
                           "       o.id as id_obtainer, o.oper_fio as obtainer "
                           "  from blanks_vs b "
                           "  left join blanks_vs_acts a on(b.id_blanks_vs_act=a.id) "
                           "  left join points p0 on(p0.id=a.id_sender) "
                           "  left join points p on(p.id=a.id_point) "
                           "  left join operators o on(o.id=a.id_obtainer) "
                           "  left join spr_blank_status s on(s.code=b.status) ";
    if ( ui->ch_act->isChecked() ) {
        sql_vs2point  +=  " where ";

        if (ui->combo_act->currentIndex()==0) {
            sql_vs2point  +=  " a.id is NULL ";
        } else {
            sql_vs2point  +=  " a.id=" + QString::number(ui->combo_act->currentData().toInt()) + " ";
        }
    }
    sql_vs2point += " order by b.vs_num ; ";

    model_act_polises.setQuery(sql_vs2point,db);
    QString err2 = model_act_polises.lastError().driverText();

    // подключаем модель из БД
    ui->tab_act_blanks->setModel(&model_act_polises);

    // обновляем таблицу
    ui->tab_act_blanks->reset();

    // задаём ширину колонок
    ui->tab_act_blanks->setColumnWidth( 0, 80);     // b.vs_num,
    ui->tab_act_blanks->setColumnWidth( 1,  1);     //
    ui->tab_act_blanks->setColumnWidth( 2,150);     // s.text as status,
    ui->tab_act_blanks->setColumnWidth( 3,  1);     // a.id,
    ui->tab_act_blanks->setColumnWidth( 4, 80);     // a.act_num,
    ui->tab_act_blanks->setColumnWidth( 5, 70);     // a.act_date,
    ui->tab_act_blanks->setColumnWidth( 6,  1);     //
    ui->tab_act_blanks->setColumnWidth( 7,150);     // '('||p0.point_regnum||')  '||p0.point_name as sender,
    ui->tab_act_blanks->setColumnWidth( 8,  1);     //
    ui->tab_act_blanks->setColumnWidth( 9,150);     // '('||p.point_regnum||')  '||p.point_name as point,
    ui->tab_act_blanks->setColumnWidth(10,  1);     //
    ui->tab_act_blanks->setColumnWidth(11,135);     // o.oper_fio as obtainer

    // правим заголовки
    model_act_polises.setHeaderData( 0, Qt::Horizontal, ("номер ВС"));
    model_act_polises.setHeaderData( 2, Qt::Horizontal, ("статус\nбланка ВС"));
    model_act_polises.setHeaderData( 4, Qt::Horizontal, ("номер акта"));
    model_act_polises.setHeaderData( 5, Qt::Horizontal, ("дата \nакта"));
    model_act_polises.setHeaderData( 7, Qt::Horizontal, ("передающая \nсторона"));
    model_act_polises.setHeaderData( 9, Qt::Horizontal, ("получающая \nсторона"));
    model_act_polises.setHeaderData(11, Qt::Horizontal, ("ответственное \nлицо"));
    ui->tab_act_blanks->repaint();

    this->setCursor(Qt::ArrowCursor);

    ui->lab_cnt->setText(QString::number(model_act_polises.rowCount()) + " строк");
}

void act_vs2point_wnd::on_bn_refresh_clicked() {
    refresh_tab_act_blanks();
}
void act_vs2point_wnd::on_combo_act_currentIndexChanged(int index) {
    refresh_tab_act_blanks();
}

void act_vs2point_wnd::on_bn_move_blanks_clicked() {
    // обработаем только выделенные
    QModelIndexList list = ui->tab_act_blanks->selectionModel()->selectedIndexes();

    int n = list.count()/12;
    if (n==0) {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано.");
        return;
    } else if (n>0) {
        if (QMessageBox::question(this, "Нужно подтверждение",
                                  "Вы действительно хотите включить \n" +
                                  QString::number(n) + " бланков ВС \n\n"
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

    // получим данные акта
    QString sql_data = "select id_sender, id_point, status "
                       "  from blanks_vs_acts "
                       " where id=" + QString::number(id_act_new) + " ; ";
    QSqlQuery *query_data = new QSqlQuery(db);
    if (!mySQL.exec(this, sql_data, "Получим данные акта", *query_data, true, db, data_app) ) {
        db.rollback();
        QMessageBox::warning(this, "Не удалось получить данные ата",
                             "При попытке получить данные акта произошла неожиданная ошибка.\n\n"
                             "Операция отменена.");
        delete query_data;
        return;
    }
    query_data->next();
    int id_sender = query_data->value(0).toInt();
    int id_point = query_data->value(1).toInt();
    int status = query_data->value(2).toInt();
    delete query_data;
    int id_point2 = (status>0 ? id_point : id_sender);

    db.transaction();

    for (int i=0; i<list.count(); i++) {
        QModelIndex index = list.at(i);
        if (index.column()!=0) continue;

        int vs_num = model_act_polises.data(model_act_polises.index(index.row(), 0), Qt::EditRole).toInt();

        QString sql = "update blanks_vs "
                      "   set id_blanks_vs_act=" + QString::number(id_act_new) + ", "
                      "       id_point=" + QString::number(id_point2) + " "
                      " where vs_num=" + QString::number(vs_num) + " ; ";

        QSqlQuery *query = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Перенос бланка ВС в другой акт", *query, true, db, data_app) ) {
            db.rollback();
            QMessageBox::warning(this, "Не удалось перенести бланк ВС в другой акт",
                                 "При попытке перенести бланк ВС в другой акт произошла неожиданная ошибка.\n\n"
                                 "Операция отменена.");
            delete query;
            return;
        }
        delete query;
    }
    db.commit();
    refresh_tab_act_blanks();
}

void act_vs2point_wnd::on_bn_remove_blanks_clicked() {
    // обработаем только выделенные
    QModelIndexList list = ui->tab_act_blanks->selectionModel()->selectedIndexes();

    int n = list.count()/12;
    if (n==0) {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано.");
        return;
    } else if (n>0) {
        if (QMessageBox::question(this, "Нужно подтверждение",
                                  "Вы действительно хотите изъять \n" +
                                  QString::number(n) + " бланков ВС \n\n"
                                  "из акта (актов) передачи на ПВП ?",
                                  QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel)==QMessageBox::Cancel) {
            return;
        }
    }
    db.transaction();

    for (int i=0; i<list.count(); i++) {
        QModelIndex index = list.at(i);
        if (index.column()!=0) continue;

        int vs_num = model_act_polises.data(model_act_polises.index(index.row(), 0), Qt::EditRole).toInt();

        QString sql = "update blanks_vs "
                      "   set id_blanks_vs_act=NULL, "
                      "       id_point=NULL "
                      " where vs_num=" + QString::number(vs_num) + " ; ";

        QSqlQuery *query = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Изъятие бланка полиса из акта", *query, true, db, data_app) ) {
            db.rollback();
            QMessageBox::warning(this, "Не удалось изъять бланк ВС из акта",
                                 "При попытке изъять бланк ВС из акта произошла неожиданная ошибка.\n\n"
                                 "Операция отменена.");
            delete query;
            return;
        }
        delete query;
    }
    db.commit();
    refresh_tab_act_blanks();
}

void act_vs2point_wnd::on_ch_act_clicked(bool checked) {
    ui->combo_act->setEnabled(checked);
    refresh_tab_act_blanks();
}


void act_vs2point_wnd::on_bn_add_blanks_clicked()  {
    QStringList blanknums;

    delete act_blanknums_w;
    act_blanknums_w = new act_blanknums_wnd(blanknums, this);

    if (act_blanknums_w->exec()) {

        db.transaction();

        for (int i=0; i<blanknums.size(); i++) {
            QString vs_num = blanknums.at(i);

            QString sql = "update blanks_vs "
                          "   set id_blanks_vs_act=" + QString::number(data_act.id) + ", "
                          "       id_point=" + QString::number(data_act.id_point) + " "
                          " where vs_num='" + vs_num + "' ; ";

            QSqlQuery *query = new QSqlQuery(db);
            if (!mySQL.exec(this, sql, "Перенос бланка полиса в другой акт", *query, true, db, data_app) ) {
                db.rollback();
                QMessageBox::warning(this, "Не удалось включить бланк ВС в этот акт",
                                     "При попытке включить бланк ВС в этот акт (для ПВП) произошла неожиданная ошибка.\n\n"
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
