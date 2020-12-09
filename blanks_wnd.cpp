#include "blanks_wnd.h"
#include "ui_blanks_wnd.h"

blanks_wnd::blanks_wnd(QSqlDatabase &db, s_data_blank &data_blank, s_data_app &data_app, QSettings &settings, QWidget *parent)
    : db(db), data_blank(data_blank), data_app(data_app), settings(settings), QDialog(parent), ui(new Ui::blanks_wnd)
{
    ui->setupUi(this);

    barcode_w = NULL;
    add_blank_scan_w = NULL;
    add_blank_w = NULL;

    ui->lab_blank_vs_point->setText(data_app.point_name);
    ui->lab_blank_vs_operator->setText(data_app.operator_fio);
    ui->lab_blank_pol_point->setText(data_app.point_name);
    ui->lab_blank_pol_operator->setText(data_app.operator_fio);

    id_point_vs = -1;
    id_operator_vs = -1;
    id_point_pol = -1;
    id_operator_pol = -1;

    ui->bn_scan_blanks->setEnabled(!data_app.is_locl);
    ui->bn_add_blank->setEnabled(!data_app.is_locl);

    // заполним значения по умолчанию
    ui->ln_fam->setText(data_blank.fam);
    ui->ln_im->setText(data_blank.im);
    ui->ln_ot->setText(data_blank.ot);
    ui->ln_filter_fam->setText(data_blank.fam);
    ui->ln_filter_im->setText(data_blank.im);
    ui->ln_filter_ot->setText(data_blank.ot);
    ui->date_birth->setDate(data_blank.date_birth);
    ui->ln_snils->setText(data_blank.snils);
    ui->ln_vs_num->setText(data_blank.vs_num);
    ui->ln_pol_ser->setText(data_blank.pol_ser);
    ui->ln_pol_num->setText(data_blank.pol_num);
    ui->ln_pol_enp->setText(data_blank.enp);

    ui->rb_person->setChecked(!data_blank.fam.isEmpty());
    on_rb_person_clicked(!data_blank.fam.isEmpty());

    refresh_persons();

    refresh_blanks_points();
    refresh_blanks_statuses();
    refresh_polis_forms();
    refresh_blanks_vs_tab();
    refresh_blanks_pol_tab();

    // редактирование бланков доступно технику и в головном офисе
    ui->bns_blanks_vs->setEnabled(data_app.is_tech || data_app.is_head);
    ui->bns_blanks_pol->setEnabled(data_app.is_tech || data_app.is_head);
}

blanks_wnd::~blanks_wnd() {
    delete ui;
}

void blanks_wnd::on_bn_close_clicked() {
    close();
}

void blanks_wnd::refresh_blanks_statuses() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка категорий ПВП
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, text "
                  "  from public.spr_blank_status s "
                  " order by s.code ; ";
    mySQL.exec(this, sql, QString("статусы бланков"), *query, true, db, data_app);
    ui->combo_filter_vs_status->clear();
    ui->combo_filter_pol_status->clear();
    ui->combo_blank_vs_status->clear();
    ui->combo_blank_pol_status->clear();
    ui->combo_filter_vs_status->addItem(" - все - ", -999);
    ui->combo_filter_pol_status->addItem(" - все - ", -999);
    while (query->next()) {
        int n = query->value(0).toInt();
        ui->combo_blank_vs_status->addItem("(" + QString::number(n) + ") " + query->value(1).toString(), n);
        ui->combo_blank_pol_status->addItem("(" + QString::number(n) + ") " + query->value(1).toString(), n);
        ui->combo_filter_vs_status->addItem("(" + QString::number(n) + ") " + query->value(1).toString(), n);
        ui->combo_filter_pol_status->addItem("(" + QString::number(n) + ") " + query->value(1).toString(), n);
    }
    ui->combo_blank_vs_status->setCurrentIndex(ui->combo_blank_vs_status->findData(0));
    ui->combo_blank_pol_status->setCurrentIndex(ui->combo_blank_pol_status->findData(0));
    ui->combo_filter_vs_status->setCurrentIndex(ui->combo_filter_vs_status->findData(-999));
    ui->combo_filter_pol_status->setCurrentIndex(ui->combo_filter_pol_status->findData(-999));
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void blanks_wnd::refresh_blanks_points() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка категорий ПВП
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select id, '('||point_regnum||')  '||point_name as point_name "
                  "  from points p "
                  " where p.status>0 "
                  " order by point_code ; ";
    mySQL.exec(this, sql, QString("статусы бланков"), *query, true, db, data_app);
    ui->combo_filter_vs_point->clear();
    ui->combo_filter_pol_point->clear();
    ui->combo_filter_vs_point->addItem(" - все ПВП - ", -999);
    ui->combo_filter_pol_point->addItem(" - все ПВП - ", -999);
    while (query->next()) {
        ui->combo_filter_vs_point->addItem(query->value(1).toString(), query->value(0).toInt());
        ui->combo_filter_pol_point->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_filter_vs_point->setCurrentIndex(ui->combo_filter_vs_point->findData(-999));
    ui->combo_filter_pol_point->setCurrentIndex(ui->combo_filter_pol_point->findData(-999));
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void blanks_wnd::refresh_polis_forms() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка форм выпуска полисов
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, text "
                  " from public.spr_fpolis s "
                  " where s.code>0 "
                  " order by s.code ; ";
    mySQL.exec(this, sql, QString("статусы бланков"), *query, true, db, data_app);
    ui->combo_blank_pol_form->clear();
    while (query->next()) {
        int n = query->value(0).toInt();
        ui->combo_blank_pol_form->addItem("(" + QString::number(n) + ") " + query->value(1).toString(), n);
    }
    ui->combo_blank_pol_form->setCurrentIndex(ui->combo_blank_pol_status->findData(0));
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void blanks_wnd::refresh_persons() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка застрахованных
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select id, COALESCE(fam,'-')||' '||COALESCE(im,'-')||' '||COALESCE(ot,'-')||', '||date_birth || '  (ID=' || id || ')' as pers_fio "
                  "  from persons s "
                  " where COALESCE(fam,'') like ('" + ui->ln_filter_fam->text().trimmed().toUpper() + "%') "
                  "   and COALESCE(im,'')  like ('" + ui->ln_filter_im->text().trimmed().toUpper()  + "%') "
                  "   and COALESCE(ot,'')  like ('" + ui->ln_filter_ot->text().trimmed().toUpper()  + "%') "
                  //"   and s.status>-100 "
                  " order by COALESCE(fam,'-')||' '||COALESCE(im,'-')||' '||COALESCE(ot,'-')||', '||date_birth || '  (ID=' || id || ')' ; ";
    mySQL.exec(this, sql, QString("список персон"), *query, true, db, data_app);
    ui->combo_persons->clear();
    persons_list.clear();
    while (query->next()) {
        int id = query->value(0).toInt();
        ui->combo_persons->addItem(query->value(1).toString(), id);
        persons_list.append(query->value(1).toString());
    }
    ui->combo_persons->setCurrentIndex(0);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}


// -----------------------------------------------------------
// бланки ВС
// -----------------------------------------------------------
void blanks_wnd::on_bn_refresh_blanks_vs_clicked() {
    refresh_blanks_vs_tab();
}

