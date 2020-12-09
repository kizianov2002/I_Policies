#include "blanks_bso_wnd.h"
#include "ui_blanks_bso_wnd.h"

blanks_BSO_wnd::blanks_BSO_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent)
    : db(db), data_app(data_app), settings(settings), QDialog(parent), ui(new Ui::blanks_BSO_wnd)
{
    ui->setupUi(this);

    refresh_combo_blanks_type();

    ui->date0_2->setDate(QDate(QDate::currentDate().year(), 1, 1));
    ui->date1_2->setDate(QDate::currentDate());
    QString str;
    int days = ui->date0->date().daysTo(ui->date1->date()) +1;
    if (days==1)     str = QString::number(days) + " день";
    else if (days<5) str = QString::number(days) + " дня";
    else             str = QString::number(days) + " дней";
    ui->lab_days->setText(str);

    ui->date0->setDate(QDate(QDate::currentDate().year(), 1, 1));
    refresh_combo_pnt0();
    refresh_combo_status0();

    ui->date1->setDate(QDate::currentDate());
    refresh_combo_pnt1();
    refresh_combo_status1();
}

blanks_BSO_wnd::~blanks_BSO_wnd() {
    delete ui;
}

void blanks_BSO_wnd::refresh_combo_blanks_type() {
    ui->combo_blanks_type->clear();
    ui->combo_blanks_type->addItem("временные свидетельсва", 2);
    ui->combo_blanks_type->addItem("полисы единого образца", 3);
    ui->combo_blanks_type->setCurrentIndex(0);

    refresh_combo_blanks_act();
}

void blanks_BSO_wnd::on_combo_blanks_type_activated(int index) {
    refresh_combo_blanks_act();
}

void blanks_BSO_wnd::refresh_combo_blanks_act() {
    ui->combo_blanks_act->clear();
    ui->combo_blanks_act->addItem(" - все - ", 0);

    QString sql;
    QSqlQuery *query = new QSqlQuery(db);
    switch (ui->combo_blanks_type->currentIndex()) {
    case 0:
        sql = "select id, act_num||'  ('||act_date||')' "
              "  from blanks_vs_acts "
              " order by act_num||'  ('||act_date||')' ; ";
        query->exec(sql);
        while (query->next()) {
            ui->combo_blanks_act->addItem(query->value(1).toString(), query->value(0).toInt());
        }
        break;
    case 1:
        sql = "select id, act_num||'  ('||act_date||')' "
              "  from blanks_pol_acts "
              " order by act_num||'  ('||act_date||')' ; ";
        query->exec(sql);
        while (query->next()) {
            ui->combo_blanks_act->addItem(query->value(1).toString(), query->value(0).toInt());
        }
        break;
    default:
        break;
    }
    ui->combo_blanks_act->setCurrentIndex(0);
}

void blanks_BSO_wnd::refresh_combo_pnt0() {
    ui->combo_pnt0->clear();
    ui->combo_pnt0->addItem(" - без учёта ПВП - ", 0);
    ui->combo_pnt0->addItem(" - ещё не распределён - ", -2);
    ui->combo_pnt0->addItem(" - ещё не распределён или другой ПВП - ", -1);
    ui->combo_pnt0->addItem(" - любой ПВП - ", -3);

    QString sql;
    QSqlQuery *query = new QSqlQuery(db);
    sql = "select id, '('||point_regnum||')  '||point_name "
          "  from points "
          " order by '('||point_regnum||')  '||point_name ; ";
    query->exec(sql);
    while (query->next()) {
        ui->combo_pnt0->addItem(query->value(1).toString(), query->value(0).toInt());
    }

    ui->combo_pnt0->setCurrentIndex(-1);
}

void blanks_BSO_wnd::refresh_combo_pnt1() {
    ui->combo_pnt1->clear();
    ui->combo_pnt1->addItem(" - без учёта ПВП - ", 0);
    ui->combo_pnt1->addItem(" - ещё не распределён - ", -2);
    ui->combo_pnt1->addItem(" - ещё не распределён или другой ПВП - ", -1);
    ui->combo_pnt1->addItem(" - любой ПВП - ", -3);

    QString sql;
    QSqlQuery *query = new QSqlQuery(db);
    sql = "select id, '('||point_regnum||')  '||point_name "
          "  from points "
          " order by '('||point_regnum||')  '||point_name ; ";
    query->exec(sql);
    while (query->next()) {
        ui->combo_pnt1->addItem(query->value(1).toString(), query->value(0).toInt());
    }

    ui->combo_pnt1->setCurrentIndex(-1);
}

