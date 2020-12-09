#include "disp_plans_wnd.h"
#include "ui_disp_plans_wnd.h"

disp_plans_wnd::disp_plans_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent)
   : db(db), data_app(data_app), settings(settings), QDialog(parent), ui(new Ui::disp_plans_wnd)
{
    ui->setupUi(this);

    get_year_w = NULL;
    show_tab_w = NULL;

    this->resize(1001, 800);

    refresh_disp_plans_tab();
}

disp_plans_wnd::~disp_plans_wnd() {
    delete ui;
}

void disp_plans_wnd::on_bn_close_clicked() {
    close();
}

void disp_plans_wnd::on_bn_refresh_clicked() {
    refresh_disp_plans_tab();
}

void disp_plans_wnd::refresh_disp_plans_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    QString persons_sql = "select q_all.year, q_all.quarter, q_all.age, q_all.quart, q_all.categ, 'план:'||coalesce(q_all.cnt, 0)||', оповещено:'||coalesce(q_tel.cnt, 0)||', прошло:'||coalesce(cast(q_set.cnt as text), '?') "
                          "  from "
                          "  ( select year, quarter, age, year||' год, '||quarter||' квартал' as quart, case age when 1 then 'дети' when 2 then 'взрослые' else 'профосмотр' end as categ , count(*) cnt "
                          "      from clinic.disp_plans "
                          "     group by year, quarter, age "
                          "     order by year desc, quarter, age "
                          "  ) q_all "
                          "  left join "
                          "  ( select year||' год, '||quarter||' квартал' as quart, case age when 1 then 'дети' when 2 then 'взрослые' else 'профосмотр' end as categ , count(*) cnt "
                          "      from clinic.disp_plans "
                          "     where status=-1 "
                          "     group by year, quarter, age "
                          "     order by year desc, quarter, age "
                          "  ) q_tel on (q_all.quart=q_tel.quart and q_all.categ=q_tel.categ) "
                          "  left join "
                          "  ( select year||' год, '||quarter||' квартал' as quart, case age when 1 then 'дети' when 2 then 'взрослые' else 'профосмотр' end as categ , count(*) cnt "
                          "      from clinic.disp_plans "
                          "     where status=1 "
                          "     group by year, quarter, age "
                          "     order by year desc, quarter, age "
                          "  ) q_set on (q_all.quart=q_set.quart and q_all.categ=q_set.categ) "
                          " order by q_all.year desc, q_all.age, q_all.quarter ; ";

    model_disp_plans.setQuery(persons_sql,db);
    QString err2 = model_disp_plans.lastError().driverText();

    // подключаем модель из БД
    ui->tab_disp_plans->setModel(&model_disp_plans);

    // обновляем таблицу
    ui->tab_disp_plans->reset();

    // задаём ширину колонок
    ui->tab_disp_plans->setColumnWidth( 0,  1);     // q_all.year,
    ui->tab_disp_plans->setColumnWidth( 1,  1);     // q_all.quarter,
    ui->tab_disp_plans->setColumnWidth( 2,  1);     // q_all.age,
    ui->tab_disp_plans->setColumnWidth( 3,150);     // q_all.quart,
    ui->tab_disp_plans->setColumnWidth( 4,100);     // q_all.categ,
    ui->tab_disp_plans->setColumnWidth( 5,250);     // 'план:'||coalesce(q_all.cnt, 0)||', оповещено:'||coalesce(q_tel.cnt, 0)||', прошло:'||coalesce(cast(q_set.cnt as text), '?')

    // правим заголовки
    model_disp_plans.setHeaderData( 3, Qt::Horizontal, ("период диспансеризации"));
    model_disp_plans.setHeaderData( 4, Qt::Horizontal, ("категория"));
    model_disp_plans.setHeaderData( 5, Qt::Horizontal, ("число пациентов"));
    ui->tab_disp_plans->repaint();

    this->setCursor(Qt::ArrowCursor);
}




