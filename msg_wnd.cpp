#include "msg_wnd.h"
#include "ui_msg_wnd.h"
#include <QTimer>

msg_wnd::msg_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent)
    : db(db), data_app(data_app), QDialog(parent), settings(settings), ui(new Ui::msg_wnd)
{
    ui->setupUi(this);

    msg_timer = NULL;

    ui->split_main->setStretchFactor(0,3);
    ui->split_main->setStretchFactor(1,1);

    refresh_operators_tab();
    refresh_history();

    ui->te_msg->clear();
    ui->lab_getter->setText("?");

    // запустим таймер проверки сообщений
    delete msg_timer;
    msg_timer = new QTimer(this);
    connect(msg_timer, SIGNAL(timeout()), this, SLOT(msg_update()));
    msg_timer->setInterval(data_app.msg_timeout_front);
    msg_timer->start(data_app.msg_timeout_front);
}

msg_wnd::~msg_wnd() {
    delete ui;
}


void msg_wnd::refresh_history() {
    ui->te_hist->clear();
    // прочитаем файл исории
    QFile hist_file(data_app.path_install + "msg_history.txt");
    if (hist_file.exists()) {
        hist_file.open(QIODevice::ReadOnly);
        QString hist_str = hist_file.readAll();
        ui->te_hist->append(hist_str);
    }
}

void msg_wnd::on_bn_hist_reload_clicked() {
    refresh_history();
}

void msg_wnd::on_bn_hist_drop_clicked() {
    if (QMessageBox::warning(this, "Нужно подтверждение", "Вы действительно хотите безвозвратно очистить историю сообщений?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No)!=QMessageBox::Yes) {
        return;
    }
    QFile hist_file(data_app.path_install + "msg_history.txt");
    hist_file.open(QIODevice::WriteOnly);
    hist_file.write("-");
    refresh_history();
}


void msg_wnd::refresh_operators_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    QString operators_sql = "select p.id as id_point, p.point_name, o.id as id_operators, o.oper_fio "
                          " from points p "
                          "      left join operators o on(p.id=o.id_point) "
                          " where p.point_regnum<>'000' and p.status>0 and o.status>0 and o.id<>" + QString::number(data_app.id_operator) + " "
                          " order by p.id, o.oper_fio ; ";

    model_operators.setQuery(operators_sql,db);
    QString err = model_operators.lastError().driverText();

    // подключаем модель из БД
    ui->tab_operators->setModel(&model_operators);

    // обновляем таблицу
    ui->tab_operators->reset();

    // задаём ширину колонок
    ui->tab_operators->setColumnWidth( 0,  1);    // id_point
    ui->tab_operators->setColumnWidth( 1,180);    // point_name
    ui->tab_operators->setColumnWidth( 2,  1);    // id_operators
    ui->tab_operators->setColumnWidth( 3,200);    // user_fio

    // правим заголовки
    model_operators.setHeaderData( 1, Qt::Horizontal, ("ПВП"));
    model_operators.setHeaderData( 3, Qt::Horizontal, ("оператор"));
    ui->tab_operators->repaint();

    this->setCursor(Qt::ArrowCursor);
}


void msg_wnd::on_tab_operators_clicked(const QModelIndex &index) {
    this->setCursor(Qt::WaitCursor);

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_operators->selectionModel()->selection().indexes();
    this->getters_id.clear();
    this->getters_name.clear();
    QString getters = (indexes.size()>0 ? "" : "?");

    for (int i=0; i<indexes.size(); i++) {
        QModelIndex index = indexes.at(i);
        int col = index.column();
        int row = index.row();
        if (col==0) {
            QModelIndex index = indexes.at(i);

            this->getters_id.append(model_operators.data(model_operators.index(row, 2), Qt::EditRole).toInt());
            this->getters_name.append(model_operators.data(model_operators.index(row, 3), Qt::EditRole).toString());
            if (i>0)  getters += "\n";
            getters += model_operators.data(model_operators.index(row, 1), Qt::EditRole).toString() + ",   " +
                       model_operators.data(model_operators.index(row, 3), Qt::EditRole).toString();
        }
    }
    ui->lab_getter->setText(getters);

    this->setCursor(Qt::ArrowCursor);
}

void msg_wnd::on_bn_clear_clicked() {
    ui->te_msg->clear();
}

void msg_wnd::on_bn_oper_refresh_clicked() {
    refresh_operators_tab();
    ui->lab_getter->clear();
    this->getters_id.clear();
    this->getters_name.clear();
}