void blanks_BSO_wnd::refresh_combo_status0() {
    ui->combo_status0->clear();
    ui->combo_status0->addItem(" - любой статус - ", -101);
    ui->combo_status0->addItem("(0,-1)  - бланк в наличии - ", -102);
    ui->combo_status0->addItem("(+4,+1,-2,-3,-5)  - бланк использован - ", -103);
    ui->combo_status0->addItem("(+4,+1,0,-2,-3,-5)  - бланк не испорчен - ", -104);

    QString sql;
    QSqlQuery *query = new QSqlQuery(db);
    sql = "select code, '('||code||')  '||text "
          "  from spr_blank_status "
          " order by code desc ; ";
    query->exec(sql);
    while (query->next()) {
        ui->combo_status0->addItem(query->value(1).toString(), query->value(0).toInt());
    }

    ui->combo_status0->setCurrentIndex(0);
}

void blanks_BSO_wnd::refresh_combo_status1() {
    ui->combo_status1->clear();
    ui->combo_status1->addItem(" - любой статус - ", -101);
    ui->combo_status1->addItem("(0,-1)  - бланк в наличии - ", -102);
    ui->combo_status1->addItem("(+4,+1,-2,-3,-5)  - бланк использован - ", -103);
    ui->combo_status1->addItem("(+4,+1,0,-2,-3,-5)  - бланк не испорчен - ", -104);

    QString sql;
    QSqlQuery *query = new QSqlQuery(db);
    sql = "select code, '('||code||')  '||text "
          "  from spr_blank_status "
          " order by code desc ; ";
    query->exec(sql);
    while (query->next()) {
        ui->combo_status1->addItem(query->value(1).toString(), query->value(0).toInt());
    }

    ui->combo_status1->setCurrentIndex(0);
}


void blanks_BSO_wnd::on_bn_refresh_clicked() {
    refresh_blanks_tab();
}


