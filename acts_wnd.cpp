#include "acts_wnd.h"
#include "ui_acts_wnd.h"

acts_wnd::acts_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    db(db),
    data_app(data_app),
    settings(settings),
    QDialog(parent),
    ui(new Ui::acts_wnd)
{
    ui->setupUi(this);

    show_tab_w = NULL;
    act_vs2point_w = NULL;
    act_pol2smo_w = NULL;
    act_pol2point_w = NULL;
    print_blanks_pol_act_w = NULL;
    print_blanks_act_w = NULL;

    ui->split_main->setStretchFactor(0,4);
    ui->split_main->setStretchFactor(1,4);
    ui->split_vs->setStretchFactor(0,1);
    ui->split_vs->setStretchFactor(1,2);
    ui->split_pol->setStretchFactor(0,2);
    ui->split_pol->setStretchFactor(1,3);

    ui->ch_filter_vs2point->setChecked(false);
    ui->ch_filter_pol2point->setChecked(false);
    ui->pan_filter_vs2point->setVisible(false);
    ui->pan_filter_pol2point->setVisible(false);

    ui->date_act_vs2smo->setDate(QDate::currentDate());
    ui->date_act_vs2point->setDate(QDate::currentDate());
    ui->date_act_pol2smo->setDate(QDate::currentDate());
    ui->date_act_pol2point->setDate(QDate::currentDate());

    refresh_vs2smo_tab();
    refresh_vs2point_tab();
    refresh_pol2smo_tab();
    refresh_pol2point_tab();

    refresh_combo_sender_vs2point();
    refresh_combo_point_vs2point();
    refresh_combo_obtainer_vs2point(ui->combo_point_vs2point->currentData().toInt());


    // ---------------------
    // выберем головной офис
    refresh_combo_head_point_pol2smo();
    // восстановим прежнее значение
    if (settings.value("polis_acts/head_regnum", "").toString()!="") {
        data_app.head_act_regnum = QString::fromUtf8(settings.value("polis_acts/head_regnum", " ").toByteArray());
        ui->combo_head_point_pol2smo->setCurrentIndex(head_regnums.indexOf(data_app.head_act_regnum));
    } else
        ui->combo_head_point_pol2smo->setCurrentIndex(0);

    // ----------------------------------
    // выберем сотрудника головного офиса
    refresh_combo_obtainer_pol2smo(ui->combo_head_point_pol2smo->currentData().toInt());
    // восстановим прежнее значение
    if (settings.value("polis_acts/head_obtainer", "").toString()!="") {
        data_app.head_act_obtainer = QString::fromUtf8(settings.value("polis_acts/head_obtainer", " ").toByteArray());
        ui->combo_obtainer_pol2smo->setCurrentIndex(ui->combo_obtainer_pol2smo->findText(data_app.head_act_obtainer));
    } else
        ui->combo_obtainer_pol2smo->setCurrentIndex(0);


    refresh_combo_sender_pol2point();
    refresh_combo_point_pol2point();
    refresh_combo_obtainer_pol2point(ui->combo_point_pol2point->currentData().toInt());
}

acts_wnd::~acts_wnd() {
    delete ui;
}

void acts_wnd::on_bn_close_clicked() {
    close();
}


QString acts_wnd::month_to_str(QDate date) {
    QString res;
    switch (date.month()) {
    case 1:
        res += "января ";
        break;
    case 2:
        res += "февраля ";
        break;
    case 3:
        res += "марта ";
        break;
    case 4:
        res += "апреля ";
        break;
    case 5:
        res += "мая ";
        break;
    case 6:
        res += "июня ";
        break;
    case 7:
        res += "июля ";
        break;
    case 8:
        res += "августа ";
        break;
    case 9:
        res += "сентября ";
        break;
    case 10:
        res += "октября ";
        break;
    case 11:
        res += "ноября ";
        break;
    case 12:
        res += "декабря ";
        break;
    default:
        break;
    }
    return res;
}


QString acts_wnd::date_to_str(QDate date) {
    QString date_s = " «" + QString::number(date.day()) + "» " + month_to_str(date) + " " + QString::number(date.year()) + " г. ";
    return date_s;
}



void acts_wnd::refresh_vs2smo_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    sql_vs2smo = "select * "
                 "  from ( "
                 "    select i1.id, i1.act_num, i1.act_date, i1.date_get, i1.vs_from, i1.vs_to, i1.vs_count, count(*) as cnt "
                 "      from blanks_vs b "
                 "      left join vs_intervals i1 on(i1.id=b.id_vs_interval) "
                 "     group by i1.id "
                 "  union "
                 "    select -1 as id, ' !!! нет акта !!! ' as act_num, NULL, NULL, NULL as vs_from, NULL as vs_to, NULL as vs_count, count(*) as cnt "
                 "      from blanks_vs b "
                 "      left join vs_intervals i on(i.id=b.id_vs_interval) "
                 "     where i.id is NULL "
                 "    ) q ";
    switch (ui->combo_ord_vs2smo->currentIndex()) {
    case 0:
        sql_vs2smo += "  order by q.vs_from, q.act_date ; ";
        break;
    case 1:
        sql_vs2smo += "  order by q.act_date, q.vs_from ; ";
        break;
    default:
        sql_vs2smo += "  order by q.vs_from, q.act_date ; ";
        break;
    }

    model_vs2smo.setQuery(sql_vs2smo,db);
    QString err2 = model_vs2smo.lastError().driverText();

    // подключаем модель из БД
    ui->tab_vs2smo->setModel(&model_vs2smo);

    // обновляем таблицу
    ui->tab_vs2smo->reset();

    // задаём ширину колонок
    ui->tab_vs2smo->setColumnWidth( 0,  2);     // id,
    ui->tab_vs2smo->setColumnWidth( 1,110);     // act_num,
    ui->tab_vs2smo->setColumnWidth( 2, 80);     // act_date,
    ui->tab_vs2smo->setColumnWidth( 3, 80);     // date_get,
    ui->tab_vs2smo->setColumnWidth( 4, 80);     // vs_from,
    ui->tab_vs2smo->setColumnWidth( 5, 80);     // vs_to
    ui->tab_vs2smo->setColumnWidth( 6, 70);     // число \nВС
    ui->tab_vs2smo->setColumnWidth( 7, 70);     // число \nстрок

    // правим заголовки
    model_vs2smo.setHeaderData( 1, Qt::Horizontal, ("номер\nакта"));
    model_vs2smo.setHeaderData( 2, Qt::Horizontal, ("дата\nакта"));
    model_vs2smo.setHeaderData( 3, Qt::Horizontal, ("дата\nполучения"));
    model_vs2smo.setHeaderData( 4, Qt::Horizontal, ("№ ВС, от"));
    model_vs2smo.setHeaderData( 5, Qt::Horizontal, ("№ ВС, до"));
    model_vs2smo.setHeaderData( 6, Qt::Horizontal, ("интервал"));
    model_vs2smo.setHeaderData( 7, Qt::Horizontal, ("число \nВС"));
    ui->tab_vs2smo->repaint();

    this->setCursor(Qt::ArrowCursor);
}

void acts_wnd::refresh_vs2point_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    sql_vs2point  = "select q.id, q.act_num, q.act_date, q.date_add, q.id_sender, q.sender, q.id_point, q.point, q.id_obtainer, q.obtainer, coalesce(q_cnt.cnt,'0')||' / '||coalesce(q2.cnt,'0'), q.status "
                    "  from "
                    "   ( select a.id, a.act_num, a.act_date, a.date_add, a.id_sender, '('||s.point_regnum||')  '||s.point_name as sender, a.id_point, '('||p.point_regnum||')  '||p.point_name as point, a.id_obtainer, o.oper_fio as obtainer, count(*) as cnt, a.status "
                    "       from blanks_vs_acts a "
                    "       left join points s on(s.id=a.id_sender) "
                    "       left join points p on(p.id=a.id_point) "
                    "       left join operators o on(o.id=a.id_obtainer) "
                    "       left join blanks_vs b on(b.id_blanks_vs_act=a.id) "
                    "      where replace(a.act_num,' / ','/') like(replace('%" + ui->ln_filter_vs2point->text() + "%',' / ','/')) "
                    "      group by a.id, a.act_num, a.act_date, a.date_add, a.id_sender, s.point_regnum, s.point_name, a.id_point, p.point_regnum, p.point_name, a.id_obtainer, o.oper_fio "
                    "    union "
                    "     select -1, ' !!! нет акта !!! ' as act_num, NULL, NULL, NULL, NULL as sender, NULL as id_point, NULL as point, NULL as id_obtainer, NULL as obtainer, count(*) as cnt, NULL "
                    "      from blanks_vs b "
                    "     where b.id_blanks_vs_act is NULL "
                    "   ) q "
                    "  left join "
                    "   ( select a.id, count(*) as cnt "
                    "       from blanks_vs_acts a "
                    "       left join blanks_vs b on(b.id_blanks_vs_act=a.id) "
                    "      where replace(a.act_num,' / ','/') like(replace('%" + ui->ln_filter_vs2point->text() + "%',' / ','/')) "
                    "        and b.status=0 "
                    "      group by a.id "
                    "    union "
                    "     select -1, count(*) as cnt "
                    "       from blanks_vs b "
                    "      where b.id_blanks_vs_act is NULL "
                    "   ) q2 on(q2.id=q.id) "
                    "  left join "
                    "   ( select a.id, count(*) as cnt "
                    "       from blanks_vs_acts a "
                    "            join blanks_vs b on(b.id_blanks_vs_act=a.id) "
                    "      where replace(a.act_num,' / ','/') like(replace('%" + ui->ln_filter_vs2point->text() + "%',' / ','/')) "
                    "      group by a.id "
                    "    union "
                    "     select -1, count(*) as cnt "
                    "       from blanks_vs b "
                    "      where b.id_blanks_vs_act is NULL "
                    "   ) q_cnt on(q_cnt.id=q.id) ";
    switch (ui->combo_ord_vs2point->currentIndex()) {
    case 0:
        sql_vs2point += " order by q.act_date desc, q.act_num desc, q.date_add desc ; ";
        break;
    case 1:
        sql_vs2point += " order by q.act_num desc, q.act_date desc, q.date_add desc ; ";
        break;
    case 2:
        sql_vs2point += " order by q.date_add desc, q.act_date desc, q.act_num desc ; ";
        break;
    case 3:
        sql_vs2point += " order by q.sender, q.point, q.date_add desc, q.act_date desc, q.act_num desc ; ";
        break;
    case 4:
        sql_vs2point += " order by q.point, q.sender, q.date_add desc, q.act_date desc, q.act_num desc ; ";
        break;
    default:
        sql_vs2point += " order by q.act_date desc, q.act_num desc, q.date_add desc ; ";
        break;
    }

    model_vs2point.setQuery(sql_vs2point,db);
    QString err2 = model_vs2point.lastError().driverText();

    // подключаем модель из БД
    ui->tab_vs2point->setModel(&model_vs2point);

    // обновляем таблицу
    ui->tab_vs2point->reset();

    // задаём ширину колонок
    ui->tab_vs2point->setColumnWidth( 0,  2);     // a.id,
    ui->tab_vs2point->setColumnWidth( 1,100);     // a.act_num,
    ui->tab_vs2point->setColumnWidth( 2, 70);     // a.act_date,
    ui->tab_vs2point->setColumnWidth( 3,  2);     // a.date_add,
    ui->tab_vs2point->setColumnWidth( 4,  2);     // a.id_sender,
    ui->tab_vs2point->setColumnWidth( 5,100);     // '('||s.point_regnum||')  '||s.point_name as sender,
    ui->tab_vs2point->setColumnWidth( 6,  2);     // a.id_point,
    ui->tab_vs2point->setColumnWidth( 7,150);     // '('||p.point_regnum||')  '||p.point_name as point,
    ui->tab_vs2point->setColumnWidth( 8,  2);     // a.id_obtainer,
    ui->tab_vs2point->setColumnWidth( 9,100);     // o.oper_fio as obtainer
    ui->tab_vs2point->setColumnWidth(10, 70);     // cnt
    ui->tab_vs2point->setColumnWidth(11, 70);     // status

    // правим заголовки
    model_vs2point.setHeaderData( 1, Qt::Horizontal, ("номер \nакта"));
    model_vs2point.setHeaderData( 2, Qt::Horizontal, ("дата\nакта"));
    model_vs2point.setHeaderData( 3, Qt::Horizontal, ("дата\nсоздания"));
    model_vs2point.setHeaderData( 5, Qt::Horizontal, ("передающая \nсторона"));
    model_vs2point.setHeaderData( 7, Qt::Horizontal, ("получающая \nсторона"));
    model_vs2point.setHeaderData( 9, Qt::Horizontal, ("ответственное \nлицо"));
    model_vs2point.setHeaderData(10, Qt::Horizontal, ("ВС /\nосталось"));
    model_vs2point.setHeaderData(11, Qt::Horizontal, ("испол-\nнен?"));
    ui->tab_vs2point->repaint();

    this->setCursor(Qt::ArrowCursor);
}

void acts_wnd::refresh_pol2smo_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    sql_pol2smo = "select q.id, q.act_num, q.act_date, q.date_add, q.id_head_point, q.point, q.id_obtainer, q.obtainer, q_cnt.cnt "
                  "  from ( "
                  "    select a.id, a.act_num, a.act_date, a.date_add, a.id_head_point, '('||p.point_regnum||')  '||p.point_name as point, a.id_obtainer, o.oper_fio as obtainer, count(*) as cnt "
                  "      from blanks_boxes_acts a "
                  "      left join points p on(p.id=a.id_head_point) "
                  "      left join operators o on(o.id=a.id_obtainer) "
                  "      left join blanks_pol b on( b.id_blanks_box_act=a.id /*((b.pol_ser is NULL and bb.pol_ser is NULL) or (b.pol_ser=bb.pol_ser)) and cast(b.pol_num as integer)=cast(bb.pol_num as integer)*/ ) "
                  "     group by a.id, a.act_num, a.act_date, a.date_add, a.id_head_point, p.point_regnum, p.point_name, a.id_obtainer, o.oper_fio "
                  "   union "
                  "    select -1, ' !!! нет акта !!! ' as act_num, NULL as act_date, NULL as date_add, NULL as id_head_point, NULL as point, NULL as id_obtainer, NULL as obtainer, count(*) as cnt "
                  "      from blanks_pol b "
                  "     where b.id_blanks_box_act is NULL "
                  "   ) q "
                  "  left join "
                  "   ( select a.id, count(*) as cnt "
                  "       from blanks_boxes_acts a "
                  "            join blanks_pol b on(b.id_blanks_box_act=a.id) "
                  "      group by a.id "
                  "    union "
                  "     select -1, count(*) as cnt "
                  "       from blanks_pol b "
                  "      where b.id_blanks_box_act is NULL "
                  "   ) q_cnt on(q_cnt.id=q.id) ";
    switch (ui->combo_ord_pol2smo->currentIndex()) {
    case 0:
        sql_pol2smo += " order by q.act_date desc, q.act_num desc, q.date_add desc ; ";
        break;
    case 1:
        sql_pol2smo += " order by q.act_num desc, q.act_date desc, q.date_add desc ; ";
        break;
    case 2:
        sql_pol2smo += " order by q.date_add desc, q.act_date desc, q.act_num desc ; ";
        break;
    default:
        sql_pol2smo += " order by q.act_date desc, q.act_num desc, q.date_add desc ; ";
        break;
    }

    model_pol2smo.setQuery(sql_pol2smo,db);
    QString err2 = model_pol2smo.lastError().driverText();

    // подключаем модель из БД
    ui->tab_pol2smo->setModel(&model_pol2smo);

    // обновляем таблицу
    ui->tab_pol2smo->reset();

    // задаём ширину колонок
    ui->tab_pol2smo->setColumnWidth( 0,  2);     // a.id,
    ui->tab_pol2smo->setColumnWidth( 1,100);     // a.act_num,
    ui->tab_pol2smo->setColumnWidth( 2, 70);     // a.act_date,
    ui->tab_pol2smo->setColumnWidth( 3,  2);     // a.date_add,
    ui->tab_pol2smo->setColumnWidth( 4,  2);     // a.id_head_point,
    ui->tab_pol2smo->setColumnWidth( 5,200);     // '('||p.point_regnum||')  '||p.point_name as point,
    ui->tab_pol2smo->setColumnWidth( 6,  2);     // a.id_obtainer,
    ui->tab_pol2smo->setColumnWidth( 7,140);     // o.oper_fio as obtainer
    ui->tab_pol2smo->setColumnWidth( 8, 70);     // cnt
    // правим заголовки
    model_pol2smo.setHeaderData( 1, Qt::Horizontal, ("номер \nакта"));
    model_pol2smo.setHeaderData( 2, Qt::Horizontal, ("дата\nакта"));
    model_pol2smo.setHeaderData( 3, Qt::Horizontal, ("дата\nсоздания"));
    model_pol2smo.setHeaderData( 5, Qt::Horizontal, ("получающая \nсторона"));
    model_pol2smo.setHeaderData( 7, Qt::Horizontal, ("ответственное \nлицо"));
    model_pol2smo.setHeaderData( 8, Qt::Horizontal, ("число \nбланков"));
    ui->tab_pol2smo->repaint();

    this->setCursor(Qt::ArrowCursor);
}