void msg_wnd::on_bn_send_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    // проверка, введено ли сообщение
    ui->te_msg->setText(ui->te_msg->toPlainText().trimmed().simplified());
    QString message = ui->te_msg->toPlainText();

    if ( message.isEmpty() ||
         message=="- введите сообщение -" ) {
        QMessageBox::warning(this, "Введите сообщение",
                             "Сообщение не может быть пустым.\n\nВведите текст сообщения.");
        return;
    }

    // проверка, указан ли получатель
    if ( this->getters_id.size()<1 ) {
        QMessageBox::warning(this, "Не указан получатель",
                             "Не указан получатель.\nВыделите получателя/получателей сообщения в таблице доступных адресатов.");
        return;
    }

    // отправим сообщение
    int n = this->getters_id.size();
    QFile hist_file(data_app.path_install + "msg_history.txt");
    hist_file.open(QIODevice::Append);

    for (int i=0; i<n; i++) {
        db.transaction();
        QString sql = "insert into msg (id_sender, id_getter, dt_send, dt_get, message, status) "
                      " values ( "
                      "   " + QString::number(data_app.id_operator) + ", "
                      "   " + QString::number(this->getters_id.at(i)) + ", "
                      "   CURRENT_TIMESTAMP, "
                      "   NULL, "
                      "   '" + message + "', "
                      "   0 ) "
                      " returning id ; ";

        QSqlQuery *query = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Добавление сообщения", *query, true, db, data_app) || !query->next()) {
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке добавить сообщение произошла неожиданная ошибка.\n\nСообщение не передлано.");
            delete query;
            db.rollback();
            return;
        }
        delete query;
        db.commit();

        // внесём запись в историю
        hist_file.write("\n\n  <<  ");
        hist_file.write(QString(QDate::currentDate().toString("yyyy-MM-dd") + "  ").toUtf8());
        hist_file.write(QString(QTime::currentTime().toString("hh:mm:ss") + "  ").toUtf8());
        hist_file.write(QString("сообщение к  ").toUtf8());
        hist_file.write(this->getters_name.at(i).toUtf8());
        hist_file.write(QString("\n").toUtf8());
        hist_file.write(message.toUtf8());
    }
    hist_file.close();

    ui->te_msg->clear();
    refresh_history();
}


// обработка таймера сообщений
void msg_wnd::msg_update() {
    db.transaction();
    QString sql = "select o.id, o.oper_fio, m.id, m.message, m.status "
                  " from msg m "
                  "      left join operators o on(o.id=m.id_sender) "
                  " where m.id_getter=" + QString::number(data_app.id_operator) + " and m.status=0 "
                  " order by m.id ; ";

    QSqlQuery *query = new QSqlQuery(db);
    if (!mySQL.exec(this, sql, "Проверка сообщений MSG", *query, true, db, data_app)) {
        db.rollback();
        QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке проверить сообщения MSG произошла неожиданная ошибка.\n\nСообщение не передлано.");
        delete query;
        return;
    }
    QFile hist_file(data_app.path_install + "msg_history.txt");
    hist_file.open(QIODevice::Append);

    while (query->next()) {
        int id = query->value(2).toInt();

        // внесём запись в историю
        hist_file.write("\n  >>  ");
        hist_file.write(QString(QDate::currentDate().toString("yyyy-MM-dd") + "  ").toUtf8());
        hist_file.write(QString(QTime::currentTime().toString("hh:mm:ss") + "  ").toUtf8());
        hist_file.write(query->value(1).toString().toUtf8());
        hist_file.write("\n");
        hist_file.write(query->value(3).toString().toUtf8());

        // сбросим статус принятого сообщения
        QString sql = "update msg "
                      " set status=1, "
                      "     dt_get=CURRENT_TIMESTAMP "
                      " where id=" + QString::number(id) + " ; ";

        QSqlQuery *query_stat = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Проверка сообщений MSG", *query_stat, true, db, data_app)) {
            QMessageBox::critical(this, "Непредвиденная ошибка",
                                  "При попытке сбросить статус сообщения MSG произошла неожиданная ошибка.\n\nСообщение не принято.");
            delete query_stat;
            delete query;
            db.rollback();
            hist_file.close();
            return;
        }
        delete query_stat;
    }
    hist_file.close();
    refresh_history();

    delete query;
    db.commit();
}

void msg_wnd::on_msg_wnd_accepted() {
    if (msg_timer) {
        msg_timer->stop();
        delete msg_timer;
        msg_timer = NULL;
    }
}

void msg_wnd::on_msg_wnd_rejected() {
    if (msg_timer) {
        msg_timer->stop();
        delete msg_timer;
        msg_timer = NULL;
    }
}

void msg_wnd::on_msg_wnd_finished(int result) {
    if (msg_timer) {
        msg_timer->stop();
        delete msg_timer;
        msg_timer = NULL;
    }
}

void msg_wnd::on_bn_close_clicked() {
    accept();
}

void msg_wnd::on_bn_oper_all_clicked() {
    QString getters = "";
    this->getters_id.clear();
    this->getters_name.clear();

    QItemSelectionModel *sel_model = ui->tab_operators->selectionModel();
    QModelIndex ind;

    for (int i=0; i<model_operators.rowCount(); i++) {
        ind = model_operators.index(i, 0);
        sel_model->select(ind,QItemSelectionModel::Select);
        ind = model_operators.index(i, 1);
        sel_model->select(ind,QItemSelectionModel::Select);
        ind = model_operators.index(i, 2);
        sel_model->select(ind,QItemSelectionModel::Select);
        ind = model_operators.index(i, 3);
        sel_model->select(ind,QItemSelectionModel::Select);

        this->getters_id.append(model_operators.data(model_operators.index(i, 2), Qt::EditRole).toInt());
        this->getters_name.append(model_operators.data(model_operators.index(i, 3), Qt::EditRole).toString());
        if (i>0)  getters += "\n";
        getters += model_operators.data(model_operators.index(i, 1), Qt::EditRole).toString() + ",   " +
                   model_operators.data(model_operators.index(i, 3), Qt::EditRole).toString();
    }
    ui->lab_getter->setText(getters);
}
