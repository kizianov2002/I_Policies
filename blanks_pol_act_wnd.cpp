#include "blanks_pol_act_wnd.h"
#include "ui_blanks_pol_act_wnd.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QTableWidgetItem>

blanks_pol_act_wnd::blanks_pol_act_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    QDialog(parent), db(db), data_app(data_app), settings(settings), ui(new Ui::blanks_pol_act_wnd)
{
    ui->setupUi(this);

    print_blanks_pol_act_w = NULL;

    refresh_points();
    refresh_fpolis();
    ui->date_back_from->setDate(QDate::currentDate().addMonths(-1));
    ui->date_back_to->setDate(QDate::currentDate());

    ui->pan_back->setEnabled(false);

    ui->pan_filter->setVisible(false);

    on_bn_gen_list_blanks_clicked();
}

blanks_pol_act_wnd::~blanks_pol_act_wnd() {
    delete ui;
}

void blanks_pol_act_wnd::refresh_points() {
    this->setCursor(Qt::WaitCursor);

    QString sql = "";
    sql += QString("select p.id, '('||p.point_regnum||') '||p.point_name as name"
                   "  from points p "
                   " order by p.point_regnum ; ");
    QSqlQuery *query = new QSqlQuery(db);
    query->exec( sql );
    ui->combo_point->clear();

    // заполнение списка пунктов выдачи
    //ui->combo_point->addItem(" - все ПВП - ");
    while (query->next()) {
        ui->combo_point->addItem( query->value(1).toString(),
                                  query->value(0).toInt() );
    }
    query->finish();
    delete query;
    ui->combo_point->setCurrentIndex(0);

    refresh_combo_obtainers(ui->combo_point->currentData().toInt());

    this->setCursor(Qt::ArrowCursor);
}

void blanks_pol_act_wnd::refresh_combo_obtainers(int id_point) {
    this->setCursor(Qt::WaitCursor);

    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "";
    sql += "select u.id, u.oper_fio "
           " from operators u "
           " where u.status>0 ";
    sql += (id_point==-1) ? (QString(" ")) : (QString(" and u.id_point=") + QString::number(id_point));
    sql += " order by u.oper_fio ; ";
    query->exec( sql );
    ui->combo_obtainer->clear();

    // заполнение списка операторов
    while (query->next())
        ui->combo_obtainer->addItem( query->value(1).toString(), query->value(0).toInt() );

    query->finish();
    delete query;
    ui->combo_obtainer->setCurrentIndex(0);
    this->setCursor(Qt::ArrowCursor);
}

void blanks_pol_act_wnd::refresh_fpolis() {
    this->setCursor(Qt::WaitCursor);

    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "";
    sql += QString("select s.code, '('||s.code||') '||s.text as name"
                   " from spr_fpolis s "
                   " where s.code>0 and s.code<3 "
                   " order by s.code ; ");
    query->exec( sql );
    ui->combo_fpolis->clear();

    // заполнение списка пунктов выдачи
    ui->combo_fpolis->addItem(" - любые полисы - ");
    while (query->next()) {
        ui->combo_fpolis->addItem( query->value(1).toString(),
                                  query->value(0).toInt() );
    }
    query->finish();
    delete query;
    ui->combo_fpolis->setCurrentIndex(0);

    this->setCursor(Qt::ArrowCursor);
}

void blanks_pol_act_wnd::on_bn_close_clicked() {
    close();
}

