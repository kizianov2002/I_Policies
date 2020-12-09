#include "resend_events_wnd.h"
#include "ui_resend_events_wnd.h"

#include <QMessageBox>

resend_events_wnd::resend_events_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    db(db), data_app(data_app), settings(settings),
    QDialog(parent),
    ui(new Ui::resend_events_wnd)
{
    ui->setupUi(this);

    QString sss = "";
    refresh_tab_events_data(sss);
}

resend_events_wnd::~resend_events_wnd() {
    delete ui;
}

void resend_events_wnd::on_bn_cansel_clicked() {
    reject();
}

void resend_events_wnd::on_line_events_editingFinished() {
    QString sss = ui->line_events->text();
    sss = sss.replace(";", " ").replace(".", " ").replace(",", " ").replace("|", " ");
    sss = sss.simplified().trimmed();

    // вычистим не-цифры
    QString s_ch = "0123456789 ";
    QString s, ss2;
    for (int i=0; i<sss.size(); i++) {
        s = sss.at(i);
        if (s_ch.indexOf(s)>-1)  ss2 += s;
    }
    ss2 = ss2.replace(" ",",");
    ui->line_events->setText(ss2);

    refresh_tab_events_data(ss2);
}

void resend_events_wnd::refresh_tab_events_data(QString ids) {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    QString sql_shows = "select a.id, a.event_code, a.event_dt, a.event_send, a.status, e.fam, e.im, e.ot, e.date_birth, p.pol_v, p.vs_num, p.pol_ser, p.pol_num, p.enp, p.date_begin, p.date_end, p.date_stop, p.date_activate, p.date_get2hand "
                        "  from events a "
                        "  left join polises p on(p.id=a.id_polis)"
                        "  left join persons e on(e.id=p.id_person) "
                        " where a.id in(-1" + ids + ") "
                        " order by a.id ; ";

    model_events_data.setQuery(sql_shows,db);
    QString err2 = model_events_data.lastError().driverText();

    // подключаем модель из БД
    ui->tab_events_data->setModel(&model_events_data);

    // обновляем таблицу
    ui->tab_events_data->reset();

    // задаём ширину колонок
    ui->tab_events_data->setColumnWidth( 0, 80);     // a.id,
    ui->tab_events_data->setColumnWidth( 1, 65);     // a.event_code,
    ui->tab_events_data->setColumnWidth( 2, 75);     // a.event_dt,
    ui->tab_events_data->setColumnWidth( 3, 70);     // a.event_send,
    ui->tab_events_data->setColumnWidth( 4, 40);     // a.status,
    ui->tab_events_data->setColumnWidth( 5, 90);     // e.fam,
    ui->tab_events_data->setColumnWidth( 6, 80);     // e.im,
    ui->tab_events_data->setColumnWidth( 7, 90);     // e.ot,
    ui->tab_events_data->setColumnWidth( 8, 70);     // e.date_birth,
    ui->tab_events_data->setColumnWidth( 9, 50);     // p.pol_v,
    ui->tab_events_data->setColumnWidth(10, 70);     // p.vs_num,
    ui->tab_events_data->setColumnWidth(11, 50);     // p.pol_ser,
    ui->tab_events_data->setColumnWidth(12, 60);     // p.pol_num,
    ui->tab_events_data->setColumnWidth(13,110);     // p.enp,
    ui->tab_events_data->setColumnWidth(14, 70);     // p.date_begin,
    ui->tab_events_data->setColumnWidth(15, 70);     // p.date_end,
    ui->tab_events_data->setColumnWidth(16, 70);     // p.date_stop,
    ui->tab_events_data->setColumnWidth(17, 70);     // p.date_activate,
    ui->tab_events_data->setColumnWidth(18, 70);     // p.date_get2hand

    // правим заголовки
    model_events_data.setHeaderData( 1, Qt::Horizontal, ("событие"));
    model_events_data.setHeaderData( 2, Qt::Horizontal, ("дата"));
    model_events_data.setHeaderData( 3, Qt::Horizontal, ("дата\nотправки"));
    model_events_data.setHeaderData( 4, Qt::Horizontal, ("стат."));
    model_events_data.setHeaderData( 5, Qt::Horizontal, ("фамилия"));
    model_events_data.setHeaderData( 6, Qt::Horizontal, ("имя"));
    model_events_data.setHeaderData( 7, Qt::Horizontal, ("отчество"));
    model_events_data.setHeaderData( 8, Qt::Horizontal, ("дата\nрождения"));
    model_events_data.setHeaderData( 9, Qt::Horizontal, ("тип\nполиса"));
    model_events_data.setHeaderData(10, Qt::Horizontal, ("№ ВС"));
    model_events_data.setHeaderData(11, Qt::Horizontal, ("серия\nполиса"));
    model_events_data.setHeaderData(12, Qt::Horizontal, ("№\nполиса"));
    model_events_data.setHeaderData(13, Qt::Horizontal, ("ЕНП"));
    model_events_data.setHeaderData(14, Qt::Horizontal, ("дата\nначала"));
    model_events_data.setHeaderData(15, Qt::Horizontal, ("дата\nизъятия"));
    model_events_data.setHeaderData(16, Qt::Horizontal, ("план по\nокончанию"));
    model_events_data.setHeaderData(17, Qt::Horizontal, ("дата\nактиваци"));
    model_events_data.setHeaderData(18, Qt::Horizontal, ("выдача\nна руки"));
    ui->tab_events_data->repaint();

    this->setCursor(Qt::ArrowCursor);
}