void blanks_BSO_wnd::refresh_blanks_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    ui->tab_blanks->setEnabled(false);
    QApplication::processEvents();

    QDate date0 = QDate(2000, 1, 1);
    if (ui->ch_filter_from->isChecked())  date0 = ui->date0->date().addDays(-1);

    QDate date1 = QDate::currentDate().addYears(1);
    if (ui->ch_filter_to->isChecked())    date1 = ui->date1->date()/*.addDays(-1)*/;

    QString blanks_fields =
            "select  q.vs_num, "
            "        q.id_pnt0, '('||pnt0.point_regnum||') '||pnt0.point_name, cast('" + date0.toString("yyyy-MM-dd") + "' as date), q.st_blank0, st0.text, "
            "        q.id_pnt1, '('||pnt1.point_regnum||') '||pnt1.point_name, cast('" + date1.toString("yyyy-MM-dd") + "' as date), q.st_blank1, st1.text, "
            "        q.id_blanks_vs_act, q.act_num, q.act_date, q.act_status, case q.act_status when 0 then 'не исполнен' else '('||q.act_status||')  исполнен' end as act_status_text ";

    QString blanks_tabs =
            "  from ( select bv.vs_num, "
            "                id_blank_vs_pnt(bv.vs_num, '" + date0.toString("yyyy-MM-dd") + "') as id_pnt0, "
            "                st_blank_vs(bv.vs_num, '"     + date0.toString("yyyy-MM-dd") + "') as st_blank0, "
            "                id_blank_vs_pnt(bv.vs_num, '" + date1.toString("yyyy-MM-dd") + "') as id_pnt1, "
            "                st_blank_vs(bv.vs_num, '"     + date1.toString("yyyy-MM-dd") + "') as st_blank1, "
            "                bv.id_blanks_vs_act, a.act_num, a.act_date, a.status as act_status "
            "           from blanks_vs bv "
            "           left join blanks_vs_acts a on(a.id=bv.id_blanks_vs_act) "
            "        ) q "
            "        left join points pnt0 on(pnt0.id=q.id_pnt0) "
            "        left join points pnt1 on(pnt1.id=q.id_pnt1) "
            "        left join spr_blank_status st0 on(st0.code=q.st_blank0) "
            "        left join spr_blank_status st1 on(st1.code=q.st_blank1) ";
    if ( ui->combo_blanks_act->currentIndex()>0
         || ui->combo_pnt0->currentData().toInt()!=0
         || ui->combo_pnt1->currentData().toInt()!=0
         || ui->combo_status0->currentData().toInt()!=0
         || ui->combo_status1->currentData().toInt()!=0 ) {
        blanks_tabs +=     " where 1=1 ";

        // акт приёма-передачи банков на ПВП
        if (ui->combo_blanks_act->currentData().toInt()!=0 )
            blanks_tabs += "   and q.id_blanks_vs_act = " + QString::number(ui->combo_blanks_act->currentData().toInt()) + " ";

        if (ui->ch_filter_from->isChecked()) {
            // начальный ПВП
            if (ui->combo_pnt0->currentData().toInt()==0)
                 blanks_tabs += "";
            else if (ui->combo_pnt0->currentData().toInt()==-1)
                 blanks_tabs += "   and coalesce(q.id_pnt0,-1) <> q.id_pnt1 ";
            else if (ui->combo_pnt0->currentData().toInt()==-2)
                 blanks_tabs += "   and q.id_pnt0 is NULL ";
            else if (ui->combo_pnt0->currentData().toInt()==-3)
                 blanks_tabs += "   and q.id_pnt0 is not NULL ";
            else blanks_tabs += "   and q.id_pnt0 = " + QString::number(ui->combo_pnt0->currentData().toInt()) + " ";

            // начальный статус
            if (ui->combo_status0->currentData().toInt()==-101)
                 blanks_tabs += "";
            else if (ui->combo_status0->currentData().toInt()==-102)
                 blanks_tabs += "   and q.st_blank0 in(0, -1) ";
            else if (ui->combo_status0->currentData().toInt()==-103)
                 blanks_tabs += "   and q.st_blank0 in(1, 4, -2, -3, -5) ";
            else if (ui->combo_status0->currentData().toInt()==-104)
                 blanks_tabs += "   and q.st_blank0 in(1, 4, 0, -2, -3, -5) ";
            else blanks_tabs += "   and q.st_blank0 = " + QString::number(ui->combo_status0->currentData().toInt()) + " ";
        }

        if (ui->ch_filter_to->isChecked()) {
            // конечный ПВП
            if (ui->combo_pnt1->currentData().toInt()==0)
                 blanks_tabs += "";
            else if (ui->combo_pnt1->currentData().toInt()==-1)
                 blanks_tabs += "   and coalesce(q.id_pnt1,-1) <> q.id_pnt0 ";
            else if (ui->combo_pnt1->currentData().toInt()==-2)
                 blanks_tabs += "   and q.id_pnt1 is NULL ";
            else if (ui->combo_pnt1->currentData().toInt()==-3)
                 blanks_tabs += "   and q.id_pnt1 is not NULL ";
            else blanks_tabs += "   and q.id_pnt1 = " + QString::number(ui->combo_pnt1->currentData().toInt()) + " ";

            // конечный статус
            if (ui->combo_status1->currentData().toInt()==-101)
                 blanks_tabs += "";
            else if (ui->combo_status1->currentData().toInt()==-102)
                 blanks_tabs += "   and q.st_blank1 in(0, -1) ";
            else if (ui->combo_status1->currentData().toInt()==-103)
                 blanks_tabs += "   and q.st_blank1 in(1, 4, -2, -3, -5) ";
            else if (ui->combo_status1->currentData().toInt()==-104)
                 blanks_tabs += "   and q.st_blank1 in(1, 4, 0, -2, -3, -5) ";
            else blanks_tabs += "   and q.st_blank1 = " + QString::number(ui->combo_status1->currentData().toInt()) + " ";
        }
    }

    // подсчитаем число строк
    sql_cnt = "select count(*) as cnt "
              + blanks_tabs + " ; ";
    QSqlQuery *query_count = new QSqlQuery(db);
    bool res_count = mySQL.exec(this, sql_cnt, QString("Подсчёт числа отображаемых строк бланков"), *query_count, true, db, data_app);
    if (!res_count) {
        QMessageBox::warning(this, "Ошибка при подсчёте числа отображаемых строк бланков",
                             "При подсчёте числа отображаемых строк бланков произошла неожиданная ошибка");
        ui->lab_cnt->setText(" -//- ");
    } else {
        query_count->next();
        QString cnt = query_count->value("cnt").toString();
        ui->lab_cnt->setText(cnt);
    }
    delete query_count;

    sql_blanks = blanks_fields + blanks_tabs;
    sql_blanks += " order by  q.vs_num ";

    model_blanks.setQuery(sql_blanks,db);
    QString err = model_blanks.lastError().driverText();

    // подключаем модель из БД
    ui->tab_blanks->setModel(&model_blanks);

    // обновляем таблицу
    ui->tab_blanks->reset();

    // задаём ширину колонок
    ui->tab_blanks->setColumnWidth( 0, 70);    // q.vs_num,

    ui->tab_blanks->setColumnWidth( 1,  2);    // q.id_pnt0,
    ui->tab_blanks->setColumnWidth( 2,120);    // pnt0.point_name,
    ui->tab_blanks->setColumnWidth( 3,  2);    // cast('" + ui->date0->date().toString("yyyy-MM-dd") + "' as date),
    ui->tab_blanks->setColumnWidth( 4,  2);    // q.st_blank0,
    ui->tab_blanks->setColumnWidth( 5,180);    // st0.text,

    ui->tab_blanks->setColumnWidth( 6,  2);    // q.id_pnt1,
    ui->tab_blanks->setColumnWidth( 7,120);    // pnt1.point_name,
    ui->tab_blanks->setColumnWidth( 8,  2);    // cast('" + ui->date1->date().toString("yyyy-MM-dd") + "' as date),
    ui->tab_blanks->setColumnWidth( 9,  2);    // q.st_blank1,
    ui->tab_blanks->setColumnWidth(10,180);    // st1.text,

    ui->tab_blanks->setColumnWidth(11,  2);    // q.id_blanks_vs_act,
    ui->tab_blanks->setColumnWidth(12,120);    // q.act_num,
    ui->tab_blanks->setColumnWidth(13, 75);    // q.act_date,
    ui->tab_blanks->setColumnWidth(14,  2);    // q.act_status
    ui->tab_blanks->setColumnWidth(15, 85);    // q.act_status_text
    // правим заголовки
    model_blanks.setHeaderData( 0, Qt::Horizontal, ("ВС"));

    model_blanks.setHeaderData( 2, Qt::Horizontal, ("начальный ПВП"));
    model_blanks.setHeaderData( 3, Qt::Horizontal, ("на дату"));
    model_blanks.setHeaderData( 5, Qt::Horizontal, ("начальный статус бланка"));

    model_blanks.setHeaderData( 7, Qt::Horizontal, ("конечный ПВП"));
    model_blanks.setHeaderData( 8, Qt::Horizontal, ("статус бланка"));
    model_blanks.setHeaderData(10, Qt::Horizontal, ("на дату"));

    model_blanks.setHeaderData(12, Qt::Horizontal, ("акт"));
    model_blanks.setHeaderData(13, Qt::Horizontal, ("дата акта"));
    model_blanks.setHeaderData(15, Qt::Horizontal, ("статус акта"));
    ui->tab_blanks->repaint();

    ui->tab_blanks->setEnabled(true);
    this->setCursor(Qt::ArrowCursor);
}