bool disp_plans_wnd::gen_disp_plan(int age) {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных? Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return false;
    }

    // запросим год диспансеризации
    delete get_year_w;
    get_year_w = new get_year_wnd("Год/квартал диспансеризации",
                                  "Выберите год/квартал, на который надо сформировать план по диспансеризации взрослых застрахованных.",
                                  false, true, QDate::currentDate().year(), (QDate::currentDate().month()-1)/3 +1, db, data_app, settings, this);
    if (!get_year_w->exec())
        return false;

    int year = get_year_w->year;
    int quarter = get_year_w->quarter;

    db.transaction();

    if ( quarter==1 || quarter==2 || quarter==3 || quarter==4 ) {
        // это квартальный план
        if ( year<QDate::currentDate().year()
             || (year==QDate::currentDate().year() && quarter<((QDate::currentDate().month()-1)/3 +1) ) ) {
            db.rollback();
            QMessageBox::critical(this, "Ошибка в данных", "Выбранный год или квартал уже закончился.\nОтмена операции.");
            return false;
        }
        // проверим, есть ли готовый план на квартал
        QString sql_plan = "select quarter, count(*) as cnt "
                           "  from clinic.disp_plans "
                           " where year=" + QString::number(year) + " "
                           "   and quarter=" + QString::number(quarter) + " "
                           "   and age=" + QString::number(age) + " "
                           " group by quarter "
                           " order by quarter ; ";
        QSqlQuery *query_plan = new QSqlQuery(db);
        bool res_plan = query_plan->exec(sql_plan);
        QString err_plan = db.lastError().databaseText();
        if (!res_plan) {
            delete query_plan;
            db.rollback();
            QMessageBox::warning(this, "Ошибка при проверке существования плана диспансеризации",
                                 "При проверке существования плана диспансеризации произошла ошибка!\n\n"
                                 + sql_plan + "\n\n"
                                 "Операция отменена.");
            return false;
        }
        int cnt = 0;
        QString msg = QString("В базе данных уже есть план по ") + QString(age==1 ? "диспансеризации застрахованных детей" : (age==2 ? "диспансеризации взрослых застрахованных" : "прохождению профосмотров")) + " на " + QString::number(quarter) + " квартал " + QString::number(year) + " года. \n\n";
        while (query_plan->next()) {
            cnt += query_plan->value(1).toInt();
            msg += " - " + QString::number(query_plan->value(0).toInt()) + " квартал - " + QString::number(query_plan->value(1).toInt()) + " человек;\n";
        }
        delete query_plan;

        if ( cnt>0 ) {
            if ( QMessageBox::question(this, "Нужно подтверждение",
                                       msg + "\nДля повторного формирования плана по " + QString(age==1 ? "диспансеризации застрахованных детей" : (age==2 ? "диспансеризации взрослых застрахованных" : "прохождению профосмотров")) + " на " + QString::number(quarter) + " квартал " + QString::number(year) + " года надо будет удалить старую версию. "
                                       "При этом список направленных на диспансеризацию может измениться.\n"
                                       "Это необратимая операция.\n\n"
                                       "Продолжить?", QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel)==QMessageBox::Yes ) {
                // удалим старый план диспансеризации
                QString sql_del = "delete from clinic.disp_plans "
                                  " where year=" + QString::number(year) + " "
                                  "   and quarter=" + QString::number(quarter) + " "
                                  "   and age=" + QString::number(age) + " ; ";
                QSqlQuery *query_del = new QSqlQuery(db);
                bool res_del = query_del->exec(sql_del);
                QString err_del = db.lastError().databaseText();
                if (!res_del) {
                    delete query_del;
                    db.rollback();
                    QMessageBox::warning(this, "Ошибка при попытке удалить план диспансеризации",
                                         "При попытке удалить план диспансеризации произошла ошибка!\n\n"
                                         + sql_del + "\n\n"
                                         "Операция отменена.");
                    return false;
                }
                delete query_del;

            } else {
                db.rollback();
                return false;
            }
        }

    } else {
        // зто поквартальный годовой план
        if (year<QDate::currentDate().year()) {
            QMessageBox::critical(this, "Ошибка в данных", "Выбранный год уже закончился.\nОтмена операции.");
            return false;
        }
        // проверим, есть ли готовый план
        QString sql_plan = "select quarter, count(*) as cnt "
                           "  from clinic.disp_plans "
                           " where year=" + QString::number(year) + " "
                           "   and age=" + QString::number(age) + " "
                           " group by quarter "
                           " order by quarter ; ";
        QSqlQuery *query_plan = new QSqlQuery(db);
        bool res_plan = query_plan->exec(sql_plan);
        QString err_plan = db.lastError().databaseText();
        if (!res_plan) {
            delete query_plan;
            QMessageBox::warning(this, "Ошибка при проверке существования плана диспансеризации",
                                 "При проверке существования плана диспансеризации произошла ошибка!\n\n"
                                 + sql_plan + "\n\n"
                                 "Операция отменена.");
            db.rollback();
            return false;
        }
        int cnt = 0;
        QString msg = QString("В базе данных уже есть план по ") + QString(age==1 ? "диспансеризации застрахованных детей" : (age==2 ? "диспансеризации взрослых застрахованных" : "прохождению профосмотров")) + " на " + QString::number(year) + " год. \n\n";
        while (query_plan->next()) {
            cnt += query_plan->value(1).toInt();
            msg += " - " + QString::number(query_plan->value(0).toInt()) + " квартал - " + QString::number(query_plan->value(1).toInt()) + " человек;\n";
        }
        delete query_plan;

        if ( cnt>0 ) {
            if ( QMessageBox::question(this, "Нужно подтверждение",
                                       msg + "\nДля повторного формирования плана по " + QString(age==1 ? "диспансеризации застрахованных детей" : (age==2 ? "диспансеризации взрослых застрахованных" : "прохождению профосмотров")) + " на " + QString::number(year) + " год надо будет удалить старую версию. "
                                       "При этом список направленных на диспансеризацию может измениться.\n"
                                       "Это необратимая операция.\n\n"
                                       "Продолжить?", QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel)==QMessageBox::Yes ) {
                // удалим старый план диспансеризации
                QString sql_del = "delete from clinic.disp_plans "
                                  " where year=" + QString::number(year) + " "
                                  "   and age=" + QString::number(age) + " ; ";
                QSqlQuery *query_del = new QSqlQuery(db);
                bool res_del = query_del->exec(sql_del);
                QString err_del = db.lastError().databaseText();
                if (!res_del) {
                    delete query_del;
                    db.rollback();
                    QMessageBox::warning(this, "Ошибка при попытке удалить план диспансеризации",
                                         "При попытке удалить план диспансеризации произошла ошибка!\n\n"
                                         + sql_del + "\n\n"
                                         "Операция отменена.");
                    return false;
                }
                delete query_del;

            } else {
                db.rollback();
                return false;
            }
        }
    }

    // запрос числа персон, попадающих на диспансеризацию на этот год
    QString sql_cnt  = "select count(*) "
                       "  from persons e "
                       "  left join persons_assig a on(a.id_person=e.id and a.is_actual=0) "
                       " where a.code_mo is not NULL "                                       // прикреплён к МО
                       "   and st_person(e.id, '" + QString::number(year) + "-12-31') in(1,2,3,4,11,12,13,14) "    // был/будет застрахован на конец года
                       "   and extract(year from e.date_birth) in(#YEARS#) ; ";

    // запрос всех, ещё не вошедьших в план диспансеризации на этот год
    QString sql_data = "select e.id, a.code_mo, mo.name_mo, a.code_mp, a.code_ms, "
                       "       e.fam, e.im, e.ot, e.sex, e.date_birth, coalesce(p.enp, e.enp) as enp, "
                       "       coalesce(e.phone_cell, e.phone_home, e.phone_work) as phone, e.email, "
                       "       replace(coalesce(ar.subj_name,'')||','||coalesce(ar.dstr_name,'')||','||coalesce(ar.city_name,'')||','||coalesce(ar.nasp_name,'')||','||coalesce(ar.strt_name,'')||','||coalesce(ar.house,0)||' '||coalesce(ar.corp,'')||coalesce(', '||ar.quart,''), ',,,,,0', '') as addr_reg, "
                       "       replace(coalesce(al.subj_name,'')||','||coalesce(al.dstr_name,'')||','||coalesce(al.city_name,'')||','||coalesce(al.nasp_name,'')||','||coalesce(al.strt_name,'')||','||coalesce(al.house,0)||' '||coalesce(al.corp,'')||coalesce(', '||al.quart,''), ',,,,,0', '') as addr_liv, "
                       "       '-'"
                       "  from persons e "
                       "  left join clinic.disp_plans d on(d.year=" + QString::number(year) + " and e.id=d.id_person) "
                       "  left join polises p on(p.id=e.id_polis) "
                       "  left join persons_assig a on(a.id_person=e.id and a.is_actual=0) "
                       "  left join spr_medorgs mo on(mo.code_mo=a.code_mo and mo.is_head=1) "
                       "  left join addresses ar on(ar.id=e.id_addr_reg) "
                       "  left join addresses al on(al.id=e.id_addr_liv) "
                       " where a.code_mo is not NULL "                                       // прикреплён к МО
                       "   and coalesce(p.enp, e.enp) is not NULL "                            // есть ЕНП
                       "   and d.id is NULL "                                                // ещё не включён в план
                       "   and st_person(e.id, '" + QString::number(year) + "-12-31') in(1,2,3,4,11,12,13,14) "    // был/будет застрахован на конец года
                       "   and extract(year from e.date_birth) in(#YEARS#) "                 // родился в один из годов диспансеризации
                       " order by a.code_mo, a.code_ms, e.fam, e.im, e.ot, e.date_birth ; ";
    // выберем годы
    QString s_years;
    if (age==1) {
        for (int i=0; i<18; i++) {
            if (i>0)  s_years += ",";
            s_years += QString::number(year - 17 + i);
        }
    }
    else if (age==2) {
        for (int i=0; i<50; i++) {
            if (i>0)  s_years += ",";
            s_years += QString::number(year - 18 - 3*i);
        }
    }
    else {
        for (int i=0; i<50; i++) {
            if (i>0)  s_years += ",";
            s_years += QString::number(year - 18 - 3*i -1);
            s_years += ",";
            s_years += QString::number(year - 18 - 3*i -2);
        }
    }
    sql_cnt  = sql_cnt .replace("#YEARS#", s_years);
    sql_data = sql_data.replace("#YEARS#", s_years);


    // ------------------------------- //
    // собственно подсчёт числа персон //
    // ------------------------------- //
    QSqlQuery *query_cnt = new QSqlQuery(db);
    bool res_cnt = query_cnt->exec(sql_cnt);
    QString err_cnt = db.lastError().databaseText();
    if (!res_cnt) {
        delete query_cnt;
        QMessageBox::warning(this, "Ошибка при подсчёте числа персон, попадающих на диспансеризацию на этот год",
                             "При подсчёте числа персон, попадающих на диспансеризацию на этот год произошла ошибка!\n\n"
                             + sql_cnt + "\n\n"
                             "Операция отменена.");
        db.rollback();
        return false;
    }
    query_cnt->next();
    int cnt_to_process = query_cnt->value(0).toInt();
    delete query_cnt;


    // -------------------------------------- //
    // собственно формирование плана и отчёта //
    // -------------------------------------- //
    QSqlQuery *query_data = new QSqlQuery(db);
    bool res_data = query_data->exec(sql_data.replace("\n", ""));
    QString err_data = db.lastError().databaseText();
    if (!res_data) {
        delete query_data;
        QMessageBox::warning(this, "Ошибка при попытке получить данные на план диспансеризации",
                             "При попытке получить данные на план диспансеризации произошла ошибка!\n\n"
                             + sql_data + "\n\n"
                             "Операция отменена.");
        db.rollback();
        return false;
    }
    // подсчитаем число застрахованных на квартал
    int cnt1=0, cnt2=0, cnt3=0, cnt4=0;
    switch (quarter) {
        case 1:
            cnt1 = ((cnt_to_process)*26)/100;
            break;
        case 2:
            cnt2 = ((cnt_to_process)*26)/100;
            break;
        case 3:
            cnt3 = ((cnt_to_process)*26)/100;
            break;
        case 4:
            cnt4 = cnt_to_process - ((cnt_to_process)*26)/100 - ((cnt_to_process)*26)/100 - ((cnt_to_process)*26)/100;
            break;
        default:
            cnt1 = ((cnt_to_process)*26)/100;
            cnt2 = ((cnt_to_process)*26)/100;
            cnt3 = ((cnt_to_process)*26)/100;
            cnt4 = cnt_to_process - cnt1 - cnt2 - cnt3;
            break;
    }

    // переберём нужное количество строк запроса
    int n = 0;
    while (query_data->next()) {
        n++;
        if (cnt1>0)      { cnt1--; quarter = 1; }
        else if (cnt2>0) { cnt2--; quarter = 2; }
        else if (cnt3>0) { cnt3--; quarter = 3; }
        else if (cnt4>0) { cnt4--; quarter = 4; }
        else  break;

        int     id_person = query_data->value( 0).toInt();      //  id_person,
        QString code_mo   = query_data->value( 1).toString();   //  a.code_mo,
        QString name_mo   = query_data->value( 2).toString();   //  mo.name_mo,
        QString name_mp   = query_data->value( 3).toString();   //  a.code_mp,
        QString name_ms   = query_data->value( 4).toString();   //  a.code_ms,
        QString fam       = query_data->value( 5).toString();   //  e.fam,
        QString im        = query_data->value( 6).toString();   //  e.im,
        QString ot        = query_data->value( 7).toString();   //  e.ot,
        int     sex       = query_data->value( 8).toInt();      //  e.sex,
        QDate   date_birth= query_data->value( 9).toDate();     //  e.date_birth,
        QString enp       = query_data->value(10).toString();   //  coalesce(p.enp, e.enp) as enp,
        QString phone     = query_data->value(11).toString();   //  coalesce(e.phone_cell, e.phone_home, e.phone_work) as phone,
        QString email     = query_data->value(12).toString();   //  e.email,
        QString addr_r    = query_data->value(13).toString();   //  replace(coalesce(ar.subj_name,'')||','||coalesce(ar.dstr_name,'')||','||coalesce(ar.city_name,'')||','||coalesce(ar.nasp_name,'')||','||coalesce(ar.strt_name,'')||','||coalesce(ar.house,0)||' '||coalesce(ar.corp,'')||coalesce(', '||ar.quart,''), ',,,,,0', '') as addr_reg,
        QString addr_l    = query_data->value(14).toString();   //  replace(coalesce(al.subj_name,'')||','||coalesce(al.dstr_name,'')||','||coalesce(al.city_name,'')||','||coalesce(al.nasp_name,'')||','||coalesce(al.strt_name,'')||','||coalesce(al.house,0)||' '||coalesce(al.corp,'')||coalesce(', '||al.quart,''), ',,,,,0', '') as addr_liv,
        int     age_pers  = age;
        /*if (age==0)
            // определим возрастную категорию по дате рождения и году диспансеризации
            age_pers = (year-date_birth.year()>18 ? 1 : 2);*/

        // добавим застрахованного в план
        QString sql_ins = "insert into clinic.disp_plans(id_person, year, quarter, fam, im, ot, sex, date_birth, enp, age, status) "
                          "values (" + QString::number(id_person) + ", "
                          "        " + QString::number(year) + ", "
                          "        " + QString::number(quarter) + ", "
                          "       '" + fam + "', "
                          "       '" + im  + "', "
                          "       '" + ot  + "', "
                          "        " + QString::number(sex) + ", "
                          "       '" + date_birth.toString("yyyy-MM-dd") + "', "
                          "       '" + enp + "', "
                          "        " + QString::number(age) + ", "
                          "        0 "
                          "); ";
        QSqlQuery *query_ins = new QSqlQuery(db);
        bool res_ins = query_ins->exec(sql_ins);
        QString err_ins = db.lastError().databaseText();
        if (!res_ins) {
            delete query_ins;
            QMessageBox::warning(this, "Ошибка при попытке добавить строку в план диспансеризации",
                                 "При попытке добавить строку в план диспансеризации произошла ошибка!\n\n"
                                 + sql_ins + "\n\n"
                                 "Операция отменена.");
            db.rollback();
            return false;
        }
    }

    delete query_data;
    db.commit();

    return true;

    //delete show_tab_w;
    //show_tab_w = new show_tab_wnd("Список застрахованных на диспансеризацию на " + QString::number(year) + " год - взрослые (с 18 лет)", sql, db, data_app, this);
    //show_tab_w->exec();

}