void blanks_wnd::refresh_blanks_vs_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    ui->tab_blanks_vs->setEnabled(false);
    QApplication::processEvents();

    // данные персоны
    ui->ln_blank_num_vs->clear();
    ui->lab_blank_vs_point->setText(data_app.point_name);
    ui->lab_blank_vs_operator->setText(data_app.operator_fio);
    ui->date_blank_vs_add->setDate(QDate::currentDate());
    ui->date_blank_vs_spent->setDate(QDate::fromString("1900-01-01", "yyyy-MM-dd"));
    ui->date_blank_vs_waste->setDate(QDate::fromString("1900-01-01", "yyyy-MM-dd"));
    ui->combo_blank_vs_status->setCurrentIndex(ui->combo_blank_vs_status->findData(0));

    QString blanks_vs_fields =
            "select b.vs_num, b.date_add, b.status, '('||b.status||') '||s.text as status_ext, p.id as id_point, "
            "       '('||p.point_regnum||') '||p.point_name as point_name, o.id as id_operator, o.oper_fio, "
            "       pol.date_begin, pol.date_end, pol.date_stop, in_erp, pol.order_num, pol.order_date, "
            "       e.fam, e.im, e.ot, e.sex, e.date_birth, e.bomj, e.snils, e.phone_cell, "
            "       b.date_spent, b.date_waste ";

    QString blanks_vs_tabs =
            "  from blanks_vs b "
            "       left join points p on(p.id=b.id_point) "
            "       left join operators o on(o.id=b.id_operator) "
            "       left join spr_blank_status s on(s.code=b.status) "
            "       left join polises pol on(pol.vs_num=b.vs_num) "
            "       left join persons e on(e.id=pol.id_person) ";
    bool f_wh = false;
    if (ui->combo_filter_vs_status->currentData().toInt()!=-999) {
        blanks_vs_tabs += " where b.status=" + QString::number(ui->combo_filter_vs_status->currentData().toInt());
        f_wh = true;
    }
    //фамилия
    if (ui->rb_person->isChecked() && !(ui->ln_fam->text().trimmed().isEmpty())) {
        if (!f_wh) { blanks_vs_tabs += " where "; f_wh = true; }
        else { blanks_vs_tabs += " and "; }
        blanks_vs_tabs += " e.fam like('" + ui->ln_fam->text().trimmed().toUpper() + "%') ";
    }
    // имя
    if (ui->rb_person->isChecked() && !(ui->ln_im->text().trimmed().isEmpty())) {
        if (!f_wh) { blanks_vs_tabs += " where "; f_wh = true; }
        else { blanks_vs_tabs += " and "; }
        blanks_vs_tabs += " e.im like('" + ui->ln_im->text().trimmed().toUpper() + "%') ";
    }
    // отество
    if (ui->rb_person->isChecked() && !(ui->ln_ot->text().trimmed().isEmpty())) {
        if (!f_wh) { blanks_vs_tabs += " where "; f_wh = true; }
        else { blanks_vs_tabs += " and "; }
        blanks_vs_tabs += " e.ot like('" + ui->ln_ot->text().trimmed().toUpper() + "%') ";
    }
    // дата рождения
    if (ui->rb_person->isChecked() && ui->ch_date_birth->isChecked()) {
        if (!f_wh) { blanks_vs_tabs += " where "; f_wh = true; }
        else { blanks_vs_tabs += " and "; }
        blanks_vs_tabs += " e.date_birth='" + ui->date_birth->date().toString("yyyy-MM-dd") + "' ";
    }
    // СНИЛС
    if (ui->rb_person->isChecked() && !(ui->ln_snils->text().trimmed().isEmpty())) {
        if (!f_wh) { blanks_vs_tabs += " where "; f_wh = true; }
        else { blanks_vs_tabs += " and "; }
        blanks_vs_tabs += " e.snils like('" + ui->ln_snils->text().trimmed().toUpper() + "%') ";
    }
    // ВС
    if (ui->rb_vs->isChecked() && !(ui->ln_vs_num->text().trimmed().isEmpty())) {
        if (!f_wh) { blanks_vs_tabs += " where "; f_wh = true; }
        else { blanks_vs_tabs += " and "; }
        if (ui->ch_vs_num_2->isChecked()) {
            blanks_vs_tabs += " (b.vs_num>=" + ui->ln_vs_num->text().trimmed() + " and b.vs_num<=" + ui->ln_vs_num_2->text().trimmed() + ") ";
        } else {
            blanks_vs_tabs += " cast(b.vs_num as text) like('" + ui->ln_vs_num->text().trimmed().toUpper() + "%') ";
        }
    }
    // серия полиса
    if (ui->rb_pol->isChecked() && !(ui->ln_pol_ser->text().trimmed().isEmpty())) {
        if (!f_wh) { blanks_vs_tabs += " where "; f_wh = true; }
        else { blanks_vs_tabs += " and "; }
        blanks_vs_tabs += " pol.pol_ser like('" + ui->ln_pol_ser->text().trimmed().toUpper() + "%') ";
    }
    // номер полиса
    if (ui->rb_pol->isChecked() && !(ui->ln_pol_num->text().trimmed().isEmpty())) {
        if (!f_wh) { blanks_vs_tabs += " where "; f_wh = true; }
        else { blanks_vs_tabs += " and "; }
        blanks_vs_tabs += " pol.pol_num like('" + ui->ln_pol_num->text().trimmed().toUpper() + "%') ";
    }
    // ЕНП
    if (ui->rb_pol->isChecked() && !(ui->ln_pol_enp->text().trimmed().isEmpty())) {
        if (!f_wh) { blanks_vs_tabs += " where "; f_wh = true; }
        else { blanks_vs_tabs += " and "; }
        blanks_vs_tabs += " pol.enp like('" + ui->ln_pol_enp->text().trimmed().toUpper() + "%') ";
    }
    // ПВП
    if (ui->combo_filter_vs_point->currentData().toInt()>0) {
        if (!f_wh) { blanks_vs_tabs += " where "; f_wh = true; }
        else { blanks_vs_tabs += " and "; }
        blanks_vs_tabs += " b.id_point = " + QString::number(ui->combo_filter_vs_point->currentData().toInt()) + " ";
    }


    // подсчитаем число строк
    QString sql_cnt = "select count(*) as cnt "
                      " " + blanks_vs_tabs + " ; ";
    QSqlQuery *query_count = new QSqlQuery(db);
    bool res_count = mySQL.exec(this, sql_cnt, QString("Подсчёт числа отображаемых строк бланков"), *query_count, true, db, data_app);
    if (!res_count) {
        QMessageBox::warning(this, "Ошибка при подсчёте числа отображаемых строк бланков", "При подсчёте числа отображаемых строк бланков произошла неожиданная ошибка");
        ui->lab_vs_cnt->setText(" -//- ");
    } else {
        query_count->next();
        QString cnt = query_count->value("cnt").toString();
        ui->lab_vs_cnt->setText(cnt);
    }
    delete query_count;


    QString blanks_vs_sql = blanks_vs_fields + blanks_vs_tabs;
    blanks_vs_sql += " order by  b.vs_num ";

    model_blanks_vs.setQuery(blanks_vs_sql,db);
    QString err = model_blanks_vs.lastError().driverText();

    // подключаем модель из БД
    ui->tab_blanks_vs->setModel(&model_blanks_vs);

    // обновляем таблицу
    ui->tab_blanks_vs->reset();

    // задаём ширину колонок
    ui->tab_blanks_vs->setColumnWidth( 0, 80);    // b.vs_num
    ui->tab_blanks_vs->setColumnWidth( 1, 80);    // b.date_add
    ui->tab_blanks_vs->setColumnWidth( 2,  1);    // b.status
    ui->tab_blanks_vs->setColumnWidth( 3,120);    // s.text as status_ext
    ui->tab_blanks_vs->setColumnWidth( 4,  1);    // id_point
    ui->tab_blanks_vs->setColumnWidth( 5,150);    // p.point_name
    ui->tab_blanks_vs->setColumnWidth( 6,  1);    // id_operator
    ui->tab_blanks_vs->setColumnWidth( 7,110);    // o.oper_fio

    // правим заголовки
    model_blanks_vs.setHeaderData( 0, Qt::Horizontal, ("№ ВС"));
    model_blanks_vs.setHeaderData( 1, Qt::Horizontal, ("дата"));
    model_blanks_vs.setHeaderData( 3, Qt::Horizontal, ("статус бланка"));
    model_blanks_vs.setHeaderData( 5, Qt::Horizontal, ("ПВП"));
    model_blanks_vs.setHeaderData( 7, Qt::Horizontal, ("оператор"));
    ui->tab_blanks_vs->repaint();

    ui->tab_blanks_vs->setEnabled(true);
    this->setCursor(Qt::ArrowCursor);
}

void blanks_wnd::on_combo_filter_vs_status_currentIndexChanged(int index) {
    refresh_blanks_vs_tab();
}