void blanks_BSO_wnd::on_date0_editingFinished() {
    ui->tab_blanks->setEnabled(false);
    if (ui->date0->date()>ui->date1->date())
        ui->date1->setDate(ui->date0->date());
}

void blanks_BSO_wnd::on_date1_editingFinished() {
    ui->tab_blanks->setEnabled(false);
    if (ui->date0->date()>ui->date1->date())
        ui->date0->setDate(ui->date1->date());
}



void blanks_BSO_wnd::on_date0_2_editingFinished() {
    ui->tab_blanks->setEnabled(false);
    if (ui->date0_2->date()>ui->date1_2->date())
        ui->date1_2->setDate(ui->date0_2->date());
    QString str;
    int days = ui->date0_2->date().daysTo(ui->date1_2->date()) +1;
    if (days==1)     str = QString::number(days) + " день";
    else if (days<5) str = QString::number(days) + " дня";
    else             str = QString::number(days) + " дней";
    ui->lab_days->setText(str);
    ui->date0->setDate(ui->date0_2->date());
    QApplication::processEvents();
}

void blanks_BSO_wnd::on_date1_2_editingFinished() {
    ui->tab_blanks->setEnabled(false);
    if (ui->date0_2->date()>ui->date1_2->date())
        ui->date0_2->setDate(ui->date1_2->date());
    QString str;
    int days = ui->date0_2->date().daysTo(ui->date1_2->date()) +1;
    if (days==1)     str = QString::number(days) + " день";
    else if (days<5) str = QString::number(days) + " дня";
    else             str = QString::number(days) + " дней";
    ui->lab_days->setText(str);
    ui->date1->setDate(ui->date1_2->date());
    QApplication::processEvents();
}