void blanks_pol_act_wnd::on_bn_gen_list_blanks_clicked() {
    // проверка даты
    if (ui->rb_back->isChecked() && ui->date_back_from->date()<QDate(1950,1,1)) {
        QMessageBox::warning(this, "Задайте предельную дату", "Дата начала диапазона не должны быть раньше 1 января 1950 г.");
    }
    if (ui->rb_back->isChecked() && ui->date_back_to->date()<QDate(1950,1,1)) {
        QMessageBox::warning(this, "Задайте предельную дату", "Дата конца диапазона не должны быть раньше 1 января 1950 г.");
    }
    if (ui->rb_back->isChecked() && ui->date_back_from->date()>ui->date_back_to->date()) {
        QMessageBox::warning(this, "Задайте предельную дату", "Дата конца диапазона не может быть раньше даты начала диапазона.");
    }

    // сформируем список бланков полисов вплоть до указанной даты, для которых не выполнялась передача
    sql_tab = "select bp.id as id_blank_pol, bp.pol_ser, bp.pol_num, bp.enp, e.id as id_person, e.fam, e.im, e.ot, e.date_birth, ap.id as id_act, ap.act_date, pt.id as id_point, pt.point_name "
              "  from blanks_pol bp"
              "       left join persons e on(e.id=bp.id_person)"
              "       left join blanks_pol_acts ap on(ap.id=bp.id_blanks_pol_act) "
              "       left join points pt on(ap.id_point=pt.id) ";

    if (ui->rb_new->isChecked()) {
        sql_tab+= "   left join polises vs on(vs.id=e.id_polis and vs.pol_v=2) "
                  " where vs._id_last_point=" + QString::number(ui->combo_point->currentData().toInt()) + " "
                  "   and bp.status in(0,1) ";
    } else if (ui->rb_back->isChecked()) {
        sql_tab+= "   left join polises p on(p.id=e.id_polis) "
                  " where p._id_last_point=" + QString::number(ui->combo_point->currentData().toInt()) + " "
                  "   and bp.status in(0,1,2,3) "
                  "   and bp.date_scan_enp>='" + ui->date_back_from->date().toString("yyyy-MM-dd") + "' "
                  "   and bp.date_scan_enp<='" + ui->date_back_to->date().toString("yyyy-MM-dd")   + "' ";
    }

    if (ui->ch_no_act->isChecked()) {
        sql_tab+= "   and ap.id is NULL ";
    }
    if (ui->combo_fpolis->currentIndex()>0) {
        if (ui->rb_new->isChecked())
             sql_tab+= "   and vs.f_polis=" + QString::number(ui->combo_fpolis->currentData().toInt()) + " ";
        else sql_tab+= "   and p.f_polis=" + QString::number(ui->combo_fpolis->currentData().toInt()) + " ";
    }
    if (ui->combo_polis_post_way->currentIndex()>0) {
        sql_tab+= "   and e.polis_post_way=" + QString::number(ui->combo_polis_post_way->currentIndex()) + " ";
    }

    sql_tab+= " order by e.fam, e.im, e.ot, e.date_birth ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql_tab, QString("Выбор бланков полисов для формирования передаточного акта"), *query, true, db, data_app);
    if (!res) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
    }

    model_phones.setQuery(sql_tab,db);
    QString err2 = model_phones.lastError().driverText();

    // подключаем модель из БД
    ui->tab_blanks_pol->setModel(&model_phones);

    // обновляем таблицу
    ui->tab_blanks_pol->reset();

    // задаём ширину колонок
    ui->tab_blanks_pol->setColumnWidth( 0,  1);     // bp.id as id_blank_pol,
    ui->tab_blanks_pol->setColumnWidth( 1, 40);     // bp.pol_ser,
    ui->tab_blanks_pol->setColumnWidth( 2, 60);     // bp.pol_num,
    ui->tab_blanks_pol->setColumnWidth( 3,115);     // bp.enp,
    ui->tab_blanks_pol->setColumnWidth( 4,  1);     // e.id as id_person,
    ui->tab_blanks_pol->setColumnWidth( 5,100);     // e.fam,
    ui->tab_blanks_pol->setColumnWidth( 6,100);     // e.im,
    ui->tab_blanks_pol->setColumnWidth( 7,100);     // e.ot,
    ui->tab_blanks_pol->setColumnWidth( 8, 80);     // e.date_birth,
    ui->tab_blanks_pol->setColumnWidth( 9,  1);     // ap.id as id_act,
    ui->tab_blanks_pol->setColumnWidth(10, 80);     // ap.act_date,
    ui->tab_blanks_pol->setColumnWidth(11,  1);     // pt.id as id_point,
    ui->tab_blanks_pol->setColumnWidth(12,150);     // pt.point_name

    // правим заголовки
    model_phones.setHeaderData( 0, Qt::Horizontal, ("ID бланка"));
    model_phones.setHeaderData( 1, Qt::Horizontal, ("серия"));
    model_phones.setHeaderData( 2, Qt::Horizontal, ("номер"));
    model_phones.setHeaderData( 3, Qt::Horizontal, ("ЕНП"));
    model_phones.setHeaderData( 4, Qt::Horizontal, ("ID персоны"));
    model_phones.setHeaderData( 5, Qt::Horizontal, ("фамилия"));
    model_phones.setHeaderData( 6, Qt::Horizontal, ("имя"));
    model_phones.setHeaderData( 7, Qt::Horizontal, ("отчество"));
    model_phones.setHeaderData( 8, Qt::Horizontal, ("дата \nрождения"));
    model_phones.setHeaderData( 9, Qt::Horizontal, ("ID акта"));
    model_phones.setHeaderData(10, Qt::Horizontal, ("дата акта\nпередачи"));
    model_phones.setHeaderData(11, Qt::Horizontal, ("ID ПВП"));
    model_phones.setHeaderData(12, Qt::Horizontal, ("передан в ПВП"));
    ui->tab_blanks_pol->repaint();

    if (!ui->ch_filter->isChecked()) {
        ui->lab_cnt->setText(QString::number(model_phones.rowCount()));
    } else {
        //ui->tab_blanks_pol->clearSelection();
        ui->lab_cnt->setText(QString::number(ui->tab_blanks_pol->selectionModel()->selectedIndexes().count()/13));
    }

    this->setCursor(Qt::ArrowCursor);
}