void blanks_wnd::on_bn_add_blank_vs_clicked() {
    /*// Проверим заполнение полей
    if (ui->spin_blank_num_vs->value()<1) {
        QMessageBox::warning(this, "Не достаточно данных!", "Номер ВС не может быть меньше 1.");
        return;
    }
    if (ui->combo_blank_vs_status->currentData().toInt()==0) {
        QMessageBox::warning(this, "Не достаточно данных!", "Выберите статус бланка ВС.");
        return;
    }
    // проверим номер ВС на уникальность
    QString sql_test = "select count(*) from blanks_vs where vs_num=" +QString::number(ui->spin_blank_num_vs->value())+ " ; ";
    QSqlQuery *query_test = new QSqlQuery(db);
    mySQL.exec(this, sql_test, "Проверка номера ВС на уникальность", *query_test, true, db, data_app);
    if (query_test->next() && query_test->value(0).toInt()>0) {
        QMessageBox::warning(this, "Номер ВС уже занят", "Введённый номер ВС уже использован в другом полисе.\n\nоперация отменена.");
        delete query_test;
        return;
    }
    delete query_test;

    // собственно сохранение
    QString sql = "insert into blanks_vs(vs_num, id_point, id_operator, date_add, status) ";
    sql += " values (";
    sql += " "  + QString::number(ui->spin_blank_num_vs->value()) + ", ";
    sql += " "  + QString::number(data_app.id_point) + ", ";
    sql += " "  + QString::number(data_app.id_operator) + ", ";
    sql += " '" + ui->date_blank_vs_add->date().toString("yyyy-MM-dd") + "', ";
    sql += " "  + QString::number(ui->combo_blank_vs_status->currentData().toInt()) + ") ; ";

    QSqlQuery *query = new QSqlQuery(db);
    mySQL.exec(this, sql, "Добавление записи бланка ВС", *query, true, db, data_app);
    delete query;*/

    refresh_blanks_vs_tab();
}

void blanks_wnd::on_combo_filter_vs_point_currentIndexChanged(int index) {
    refresh_blanks_vs_tab();
}

void blanks_wnd::on_tab_blanks_vs_clicked(const QModelIndex &index) {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_blanks_vs || !ui->tab_blanks_vs->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить].");
        return;
    }
    this->setCursor(Qt::WaitCursor);

    if (!ui->rb_vs->isChecked())
        ui->ln_vs_num->setText(QString::number(model_blanks_vs.data(model_blanks_vs.index(index.row(), 0), Qt::EditRole).toInt()));

    ui->ln_blank_num_vs->setText (QString::number(model_blanks_vs.data(model_blanks_vs.index(index.row(), 0), Qt::EditRole).toInt()));
    ui->lab_blank_vs_point->setText (model_blanks_vs.data(model_blanks_vs.index(index.row(), 5), Qt::EditRole).toString());
    ui->lab_blank_vs_operator->setText (model_blanks_vs.data(model_blanks_vs.index(index.row(), 7), Qt::EditRole).toString());
    ui->date_blank_vs_add->setDate (model_blanks_vs.data(model_blanks_vs.index(index.row(), 1), Qt::EditRole).toDate());
    ui->date_blank_vs_spent->setDate (QDate(1900,1,1));
    ui->date_blank_vs_spent->setDate (model_blanks_vs.data(model_blanks_vs.index(index.row(),22), Qt::EditRole).toDate());
    ui->date_blank_vs_spent->setVisible (!model_blanks_vs.data(model_blanks_vs.index(index.row(),22), Qt::EditRole).isNull());
    ui->date_blank_vs_waste->setDate (QDate(1900,1,1));
    ui->date_blank_vs_waste->setDate (model_blanks_vs.data(model_blanks_vs.index(index.row(),23), Qt::EditRole).toDate());
    ui->date_blank_vs_waste->setVisible (!model_blanks_vs.data(model_blanks_vs.index(index.row(),23), Qt::EditRole).isNull());
    ui->combo_blank_vs_status->setCurrentIndex (ui->combo_blank_vs_status->findData(model_blanks_vs.data(model_blanks_vs.index(index.row(), 2), Qt::EditRole).toInt()));

    id_point_vs = model_blanks_vs.data(model_blanks_vs.index(index.row(), 4), Qt::EditRole).toInt();
    id_operator_vs = model_blanks_vs.data(model_blanks_vs.index(index.row(), 6), Qt::EditRole).toInt();

    this->setCursor(Qt::ArrowCursor);
}

void blanks_wnd::on_bn_edit_blank_vs_clicked() {
    if ( data_app.is_tech
         || data_app.is_head ) {
        // проверим выделенную строку
        QModelIndexList indexes = ui->tab_blanks_vs->selectionModel()->selection().indexes();
        if (indexes.size()>0) {
            QModelIndex index = indexes.at(0);
            QString vs_num = model_blanks_vs.data(model_blanks_vs.index(index.row(), 0), Qt::EditRole).toString();

            if ( vs_num!=ui->ln_blank_num_vs->text().trimmed() ) {
                // проверим номер ВС на уникальность
                QString sql_test = "select count(*) from blanks_vs where vs_num=" +ui->ln_blank_num_vs->text().trimmed();
                QSqlQuery *query_test = new QSqlQuery(db);
                mySQL.exec(this, sql_test, "Проверка номера ВС на уникальность", *query_test, true, db, data_app);
                if (query_test->next() && query_test->value(0).toInt()>0) {
                    QMessageBox::warning(this, "Номер ВС уже занят", "Введённый номер ВС уже использован в другом полисе.\n\nОперация отменена.");
                    delete query_test;
                    return;
                }
                delete query_test;
            }

            // изменим текущую запись
            QString sql = "update blanks_vs set";
            sql += " vs_num=" + ui->ln_blank_num_vs->text().trimmed() + ", ";
            sql += " id_point=" + QString::number(id_point_vs) + ", ";
            sql += " id_operator=" + QString::number(id_operator_vs) + ", ";
            //sql += " date_add='" + ui->date_blank_vs_add->date().toString("yyyy-MM-dd") + "', ";
            sql += ui->date_blank_vs_spent->date()>QDate(1920,1,1) ? (" date_spent='" + ui->date_blank_vs_spent->date().toString("yyyy-MM-dd") + "', ") : (" date_spent=NULL, ");
            sql += " status=" + QString::number(ui->combo_blank_vs_status->currentData().toInt()) + " ";
            sql += " where vs_num=" + vs_num;
            sql += " ; ";

            QSqlQuery *query2 = new QSqlQuery(db);
            if (!mySQL.exec(this, sql, "Правка записи бланка ВС", *query2, true, db, data_app)) {
                delete query2;
                return;
            }
            delete query2;
            refresh_blanks_vs_tab();
        } else {
            QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано.");
        }
    } else {
        QMessageBox::information(this, "Не доступно", "Не доступно на ПВП");
    }
}

void blanks_wnd::on_bn_delete_blank_vs_clicked() {
    /*// проверим выделенную строку
    QModelIndexList indexes = ui->tab_blanks_vs->selectionModel()->selection().indexes();
    if (indexes.size()>0) {
        if (QMessageBox::warning(this, "Нужно подтверждение!", "Вы действительно хотите удалить эту запись?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::No) {
            return;
        }
        QModelIndex index = indexes.at(0);
        int vs_num = model_blanks_vs.data(model_blanks_vs.index(index.row(), 0), Qt::EditRole).toInt();

        // изменим текущую запись
        QString sql = "delete from blanks_vs ";
        sql += " where vs_num=" + QString::number(vs_num);
        sql += " ; ";

        QSqlQuery *query2 = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Удаление записи бланкак ВС", *query2, true, db, data_app)) {
            delete query2;
            return;
        }
        delete query2;
        refresh_blanks_vs_tab();
    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано.");
    }*/
}



// -----------------------------------------------------------
// бланки полисов
// -----------------------------------------------------------
void blanks_wnd::on_bn_refresh_blanks_pol_clicked() {
    refresh_blanks_pol_tab();
}