void blanks_BSO_wnd::on_combo_blanks_act_activated(int index)
{   ui->tab_blanks->setEnabled(false);   }
void blanks_BSO_wnd::on_combo_pnt0_activated(int index)
{   ui->tab_blanks->setEnabled(false);   }
void blanks_BSO_wnd::on_combo_pnt1_activated(int index)
{   ui->tab_blanks->setEnabled(false);   }
void blanks_BSO_wnd::on_combo_status0_activated(int index)
{   ui->tab_blanks->setEnabled(false);   }
void blanks_BSO_wnd::on_combo_status1_activated(int index)
{   ui->tab_blanks->setEnabled(false);   }

void blanks_BSO_wnd::on_ch_filter_from_clicked(bool checked) {
    ui->date0->setEnabled(checked);
    ui->combo_pnt0->setEnabled(checked);
    ui->combo_status0->setEnabled(checked);
}
void blanks_BSO_wnd::on_ch_filter_to_clicked(bool checked) {
    ui->date1->setEnabled(checked);
    ui->combo_pnt1->setEnabled(checked);
    ui->combo_status1->setEnabled(checked);
}


// ---------------------------------------------
// автофильтры
void blanks_BSO_wnd::on_bn_from_clicked() {
    ui->combo_blanks_type->setCurrentIndex(0);
    ui->combo_blanks_act->setCurrentIndex(0);

    ui->ch_filter_from->setChecked(false);
    ui->date0->setEnabled(false);
    ui->date0->setDate(ui->date0_2->date().addDays(-1));
    ui->combo_pnt0->setEnabled(false);
    ui->combo_pnt0->setCurrentIndex(0);
    ui->combo_status0->setEnabled(false);
    ui->combo_status0->setCurrentIndex(0);

    ui->ch_filter_to->setChecked(true);
    ui->date1->setEnabled(true);
    ui->date1->setDate(ui->date0_2->date().addDays(-1));
    ui->combo_pnt1->setEnabled(true);
    int pnt1 = ui->combo_pnt1->currentData().toInt();
    if (pnt1<=0)
        ui->combo_pnt1->setCurrentIndex(ui->combo_pnt1->findData(-3));
    ui->combo_status1->setEnabled(true);
    ui->combo_status1->setCurrentIndex(ui->combo_status1->findData(-102));

    QFont font_l = ui->label->font(), font_b = ui->label->font();
    font_l.setWeight(25);
    font_b.setWeight(75);
    ui->bn_from    ->setFont(font_b);
    ui->bn_from_sp ->setFont(font_l);
    ui->bn_add     ->setFont(font_l);
    ui->bn_add_sp  ->setFont(font_l);
    ui->bn_dec     ->setFont(font_l);
    ui->bn_dec_sp  ->setFont(font_l);
    ui->bn_to      ->setFont(font_l);
    ui->bn_to_sp   ->setFont(font_l);

    refresh_blanks_tab();
}