// взрослые
void disp_plans_wnd::on_bn_gen_disp_plan_clicked() {
    if (!gen_disp_plan(2))
        QMessageBox::warning(this, "Отказ при генерации плана", "Не удалось сформировать новый план диспансеризации взрослых застрахованных (18 лет и старше).");
    else
        refresh_disp_plans_tab();
}
// дети (до 18)
void disp_plans_wnd::on_bn_gen_disp_plan_18_clicked() {
    if (!gen_disp_plan(1))
        QMessageBox::warning(this, "Отказ при генерации плана", "Не удалось сформировать новый план диспансеризации застрахованных детей (до 18 лет).");
    else
        refresh_disp_plans_tab();
}
// профосмотры
void disp_plans_wnd::on_bn_gen_prof_plan_clicked() {
    if (!gen_disp_plan(0))
        QMessageBox::warning(this, "Отказ при генерации плана", "Не удалось сформировать новый план профосмотров взрослых застрахованных (18 лет и старше).");
    else
        refresh_disp_plans_tab();
}

void disp_plans_wnd::on_bn_delete_data_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_disp_plans || !ui->tab_disp_plans->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_disp_plans->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Удалить выбранную запись из базы?\n\n"
                                  "При этом будут удалены все данные о направлении соответствующих застрахованных на диспансеризацию/профосмотр.\n"
                                  "Это необратимое действие.\n",
                                  QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes)==QMessageBox::Cancel) {
            return;
        }

        QModelIndex index = indexes.front();

        int year    = model_disp_plans.data(model_disp_plans.index(index.row(), 0), Qt::EditRole).toInt();
        int quarter = model_disp_plans.data(model_disp_plans.index(index.row(), 1), Qt::EditRole).toInt();
        int age     = model_disp_plans.data(model_disp_plans.index(index.row(), 2), Qt::EditRole).toInt();

        db.transaction();
        // удалим записи
        QString sql = "delete from clinic.disp_plans "
                      " where year="    + QString::number(year)    + " "
                      "   and quarter=" + QString::number(quarter) + " "
                      "   and age="     + QString::number(age)     + " ; ";
        QSqlQuery *query2 = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "ОУдаление записей в плане диспансеризации", *query2, true, db, data_app)) {
            db.rollback();
            QMessageBox::critical(this, "Непредвиденная ошибка", "При попытке удалить записи в плане диспансеризации произошла неожиданная ошибка.\n\nУдаление отменено.");
            delete query2;
            return;
        }
        delete query2;

        db.commit();

        refresh_disp_plans_tab();

    } else {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
    }
}

void disp_plans_wnd::show_disp_plan(int year, int quarter, int age) {
    QString
    sql_data = "select e.id, d.year, d.quarter, d.age, "
               "       a.code_mo, mo.name_mo, a.code_mp, a.code_ms, \n"
               "       e.fam, e.im, e.ot, e.date_birth, coalesce(p.enp, e.enp) as enp, \n"
               "       coalesce(e.phone_cell, e.phone_home, e.phone_work) as phone, e.email, \n"
               "       replace(coalesce(ar.subj_name,'')||','||coalesce(ar.dstr_name,'')||','||coalesce(ar.city_name,'')||','||coalesce(ar.nasp_name,'')||','||coalesce(ar.strt_name,'')||','||coalesce(ar.house,0)||' '||coalesce(ar.corp,'')||coalesce(', '||ar.quart,''), ',,,,,0', '') as addr_reg, \n"
               "       replace(coalesce(al.subj_name,'')||','||coalesce(al.dstr_name,'')||','||coalesce(al.city_name,'')||','||coalesce(al.nasp_name,'')||','||coalesce(al.strt_name,'')||','||coalesce(al.house,0)||' '||coalesce(al.corp,'')||coalesce(', '||al.quart,''), ',,,,,0', '') as addr_liv \n"
               "  from clinic.disp_plans d \n"
               "  left join persons e on(e.id=d.id_person) \n"
               "  left join polises p on(p.id=e.id_polis) \n"
               "  left join persons_assig a on(a.id_person=e.id and a.is_actual=0) \n"
               "  left join spr_medorgs mo on(mo.code_mo=a.code_mo and mo.is_head=1) \n"
               "  left join addresses ar on(ar.id=e.id_addr_reg) \n"
               "  left join addresses al on(al.id=e.id_addr_liv) \n"
               " where d.year=" + QString::number(year) + " \n"
               "   and d.age=" + QString::number(age) + " \n"
               "   and a.code_mo is not NULL \n"                // прикреплён к МО
               "   and coalesce(p.enp, e.enp) is not NULL ";    // есть ЕНП

    if ( quarter==1 || quarter==2 || quarter==3 || quarter==4 )
        sql_data +=  "   and d.quarter=" + QString::number(quarter) + " \n";

    sql_data+= " order by a.code_mo, a.code_ms, e.fam, e.im, e.ot, e.date_birth ; ";

    QString qy_str = "";
    if ( quarter==1 || quarter==2 || quarter==3 || quarter==4 )
        qy_str += QString::number(quarter) + " квартал ";
    qy_str += QString::number(year);
    if ( quarter==1 || quarter==2 || quarter==3 || quarter==4 )
        qy_str += " года ";
    else  qy_str += " год ";

    delete show_tab_w;
    show_tab_w = new show_tab_wnd("Список застрахованных на диспансеризацию на " + qy_str + QString(age==1 ? QString(" - дети (до 18 лет) ") : QString(age==2 ? " - взрослые (с 18 лет)" : " - профосмотр")),
                                  sql_data,
                                  db,
                                  data_app,
                                  this,
                                  data_app.path_out + "_ДИСПАНСЕРИЗАЦИЯ_/");
    show_tab_w->exec();
}

void disp_plans_wnd::on_bn_show_disp_plan_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    // запросим год диспансеризации
    delete get_year_w;
    get_year_w = new get_year_wnd("Год/квартал диспансеризации",
                                  "Выберите год/квартал, на который надо сформировать план по диспансеризации взрослых застрахованных.",
                                  false, true, QDate::currentDate().year(), (QDate::currentDate().month()-1)/3 +1, db, data_app, settings, this);
    if (!get_year_w->exec())
        return;

    int year = get_year_w->year;
    int quarter = get_year_w->quarter;

    show_disp_plan(year, quarter, 2);
}