void blanks_wnd::refresh_blanks_pol_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    ui->tab_blanks_pol->setEnabled(false);
    QApplication::processEvents();

    // данные персоны
    ui->ln_blank_ser_pol->setText("");
    ui->ln_blank_num_pol->setText("");
    ui->ln_enp->setText("");
    ui->lab_blank_pol_point->setText(data_app.point_name);
    ui->lab_blank_pol_operator->setText(data_app.operator_fio);
    ui->date_blank_pol_add->setDate(QDate::currentDate());
    ui->date_blank_pol_spent->setDate(QDate(1900,1,1));
    ui->date_blank_pol_waste->setDate(QDate(1900,1,1));
    ui->combo_blank_pol_status->setCurrentIndex(ui->combo_blank_pol_status->findData(0));

    QString blanks_pol_fields =
            "select b.f_polis, f.text as f_text, b.pol_ser, b.pol_num, b.date_add, b.status, '('||b.status||') '||s.text as status_text, b.id_person, b.id_polis, p.id as id_point, "
            "       '('||p.point_regnum||') '||p.point_name as point_name, o.id as id_operator, o.oper_fio, b.date_sms, b.enp, b.date_scan_enp, "
            "       pol.date_begin, pol.date_end, pol.date_stop, pol.in_erp, pol.order_num,  pol.order_date, "
            "       e.fam, e.im, e.ot, e.sex, e.date_birth, e.bomj, e.snils, e.phone_cell, "
            "       b.id, b.date_spent, b.date_waste ";

    QString blanks_pol_tabs =
            "  from blanks_pol b "
            "       left join spr_fpolis f on(f.code=b.f_polis) "
            "       left join points p on(p.id=b.id_point) "
            "       left join operators o on(o.id=b.id_operator) "
            "       left join spr_blank_status s on(s.code=b.status) "
            "       left join polises pol on(pol.id=b.id_polis) "
            "       left join persons e on(e.id=b.id_person) ";
    bool f_wh = false;
    if (ui->combo_filter_pol_status->currentData().toInt()!=-999) {
        blanks_pol_tabs += " where b.status=" + QString::number(ui->combo_filter_pol_status->currentData().toInt());
        f_wh = true;
    }
    //фамилия
    if (ui->rb_person->isChecked() && !(ui->ln_fam->text().trimmed().isEmpty())) {
        if (!f_wh) { blanks_pol_tabs += " where "; f_wh = true; }
        else { blanks_pol_tabs += " and "; }
        blanks_pol_tabs += " e.fam like('" + ui->ln_fam->text().trimmed().toUpper() + "%') ";
    }
    // имя
    if (ui->rb_person->isChecked() && !(ui->ln_im->text().trimmed().isEmpty())) {
        if (!f_wh) { blanks_pol_tabs += " where "; f_wh = true; }
        else { blanks_pol_tabs += " and "; }
        blanks_pol_tabs += " e.im like('" + ui->ln_im->text().trimmed().toUpper() + "%') ";
    }
    // отество
    if (ui->rb_person->isChecked() && !(ui->ln_ot->text().trimmed().isEmpty())) {
        if (!f_wh) { blanks_pol_tabs += " where "; f_wh = true; }
        else { blanks_pol_tabs += " and "; }
        blanks_pol_tabs += " e.ot like('" + ui->ln_ot->text().trimmed().toUpper() + "%') ";
    }
    // дата рождения
    if (ui->rb_person->isChecked() && ui->ch_date_birth->isChecked()) {
        if (!f_wh) { blanks_pol_tabs += " where "; f_wh = true; }
        else { blanks_pol_tabs += " and "; }
        blanks_pol_tabs += " e.date_birth='" + ui->date_birth->date().toString("yyyy-MM-dd") + "' ";
    }
    // СНИЛС
    if (ui->rb_person->isChecked() && !(ui->ln_snils->text().trimmed().isEmpty())) {
        if (!f_wh) { blanks_pol_tabs += " where "; f_wh = true; }
        else { blanks_pol_tabs += " and "; }
        blanks_pol_tabs += " e.snils like('" + ui->ln_snils->text().trimmed().toUpper() + "%') ";
    }
    // ВС
    if (ui->rb_vs->isChecked() && !(ui->ln_vs_num->text().trimmed().isEmpty())) {
        if (!f_wh) { blanks_pol_tabs += " where "; f_wh = true; }
        else { blanks_pol_tabs += " and "; }
        blanks_pol_tabs += " cast(pol.vs_num as text) like('" + ui->ln_vs_num->text().trimmed().toUpper() + "%') ";
    }
    // серия полиса
    if (ui->rb_pol->isChecked() && !(ui->ln_pol_ser->text().trimmed().isEmpty())) {
        if (!f_wh) { blanks_pol_tabs += " where "; f_wh = true; }
        else { blanks_pol_tabs += " and "; }
        blanks_pol_tabs += " b.pol_ser like('" + ui->ln_pol_ser->text().trimmed().toUpper() + "%') ";
    }
    // номер полиса
    if (ui->rb_pol->isChecked() && !(ui->ln_pol_num->text().trimmed().isEmpty())) {
        if (!f_wh) { blanks_pol_tabs += " where "; f_wh = true; }
        else { blanks_pol_tabs += " and "; }
        blanks_pol_tabs += " b.pol_num like('" + ui->ln_pol_num->text().trimmed().toUpper() + "%') ";
    }
    // ЕНП
    if (ui->rb_pol->isChecked() && !(ui->ln_pol_enp->text().trimmed().isEmpty())) {
        if (!f_wh) { blanks_pol_tabs += " where "; f_wh = true; }
        else { blanks_pol_tabs += " and "; }
        blanks_pol_tabs += " b.enp like('" + ui->ln_pol_enp->text().trimmed().toUpper() + "%') ";
    }
    // ПВП
    if (ui->combo_filter_pol_point->currentData().toInt()>0) {
        if (!f_wh) { blanks_pol_tabs += " where "; f_wh = true; }
        else { blanks_pol_tabs += " and "; }
        blanks_pol_tabs += " b.id_point = " + QString::number(ui->combo_filter_pol_point->currentData().toInt()) + " ";
    }



    // подсчитаем число строк
    QString sql_cnt = "select count(*) as cnt "
                      " " + blanks_pol_tabs + " ; ";
    QSqlQuery *query_count = new QSqlQuery(db);
    bool res_count = mySQL.exec(this, sql_cnt, QString("Подсчёт числа отображаемых строк бланков"), *query_count, true, db, data_app);
    if (!res_count) {
        QMessageBox::warning(this, "Ошибка при подсчёте числа отображаемых строк бланков", "При подсчёте числа отображаемых строк бланков произошла неожиданная ошибка");
        ui->lab_pol_cnt->setText(" -//- ");
    } else {
        query_count->next();
        QString cnt = query_count->value("cnt").toString();
        ui->lab_pol_cnt->setText(cnt);
    }
    delete query_count;



    QString blanks_pol_sql = blanks_pol_fields + blanks_pol_tabs;
    blanks_pol_sql += " order by  b.pol_ser, b.pol_num ";

    model_blanks_pol.setQuery(blanks_pol_sql,db);
    QString err = model_blanks_pol.lastError().driverText();

    // подключаем модель из БД
    ui->tab_blanks_pol->setModel(&model_blanks_pol);

    // обновляем таблицу
    ui->tab_blanks_pol->reset();

    // задаём ширину колонок
    ui->tab_blanks_pol->setColumnWidth( 0,  1);    // b.f_polis
    ui->tab_blanks_pol->setColumnWidth( 1, 80);    // b.f_text
    ui->tab_blanks_pol->setColumnWidth( 2, 50);    // b.pol_ser
    ui->tab_blanks_pol->setColumnWidth( 3, 80);    // b.pol_num
    ui->tab_blanks_pol->setColumnWidth( 4, 80);    // b.date_add
    ui->tab_blanks_pol->setColumnWidth( 5,  1);    // b.status
    ui->tab_blanks_pol->setColumnWidth( 6,120);    // s.text as status_ext
    ui->tab_blanks_pol->setColumnWidth( 7, 50);    // id_person
    ui->tab_blanks_pol->setColumnWidth( 8, 50);    // id_polis
    ui->tab_blanks_pol->setColumnWidth( 9,  1);    // id_point
    ui->tab_blanks_pol->setColumnWidth(10,120);    // p.point_name
    ui->tab_blanks_pol->setColumnWidth(11,  1);    // id_operator
    ui->tab_blanks_pol->setColumnWidth(12,110);    // o.oper_fio
    ui->tab_blanks_pol->setColumnWidth(13, 90);    // enp
    ui->tab_blanks_pol->setColumnWidth(14, 80);    // b.date_scan_enp

    // правим заголовки
    model_blanks_pol.setHeaderData( 0, Qt::Horizontal, ("форма"));
    model_blanks_pol.setHeaderData( 1, Qt::Horizontal, ("форма"));
    model_blanks_pol.setHeaderData( 2, Qt::Horizontal, ("серия"));
    model_blanks_pol.setHeaderData( 3, Qt::Horizontal, ("номер"));
    model_blanks_pol.setHeaderData( 4, Qt::Horizontal, ("дата получ."));
    model_blanks_pol.setHeaderData( 6, Qt::Horizontal, ("статус бланка"));
    model_blanks_pol.setHeaderData( 7, Qt::Horizontal, ("ID перс."));
    model_blanks_pol.setHeaderData( 8, Qt::Horizontal, ("ID пол."));
    model_blanks_pol.setHeaderData(10, Qt::Horizontal, ("пункт ВП"));
    model_blanks_pol.setHeaderData(12, Qt::Horizontal, ("оператор"));
    model_blanks_pol.setHeaderData(13, Qt::Horizontal, ("дата СМС"));
    model_blanks_pol.setHeaderData(14, Qt::Horizontal, ("ЕНП"));
    model_blanks_pol.setHeaderData(15, Qt::Horizontal, ("дат.скан.ЕНП"));
    ui->tab_blanks_pol->repaint();

    ui->tab_blanks_pol->setEnabled(true);
    this->setCursor(Qt::ArrowCursor);
}