void acts_wnd::refresh_pol2point_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    sql_pol2point = "select q.id, q.act_num, q.act_date, q.date_add, q.id_sender, q.sender, q.id_point, q.point, q.id_obtainer, q.obtainer, coalesce(q_cnt.cnt,'0')||' / '||coalesce(q2.cnt,'0'), q.status "
                    "  from "
                    "   ( select a.id, a.act_num, a.act_date, a.date_add, a.id_sender, '('||s.point_regnum||')  '||s.point_name as sender, a.id_point, '('||p.point_regnum||')  '||p.point_name as point, a.id_obtainer, o.oper_fio as obtainer, count(*) as cnt, a.status "
                    "       from blanks_pol_acts a "
                    "       left join blanks_pol b on(b.id_blanks_pol_act=a.id) "
                    "       left join points s on(s.id=a.id_sender) "
                    "       left join points p on(p.id=a.id_point) "
                    "       left join operators o on(o.id=a.id_obtainer) "
                    "      where replace(a.act_num,' / ','/') like(replace('%" + ui->ln_filter_pol2point->text() + "%',' / ','/')) "
                    "      group by a.id, a.act_num, a.act_date, a.date_add, a.id_sender, s.point_regnum, s.point_name, a.id_point, p.point_regnum, p.point_name, a.id_obtainer, o.oper_fio "
                    "   union "
                    "     select -1, ' !!! нет акта !!! ' as act_num, NULL as act_date, NULL as date_add, NULL as id_sender, NULL as sender, NULL as id_point, NULL as point, NULL as id_obtainer, NULL as obtainer, count(*) as cnt, NULL "
                    "       from blanks_pol b "
                    "      where b.id_blanks_pol_act is NULL "
                    "   ) q "
                    "  left join "
                    "   ( select a.id, count(*) as cnt "
                    "       from blanks_pol_acts a "
                    "       left join blanks_pol b on(b.id_blanks_pol_act=a.id) "
                    "      where replace(a.act_num,' / ','/') like(replace('%" + ui->ln_filter_pol2point->text() + "%',' / ','/')) "
                    "        and b.status=0 "
                    "      group by a.id "
                    "    union "
                    "     select -1, count(*) as cnt "
                    "       from blanks_pol b "
                    "      where b.id_blanks_pol_act is NULL "
                    "   ) q2 on(q2.id=q.id) "
                    "  left join "
                    "   ( select a.id, count(*) as cnt "
                    "       from blanks_pol_acts a "
                    "            join blanks_pol b on(b.id_blanks_pol_act=a.id) "
                    "      where replace(a.act_num,' / ','/') like(replace('%" + ui->ln_filter_pol2point->text() + "%',' / ','/')) "
                    "      group by a.id "
                    "    union "
                    "     select -1, count(*) as cnt "
                    "       from blanks_pol b "
                    "      where b.id_blanks_pol_act is NULL "
                    "   ) q_cnt on(q_cnt.id=q.id) ";
    switch (ui->combo_ord_pol2point->currentIndex()) {
    case 0:
        sql_pol2point += " order by q.act_date desc, q.act_num desc, q.date_add desc ; ";
        break;
    case 1:
        sql_pol2point += " order by q.act_num desc, q.act_date desc, q.date_add desc ; ";
        break;
    case 2:
        sql_pol2point += " order by q.date_add desc, q.act_date desc, q.act_num desc ; ";
        break;
    case 3:
        sql_pol2point += " order by q.sender, q.point, q.date_add desc, q.act_date desc, q.act_num desc ; ";
        break;
    case 4:
        sql_pol2point += " order by q.point, q.sender, q.date_add desc, q.act_date desc, q.act_num desc ; ";
        break;
    default:
        sql_pol2point += " order by q.act_date desc, q.act_num desc, q.date_add desc ; ";
        break;
    }

    model_pol2point.setQuery(sql_pol2point,db);
    QString err2 = model_pol2point.lastError().driverText();

    // подключаем модель из БД
    ui->tab_pol2point->setModel(&model_pol2point);

    // обновляем таблицу
    ui->tab_pol2point->reset();

    // задаём ширину колонок
    ui->tab_pol2point->setColumnWidth( 0,  2);     // a.id,
    ui->tab_pol2point->setColumnWidth( 1,100);     // a.act_num,
    ui->tab_pol2point->setColumnWidth( 2, 70);     // a.act_date,
    ui->tab_pol2point->setColumnWidth( 3,  2);     // a.date_add,
    ui->tab_pol2point->setColumnWidth( 4,  2);     // a.id_sender,
    ui->tab_pol2point->setColumnWidth( 5,100);     // '('||s.point_regnum||')  '||s.point_name as sender,
    ui->tab_pol2point->setColumnWidth( 6,  2);     // a.id_point,
    ui->tab_pol2point->setColumnWidth( 7,150);     // '('||p.point_regnum||')  '||p.point_name as point,
    ui->tab_pol2point->setColumnWidth( 8,  2);     // a.id_obtainer,
    ui->tab_pol2point->setColumnWidth( 9,100);     // o.oper_fio as obtainer
    ui->tab_pol2point->setColumnWidth(10, 70);     // cnt
    ui->tab_pol2point->setColumnWidth(11, 70);     // статус

    // правим заголовки
    model_pol2point.setHeaderData( 1, Qt::Horizontal, ("номер \nакта"));
    model_pol2point.setHeaderData( 2, Qt::Horizontal, ("дата\nакта"));
    model_pol2point.setHeaderData( 3, Qt::Horizontal, ("дата\nсоздания"));
    model_pol2point.setHeaderData( 5, Qt::Horizontal, ("передающая \nсторона"));
    model_pol2point.setHeaderData( 7, Qt::Horizontal, ("получающая \nсторона"));
    model_pol2point.setHeaderData( 9, Qt::Horizontal, ("ответственное \nлицо"));
    model_pol2point.setHeaderData(10, Qt::Horizontal, ("бланков /\nосталось"));
    model_pol2point.setHeaderData(11, Qt::Horizontal, ("испол-\nнен?"));
    ui->tab_pol2point->repaint();

    this->setCursor(Qt::ArrowCursor);
}

void acts_wnd::refresh_combo_sender_vs2point() {
    this->setCursor(Qt::WaitCursor);

    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "";
    sql += QString("select p.id, '('||p.point_regnum||') '||p.point_name as name"
                   " from points p "
                   " where point_regnum<>'000' "
                   "   and status>0 "
                   " order by p.point_regnum, id ; ");
    query->exec( sql );
    ui->combo_sender_vs2point->clear();
    ui->combo_sender_vs2point->addItem( " - головной офис - ", 0 );

    // заполнение списка пунктов выдачи
    while (query->next())
        ui->combo_sender_vs2point->addItem( query->value(1).toString(), query->value(0).toInt() );

    query->finish();
    delete query;
    ui->combo_sender_vs2point->setCurrentIndex(0);
    this->setCursor(Qt::ArrowCursor);
}

void acts_wnd::refresh_combo_point_vs2point() {
    this->setCursor(Qt::WaitCursor);

    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "";
    sql += QString("select p.id, '('||p.point_regnum||') '||p.point_name as name"
                   " from points p "
                   " where point_regnum<>'000' "
                   "   and status>0 "
                   " order by p.point_regnum, id ; ");
    query->exec( sql );
    ui->combo_point_vs2point->clear();

    // заполнение списка пунктов выдачи
    while (query->next())
        ui->combo_point_vs2point->addItem( query->value(1).toString(), query->value(0).toInt() );

    query->finish();
    delete query;
    ui->combo_point_vs2point->setCurrentIndex(0);
    this->setCursor(Qt::ArrowCursor);
}

void acts_wnd::refresh_combo_obtainer_vs2point(int id_point) {
    this->setCursor(Qt::WaitCursor);

    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "";
    sql += "select u.id, u.oper_fio "
           " from operators u "
           " where u.status>0 ";
    sql += (id_point==-1) ? (QString(" ")) : (QString(" and u.id_point=") + QString::number(id_point));
    sql += " order by u.oper_fio ; ";
    query->exec( sql );
    ui->combo_obtainer_vs2point->clear();

    // заполнение списка операторов
    while (query->next())
        ui->combo_obtainer_vs2point->addItem( query->value(1).toString(), query->value(0).toInt() );

    query->finish();
    delete query;
    ui->combo_obtainer_vs2point->setCurrentIndex(0);
    this->setCursor(Qt::ArrowCursor);
}

void acts_wnd::refresh_combo_head_point_pol2smo() {
    this->setCursor(Qt::WaitCursor);

    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "";
    sql += QString("select p.id, '('||p.point_regnum||') '||p.point_name as name, p.point_regnum "
                   " from points p "
                   " where point_regnum<>'000' "
                   "   and status>0 "
                   "   and point_rights=2 "
                   " order by p.point_regnum, id ; ");
    query->exec( sql );
    ui->combo_head_point_pol2smo->clear();
    head_regnums.clear();

    // заполнение списка пунктов выдачи
    while (query->next()) {
        ui->combo_head_point_pol2smo->addItem( query->value(1).toString(), query->value(0).toInt() );
        head_regnums.append(query->value(2).toString());
    }

    query->finish();
    delete query;
    ui->combo_head_point_pol2smo->setCurrentIndex(0);
    this->setCursor(Qt::ArrowCursor);
}

void acts_wnd::refresh_combo_obtainer_pol2smo(int id_point) {
    this->setCursor(Qt::WaitCursor);

    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "";
    sql += "select u.id, u.oper_fio "
           " from operators u "
           " where u.status>0 ";
    sql += (id_point==-1) ? (QString(" ")) : (QString(" and u.id_point=") + QString::number(id_point));
    sql += " order by u.oper_fio ; ";
    query->exec( sql );
    ui->combo_obtainer_pol2smo->clear();

    // заполнение списка операторов
    while (query->next())
        ui->combo_obtainer_pol2smo->addItem( query->value(1).toString(), query->value(0).toInt() );

    query->finish();
    delete query;
    ui->combo_obtainer_pol2smo->setCurrentIndex(0);
    this->setCursor(Qt::ArrowCursor);
}

void acts_wnd::refresh_combo_sender_pol2point() {
    this->setCursor(Qt::WaitCursor);

    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "";
    sql += QString("select p.id, '('||p.point_regnum||') '||p.point_name as name"
                   " from points p "
                   " where point_regnum<>'000' "
                   "   and status>0 "
                   " order by p.point_regnum, id ; ");
    query->exec( sql );
    ui->combo_sender_pol2point->clear();
    ui->combo_sender_pol2point->addItem( " - головной офис - ", 0 );

    // заполнение списка пунктов выдачи
    while (query->next())
        ui->combo_sender_pol2point->addItem( query->value(1).toString(), query->value(0).toInt() );

    query->finish();
    delete query;
    ui->combo_sender_pol2point->setCurrentIndex(0);
    this->setCursor(Qt::ArrowCursor);
}

void acts_wnd::refresh_combo_point_pol2point() {
    this->setCursor(Qt::WaitCursor);

    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "";
    sql += QString("select p.id, '('||p.point_regnum||') '||p.point_name as name"
                   " from points p "
                   " where point_regnum<>'000' "
                   "   and status>0 "
                   " order by p.point_regnum, id ; ");
    query->exec( sql );
    ui->combo_point_pol2point->clear();

    // заполнение списка пунктов выдачи
    while (query->next())
        ui->combo_point_pol2point->addItem( query->value(1).toString(), query->value(0).toInt() );

    query->finish();
    delete query;
    ui->combo_point_pol2point->setCurrentIndex(0);
    this->setCursor(Qt::ArrowCursor);
}

void acts_wnd::refresh_combo_obtainer_pol2point(int id_point) {
    this->setCursor(Qt::WaitCursor);

    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "";
    sql += "select u.id, u.oper_fio "
           " from operators u "
           " where u.status>0 ";
    sql += (id_point==-1) ? (QString(" ")) : (QString(" and u.id_point=") + QString::number(id_point));
    sql += " order by u.oper_fio ; ";
    query->exec( sql );
    ui->combo_obtainer_pol2point->clear();

    // заполнение списка операторов
    while (query->next())
        ui->combo_obtainer_pol2point->addItem( query->value(1).toString(), query->value(0).toInt() );

    query->finish();
    delete query;
    ui->combo_obtainer_pol2point->setCurrentIndex(0);
    this->setCursor(Qt::ArrowCursor);
}



void acts_wnd::on_bn_refresh_vs2smo_clicked() {
    refresh_vs2smo_tab();
}

void acts_wnd::on_bn_refresh_vs2point_clicked() {
    refresh_vs2point_tab();
}

void acts_wnd::on_bn_refresh_pol2smo_clicked() {
    refresh_pol2smo_tab();
}

void acts_wnd::on_bn_refresh_pol2point_clicked() {
    refresh_pol2point_tab();
}



void acts_wnd::on_combo_point_vs2point_activated(int index) {
    refresh_combo_obtainer_vs2point(ui->combo_point_vs2point->currentData().toInt());
}

void acts_wnd::on_combo_head_point_pol2smo_activated(int index) {
    refresh_combo_obtainer_pol2smo(ui->combo_head_point_pol2smo->currentData().toInt());
}

void acts_wnd::on_combo_point_pol2point_activated(int index) {
    refresh_combo_obtainer_pol2point(ui->combo_point_pol2point->currentData().toInt());
}

void acts_wnd::on_tab_vs2smo_clicked(const QModelIndex &index) {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_vs2smo || !ui->tab_vs2smo->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить].");
        return;
    }
    this->setCursor(Qt::WaitCursor);

    ui->line_act_num_vs2smo->setText (model_vs2smo.data(model_vs2smo.index(index.row(), 2), Qt::EditRole).toString());
    ui->date_act_vs2smo->setDate (model_vs2smo.data(model_vs2smo.index(index.row(), 3), Qt::EditRole).toDate());

    //ui->date_blank_vs_add->setDate (model_blanks_vs.data(model_blanks_vs.index(index.row(), 1), Qt::EditRole).toDate());
    //ui->combo_blank_vs_status->setCurrentIndex (ui->combo_blank_vs_status->findData(model_blanks_vs.data(model_blanks_vs.index(index.row(), 2), Qt::EditRole).toInt()));

    this->setCursor(Qt::ArrowCursor);
}

void acts_wnd::on_tab_vs2point_clicked(const QModelIndex &index) {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_vs2point || !ui->tab_vs2point->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!", "Таблица не загружена!\nНажмите кнопку [Обновить].");
        return;
    }
    this->setCursor(Qt::WaitCursor);

    ui->line_act_num_vs2point->setText (model_vs2point.data(model_vs2point.index(index.row(), 1), Qt::EditRole).toString());
    ui->date_act_vs2point->setDate (model_vs2point.data(model_vs2point.index(index.row(), 2), Qt::EditRole).toDate());
    ui->ch_vs2point_apply->setChecked (model_vs2point.data(model_vs2point.index(index.row(), 11), Qt::EditRole).toInt()>0);

    ui->combo_sender_vs2point->setCurrentIndex (ui->combo_sender_vs2point->findData(model_vs2point.data(model_vs2point.index(index.row(), 4), Qt::EditRole).toInt()));
    ui->combo_point_vs2point->setCurrentIndex (ui->combo_point_vs2point->findData(model_vs2point.data(model_vs2point.index(index.row(), 6), Qt::EditRole).toInt()));
    refresh_combo_obtainer_vs2point(model_vs2point.data(model_vs2point.index(index.row(), 6), Qt::EditRole).toInt());
    ui->combo_obtainer_vs2point->setCurrentIndex (ui->combo_obtainer_vs2point->findData(model_vs2point.data(model_vs2point.index(index.row(), 8), Qt::EditRole).toInt()));

    this->setCursor(Qt::ArrowCursor);
}

void acts_wnd::on_tab_pol2smo_clicked(const QModelIndex &index) {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_pol2smo || !ui->tab_pol2smo->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!", "Таблица не загружена!\nНажмите кнопку [Обновить].");
        return;
    }
    this->setCursor(Qt::WaitCursor);

    ui->line_act_num_pol2smo->setText (model_pol2smo.data(model_pol2smo.index(index.row(), 1), Qt::EditRole).toString());
    ui->date_act_pol2smo->setDate (model_pol2smo.data(model_pol2smo.index(index.row(), 2), Qt::EditRole).toDate());

    ui->combo_head_point_pol2smo->setCurrentIndex (ui->combo_head_point_pol2smo->findData(model_pol2smo.data(model_pol2smo.index(index.row(), 4), Qt::EditRole).toInt()));
    refresh_combo_obtainer_pol2smo(model_pol2smo.data(model_pol2smo.index(index.row(), 4), Qt::EditRole).toInt());
    ui->combo_obtainer_pol2smo->setCurrentIndex (ui->combo_obtainer_pol2smo->findData(model_pol2smo.data(model_pol2smo.index(index.row(), 6), Qt::EditRole).toInt()));

    this->setCursor(Qt::ArrowCursor);
}

void acts_wnd::on_tab_pol2point_clicked(const QModelIndex &index) {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_pol2point || !ui->tab_pol2point->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!", "Таблица не загружена!\nНажмите кнопку [Обновить].");
        return;
    }
    this->setCursor(Qt::WaitCursor);

    ui->line_act_num_pol2point->setText (model_pol2point.data(model_pol2point.index(index.row(), 1), Qt::EditRole).toString());
    ui->date_act_pol2point->setDate (model_pol2point.data(model_pol2point.index(index.row(), 2), Qt::EditRole).toDate());
    ui->ch_pol2point_apply->setChecked (model_pol2point.data(model_pol2point.index(index.row(), 11), Qt::EditRole).toInt()>0);

    ui->combo_sender_pol2point->setCurrentIndex (ui->combo_sender_pol2point->findData(model_pol2point.data(model_pol2point.index(index.row(), 4), Qt::EditRole).toInt()));
    ui->combo_point_pol2point->setCurrentIndex (ui->combo_point_pol2point->findData(model_pol2point.data(model_pol2point.index(index.row(), 6), Qt::EditRole).toInt()));
    refresh_combo_obtainer_pol2point(model_pol2point.data(model_pol2point.index(index.row(), 6), Qt::EditRole).toInt());
    ui->combo_obtainer_pol2point->setCurrentIndex (ui->combo_obtainer_pol2point->findData(model_pol2point.data(model_pol2point.index(index.row(), 8), Qt::EditRole).toInt()));

    this->setCursor(Qt::ArrowCursor);
}



