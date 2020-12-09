#include "sessions_wnd.h"
#include "ui_sessions_wnd.h"

sessions_wnd::sessions_wnd(QSqlDatabase &db, QWidget *parent)
   : db(db), QDialog(parent), ui(new Ui::sessions_wnd)
{
    ui->setupUi(this);

    refresh_sessions_tab();
}

sessions_wnd::~sessions_wnd() {
    delete ui;
}

void sessions_wnd::on_bn_close_clicked() {
    close();
}

void sessions_wnd::refresh_sessions_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    QString sessions_sql = "select s.id, s.id_point, p.point_name, s.id_terminal, t.name, s.id_operator, o.oper_fio, s.dt_start, s.dt_last, s.status, s.dt_end "
                           " from db_sessions s"
                           "      left join operators o on(o.id=s.id_operator) "
                           "      left join db_terminals t on(t.id=s.id_terminal) "
                           "      left join points p on(p.id=s.id_point) ";
    if (ui->ch_open->isChecked()) {
        sessions_sql += " where s.status=1 ";
    }
    sessions_sql += " order by s.dt_start ; ";

    model_sessions.setQuery(sessions_sql,db);
    QString err2 = model_sessions.lastError().driverText();

    // подключаем модель из БД
    ui->tab_sessions->setModel(&model_sessions);

    // обновляем таблицу
    ui->tab_sessions->reset();

    // задаём ширину колонок
    ui->tab_sessions->setColumnWidth( 0,  1);     // id,
    ui->tab_sessions->setColumnWidth( 1,  1);     // ,
    ui->tab_sessions->setColumnWidth( 2,150);     // ,
    ui->tab_sessions->setColumnWidth( 3,  1);     // ,
    ui->tab_sessions->setColumnWidth( 4,150);     // ,
    ui->tab_sessions->setColumnWidth( 5,  1);     // ,
    ui->tab_sessions->setColumnWidth( 6,150);     // ,
    ui->tab_sessions->setColumnWidth( 7,120);     // ,
    ui->tab_sessions->setColumnWidth( 8,120);     // ,
    ui->tab_sessions->setColumnWidth( 9, 60);     // ,
    ui->tab_sessions->setColumnWidth(10,120);     // ,

    // правим заголовки
    model_sessions.setHeaderData( 2, Qt::Horizontal, ("ПВП"));
    model_sessions.setHeaderData( 4, Qt::Horizontal, ("терминал"));
    model_sessions.setHeaderData( 6, Qt::Horizontal, ("оператор"));
    model_sessions.setHeaderData( 7, Qt::Horizontal, ("начало \nсессии"));
    model_sessions.setHeaderData( 8, Qt::Horizontal, ("последнее \nобращение"));
    model_sessions.setHeaderData( 9, Qt::Horizontal, ("статус"));
    model_sessions.setHeaderData(10, Qt::Horizontal, ("конец \nсессии"));
    ui->tab_sessions->repaint();

    this->setCursor(Qt::ArrowCursor);
}

void sessions_wnd::on_bn_refresh_clicked() {
    refresh_sessions_tab();
}

void sessions_wnd::on_ch_open_clicked() {
    refresh_sessions_tab();
}