void disp_plans_wnd::on_bn_show_disp_plan_18_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    // запросим год диспансеризации
    delete get_year_w;
    get_year_w = new get_year_wnd("Год/квартал диспансеризации",
                                  "Выберите год/квартал, на который надо сформировать план по диспансеризации взрослых застрахованных.",
                                  false, true, QDate::currentDate().year(), (QDate::currentDate().month()-1)/3 +1, db, data_app, settings, this);
    if (!get_year_w->exec())
        return;

    int year = get_year_w->year;
    int quarter = get_year_w->quarter;

    show_disp_plan(year, quarter, 1);
}

void disp_plans_wnd::on_bn_show_prof_plan_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    // запросим год диспансеризации
    delete get_year_w;
    get_year_w = new get_year_wnd("Год/квартал диспансеризации",
                                  "Выберите год/квартал, на который надо сформировать план по профосмотрам взрослых застрахованных.",
                                  false, true, QDate::currentDate().year(), (QDate::currentDate().month()-1)/3 +1, db, data_app, settings, this);
    if (!get_year_w->exec())
        return;

    int year = get_year_w->year;
    int quarter = get_year_w->quarter;

    show_disp_plan(year, quarter, 0);
}

void disp_plans_wnd::on_bn_show_disp_plan_dead_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    // запросим год диспансеризации
    delete get_year_w;
    get_year_w = new get_year_wnd("Год/квартал диспансеризации",
                                  "Выберите год/квартал, на который надо сформировать план по диспансеризации взрослых застрахованных.",
                                  false, false, QDate::currentDate().year(), (QDate::currentDate().month()-1)/3 +1, db, data_app, settings, this);
    if (!get_year_w->exec())
        return;

    int year = get_year_w->year;
    int quarter = get_year_w->quarter;

    QString sql_data = "select e.id, e.fam, e.im, e.ot, e.date_birth, e.date_death \n"
                       "  from clinic.disp_plans d \n"
                       "       join persons e on(e.id=d.id_person) \n"
                       " where st_person(e.id, NULL)=-10 \n"
                       "   and d.year=" + QString::number(year) + " \n"
                       /*"   and d.age=" + QString::number(age) + " \n"*/;

    if ( quarter==1 || quarter==2 || quarter==3 || quarter==4 )
        sql_data +=  "   and d.quarter=" + QString::number(quarter) + " \n";

    sql_data+= " order by e.fam, e.im, e.ot, e.date_birth ; ";

    delete show_tab_w;
    show_tab_w = new show_tab_wnd("Список умерших застрахованных из списка на диспансеризацию", sql_data, db, data_app, this, data_app.path_out + "_ДИСПАНСЕРИЗАЦИЯ_/");
    show_tab_w->exec();
}