void acts_wnd::on_bn_edit_vs2smo_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_vs2smo || !ui->tab_vs2smo->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите строку в таблице.");
        return;
    }

    // проверка, введено ли номер акта
    if (ui->line_act_num_vs2smo->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Номер акта не может быть пустым.\n\nСохранение отменено.");
        return;
    }
    if (ui->date_act_vs2smo->date()<QDate(2014,1,1)) {
        QMessageBox::warning(this, "Недостаточно данных",
                             "Дата акта не может быть раньше 01.01.2014 !");
        return;
    }


    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_vs2smo->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Изменить данные выбранного акта?",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        QModelIndex index = indexes.front();
        int ind = model_vs2smo.data(model_vs2smo.index(index.row(), 0), Qt::EditRole).toInt();

        // проверим номер на уникальность
        QString sql = "select count(*) from vs_intervals  "
                      " where upper(act_num)='" + ui->line_act_num_vs2smo->text().trimmed().simplified().toUpper() + "' "
                      "   and id<>" + QString::number(ind) + " ; ";

        QSqlQuery *query = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Проверка названия акта на уникальность", *query, true, db, data_app) || !query->next()) {
            QMessageBox::warning(this, "Непредвиденная ошибка", "При попытке проверить акт на уникальность произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query;
            return;
        }
        if (query->value(0).toInt()>0) {
            QMessageBox::warning(this,
                                  "Такой номер акта уже есть",
                                  "В базе данных уже есть акт с таким номером.\nИзмените номер акта.\n\nСохранение отменено.");
            delete query;
            return;
        }

        db.transaction();
        // добавим акт
        QString sql_upd = "update vs_intervals "
                          "   set act_num='" + ui->line_act_num_vs2smo->text().trimmed().simplified() + "', "
                          "       act_date='" + ui->date_act_vs2smo->date().toString("yyyy-MM-dd") + "' "
                          " where id=" + QString::number(ind) + " ; ";

        QSqlQuery *query_upd = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_upd, "Обновление акта", *query_upd, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке изменить данные акта произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query_upd;
            return;
        }
        delete query_upd;
        db.commit();

        refresh_vs2smo_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void acts_wnd::on_bn_edit_vs2point_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_vs2point || !ui->tab_vs2point->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите строку в таблице.");
        return;
    }

    // проверим заполнение полей
    if (ui->line_act_num_vs2point->text().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных",
                             "Номер акта не может быть пустым !");
        return;
    }
    if (ui->date_act_vs2point->date()<QDate(2014,1,1)) {
        QMessageBox::warning(this, "Недостаточно данных",
                             "Дата акта не может быть раньше 01.01.2014 !");
        return;
    }
    if (ui->combo_point_vs2point->currentText().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных",
                             "Выберите принимающую сторону акта !");
        return;
    }
    if (ui->combo_obtainer_vs2point->currentText().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных",
                             "Выберите ответственное лицо принимающей стороны по акту !");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_vs2point->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Изменить данные выбранного акта?",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        QModelIndex index = indexes.front();
        int ind = model_vs2point.data(model_vs2point.index(index.row(), 0), Qt::EditRole).toInt();

        // проверим номер на уникальность
        QString sql = "select count(*) from blanks_vs_acts  "
                      " where upper(act_num)='" + ui->line_act_num_pol2point->text().trimmed().simplified().toUpper() + "' "
                      "   and id<>" + QString::number(ind) + " ; ";

        QSqlQuery *query = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Проверка названия акта на уникальность", *query, true, db, data_app) || !query->next()) {
            QMessageBox::warning(this, "Непредвиденная ошибка", "При попытке проверить акт на уникальность произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query;
            return;
        }
        if (query->value(0).toInt()>0) {
            QMessageBox::warning(this,
                                  "Такой номер акта уже есть",
                                  "В базе данных уже есть акт с таким номером.\nИзмените номер акта.\n\nСохранение отменено.");
            delete query;
            return;
        }






        db.transaction();
        // добавим акт
        QString sql_upd = "update blanks_vs_acts "
                          "   set act_num='" + ui->line_act_num_vs2point->text().trimmed().simplified() + "', "
                          "       act_date='" + ui->date_act_vs2point->date().toString("yyyy-MM-dd") + "', "
                          "       id_sender="   + QString::number(ui->combo_sender_vs2point->currentData().toInt()) + ", "
                          "       id_point="    + QString::number(ui->combo_point_vs2point->currentData().toInt()) + ", "
                          "       id_obtainer=" + QString::number(ui->combo_obtainer_vs2point->currentData().toInt()) + " "
                          " where id=" + QString::number(ind) + " ; ";

        QSqlQuery *query_upd = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_upd, "Обновление акта", *query_upd, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке изменить данные акта произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query_upd;
            return;
        }
        delete query_upd;
        db.commit();

        refresh_vs2point_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void acts_wnd::on_bn_edit_pol2smo_clicked() {    
    // проверим заполнение полей
    if (ui->line_act_num_pol2smo->text().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных",
                             "Номер акта не может быть пустым !");
        return;
    }
    if (ui->date_act_pol2smo->date()<QDate(2014,1,1)) {
        QMessageBox::warning(this, "Недостаточно данных",
                             "Дата акта не может быть раньше 01.01.2014 !");
        return;
    }
    if (ui->combo_head_point_pol2smo->currentText().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных",
                             "Выберите принимающую сторону акта !");
        return;
    }
    if (ui->combo_obtainer_pol2smo->currentText().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных",
                             "Выберите ответственное лицо принимающей стороны по акту !");
        return;
    }

    // запишем данные головного ПВП в INI-файл
    settings.beginGroup("polis_acts");
    settings.setValue("head_regnum", head_regnums.at(ui->combo_head_point_pol2smo->currentIndex()));
    settings.setValue("head_obtainer", ui->combo_obtainer_pol2smo->currentText());
    settings.endGroup();
    settings.sync();

    // собственно сохранение
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_pol2smo || !ui->tab_pol2smo->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите строку в таблице.");
        return;
    }

    // проверка, введено ли номер акта
    if (ui->line_act_num_pol2smo->text().trimmed().simplified().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Номер акта не может быть пустым.\n\nСохранение отменено.");
        return;
    }


    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_pol2smo->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Изменить данные выбранного акта?",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        QModelIndex index = indexes.front();
        int ind = model_pol2smo.data(model_pol2smo.index(index.row(), 0), Qt::EditRole).toInt();

        // проверим номер на уникальность
        QString sql = "select count(*) from blanks_boxes_acts  "
                      " where upper(act_num)='" + ui->line_act_num_pol2point->text().trimmed().simplified().toUpper() + "' "
                      "   and id<>" + QString::number(ind) + " ; ";

        QSqlQuery *query = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Проверка названия акта на уникальность", *query, true, db, data_app) || !query->next()) {
            QMessageBox::warning(this, "Непредвиденная ошибка", "При попытке проверить акт на уникальность произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query;
            return;
        }
        if (query->value(0).toInt()>0) {
            QMessageBox::warning(this,
                                  "Такой номер акта уже есть",
                                  "В базе данных уже есть акт с таким номером.\nИзмените номер акта.\n\nСохранение отменено.");
            delete query;
            return;
        }






        db.transaction();
        // добавим акт
        QString sql_upd = "update blanks_boxes_acts "
                          "   set act_num='" + ui->line_act_num_pol2smo->text().trimmed().simplified() + "', "
                          "       act_date='" + ui->date_act_pol2smo->date().toString("yyyy-MM-dd") + "', "
                          "       id_head_point="    + QString::number(ui->combo_head_point_pol2smo->currentData().toInt()) + ", "
                          "       id_obtainer=" + QString::number(ui->combo_obtainer_pol2smo->currentData().toInt()) + " "
                          " where id=" + QString::number(ind) + " ; ";

        QSqlQuery *query_upd = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_upd, "Обновление акта", *query_upd, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке изменить данные акта произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query_upd;
            return;
        }
        delete query_upd;
        db.commit();

        refresh_pol2smo_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void acts_wnd::on_bn_edit_pol2point_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_pol2point || !ui->tab_pol2point->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите строку в таблице.");
        return;
    }

    // проверим заполнение полей
    if (ui->line_act_num_pol2point->text().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных",
                             "Номер акта не может быть пустым !");
        return;
    }
    if (ui->date_act_pol2point->date()<QDate(2014,1,1)) {
        QMessageBox::warning(this, "Недостаточно данных",
                             "Дата акта не может быть раньше 01.01.2014 !");
        return;
    }
    if (ui->combo_point_pol2point->currentText().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных",
                             "Выберите принимающую сторону акта !");
        return;
    }
    if (ui->combo_obtainer_pol2point->currentText().isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных",
                             "Выберите ответственное лицо принимающей стороны по акту !");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_pol2point->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Изменить данные выбранного акта?",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        QModelIndex index = indexes.front();
        int ind = model_pol2point.data(model_pol2point.index(index.row(), 0), Qt::EditRole).toInt();

        // проверим номер на уникальность
        QString sql = "select count(*) from blanks_pol_acts  "
                      " where upper(act_num)='" + ui->line_act_num_pol2point->text().trimmed().simplified().toUpper() + "' "
                      "   and id<>" + QString::number(ind) + " ; ";

        QSqlQuery *query = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Проверка названия акта на уникальность", *query, true, db, data_app) || !query->next()) {
            QMessageBox::warning(this, "Непредвиденная ошибка", "При попытке проверить акт на уникальность произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query;
            return;
        }
        if (query->value(0).toInt()>0) {
            QMessageBox::warning(this,
                                  "Такой номер акта уже есть",
                                  "В базе данных уже есть акт с таким номером.\nИзмените номер акта.\n\nСохранение отменено.");
            delete query;
            return;
        }

        db.transaction();
        // добавим акт
        QString sql_upd = "update blanks_pol_acts "
                          "   set act_num='" + ui->line_act_num_pol2point->text().trimmed().simplified() + "', "
                          "       act_date='" + ui->date_act_pol2point->date().toString("yyyy-MM-dd") + "', "
                          "       id_sender="   + QString::number(ui->combo_sender_pol2point->currentData().toInt()) + ", "
                          "       id_point="    + QString::number(ui->combo_point_pol2point->currentData().toInt()) + ", "
                          "       id_obtainer=" + QString::number(ui->combo_obtainer_pol2point->currentData().toInt()) + " "
                          " where id=" + QString::number(ind) + " ; ";

        QSqlQuery *query_upd = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_upd, "Обновление акта", *query_upd, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке изменить данные акта произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query_upd;
            return;
        }
        delete query_upd;
        db.commit();

        refresh_pol2point_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void acts_wnd::on_bn_data_vs2smo_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_vs2smo || !ui->tab_vs2smo->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Сначала выберите акт приёма-передачи бланков ВС.");
        return;
    }

    // проверим выделенную строку персоны
    QModelIndexList indexes = ui->tab_vs2smo->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        QModelIndex index = indexes.front();
        int id = model_vs2smo.data(model_vs2smo.index(index.row(), 0), Qt::EditRole).toInt();
        QString act_num = model_vs2smo.data(model_vs2smo.index(index.row(), 1), Qt::EditRole).toString();
        QDate act_date = model_vs2smo.data(model_vs2smo.index(index.row(), 2), Qt::EditRole).toDate();

        QString sql = "select b.vs_num, s.text as status, e.fam, e.im, e.ot, e.date_birth "
                      "  from blanks_vs b "
                      "  left join spr_blank_status s on(s.code=b.status) "
                      "  left join persons e on(e.id=b.id_person) "
                      " where b.id_vs_interval=" + QString::number(id) + " "
                      " order by b.vs_num ; ";

        delete show_tab_w;
        show_tab_w = new show_tab_wnd("Акт передачи бланков ВС в СМО (интервал ВС) - номер: " + act_num + ", дата: " + act_date.toString("dd.MM.yyyy"), sql, db, data_app, this);
        show_tab_w->exec();
    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void acts_wnd::on_bn_data_vs2point_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_vs2point || !ui->tab_vs2point->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Сначала выберите акт приёма-передачи бланков полисов.");
        return;
    }

    // проверим выделенную строку персоны
    QModelIndexList indexes = ui->tab_vs2point->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        QModelIndex index = indexes.front();
        int id = model_vs2point.data(model_vs2point.index(index.row(), 0), Qt::EditRole).toInt();
        QString act_num = model_vs2point.data(model_vs2point.index(index.row(), 1), Qt::EditRole).toString();
        QDate act_date = model_vs2point.data(model_vs2point.index(index.row(), 2), Qt::EditRole).toDate();
        int id_point = model_vs2point.data(model_vs2point.index(index.row(), 6), Qt::EditRole).toInt();

        s_data_act_vs2point data_act;
        data_act.id = id;
        data_act.act_num = act_num;
        data_act.act_date = act_date;
        data_act.id_point = id_point;

        delete act_vs2point_w;
        act_vs2point_w = new act_vs2point_wnd(db, data_app, data_act, settings, this);
        act_vs2point_w->exec();

        refresh_vs2point_tab();
    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void acts_wnd::on_bn_data_pol2smo_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_pol2smo || !ui->tab_pol2smo->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Сначала выберите акт приёма-передачи бланков полисов.");
        return;
    }

    // проверим выделенную строку персоны
    QModelIndexList indexes = ui->tab_pol2smo->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        QModelIndex index = indexes.front();
        int id = model_pol2smo.data(model_pol2smo.index(index.row(), 0), Qt::EditRole).toInt();
        QString act_num = model_pol2smo.data(model_pol2smo.index(index.row(), 1), Qt::EditRole).toString();
        QDate act_date = model_pol2smo.data(model_pol2smo.index(index.row(), 2), Qt::EditRole).toDate();

        s_data_act_pol2smo data;
        data.id = id;
        data.act_num = act_num;
        data.act_date = act_date;
        data.id_point = -1;

        delete act_pol2smo_w;
        act_pol2smo_w = new act_pol2smo_wnd(db, data_app, data, settings, this);
        act_pol2smo_w->exec();

        refresh_pol2smo_tab();
    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }



    /*
    // проверим выделенную строку персоны
    QModelIndexList indexes = ui->tab_pol2smo->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        QModelIndex index = indexes.front();
        int id = model_pol2smo.data(model_pol2smo.index(index.row(), 0), Qt::EditRole).toInt();
        QString act_num = model_pol2smo.data(model_pol2smo.index(index.row(), 1), Qt::EditRole).toString();
        QDate act_date = model_pol2smo.data(model_pol2smo.index(index.row(), 2), Qt::EditRole).toDate();

        QString sql = "select b.pol_ser, b.pol_num, s.text as status, '('||p.point_regnum||')  '||p.point_name as point, o.oper_fio as obtainer "
                      "  from blanks_boxes_acts a "
                      "  left join points p on(p.id=a.id_head_point) "
                      "  left join operators o on(o.id=a.id_obtainer) "
                      "  left join blanks_boxes bb on(bb.box_num=a.box_num) "
                      "  left join blanks_pol b on( ((b.pol_ser is NULL and bb.pol_ser is NULL) or (b.pol_ser=bb.pol_ser)) and cast(b.pol_num as integer)=cast(bb.pol_num as integer)) "
                      "  left join spr_blank_status s on(s.code=b.status) "
                      " where a.id=" + QString::number(id) + " "
                      " order by b.pol_ser, b.pol_num ; ";

        delete show_tab_w;
        show_tab_w = new show_tab_wnd("Акт передачи бланков полисов в СМО (короб полисов) - номер: " + act_num + ", дата: " + act_date.toString("dd.MM.yyyy"), sql, db, data_app, this);
        show_tab_w->exec();
    */

}

void acts_wnd::on_bn_data_pol2point_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_pol2point || !ui->tab_pol2point->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Сначала выберите акт приёма-передачи бланков полисов.");
        return;
    }

    // проверим выделенную строку персоны
    QModelIndexList indexes = ui->tab_pol2point->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        QModelIndex index = indexes.front();
        int id = model_pol2point.data(model_pol2point.index(index.row(), 0), Qt::EditRole).toInt();
        QString act_num = model_pol2point.data(model_pol2point.index(index.row(), 1), Qt::EditRole).toString();
        QDate act_date = model_pol2point.data(model_pol2point.index(index.row(), 2), Qt::EditRole).toDate();
        int id_point = model_pol2point.data(model_pol2point.index(index.row(), 6), Qt::EditRole).toInt();

        s_data_act_pol2point data_act;
        data_act.id = id;
        data_act.act_num = act_num;
        data_act.act_date = act_date;
        data_act.id_point = id_point;

        delete act_pol2point_w;
        act_pol2point_w = new act_pol2point_wnd(db, data_app, data_act, settings, this);
        act_pol2point_w->exec();

        refresh_pol2point_tab();
    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void acts_wnd::on_bn_add_pol2point_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (QMessageBox::warning(this, "Нужно подтверждение!",
                             "Вы действительно хотите добавить новый акт приёма-передачи бланков полисов единого образца на ПВП?",
                             QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Yes)==QMessageBox::Cancel) {
        return;
    }

    // проверим номер на уникальность
    QString sql = "select count(*) from blanks_pol_acts  "
                  " where upper(act_num)='" + ui->line_act_num_pol2point->text().trimmed().simplified().toUpper() + "' ; ";

    QSqlQuery *query = new QSqlQuery(db);
    if (!mySQL.exec(this, sql, "Проверка названия акта на уникальность", *query, true, db, data_app) || !query->next()) {
        QMessageBox::warning(this, "Непредвиденная ошибка", "При попытке проверить акт приёма-передачи бланков полисов единого образца на ПВП на уникальность произошла неожиданная ошибка.\n\nСохранение отменено.");
        delete query;
        return;
    }
    if (query->value(0).toInt()>0) {
        QMessageBox::warning(this,
                              "Такой номер акта уже есть",
                              "В базе данных уже есть акт с таким номером.\nИзмените номер акта.\n\nСохранение отменено.");
        delete query;
        return;
    }

    db.transaction();
    // добавим акт
    QString sql_ins = "insert into blanks_pol_acts(act_num, act_date, id_sender, id_point, id_obtainer) "
                      "values ('" + ui->line_act_num_pol2point->text().trimmed().simplified() + "', "
                      "        '" + ui->date_act_pol2point->date().toString("yyyy-MM-dd") + "', "
                      "         " + QString::number(ui->combo_sender_pol2point->currentData().toInt()) + ", "
                      "         " + QString::number(ui->combo_point_pol2point->currentData().toInt()) + ", "
                      "         " + QString::number(ui->combo_obtainer_pol2point->currentData().toInt()) + ") ; ";

    QSqlQuery *query_ins = new QSqlQuery(db);
    if (!mySQL.exec(this, sql_ins, "Добавление акта", *query_ins, true, db, data_app)) {
        db.rollback();
        QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке добавить новый акт приёма-передачи бланков полисов единого образца на ПВП произошла неожиданная ошибка.\n\nСохранение отменено.");
        delete query_ins;
        return;
    }
    delete query_ins;
    db.commit();

    refresh_pol2point_tab();
}