void blanks_pol_act_wnd::on_rb_new_clicked(bool checked) {
    //ui->ch_no_act->setEnabled(checked);
    ui->ch_no_act->setChecked(checked);
    //ui->ch_no_activate->setEnabled(checked);
    //ui->ch_no_activate->setChecked(checked);
    //ui->ch_no_get2hand->setEnabled(checked);
    //ui->ch_no_get2hand->setChecked(checked);

    ui->pan_back->setEnabled(!checked);

    on_bn_gen_list_blanks_clicked();
}

void blanks_pol_act_wnd::on_rb_back_clicked(bool checked) {
    if (checked) {
        QString s = QInputDialog::getText(this,"Нужно подтверждение",QString("Введите пароль для получения выгрузки задним числом.\n") +
                                          (data_app.is_head||data_app.is_tech ? "ПЕРЕДАЧА" : "***")).toUpper();
        if (s!="ПЕРЕДАЧА" && s!="GTHTLFXF") {
            ui->rb_new->setChecked(true);
            ui->pan_back->setEnabled(false);

            ui->ch_no_act->setChecked(true);
            //ui->ch_no_act->setEnabled(true);
            //ui->ch_no_activate->setChecked(true);
            //ui->ch_no_activate->setEnabled(true);
            //ui->ch_no_get2hand->setChecked(true);
            //ui->ch_no_get2hand->setEnabled(true);

            on_bn_gen_list_blanks_clicked();
            return;
        } else {
            ui->rb_back->setChecked(true);
            ui->pan_back->setEnabled(true);

            ui->ch_no_act->setChecked(false);
            //ui->ch_no_act->setEnabled(false);
            //ui->ch_no_activate->setChecked(false);
            //ui->ch_no_activate->setEnabled(false);
            //ui->ch_no_get2hand->setChecked(false);
            //ui->ch_no_get2hand->setEnabled(false);
        }
    } else {
        ui->rb_new->setChecked(true);
        ui->pan_back->setEnabled(false);

        ui->ch_no_act->setChecked(true);
        //ui->ch_no_act->setEnabled(true);
        //ui->ch_no_activate->setChecked(true);
        //ui->ch_no_activate->setEnabled(true);
        //ui->ch_no_get2hand->setChecked(true);
        //ui->ch_no_get2hand->setEnabled(true);
    }
    on_bn_gen_list_blanks_clicked();
}