void blanks_wnd::on_combo_filter_pol_status_currentIndexChanged(int index) {
    refresh_blanks_pol_tab();
}

void blanks_wnd::on_ln_blank_ser_pol_textChanged(const QString &arg1) {
    QString txt0 = arg1.trimmed(), txt = "";
    for (int i=0; i<txt0.size(); ++i) {
        if (txt.length()>=4)
            break;
        QString c = txt0.mid(i, 1);
        if ( c=="0" || c=="1" || c=="2" || c=="3" || c=="4" || c=="5" || c=="6" || c=="7" || c=="8" || c=="9" ) {
            txt += c;
        }
    }
    ui->ln_blank_ser_pol->setText(txt);
    ui->ln_blank_ser_pol->setCursorPosition(txt.length());
}

void blanks_wnd::on_combo_filter_pol_point_currentIndexChanged(int index) {
    refresh_blanks_pol_tab();
}

void blanks_wnd::on_ln_blank_num_pol_textChanged(const QString &arg1) {
    QString txt0 = arg1.trimmed(), txt = "";
    for (int i=0; i<txt0.size(); ++i) {
        if (txt.length()>=10)
            break;
        QString c = txt0.mid(i, 1);
        if ( c=="0" || c=="1" || c=="2" || c=="3" || c=="4" || c=="5" || c=="6" || c=="7" || c=="8" || c=="9" ) {
            txt += c;
        }
    }
    ui->ln_blank_num_pol->setText(txt);
    ui->ln_blank_num_pol->setCursorPosition(txt.length());
}

void blanks_wnd::on_ln_enp_textChanged(const QString &arg1) {
    QString txt0 = arg1.trimmed(), txt = "";
    for (int i=0; i<txt0.size(); ++i) {
        if (txt.length()>=16)
            break;
        QString c = txt0.mid(i, 1);
        if ( c=="0" || c=="1" || c=="2" || c=="3" || c=="4" || c=="5" || c=="6" || c=="7" || c=="8" || c=="9" ) {
            txt += c;
        }
    }
    ui->ln_enp->setText(txt);
    ui->ln_enp->setCursorPosition(txt.length());
}

void blanks_wnd::on_bn_add_blank_pol_clicked() {
    // Проверим заполнение полей
    if (ui->ln_blank_ser_pol->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Не достаточно данных!", "Серия бланка полиса не должна былть пустой.");
        return;
    }
    if (ui->ln_blank_num_pol->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Не достаточно данных!", "Номер бланка полиса не может быть пустым.");
        return;
    }
    if (ui->ln_enp->text().trimmed().isEmpty() && QMessageBox::warning(this, "Не достаточно данных!", "Номер бланка полиса не может быть пустым.")) {
        return;
    }
    /*if (ui->date_blank_pol_add->date()<QDate::currentDate().addMonths(-1) || ui->date_blank_pol_add->date()>QDate::currentDate().addMonths(1)) {
        QMessageBox::warning(this, "Ошибка в данных!", "Дата добавления нового бланка полиса не может отстоять от текущей даты больше чем на 1 месяц.");
        return;
    }*/
    // проверим соответствие серии форме полиса
    if (ui->ln_blank_ser_pol->text().left(ui->ln_blank_ser_pol->text().size()-2).toInt() != ui->combo_blank_pol_form->currentData().toInt()){
        QMessageBox::warning(this,
                             "Ошибка в данных!",
                             "Введённая серия полиса не соответствует выбранной форме выпуска полиса.\n\n" +
                             ui->ln_blank_ser_pol->text() +
                             "\n\n" +
                             ui->ln_blank_ser_pol->text().left(ui->ln_blank_ser_pol->text().size()-2));
        return;
    }
    /*if (ui->combo_blank_pol_status->currentData().toInt()==0) {
        QMessageBox::warning(this, "Не достаточно данных!", "Выберите статус бланка полиса.");
        return;
    }*/
    // проверим серию и номер бланка полиса на уникальность
    QString sql_tst = "select count(*) from blanks_pol where pol_ser='" +ui->ln_blank_ser_pol->text()+ "' and pol_num='" +ui->ln_blank_num_pol->text()+ "' ; ";
    QSqlQuery *query_test = new QSqlQuery(db);
    mySQL.exec(this, sql_tst, "Проверка серии и номера на уникальность", *query_test, true, db, data_app);
    if (query_test->next() && query_test->value(0).toInt()>0) {
        QMessageBox::warning(this, "Серия и номер полиса уже заняты", "Введённые серия и номер полиса уже использованы в другом полисе.\n\nОперация отменена.");
        delete query_test;
        return;
    }
    delete query_test;

    // собственно сохранение
    QString sql = "insert into blanks_pol(f_polis, pol_ser, pol_num, id_point, id_operator, enp, date_add, status, date_spent) ";
    sql += " values (";
    sql += " "  + QString::number(ui->combo_blank_pol_form->currentData().toInt()) + ", ";
    sql += " '" + ui->ln_blank_ser_pol->text().trimmed() + "', ";
    sql += " '" + ui->ln_blank_num_pol->text().trimmed() + "', ";
    sql += " "  + QString::number(data_app.id_point) + ", ";
    sql += " "  + QString::number(data_app.id_operator) + ", ";
    sql += " '" + ui->ln_enp->text().trimmed() + "', ";
    sql += " '" + ui->date_blank_vs_add->date().toString("yyyy-MM-dd") + "', ";
    sql += " "  + QString::number(ui->combo_blank_pol_status->currentData().toInt()) + ", ";
    sql += " "  + (ui->date_blank_vs_spent->date()>QDate(1920,1,1) ? ("'" + ui->date_blank_vs_spent->date().toString("yyyy-MM-dd") + "'") : "NULL") + ") ; ";

    QSqlQuery *query = new QSqlQuery(db);
    mySQL.exec(this, sql, "Добавление записи бланка полиса", *query, true, db, data_app);
    delete query;

    refresh_blanks_pol_tab();
}

