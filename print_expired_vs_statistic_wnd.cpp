#include "print_expired_vs_statistic_wnd.h"
#include "ui_print_expired_vs_statistic_wnd.h"

print_expired_vs_statistic_wnd::print_expired_vs_statistic_wnd(QSqlDatabase &db, s_data_eVStat &data_eVStat, s_data_app &data_app, QSettings &settings, QWidget *parent)
    : db(db), data_eVStat(data_eVStat), data_app(data_app), settings(settings), QDialog(parent), ui(new Ui::print_expired_vs_statistic_wnd)
{
    ui->setupUi(this);

    refresh_points();
    refresh_years_mons();

    show_tab_w = NULL;

    ui->date_date->setDate(QDate::currentDate());
}

print_expired_vs_statistic_wnd::~print_expired_vs_statistic_wnd() {
    delete ui;
}

void print_expired_vs_statistic_wnd::on_bn_cansel_clicked() {
    reject();
}

void print_expired_vs_statistic_wnd::on_bn_today_clicked() {
    ui->date_date->setDate(QDate::currentDate());
}

void print_expired_vs_statistic_wnd::on_date_date_dateChanged(const QDate &date) {
}

void print_expired_vs_statistic_wnd::on_rb_list_clicked(bool checked) {
}

void print_expired_vs_statistic_wnd::on_rb_count_clicked(bool checked) {
}

void print_expired_vs_statistic_wnd::on_ch_by_date_clicked(bool checked) {
}

void print_expired_vs_statistic_wnd::on_ch_by_points_clicked(bool checked) {
    ui->combo_point->setEnabled(checked);
}

void print_expired_vs_statistic_wnd::on_ch_by_years_clicked(bool checked) {
    ui->combo_year->setEnabled(checked);
    ui->ch_by_months->setChecked(false);
    ui->combo_month->setEnabled(false);
}

void print_expired_vs_statistic_wnd::on_ch_by_months_clicked(bool checked) {
    ui->combo_month->setEnabled(checked);
    ui->ch_by_years->setChecked(false);
    ui->combo_year->setEnabled(false);
}