void blanks_pol_act_wnd::on_date_back_from_editingFinished() {
    on_bn_gen_list_blanks_clicked();
}
void blanks_pol_act_wnd::on_date_back_to_editingFinished() {
    on_bn_gen_list_blanks_clicked();
}
void blanks_pol_act_wnd::on_date_to_editingFinished() {
    on_bn_gen_list_blanks_clicked();
}
void blanks_pol_act_wnd::on_ch_phone_clicked() {
    on_bn_gen_list_blanks_clicked();
}
void blanks_pol_act_wnd::on_combo_point_activated(int index) {
    on_bn_gen_list_blanks_clicked();
}
void blanks_pol_act_wnd::on_combo_fpolis_activated(int index) {
    on_bn_gen_list_blanks_clicked();
}
void blanks_pol_act_wnd::on_combo_polis_post_way_activated(int index) {
    on_bn_gen_list_blanks_clicked();
}
void blanks_pol_act_wnd::on_ch_no_activate_clicked(bool checked) {
    on_bn_gen_list_blanks_clicked();
}
void blanks_pol_act_wnd::on_ch_no_get2hand_clicked(bool checked) {
    on_bn_gen_list_blanks_clicked();
}
void blanks_pol_act_wnd::on_ch_no_act_clicked(bool checked) {
    on_bn_gen_list_blanks_clicked();
}

#include "qt_windows.h"
#include "qwindowdefs_win.h"
#include <shellapi.h>