void blanks_wnd::on_tab_blanks_pol_clicked(const QModelIndex &index) {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_blanks_pol || !ui->tab_blanks_pol->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить].");
        return;
    }
    this->setCursor(Qt::WaitCursor);

    if (!ui->rb_pol->isChecked())
        ui->ln_pol_enp->setText(model_blanks_pol.data(model_blanks_pol.index(index.row(), 14), Qt::EditRole).toString());

    // внесём переданные значения в поля ввода
    QString fam = model_blanks_pol.data(model_blanks_pol.index(index.row(), 22), Qt::EditRole).toString();
    QString im  = model_blanks_pol.data(model_blanks_pol.index(index.row(), 23), Qt::EditRole).toString();
    QString ot  = model_blanks_pol.data(model_blanks_pol.index(index.row(), 24), Qt::EditRole).toString();
    QString date_birth = model_blanks_pol.data(model_blanks_pol.index(index.row(), 26), Qt::EditRole).toDate().toString("yyyy-MM-dd");

    ui->ln_filter_fam->setText(fam.trimmed().toUpper());
    ui->ln_filter_im->setText(im.trimmed().toUpper());
    ui->ln_filter_ot->setText(ot.trimmed().toUpper());
    refresh_persons();

    QString id_pers    = QString::number(model_blanks_pol.data(model_blanks_pol.index(index.row(),  7), Qt::EditRole).toInt());
    if (fam.trimmed().isEmpty())
        fam = "-";
    if (im.trimmed().isEmpty())
        im  = "-";
    if (ot.trimmed().isEmpty())
        ot  = "-";
    QString s = fam.trimmed().toUpper() + " " +
            im.trimmed().toUpper()  + " " +
            ot.trimmed().toUpper()  + ", " +
            date_birth  + "  (ID=" +
            id_pers + ")";
    //найдём персону
    int n = -1;
    for (int i=0; i<persons_list.count(); i++) {
        if (persons_list.at(i)==s) {
            n = i;
            break;
        }
    }
    ui->combo_persons->setCurrentIndex(n);

    ui->combo_blank_pol_form->setCurrentIndex (ui->combo_blank_pol_form->findData(model_blanks_pol.data(model_blanks_pol.index(index.row(), 0), Qt::EditRole).toInt()));
    ui->ln_blank_ser_pol->setText (model_blanks_pol.data(model_blanks_pol.index(index.row(), 2), Qt::EditRole).toString());
    ui->ln_blank_num_pol->setText (model_blanks_pol.data(model_blanks_pol.index(index.row(), 3), Qt::EditRole).toString());
    ui->lab_blank_pol_point->setText (model_blanks_pol.data(model_blanks_pol.index(index.row(),10), Qt::EditRole).toString());
    ui->lab_blank_pol_operator->setText (model_blanks_pol.data(model_blanks_pol.index(index.row(),12), Qt::EditRole).toString());
    ui->ln_enp->setText (model_blanks_pol.data(model_blanks_pol.index(index.row(), 14), Qt::EditRole).toString());
    ui->date_blank_pol_add->setDate (model_blanks_pol.data(model_blanks_pol.index(index.row(), 4), Qt::EditRole).toDate());
    ui->date_blank_pol_spent->setDate (QDate(1900,1,1));
    ui->date_blank_pol_spent->setDate (model_blanks_pol.data(model_blanks_pol.index(index.row(),31), Qt::EditRole).toDate());
    ui->date_blank_pol_spent->setVisible (!model_blanks_pol.data(model_blanks_pol.index(index.row(),31), Qt::EditRole).isNull());
    ui->date_blank_pol_waste->setDate (QDate(1900,1,1));
    ui->date_blank_pol_waste->setDate (model_blanks_pol.data(model_blanks_pol.index(index.row(),32), Qt::EditRole).toDate());
    ui->date_blank_pol_waste->setVisible (!model_blanks_pol.data(model_blanks_pol.index(index.row(),32), Qt::EditRole).isNull());
    ui->combo_blank_pol_status->setCurrentIndex (ui->combo_blank_pol_status->findData(model_blanks_pol.data(model_blanks_pol.index(index.row(), 5), Qt::EditRole).toInt()));

    id_point_pol = model_blanks_pol.data(model_blanks_pol.index(index.row(), 7), Qt::EditRole).toInt();
    id_operator_pol = model_blanks_pol.data(model_blanks_pol.index(index.row(), 9), Qt::EditRole).toInt();

    this->setCursor(Qt::ArrowCursor);
}

void blanks_wnd::on_bn_edit_blank_pol_clicked() {
    if ( data_app.is_tech
         || data_app.is_head ) {
        // проверим выделенную строку
        QModelIndexList indexes = ui->tab_blanks_pol->selectionModel()->selection().indexes();
        if (indexes.size()>0) {
            QModelIndex index = indexes.at(0);
            QString pol_ser = model_blanks_pol.data(model_blanks_pol.index(index.row(), 2), Qt::EditRole).toString();
            QString pol_num = model_blanks_pol.data(model_blanks_pol.index(index.row(), 3), Qt::EditRole).toString();

            // Проверим заполнение полей
            if (ui->ln_blank_ser_pol->text().trimmed().isEmpty()) {
                QMessageBox::warning(this, "Не достаточно данных!", "Серия бланка полиса не должна былть пустой.");
                return;
            }
            if (ui->ln_blank_num_pol->text().trimmed().isEmpty()) {
                QMessageBox::warning(this, "Не достаточно данных!", "Номер бланка полиса не может быть пустым.");
                return;
            }
            if (ui->ln_enp->text().trimmed().isEmpty() && QMessageBox::warning(this, "Не достаточно данных!", "Номер бланка полиса не может быть пустым.")) {
                return;
            }
            /*if (ui->date_blank_pol_add->date()<QDate::currentDate().addMonths(-1) || ui->date_blank_pol_add->date()>QDate::currentDate().addMonths(1)) {
                QMessageBox::warning(this, "Не достаточно данных!", "Дата добавления нового бланка полиса не может отстоять от текущей даты больше чем на 1 месяц.");
                return;
            }*/
            /*if (ui->combo_blank_pol_status->currentData().toInt()==0) {
                QMessageBox::warning(this, "Не достаточно данных!", "Выберите статус бланка полиса.");
                return;
            }*/

            if (ui->ln_blank_ser_pol->text()!=pol_ser || ui->ln_blank_num_pol->text()!=pol_num) {
                // проверим серию и номер бланка полиса на уникальность
                QString sql_test = "select count(*) from blanks_pol where pol_ser='" +ui->ln_blank_ser_pol->text()+ "' and pol_num='" +ui->ln_blank_num_pol->text()+ "' ; ";
                QSqlQuery *query_test = new QSqlQuery(db);
                mySQL.exec(this, sql_test, "Проверка серии и номера на уникальность", *query_test, true, db, data_app);
                if (query_test->next() && query_test->value(0).toInt()>0) {
                    QMessageBox::warning(this, "Серия и номер полиса уже заняты", "Введённые серия и номер полиса уже использованы в другом полисе.\n\nОперация отменена.");
                    delete query_test;
                    return;
                }
                delete query_test;
            }

            // изменим текущую запись
            QString sql = "update blanks_pol set";
            sql += " f_polis=" + QString::number(ui->combo_blank_pol_form->currentData().toInt()) + ", ";
            sql += " pol_ser='" + ui->ln_blank_ser_pol->text().trimmed() + "', ";
            sql += " pol_num='" + ui->ln_blank_num_pol->text().trimmed() + "', ";
            sql += " id_point=" + QString::number(id_point_pol) + ", ";
            sql += " id_operator=" + QString::number(id_operator_pol) + ", ";
            sql += " enp='" + ui->ln_enp->text().trimmed() + "', ";
            sql += " date_add='" + ui->date_blank_pol_add->date().toString("yyyy-MM-dd") + "', ";
            sql += " date_spent=" + (ui->date_blank_pol_spent->date()>QDate(1920,1,1) ? ("'" + ui->date_blank_pol_spent->date().toString("yyyy-MM-dd") + "'") : "NULL") + ", ";
            sql += " status=" + QString::number(ui->combo_blank_pol_status->currentData().toInt()) + ", ";
            sql += " id_person=" + QString::number(ui->combo_persons->currentData().toInt()) + " ";
            sql += " where pol_ser='" +pol_ser+ "' and pol_num='" +pol_num+ "' ; ";
            sql += " ; ";

            QSqlQuery *query2 = new QSqlQuery(db);
            if (!mySQL.exec(this, sql, "Правка записи бланка полиса", *query2, true, db, data_app)) {
                delete query2;
                return;
            }
            delete query2;
            refresh_blanks_pol_tab();
        } else {
            QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано.");
        }
    } else {
        QMessageBox::information(this, "Не доступно", "Не доступно на ПВП");
    }
}

void blanks_wnd::on_bn_delete_blank_pol_clicked() {
    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_blanks_pol->selectionModel()->selection().indexes();
    if (indexes.size()>0) {
        if (QMessageBox::warning(this, "Нужно подтверждение!", "Вы действительно хотите удалить эту запись?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::No) {
            return;
        }
        QModelIndex index = indexes.at(0);
        int id = model_blanks_pol.data(model_blanks_pol.index(index.row(), 30), Qt::EditRole).toInt();
        //QString pol_ser = model_blanks_pol.data(model_blanks_pol.index(index.row(), 2), Qt::EditRole).toString();
        //QString pol_num = model_blanks_pol.data(model_blanks_pol.index(index.row(), 3), Qt::EditRole).toString();

        // изменим текущую запись
        QString sql = "delete from blanks_pol ";
        sql += " where id=" + QString::number(id) + " ; ";

        QSqlQuery *query2 = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Удаление записи бланкак полиса", *query2, true, db, data_app)) {
            delete query2;
            return;
        }
        delete query2;
        refresh_blanks_pol_tab();
    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано.");
    }
}