void blanks_BSO_wnd::on_bn_from_sp_clicked() {
    ui->combo_blanks_type->setCurrentIndex(0);
    ui->combo_blanks_act->setCurrentIndex(0);

    ui->ch_filter_from->setChecked(false);
    ui->date0->setEnabled(false);
    ui->date0->setDate(ui->date0_2->date().addDays(-1));
    ui->combo_pnt0->setEnabled(false);
    ui->combo_pnt0->setCurrentIndex(0);
    ui->combo_status0->setEnabled(false);
    ui->combo_status0->setCurrentIndex(0);

    ui->ch_filter_to->setChecked(true);
    ui->date1->setEnabled(true);
    ui->date1->setDate(ui->date0_2->date().addDays(-1));
    ui->combo_pnt1->setEnabled(true);
    int pnt1 = ui->combo_pnt1->currentData().toInt();
    if (pnt1<=0)
        ui->combo_pnt1->setCurrentIndex(ui->combo_pnt1->findData(-3));
    ui->combo_status1->setEnabled(true);
    ui->combo_status1->setCurrentIndex(ui->combo_status1->findData(-1));

    QFont font_l = ui->label->font(), font_b = ui->label->font();
    font_l.setWeight(25);
    font_b.setWeight(75);
    ui->bn_from    ->setFont(font_l);
    ui->bn_from_sp ->setFont(font_b);
    ui->bn_add     ->setFont(font_l);
    ui->bn_add_sp  ->setFont(font_l);
    ui->bn_dec     ->setFont(font_l);
    ui->bn_dec_sp  ->setFont(font_l);
    ui->bn_to      ->setFont(font_l);
    ui->bn_to_sp   ->setFont(font_l);

    refresh_blanks_tab();
}

void blanks_BSO_wnd::on_bn_add_clicked() {
    ui->combo_blanks_type->setCurrentIndex(0);
    ui->combo_blanks_act->setCurrentIndex(0);

    ui->ch_filter_from->setChecked(true);
    ui->date0->setEnabled(true);
    ui->date0->setDate(ui->date0_2->date());
    ui->combo_pnt0->setEnabled(true);
    ui->combo_pnt0->setCurrentIndex(ui->combo_pnt0->findData(-1));
    ui->combo_status0->setEnabled(true);
    ui->combo_status0->setCurrentIndex(ui->combo_status0->findData(-101));

    ui->ch_filter_to->setChecked(true);
    ui->date1->setEnabled(true);
    ui->date1->setDate(ui->date1_2->date());
    ui->combo_pnt1->setEnabled(true);
    int pnt1 = ui->combo_pnt1->currentData().toInt();
    if (pnt1<=0)
        ui->combo_pnt1->setCurrentIndex(ui->combo_pnt1->findData(-3));
    ui->combo_status1->setEnabled(true);
    ui->combo_status1->setCurrentIndex(ui->combo_status1->findData(-101));

    QFont font_l = ui->label->font(), font_b = ui->label->font();
    font_l.setWeight(25);
    font_b.setWeight(75);
    ui->bn_from    ->setFont(font_l);
    ui->bn_from_sp ->setFont(font_l);
    ui->bn_add     ->setFont(font_b);
    ui->bn_add_sp  ->setFont(font_l);
    ui->bn_dec     ->setFont(font_l);
    ui->bn_dec_sp  ->setFont(font_l);
    ui->bn_to      ->setFont(font_l);
    ui->bn_to_sp   ->setFont(font_l);

    refresh_blanks_tab();
}