void print_expired_vs_statistic_wnd::refresh_years_mons() {
    this->setCursor(Qt::WaitCursor);
    // обновление списка папок
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select min(dt_ins) as date_min, max(dt_ins) as date_max "
                  "  from public.persons ; ";
    mySQL.exec(this, sql, QString("Диапазон дат страхования персон"), *query, true, db, data_app);

    ui->combo_year->clear();
    ui->combo_year->addItem("- разбить по годам -", QDate(1900,1,1));
    ui->combo_month->clear();
    ui->combo_month->addItem("- разбить по месяцам -", QDate(1900,1,1));
    query->next();
    QDate date_min = query->value(0).toDate();
    QDate date_max = query->value(1).toDate();
    for (int i=date_max.year(); i>=date_min.year(); i--){
        // годы
        ui->combo_year->addItem(QString::number(i) + " г.", QDate(i,1,1));

        // месяцы
        if ( (i==date_min.year() && date_min.month()<=12) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=12) ) {
            ui->combo_month->addItem(QString::number(i) + ", декабрь", QDate(i,12,1));
        }
        if ( (i==date_min.year() && date_min.month()<=11) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=11) ) {
            ui->combo_month->addItem(QString::number(i) + ", ноябрь", QDate(i,11,1));
        }
        if ( (i==date_min.year() && date_min.month()<=10) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=10) ) {
            ui->combo_month->addItem(QString::number(i) + ", октябрь", QDate(i,10,1));
        }
        if ( (i==date_min.year() && date_min.month()<=9) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=9) ) {
            ui->combo_month->addItem(QString::number(i) + ", сентябрь", QDate(i,9,1));
        }
        if ( (i==date_min.year() && date_min.month()<=8) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=8) ) {
            ui->combo_month->addItem(QString::number(i) + ", август", QDate(i,8,1));
        }
        if ( (i==date_min.year() && date_min.month()<=7) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=7) ) {
            ui->combo_month->addItem(QString::number(i) + ", июль", QDate(i,7,1));
        }
        if ( (i==date_min.year() && date_min.month()<=6) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=6) ) {
            ui->combo_month->addItem(QString::number(i) + ", июнь", QDate(i,6,1));
        }
        if ( (i==date_min.year() && date_min.month()<=5) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=5) ) {
            ui->combo_month->addItem(QString::number(i) + ", май", QDate(i,5,1));
        }
        if ( (i==date_min.year() && date_min.month()<=4) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=4) ) {
            ui->combo_month->addItem(QString::number(i) + ", апрель", QDate(i,4,1));
        }
        if ( (i==date_min.year() && date_min.month()<=3) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=3) ) {
            ui->combo_month->addItem(QString::number(i) + ", март", QDate(i,3,1));
        }
        if ( (i==date_min.year() && date_min.month()<=2) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=2) ) {
            ui->combo_month->addItem(QString::number(i) + ", февраль", QDate(i,2,1));
        }
        if ( (i==date_min.year() && date_min.month()<=1) ||
             (i>date_min.year()  && i<date_max.year() )  ||
             (i==date_max.year() && date_max.month()>=1) ) {
            ui->combo_month->addItem(QString::number(i) + ", январь", QDate(i,1,1));
        }
    }
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void print_expired_vs_statistic_wnd::refresh_points() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка медорганизаций
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select id, '('||point_regnum||')  '||point_name as name "
                  " from public.points "
                  " where status=1 and point_regnum<>'000' "
                  " order by point_regnum ; ";
    mySQL.exec(this, sql, QString("Список ПВП"), *query, true, db, data_app);
    ui->combo_point->clear();
    ui->combo_point->addItem("- разбить по ПВП -", -1);
    while (query->next()) {
        ui->combo_point->addItem(query->value(1).toString(), query->value(0).toString());
    }
    ui->combo_point->setCurrentIndex(0);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void print_expired_vs_statistic_wnd::on_bn_ok_clicked() {
    data_eVStat.rep_rep_type = (ui->rb_count->isChecked() ? 1 : 2);
    data_eVStat.rep_pol_type = (ui->rb_vs->isChecked() ? 1 : 2);
    data_eVStat.rep_out_time = (ui->rb_outtime->isChecked() ? 1 : 2);

    data_eVStat.date_d = ui->date_date->date();

    data_eVStat.f_points = ui->ch_by_points->isChecked();
    data_eVStat.id_point = ui->combo_point->currentData().toInt();
    data_eVStat.point_text = ui->combo_point->currentText();

    data_eVStat.f_years = ui->ch_by_years->isChecked();
    data_eVStat.year_d = ui->combo_year->currentData().toDate();
    data_eVStat.year_text = ui->combo_year->currentText();

    data_eVStat.f_months = ui->ch_by_months->isChecked();
    data_eVStat.month_d = ui->combo_month->currentData().toDate();
    data_eVStat.month_text = ui->combo_month->currentText();

    // заголовок окна
    QString header_s = "";

    if (data_eVStat.rep_rep_type==1) header_s += "Число ";
    else header_s += "Список ";

    if (data_eVStat.rep_out_time==1) header_s += "просроченных ";
    else header_s += "выданных ";

    if (data_eVStat.rep_pol_type==1) header_s += "ВС ";
    else header_s += "полисов ";

    header_s += ", по состоянию на " + data_eVStat.date_d.toString("dd.MM.yyyy") + " ";

    if (data_eVStat.f_points)
        header_s += ",  на ПВП: " + data_eVStat.point_text;
    if (data_eVStat.f_years)
        header_s += ",  по годам: " + data_eVStat.year_text;
    if (data_eVStat.f_months)
        header_s += ",  по месяцам: " + data_eVStat.month_text;


    // ----------------------------------- //
    // собственно формирование стат-отчёта //
    // ----------------------------------- //

    QString sql;
    // Закешируем ID полисов, действовавших на дату
    sql = "update persons "
          "  set _id=( "
          "   select max(pol.id) "
          "     from polises pol "
          "    where pol.id_person=persons.id and pol.pol_v=2 and pol.date_begin<'#DATE#') ; ";
    sql+= "update persons "
          "   set _id2=( "
          "    select min(pol.id) "
          "      from polises pol "
          "    where pol.id_person=persons.id and pol.pol_v=3 and pol.date_begin<'#DATE#') ; ";

    sql = sql.replace("#DATE#", data_eVStat.date_d.toString("yyyy-MM-dd"));
    QSqlQuery *query = new QSqlQuery(db);
    mySQL.exec(this, sql, QString("Закешируем ID полисов, действовавших на дату"), *query, true, db, data_app);
    delete query;


    QString sql_up = "select #FIELDS# \n"
                     "  from #TABLES# \n"
                     " where #WHERES# \n"
                     " #GROUPS# \n"
                     " #ORDERS# ; ";
    QString fields_up = " ";
    QString tables_up = " ";
    QString wheres_up = " ";
    QString orders_up = " ";
    QString groups_up = " ";


    // -------- //
    // - ПОЛЯ - //
    // -------- //

    // по выбору ПВП
    if (data_eVStat.f_points) {
        if (data_eVStat.id_point==-1) {
            // в каждой строке будет повторено своё название ПВП
            fields_up += " '(' || pt.point_regnum || ')  ' || pt.point_name as point_name, \n";
        } else {
            // только один ПВП
            fields_up += " '(' || pt.point_regnum || ')  ' || pt.point_name as point_name, \n";
        }
    } else {
        fields_up += " ' - все ПВП - ' as point_name, \n";
    }
    // по выбору месяца/года
    if (data_eVStat.f_years) {
        if (data_eVStat.year_d==QDate(1900,1,1)) {
            // в каждой строке будет повторено своё название года
            fields_up += " EXTRACT(year FROM pol.date_begin) as year, \n";
        } else {
            // только один год
            fields_up += " EXTRACT(year FROM pol.date_begin) as year, \n";
        }
    } else if (data_eVStat.f_months) {
        if (data_eVStat.year_d==QDate(1900,1,1)) {
            // в каждой строке будет повторено своё название месяца
            fields_up += " EXTRACT(year FROM pol.date_begin) || '-' || right('00' || EXTRACT(month FROM pol.date_begin), 2) as month, \n";
        } else {
            // только один месяц
            fields_up += " EXTRACT(year FROM pol.date_begin) || '-' || right('00' || EXTRACT(month FROM pol.date_begin), 2) as month, \n";
        }
    } else {
        fields_up += " ' - все даты - ', \n";
    }

    // по типу отчёта
    if (data_eVStat.rep_rep_type==2) {
        // по типу полисов
        if (data_eVStat.rep_pol_type==1) {
            fields_up += "  e.fam, e.im, e.ot, e.date_birth, \n"
                         "  case pol.pol_v "
                         "    when 1 then 'старый полис' "
                         "    when 2 then 'ВС' "
                         "    when 3 then 'полис единого образца' "
                         "    else '???' "
                         "  end as vs_v, \n"
                         "  pol.vs_num, pol.date_begin as vs_date_begin, pol.date_end as vs_date_end, pol.date_stop as vs_date_stop, \n"
                         "  pol.enp, pol.date_get2hand as p_date_get2hand, \n"
                         "  'тел. ' || right('___________' || e.phone_cell, 11) as phone_cell, \n"
                         "  coalesce(ar.subj_name,' ') as terr_reg, \n"
                         "  coalesce(ar.dstr_name,' ') as dstr_reg, \n"
                         "  coalesce(ar.city_name,' ') as city_reg, \n"
                         "  coalesce(ar.nasp_name,' ') as nasp_reg, \n"
                         "  case "
                         "    when (coalesce(ar.strt_name,' ') || ',' || coalesce(ar.house,0) || ',' || coalesce(ar.quart,''))=' ,0,' \n"
                         "    then '' else coalesce(ar.strt_name,' ') || ',' || coalesce(ar.house,0) || ',' || coalesce(ar.quart,'') \n"
                         "  end as address_reg, \n"
                         "  coalesce(al.subj_name,' ') as terr_liv, \n"
                         "  coalesce(al.dstr_name,' ') as dstr_liv, \n"
                         "  coalesce(al.city_name,' ') as city_liv, \n"
                         "  coalesce(al.nasp_name,' ') as nasp_liv, \n"
                         "  case "
                         "    when (coalesce(al.strt_name,' ') || ',' || coalesce(al.house,0) || ',' || coalesce(al.quart,''))=' ,0,' \n"
                         "    then '' else coalesce(al.strt_name,' ') || ',' || coalesce(al.house,0) || ',' || coalesce(al.quart,'') \n"
                         "  end as address_liv  \n";
        } else {
            fields_up += "  e.fam, e.im, e.ot, e.date_birth, \n"
                         "  case pol.pol_v "
                         "    when 1 then 'старый полис' "
                         "    when 2 then 'ВС' "
                         "    when 3 then 'полис единого образца' "
                         "    else '???' "
                         "  end as pol_v, \n"
                         "  pol.pol_ser, pol.pol_num, pol.date_begin as pol_date_begin, pol.date_end as pol_date_end, pol.date_stop as pol_date_stop, \n"
                         "  pol.enp, pol.date_get2hand as p_date_get2hand, \n"
                         "  'тел. ' || right('___________' || e.phone_cell, 11) as phone_cell, \n"
                         "  coalesce(ar.subj_name,' ') as terr_reg, \n"
                         "  coalesce(ar.dstr_name,' ') as dstr_reg, \n"
                         "  coalesce(ar.city_name,' ') as city_reg, \n"
                         "  coalesce(ar.nasp_name,' ') as nasp_reg, \n"
                         "  case "
                         "    when (coalesce(ar.strt_name,' ') || ',' || coalesce(ar.house,0) || ',' || coalesce(ar.quart,''))=' ,0,' \n"
                         "    then '' else coalesce(ar.strt_name,' ') || ',' || coalesce(ar.house,0) || ',' || coalesce(ar.quart,'') \n"
                         "  end as address_reg, \n"
                         "  coalesce(al.subj_name,' ') as terr_liv, \n"
                         "  coalesce(al.dstr_name,' ') as dstr_liv, \n"
                         "  coalesce(al.city_name,' ') as city_liv, \n"
                         "  coalesce(al.nasp_name,' ') as nasp_liv, \n"
                         "  case "
                         "    when (coalesce(al.strt_name,' ') || ',' || coalesce(al.house,0) || ',' || coalesce(al.quart,''))=' ,0,' \n"
                         "    then '' else coalesce(al.strt_name,' ') || ',' || coalesce(al.house,0) || ',' || coalesce(al.quart,'') \n"
                         "  end as address_liv  \n";
        }
    } else if (data_eVStat.rep_rep_type==1) {
        fields_up += " count(*) as cnt \n";
    }


    // ----------- //
    // - ТАБЛИЦЫ - //
    // ----------- //

    // по типу полисов
    if (data_eVStat.rep_rep_type==1) {
        // просроченные
        if (data_eVStat.rep_pol_type==1) {
            // просроченные ВС
            tables_up += " persons e \n"
                         "         join polises pol on(pol.id=e._id and pol.pol_v=2) \n"
                         "    left join addresses ar on(ar.id=e.id_addr_reg) \n"
                         "    left join addresses al on(al.id=e.id_addr_liv) \n"
                         "    left join points pt on(pt.id=pol._id_first_point) \n";
        } else {
            // просроченные полисы
            tables_up += " persons e \n"
                         "         join polises pol on(pol.id=e._id2 and pol.pol_v=3) \n"
                         "    left join addresses ar on(ar.id=e.id_addr_reg) \n"
                         "    left join addresses al on(al.id=e.id_addr_liv) \n"
                         "    left join points pt on(pt.id=pol._id_first_point) \n";
        }
    } else {
        // выданыне
        if (data_eVStat.rep_pol_type==1) {
            // выданные ВС
            tables_up += " persons e \n"
                         "         join polises pol on(pol.id=e._id and pol.pol_v=2) \n"
                         "    left join addresses ar on(ar.id=e.id_addr_reg) \n"
                         "    left join addresses al on(al.id=e.id_addr_liv) \n"
                         "    left join points pt on(pt.id=pol._id_first_point) \n";
        } else {
            // выданыне полисы
            tables_up += " persons e \n"
                         "         join polises pol on(pol.id=e._id2 and pol.pol_v=3) \n"
                         "    left join addresses ar on(ar.id=e.id_addr_reg) \n"
                         "    left join addresses al on(al.id=e.id_addr_liv) \n"
                         "    left join points pt on(pt.id=pol._id_first_point) \n";
        }
    }


    // ----------- //
    // - УСЛОВИЯ - //
    // ----------- //

    // по типу отчёта
    if (data_eVStat.rep_rep_type==1) {
        // число
        if (data_eVStat.rep_out_time==1) {
            // число просроченных
            if (data_eVStat.rep_pol_type==1) {
                // число просроченных ВС
                wheres_up += " pol.pol_v=2 \n";

                if (data_eVStat.f_points && data_eVStat.id_point>-1) {
                    wheres_up += " and pt.id=" + QString::number(data_eVStat.id_point) + " \n";
                }

                wheres_up += "   and pol.date_end is not NULL and pol.date_end<'#DATE#' \n"
                             "   and (pol.date_stop is NULL or pol.date_stop>'#DATE#') \n"
                             "   and (pol.date_get2hand is NULL or pol.date_get2hand>'#DATE#') \n";

                if (data_eVStat.f_years && data_eVStat.year_d!=QDate(1900,1,1)) {
                    wheres_up += "   and pol.date_begin>='" + data_eVStat.year_d.toString("yyyy-MM-dd") + "' \n"
                                 "   and pol.date_begin<'"  + data_eVStat.year_d.addYears(1).toString("yyyy-MM-dd") + "' \n";
                }
                if (data_eVStat.f_months && data_eVStat.month_d!=QDate(1900,1,1)) {
                    wheres_up += "   and pol.date_begin>='" + data_eVStat.month_d.toString("yyyy-MM-dd") + "' \n"
                                 "   and pol.date_begin<'"  + data_eVStat.month_d.addMonths(1).toString("yyyy-MM-dd") + "' \n";
                }
            } else {
                // число просроченных полисов
                wheres_up += " pol.pol_v=3 \n";

                if (data_eVStat.f_points && data_eVStat.id_point>-1) {
                    wheres_up += " and pt.id=" + QString::number(data_eVStat.id_point) + " \n";
                }

                wheres_up += "   and pol.date_end is not NULL and pol.date_end<'#DATE#' \n"
                             "   and (pol.date_stop is NULL or pol.date_stop>'#DATE#') \n"
                             "   and (pol.date_get2hand is NULL or pol.date_get2hand>'#DATE#') \n";

                if (data_eVStat.f_years && data_eVStat.year_d!=QDate(1900,1,1)) {
                    wheres_up += "   and pol.date_begin>='" + data_eVStat.year_d.toString("yyyy-MM-dd") + "' \n"
                                 "   and pol.date_begin<'"  + data_eVStat.year_d.addYears(1).toString("yyyy-MM-dd") + "' \n";
                }
                if (data_eVStat.f_months && data_eVStat.month_d!=QDate(1900,1,1)) {
                    wheres_up += "   and pol.date_begin>='" + data_eVStat.month_d.toString("yyyy-MM-dd") + "' \n"
                                 "   and pol.date_begin<'"  + data_eVStat.month_d.addMonths(1).toString("yyyy-MM-dd") + "' \n";
                }
            }
        } else {
            // число выданных
            if (data_eVStat.rep_pol_type==1) {
                // число выданных ВС
                wheres_up += " pol.pol_v=2 \n";

                if (data_eVStat.f_points && data_eVStat.id_point>-1) {
                    wheres_up += " and pt.id=" + QString::number(data_eVStat.id_point) + " \n";
                }

                wheres_up += "   and pol.date_get2hand<='#DATE#' \n";

                if (data_eVStat.f_years && data_eVStat.year_d!=QDate(1900,1,1)) {
                    wheres_up += "   and pol.date_begin>='" + data_eVStat.year_d.toString("yyyy-MM-dd") + "' \n"
                                 "   and pol.date_begin<'"  + data_eVStat.year_d.addYears(1).toString("yyyy-MM-dd") + "' \n";
                }
                if (data_eVStat.f_months && data_eVStat.month_d!=QDate(1900,1,1)) {
                    wheres_up += "   and pol.date_begin>='" + data_eVStat.month_d.toString("yyyy-MM-dd") + "' \n"
                                 "   and pol.date_begin<'"  + data_eVStat.month_d.addMonths(1).toString("yyyy-MM-dd") + "' \n";
                }
            } else {
                // число выданных полисов
                wheres_up += " pol.pol_v=3 \n";

                if (data_eVStat.f_points && data_eVStat.id_point>-1) {
                    wheres_up += " and pt.id=" + QString::number(data_eVStat.id_point) + " \n";
                }

                wheres_up += "   and pol.date_get2hand<='#DATE#' \n";

                if (data_eVStat.f_years && data_eVStat.year_d!=QDate(1900,1,1)) {
                    wheres_up += "   and pol.date_begin>='" + data_eVStat.year_d.toString("yyyy-MM-dd") + "' \n"
                                 "   and pol.date_begin<'"  + data_eVStat.year_d.addYears(1).toString("yyyy-MM-dd") + "' \n";
                }
                if (data_eVStat.f_months && data_eVStat.month_d!=QDate(1900,1,1)) {
                    wheres_up += "   and pol.date_begin>='" + data_eVStat.month_d.toString("yyyy-MM-dd") + "' \n"
                                 "   and pol.date_begin<'"  + data_eVStat.month_d.addMonths(1).toString("yyyy-MM-dd") + "' \n";
                }
            }
        }
    } else {
        // список
        if (data_eVStat.rep_out_time==1) {
            // список просроченных
            if (data_eVStat.rep_pol_type==1) {
                // список просроченных ВС
                wheres_up += " pol.pol_v=2 \n";

                if (data_eVStat.f_points && data_eVStat.id_point>-1) {
                    wheres_up += " and pt.id=" + QString::number(data_eVStat.id_point) + " \n";
                }

                wheres_up += "   and pol.date_end is not NULL and pol.date_end<'#DATE#' \n"
                             "   and (pol.date_stop is NULL or pol.date_stop>'#DATE#') \n"
                             "   and (pol.date_get2hand is NULL or pol.date_get2hand>'#DATE#') \n";

                if (data_eVStat.f_years && data_eVStat.year_d!=QDate(1900,1,1)) {
                    wheres_up += "   and pol.date_begin>='" + data_eVStat.year_d.toString("yyyy-MM-dd") + "' \n"
                                 "   and pol.date_begin<'"  + data_eVStat.year_d.addYears(1).toString("yyyy-MM-dd") + "' \n";
                }
                if (data_eVStat.f_months && data_eVStat.month_d!=QDate(1900,1,1)) {
                    wheres_up += "   and pol.date_begin>='" + data_eVStat.month_d.toString("yyyy-MM-dd") + "' \n"
                                 "   and pol.date_begin<'"  + data_eVStat.month_d.addMonths(1).toString("yyyy-MM-dd") + "' \n";
                }
            } else {
                // список просроченных полисов
                wheres_up += " pol.pol_v=3 \n";

                if (data_eVStat.f_points && data_eVStat.id_point>-1) {
                    wheres_up += " and pt.id=" + QString::number(data_eVStat.id_point) + " \n";
                }

                wheres_up += "   and pol.date_end is not NULL and pol.date_end<'#DATE#' \n"
                             "   and (pol.date_stop is NULL or pol.date_stop>'#DATE#') \n"
                             "   and (pol.date_get2hand is NULL or pol.date_get2hand>'#DATE#') \n";

                if (data_eVStat.f_years && data_eVStat.year_d!=QDate(1900,1,1)) {
                    wheres_up += "   and pol.date_begin>='" + data_eVStat.year_d.toString("yyyy-MM-dd") + "' \n"
                                 "   and pol.date_begin<'"  + data_eVStat.year_d.addYears(1).toString("yyyy-MM-dd") + "' \n";
                }
                if (data_eVStat.f_months && data_eVStat.month_d!=QDate(1900,1,1)) {
                    wheres_up += "   and pol.date_begin>='" + data_eVStat.month_d.toString("yyyy-MM-dd") + "' \n"
                                 "   and pol.date_begin<'"  + data_eVStat.month_d.addMonths(1).toString("yyyy-MM-dd") + "' \n";
                }
            }
        } else {
            // список выданных
            if (data_eVStat.rep_pol_type==1) {
                // список выданных ВС
                wheres_up += " pol.pol_v=2 \n";

                if (data_eVStat.f_points && data_eVStat.id_point>-1) {
                    wheres_up += " and pt.id=" + QString::number(data_eVStat.id_point) + " \n";
                }

                wheres_up += "   and pol.date_get2hand<='#DATE#' \n";

                if (data_eVStat.f_years && data_eVStat.year_d!=QDate(1900,1,1)) {
                    wheres_up += "   and pol.date_begin>='" + data_eVStat.year_d.toString("yyyy-MM-dd") + "' \n"
                                 "   and pol.date_begin<'"  + data_eVStat.year_d.addYears(1).toString("yyyy-MM-dd") + "' \n";
                }
                if (data_eVStat.f_months && data_eVStat.month_d!=QDate(1900,1,1)) {
                    wheres_up += "   and pol.date_begin>='" + data_eVStat.month_d.toString("yyyy-MM-dd") + "' \n"
                                 "   and pol.date_begin<'"  + data_eVStat.month_d.addMonths(1).toString("yyyy-MM-dd") + "' \n";
                }
            } else {
                // список выданных полисов
                wheres_up += " pol.pol_v=3 \n";

                if (data_eVStat.f_points && data_eVStat.id_point>-1) {
                    wheres_up += " and pt.id=" + QString::number(data_eVStat.id_point) + " \n";
                }

                wheres_up += "   and pol.date_get2hand<='#DATE#' \n";

                if (data_eVStat.f_years && data_eVStat.year_d!=QDate(1900,1,1)) {
                    wheres_up += "   and pol.date_begin>='" + data_eVStat.year_d.toString("yyyy-MM-dd") + "' \n"
                                 "   and pol.date_begin<'"  + data_eVStat.year_d.addYears(1).toString("yyyy-MM-dd") + "' \n";
                }
                if (data_eVStat.f_months && data_eVStat.month_d!=QDate(1900,1,1)) {
                    wheres_up += "   and pol.date_begin>='" + data_eVStat.month_d.toString("yyyy-MM-dd") + "' \n"
                                 "   and pol.date_begin<'"  + data_eVStat.month_d.addMonths(1).toString("yyyy-MM-dd") + "' \n";
                }
            }
        }
    }


    // --------------- //
    // - ГРУППИРОВКА - //
    // --------------- //

    groups_up = " 1 ";

    // по типу отчёта
    if (data_eVStat.rep_rep_type==1) {
        // число
        if (data_eVStat.rep_out_time==1) {
            // число просроченных
            if (data_eVStat.rep_pol_type==1) {
                // число просроченных ВС
                if (data_eVStat.f_months) {
                    groups_up += ", EXTRACT(year FROM pol.date_begin), EXTRACT(month FROM pol.date_begin) ";
                }
                if (data_eVStat.f_years) {
                    groups_up += ", EXTRACT(year FROM pol.date_begin) ";
                }
                if (data_eVStat.f_points) {
                    groups_up += ", pt.point_regnum, pt.point_name ";
                }
            } else {
                // число просроченных полисов
                if (data_eVStat.f_months) {
                    groups_up += ", EXTRACT(year FROM pol.date_begin), EXTRACT(month FROM pol.date_begin) ";
                }
                if (data_eVStat.f_years) {
                    groups_up += ", EXTRACT(year FROM pol.date_begin) ";
                }
                if (data_eVStat.f_points) {
                    groups_up += ", pt.point_regnum, pt.point_name ";
                }
            }
        } else {
            // число выданных
            if (data_eVStat.rep_pol_type==1) {
                // число выданынх ВС
                if (data_eVStat.f_months) {
                    groups_up += ", EXTRACT(year FROM pol.date_begin), EXTRACT(month FROM pol.date_begin) ";
                }
                if (data_eVStat.f_years) {
                    groups_up += ", EXTRACT(year FROM pol.date_begin) ";
                }
                if (data_eVStat.f_points) {
                    groups_up += ", pt.point_regnum, pt.point_name ";
                }
            } else {
                // число выданнык полисов
                if (data_eVStat.f_months) {
                    groups_up += ", EXTRACT(year FROM pol.date_begin), EXTRACT(month FROM pol.date_begin) ";
                }
                if (data_eVStat.f_years) {
                    groups_up += ", EXTRACT(year FROM pol.date_begin) ";
                }
                if (data_eVStat.f_points) {
                    groups_up += ", pt.point_regnum, pt.point_name ";
                }
            }
        }
    } else {
        // список
        if (data_eVStat.rep_out_time==1) {
            // список просроченных
            if (data_eVStat.rep_pol_type==1) {
                // список просроченных ВС
                groups_up = "  pt.point_regnum, pt.point_name, \n"
                            "  e.fam, e.im, e.ot, e.date_birth, \n"
                            "  pol.pol_v, pol.vs_num, pol.date_begin, pol.date_end, pol.date_stop, \n"
                            "  pol.enp, pol.date_get2hand, \n"
                            "  e.phone_cell, \n"
                            "  ar.subj_name, ar.dstr_name, ar.city_name, ar.nasp_name, \n"
                            "  ar.strt_name, ar.house, ar.quart, \n"
                            "  al.subj_name, al.dstr_name, al.city_name, al.nasp_name, \n"
                            "  al.strt_name, al.house, al.quart \n";
            } else {
                // список просроченных полисов
                groups_up = "  pt.point_regnum, pt.point_name, \n"
                            "  e.fam, e.im, e.ot, e.date_birth, \n"
                            "  pol.pol_v, pol.pol_ser, pol.pol_num, pol.date_begin, pol.date_end, pol.date_stop, \n"
                            "  pol.enp, pol.date_get2hand, \n"
                            "  e.phone_cell, \n"
                            "  ar.subj_name, ar.dstr_name, ar.city_name, ar.nasp_name, \n"
                            "  ar.strt_name, ar.house, ar.quart, \n"
                            "  al.subj_name, al.dstr_name, al.city_name, al.nasp_name, \n"
                            "  al.strt_name, al.house, al.quart \n";
            }
        } else {
            // список ыданных
            if (data_eVStat.rep_pol_type==1) {
                // список ыданных ВС
                groups_up = "  pt.point_regnum, pt.point_name, \n"
                            "  e.fam, e.im, e.ot, e.date_birth, \n"
                            "  pol.pol_v, pol.vs_num, pol.date_begin, pol.date_end, pol.date_stop, \n"
                            "  pol.enp, pol.date_get2hand, \n"
                            "  e.phone_cell, \n"
                            "  ar.subj_name, ar.dstr_name, ar.city_name, ar.nasp_name, \n"
                            "  ar.strt_name, ar.house, ar.quart, \n"
                            "  al.subj_name, al.dstr_name, al.city_name, al.nasp_name, \n"
                            "  al.strt_name, al.house, al.quart \n";
            } else {
                // список ыданных полисов
                groups_up = "  pt.point_regnum, pt.point_name, \n"
                            "  e.fam, e.im, e.ot, e.date_birth, \n"
                            "  pol.pol_v, pol.pol_ser, pol.pol_num, pol.date_begin, pol.date_end, pol.date_stop, \n"
                            "  pol.enp, pol.date_get2hand, \n"
                            "  e.phone_cell, \n"
                            "  ar.subj_name, ar.dstr_name, ar.city_name, ar.nasp_name, \n"
                            "  ar.strt_name, ar.house, ar.quart, \n"
                            "  al.subj_name, al.dstr_name, al.city_name, al.nasp_name, \n"
                            "  al.strt_name, al.house, al.quart \n";
            }
        }
    }

    if (groups_up.right(3)==", \n")
        groups_up = groups_up.left(groups_up.length()-3) + " ";


    // -------------- //
    // - СОРТИРОВКА - //
    // -------------- //

    orders_up = " 1 ";

    if ( data_eVStat.f_points &&
         !data_eVStat.f_years  &&
         !data_eVStat.f_months ) {
        orders_up += " , pt.point_regnum ";
    } else
    if ( data_eVStat.f_points &&
         data_eVStat.f_years  &&
         !data_eVStat.f_months ) {
        orders_up += " , pt.point_regnum, EXTRACT(year FROM pol.date_begin) ";
    } else
    if ( data_eVStat.f_points &&
         !data_eVStat.f_years  &&
         data_eVStat.f_months ) {
        orders_up += " , pt.point_regnum, EXTRACT(year FROM pol.date_begin) || '-' || right('00' || EXTRACT(month FROM pol.date_begin), 2) ";
    } else
    if ( !data_eVStat.f_points &&
         data_eVStat.f_years  &&
         !data_eVStat.f_months ) {
        orders_up += " , EXTRACT(year FROM pol.date_begin) ";
    } else
    if ( !data_eVStat.f_points &&
         !data_eVStat.f_years  &&
         data_eVStat.f_months ) {
        orders_up += " , EXTRACT(year FROM pol.date_begin) || '-' || right('00' || EXTRACT(month FROM pol.date_begin), 2) ";
    }

    if (data_eVStat.rep_rep_type==2) {
        orders_up += " , e.fam, e.im, e.ot, e.date_birth ";
    }

    if (orders_up.right(3)==", \n")
        orders_up = orders_up.left(orders_up.length()-3) + " ";


    // ------------------------- //
    // - СОБСТВЕННО САМ ЗАПРОС - //
    // ------------------------- //

    sql = sql_up.replace("#FIELDS#", fields_up).replace("#TABLES#", tables_up).replace("#WHERES#", wheres_up);

    if (groups_up==" 1 ") {
        sql = sql.replace("#GROUPS#", "");
    } else {
        groups_up = "group by " + groups_up;
        sql = sql.replace("#GROUPS#", groups_up);
    }

    if (orders_up==" 1 ") {
        sql = sql.replace("#ORDERS#", "");
    } else {
        orders_up = "order by " + orders_up;
        sql = sql.replace("#ORDERS#", orders_up);
    }

    sql = sql.replace("#DATE#", data_eVStat.date_d.toString("yyyy-MM-dd"));


    delete show_tab_w;
    show_tab_w = new show_tab_wnd(header_s, sql, db, data_app, this);
    show_tab_w->exec();

}