void resend_events_wnd::on_bn_send_clicked() {
    if (ui->line_events->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Нет данных", "Список ID событий пустой!");
        return;
    }
    // подготовим список ID событий
    QString sss = ui->line_events->text();
    sss = sss.replace(";", " ").replace(".", " ").replace(",", " ").replace("|", " ");
    sss = sss.simplified().trimmed();
    QStringList sss_lst = sss.split(" ");

    if (sss_lst.count()<1) {
        QMessageBox::warning(this, "Нет данных", "Из списка ID выбрано 0 значений!");
        return;
    }

    QString msg = "Вы действительно хотите повторно отправить в ТФОМС текущим числом " + QString::number(sss_lst.count()) + " записей событий?\n"
                  "Внимание - это необратимое действие.\n\n"
                  "ID событий:\n";
    QString ids = "";
    for (int i=0; i<sss_lst.count(); i++) {
        ids += ", ";
        ids += sss_lst.at(i);
    }
    msg += ids;
    msg += "\n\nПри утвердительном ответе эти события будут отправлены в ТФОМС при выгрузке первой же посылки.";
    if (QMessageBox::question(this, "Нужно подтверждение", msg, QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel)==QMessageBox::Cancel) {
        return;

    } else {
        refresh_tab_events_data(ids);

        db.transaction();
        // выполним необходимые действия для переотправки записей в ТФОМС
        QString sql_resend = "update events "
                             "   set duty_field = duty_field||'  ! '||CURRENT_DATE||' - сделана попытка повторно переотправить это событие в рамках массовой переотправки в ТФОМС. Попытка отклонена - событие итак ещё не отправленно.' "
                             " where id in(-1" + ids + ") "
                             "   and status=0 ; "

                             "update events "
                             "   set status=0, "
                             "       duty_field = duty_field||'  ! '||CURRENT_DATE||' - дата этого события была принудительно поправлена на текущую в рамках массовой переотправки событий в ТФОМС.' "
                             " where id in(-1" + ids + ") "
                             "   and status<>0 ; "

                             "update events a "
                             "   set event_dt=CURRENT_TIMESTAMP "
                             " where a.id in(-1" + ids + ") "
                             "   and (select p.tfoms_id from polises p where p.id=a.id_polis) is NULL ; "

                             "update polises "
                             "   set date_activate=CURRENT_DATE, "
                             "       date_get2hand=CURRENT_DATE  "
                             " where id in(SELECT ID_POLIS FROM EVENTS WHERE ID IN(-1" + ids + ")) "
                             "   and tfoms_id is NULL ; "

                             "update vizits v "
                             "   set v_date=CURRENT_DATE "
                             " where v.id in(SELECT ID_VIZIT FROM EVENTS WHERE ID IN(-1" + ids + ")) "
                             "   and (select p.tfoms_id from events a left join polises p on(p.id=a.id_polis) where a.id_vizit=v.id) is NULL ; ";

        QSqlQuery *query_resend = new QSqlQuery(db);
        bool res_resend = query_resend->exec(sql_resend);
        QString err_resend = db.lastError().databaseText();
        if (!res_resend) {
            delete query_resend;
            db.rollback();
            QMessageBox::warning(this, "Ошибка при попытке переотправить данные в ТФОМС",
                                 "При попытке переотправить данные в ТФОМС произошла ошибка!\n\n"
                                 + sql_resend + "\n\n"
                                 "Операция отменена");
            return;
        }
        delete query_resend;
        db.commit();

        QMessageBox::warning(this, "Данные будут переотправлены в ТФОМС",
                             "Указанные данные будут переотправлены в ТФОМС при первой же выгрузке посылки для фонда.\n");
        accept();
        return;
    }
}

void resend_events_wnd::on_bn_clear_clicked() {
    ui->line_events->clear();
    refresh_tab_events_data(ui->line_events->text());
}