void blanks_wnd::on_rb_person_clicked(bool checked) {
    // персона
    ui->lab_fam->setEnabled(checked);
    ui->lab_im->setEnabled(checked);
    ui->lab_ot->setEnabled(checked);
    ui->ch_date_birth->setEnabled(checked);
    ui->date_birth->setEnabled(checked);
    ui->lab_snils->setEnabled(checked);
    ui->ln_fam->setEnabled(checked);
    ui->ln_im->setEnabled(checked);
    ui->ln_ot->setEnabled(checked);
    ui->date_birth->setEnabled(checked);
    ui->ln_snils->setEnabled(checked);
    // ВС
    ui->lab_vs_num->setEnabled(false);
    ui->ln_vs_num->setEnabled(false);
    ui->ch_vs_num_2->setEnabled(false);
    ui->ln_vs_num_2->setEnabled(false && ui->ch_vs_num_2->isChecked());
    ui->lab_vs_N->setEnabled(false);
    //полис
    ui->lab_pol_ser->setEnabled(false);
    ui->lab_pol_num->setEnabled(false);
    ui->lab_pol_enp->setEnabled(false);
    ui->ln_pol_ser->setEnabled(false);
    ui->ln_pol_num->setEnabled(false);
    ui->ln_pol_enp->setEnabled(false);
}

void blanks_wnd::on_rb_vs_clicked(bool checked) {
    // персона
    ui->lab_fam->setEnabled(false);
    ui->lab_im->setEnabled(false);
    ui->lab_ot->setEnabled(false);
    ui->ch_date_birth->setEnabled(false);
    ui->ch_date_birth->setChecked(false);
    ui->date_birth->setEnabled(false);
    ui->lab_snils->setEnabled(false);
    ui->ln_fam->setEnabled(false);
    ui->ln_im->setEnabled(false);
    ui->ln_ot->setEnabled(false);
    ui->date_birth->setEnabled(false);
    ui->ln_snils->setEnabled(false);
    // ВС
    ui->lab_vs_num->setEnabled(checked);
    ui->ln_vs_num->setEnabled(checked);
    ui->ch_vs_num_2->setEnabled(checked);
    ui->ln_vs_num_2->setEnabled(checked && ui->ch_vs_num_2->isChecked());
    ui->lab_vs_N->setEnabled(checked);
    // полис
    ui->lab_pol_ser->setEnabled(false);
    ui->lab_pol_num->setEnabled(false);
    ui->lab_pol_enp->setEnabled(false);
    ui->ln_pol_ser->setEnabled(false);
    ui->ln_pol_num->setEnabled(false);
    ui->ln_pol_enp->setEnabled(false);
}

void blanks_wnd::on_ch_vs_num_2_clicked(bool checked) {
    ui->ln_vs_num_2->setEnabled(checked);
    if (checked)
        ui->lab_vs_num->setText("№ ВС (от)");
    else
        ui->lab_vs_num->setText("№ ВС");

    on_ln_vs_num_editingFinished();
}

void blanks_wnd::on_rb_pol_clicked(bool checked) {
    // персона
    ui->lab_fam->setEnabled(false);
    ui->lab_im->setEnabled(false);
    ui->lab_ot->setEnabled(false);
    ui->ch_date_birth->setEnabled(false);
    ui->ch_date_birth->setChecked(false);
    ui->date_birth->setEnabled(false);
    ui->lab_snils->setEnabled(false);
    ui->ln_fam->setEnabled(false);
    ui->ln_im->setEnabled(false);
    ui->ln_ot->setEnabled(false);
    ui->date_birth->setEnabled(false);
    ui->ln_snils->setEnabled(false);
    // ВС
    ui->lab_vs_num->setEnabled(false);
    ui->ln_vs_num->setEnabled(false);
    ui->ch_vs_num_2->setEnabled(false);
    ui->ln_vs_num_2->setEnabled(false && ui->ch_vs_num_2->isChecked());
    ui->lab_vs_N->setEnabled(false);
    // полис
    ui->lab_pol_ser->setEnabled(checked);
    ui->lab_pol_num->setEnabled(checked);
    ui->lab_pol_enp->setEnabled(checked);
    ui->ln_pol_ser->setEnabled(checked);
    ui->ln_pol_num->setEnabled(checked);
    ui->ln_pol_enp->setEnabled(checked);
}

void blanks_wnd::on_bn_use_filter_clicked() {
    refresh_blanks_vs_tab();
    refresh_blanks_pol_tab();
}

void blanks_wnd::on_bn_clear_filter_clicked() {
    ui->rb_person->setChecked(false);
    ui->rb_vs->setChecked(false);
    ui->rb_pol->setChecked(false);

    ui->ln_fam->setText("");
    ui->ln_im->setText("");
    ui->ln_ot->setText("");
    ui->ln_filter_fam->setText("");
    ui->ln_filter_im->setText("");
    ui->ln_filter_ot->setText("");
    ui->ln_snils->setText("");
    ui->ln_vs_num->setText("");
    ui->ln_pol_ser->setText("");
    ui->ln_pol_num->setText("");
    ui->ln_pol_enp->setText("");
    ui->date_birth->setDate(QDate::fromString("1900-01-01"));
    ui->ch_date_birth->setChecked(false);

    on_bn_use_filter_clicked();
}

void blanks_wnd::on_ln_snils_textEdited(const QString &arg1) {
    QString str0 = arg1.trimmed(), str = "";
    for (int i=0; i<str0.size(); ++i) {
        if (str.length()>=11)
            break;
        QString c = str0.mid(i, 1);
        if ( c=="0" || c=="1" || c=="2" || c=="3" || c=="4" || c=="5" || c=="6" || c=="7" || c=="8" || c=="9" ) {
            str += c;
        }
    }
    ui->ln_snils->setText(str);
    ui->ln_snils->setCursorPosition(str.size());
}

void blanks_wnd::on_ln_vs_num_textEdited(const QString &arg1) {
    QString str0 = arg1.trimmed(), str = "";
    for (int i=0; i<str0.size(); ++i) {
        if (str.length()>=14)
            break;
        QString c = str0.mid(i, 1);
        if ( c=="0" || c=="1" || c=="2" || c=="3" || c=="4" || c=="5" || c=="6" || c=="7" || c=="8" || c=="9" ) {
            str += c;
        }
    }
    ui->ln_vs_num->setText(str);
    ui->ln_vs_num->setCursorPosition(str.size());
}

void blanks_wnd::on_ln_pol_ser_textEdited(const QString &arg1) {
    QString str0 = arg1.trimmed(), str = "";
    for (int i=0; i<str0.size(); ++i) {
        if (str.length()>=4)
            break;
        QString c = str0.mid(i, 1);
        if ( c=="0" || c=="1" || c=="2" || c=="3" || c=="4" || c=="5" || c=="6" || c=="7" || c=="8" || c=="9" ) {
            str += c;
        }
    }
    ui->ln_pol_ser->setText(str);
    ui->ln_pol_ser->setCursorPosition(str.size());
}

void blanks_wnd::on_ln_pol_num_textEdited(const QString &arg1) {
    QString str0 = arg1.trimmed(), str = "";
    for (int i=0; i<str0.size(); ++i) {
        if (str.length()>=14)
            break;
        QString c = str0.mid(i, 1);
        if ( c=="0" || c=="1" || c=="2" || c=="3" || c=="4" || c=="5" || c=="6" || c=="7" || c=="8" || c=="9" ) {
            str += c;
        }
    }
    ui->ln_pol_num->setText(str);
    ui->ln_pol_num->setCursorPosition(str.size());
}

void blanks_wnd::on_ln_pol_enp_textEdited(const QString &arg1) {
    QString str0 = arg1.trimmed(), str = "";
    for (int i=0; i<str0.size(); ++i) {
        if (str.length()>=16)
            break;
        QString c = str0.mid(i, 1);
        if ( c=="0" || c=="1" || c=="2" || c=="3" || c=="4" || c=="5" || c=="6" || c=="7" || c=="8" || c=="9" ) {
            str += c;
        }
    }
    ui->ln_pol_enp->setText(str);
    ui->ln_pol_enp->setCursorPosition(str.size());
}

void blanks_wnd::on_ln_blank_num_pol_cursorPositionChanged(int arg1, int arg2) {
    /*QString str = ui->ln_blank_num_pol->text().trimmed().simplified();
    ui->ln_blank_num_pol->setText(str);
    ui->ln_blank_num_pol->setCursorPosition(str.length());*/
}