void blanks_BSO_wnd::on_bn_add_sp_clicked() {
    ui->combo_blanks_type->setCurrentIndex(0);
    ui->combo_blanks_act->setCurrentIndex(0);

    ui->ch_filter_from->setChecked(true);
    ui->date0->setEnabled(true);
    ui->date0->setDate(ui->date0_2->date());
    ui->combo_pnt0->setEnabled(true);
    ui->combo_pnt0->setCurrentIndex(ui->combo_pnt0->findData(0));
    ui->combo_status0->setEnabled(true);
    ui->combo_status0->setCurrentIndex(ui->combo_status0->findData(-104));

    ui->ch_filter_to->setChecked(true);
    ui->date1->setEnabled(true);
    ui->date1->setDate(ui->date1_2->date());
    ui->combo_pnt1->setEnabled(true);
    int pnt1 = ui->combo_pnt1->currentData().toInt();
    if (pnt1<=0)
        ui->combo_pnt1->setCurrentIndex(ui->combo_pnt1->findData(-3));
    ui->combo_status1->setEnabled(true);
    ui->combo_status1->setCurrentIndex(ui->combo_status1->findData(-1));

    QFont font_l = ui->label->font(), font_b = ui->label->font();
    font_l.setWeight(25);
    font_b.setWeight(75);
    ui->bn_from    ->setFont(font_l);
    ui->bn_from_sp ->setFont(font_l);
    ui->bn_add     ->setFont(font_l);
    ui->bn_add_sp  ->setFont(font_b);
    ui->bn_dec     ->setFont(font_l);
    ui->bn_dec_sp  ->setFont(font_l);
    ui->bn_to      ->setFont(font_l);
    ui->bn_to_sp   ->setFont(font_l);

    refresh_blanks_tab();
}

void blanks_BSO_wnd::on_bn_dec_clicked() {
    ui->combo_blanks_type->setCurrentIndex(0);
    ui->combo_blanks_act->setCurrentIndex(0);

    ui->ch_filter_from->setChecked(true);
    ui->date0->setEnabled(true);
    ui->date0->setDate(ui->date0_2->date());
    ui->combo_pnt0->setEnabled(true);
    ui->combo_pnt0->setCurrentIndex(0);
    ui->combo_status0->setEnabled(true);
    ui->combo_status0->setCurrentIndex(ui->combo_status1->findData(-102));

    ui->ch_filter_to->setChecked(true);
    ui->date1->setEnabled(true);
    ui->date1->setDate(ui->date1_2->date());
    ui->combo_pnt1->setEnabled(true);
    int pnt1 = ui->combo_pnt1->currentData().toInt();
    if (pnt1<=0)
        ui->combo_pnt1->setCurrentIndex(ui->combo_pnt1->findData(-3));
    ui->combo_status1->setEnabled(true);
    ui->combo_status1->setCurrentIndex(ui->combo_status1->findData(-103));

    QFont font_l = ui->label->font(), font_b = ui->label->font();
    font_l.setWeight(25);
    font_b.setWeight(75);
    ui->bn_from    ->setFont(font_l);
    ui->bn_from_sp ->setFont(font_l);
    ui->bn_add     ->setFont(font_l);
    ui->bn_add_sp  ->setFont(font_l);
    ui->bn_dec     ->setFont(font_b);
    ui->bn_dec_sp  ->setFont(font_l);
    ui->bn_to      ->setFont(font_l);
    ui->bn_to_sp   ->setFont(font_l);

    refresh_blanks_tab();
}

void blanks_BSO_wnd::on_bn_dec_sp_clicked() {
    ui->combo_blanks_type->setCurrentIndex(0);
    ui->combo_blanks_act->setCurrentIndex(0);

    ui->ch_filter_from->setChecked(true);
    ui->date0->setEnabled(true);
    ui->date0->setDate(ui->date0_2->date());
    ui->combo_pnt0->setEnabled(true);
    int pnt0 = ui->combo_pnt0->currentData().toInt();
    if (pnt0<=0)
        ui->combo_pnt0->setCurrentIndex(ui->combo_pnt0->findData(-3));
    ui->combo_status0->setEnabled(true);
    ui->combo_status0->setCurrentIndex(ui->combo_status0->findData(-1));

    ui->ch_filter_to->setChecked(true);
    ui->date1->setEnabled(true);
    ui->date1->setDate(ui->date1_2->date());
    ui->combo_pnt1->setEnabled(true);
    ui->combo_pnt1->setCurrentIndex(ui->combo_pnt1->findData(-1));
    ui->combo_status1->setEnabled(true);
    ui->combo_status1->setCurrentIndex(ui->combo_status1->findData(-101));


    QFont font_l = ui->label->font(), font_b = ui->label->font();
    font_l.setWeight(25);
    font_b.setWeight(75);
    ui->bn_from    ->setFont(font_l);
    ui->bn_from_sp ->setFont(font_l);
    ui->bn_add     ->setFont(font_l);
    ui->bn_add_sp  ->setFont(font_b);
    ui->bn_dec     ->setFont(font_l);
    ui->bn_dec_sp  ->setFont(font_l);
    ui->bn_to      ->setFont(font_l);
    ui->bn_to_sp   ->setFont(font_l);

    refresh_blanks_tab();
}