void acts_wnd::on_bn_del_pol2point_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_pol2point->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::warning(this, "Нужно подтверждение!",
                                 "Вы действительно хотите удалить выбранный акт приёма-передачи бланков полисов единого образца?\n\n"
                                 "Все полисы из этого акта будут помечены как не распределённые.",
                                 QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        QModelIndex index = indexes.front();
        int ind       = model_pol2point.data(model_pol2point.index(index.row(), 0), Qt::EditRole).toInt();
        QString cnt_s = model_pol2point.data(model_pol2point.index(index.row(),10), Qt::EditRole).toString();
        cnt_s = cnt_s.trimmed().simplified();
        QString status = model_pol2point.data(model_pol2point.index(index.row(),11), Qt::EditRole).toString();

        if ( (!cnt_s.isEmpty() && cnt_s!="0 / 0")
             && QMessageBox::warning(this, "Нужно подтверждение!",
                                     "Выбранный акт не пустой!\n\n" + cnt_s + "\n\n"
                                     "При его удалении все бланки будут переведены в пул нераспределённых бланков. \n"
                                     "Удалить акт?", QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel )==QMessageBox::Cancel ) {
            QMessageBox::information(this, "Отмена операции", "Действие отменено пользователем.");
            return;
        }
        if ( (!status.isEmpty() && status!="0")
             && QMessageBox::warning(this, "Нужно подтверждение!",
                                     "Выбранный акт помеченн как исполненный!\n\n" + cnt_s + "\n\n"
                                     "При его удалении будет невозможно восстановить информацию об исполнении этого акта. \n"
                                     "Удалить акт?", QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel )==QMessageBox::Cancel ) {
            QMessageBox::information(this, "Отмена операции", "Действие отменено пользователем.");
            return;
        }

        db.transaction();
        // добавим акт
        QString sql_del = "update blanks_pol "
                          "   set id_blanks_pol_act=NULL "
                          " where id_blanks_pol_act=" + QString::number(ind) + " ; "

                          "delete from blanks_pol_acts"
                          " where id=" + QString::number(ind) + " ; ";

        QSqlQuery *query_del = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_del, "Удаление акта полисов на ПВП", *query_del, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке удалить акт полисов на ПВП произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query_del;
            return;
        }
        delete query_del;
        db.commit();

        refresh_pol2point_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }

    refresh_pol2point_tab();
}

void acts_wnd::on_bn_del_pol2smo_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_pol2smo->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::warning(this, "Нужно подтверждение!",
                                 "Вы действительно хотите удалить выбранный акт приёма-передачи бланков полисов единого образца?\n\n"
                                 "Все полисы из этого акта будут помечены как не распределённые.",
                                 QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        QModelIndex index = indexes.front();
        int ind = model_pol2smo.data(model_pol2smo.index(index.row(), 0), Qt::EditRole).toInt();
        QString cnt_s = model_pol2smo.data(model_pol2smo.index(index.row(), 8), Qt::EditRole).toString();
        cnt_s = cnt_s.trimmed().simplified();

        if ( (!cnt_s.isEmpty() && cnt_s!="0 / 0")
             && QMessageBox::warning(this, "Нужно подтверждение!",
                                     "Выбранный акт не пустой!\n\n" + cnt_s + "\n\n"
                                     "При его удалении все ВС будут переведены в пул нераспределённых ВС. \n"
                                     "Удалить акт?", QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel )==QMessageBox::Cancel ) {
            QMessageBox::information(this, "Отмена операции", "Действие отменено пользователем.");
            return;
        }

        db.transaction();
        // добавим акт
        QString sql_del = "update blanks_pol "
                          "   set id_blanks_box_act=NULL "
                          " where id_blanks_box_act=" + QString::number(ind) + " ; "

                          "delete from blanks_boxes_acts "
                          " where id=" + QString::number(ind) + " ; ";

        QSqlQuery *query_del = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_del, "Удаление акта полисов в СМО", *query_del, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке удалить акт полисов в СМО произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query_del;
            return;
        }
        delete query_del;
        db.commit();

        refresh_pol2smo_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }

    refresh_pol2smo_tab();
}

void acts_wnd::on_bn_del_vs2point_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_vs2point->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::warning(this, "Нужно подтверждение!",
                                 "Вы действительно хотите удалить выбранный акт приёма-передачи бланков ВС?\n\n"
                                 "Все ВС из этого акта будут помечены как не распределённые.",
                                 QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        QModelIndex index = indexes.front();
        int ind = model_vs2point.data(model_vs2point.index(index.row(), 0), Qt::EditRole).toInt();
        QString cnt_s = model_vs2point.data(model_vs2point.index(index.row(),10), Qt::EditRole).toString();
        cnt_s = cnt_s.trimmed().simplified();
        QString status = model_vs2point.data(model_vs2point.index(index.row(),11), Qt::EditRole).toString();

        if ( (!cnt_s.isEmpty() && cnt_s!="0 / 0")
             && QMessageBox::warning(this, "Нужно подтверждение!",
                                     "Выбранный акт не пустой!\n\n" + cnt_s + "\n\n"
                                     "При его удалении все ВС будут переведены в пул нераспределённых ВС. \n"
                                     "Удалить акт?", QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel )==QMessageBox::Cancel ) {
            QMessageBox::information(this, "Отмена операции", "Действие отменено пользователем.");
            return;
        }
        if ( (!status.isEmpty() && status!="0")
             && QMessageBox::warning(this, "Нужно подтверждение!",
                                     "Выбранный акт помеченн как исполненный!\n\n" + cnt_s + "\n\n"
                                     "При его удалении будет невозможно восстановить информацию об исполнении этого акта. \n"
                                     "Удалить акт?", QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel )==QMessageBox::Cancel ) {
            QMessageBox::information(this, "Отмена операции", "Действие отменено пользователем.");
            return;
        }

        db.transaction();
        // добавим акт
        QString sql_del = "update blanks_vs "
                          "   set id_blanks_vs_act=NULL "
                          " where id_blanks_vs_act=" + QString::number(ind) + " ; "

                          "delete from blanks_vs_acts "
                          " where id=" + QString::number(ind) + " ; ";

        QSqlQuery *query_del = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_del, "Удаление акта ВС на ПВП", *query_del, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке удалить акт ВС на ПВП произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query_del;
            return;
        }
        delete query_del;
        db.commit();

        refresh_vs2point_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }

    refresh_vs2point_tab();
}

void acts_wnd::on_bn_add_vs2point_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (QMessageBox::warning(this, "Нужно подтверждение!",
                             "Вы действительно хотите добавить новый акт приёма-передачи бланков ВС на ПВП?",
                             QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Yes)==QMessageBox::Cancel) {
        return;
    }

    // проверим номер на уникальность
    QString sql = "select count(*) from blanks_vs_acts  "
                  " where upper(act_num)='" + ui->line_act_num_vs2point->text().trimmed().simplified().toUpper() + "' ; ";

    QSqlQuery *query = new QSqlQuery(db);
    if (!mySQL.exec(this, sql, "Проверка названия акта на уникальность", *query, true, db, data_app) || !query->next()) {
        QMessageBox::warning(this, "Непредвиденная ошибка", "При попытке проверить акт приёма-передачи бланков ВС на ПВП на уникальность произошла неожиданная ошибка.\n\nСохранение отменено.");
        delete query;
        return;
    }
    if (query->value(0).toInt()>0) {
        QMessageBox::warning(this,
                              "Такой номер акта уже есть",
                              "В базе данных уже есть акт с таким номером.\nИзмените номер акта.\n\nСохранение отменено.");
        delete query;
        return;
    }

    db.transaction();
    // добавим акт
    QString sql_ins = "insert into blanks_vs_acts(act_num, act_date, id_sender, id_point, id_obtainer) "
                      "values ('" + ui->line_act_num_vs2point->text().trimmed().simplified() + "', "
                      "        '" + ui->date_act_vs2point->date().toString("yyyy-MM-dd") + "', "
                      "         " + QString::number(ui->combo_sender_vs2point->currentData().toInt()) + ", "
                      "         " + QString::number(ui->combo_point_vs2point->currentData().toInt()) + ", "
                      "         " + QString::number(ui->combo_obtainer_vs2point->currentData().toInt()) + ") ; ";

    QSqlQuery *query_ins = new QSqlQuery(db);
    if (!mySQL.exec(this, sql_ins, "Добавление акта", *query_ins, true, db, data_app)) {
        db.rollback();
        QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке добавить новый акт приёма-передачи бланков ВС на ПВП произошла неожиданная ошибка.\n\nСохранение отменено.");
        delete query_ins;
        return;
    }
    delete query_ins;
    db.commit();

    refresh_vs2point_tab();
}

void acts_wnd::on_bn_add_pol2smo_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (QMessageBox::warning(this, "Нужно подтверждение!",
                             "Вы действительно хотите добавить новый акт приёма-передачи бланков полисов единого образца в СМО?",
                             QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Yes)==QMessageBox::Cancel) {
        return;
    }

    // проверим номер на уникальность
    QString sql = "select count(*) from blanks_boxes_acts  "
                  " where upper(act_num)='" + ui->line_act_num_pol2smo->text().trimmed().simplified().toUpper() + "' ; ";

    QSqlQuery *query = new QSqlQuery(db);
    if (!mySQL.exec(this, sql, "Проверка названия акта на уникальность", *query, true, db, data_app) || !query->next()) {
        QMessageBox::warning(this, "Непредвиденная ошибка", "При попытке проверить акт приёма-передачи бланков полисов единого образца в СМО на уникальность произошла неожиданная ошибка.\n\nСохранение отменено.");
        delete query;
        return;
    }
    if (query->value(0).toInt()>0) {
        QMessageBox::warning(this,
                              "Такой номер акта уже есть",
                              "В базе данных уже есть акт с таким номером.\nИзмените номер акта.\n\nСохранение отменено.");
        delete query;
        return;
    }

    db.transaction();
    // добавим акт
    QString sql_ins = "insert into blanks_boxes_acts(act_num, act_date, id_head_point, id_obtainer) "
                      "values ('" + ui->line_act_num_pol2smo->text().trimmed().simplified() + "', "
                      "        '" + ui->date_act_pol2smo->date().toString("yyyy-MM-dd") + "', "
                      "         " + QString::number(ui->combo_head_point_pol2smo->currentData().toInt()) + ", "
                      "         " + QString::number(ui->combo_obtainer_pol2smo->currentData().toInt()) + ") ; ";

    QSqlQuery *query_ins = new QSqlQuery(db);
    if (!mySQL.exec(this, sql_ins, "Добавление акта", *query_ins, true, db, data_app)) {
        db.rollback();
        QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке добавить новый акт приёма-передачи бланков полисов единого образца в СМО произошла неожиданная ошибка.\n\nСохранение отменено.");
        delete query_ins;
        return;
    }
    delete query_ins;
    db.commit();

    refresh_pol2smo_tab();
}

void acts_wnd::on_line_act_num_vs2point_textChanged(const QString &arg1) {
    /*QString txt0 = arg1;
    QString txt = txt0.replace("/", " / ").trimmed().simplified();
    ui->line_act_num_vs2point->setText(txt);
    ui->line_act_num_vs2point->setCursorPosition(txt.length());*/
}

void acts_wnd::on_line_act_num_pol2point_textChanged(const QString &arg1) {
    /*QString txt0 = arg1;
    QString txt = txt0.replace("/", " / ").trimmed().simplified();
    ui->line_act_num_pol2point->setText(txt);
    ui->line_act_num_pol2point->setCursorPosition(txt.length());*/
}

void acts_wnd::on_line_act_num_pol2smo_textChanged(const QString &arg1) {
    /*QString txt0 = arg1;
    QString txt = txt0.replace("/", " / ").trimmed().simplified();
    ui->line_act_num_pol2point->setText(txt);
    ui->line_act_num_pol2point->setCursorPosition(txt.length());*/
}

void acts_wnd::on_ch_vs2point_apply_clicked(bool checked) {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_vs2point || !ui->tab_vs2point->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите строку в таблице.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_vs2point->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        QModelIndex index = indexes.front();
        int ind = model_vs2point.data(model_vs2point.index(index.row(), 0), Qt::EditRole).toInt();
        QString act_num = model_vs2point.data(model_vs2point.index(index.row(), 1), Qt::EditRole).toString();
        QDate act_date = model_vs2point.data(model_vs2point.index(index.row(), 2), Qt::EditRole).toDate();
        int id_sender = model_vs2point.data(model_vs2point.index(index.row(), 4), Qt::EditRole).toInt();
        int id_point = model_vs2point.data(model_vs2point.index(index.row(), 6), Qt::EditRole).toInt();

        if (checked) {
            if (QMessageBox::question(this,
                                      "Нужно подтверждение",
                                      "Вы действительно хотите отметить как исполненный\n акт '" + act_num + "' от " + act_date.toString("dd.MM.yyyy") + "?\n\n"
                                      "Все включённые в акт бланки ВС будут считаться переданными на целевой ПВП начиная с текущей даты. \n"
                                      "(эта дата не обязательно должна совпадать с датой акта и служит только для внутреннего учёта движения бланков)\n\n"
                                      "При необходимости, отметку об исполнении акта можно снять.",
                                      QMessageBox::Yes|QMessageBox::Cancel,
                                      QMessageBox::Cancel)==QMessageBox::Cancel) {
                ui->ch_vs2point_apply->setChecked(false);
                return;
            }
            db.transaction();

            QString sql_upd = "update blanks_vs "
                              "   set id_point=" + QString::number(id_point) + " "
                              " where id_blanks_vs_act=" + QString::number(ind) + " ; "

                              "update blanks_vs_acts "
                              "   set status=1 "
                              " where id=" + QString::number(ind) + " ";

            QSqlQuery *query_upd = new QSqlQuery(db);
            if (!mySQL.exec(this, sql_upd, "Отметка об исполнении акта", *query_upd, true, db, data_app)) {
                db.rollback();
                QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке проставить отметки об исполнении акта произошла неожиданная ошибка.\n\nСохранение отменено.");
                delete query_upd;
                return;
            }
            delete query_upd;
            db.commit();

        } else {
            if (QMessageBox::question(this,
                                      "Нужно подтверждение",
                                      "Вы действительно хотите снять отметку об исполнении \n акта '" + act_num + "' от " + act_date.toString("dd.MM.yyyy") + " ?\n\n"
                                      "Все включённые в акт бланки ВС будут переданы назад на передающий ПВП начиная с текущей даты. \n"
                                      "(эта дата не обязательно должна совпадать с датой акта и служит только для внутреннего учёта движения бланков)\n\n"
                                      "При необходимости, отметку об исполнении акта можно восстановить.",
                                      QMessageBox::Yes|QMessageBox::Cancel,
                                      QMessageBox::Cancel)==QMessageBox::Cancel) {
                ui->ch_vs2point_apply->setChecked(true);
                return;
            }
            db.transaction();

            QString sql_upd = "update blanks_vs "
                              "   set id_point=" + QString::number(id_sender) + " "
                              " where id_blanks_vs_act=" + QString::number(ind) + " ; "

                              "update blanks_vs_acts "
                              "   set status=0 "
                              " where id=" + QString::number(ind) + " ";

            QSqlQuery *query_upd = new QSqlQuery(db);
            if (!mySQL.exec(this, sql_upd, "Снятие отметки об исполнении акта", *query_upd, true, db, data_app)) {
                db.rollback();
                QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке снять отметки об исполнении акта произошла неожиданная ошибка.\n\nСохранение отменено.");
                delete query_upd;
                return;
            }
            delete query_upd;
            db.commit();
        }
        refresh_vs2point_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void acts_wnd::on_ch_pol2point_apply_clicked(bool checked) {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_pol2point || !ui->tab_pol2point->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите строку в таблице.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_pol2point->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        QModelIndex index = indexes.front();
        int ind = model_pol2point.data(model_pol2point.index(index.row(), 0), Qt::EditRole).toInt();
        QString act_num = model_pol2point.data(model_pol2point.index(index.row(), 1), Qt::EditRole).toString();
        QDate act_date = model_pol2point.data(model_pol2point.index(index.row(), 2), Qt::EditRole).toDate();
        int id_sender = model_pol2point.data(model_pol2point.index(index.row(), 4), Qt::EditRole).toInt();
        int id_point = model_pol2point.data(model_pol2point.index(index.row(), 6), Qt::EditRole).toInt();

        if (checked) {
            if (QMessageBox::question(this,
                                      "Нужно подтверждение",
                                      "Вы действительно хотите отметить как исполненный\n акт '" + act_num + "' от " + act_date.toString("dd.MM.yyyy") + "?\n\n"
                                      "Все включённые в акт бланки ВС будут считаться переданными на целевой ПВП начиная с текущей даты. \n"
                                      "(эта дата не обязательно должна совпадать с датой акта и служит только для внутреннего учёта движения бланков)\n\n"
                                      "При необходимости, отметку об исполнении акта можно снять.",
                                      QMessageBox::Yes|QMessageBox::Cancel,
                                      QMessageBox::Cancel)==QMessageBox::Cancel) {
                ui->ch_pol2point_apply->setChecked(false);
                return;
            }
            db.transaction();

            QString sql_upd = "update blanks_pol "
                              "   set id_point=" + QString::number(id_point) + " "
                              " where id_blanks_pol_act=" + QString::number(ind) + " ; "

                              "update blanks_pol_acts "
                              "   set status=1 "
                              " where id=" + QString::number(ind) + " ";

            QSqlQuery *query_upd = new QSqlQuery(db);
            if (!mySQL.exec(this, sql_upd, "Отметка об исполнении акта", *query_upd, true, db, data_app)) {
                db.rollback();
                QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке проставить отметки об исполнении акта произошла неожиданная ошибка.\n\nСохранение отменено.");
                delete query_upd;
                return;
            }
            delete query_upd;
            db.commit();

        } else {
            if (QMessageBox::question(this,
                                      "Нужно подтверждение",
                                      "Вы действительно хотите снять отметку об исполнении \n акта '" + act_num + "' от " + act_date.toString("dd.MM.yyyy") + " ?\n\n"
                                      "Все включённые в акт бланки ВС будут переданы назад на передающий ПВП начиная с текущей даты. \n"
                                      "(эта дата не обязательно должна совпадать с датой акта и служит только для внутреннего учёта движения бланков)\n\n"
                                      "При необходимости, отметку об исполнении акта можно восстановить.",
                                      QMessageBox::Yes|QMessageBox::Cancel,
                                      QMessageBox::Cancel)==QMessageBox::Cancel) {
                ui->ch_pol2point_apply->setChecked(true);
                return;
            }
            db.transaction();

            QString sql_upd = "update blanks_pol "
                              "   set id_point=" + QString::number(id_sender) + " "
                              " where id_blanks_pol_act=" + QString::number(ind) + " ; "

                              "update blanks_pol_acts "
                              "   set status=0 "
                              " where id=" + QString::number(ind) + " ";

            QSqlQuery *query_upd = new QSqlQuery(db);
            if (!mySQL.exec(this, sql_upd, "Снятие отметки об исполнении акта", *query_upd, true, db, data_app)) {
                db.rollback();
                QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке снять отметки об исполнении акта произошла неожиданная ошибка.\n\nСохранение отменено.");
                delete query_upd;
                return;
            }
            delete query_upd;
            db.commit();
        }
        refresh_pol2point_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void acts_wnd::on_bn_vs2smo_print_clicked() {

    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_vs2smo || !ui->tab_vs2smo->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите строку в таблице.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_vs2smo->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this, "Формирование печатной формы акта",
                                  "Запустить формирование печатной формы акта на получение интервала ВС в СМО?",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            QMessageBox::warning(this, "Действие отменено",
                                 "Формирование печатной формы акта на получение интервала ВС в СМО отменено пользователем.");
            return;
        }

        QModelIndex index = indexes.front();
        int ind = model_vs2smo.data(model_vs2smo.index(index.row(), 0), Qt::EditRole).toInt();
        QString act_num = model_vs2smo.data(model_vs2smo.index(index.row(), 1), Qt::EditRole).toString();
        QDate act_date = model_vs2smo.data(model_vs2smo.index(index.row(), 2), Qt::EditRole).toDate();
        if (ind<=0) {
            QMessageBox::warning(this, "Выберите акт",
                                 "Выбранная строка не соответствует ни одному акту.\n"
                                 "Выберите акт и повторите попытку.");
            return;
        }

        // сформруем печатную форму акта
        QString sql_data = "select right('00000000000' || vs_num, 11) "
                          "  from blanks_vs "
                          " where id_vs_interval=" + QString::number(ind) + " "
                          " order by right('00000000000' || vs_num, 11) ; ";

        QSqlQuery *query_data = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_data, "Обновление акта", *query_data, true, db, data_app)) {
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке получить данные акта произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query_data;
            return;
        }
        act_num = act_num.replace(" / ", "-").replace("/", "-");
        QString freport = data_app.path_acts + "Акт - получение ВС для СМО - № '" + act_num + "' от " + act_date.toString("dd.MM.yyyy") + ".csv";
        QFile frep(freport);
        QFile::remove(freport);
        while (!frep.open(QIODevice::WriteOnly)) {
            if (QMessageBox::question(this, "Файл занят?",
                                      "При попытке создать целевой файл произошла ошибка.\n\n"
                                      "Файл уже создан и открыт в другой программе?\n\n" +
                                      freport +
                                      "\n\nЕсли да - закройте файл и повторите попытку.\n\n"
                                      "Повторить попытку?",
                                      QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel)==QMessageBox::Cancel) {
                QMessageBox::warning(this, "Действие отменено",
                                     "Формирование печатной формы акта на получение интервала ВС в СМО отменено пользователем.");
                delete query_data;
                return;
            }
        }
        frep.write(QString("№; номер ВС\n").toLocal8Bit());

        int n=0;
        while (query_data->next()) {
            n++;
            frep.write(QString::number(n).toLocal8Bit());
            frep.write(";");
            frep.write(query_data->value(0).toString().toLocal8Bit());
            frep.write("\n");
        }
        frep.close();

        long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(freport.utf16()), 0, 0, SW_NORMAL);
        if (result<32) {
            QMessageBox::warning(this,
                                 "Файл не найден",
                                 "Файл не найден:\n\n" + freport +
                                 "\nКод ошибки: " + QString::number(result) +
                                 "\n\nПроверьте, был ли сохранён целевой файл и установлено ли в системе средство для его просмотра.");
        }

        delete query_data;

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void acts_wnd::on_bn_vs2point_print_clicked() {

    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_vs2point || !ui->tab_vs2point->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите строку в таблице.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_vs2point->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this, "Формирование печатной формы акта",
                                  "Запустить формирование печатной формы акта на передачу интервала ВС на ПВП?",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            QMessageBox::warning(this, "Действие отменено",
                                 "Формирование печатной формы акта на передачу интервала ВС на ПВП отменено пользователем.");
            return;
        }

        QModelIndex index = indexes.front();
        int ind = model_vs2point.data(model_vs2point.index(index.row(), 0), Qt::EditRole).toInt();
        QString act_num = model_vs2point.data(model_vs2point.index(index.row(), 1), Qt::EditRole).toString();
        QDate act_date = model_vs2point.data(model_vs2point.index(index.row(), 2), Qt::EditRole).toDate();
        if (ind<=0) {
            QMessageBox::warning(this, "Выберите акт",
                                 "Выбранная строка не соответствует ни одному акту.\n"
                                 "Выберите акт и повторите попытку.");
            return;
        }

        // сформруем печатную форму акта
        QString sql_data = "select right('00000000000' || vs_num, 11) "
                          "  from blanks_vs "
                          " where id_blanks_vs_act=" + QString::number(ind) + " "
                          " order by right('00000000000' || vs_num, 11) ; ";

        QSqlQuery *query_data = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_data, "Обновление акта", *query_data, true, db, data_app)) {
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке изменить данные акта произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query_data;
            return;
        }
        act_num = act_num.replace(" / ", "-").replace("/", "-");
        QString freport = data_app.path_acts + "Акт - передача ВС на ПВП - № '" + act_num + "' от " + act_date.toString("dd.MM.yyyy") + ".csv";
        QFile frep(freport);
        QFile::remove(freport);
        while (!frep.open(QIODevice::WriteOnly)) {
            if (QMessageBox::question(this, "Файл занят?",
                                      "При попытке создать целевой файл произошла ошибка.\n\n"
                                      "Файл уже создан и открыт в другой программе?\n\n" +
                                      freport +
                                      "\n\nЕсли да - закройте файл и повторите попытку.\n\n"
                                      "Повторить попытку?",
                                      QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel)==QMessageBox::Cancel) {
                QMessageBox::warning(this, "Действие отменено",
                                     "Формирование печатной формы акта на передачу интервала ВС на ПВП отменено пользователем.");
                delete query_data;
                return;
            }
        }
        frep.write(QString("№; номер ВС\n").toLocal8Bit());

        int n=0;
        while (query_data->next()) {
            n++;
            frep.write(QString::number(n).toLocal8Bit());
            frep.write(";");
            frep.write(query_data->value(0).toString().toLocal8Bit());
            frep.write("\n");
        }
        frep.close();

        long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(freport.utf16()), 0, 0, SW_NORMAL);
        if (result<32) {
            QMessageBox::warning(this,
                                 "Файл не найден",
                                 "Файл не найден:\n\n" + freport +
                                 "\nКод ошибки: " + QString::number(result) +
                                 "\n\nПроверьте, был ли сохранён целевой файл и установлено ли в системе средство для его просмотра.");
        }

        delete query_data;

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}