void blanks_wnd::on_bn_add_blank_clicked() {
    ui->pan_vs->setVisible(false);

    // откроем окно сканера штрихкодов
    s_data_new_blank data_new_blank;
    data_new_blank.bc_type = 2;
    data_new_blank.date_birth = QDate(1900,1,1);
    data_new_blank.date_exp = QDate(1900,1,1);
    data_new_blank.eds = "";
    data_new_blank.enp = "";
    data_new_blank.fam = "";
    data_new_blank.im = "";
    data_new_blank.ot = "";
    data_new_blank.pol_num = "";
    data_new_blank.pol_ser = "";

    delete add_blank_w;
    add_blank_w = new add_blank_wnd(db, data_app, data_new_blank, settings, this);
    add_blank_w->exec();

    refresh_blanks_pol_tab();
    ui->pan_vs->setVisible(true);
}

void blanks_wnd::on_bn_scan_blanks_clicked() {
    // откроем окно сканера штрихкодов
    delete add_blank_scan_w;
    add_blank_scan_w = new add_blank_scan_wnd(db, data_app, settings, this);
    add_blank_scan_w->exec();
    refresh_blanks_pol_tab();
}

void blanks_wnd::on_bn_toCSV_vs_clicked() {
    bool res = mySQL.tab_to_CSV(model_blanks_vs, ui->tab_blanks_vs);
}

void blanks_wnd::on_bn_toCSV_pol_clicked() {
    bool res = mySQL.tab_to_CSV(model_blanks_pol, ui->tab_blanks_pol);
}

void blanks_wnd::on_ln_vs_num_editingFinished() {
    if (ui->ch_vs_num_2->isChecked()) {
        int i1 = ui->ln_vs_num->text().toInt();
        int i2 = ui->ln_vs_num_2->text().toInt();
        if (i2<i1) {
            ui->ln_vs_num_2->setText(QString::number(i1));
            i2=i1;
        }
        ui->lab_vs_N->setText(QString::number(i2-i1+1) + " ВС");
    } else {
        ui->lab_vs_N->setText("-");
    }
}

void blanks_wnd::on_ln_vs_num_2_editingFinished() {
    if (ui->ch_vs_num_2->isChecked()) {
        int i1 = ui->ln_vs_num->text().toInt();
        int i2 = ui->ln_vs_num_2->text().toInt();
        if (i2<i1) {
            ui->ln_vs_num_2->setText(QString::number(i1));
            i2=i1;
        }
        ui->lab_vs_N->setText(QString::number(i2-i1+1) + " ВС");
    } else {
        ui->lab_vs_N->setText("-");
    }
}

void blanks_wnd::on_ln_filter_fam_editingFinished() {
    refresh_persons();
}
void blanks_wnd::on_ln_filter_im_editingFinished() {
    refresh_persons();
}
void blanks_wnd::on_ln_filter_ot_editingFinished() {
    refresh_persons();
}

void blanks_wnd::on_bn_clear_clicked() {
    ui->ln_filter_fam->clear();
    ui->ln_filter_im->clear();
    ui->ln_filter_ot->clear();
    refresh_persons();
}

void blanks_wnd::on_ln_blank_num_vs_textChanged(const QString &arg1)  {
    QString txt0 = arg1.trimmed(), txt = "";
    for (int i=0; i<txt0.size(); ++i) {
        if (txt.length()>=11)
            break;
        QString c = txt0.mid(i, 1);
        if ( c=="0" || c=="1" || c=="2" || c=="3" || c=="4" || c=="5" || c=="6" || c=="7" || c=="8" || c=="9" ) {
            txt += c;
        }
    }
    ui->ln_blank_num_vs->setText(txt);
    ui->ln_blank_num_vs->setCursorPosition(txt.length());
}

void blanks_wnd::on_ln_vs_num_textChanged(const QString &arg1) {
    QString txt0 = arg1.trimmed(), txt = "";
    for (int i=0; i<txt0.size(); ++i) {
        if (txt.length()>=11)
            break;
        QString c = txt0.mid(i, 1);
        if ( c=="0" || c=="1" || c=="2" || c=="3" || c=="4" || c=="5" || c=="6" || c=="7" || c=="8" || c=="9" ) {
            txt += c;
        }
    }
    ui->ln_vs_num->setText(txt);
    ui->ln_vs_num->setCursorPosition(txt.length());
}

void blanks_wnd::on_ln_vs_num_2_textChanged(const QString &arg1) {
    QString txt0 = arg1.trimmed(), txt = "";
    for (int i=0; i<txt0.size(); ++i) {
        if (txt.length()>=11)
            break;
        QString c = txt0.mid(i, 1);
        if ( c=="0" || c=="1" || c=="2" || c=="3" || c=="4" || c=="5" || c=="6" || c=="7" || c=="8" || c=="9" ) {
            txt += c;
        }
    }
    ui->ln_vs_num_2->setText(txt);
    ui->ln_vs_num_2->setCursorPosition(txt.length());
}

void blanks_wnd::on_ln_pol_ser_textChanged(const QString &arg1) {
    QString txt0 = arg1.trimmed(), txt = "";
    for (int i=0; i<txt0.size(); ++i) {
        if (txt.length()>=4)
            break;
        QString c = txt0.mid(i, 1);
        if ( c=="0" || c=="1" || c=="2" || c=="3" || c=="4" || c=="5" || c=="6" || c=="7" || c=="8" || c=="9" ) {
            txt += c;
        }
    }
    ui->ln_pol_ser->setText(txt);
    ui->ln_pol_ser->setCursorPosition(txt.length());
}

void blanks_wnd::on_ln_pol_num_textChanged(const QString &arg1) {
    QString txt0 = arg1.trimmed(), txt = "";
    for (int i=0; i<txt0.size(); ++i) {
        if (txt.length()>=11)
            break;
        QString c = txt0.mid(i, 1);
        if ( c=="0" || c=="1" || c=="2" || c=="3" || c=="4" || c=="5" || c=="6" || c=="7" || c=="8" || c=="9" ) {
            txt += c;
        }
    }
    ui->ln_pol_num->setText(txt);
    ui->ln_pol_num->setCursorPosition(txt.length());
}

void blanks_wnd::on_ln_pol_enp_textChanged(const QString &arg1) {
    QString txt0 = arg1.trimmed(), txt = "";
    for (int i=0; i<txt0.size(); ++i) {
        if (txt.length()>=16)
            break;
        QString c = txt0.mid(i, 1);
        if ( c=="0" || c=="1" || c=="2" || c=="3" || c=="4" || c=="5" || c=="6" || c=="7" || c=="8" || c=="9" ) {
            txt += c;
        }
    }
    ui->ln_pol_enp->setText(txt);
    ui->ln_pol_enp->setCursorPosition(txt.length());
}

void blanks_wnd::on_ln_snils_textChanged(const QString &arg1) {
    QString txt0 = arg1.trimmed(), txt = "";
    for (int i=0; i<txt0.size(); ++i) {
        if (txt.length()>=11)
            break;
        QString c = txt0.mid(i, 1);
        if ( c=="0" || c=="1" || c=="2" || c=="3" || c=="4" || c=="5" || c=="6" || c=="7" || c=="8" || c=="9" ) {
            txt += c;
        }
    }
    ui->ln_snils->setText(txt);
    ui->ln_snils->setCursorPosition(txt.length());
}

void blanks_wnd::on_ln_fam_returnPressed() {
    refresh_blanks_vs_tab();
    refresh_blanks_pol_tab();
}

void blanks_wnd::on_ln_im_returnPressed() {
    refresh_blanks_vs_tab();
    refresh_blanks_pol_tab();
}

void blanks_wnd::on_ln_ot_returnPressed() {
    refresh_blanks_vs_tab();
    refresh_blanks_pol_tab();
}

void blanks_wnd::on_date_birth_editingFinished() {
    refresh_blanks_vs_tab();
    refresh_blanks_pol_tab();
}

void blanks_wnd::on_ln_snils_returnPressed() {
    refresh_blanks_vs_tab();
    refresh_blanks_pol_tab();
}

void blanks_wnd::on_ln_vs_num_returnPressed() {
    refresh_blanks_vs_tab();
    refresh_blanks_pol_tab();
}

void blanks_wnd::on_ln_vs_num_2_returnPressed() {
    refresh_blanks_vs_tab();
    refresh_blanks_pol_tab();
}

void blanks_wnd::on_ln_pol_ser_returnPressed() {
    refresh_blanks_vs_tab();
    refresh_blanks_pol_tab();
}

void blanks_wnd::on_ln_pol_num_returnPressed() {
    refresh_blanks_vs_tab();
    refresh_blanks_pol_tab();
}

void blanks_wnd::on_ln_pol_enp_returnPressed() {
    refresh_blanks_vs_tab();
    refresh_blanks_pol_tab();
}