void blanks_BSO_wnd::on_bn_to_clicked() {
    ui->combo_blanks_type->setCurrentIndex(0);
    ui->combo_blanks_act->setCurrentIndex(0);

    ui->ch_filter_from->setChecked(false);
    ui->date0->setEnabled(false);
    ui->date0->setDate(ui->date1_2->date());
    ui->combo_pnt0->setEnabled(false);
    ui->combo_pnt0->setCurrentIndex(0);
    ui->combo_status0->setEnabled(false);
    ui->combo_status0->setCurrentIndex(0);

    ui->ch_filter_to->setChecked(true);
    ui->date1->setEnabled(true);
    ui->date1->setDate(ui->date1_2->date());
    ui->combo_pnt1->setEnabled(true);
    int pnt1 = ui->combo_pnt1->currentData().toInt();
    if (pnt1<=0)
        ui->combo_pnt1->setCurrentIndex(ui->combo_pnt1->findData(-3));
    ui->combo_status1->setEnabled(true);
    ui->combo_status1->setCurrentIndex(ui->combo_status1->findData(-102));

    QFont font_l = ui->label->font(), font_b = ui->label->font();
    font_l.setWeight(25);
    font_b.setWeight(75);
    ui->bn_from    ->setFont(font_l);
    ui->bn_from_sp ->setFont(font_l);
    ui->bn_add     ->setFont(font_l);
    ui->bn_add_sp  ->setFont(font_l);
    ui->bn_dec     ->setFont(font_l);
    ui->bn_dec_sp  ->setFont(font_l);
    ui->bn_to      ->setFont(font_b);
    ui->bn_to_sp   ->setFont(font_l);

    refresh_blanks_tab();
}

void blanks_BSO_wnd::on_bn_to_sp_clicked() {
    ui->combo_blanks_type->setCurrentIndex(0);
    ui->combo_blanks_act->setCurrentIndex(0);

    ui->ch_filter_from->setChecked(false);
    ui->date0->setEnabled(false);
    ui->date0->setDate(ui->date1_2->date());
    ui->combo_pnt0->setEnabled(false);
    ui->combo_pnt0->setCurrentIndex(0);
    ui->combo_status0->setEnabled(false);
    ui->combo_status0->setCurrentIndex(0);

    ui->ch_filter_to->setChecked(true);
    ui->date1->setEnabled(true);
    ui->date1->setDate(ui->date1_2->date());
    ui->combo_pnt1->setEnabled(true);
    int pnt1 = ui->combo_pnt1->currentData().toInt();
    if (pnt1<=0)
        ui->combo_pnt1->setCurrentIndex(ui->combo_pnt1->findData(-3));
    ui->combo_status1->setEnabled(true);
    ui->combo_status1->setCurrentIndex(ui->combo_status1->findData(-1));

    QFont font_l = ui->label->font(), font_b = ui->label->font();
    font_l.setWeight(25);
    font_b.setWeight(75);
    ui->bn_from    ->setFont(font_l);
    ui->bn_from_sp ->setFont(font_l);
    ui->bn_add     ->setFont(font_l);
    ui->bn_add_sp  ->setFont(font_l);
    ui->bn_dec     ->setFont(font_l);
    ui->bn_dec_sp  ->setFont(font_l);
    ui->bn_to      ->setFont(font_l);
    ui->bn_to_sp   ->setFont(font_b);

    refresh_blanks_tab();
}

void blanks_BSO_wnd::on_bn_toCSV_clicked() {
    bool res = mySQL.tab_to_CSV(model_blanks, ui->tab_blanks);
}