void blanks_pol_act_wnd::on_bn_generate_clicked() {
    if (QMessageBox::question(this,
                              "Данные будут изменены",
                              "Внимание!\nПосле этой операции все выбранные бланки полисов будут помечены как переданные в ПВП и больше передаваться не будут.\n"
                              "(при необходимости их можно выгрузить задним числом.)\n\n"
                              "Сформировать акт передачи бланков полисов?",
                              QMessageBox::Yes|QMessageBox::Cancel,
                              QMessageBox::Yes)==QMessageBox::Cancel) {
        return;
    }

    // получим код пункта выдачи
    QString sql_pnt = "select point_regnum "
                      "  from points "
                      " where id=" + QString::number(ui->combo_point->currentData().toInt()) + " ";
    QSqlQuery *query_pnt = new QSqlQuery(db);
    if (!mySQL.exec(this, sql_pnt, "Получим код ПВП", *query_pnt, true, db, data_app)) {
        QMessageBox::warning(this, "Операция отменена",
                             "При попытке получить код ПВП произошла непредвиденная ошибка.\n\n"
                             "Изменения отменены.");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    query_pnt->next();
    QString point_code = query_pnt->value(0).toString();
    delete query_pnt;

    // подготовка данных для заполнения полей
    s_data_blanks_pol_act data_act;
    data_act.act_num = " ??? ";
    data_act.act_date = QDate::currentDate();
    data_act.sender = "головной офис";
    data_act.sender_post = data_app.filial_director + "  -  " + data_app.filial_director_2;
    data_act.sender_fio = data_app.filial_director_fio;
    data_act.obtainer = ui->combo_point->currentText();
    data_act.obtainer_post = "инспектор";
    data_act.obtainer_fio = ui->combo_obtainer->currentText();

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

    // создадим запись акта
    QString sql_act = "insert into blanks_pol_acts (id_point, act_date, act_num, status, id_sender, date_add, id_obtainer) "
                      " values (" + QString::number(ui->combo_point->currentData().toInt()) + ", "
                                " '" + data_act.act_date.toString("yyyy-MM-dd") + "', "
                                " '" + data_act.act_num + "', "
                                " 1, "
                                " 0, "
                                " CURRENT_DATE, "
                                " " + QString::number(ui->combo_obtainer->currentData().toInt()) + ") "
                      " returning id ; ";
    QSqlQuery *query_act = new QSqlQuery(db);
    if (!mySQL.exec(this, sql_act, "Добавим запись акта", *query_act, true, db, data_app)) {
        QMessageBox::warning(this, "Операция отменена",
                             "При попытке создать новую запись акта произошла непредвиденная ошибка.\n\n"
                             "Изменения отменены.");
        this->setCursor(Qt::ArrowCursor);
        db.rollback();
        return;
    }
    query_act->next();
    data_act.id = query_act->value(0).toInt();
    delete query_act;


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


    if (ui->ch_filter->isChecked()) {
        // добавим только выделенные строки
        QModelIndexList list = ui->tab_blanks_pol->selectionModel()->selectedIndexes();

        int n = 0;
        for (int i=list.count()-1; i>=0; i--) {
            QModelIndex index = list.at(i);
            if (index.column()!=0)
                continue;

            int id_blank_pol   = model_phones.data(model_phones.index(index.row(), 0), Qt::EditRole).toInt();
            QString pol_ser    = model_phones.data(model_phones.index(index.row(), 1), Qt::EditRole).toString();
            QString pol_num    = model_phones.data(model_phones.index(index.row(), 2), Qt::EditRole).toString();
            QString enp        = model_phones.data(model_phones.index(index.row(), 3), Qt::EditRole).toString();
            int id_person      = model_phones.data(model_phones.index(index.row(), 4), Qt::EditRole).toInt();
            QString fam        = model_phones.data(model_phones.index(index.row(), 5), Qt::EditRole).toString();
            QString im         = model_phones.data(model_phones.index(index.row(), 6), Qt::EditRole).toString();
            QString ot         = model_phones.data(model_phones.index(index.row(), 7), Qt::EditRole).toString();
            QDate date_birth   = model_phones.data(model_phones.index(index.row(), 8), Qt::EditRole).toDate();
            int id_act2        = model_phones.data(model_phones.index(index.row(), 9), Qt::EditRole).toInt();
            QDate act_date2    = model_phones.data(model_phones.index(index.row(),10), Qt::EditRole).toDate();
            int id_point       = model_phones.data(model_phones.index(index.row(),11), Qt::EditRole).toInt();
            QString point_name = model_phones.data(model_phones.index(index.row(),12), Qt::EditRole).toString();
            n++;

            QString s_row_new = s_row;
            s_row_new = s_row_new.
                    replace("#NN#",   QString::number(n)).
                    replace("#ENP#",  enp).
                    replace("#FAM#",  fam).
                    replace("#IM#",   im).
                    replace("#OT#",   ot).
                    replace("#DR#",   date_birth.toString("d.MM.yyyy"));
            s_content_new += s_row_new;


            // добавим связь бланка полиса с актом
            QString sql_pol = "update blanks_pol "
                              "   set id_blanks_pol_act=" + QString::number(data_act.id) + ", "
                              "       id_point=" + QString::number(ui->combo_point->currentData().toInt()) + " "
                              " where id=" + QString::number(id_blank_pol) + " ; ";
            QSqlQuery *query_pol = new QSqlQuery(db);
            if (!mySQL.exec(this, sql_pol, "Добавим связку полиса с актом", *query_pol, true, db, data_app)) {
                QMessageBox::warning(this, "Операция отменена",
                                     "При попытке добавить связку полиса с актом произошла непредвиденная ошибка.\n\n"
                                     "Изменения отменены.");
                this->setCursor(Qt::ArrowCursor);
                db.rollback();
                return;
            }
            delete query_pol;


            ui->lab_cnt->setText(QString::number(n));
            QApplication::processEvents();
        }

    } else {
        // добавим все строки выборки
        QSqlQuery *query = new QSqlQuery(db);
        if (!mySQL.exec(this, sql_tab, "Повторно запросим данные бланков", *query, true, db, data_app))
            QMessageBox::warning(this, "Операция отменена", "При повторном запросе данных бланков произошла непредвиденная ошибка.\n\nОперация пропущена.");

        int n = 0;
        while (query->next()) {
            int id_blank_pol   = query->value( 0).toInt();
            QString pol_ser    = query->value( 1).toString();
            QString pol_num    = query->value( 2).toString();
            QString enp        = query->value( 3).toString();
            int id_person      = query->value( 4).toInt();
            QString fam        = query->value( 5).toString();
            QString im         = query->value( 6).toString();
            QString ot         = query->value( 7).toString();
            QDate date_birth   = query->value( 8).toDate();
            int id_act2        = query->value( 9).toInt();
            QDate act_date2    = query->value(10).toDate();
            int id_point       = query->value(11).toInt();
            QString point_name = query->value(12).toString();
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


            // добавим связь бланка полиса с актом
            QString sql_pol = "update blanks_pol "
                              "   set id_blanks_pol_act=" + QString::number(data_act.id) + ", "
                              "       id_point=" + QString::number(ui->combo_point->currentData().toInt()) + " "
                              " where id=" + QString::number(id_blank_pol) + " ; ";
            QSqlQuery *query_pol = new QSqlQuery(db);
            if (!mySQL.exec(this, sql_pol, "Добавим связку полиса с актом", *query_pol, true, db, data_app)) {
                QMessageBox::warning(this, "Операция отменена",
                                     "При попытке добавить связку полиса с актом произошла непредвиденная ошибка.\n\n"
                                     "Изменения отменены.");
                this->setCursor(Qt::ArrowCursor);
                db.rollback();
                return;
            }
            delete query_pol;


            ui->lab_cnt->setText(QString::number(n));
            QApplication::processEvents();
        }
        delete query;
    }

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
    QString fname_res = data_app.path_out + "_TRANSFER_ACTS_/" +
            QDate::currentDate().toString("yyyy-MM-dd") + " - " + data_app.filial_name + " - rep_blanks_pol_act - " + ui->combo_point->currentData().toString() + ".ods";

    QFile::remove(fname_res);
    while (QFile::exists(fname_res)) {
        if (QMessageBox::warning(this,
                                 "Ошибка сохранения акта приёма-передачи бланков полисов",
                                 "Не удалось сохранить форму передаточного акта бланков полисов в ПВП: \n" + fname_template +
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
        QMessageBox::warning(this, "Ошибка копирования отчёта", "Не удалось скопировать форму передаточного акта бланков полисов в ПВП: \n" + fname_template +
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
                             "При добавлении нового контента в форму передаточного акта бланков полисов в ПВП произошла непредвиденная ошибка\n\n"
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
    accept();
}

void blanks_pol_act_wnd::on_ch_filter_clicked() {
    ui->pan_filter->setVisible(ui->ch_filter->isChecked());
    ui->tab_blanks_pol->clearSelection();

    if (!ui->ch_filter->isChecked()) {
        ui->lab_cnt->setText(QString::number(model_phones.rowCount()));
    } else {
        //ui->tab_blanks_pol->clearSelection();
        ui->lab_cnt->setText(QString::number(ui->tab_blanks_pol->selectionModel()->selectedIndexes().count()/13));
    }
}

void blanks_pol_act_wnd::on_bn_filter_all_clicked() {
    ui->tab_blanks_pol->selectAll();
}

void blanks_pol_act_wnd::on_bn_filter_none_clicked() {
    ui->tab_blanks_pol->clearSelection();
}

void blanks_pol_act_wnd::on_tab_blanks_pol_clicked(const QModelIndex &index) {
    if (!ui->ch_filter->isChecked()) {
        ui->lab_cnt->setText(QString::number(model_phones.rowCount()));
    } else {
        //ui->tab_blanks_pol->clearSelection();
        ui->lab_cnt->setText(QString::number(ui->tab_blanks_pol->selectionModel()->selectedIndexes().count()/13));
    }
}

void blanks_pol_act_wnd::on_date_back_from_dateChanged(const QDate &date) {
    if ( ui->date_back_from->date()>QDate::currentDate() ) {
        ui->date_back_from->setDate(QDate::currentDate());
    }
    if ( ui->date_back_from->date()>ui->date_back_to->date() ||
         !ui->ch_to->isChecked() ) {
        ui->date_back_to->setDate(ui->date_back_from->date());
    }
    int days = ui->date_back_from->date().daysTo(ui->date_back_to->date()) +1;
    ui->lab_days->setText(" " + QString::number(days) + " дней");

    on_bn_gen_list_blanks_clicked();
}

void blanks_pol_act_wnd::on_date_back_to_dateChanged(const QDate &date) {
    if ( ui->date_back_to->date()>QDate::currentDate() ) {
        ui->date_back_to->setDate(QDate::currentDate());
    }
    if ( ui->date_back_from->date()>ui->date_back_to->date() ||
         !ui->ch_to->isChecked() ) {
        ui->date_back_to->setDate(ui->date_back_from->date());
    }
    int days = ui->date_back_from->date().daysTo(ui->date_back_to->date()) +1;
    ui->lab_days->setText(" " + QString::number(days) + " дней");

    on_bn_gen_list_blanks_clicked();
}

void blanks_pol_act_wnd::on_ch_to_clicked(bool checked) {
    ui->date_back_to->setEnabled(checked);
    ui->date_back_from->setDate(QDate::currentDate().addMonths(-1));
    ui->date_back_to->setDate(QDate::currentDate());
    if (!checked) {
        ui->date_back_to->setDate(ui->date_back_from->date());
    }
    on_bn_gen_list_blanks_clicked();
}

void blanks_pol_act_wnd::on_combo_point_currentIndexChanged(const QString &arg1) {
    refresh_combo_obtainers(ui->combo_point->currentData().toInt());
}