void acts_wnd::on_bn_pol2smo_print_clicked() {

    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_pol2smo || !ui->tab_pol2smo->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите строку в таблице.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_pol2smo->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this, "Формирование печатной формы акта",
                                  "Запустить формирование печатной формы акта на получение короба полисов в СМО?",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            QMessageBox::warning(this, "Действие отменено",
                                 "Формирование печатной формы акта на получение короба полисов в СМО отменено пользователем.");
            return;
        }

        QModelIndex index = indexes.front();
        int ind = model_pol2smo.data(model_pol2smo.index(index.row(), 0), Qt::EditRole).toInt();
        QString act_num = model_pol2smo.data(model_pol2smo.index(index.row(), 1), Qt::EditRole).toString();
        QDate act_date = model_pol2smo.data(model_pol2smo.index(index.row(), 2), Qt::EditRole).toDate();
        if (ind<=0) {
            QMessageBox::warning(this, "Выберите акт",
                                 "Выбранная строка не соответствует ни одному акту.\n"
                                 "Выберите акт и повторите попытку.");
            return;
        }

        // сформруем печатную форму акта
        QString sql_data = "select b.pol_ser||' '||b.pol_num as ser_num, b.enp, e.fam||' '||e.im||' '||e.ot as fio, e.date_birth "
                           "  from blanks_pol b "
                           "  left join persons e on(e.id=b.id_person) "
                           " where id_blanks_box_act=" + QString::number(ind) + " "
                           " order by b.pol_ser, b.pol_num ; ";

        QSqlQuery *query_data = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_data, "Обновление акта", *query_data, true, db, data_app)) {
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке получить данные акта произошла неожиданная ошибка.\n\nСохранение отменено.");
            delete query_data;
            return;
        }
        act_num = act_num.replace(" / ", "-").replace("/", "-");
        QString freport = data_app.path_acts + "Акт - получение короба полисов для СМО - № '" + act_num + "' от " + act_date.toString("dd.MM.yyyy") + ".csv";
        QFile frep(freport);
        QFile::remove(freport);
        while (!frep.open(QIODevice::WriteOnly)) {
            if (QMessageBox::question(this, "Файл занят?",
                                      "При попытке создать целевой файл произошла ошибка.\n\n"
                                      "Файл уже создан и открыт в другой программе?\n\n" +
                                      freport +
                                      "\n\nЕсли да - закройте файл и повторите попытку.\n\n"
                                      "Повторить попытку?",
                                      QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel)==QMessageBox::Cancel) {
                QMessageBox::warning(this, "Действие отменено",
                                     "Формирование печатной формы акта на получение короба бланков полисов в СМО отменено пользователем.");
                delete query_data;
                return;
            }
        }
        frep.write(QString("№; серия № номер; ЕНП; ФИО застрахованного; Дата рождения;\n").toLocal8Bit());

        int n=0;
        while (query_data->next()) {
            n++;
            frep.write(QString::number(n).toLocal8Bit());
            frep.write(";");
            // b.pol_ser||' '||b.pol_num as ser_num, b.enp, e.fam||' '||e.im||' '||e.ot as fio, e.date_birth
            frep.write(query_data->value(0).toString().toLocal8Bit());
            frep.write(";");
            frep.write(query_data->value(1).toString().toLocal8Bit());
            frep.write(";");
            frep.write(query_data->value(2).toString().toLocal8Bit());
            frep.write(";");
            frep.write(query_data->value(3).toDate().toString("dd.MM.yyyy").toLocal8Bit());
            frep.write(";\n");
        }
        frep.close();

        long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(freport.utf16()), 0, 0, SW_NORMAL);
        if (result<32) {
            QMessageBox::warning(this,
                                 "Файл не найден",
                                 "Файл не найден:\n\n" + freport +
                                 "\nКод ошибки: " + QString::number(result) +
                                 "\n\nПроверьте, был ли сохранён целевой файл и установлено ли в системе средство для его просмотра.");
        }

        delete query_data;

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void acts_wnd::on_bn_pol2point_print_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_pol2point || !ui->tab_pol2point->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите строку в таблице.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_pol2point->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this, "Формирование печатной формы акта",
                                  "Запустить формирование печатной формы акта на передачу полисов единого образца на ПВП?",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            QMessageBox::warning(this, "Действие отменено",
                                 "Формирование печатной формы акта на передачу полисов единого образца на ПВП отменено пользователем.");
            return;
        }

        QModelIndex index = indexes.front();
        int id_act = model_pol2point.data(model_pol2point.index(index.row(), 0), Qt::EditRole).toInt();
        QString act_num = model_pol2point.data(model_pol2point.index(index.row(), 1), Qt::EditRole).toString();
        QDate act_date = model_pol2point.data(model_pol2point.index(index.row(), 2), Qt::EditRole).toDate();
        QString sender = model_pol2point.data(model_pol2point.index(index.row(), 5), Qt::EditRole).toString();
        if (sender.isEmpty())  sender = "головной офис";
        QString sender_post = data_app.filial_director + "  -  " + data_app.filial_director_2;
        QString sender_fio = data_app.filial_director_fio;
        QString obtainer = model_pol2point.data(model_pol2point.index(index.row(), 7), Qt::EditRole).toString();
        QString obtainer_post = "инспектор";
        QString obtainer_fio = model_pol2point.data(model_pol2point.index(index.row(), 9), Qt::EditRole).toString();

        if (id_act<=0) {
            QMessageBox::warning(this, "Выберите акт",
                                 "Выбранная строка не соответствует ни одному акту.\n"
                                 "Выберите акт и повторите попытку.");
            return;
        }

        // сформруем печатную форму акта
        QString sql_data = "select bp.id as id_blank_pol, bp.pol_ser, bp.pol_num, bp.enp, e.id as id_person, e.fam, e.im, e.ot, e.date_birth, ap.id as ind, ap.act_date, pt.id as id_point, pt.point_name "
                           "  from blanks_pol bp"
                           "       left join persons e on(e.id=bp.id_person)"
                           "       left join blanks_pol_acts ap on(ap.id=bp.id_blanks_pol_act) "
                           "       left join points pt on(ap.id_point=pt.id) "
                           " where id_blanks_pol_act=" + QString::number(id_act) + " "
                           " order by e.fam, e.im, e.ot, e.date_birth ; ";

        QSqlQuery *query_data = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_data, "Чтение данных акта", *query_data, true, db, data_app)) {
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке получить данные акта произошла неожиданная ошибка.\n\nДействие отменено.");
            delete query_data;
            return;
        }

        // подготовка данных для заполнения полей
        s_data_blanks_pol_act data_act;
        data_act.act_num = act_num;
        data_act.act_date = act_date;
        data_act.sender = sender;
        data_act.sender_post = sender_post;
        data_act.sender_fio = sender_fio;
        data_act.obtainer = obtainer;
        data_act.obtainer_post = obtainer_post;
        data_act.obtainer_fio = obtainer_fio;

        delete  print_blanks_pol_act_w;
        print_blanks_pol_act_w = new print_blanks_pol_act_wnd(db, data_act, data_app, settings, this);
        if (!print_blanks_pol_act_w->exec())
            return;

        // распаковка шаблона акта
        QString fname_template = data_app.path_reports + "rep_blanks_pol_act.ods";
        QString temp_folder = data_app.path_temp + "rep_blanks_pol_act";
        if (!QFile::exists(fname_template)) {
            QMessageBox::warning(this,
                                 "Шаблон не найден",
                                 "Не найден шаблон печатной формы половозрастной таблицы прикреплённых к МО: \n" + fname_template +
                                 "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        QDir tmp_dir(temp_folder);
        if (tmp_dir.exists(temp_folder) && !tmp_dir.removeRecursively()) {
            QMessageBox::warning(this,
                                 "Ошибка при очистке папки",
                                 "Не удалось очистить папку временных файлов: \n" + temp_folder +
                                 "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        tmp_dir.mkpath(temp_folder);
        QProcess myProcess;
        QString com = "\"" + data_app.path_arch + "7z.exe\" e \"" + fname_template + "\" -o\"" + temp_folder + "\" content.xml";
        if (myProcess.execute(com)!=0) {
            QMessageBox::warning(this,
                                 "Ошибка распаковки шаблона",
                                 "Не удалось распаковать файл контента шаблона\n" + fname_template + " -> content.xml"
                                 "\nпечатной формы во временную папку\n" + temp_folder +
                                 "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        com = "\"" + data_app.path_arch + "7z.exe\" e \"" + fname_template + "\" -o\"" + temp_folder + "\" styles.xml";
        if (myProcess.execute(com)!=0) {
            QMessageBox::warning(this,
                                 "Ошибка распаковки шаблона",
                                 "Не удалось распаковать файл стилей шаблона \n" + fname_template + " -> styles.xml"
                                 "\nпечатной формы во временную папку\n" + temp_folder +
                                 "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }

        db.transaction();

        // ===========================
        // правка контента шаблона
        // ===========================
        // открытие контента шаблона
        QFile f_content(temp_folder + "/content.xml");
        f_content.open(QIODevice::ReadOnly);
        QString s_content = f_content.readAll();
        f_content.close();
        if (s_content.isEmpty()) {
            QMessageBox::warning(this,
                                 "Файл контента шаблона пустой",
                                 "Файл контента шаблона пустой. Возможно шаблон был испорчен.\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            db.rollback();
            return;
        }
        // выбор шаблона
        int pos0 = s_content.indexOf("<table:table-row table:style-name=\"ro6\">", 0);
        int pos1 = s_content.indexOf("</table:table-row>", pos0) + 18;
        QString s_row = s_content.mid(pos0, pos1-pos0);

        // правка полей контента шаблона
        QString s_content_new = s_content.left(pos0);


        // добавим все строки выборки
        {
            int n = 0;
            while (query_data->next()) {
                int id_blank_pol   = query_data->value( 0).toInt();
                QString pol_ser    = query_data->value( 1).toString();
                QString pol_num    = query_data->value( 2).toString();
                QString enp        = query_data->value( 3).toString();
                int id_person      = query_data->value( 4).toInt();
                QString fam        = query_data->value( 5).toString();
                QString im         = query_data->value( 6).toString();
                QString ot         = query_data->value( 7).toString();
                QDate date_birth   = query_data->value( 8).toDate();
                int ind            = query_data->value( 9).toInt();
                QDate act_date     = query_data->value(10).toDate();
                int id_point       = query_data->value(11).toInt();
                QString point_name = query_data->value(12).toString();
                n++;

                QString s_row_new = s_row;
                s_row_new = s_row_new.
                        replace("#NN#", QString::number(n)).
                        replace("#ENP#",enp).
                        replace("#FAM#",fam).
                        replace("#IM#", im).
                        replace("#OT#", ot).
                        replace("#DR#", date_birth.toString("d.MM.yyyy"));
                s_content_new += s_row_new;
            }
        }
        delete query_data;


        s_content_new += s_content.right(s_content.size()-pos1);

        s_content_new = s_content_new.
                replace("#CITY#",  data_app.filial_city).
                replace("#ACT_NUM#",  data_act.act_num).
                replace("#ACT_DATE#", data_act.act_date.toString("d.MM.yyyy")).
                replace("#SENDER#",  data_act.sender).
                replace("#SENDER_POST#",  data_act.sender_post).
                replace("#SENDER_FIO#",  data_act.sender_fio).
                replace("#OBTAINER#",  data_act.obtainer).
                replace("#OBTAINER_POST#",  data_act.obtainer_post).
                replace("#OBTAINER_FIO#",  data_act.obtainer_fio).
                replace("#FILIAL_NAME_RP#",  data_app.filial_name_rp);

        // сохранение контента шаблона
        QFile f_content_save(temp_folder + "/content.xml");
        f_content_save.open(QIODevice::WriteOnly);
        f_content_save.write(s_content_new.toUtf8());
        f_content_save.close();

        // ===========================
        // архивация шаблона
        // ===========================
        act_num = act_num.replace(" / ", " - ").replace("/", "-");
        QString fname_res = data_app.path_acts + "Акт - передача полисов единого образца на ПВП - № '" + act_num + "' от " + act_date.toString("dd.MM.yyyy") + ".ods";
        //QString fname_res = data_app.path_out + "_TRANSFER_ACTS_/" + QDate::currentDate().toString("yyyy-MM-dd") + " - " + data_app.filial_name + " - pol2point_act - " + ui->combo_point->currentData().toString() + ".ods";

        QFile::remove(fname_res);
        while (QFile::exists(fname_res)) {
            if (QMessageBox::warning(this,
                                     "Ошибка сохранения акта передачи полисов на ПВП",
                                     "Не удалось сохранить форму передаточного акта бланков полисов на ПВП: \n" + fname_template +
                                     "\n\nво временную папку\n" + fname_res +
                                     "\n\nПроверьте, не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                     QMessageBox::Retry|QMessageBox::Abort,
                                     QMessageBox::Retry)==QMessageBox::Abort) {
                this->setCursor(Qt::ArrowCursor);
                db.rollback();
                return;
            }
            QFile::remove(fname_res);
        }
        if (!QFile::copy(fname_template, fname_res) || !QFile::exists(fname_res)) {
            QMessageBox::warning(this, "Ошибка копирования отчёта", "Не удалось скопировать форму передаточного акта бланков полисов на ПВП: \n" + fname_template +
                                 "\n\nво временную папку\n" + fname_res +
                                 "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            db.rollback();
            return;
        }
        com = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname_res + "\" \"" + temp_folder + "/content.xml\"";
        if (myProcess.execute(com)!=0) {
            QMessageBox::warning(this,
                                 "Ошибка обновления контента",
                                 "При добавлении нового контента в форму передаточного акта бланков полисов на ПВП произошла непредвиденная ошибка\n\n"
                                 "Опреация прервана.");
            this->setCursor(Qt::ArrowCursor);
            db.rollback();
            return;
        }

        // ===========================
        // собственно открытие шаблона
        // ===========================
        // открытие полученного ODT
        long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);

        db.commit();
        //accept();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}


void acts_wnd::on_bn_pol2point_print_3_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, "Нет доступа к базе данных",
                             "Нет открытого соединения к базе данных?\n"
                             " Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n"
                             " Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику.");
        return;
    }
    if (!ui->tab_pol2point || !ui->tab_pol2point->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите строку в таблице.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_pol2point->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this, "Формирование печатной формы акта",
                                  "Запустить формирование печатной формы акта на передачу бланков полисов на ПВП?",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            QMessageBox::warning(this, "Действие отменено",
                                 "Формирование печатной формы акта на передачу бланков полисов на ПВП отменено пользователем.");
            return;
        }

        QModelIndex index = indexes.front();
        int id_act = model_pol2point.data(model_pol2point.index(index.row(), 0), Qt::EditRole).toInt();
        QString act_num = model_pol2point.data(model_pol2point.index(index.row(), 1), Qt::EditRole).toString();
        act_num = act_num.replace(" / ", "-").replace("/", "-");
        QDate act_date = model_pol2point.data(model_pol2point.index(index.row(), 2), Qt::EditRole).toDate();
        if (id_act<=0) {
            QMessageBox::warning(this, "Выберите акт",
                                 "Выбранная строка не соответствует ни одному акту.\n"
                                 "Выберите акт и повторите попытку.");
            return;
        }

        // сформруем печатную форму акта
        QString sql_data = "select bp.id as od_polis, 'бланк полиса' as pol_type, right('0000'||bp.pol_ser, 4) ||'  '|| right('0000000'||bp.pol_num, 7), 1 as cnt "
                           "  from blanks_pol bp "
                           " where id_blanks_pol_act=" + QString::number(id_act) + " "
                           " order by bp.pol_ser, bp.pol_num ; ";

        QSqlQuery *query_data = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_data, "Чтение данных акта", *query_data, true, db, data_app)) {
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке получить данные акта произошла неожиданная ошибка.\n\nДействие отменено.");
            delete query_data;
            return;
        }

        // подготовка данных для заполнения полей
        data_app.act_num  = act_num;
        data_app.act_date = act_date;
        delete  print_blanks_act_w;
        print_blanks_act_w = new print_blanks_act_wnd(db, data_app, settings, this);
        if (!print_blanks_act_w->exec())
            return;

        // распаковка шаблона акта
        QString fname_template = data_app.path_reports + "rep_blanks_act_2.odt";
        QString temp_folder = data_app.path_temp + "rep_blanks_act_2";
        if (!QFile::exists(fname_template)) {
            QMessageBox::warning(this,
                                 "Шаблон не найден",
                                 "Не найден шаблон печатной формы половозрастной таблицы прикреплённых к МО: \n" + fname_template +
                                 "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        QDir tmp_dir(temp_folder);
        if (tmp_dir.exists(temp_folder) && !tmp_dir.removeRecursively()) {
            QMessageBox::warning(this,
                                 "Ошибка при очистке папки",
                                 "Не удалось очистить папку временных файлов: \n" + temp_folder +
                                 "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        tmp_dir.mkpath(temp_folder);
        QProcess myProcess;
        QString com = "\"" + data_app.path_arch + "7z.exe\" e \"" + fname_template + "\" -o\"" + temp_folder + "\" content.xml";
        if (myProcess.execute(com)!=0) {
            QMessageBox::warning(this,
                                 "Ошибка распаковки шаблона",
                                 "Не удалось распаковать файл контента шаблона\n" + fname_template + " -> content.xml"
                                 "\nпечатной формы во временную папку\n" + temp_folder +
                                 "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        com = "\"" + data_app.path_arch + "7z.exe\" e \"" + fname_template + "\" -o\"" + temp_folder + "\" styles.xml";
        if (myProcess.execute(com)!=0) {
            QMessageBox::warning(this,
                                 "Ошибка распаковки шаблона",
                                 "Не удалось распаковать файл стилей шаблона \n" + fname_template + " -> styles.xml"
                                 "\nпечатной формы во временную папку\n" + temp_folder +
                                 "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }

        db.transaction();

        // ===========================
        // правка контента шаблона
        // ===========================
        // открытие контента шаблона
        QFile f_content(temp_folder + "/content.xml");
        f_content.open(QIODevice::ReadOnly);
        QString s_content = f_content.readAll();
        f_content.close();
        if (s_content.isEmpty()) {
            QMessageBox::warning(this,
                                 "Файл контента шаблона пустой",
                                 "Файл контента шаблона пустой. Возможно шаблон был испорчен.\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            db.rollback();
            return;
        }
        // выбор шаблона
        int pos0 = s_content.indexOf("<table:table-row table:style-name=\"Таблица4.4\">", 0);
        int pos1 = s_content.indexOf("<table:table-row table:style-name=\"Таблица4.5\">", 0);
        int pos2 = s_content.indexOf("</table:table-row>", pos0) + 18;
        QString s_row = s_content.mid(pos0, pos1-pos0);

        // правка полей контента шаблона
        QString s_content_new = s_content.left(pos0);
        s_content_new = s_content_new
                .replace("#DOG_NUM#", data_app.act_dog_num)
                .replace("#DOG_DATE#", date_to_str(data_app.act_base_doc_date))
                .replace("#CUR_DATE#", QDate::currentDate().toString("dd.MM.yyyy"))
                .replace("#FILIAL_CHIF_POST#", data_app.act_filial_chif_post)
                .replace("#FILIAL_CHIF_FIO#", data_app.act_filial_chif_fio)
                .replace("#POINT_CHIF_POST#", data_app.act_point_chif_post)
                .replace("#POINT_CHIF_FIO#", data_app.act_point_chif_fio)

                .replace("#POINT_ORG_NAME#", data_app.act_point_org_name)
                .replace("#POINT_ADDRESS#", data_app.act_point_address)
                .replace("#POINT_REQUISITS_1#", data_app.act_point_requisits_1)
                .replace("#POINT_REQUISITS_2#", data_app.act_point_requisits_2)
                .replace("#PT_OKPO#", data_app.act_point_okpo)
                .replace("#FILIAL_ORG_NAME#", data_app.act_filial_org_name)
                .replace("#FILIAL_ADDRESS#", data_app.act_filial_address)
                .replace("#FILIAL_REQUISITS_1#", data_app.act_filial_requisits_1)
                .replace("#FILIAL_REQUISITS_2#", data_app.act_filial_requisits_2)
                .replace("#FL_OKPO#", data_app.act_filial_okpo)

                .replace("#BASE_DOC_NAME#", data_app.act_base_doc_name)
                .replace("#DOC_NUM#", data_app.act_base_doc_num)
                .replace("#DOC_DATE#", date_to_str(data_app.act_base_doc_date))
                .replace("#ACT_NUM#", data_app.act_num)
                .replace("#ACT_DATE#", date_to_str(data_app.act_date))
                .replace("#COUNT#", data_app.act_count);


        // добавим все строки выборки
        int n = 0;
        {
            while (query_data->next()) {
                int     id_blank_vs  = query_data->value( 0).toInt();
                QString blank_type   = query_data->value( 1).toString();
                QString blank_sernum = query_data->value( 2).toString();
                int     blank_cnt    = query_data->value( 3).toInt();
                n += blank_cnt;

                QString s_row_new = s_row;
                s_row_new = s_row_new.
                        replace("#BLANK_NAME#",  blank_type).
                        replace("#BLANK_SERNUM#",blank_sernum).
                        replace("#BLANK_CNT#",   QString::number(blank_cnt));
                s_content_new += s_row_new;
            }
        }
        delete query_data;


        s_content_new += s_content.right(s_content.size()-pos1);
        s_content_new = s_content_new
                .replace("#CNT_ALL#", QString::number(n))
                .replace("#CNT#", QString::number(n))
                .replace("#FILIAL_CHIF_POST#", data_app.act_filial_chif_post)
                .replace("#FILIAL_CHIF_FIO#", data_app.act_filial_chif_fio)
                .replace("#TABEL_NUM#", data_app.act_tabel_num);


        // сохранение контента шаблона
        QFile f_content_save(temp_folder + "/content.xml");
        f_content_save.open(QIODevice::WriteOnly);
        f_content_save.write(s_content_new.toUtf8());
        f_content_save.close();

        // ===========================
        // архивация шаблона
        // ===========================
        QString fname_res = data_app.path_acts + "Акт - передача бланков полисов на ПВП - № '" + act_num + "' от " + act_date.toString("dd.MM.yyyy") + ".odt";

        QFile::remove(fname_res);
        while (QFile::exists(fname_res)) {
            if (QMessageBox::warning(this,
                                     "Ошибка сохранения акта передачи бланков полисов на ПВП",
                                     "Не удалось сохранить форму передаточного акта бланков полисов на ПВП: \n" + fname_template +
                                     "\n\nво временную папку\n" + fname_res +
                                     "\n\nПроверьте, не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                     QMessageBox::Retry|QMessageBox::Abort,
                                     QMessageBox::Retry)==QMessageBox::Abort) {
                this->setCursor(Qt::ArrowCursor);
                db.rollback();
                return;
            }
            QFile::remove(fname_res);
        }
        if (!QFile::copy(fname_template, fname_res) || !QFile::exists(fname_res)) {
            QMessageBox::warning(this, "Ошибка копирования отчёта", "Не удалось скопировать форму передаточного акта бланков полисов на ПВП: \n" + fname_template +
                                 "\n\nво временную папку\n" + fname_res +
                                 "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            db.rollback();
            return;
        }
        com = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname_res + "\" \"" + temp_folder + "/content.xml\"";
        if (myProcess.execute(com)!=0) {
            QMessageBox::warning(this,
                                 "Ошибка обновления контента",
                                 "При добавлении нового контента в форму передаточного акта бланков полисов на ПВП произошла непредвиденная ошибка\n\n"
                                 "Опреация прервана.");
            this->setCursor(Qt::ArrowCursor);
            db.rollback();
            return;
        }

        // ===========================
        // собственно открытие шаблона
        // ===========================
        // открытие полученного ODT
        long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);

        db.commit();
        //accept();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}


void acts_wnd::on_bn_vs2point_print_2_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_vs2point || !ui->tab_vs2point->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите строку в таблице.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_vs2point->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this, "Формирование печатной формы акта",
                                  "Запустить формирование печатной формы акта на передачу ВС на ПВП?",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            QMessageBox::warning(this, "Действие отменено",
                                 "Формирование печатной формы акта на передачу ВС на ПВП отменено пользователем.");
            return;
        }

        QModelIndex index = indexes.front();
        int id_act = model_vs2point.data(model_vs2point.index(index.row(), 0), Qt::EditRole).toInt();
        QString act_num = model_vs2point.data(model_vs2point.index(index.row(), 1), Qt::EditRole).toString();
        act_num = act_num.replace(" / ", "-").replace("/", "-");
        QDate act_date = model_vs2point.data(model_vs2point.index(index.row(), 2), Qt::EditRole).toDate();
        if (id_act<=0) {
            QMessageBox::warning(this, "Выберите акт",
                                 "Выбранная строка не соответствует ни одному акту.\n"
                                 "Выберите акт и повторите попытку.");
            return;
        }

        // сформруем печатную форму акта
        QString sql_data = "select bv.vs_num as id_vs, 'бланк ВС' as pol_type, right('00000000000'||bv.vs_num, 11), 1 as cnt "
                           "  from blanks_vs bv "
                           " where id_blanks_vs_act=" + QString::number(id_act) + " "
                           " order by bv.vs_num ; ";

        QSqlQuery *query_data = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_data, "Чтение данных акта", *query_data, true, db, data_app)) {
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке получить данные акта произошла неожиданная ошибка.\n\nДействие отменено.");
            delete query_data;
            return;
        }

        // подготовка данных для заполнения полей
        data_app.act_num  = act_num;
        data_app.act_date = act_date;
        delete  print_blanks_act_w;
        print_blanks_act_w = new print_blanks_act_wnd(db, data_app, settings, this);
        if (!print_blanks_act_w->exec())
            return;

        // распаковка шаблона акта
        QString fname_template = data_app.path_reports + "rep_blanks_act.odt";
        QString temp_folder = data_app.path_temp + "rep_blanks_act";
        if (!QFile::exists(fname_template)) {
            QMessageBox::warning(this,
                                 "Шаблон не найден",
                                 "Не найден шаблон печатной формы половозрастной таблицы прикреплённых к МО: \n" + fname_template +
                                 "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        QDir tmp_dir(temp_folder);
        if (tmp_dir.exists(temp_folder) && !tmp_dir.removeRecursively()) {
            QMessageBox::warning(this,
                                 "Ошибка при очистке папки",
                                 "Не удалось очистить папку временных файлов: \n" + temp_folder +
                                 "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        tmp_dir.mkpath(temp_folder);
        QProcess myProcess;
        QString com = "\"" + data_app.path_arch + "7z.exe\" e \"" + fname_template + "\" -o\"" + temp_folder + "\" content.xml";
        if (myProcess.execute(com)!=0) {
            QMessageBox::warning(this,
                                 "Ошибка распаковки шаблона",
                                 "Не удалось распаковать файл контента шаблона\n" + fname_template + " -> content.xml"
                                 "\nпечатной формы во временную папку\n" + temp_folder +
                                 "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        com = "\"" + data_app.path_arch + "7z.exe\" e \"" + fname_template + "\" -o\"" + temp_folder + "\" styles.xml";
        if (myProcess.execute(com)!=0) {
            QMessageBox::warning(this,
                                 "Ошибка распаковки шаблона",
                                 "Не удалось распаковать файл стилей шаблона \n" + fname_template + " -> styles.xml"
                                 "\nпечатной формы во временную папку\n" + temp_folder +
                                 "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }

        db.transaction();

        // ===========================
        // правка контента шаблона
        // ===========================
        // открытие контента шаблона
        QFile f_content(temp_folder + "/content.xml");
        f_content.open(QIODevice::ReadOnly);
        QString s_content = f_content.readAll();
        f_content.close();
        if (s_content.isEmpty()) {
            QMessageBox::warning(this,
                                 "Файл контента шаблона пустой",
                                 "Файл контента шаблона пустой. Возможно шаблон был испорчен.\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            db.rollback();
            return;
        }
        // выбор шаблона
        int pos0 = s_content.indexOf("<table:table-row table:style-name=\"Таблица2.3\">", 0);
        int pos1 = s_content.indexOf("</table:table-row>", pos0) + 18;
        QString s_row = s_content.mid(pos0, pos1-pos0);

        // правка полей контента шаблона
        QString s_content_new = s_content.left(pos0);
        s_content_new = s_content_new
                .replace("#DOG_NUM#", data_app.act_dog_num)
                .replace("#DOG_DATE#", date_to_str(data_app.act_base_doc_date))
                .replace("#FILIAL_CHIF_POST#", data_app.act_filial_chif_post)
                .replace("#FILIAL_CHIF_FIO#", data_app.act_filial_chif_fio)
                .replace("#POINT_CHIF_POST#", data_app.act_point_chif_post)
                .replace("#POINT_CHIF_FIO#", data_app.act_point_chif_fio)

                .replace("#POINT_ORG_NAME#", data_app.act_point_org_name)
                .replace("#POINT_ADDRESS#", data_app.act_point_address)
                .replace("#POINT_REQUISITS_1#", data_app.act_point_requisits_1)
                .replace("#POINT_REQUISITS_2#", data_app.act_point_requisits_2)
                .replace("#PT_OKPO#", data_app.act_point_okpo)
                .replace("#FILIAL_ORG_NAME#", data_app.act_filial_org_name)
                .replace("#FILIAL_ADDRESS#", data_app.act_filial_address)
                .replace("#FILIAL_REQUISITS_1#", data_app.act_filial_requisits_1)
                .replace("#FILIAL_REQUISITS_2#", data_app.act_filial_requisits_2)
                .replace("#FL_OKPO#", data_app.act_filial_okpo)

                .replace("#BASE_DOC_NAME#", data_app.act_base_doc_name)
                .replace("#DOC_NUM#", data_app.act_base_doc_num)
                .replace("#DOC_DATE#", date_to_str(data_app.act_base_doc_date))
                .replace("#ACT_NUM#", data_app.act_num)
                .replace("#ACT_DATE#", date_to_str(data_app.act_date))
                .replace("#COUNT#", data_app.act_count);


        // добавим все строки выборки
        int n = 0;
        {
            while (query_data->next()) {
                int     id_blank_vs  = query_data->value( 0).toInt();
                QString blank_type   = query_data->value( 1).toString();
                QString blank_sernum = query_data->value( 2).toString();
                int     blank_cnt    = query_data->value( 3).toInt();
                n += blank_cnt;

                QString s_row_new = s_row;
                s_row_new = s_row_new.
                        replace("#BLANK_NAME#",  blank_type).
                        replace("#BLANK_SERNUM#",blank_sernum).
                        replace("#BLANK_CNT#",   QString::number(blank_cnt));
                s_content_new += s_row_new;
            }
        }
        delete query_data;


        s_content_new += s_content.right(s_content.size()-pos1);
        s_content_new = s_content_new
                .replace("#CNT#", QString::number(n))
                .replace("#FILIAL_CHIF_POST#", data_app.act_filial_chif_post)
                .replace("#FILIAL_CHIF_FIO#", data_app.act_filial_chif_fio)
                .replace("#TABEL_NUM#", data_app.act_tabel_num);


        // сохранение контента шаблона
        QFile f_content_save(temp_folder + "/content.xml");
        f_content_save.open(QIODevice::WriteOnly);
        f_content_save.write(s_content_new.toUtf8());
        f_content_save.close();

        // ===========================
        // архивация шаблона
        // ===========================
        QString fname_res = data_app.path_acts + "Акт - передача бланков ВС на ПВП - № '" + act_num + "' от " + act_date.toString("dd.MM.yyyy") + ".odt";

        QFile::remove(fname_res);
        while (QFile::exists(fname_res)) {
            if (QMessageBox::warning(this,
                                     "Ошибка сохранения акта передачи ВС на ПВП",
                                     "Не удалось сохранить форму передаточного акта бланков ВС на ПВП: \n" + fname_template +
                                     "\n\nво временную папку\n" + fname_res +
                                     "\n\nПроверьте, не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                     QMessageBox::Retry|QMessageBox::Abort,
                                     QMessageBox::Retry)==QMessageBox::Abort) {
                this->setCursor(Qt::ArrowCursor);
                db.rollback();
                return;
            }
            QFile::remove(fname_res);
        }
        if (!QFile::copy(fname_template, fname_res) || !QFile::exists(fname_res)) {
            QMessageBox::warning(this, "Ошибка копирования отчёта", "Не удалось скопировать форму передаточного акта бланков ВС на ПВП: \n" + fname_template +
                                 "\n\nво временную папку\n" + fname_res +
                                 "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            db.rollback();
            return;
        }
        com = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname_res + "\" \"" + temp_folder + "/content.xml\"";
        if (myProcess.execute(com)!=0) {
            QMessageBox::warning(this,
                                 "Ошибка обновления контента",
                                 "При добавлении нового контента в форму передаточного акта бланков ВС на ПВП произошла непредвиденная ошибка\n\n"
                                 "Опреация прервана.");
            this->setCursor(Qt::ArrowCursor);
            db.rollback();
            return;
        }

        // ===========================
        // собственно открытие шаблона
        // ===========================
        // открытие полученного ODT
        long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);

        db.commit();
        //accept();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void acts_wnd::on_bn_filter_vs2point_clicked() {
    ui->ln_filter_vs2point->clear();
    refresh_vs2point_tab();
}
void acts_wnd::on_bn_filter_pol2point_clicked() {
    ui->ln_filter_pol2point->clear();
    refresh_pol2point_tab();
}
void acts_wnd::on_ln_filter_vs2point_editingFinished() {
    refresh_vs2point_tab();
}
void acts_wnd::on_ln_filter_pol2point_editingFinished() {
    refresh_pol2point_tab();
}


void acts_wnd::on_tab_vs2smo_activated(const QModelIndex &index) {
    on_bn_data_vs2smo_clicked();
}
void acts_wnd::on_tab_pol2smo_activated(const QModelIndex &index) {
    on_bn_data_pol2smo_clicked();
}
void acts_wnd::on_tab_vs2point_activated(const QModelIndex &index) {
    on_bn_data_vs2point_clicked();
}
void acts_wnd::on_tab_pol2point_activated(const QModelIndex &index) {
    on_bn_data_pol2point_clicked();
}

void acts_wnd::on_combo_ord_vs2smo_activated(const QString &arg1) {
    refresh_vs2smo_tab();
}
void acts_wnd::on_combo_ord_pol2smo_activated(const QString &arg1) {
    refresh_pol2smo_tab();
}
void acts_wnd::on_combo_ord_vs2point_activated(const QString &arg1) {
    refresh_vs2point_tab();
}
void acts_wnd::on_combo_ord_pol2point_activated(const QString &arg1) {
    refresh_pol2point_tab();
}

void acts_wnd::on_ch_filter_vs2point_clicked(bool checked) {
    ui->pan_filter_vs2point->setVisible(checked);
    if (!checked)
        ui->ln_filter_vs2point->clear();
    refresh_vs2point_tab();
}
void acts_wnd::on_ch_filter_pol2point_clicked(bool checked) {
    ui->pan_filter_pol2point->setVisible(checked);
    if (!checked)
        ui->ln_filter_pol2point->clear();
    refresh_pol2point_tab();
}

void acts_wnd::on_bn_toCSV_vs2smo_clicked() {
    QString sql = this->sql_vs2smo;

    sql = sql.replace(";"," ");
    sql = sql.replace("\n"," ");

    QString fname_s = QFileDialog::getSaveFileName(this,
                                                   "Куда сохранить файл?",
                                                   data_app.path_out + QDate::currentDate().toString("yyyy-MM-dd") + " - " + data_app.filial_name + "  -  акты передачи бланков ВС в СМО.csv",
                                                   "файлы данных (*.csv);;простой текст (*.txt)");
    if (fname_s.isEmpty()) {
        QMessageBox::warning(this,
                             "Не выбрано имя файла",
                             "Имя цедевого файла не задано.\n\n"
                             "Операция отменена.");
        return;
    }
    if (QFile::exists(fname_s)) {
        if (QMessageBox::question(this,
                                  "Удалить старый файл?",
                                  "Файл уже существует.\n"
                                  "Удалить старый файл для того чтобы сохранить новый?",
                                  QMessageBox::Yes|QMessageBox::Cancel)==QMessageBox::Yes) {
            if (!QFile::remove(fname_s)) {
                QMessageBox::warning(this,
                                     "Ошибка при удалении старого файла",
                                     "При удалении старого файла произошла непредвиденная ошибка.\n\n"
                                     "Операция отменена.");
                return;
            }
        }
    }


    // сохраним данные в файл
    QFile file;
    file.setFileName(fname_s);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this,
                             "Ошибка создания файла",
                             "При создании целевого файла произошла ошибка.\n\n"
                             "Операция отменена.");
        return;
    }

    // подготовим поток для сохранения файла
    QTextStream stream(&file);
    QString codec_name = "Windows-1251";
    QTextCodec *codec = QTextCodec::codecForName(codec_name.toLocal8Bit());
    stream.setCodec(codec);


    QString head;
    int cnt_head = 0;
    cnt_head = model_vs2smo.columnCount();
    for (int i=0; i<cnt_head; i++) {
        if (ui->tab_vs2smo->columnWidth(i)>1) {
            head += model_vs2smo.headerData(i,Qt::Horizontal).toString().replace("\n"," ");
            if (i<cnt_head-1)  head += ";";
        }
    }
    stream << head << "\n";

    // переберём полученные данные и сохраним в файл
    QSqlQuery *query_export = new QSqlQuery(db);
    query_export->exec(sql);
    while (query_export->next()) {
        for (int j=0; j<cnt_head; j++) {
            if (ui->tab_vs2smo->columnWidth(j)>1) {
                QString value;
                if (query_export->value(j).type()==QMetaType::QDate) {
                    value = query_export->value(j).toDate().toString("dd.MM.yyyy");
                } else {
                    value = query_export->value(j).toString().trimmed().replace(";",",");
                }
                if (!value.isEmpty()) {
                    //stream << "'";
                    stream << value;
                    //stream << "'";
                }
                if (j<cnt_head-1)  stream << ";";
            }
        }
        stream << "\n";
    }
    file.close();

    // ===========================
    // собственно открытие шаблона
    // ===========================
    // открытие полученного ODT
    /*long result = (long)*/ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_s.utf16()), 0, 0, SW_NORMAL);
}

void acts_wnd::on_bn_toCSV_pol2smo_clicked() {
    QString sql = this->sql_pol2smo;

    sql = sql.replace(";"," ");
    sql = sql.replace("\n"," ");

    QString fname_s = QFileDialog::getSaveFileName(this,
                                                   "Куда сохранить файл?",
                                                   data_app.path_out + QDate::currentDate().toString("yyyy-MM-dd") + " - " + data_app.filial_name + "  -  акты передачи бланков полисов в СМО.csv",
                                                   "файлы данных (*.csv);;простой текст (*.txt)");
    if (fname_s.isEmpty()) {
        QMessageBox::warning(this,
                             "Не выбрано имя файла",
                             "Имя цедевого файла не задано.\n\n"
                             "Операция отменена.");
        return;
    }
    if (QFile::exists(fname_s)) {
        if (QMessageBox::question(this,
                                  "Удалить старый файл?",
                                  "Файл уже существует.\n"
                                  "Удалить старый файл для того чтобы сохранить новый?",
                                  QMessageBox::Yes|QMessageBox::Cancel)==QMessageBox::Yes) {
            if (!QFile::remove(fname_s)) {
                QMessageBox::warning(this,
                                     "Ошибка при удалении старого файла",
                                     "При удалении старого файла произошла непредвиденная ошибка.\n\n"
                                     "Операция отменена.");
                return;
            }
        }
    }


    // сохраним данные в файл
    QFile file;
    file.setFileName(fname_s);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this,
                             "Ошибка создания файла",
                             "При создании целевого файла произошла ошибка.\n\n"
                             "Операция отменена.");
        return;
    }

    // подготовим поток для сохранения файла
    QTextStream stream(&file);
    QString codec_name = "Windows-1251";
    QTextCodec *codec = QTextCodec::codecForName(codec_name.toLocal8Bit());
    stream.setCodec(codec);


    QString head;
    int cnt_head = 0;
    cnt_head = model_pol2smo.columnCount();
    for (int i=0; i<cnt_head; i++) {
        if (ui->tab_pol2smo->columnWidth(i)>1) {
            head += model_pol2smo.headerData(i,Qt::Horizontal).toString().replace("\n"," ");
            if (i<cnt_head-1)  head += ";";
        }
    }
    stream << head << "\n";

    // переберём полученные данные и сохраним в файл
    QSqlQuery *query_export = new QSqlQuery(db);
    query_export->exec(sql);
    while (query_export->next()) {
        for (int j=0; j<cnt_head; j++) {
            if (ui->tab_pol2smo->columnWidth(j)>1) {
                QString value;
                if (query_export->value(j).type()==QMetaType::QDate) {
                    value = query_export->value(j).toDate().toString("dd.MM.yyyy");
                } else {
                    value = query_export->value(j).toString().trimmed().replace(";",",");
                }
                if (!value.isEmpty()) {
                    //stream << "'";
                    stream << value;
                    //stream << "'";
                }
                if (j<cnt_head-1)  stream << ";";
            }
        }
        stream << "\n";
    }
    file.close();

    // ===========================
    // собственно открытие шаблона
    // ===========================
    // открытие полученного ODT
    /*long result = (long)*/ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_s.utf16()), 0, 0, SW_NORMAL);
}

void acts_wnd::on_bn_toCSV_vs2point_clicked() {
    QString sql = this->sql_vs2point;

    sql = sql.replace(";"," ");
    sql = sql.replace("\n"," ");

    QString fname_s = QFileDialog::getSaveFileName(this,
                                                   "Куда сохранить файл?",
                                                   data_app.path_out + QDate::currentDate().toString("yyyy-MM-dd") + " - " + data_app.filial_name + "  -  акты передачи бланков ВС на ПВП.csv",
                                                   "файлы данных (*.csv);;простой текст (*.txt)");
    if (fname_s.isEmpty()) {
        QMessageBox::warning(this,
                             "Не выбрано имя файла",
                             "Имя цедевого файла не задано.\n\n"
                             "Операция отменена.");
        return;
    }
    if (QFile::exists(fname_s)) {
        if (QMessageBox::question(this,
                                  "Удалить старый файл?",
                                  "Файл уже существует.\n"
                                  "Удалить старый файл для того чтобы сохранить новый?",
                                  QMessageBox::Yes|QMessageBox::Cancel)==QMessageBox::Yes) {
            if (!QFile::remove(fname_s)) {
                QMessageBox::warning(this,
                                     "Ошибка при удалении старого файла",
                                     "При удалении старого файла произошла непредвиденная ошибка.\n\n"
                                     "Операция отменена.");
                return;
            }
        }
    }


    // сохраним данные в файл
    QFile file;
    file.setFileName(fname_s);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this,
                             "Ошибка создания файла",
                             "При создании целевого файла произошла ошибка.\n\n"
                             "Операция отменена.");
        return;
    }

    // подготовим поток для сохранения файла
    QTextStream stream(&file);
    QString codec_name = "Windows-1251";
    QTextCodec *codec = QTextCodec::codecForName(codec_name.toLocal8Bit());
    stream.setCodec(codec);


    QString head;
    int cnt_head = 0;
    cnt_head = model_vs2point.columnCount();
    for (int i=0; i<cnt_head; i++) {
        if (ui->tab_vs2point->columnWidth(i)>1) {
            head += model_vs2point.headerData(i,Qt::Horizontal).toString().replace("\n"," ");
            if (i<cnt_head-1)  head += ";";
        }
    }
    stream << head << "\n";

    // переберём полученные данные и сохраним в файл
    QSqlQuery *query_export = new QSqlQuery(db);
    query_export->exec(sql);
    while (query_export->next()) {
        for (int j=0; j<cnt_head; j++) {
            if (ui->tab_vs2point->columnWidth(j)>1) {
                QString value;
                if (query_export->value(j).type()==QMetaType::QDate) {
                    value = query_export->value(j).toDate().toString("dd.MM.yyyy");
                } else {
                    value = query_export->value(j).toString().trimmed().replace(";",",");
                }
                if (!value.isEmpty()) {
                    //stream << "'";
                    stream << value;
                    //stream << "'";
                }
                if (j<cnt_head-1)  stream << ";";
            }
        }
        stream << "\n";
    }
    file.close();

    // ===========================
    // собственно открытие шаблона
    // ===========================
    // открытие полученного ODT
    /*long result = (long)*/ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_s.utf16()), 0, 0, SW_NORMAL);
}

void acts_wnd::on_bn_toCSV_pol2point_clicked() {
    QString sql = this->sql_pol2point;

    sql = sql.replace(";"," ");
    sql = sql.replace("\n"," ");

    QString fname_s = QFileDialog::getSaveFileName(this,
                                                   "Куда сохранить файл?",
                                                   data_app.path_out + QDate::currentDate().toString("yyyy-MM-dd") + " - " + data_app.filial_name + "  -  акты передачи бланков полисов на ПВП.csv",
                                                   "файлы данных (*.csv);;простой текст (*.txt)");
    if (fname_s.isEmpty()) {
        QMessageBox::warning(this,
                             "Не выбрано имя файла",
                             "Имя цедевого файла не задано.\n\n"
                             "Операция отменена.");
        return;
    }
    if (QFile::exists(fname_s)) {
        if (QMessageBox::question(this,
                                  "Удалить старый файл?",
                                  "Файл уже существует.\n"
                                  "Удалить старый файл для того чтобы сохранить новый?",
                                  QMessageBox::Yes|QMessageBox::Cancel)==QMessageBox::Yes) {
            if (!QFile::remove(fname_s)) {
                QMessageBox::warning(this,
                                     "Ошибка при удалении старого файла",
                                     "При удалении старого файла произошла непредвиденная ошибка.\n\n"
                                     "Операция отменена.");
                return;
            }
        }
    }


    // сохраним данные в файл
    QFile file;
    file.setFileName(fname_s);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this,
                             "Ошибка создания файла",
                             "При создании целевого файла произошла ошибка.\n\n"
                             "Операция отменена.");
        return;
    }

    // подготовим поток для сохранения файла
    QTextStream stream(&file);
    QString codec_name = "Windows-1251";
    QTextCodec *codec = QTextCodec::codecForName(codec_name.toLocal8Bit());
    stream.setCodec(codec);


    QString head;
    int cnt_head = 0;
    cnt_head = model_pol2point.columnCount();
    for (int i=0; i<cnt_head; i++) {
        if (ui->tab_pol2point->columnWidth(i)>1) {
            head += model_pol2point.headerData(i,Qt::Horizontal).toString().replace("\n"," ");
            if (i<cnt_head-1)  head += ";";
        }
    }
    stream << head << "\n";

    // переберём полученные данные и сохраним в файл
    QSqlQuery *query_export = new QSqlQuery(db);
    query_export->exec(sql);
    while (query_export->next()) {
        for (int j=0; j<cnt_head; j++) {
            if (ui->tab_pol2point->columnWidth(j)>1) {
                QString value;
                if (query_export->value(j).type()==QMetaType::QDate) {
                    value = query_export->value(j).toDate().toString("dd.MM.yyyy");
                } else {
                    value = query_export->value(j).toString().trimmed().replace(";",",");
                }
                if (!value.isEmpty()) {
                    //stream << "'";
                    stream << value;
                    //stream << "'";
                }
                if (j<cnt_head-1)  stream << ";";
            }
        }
        stream << "\n";
    }
    file.close();

    // ===========================
    // собственно открытие шаблона
    // ===========================
    // открытие полученного ODT
    /*long result = (long)*/ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_s.utf16()), 0, 0, SW_NORMAL);
}


void acts_wnd::on_bn_pol2point_print_2_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_pol2point || !ui->tab_pol2point->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите строку в таблице.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_pol2point->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this, "Формирование печатной формы акта",
                                  "Запустить формирование печатной формы акта на передачу полиса на ПВП?",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            QMessageBox::warning(this, "Действие отменено",
                                 "Формирование печатной формы акта на передачу полиса на ПВП отменено пользователем.");
            return;
        }

        QModelIndex index = indexes.front();
        int id_act = model_pol2point.data(model_pol2point.index(index.row(), 0), Qt::EditRole).toInt();
        QString act_num = model_pol2point.data(model_pol2point.index(index.row(), 1), Qt::EditRole).toString();
        act_num = act_num.replace(" / ", "-").replace("/", "-");
        QDate act_date = model_pol2point.data(model_pol2point.index(index.row(), 2), Qt::EditRole).toDate();
        if (id_act<=0) {
            QMessageBox::warning(this, "Выберите акт",
                                 "Выбранная строка не соответствует ни одному акту.\n"
                                 "Выберите акт и повторите попытку.");
            return;
        }

        // сформруем печатную форму акта
        QString sql_data = "select bp.id, 'бланк полиса' as pol_type, right('0000'||bp.pol_ser, 4) || ' ' || right('0000000'||bp.pol_num, 7), 1 as cnt "
                           "  from blanks_pol bp "
                           " where id_blanks_pol_act=" + QString::number(id_act) + " "
                           " order by bp.pol_ser, bp.pol_num ; ";

        QSqlQuery *query_data = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_data, "Чтение данных акта", *query_data, true, db, data_app)) {
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке получить данные акта произошла неожиданная ошибка.\n\nДействие отменено.");
            delete query_data;
            return;
        }

        // подготовка данных для заполнения полей
        data_app.act_num  = act_num;
        data_app.act_date = act_date;
        delete  print_blanks_act_w;
        print_blanks_act_w = new print_blanks_act_wnd(db, data_app, settings, this);
        if (!print_blanks_act_w->exec())
            return;

        // распаковка шаблона акта
        QString fname_template = data_app.path_reports + "rep_blanks_act.odt";
        QString temp_folder = data_app.path_temp + "rep_blanks_act";
        if (!QFile::exists(fname_template)) {
            QMessageBox::warning(this,
                                 "Шаблон не найден",
                                 "Не найден шаблон печатной формы акта передачи полисов на ПВП: \n" + fname_template +
                                 "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        QDir tmp_dir(temp_folder);
        if (tmp_dir.exists(temp_folder) && !tmp_dir.removeRecursively()) {
            QMessageBox::warning(this,
                                 "Ошибка при очистке папки",
                                 "Не удалось очистить папку временных файлов: \n" + temp_folder +
                                 "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        tmp_dir.mkpath(temp_folder);
        QProcess myProcess;
        QString com = "\"" + data_app.path_arch + "7z.exe\" e \"" + fname_template + "\" -o\"" + temp_folder + "\" content.xml";
        if (myProcess.execute(com)!=0) {
            QMessageBox::warning(this,
                                 "Ошибка распаковки шаблона",
                                 "Не удалось распаковать файл контента шаблона\n" + fname_template + " -> content.xml"
                                 "\nпечатной формы во временную папку\n" + temp_folder +
                                 "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        com = "\"" + data_app.path_arch + "7z.exe\" e \"" + fname_template + "\" -o\"" + temp_folder + "\" styles.xml";
        if (myProcess.execute(com)!=0) {
            QMessageBox::warning(this,
                                 "Ошибка распаковки шаблона",
                                 "Не удалось распаковать файл стилей шаблона \n" + fname_template + " -> styles.xml"
                                 "\nпечатной формы во временную папку\n" + temp_folder +
                                 "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }

        db.transaction();

        // ===========================
        // правка контента шаблона
        // ===========================
        // открытие контента шаблона
        QFile f_content(temp_folder + "/content.xml");
        f_content.open(QIODevice::ReadOnly);
        QString s_content = f_content.readAll();
        f_content.close();
        if (s_content.isEmpty()) {
            QMessageBox::warning(this,
                                 "Файл контента шаблона пустой",
                                 "Файл контента шаблона пустой. Возможно шаблон был испорчен.\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            db.rollback();
            return;
        }
        // выбор шаблона
        int pos0 = s_content.indexOf("<table:table-row table:style-name=\"Таблица2.3\">", 0);
        int pos1 = s_content.indexOf("</table:table-row>", pos0) + 18;
        QString s_row = s_content.mid(pos0, pos1-pos0);

        // правка полей контента шаблона
        QString s_content_new = s_content.left(pos0);
        s_content_new = s_content_new
                .replace("#DOG_NUM#", data_app.act_dog_num)
                .replace("#DOG_DATE#", date_to_str(data_app.act_base_doc_date))
                .replace("#FILIAL_CHIF_POST#", data_app.act_filial_chif_post)
                .replace("#FILIAL_CHIF_FIO#", data_app.act_filial_chif_fio)
                .replace("#POINT_CHIF_POST#", data_app.act_point_chif_post)
                .replace("#POINT_CHIF_FIO#", data_app.act_point_chif_fio)

                .replace("#POINT_ORG_NAME#", data_app.act_point_org_name)
                .replace("#POINT_ADDRESS#", data_app.act_point_address)
                .replace("#POINT_REQUISITS_1#", data_app.act_point_requisits_1)
                .replace("#POINT_REQUISITS_2#", data_app.act_point_requisits_2)
                .replace("#PT_OKPO#", data_app.act_point_okpo)
                .replace("#FILIAL_ORG_NAME#", data_app.act_filial_org_name)
                .replace("#FILIAL_ADDRESS#", data_app.act_filial_address)
                .replace("#FILIAL_REQUISITS_1#", data_app.act_filial_requisits_1)
                .replace("#FILIAL_REQUISITS_2#", data_app.act_filial_requisits_2)
                .replace("#FL_OKPO#", data_app.act_filial_okpo)

                .replace("#BASE_DOC_NAME#", data_app.act_base_doc_name)
                .replace("#DOC_NUM#", data_app.act_base_doc_num)
                .replace("#DOC_DATE#", date_to_str(data_app.act_base_doc_date))
                .replace("#ACT_NUM#", data_app.act_num)
                .replace("#ACT_DATE#", date_to_str(data_app.act_date))
                .replace("#COUNT#", data_app.act_count);


        // добавим все строки выборки
        int n = 0;
        {
            while (query_data->next()) {
                int     id_blank_vs  = query_data->value( 0).toInt();
                QString blank_type   = query_data->value( 1).toString();
                QString blank_sernum = query_data->value( 2).toString();
                int     blank_cnt    = query_data->value( 3).toInt();
                n += blank_cnt;

                QString s_row_new = s_row;
                s_row_new = s_row_new.
                        replace("#BLANK_NAME#",  blank_type).
                        replace("#BLANK_SERNUM#",blank_sernum).
                        replace("#BLANK_CNT#",   QString::number(blank_cnt));
                s_content_new += s_row_new;
            }
        }
        delete query_data;


        s_content_new += s_content.right(s_content.size()-pos1);
        s_content_new = s_content_new
                .replace("#CNT#", QString::number(n))
                .replace("#FILIAL_CHIF_POST#", data_app.act_filial_chif_post)
                .replace("#FILIAL_CHIF_FIO#", data_app.act_filial_chif_fio)
                .replace("#TABEL_NUM#", data_app.act_tabel_num);


        // сохранение контента шаблона
        QFile f_content_save(temp_folder + "/content.xml");
        f_content_save.open(QIODevice::WriteOnly);
        f_content_save.write(s_content_new.toUtf8());
        f_content_save.close();

        // ===========================
        // архивация шаблона
        // ===========================
        QString fname_res = data_app.path_acts + "Акт - передача бланков полиса на ПВП - № '" + act_num + "' от " + act_date.toString("dd.MM.yyyy") + ".odt";

        QFile::remove(fname_res);
        while (QFile::exists(fname_res)) {
            if (QMessageBox::warning(this,
                                     "Ошибка сохранения акта передачи полиса на ПВП",
                                     "Не удалось сохранить форму передаточного акта бланков полиса на ПВП: \n" + fname_template +
                                     "\n\nво временную папку\n" + fname_res +
                                     "\n\nПроверьте, не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                     QMessageBox::Retry|QMessageBox::Abort,
                                     QMessageBox::Retry)==QMessageBox::Abort) {
                this->setCursor(Qt::ArrowCursor);
                db.rollback();
                return;
            }
            QFile::remove(fname_res);
        }
        if (!QFile::copy(fname_template, fname_res) || !QFile::exists(fname_res)) {
            QMessageBox::warning(this, "Ошибка копирования отчёта", "Не удалось скопировать форму передаточного акта бланков полиса на ПВП: \n" + fname_template +
                                 "\n\nво временную папку\n" + fname_res +
                                 "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            db.rollback();
            return;
        }
        com = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname_res + "\" \"" + temp_folder + "/content.xml\"";
        if (myProcess.execute(com)!=0) {
            QMessageBox::warning(this,
                                 "Ошибка обновления контента",
                                 "При добавлении нового контента в форму передаточного акта бланков полиса на ПВП произошла непредвиденная ошибка\n\n"
                                 "Опреация прервана.");
            this->setCursor(Qt::ArrowCursor);
            db.rollback();
            return;
        }

        // ===========================
        // собственно открытие шаблона
        // ===========================
        // открытие полученного ODT
        long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);

        db.commit();
        //accept();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void acts_wnd::on_bn_vs2point_print_3_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_vs2point || !ui->tab_vs2point->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите строку в таблице.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_vs2point->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this, "Формирование печатной формы акта",
                                  "Запустить формирование печатной формы акта на передачу ВС на ПВП?",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            QMessageBox::warning(this, "Действие отменено",
                                 "Формирование печатной формы акта на передачу ВС на ПВП отменено пользователем.");
            return;
        }

        QModelIndex index = indexes.front();
        int id_act = model_vs2point.data(model_vs2point.index(index.row(), 0), Qt::EditRole).toInt();
        QString act_num = model_vs2point.data(model_vs2point.index(index.row(), 1), Qt::EditRole).toString();
        act_num = act_num.replace(" / ", "-").replace("/", "-");
        QDate act_date = model_vs2point.data(model_vs2point.index(index.row(), 2), Qt::EditRole).toDate();
        if (id_act<=0) {
            QMessageBox::warning(this, "Выберите акт",
                                 "Выбранная строка не соответствует ни одному акту.\n"
                                 "Выберите акт и повторите попытку.");
            return;
        }

        // сформруем печатную форму акта
        QString sql_data = "select bv.vs_num as id_vs, 'временное св-во' as pol_type, right('00000000000'||bv.vs_num, 11), 1 as cnt "
                           "  from blanks_vs bv "
                           " where id_blanks_vs_act=" + QString::number(id_act) + " "
                           " order by bv.vs_num ; ";

        QSqlQuery *query_data = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_data, "Чтение данных акта", *query_data, true, db, data_app)) {
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке получить данные акта произошла неожиданная ошибка.\n\nДействие отменено.");
            delete query_data;
            return;
        }

        // подготовка данных для заполнения полей
        data_app.act_num  = act_num;
        data_app.act_date = act_date;
        delete  print_blanks_act_w;
        print_blanks_act_w = new print_blanks_act_wnd(db, data_app, settings, this);
        if (!print_blanks_act_w->exec())
            return;

        // распаковка шаблона акта
        QString fname_template = data_app.path_reports + "rep_blanks_act_2.odt";
        QString temp_folder = data_app.path_temp + "rep_blanks_act_2";
        if (!QFile::exists(fname_template)) {
            QMessageBox::warning(this,
                                 "Шаблон не найден",
                                 "Не найден шаблон печатной формы половозрастной таблицы прикреплённых к МО: \n" + fname_template +
                                 "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        QDir tmp_dir(temp_folder);
        if (tmp_dir.exists(temp_folder) && !tmp_dir.removeRecursively()) {
            QMessageBox::warning(this,
                                 "Ошибка при очистке папки",
                                 "Не удалось очистить папку временных файлов: \n" + temp_folder +
                                 "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        tmp_dir.mkpath(temp_folder);
        QProcess myProcess;
        QString com = "\"" + data_app.path_arch + "7z.exe\" e \"" + fname_template + "\" -o\"" + temp_folder + "\" content.xml";
        if (myProcess.execute(com)!=0) {
            QMessageBox::warning(this,
                                 "Ошибка распаковки шаблона",
                                 "Не удалось распаковать файл контента шаблона\n" + fname_template + " -> content.xml"
                                 "\nпечатной формы во временную папку\n" + temp_folder +
                                 "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        com = "\"" + data_app.path_arch + "7z.exe\" e \"" + fname_template + "\" -o\"" + temp_folder + "\" styles.xml";
        if (myProcess.execute(com)!=0) {
            QMessageBox::warning(this,
                                 "Ошибка распаковки шаблона",
                                 "Не удалось распаковать файл стилей шаблона \n" + fname_template + " -> styles.xml"
                                 "\nпечатной формы во временную папку\n" + temp_folder +
                                 "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }

        db.transaction();

        // ===========================
        // правка контента шаблона
        // ===========================
        // открытие контента шаблона
        QFile f_content(temp_folder + "/content.xml");
        f_content.open(QIODevice::ReadOnly);
        QString s_content = f_content.readAll();
        f_content.close();
        if (s_content.isEmpty()) {
            QMessageBox::warning(this,
                                 "Файл контента шаблона пустой",
                                 "Файл контента шаблона пустой. Возможно шаблон был испорчен.\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            db.rollback();
            return;
        }
        // выбор шаблона
        int pos0 = s_content.indexOf("<table:table-row table:style-name=\"Таблица4.4\">", 0);
        int pos1 = s_content.indexOf("<table:table-row table:style-name=\"Таблица4.5\">", 0);
        int pos2 = s_content.indexOf("</table:table-row>", pos0) + 18;
        QString s_row = s_content.mid(pos0, pos1-pos0);

        // правка полей контента шаблона
        QString s_content_new = s_content.left(pos0);
        s_content_new = s_content_new
                .replace("#DOG_NUM#", data_app.act_dog_num)
                .replace("#DOG_DATE#", date_to_str(data_app.act_base_doc_date))
                .replace("#CUR_DATE#", QDate::currentDate().toString("dd.MM.yyyy"))
                .replace("#FILIAL_CHIF_POST#", data_app.act_filial_chif_post)
                .replace("#FILIAL_CHIF_FIO#", data_app.act_filial_chif_fio)
                .replace("#POINT_CHIF_POST#", data_app.act_point_chif_post)
                .replace("#POINT_CHIF_FIO#", data_app.act_point_chif_fio)

                .replace("#POINT_ORG_NAME#", data_app.act_point_org_name)
                .replace("#POINT_ADDRESS#", data_app.act_point_address)
                .replace("#POINT_REQUISITS_1#", data_app.act_point_requisits_1)
                .replace("#POINT_REQUISITS_2#", data_app.act_point_requisits_2)
                .replace("#PT_OKPO#", data_app.act_point_okpo)
                .replace("#FILIAL_ORG_NAME#", data_app.act_filial_org_name)
                .replace("#FILIAL_ADDRESS#", data_app.act_filial_address)
                .replace("#FILIAL_REQUISITS_1#", data_app.act_filial_requisits_1)
                .replace("#FILIAL_REQUISITS_2#", data_app.act_filial_requisits_2)
                .replace("#FL_OKPO#", data_app.act_filial_okpo)

                .replace("#BASE_DOC_NAME#", data_app.act_base_doc_name)
                .replace("#DOC_NUM#", data_app.act_base_doc_num)
                .replace("#DOC_DATE#", date_to_str(data_app.act_base_doc_date))
                .replace("#ACT_NUM#", data_app.act_num)
                .replace("#ACT_DATE#", date_to_str(data_app.act_date))
                .replace("#COUNT#", data_app.act_count);


        // добавим все строки выборки
        int n = 0;
        {
            while (query_data->next()) {
                int     id_blank_vs  = query_data->value( 0).toInt();
                QString blank_type   = query_data->value( 1).toString();
                QString blank_sernum = query_data->value( 2).toString();
                int     blank_cnt    = query_data->value( 3).toInt();
                n += blank_cnt;

                QString s_row_new = s_row;
                s_row_new = s_row_new.
                        replace("#BLANK_NAME#",  blank_type).
                        replace("#BLANK_SERNUM#",blank_sernum).
                        replace("#BLANK_CNT#",   QString::number(blank_cnt));
                s_content_new += s_row_new;
            }
        }
        delete query_data;


        s_content_new += s_content.right(s_content.size()-pos1);
        s_content_new = s_content_new
                .replace("#CNT_ALL#", QString::number(n))
                .replace("#CNT#", QString::number(n))
                .replace("#FILIAL_CHIF_POST#", data_app.act_filial_chif_post)
                .replace("#FILIAL_CHIF_FIO#", data_app.act_filial_chif_fio)
                .replace("#TABEL_NUM#", data_app.act_tabel_num);


        // сохранение контента шаблона
        QFile f_content_save(temp_folder + "/content.xml");
        f_content_save.open(QIODevice::WriteOnly);
        f_content_save.write(s_content_new.toUtf8());
        f_content_save.close();

        // ===========================
        // архивация шаблона
        // ===========================
        QString fname_res = data_app.path_acts + "Акт - передача бланков ВС на ПВП - № '" + act_num + "' от " + act_date.toString("dd.MM.yyyy") + ".odt";

        QFile::remove(fname_res);
        while (QFile::exists(fname_res)) {
            if (QMessageBox::warning(this,
                                     "Ошибка сохранения акта передачи ВС на ПВП",
                                     "Не удалось сохранить форму передаточного акта бланков ВС на ПВП: \n" + fname_template +
                                     "\n\nво временную папку\n" + fname_res +
                                     "\n\nПроверьте, не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                     QMessageBox::Retry|QMessageBox::Abort,
                                     QMessageBox::Retry)==QMessageBox::Abort) {
                this->setCursor(Qt::ArrowCursor);
                db.rollback();
                return;
            }
            QFile::remove(fname_res);
        }
        if (!QFile::copy(fname_template, fname_res) || !QFile::exists(fname_res)) {
            QMessageBox::warning(this, "Ошибка копирования отчёта", "Не удалось скопировать форму передаточного акта бланков ВС на ПВП: \n" + fname_template +
                                 "\n\nво временную папку\n" + fname_res +
                                 "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            db.rollback();
            return;
        }
        com = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname_res + "\" \"" + temp_folder + "/content.xml\"";
        if (myProcess.execute(com)!=0) {
            QMessageBox::warning(this,
                                 "Ошибка обновления контента",
                                 "При добавлении нового контента в форму передаточного акта бланков ВС на ПВП произошла непредвиденная ошибка\n\n"
                                 "Опреация прервана.");
            this->setCursor(Qt::ArrowCursor);
            db.rollback();
            return;
        }

        // ===========================
        // собственно открытие шаблона
        // ===========================
        // открытие полученного ODT
        long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);

        db.commit();
        //accept();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}