void disp_plans_wnd::on_bn_gen_disp_count4mo_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    // запросим год диспансеризации
    delete get_year_w;
    get_year_w = new get_year_wnd("Год/квартал диспансеризации",
                                  "Выберите год/квартал, на который надо сформировать отчёт о численности диспансеризации застрахованных в разрезе МО.",
                                  false, false, QDate::currentDate().year(), (QDate::currentDate().month()-1)/3 +1, db, data_app, settings, this);
    if (!get_year_w->exec())
        return;

    int age = ui->rb_age1->isChecked() ? 1 : 2;
    int year = get_year_w->year;
    int quarter = get_year_w->quarter;


    // --------------------------- //
    // получение данных для отчёта //
    // --------------------------- //
    QString sql = "select mo.code_mo, mo.name_mo, count(*) \n"
                  "  from clinic.disp_plans d \n"
                  "       join persons e on(e.id=d.id_person) \n"
                  "  left join persons_assig a on(a.id_person=e.id and a.is_actual=0) \n"
                  "  left join spr_medorgs mo on(mo.code_mo=a.code_mo and  mo.is_head=1) \n"
                  " where d.year=" + QString::number(year) + " \n"
                  "   and a.code_mo is not NULL \n"                                       // прикреплён к МО
                  "   and d.age=" + QString::number(age) + " \n";

    if ( quarter==1 || quarter==2 || quarter==3 || quarter==4 )
        sql +=    "   and d.quarter=" + QString::number(quarter) + " \n";

    sql +=        " group by mo.code_mo, mo.name_mo \n"
                  " order by mo.code_mo, mo.name_mo ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString("Численности застрахованных, направленных на диспансеризацию на год/квартал в разрезе МО"), *query, true, db, data_app);
    if (!res) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        return;
    }


    this->setCursor(Qt::WaitCursor);


    // распаковка шаблона
    QString rep_folder = data_app.path_reports;
    QString tmp_folder = data_app.path_out + "_REPORTS_/temp/";
    QString rep_template = rep_folder + "rep_disp_count4mo.odt";



    if (!QFile::exists(rep_template)) {
        QMessageBox::warning(this,
                             "Шаблон не найден",
                             "Не найден шаблон печатной формы уведомления: \n" + rep_template +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    QDir tmp_dir(tmp_folder);
    if (tmp_dir.exists(tmp_folder) && !tmp_dir.removeRecursively()) {
        QMessageBox::warning(this,
                             "Ошибка при очистке папки",
                             "Не удалось очистить папку временных файлов: \n" + tmp_folder +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    tmp_dir.mkpath(tmp_folder);
    QProcess myProcess;
    QString com = "\"" + data_app.path_arch + "7z.exe\" e \"" + rep_template + "\" -o\"" + tmp_folder + "\" content.xml";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка распаковки шаблона",
                             "Не удалось распаковать файл контента шаблона\n" + rep_template + " -> content.xml"
                             "\nпечатной формы во временную папку\n" + tmp_folder +
                             "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return;
    }


    // ===========================
    // правка контента шаблона
    // ===========================
    // открытие контента шаблона
    QFile f_content(tmp_folder + "/content.xml");
    f_content.open(QIODevice::ReadOnly);
    QString s_content = f_content.readAll();
    f_content.close();
    if (s_content.isEmpty()) {
        QMessageBox::warning(this,
                             "Файл контента шаблона пустой",
                             "Файл контента шаблона пустой. Возможно шаблон был испорчен.\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    // выбор шаблона
    int pos0 = s_content.indexOf("<table:table-row table:style-name=\"Таблица1.2\">", 0);
    int pos1 = s_content.indexOf("</table:table-row>", pos0) + 18;
    QString s_row = s_content.mid(pos0, pos1-pos0);

    // правка полей контента шаблона
    QString s_content_new = s_content.left(pos0);

    QString qy_str = "";
    if ( quarter==1 || quarter==2 || quarter==3 || quarter==4 )
        qy_str += QString::number(quarter) + " квартал ";
    qy_str += QString::number(year);
    if ( quarter==1 || quarter==2 || quarter==3 || quarter==4 )
        qy_str += " года ";
    else  qy_str += " год ";

    s_content_new = s_content_new.replace("#QUART_YEAR#", qy_str);
    s_content_new = s_content_new.replace("#AGE#", age==1 ? "дети (до 18 лет)" : "взрослые (18 лет и старше)");



    int n = 0;

    // переберём районы и ЛПУ области
    while (query->next()) {
        n++;
        QString s_row_new = s_row;
        s_row_new = s_row_new.
                replace("#N#",       QString::number(n)).
                replace("#NAME_MO#", QString("(") + query->value(0).toString() + ")  " + query->value(1).toString()).
                replace("#COUNT#",   QString::number(query->value(2).toInt()));
        s_content_new += s_row_new;

        //ui->lab_cnt->setText(QString::number(n));
        //QApplication::processEvents();
    }
    s_content_new += s_content.right(s_content.size()-pos1);

    delete query;

    // сохранение контента шаблона
    QFile f_content_save(tmp_folder + "/content.xml");
    f_content_save.open(QIODevice::WriteOnly);
    f_content_save.write(s_content_new.toUtf8());
    f_content_save.close();


    // ===========================
    // архивация шаблона
    // ===========================
    QString fname_res = data_app.path_out + "_ДИСПАНСЕРИЗАЦИЯ_/" +
            QDate::currentDate().toString("yyyy-MM-dd") + "  -  " + data_app.smo_short + " - " + data_app.filial_name + "  -  Численность застрахованных, направляемых на диспансеризацию в разрезе МО - " + qy_str + ", " + QString(age==1 ? "ДЕТИ" : "ВЗРОСЛЫЕ") + ".odt";

    QFile::remove(fname_res);
    while (QFile::exists(fname_res)) {
        if (QMessageBox::warning(this,
                                 "Ошибка сохранения контента отчёта",
                                 "Не удалось сохранить контент отчёта: \n" + rep_template +
                                 "\n\nво временную папку\n" + fname_res +
                                 "\n\nПроверьте, не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                 QMessageBox::Retry|QMessageBox::Abort,
                                 QMessageBox::Retry)==QMessageBox::Abort) {
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        QFile::remove(fname_res);
    }
    if (!QFile::copy(rep_template, fname_res) || !QFile::exists(fname_res)) {
        QMessageBox::warning(this, "Ошибка копирования отчёта", "Не удалось скопировать шаблон отчёта: \n" + rep_template + "\n\nво временную папку\n" + fname_res + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    com = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname_res + "\" \"" + tmp_folder + "/content.xml\"";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка обновления контента",
                             "При добавлении нового контента в шаблон отчёта произошла непредвиденная ошибка\n\nОпреация прервана.");
    }

    // ===========================
    // собственно открытие шаблона
    // ===========================
    // открытие полученного ODT
    long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);
    this->setCursor(Qt::ArrowCursor);
    if (result<32) {
        QMessageBox::warning(this,
                             "Ошибка при открытии готового файла отчёта",
                             "готовый файл отчёта не найден:\n\n" + fname_res +
                             "\nКод ошибки: " + QString::number(result) +
                             "\n\nПопробуйте открыть этот файл вручную.\n\nЕсли файл не создан или ошибка будет повторяться - обратитесь к разработчику.");
    }
}







void disp_plans_wnd::on_bn_gen_disp_list4mo_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    // запросим год диспансеризации
    delete get_year_w;
    get_year_w = new get_year_wnd("Год/квартал диспансеризации",
                                  "Выберите год/квартал, на который надо сформировать списки застрахованных, направляемых на диспансеризацию в МО.",
                                  false, false, QDate::currentDate().year(), (QDate::currentDate().month()-1)/3 +1, db, data_app, settings, this);
    if (!get_year_w->exec())
        return;

    int age = ui->rb_age1->isChecked() ? 1 : 2;
    int year = get_year_w->year;
    int quarter = get_year_w->quarter;


    this->setCursor(Qt::WaitCursor);


    // подсчитаем МО, к которым сделоно прикрепление
    QString sql_mo = "select distinct mo.id, mo.code_mo, mo.name_mo "
                     "  from spr_medorgs mo "
                     "       join persons_assig a on( a.code_mo=mo.code_mo "
                                                    " and a.assig_date<='" + QString::number(year) + "-12-31' "
                                                    " and ( a.unsig_date is NULL "
                                                    "       or a.unsig_date>'" + QString::number(year) + "-12-31') ) ";
    QSqlQuery *query_mo = new QSqlQuery(db);
    bool res_mo = mySQL.exec(this, sql_mo, "Получение списка медорганизаций, по которым проводится диспансеризация за год", *query_mo, true, db, data_app);
    if (!res_mo) {
        QMessageBox::warning(this, "Ошибка при получении списка медорганизаций для диспансеризациии за год", "При получении списка медорганизаций для диспансеризациии за год произошла неожиданная ошибка.");
        delete query_mo;
        return;
    };

    while (query_mo->next()) {
        QString code_mo = query_mo->value(1).toString();
        QString name_mo = query_mo->value(2).toString();

        // выберем данные для отчёта
        QString sql = "select e.id, coalesce(e.fam,'')||' '||coalesce(e.im,'')||' '||coalesce(e.ot,''), e.date_birth, "
                      "       coalesce(p.enp, e.enp) as enp, "
                      "       coalesce(e.phone_cell,'')||'  '||coalesce(e.email,''), "
                      "       '('||a.code_mo||') '||mo.name_mo as name_mo, "
                      "       'адрес регистрации: '||coalesce(ar.subj_name,'')||', '||coalesce(ar.dstr_name,'')||', '||coalesce(ar.city_name,'')||', '||coalesce(ar.nasp_name,'')||', '||coalesce(ar.strt_name,'')||', '||ar.house||' '||coalesce(ar.corp,'')||' '||coalesce(', кв.'||ar.quart,'') as addr_reg, "
                      "       'адрес проживания: ' ||al.subj_name||', '||coalesce(al.dstr_name,'')||', '||coalesce(al.city_name,'')||', '||coalesce(al.nasp_name,'')||', '||coalesce(al.strt_name,'')||', '||al.house||' '||coalesce(al.corp,'')||' '||coalesce(', кв.'||al.quart,'') as addr_liv,"
                      "       coalesce(a.code_mo, '0'), coalesce(a.code_mp, '0'), coalesce(a.code_ms, '0') "

                      "  from clinic.disp_plans d \n"
                      "       join persons e on(e.id=d.id_person) \n"
                      "  left join polises p on(p.id=e.id_polis) "
                      "  left join persons_assig a on(a.id_person=e.id and a.is_actual=0) "
                      "  left join spr_medorgs mo on(mo.code_mo=a.code_mo and  mo.is_head=1) "
                      "  left join addresses ar on(ar.id=e.id_addr_reg) "
                      "  left join addresses al on(al.id=e.id_addr_liv) "
                      " where d.year=" + QString::number(year) + " \n"
                      "   and a.code_mo is not NULL \n"                                       // прикреплён к МО
                      "   and d.age=" + QString::number(age) + " \n";

        if ( quarter==1 || quarter==2 || quarter==3 || quarter==4 )
            sql +=    "   and d.quarter=" + QString::number(quarter) + " \n";

        sql +=        "   and mo.code_mo='" + code_mo + "' "
                      " order by coalesce(a.code_mo, '0'), coalesce(a.code_mp, '0'), coalesce(a.code_ms, '0'), "
                      "          e.fam, e.im, e.ot, e.date_birth ; ";


        QSqlQuery *query = new QSqlQuery(db);
        bool res = mySQL.exec(this, sql, "Формирование плана диспансеризации по МО за год", *query, true, db, data_app);
        if (!res) {
            QMessageBox::warning(this, "Не удалось построить план диспансеризации по МО за год", "При попытке построить план диспансеризации по МО за год произошла неожиданная ошибка.");
            delete query;
            return;
        };
        int n_rep = query->size();
        if (n_rep==0) {
            delete query;
            continue;
        }

        // -------------------------- //
        // собственно генерация плана //
        // -------------------------- //
        this->setCursor(Qt::WaitCursor);

        // распаковка шаблона
        QString rep_folder = data_app.path_reports;
        // распаковка шаблона
        QString tmp_folder = data_app.path_out + "_REPORTS_/temp/";
        QString rep_template = rep_folder + "rep_disp_list4mo.odt";


        if (!QFile::exists(rep_template)) {
            QMessageBox::warning(this,
                                 "Шаблон не найден",
                                 "Не найден шаблон печатной формы заявления: \n" + rep_template +
                                 "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        QDir tmp_dir(tmp_folder);
        if (tmp_dir.exists(tmp_folder) && !tmp_dir.removeRecursively()) {
            QMessageBox::warning(this,
                                 "Ошибка при очистке папки",
                                 "Не удалось очистить папку временных файлов: \n" + tmp_folder +
                                 "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        tmp_dir.mkpath(tmp_folder);
        QProcess myProcess;
        QString com = "\"" + data_app.path_arch + "7z.exe\" e \"" + rep_template + "\" -o\"" + tmp_folder + "\" content.xml";
        if (myProcess.execute(com)!=0) {
            QMessageBox::warning(this,
                                 "Ошибка распаковки шаблона",
                                 "Не удалось распаковать файл контента шаблона\n" + rep_template + " -> content.xml"
                                 "\nпечатной формы во временную папку\n" + tmp_folder +
                                 "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }

        // ===========================
        // правка контента шаблона
        // ===========================
        // открытие контента шаблона
        QFile f_content(tmp_folder + "/content.xml");
        f_content.open(QIODevice::ReadOnly);
        QString s_content = f_content.readAll();
        f_content.close();
        if (s_content.isEmpty()) {
            QMessageBox::warning(this,
                                 "Файл контента пустой",
                                 "Файл контента пустой. Возможно шаблон был испорчен.\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }

        // -----------------------
        // выделение строк справки
        // -----------------------
        int pos0 = 0;
        int pos1 = s_content.indexOf("<table:table-row table:style-name=\"Таблица1.2\">", pos0);
        int pos2 = s_content.indexOf("</table:table-row>", pos1) + 18;
        int pos3 = s_content.length();
        QString s_top   = s_content.mid(pos0, pos1-pos0);
        QString s_line1 = s_content.mid(pos1, pos2-pos1);
        QString s_down  = s_content.mid(pos2, pos3-pos2);

        // -----------------------------
        // правка полей контента шаблона
        // -----------------------------
        QString s_line = s_top;

        QString qy_str = "";
        if ( quarter==1 || quarter==2 || quarter==3 || quarter==4 )
            qy_str += QString::number(quarter) + " квартал ";
        qy_str += QString::number(year);
        if ( quarter==1 || quarter==2 || quarter==3 || quarter==4 )
            qy_str += " года ";
        else  qy_str += " год ";

        s_content = s_line.
                replace("#QUART_YEAR#", qy_str).
                replace("#CODE_MO#", code_mo).
                replace("#NAME_MO#", name_mo).
                replace("#AGE#", (age==1 ? "дети (до 18 лет)" : "взрослые (старше 18 лет)")).
                replace("#CNT#", QString::number(n_rep));

        int n = 0;
        while (query->next()) {
            n++;
            QString fio = query->value(1).toString() + ", " + query->value(2).toDate().toString("dd.MM.yyyy");
            QString enp = query->value(3).toString();
            QString phone = query->value(4).toString();
            QString medorg = query->value(5).toString();
            QString addr_reg = query->value(6).toString();
            QString addr_liv = query->value(7).toString();
            QString code_mo = query->value( 8).toString();
            QString code_mp = query->value( 9).toString();
            if (code_mp.trimmed().isEmpty())  code_mp = "0";
            QString code_ms = query->value(10).toString();
            if (code_ms.trimmed().isEmpty())  code_ms = "0";

            s_line = s_line1;
            s_content += s_line.
                    replace("#N#",       QString::number(n)).
                    replace("#FIO#",     fio).
                    replace("#ENP#",     enp).
                    replace("#PHONE#",   phone).
                    replace("#ADDRESS#", addr_reg + ";   " + addr_liv).
                    replace("#CODE_MP#", code_mp).
                    replace("#CODE_MS#", code_ms);
        }
        s_line = s_down;
        s_content += s_line/*.
                replace("#N_TAKE#", QString::number(n_take)).
                replace("#N_ZERO#", QString::number(n_zero))*/;

        // сохранение контента шаблона
        QFile f_content_save(tmp_folder + "/content.xml");
        f_content_save.open(QIODevice::WriteOnly);
        f_content_save.write(s_content.toUtf8());
        f_content_save.close();


        // ===========================
        // архивация шаблона
        // ===========================
        QString fname_res = data_app.path_out + "_ДИСПАНСЕРИЗАЦИЯ_/" +
                QDate::currentDate().toString("yyyy-MM-dd") + "  -  " + data_app.smo_short + " - " + data_app.filial_name + "  -  " + code_mo + " - план по диспансеризации на " + qy_str + " - " + (age==1 ? "дети (до 18 лет)" : "взрослые (старше 18 лет)") + ".odt";

        QFile::remove(fname_res);
        while (QFile::exists(fname_res)) {
            if (QMessageBox::warning(this,
                                     "Ошибка сохранения печатной формы",
                                     "Не удалось сохранить шаблон печатной формы: \n" + rep_template +
                                     "\n\nво временную папку\n" + fname_res +
                                     "\n\nПроверьте, не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                     QMessageBox::Retry|QMessageBox::Abort,
                                     QMessageBox::Retry)==QMessageBox::Abort) {
                this->setCursor(Qt::ArrowCursor);
                return;
            }
            QFile::remove(fname_res);
        }
        if (!QFile::copy(rep_template, fname_res) || !QFile::exists(fname_res)) {
            QMessageBox::warning(this, "Ошибка копирования шаблона", "Не удалось скопировать шаблон печатной формы: \n" + rep_template + "\n\nв целевую папку\n" + fname_res + "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        com = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname_res + "\" \"" + tmp_folder + "/content.xml\"";
        if (myProcess.execute(com)!=0) {
            QMessageBox::warning(this,
                                 "Ошибка обновления контента",
                                 "При добавлении нового контента в шаблон печатной формы заявления произошла непредвиденная ошибка\n\nОпреация прервана.");
        }
    }


    // ========================= //
    // собственно открытие папки //
    // ========================= //
    // открытие папки отчётов
    QString folder_res = data_app.path_out + "_ДИСПАНСЕРИЗАЦИЯ_";
    long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(folder_res.utf16()), 0, 0, SW_NORMAL);
    this->setCursor(Qt::ArrowCursor);
    if (result<32) {
        QMessageBox::warning(this,
                             "Ошибка при открытии папки",
                             "При попытке открыть папку сохранения отчётов произошла ошибка:\n\n" + folder_res +
                             "\nКод ошибки: " + QString::number(result) +
                             "\n\nПопробуйте открыть эту папку вручную.");
    }
}

void disp_plans_wnd::on_bn_gen_disp_calls4mo_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    // запросим год диспансеризации
    delete get_year_w;
    get_year_w = new get_year_wnd("Год/квартал информирования о диспансеризации",
                                  "Выберите год/квартал, на который надо сформировать план обзвона застрахованнных по вопросу необходимости прохождения диспансеризации в МО.",
                                  false, false, QDate::currentDate().year(), (QDate::currentDate().month()-1)/3 +1, db, data_app, settings, this);
    if (!get_year_w->exec())
        return;

    int age = ui->rb_age1->isChecked() ? 1 : 2;
    int year = get_year_w->year;
    int quarter = get_year_w->quarter;

    QString qy_str = "";
    if ( quarter==1 || quarter==2 || quarter==3 || quarter==4 )
        qy_str += QString::number(quarter) + " квартал ";
    qy_str += QString::number(year);
    if ( quarter==1 || quarter==2 || quarter==3 || quarter==4 )
        qy_str += " года ";
    else  qy_str += " год ";


    this->setCursor(Qt::WaitCursor);


    // подсчитаем МО, к которым сделоно прикрепление
    QString sql_mo = "select distinct mo.id, mo.code_mo, mo.name_mo "
                     "  from spr_medorgs mo "
                     "       join persons_assig a on( a.code_mo=mo.code_mo "
                                                    " and a.assig_date<='" + QString::number(year) + "-12-31' "
                                                    " and ( a.unsig_date is NULL "
                                                    "       or a.unsig_date>'" + QString::number(year) + "-12-31') ) ";
    QSqlQuery *query_mo = new QSqlQuery(db);
    bool res_mo = mySQL.exec(this, sql_mo, "Получение списка медорганизаций, по которым проводится диспансеризация за год", *query_mo, true, db, data_app);
    if (!res_mo) {
        QMessageBox::warning(this, "Ошибка при получении списка медорганизаций для диспансеризациии за год", "При получении списка медорганизаций для диспансеризациии за год произошла неожиданная ошибка.");
        delete query_mo;
        return;
    };

    while (query_mo->next()) {
        QString code_mo = query_mo->value(1).toString();
        QString name_mo = query_mo->value(2).toString();

        //--------------------------------- //
        // сформируем поквартальные таблицы //
        //--------------------------------- //
        QString sql = "select d.quarter, coalesce(a.code_mo, '0'), coalesce(a.code_mp, '0'), coalesce(a.code_ms, '0'), count(*) "
                      "  from clinic.disp_plans d "
                      "       join persons e on(e.id=d.id_person) "
                      "  join persons_assig a on(a.id_person=e.id and a.is_actual=0) "
                      "  left join spr_medorgs mo on(mo.code_mo=a.code_mo and  mo.is_head=1) "

                      " where d.year=" + QString::number(year) + " ";
        if ( quarter==1 || quarter==2 || quarter==3 || quarter==4 )
            sql +=    "   and d.quarter=" + QString::number(quarter) + " ";

        sql +=        "   and d.age=" + QString::number(age) + " "
                      "   and a.code_mo is not NULL "                                       // прикреплён к МО
                      "   and mo.code_mo='" + code_mo + "' "
                      " group by d.quarter, coalesce(a.code_mo, '0'), coalesce(a.code_mp, '0'), coalesce(a.code_ms, '0') "
                      " order by d.quarter, coalesce(a.code_mo, '0'), coalesce(a.code_mp, '0'), coalesce(a.code_ms, '0') ; ";

        QSqlQuery *query_rep2 = new QSqlQuery(db);
        bool res_rep2 = mySQL.exec(this, sql, "Формирование плана обзвона застрахованнных по вопросу необходимости прохождения диспансеризации в МО, поквартального", *query_rep2, true, db, data_app);
        if (!res_rep2) {
            QMessageBox::warning(this, "Не удалось построить план обзвона застрахованнных по вопросу необходимости прохождения диспансеризации в МО, поквартального", "При попытке построить план обзвона застрахованнных по вопросу необходимости прохождения диспансеризации в МО, поквартального, произошла неожиданная ошибка.");
            delete query_rep2;
            return;
        };
        int n_rep2 = query_rep2->size();
        if (n_rep2==0) {
            delete query_rep2;
            continue;
        }


        // распаковка шаблона
        QString rep_folder2 = data_app.path_reports;
        // распаковка шаблона
        QString tmp_folder2 = data_app.path_out + "_REPORTS_/temp/";
        QString rep_template2 = rep_folder2 + "rep_disp_calls4mo.odt";


        if (!QFile::exists(rep_template2)) {
            QMessageBox::warning(this,
                                 "Шаблон не найден",
                                 "Не найден шаблон печатной формы заявления: \n" + rep_template2 +
                                 "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        QDir tmp_dir2(tmp_folder2);
        if (tmp_dir2.exists(tmp_folder2) && !tmp_dir2.removeRecursively()) {
            QMessageBox::warning(this,
                                 "Ошибка при очистке папки",
                                 "Не удалось очистить папку временных файлов: \n" + tmp_folder2 +
                                 "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        tmp_dir2.mkpath(tmp_folder2);
        QProcess myProcess2;
        QString com2 = "\"" + data_app.path_arch + "7z.exe\" e \"" + rep_template2 + "\" -o\"" + tmp_folder2 + "\" content.xml";
        if (myProcess2.execute(com2)!=0) {
            QMessageBox::warning(this,
                                 "Ошибка распаковки шаблона",
                                 "Не удалось распаковать файл контента шаблона\n" + rep_template2 + " -> content.xml"
                                 "\nпечатной формы во временную папку\n" + tmp_folder2 +
                                 "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }

        // ===========================
        // правка контента2 шаблона
        // ===========================
        // открытие контента2 шаблона
        QFile f_content2(tmp_folder2 + "/content.xml");
        f_content2.open(QIODevice::ReadOnly);
        QString s_content2 = f_content2.readAll();
        f_content2.close();
        if (s_content2.isEmpty()) {
            QMessageBox::warning(this,
                                 "Файл контента пустой",
                                 "Файл контента пустой. Возможно шаблон был испорчен.\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }

        // -----------------------------
        // правка полей контента2 шаблона
        // -----------------------------
        s_content2 = s_content2.
                     replace("#YEAR#", QString::number(year)).
                     replace("#AGE#", (age==1 ? "дети (до 18 лет)" : "взрослые (старше 18 лет)"));

        // ---------------------------- //
        // разберём контент2 по строкам //
        // ---------------------------- //
        int pos00 = 0;
        int pos11 = s_content2.indexOf("<table:table-row table:style-name=\"Таблица2.2\">", pos00);
        int pos12 = s_content2.indexOf("</table:table-row>", pos11) + 18;
        int pos21 = s_content2.indexOf("<table:table-row table:style-name=\"Таблица4.2\">", pos12);
        int pos22 = s_content2.indexOf("</table:table-row>", pos21) + 18;
        int pos31 = s_content2.indexOf("<table:table-row table:style-name=\"Таблица6.2\">", pos22);
        int pos32 = s_content2.indexOf("</table:table-row>", pos31) + 18;
        int pos41 = s_content2.indexOf("<table:table-row table:style-name=\"Таблица8.2\">", pos32);
        int pos42 = s_content2.indexOf("</table:table-row>", pos41) + 18;
        int pos50 = s_content2.length();
        QString s2_top   = s_content2.mid(pos00, pos11-pos00);
        QString s2_line1 = s_content2.mid(pos11, pos12-pos11);
        QString s2_mid1  = s_content2.mid(pos12, pos21-pos12);
        QString s2_line2 = s_content2.mid(pos21, pos22-pos21);
        QString s2_mid2  = s_content2.mid(pos22, pos31-pos22);
        QString s2_line3 = s_content2.mid(pos31, pos32-pos31);
        QString s2_mid3  = s_content2.mid(pos32, pos41-pos32);
        QString s2_line4 = s_content2.mid(pos41, pos42-pos41);
        QString s2_down  = s_content2.mid(pos42, pos50-pos42);
        QString s2_line;


        // --------------------------------------------- //
        // рассчитаем план по диспансеризации на квартал //
        // --------------------------------------------- //

        int quart = 1;
        int quart_cnt = 0;
        s_content2 = s2_top;
        int cnt = 0;

        while (query_rep2->next()) {
            // получим данные нового участка
            int quart_new   = query_rep2->value(0).toInt();
            QString code_mo = query_rep2->value(1).toString();
            QString code_mp = query_rep2->value(2).toString();
            if (code_mp.trimmed().isEmpty())  code_mp = "0";
            QString code_ms = query_rep2->value(3).toString();
            if (code_ms.trimmed().isEmpty())  code_ms = "0";
            cnt = query_rep2->value(4).toInt();
m_quart:
            if (cnt<1)  continue;

            if (quart!=quart_new) {
                // этот квартал завершён
                //s_content2 += s2_line;
                switch (quart) {
                case 1 : s_content2 += s2_mid1; break;
                case 2 : s_content2 += s2_mid2; break;
                case 3 : s_content2 += s2_mid3; break;
                }
                quart++;
                quart_cnt = 0;
                goto m_quart;
            }

            // выберем строку шаблоны
            switch (quart) {
            case 1 : s2_line = s2_line1; break;
            case 2 : s2_line = s2_line2; break;
            case 3 : s2_line = s2_line3; break;
            default: s2_line = s2_line4; break;
            }

            // к последнему кварталу строчу добавляю без проверки
            s2_line = s2_line
                    .replace("#CODE_MP#",code_mp)
                    .replace("#CODE_MS#",code_ms)
                    .replace("#N_PERS#", QString::number(cnt));
            s_content2 += s2_line;
            // вычтем потраченное
            quart_cnt += cnt;
            cnt = 0;
        }

        // закроем не закрытые кварталы
        switch (quart) {
        case 1 :
            s_content2 += s2_mid1;
            s_content2 += s2_mid2;
            s_content2 += s2_mid3;
            break;
        case 2 :
            s_content2 += s2_mid2;
            s_content2 += s2_mid3;
            break;
        case 3 :
            s_content2 += s2_mid3;
            break;
        }
        s_content2 += s2_down;


        // сохранение контента шаблона
        QFile f_content2_save(tmp_folder2 + "/content.xml");
        f_content2_save.open(QIODevice::WriteOnly);
        f_content2_save.write(s_content2.toUtf8());
        f_content2_save.close();


        // ===========================
        // архивация шаблона
        // ===========================
        QString fname2_res = data_app.path_out + "_ДИСПАНСЕРИЗАЦИЯ_/" +
                QDate::currentDate().toString("yyyy-MM-dd") + "  -  " + data_app.smo_short + " - " + data_app.filial_name + "  -  " + code_mo + " - план информирования застрахованных по вопросу диспансеризации на " + qy_str + " - " + (age==1 ? "дети (до 18 лет)" : "взрослые (старше 18 лет)") + ".odt";

        QFile::remove(fname2_res);
        while (QFile::exists(fname2_res)) {
            if (QMessageBox::warning(this,
                                     "Ошибка сохранения печатной формы",
                                     "Не удалось сохранить шаблон печатной формы: \n" + rep_template2 +
                                     "\n\nво временную папку\n" + fname2_res +
                                     "\n\nПроверьте, не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                     QMessageBox::Retry|QMessageBox::Abort,
                                     QMessageBox::Retry)==QMessageBox::Abort) {
                this->setCursor(Qt::ArrowCursor);
                return;
            }
            QFile::remove(fname2_res);
        }
        if (!QFile::copy(rep_template2, fname2_res) || !QFile::exists(fname2_res)) {
            QMessageBox::warning(this, "Ошибка копирования шаблона", "Не удалось скопировать шаблон печатной формы: \n" + rep_template2 + "\n\nв целевую папку\n" + fname2_res + "\n\nОперация прервана.");
            this->setCursor(Qt::ArrowCursor);
            return;
        }

        QProcess myProcess;
        com2 = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname2_res + "\" \"" + tmp_folder2 + "/content.xml\"";
        if (myProcess.execute(com2)!=0) {
            QMessageBox::warning(this,
                                 "Ошибка обновления контента",
                                 "При добавлении нового контента в шаблон печатной формы заявления произошла непредвиденная ошибка\n\nОпреация прервана.");
        }
    }


    // ========================= //
    // собственно открытие папки //
    // ========================= //
    // открытие папки отчётов
    QString folder_res = data_app.path_out + "_ДИСПАНСЕРИЗАЦИЯ_";
    long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(folder_res.utf16()), 0, 0, SW_NORMAL);
    this->setCursor(Qt::ArrowCursor);
    if (result<32) {
        QMessageBox::warning(this,
                             "Ошибка при открытии папки",
                             "При попытке открыть папку сохранения отчётов произошла ошибка:\n\n" + folder_res +
                             "\nКод ошибки: " + QString::number(result) +
                             "\n\nПопробуйте открыть эту папку вручную.");
    }
}

void disp_plans_wnd::on_bn_load_autocall_clicked() {
    QMessageBox::information(this, "Придётся всё делать вручрую :((", "Данные результата автодозвона надо импортировать в таблицу AutoCall.");
}

void disp_plans_wnd::on_bn_gen_calls_res4mo_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, "Нет доступа к базе данных", "Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику.");
        return;
    }
    // запросим год диспансеризации
    delete get_year_w;
    get_year_w = new get_year_wnd("Год/квартал диспансеризации",
                                  "Выберите год/квартал, на который надо сформировать отчёт по результатам информирования застрахованных о необходимости прохождения даиспансеризации в разрезе МО.",
                                  false, false, QDate::currentDate().year(), (QDate::currentDate().month()-1)/3 +1, db, data_app, settings, this);
    if (!get_year_w->exec())
        return;

    int age = ui->rb_age1->isChecked() ? 1 : 2;
    int year = get_year_w->year;
    int quarter = get_year_w->quarter;


    // --------------------------- //
    // получение данных для отчёта //
    // --------------------------- //
    QString sql = "select q_all.name_mo, q_all.cnt, q_all.cnt, q_call.cnt, q_call.cnt, (cast(cast(q_call.cnt*10000/q_all.cnt + 0.1 as integer) as real)/100)||'%', q_all.ia1_fio, q_all.ia2_fio \n"
                  "  from \n"
                  "  ( select mo.code_mo, '('||mo.code_mo||')  '||mo.name_mo as name_mo, count(*) as cnt, ia1.fio as ia1_fio, ia2.fio as ia2_fio \n"
                  "      from clinic.disp_plans d \n"
                  "           join persons e on(e.id=d.id_person) \n"
                  "      left join clinic.autocall c on(c.id_person=e.id and c.date_call=CURRENT_DATE) \n"
                  "      left join persons_assig a on(a.id_person=e.id and a.is_actual=0) \n"
                  "      left join spr_medorgs mo on(mo.code_mo=a.code_mo and  mo.is_head=1) \n"
                  "      left join insure_agents ia1 on(ia1.id=mo.id_insure_agent1) \n"
                  "      left join insure_agents ia2 on(ia2.id=mo.id_insure_agent2) \n"
                  "     where d.year=" + QString::number(year) + " \n"
                  "       and a.code_mo is not NULL \n"
                  "       and d.age=" + QString::number(age) + " \n";
    if ( quarter==1 || quarter==2 || quarter==3 || quarter==4 )
        sql +=    "       and d.quarter=" + QString::number(quarter) + " \n";
    sql +=        "     group by mo.code_mo, mo.name_mo, ia1.fio, ia2.fio \n"
                  "     order by mo.code_mo, mo.name_mo \n"
                  "  ) q_all \n"
                  " left join \n"
                  "  ( select mo.code_mo, '('||mo.code_mo||') '||mo.name_mo as name_mo, count(*) as cnt \n"
                  "      from clinic.disp_plans d \n"
                  "           join persons e on(e.id=d.id_person) \n"
                  "      left join clinic.autocall c on(c.id_person=e.id and c.date_call=CURRENT_DATE) \n"
                  "      left join persons_assig a on(a.id_person=e.id and a.is_actual=0) \n"
                  "      left join spr_medorgs mo on(mo.code_mo=a.code_mo and  mo.is_head=1) \n"
                  "     where d.year=" + QString::number(year) + " \n"
                  "       and a.code_mo is not NULL \n"
                  "       and d.age=" + QString::number(age) + " \n";
    if ( quarter==1 || quarter==2 || quarter==3 || quarter==4 )
        sql +=    "       and d.quarter=" + QString::number(quarter) + " \n";
    sql +=        "       and c.status=1 \n"
                  "     group by mo.code_mo, mo.name_mo \n"
                  "  ) q_call on(q_call.code_mo=q_all.code_mo) ; ";

    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString("Отчёт по результатам информирования застрахованных о необходимости прохождения даиспансеризации на год/квартал в разрезе МО"), *query, true, db, data_app);
    if (!res) {
        QMessageBox::warning(this, "Данные не получены", "Данные не получены.");
        return;
    }


    this->setCursor(Qt::WaitCursor);


    // распаковка шаблона
    QString rep_folder = data_app.path_reports;
    QString tmp_folder = data_app.path_out + "_REPORTS_/temp/";
    QString rep_template = rep_folder + "rep_disp_calls_res4mo.odt";



    if (!QFile::exists(rep_template)) {
        QMessageBox::warning(this,
                             "Шаблон не найден",
                             "Не найден шаблон печатной формы уведомления: \n" + rep_template +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    QDir tmp_dir(tmp_folder);
    if (tmp_dir.exists(tmp_folder) && !tmp_dir.removeRecursively()) {
        QMessageBox::warning(this,
                             "Ошибка при очистке папки",
                             "Не удалось очистить папку временных файлов: \n" + tmp_folder +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    tmp_dir.mkpath(tmp_folder);
    QProcess myProcess;
    QString com = "\"" + data_app.path_arch + "7z.exe\" e \"" + rep_template + "\" -o\"" + tmp_folder + "\" content.xml";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка распаковки шаблона",
                             "Не удалось распаковать файл контента шаблона\n" + rep_template + " -> content.xml"
                             "\nпечатной формы во временную папку\n" + tmp_folder +
                             "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return;
    }


    // ===========================
    // правка контента шаблона
    // ===========================
    // открытие контента шаблона
    QFile f_content(tmp_folder + "/content.xml");
    f_content.open(QIODevice::ReadOnly);
    QString s_content = f_content.readAll();
    f_content.close();
    if (s_content.isEmpty()) {
        QMessageBox::warning(this,
                             "Файл контента шаблона пустой",
                             "Файл контента шаблона пустой. Возможно шаблон был испорчен.\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    // выбор шаблона
    int pos0 = s_content.indexOf("<table:table-row table:style-name=\"Таблица1.4\">", 0);
    int pos1 = s_content.indexOf("</table:table-row>", pos0) + 18;
    QString s_row = s_content.mid(pos0, pos1-pos0);

    // правка полей контента шаблона
    QString s_content_new = s_content.left(pos0);

    QString qy_str = "";
    if ( quarter==1 || quarter==2 || quarter==3 || quarter==4 )
        qy_str += QString::number(quarter) + " квартал ";
    qy_str += QString::number(year);
    if ( quarter==1 || quarter==2 || quarter==3 || quarter==4 )
        qy_str += " года ";
    else  qy_str += " год ";

    s_content_new = s_content_new.replace("#QUART_YEAR#", qy_str);
    s_content_new = s_content_new.replace("#AGE#", age==1 ? "дети (до 18 лет)" : "взрослые (18 лет и старше)");



    int n = 0;

    while (query->next()) {
        n++;
        QString s_row_new = s_row;
        s_row_new = s_row_new.
                replace("#NAME_MO#",  query->value(0).toString()).
                replace("#CNT_ALL#",  QString::number(query->value(1).toInt())).
                replace("#CNT_DISP#", QString::number(query->value(2).toInt())).
                replace("#CALL1#",    QString::number(query->value(3).toInt())).
                replace("#PHONE1#",   QString::number(query->value(4).toInt())).
                replace("#OK%#",      query->value(5).toString()).
                replace("#AGENT1#",   query->value(6).toString()).
                replace("#AGENT2#",   query->value(7).toString());
        s_content_new += s_row_new;
    }
    s_content_new += s_content.right(s_content.size()-pos1);

    delete query;

    // сохранение контента шаблона
    QFile f_content_save(tmp_folder + "/content.xml");
    f_content_save.open(QIODevice::WriteOnly);
    f_content_save.write(s_content_new.toUtf8());
    f_content_save.close();


    // ===========================
    // архивация шаблона
    // ===========================
    QString fname_res = data_app.path_out + "_ДИСПАНСЕРИЗАЦИЯ_/" +
            QDate::currentDate().toString("yyyy-MM-dd") + "  -  " + data_app.smo_short + " - " + data_app.filial_name + "  -  Информирование застрахованных о диспансеризации - " + qy_str + ", " + QString(age==1 ? "ДЕТИ" : "ВЗРОСЛЫЕ") + ".ods";

    QFile::remove(fname_res);
    while (QFile::exists(fname_res)) {
        if (QMessageBox::warning(this,
                                 "Ошибка сохранения контента отчёта",
                                 "Не удалось сохранить контент отчёта: \n" + rep_template +
                                 "\n\nво временную папку\n" + fname_res +
                                 "\n\nПроверьте, не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                 QMessageBox::Retry|QMessageBox::Abort,
                                 QMessageBox::Retry)==QMessageBox::Abort) {
            this->setCursor(Qt::ArrowCursor);
            return;
        }
        QFile::remove(fname_res);
    }
    if (!QFile::copy(rep_template, fname_res) || !QFile::exists(fname_res)) {
        QMessageBox::warning(this, "Ошибка копирования отчёта", "Не удалось скопировать шаблон отчёта: \n" + rep_template + "\n\nво временную папку\n" + fname_res + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    com = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname_res + "\" \"" + tmp_folder + "/content.xml\"";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка обновления контента",
                             "При добавлении нового контента в шаблон отчёта произошла непредвиденная ошибка\n\nОпреация прервана.");
    }

    // ===========================
    // собственно открытие шаблона
    // ===========================
    // открытие полученного ODT
    long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);
    this->setCursor(Qt::ArrowCursor);
    if (result<32) {
        QMessageBox::warning(this,
                             "Ошибка при открытии готового файла отчёта",
                             "готовый файл отчёта не найден:\n\n" + fname_res +
                             "\nКод ошибки: " + QString::number(result) +
                             "\n\nПопробуйте открыть этот файл вручную.\n\nЕсли файл не создан или ошибка будет повторяться - обратитесь к разработчику.");
    }
}
