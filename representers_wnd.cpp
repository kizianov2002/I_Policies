#include "representers_wnd.h"
#include "firms_wnd.h"
#include "photoeditor_wnd.h"
#include "ui_representers_wnd.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QTextCodec>
#include <QDesktopWidget>
#include <QStatusBar>

#include "d_date.h"

representers_wnd::representers_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent)
 : db(db), data_app(data_app), settings(settings), QDialog(parent), ui(new Ui::representers_wnd)
{
    ui->setupUi(this);

    statusLab = NULL;

    this->setWindowTitle("ИНКО-МЕД - Полисы ОМС - Таблица \"Представлители\"");

    ui->date_pers_pan_death->setVisible(false);

    on_show_pers_tabl_clicked(ui->show_pers_tabl->isChecked());
    on_show_pers_data_clicked(ui->show_pers_data->isChecked());
    on_show_pers_docs_clicked(ui->show_pers_docs->isChecked());
    ui->show_pers_tabl->setEnabled(true);
    ui->show_pers_data->setEnabled(false);
    ui->show_pers_docs->setEnabled(false);

    refresh_representers_tab();

    QRect screen = QApplication::desktop()->screenGeometry();
    if (screen.width()==1024 && screen.height()==768) {
        this->setWindowState( this->windowState() | Qt::WindowFullScreen);
    } else {
        this->resize(1200,700);
        this->move(10,50);
    }

    QStatusBar *b = new QStatusBar;
    this->layout()->addWidget(b);
    statusLab = new QLabel;
    b->addWidget(statusLab);
    statusLab->setFont(QFont("Arial",10,75,false));
    QPalette pal = statusLab->palette();
    pal.setColor(QPalette::Text,QColor(0,0,255));
    statusLab->setPalette(pal);
    statusLab->setText("программа: \"Полисы ОМС\", версия:" + data_app.version + ";  сервер: " + data_app.db_host + ";  БД: " + data_app.db_name + ";  версия: " + data_app.db_vers);
    //b->showMessage("программа: \"Полисы ОМС\", версия:" + data_app.version + ";  сервер: " + data_app.db_host + ";  БД: " + data_app.db_name + ";  версия: " + data_app.db_vers);
}

representers_wnd::~representers_wnd() {
    delete ui;
}

void representers_wnd::on_show_pers_tabl_clicked(bool checked) {
    ui->group_persons_tab->setVisible(checked);
    ui->group_persons_tab->setEnabled(checked);
}

void representers_wnd::on_show_pers_data_clicked(bool checked) {
    ui->group_person_data->setVisible(checked);
    ui->group_person_data->setEnabled(checked);
}

void representers_wnd::on_show_pers_docs_clicked(bool checked) {
    ui->group_person_docs->setVisible(checked);
    ui->group_person_docs->setEnabled(checked);
}

void representers_wnd::b_need_save(bool f) {
        ui->bn_pers_save->setVisible(f);
}

void representers_wnd::on_ln_pers_pan_fam_textChanged(const QString &arg1) {
    data_pers.fam = arg1;
    ui->lab_pers_pan_fio->setText(ui->ln_pers_pan_fam->text() + " " + ui->ln_pers_pan_im->text() + " " + ui->ln_pers_pan_ot->text() );
    b_need_save(true);
}
void representers_wnd::on_ln_pers_pan_im_textChanged(const QString &arg1) {
    data_pers.im = arg1;
    ui->lab_pers_pan_fio->setText(ui->ln_pers_pan_fam->text() + " " + ui->ln_pers_pan_im->text() + " " + ui->ln_pers_pan_ot->text() );
    b_need_save(true);
}
void representers_wnd::on_ln_pers_pan_ot_textChanged(const QString &arg1) {
    data_pers.ot = arg1;
    ui->lab_pers_pan_fio->setText(ui->ln_pers_pan_fam->text() + " " + ui->ln_pers_pan_im->text() + " " + ui->ln_pers_pan_ot->text() );
    b_need_save(true);
}
void representers_wnd::on_date_pers_pan_birth_dateChanged(const QDate &date) {
    data_pers.date_birth = date;
    ui->lab_pers_pan_fio->setText(ui->ln_pers_pan_fam->text() + " " + ui->ln_pers_pan_im->text() + " " + ui->ln_pers_pan_ot->text() );
    b_need_save(true);
}
void representers_wnd::on_ch_pers_pan_ot_clicked(bool checked) {
    if (!checked) data_pers.ot = "";
    ui->lab_pers_pan_fio->setText(ui->ln_pers_pan_fam->text() + " " + ui->ln_pers_pan_im->text() + " " + ui->ln_pers_pan_ot->text() );
    ui->ch_dost_ot->setChecked(checked);
    data_pers.dost = QString(checked ? "_" : "1") + data_pers.dost.right(5);
    ui->lab_pers_pan_dost->setText(data_pers.dost);
    b_need_save(true);
}
void representers_wnd::on_combo_pers_pan_sex_currentIndexChanged(int index) {
    data_pers.sex = index;
    ui->lab_pers_pan_fio->setText(ui->ln_pers_pan_fam->text() + " " + ui->ln_pers_pan_im->text() + " " + ui->ln_pers_pan_ot->text() );
    b_need_save(true);
}
void representers_wnd::on_line_pers_pan_snils_textChanged(const QString &arg1) {
    data_pers.snils = arg1;
    ui->lab_pers_pan_fio->setText(ui->ln_pers_pan_fam->text() + " " + ui->ln_pers_pan_im->text() + " " + ui->ln_pers_pan_ot->text() );
    b_need_save(true);
}
void representers_wnd::on_ch_pers_pan_snils_clicked(bool checked) {
    if (!checked) data_pers.snils = "";
    ui->lab_pers_pan_fio->setText(ui->ln_pers_pan_fam->text() + " " + ui->ln_pers_pan_im->text() + " " + ui->ln_pers_pan_ot->text() );
    b_need_save(true);
}


void representers_wnd::on_date_pers_pan_death_dateChanged(const QDate &date) {
    data_pers.date_death = date;
}
void representers_wnd::on_ch_pers_pan_death_clicked(bool checked) {
    if (!checked) data_pers.date_death = QDate(1990,1,1);
    b_need_save(true);
}
void representers_wnd::on_ln_pers_pan_plc_birth_textChanged(const QString &arg1) {
    data_pers.plc_birth = arg1;
}
void representers_wnd::on_date_pers_pan_reg_dateChanged(const QDate &date) {
    data_pers.reg_date = date;
}
void representers_wnd::on_ln_pers_pan_phone_code_textChanged(const QString &arg1) {
    QString txt0 = arg1.trimmed(), txt = "";
    for (int i=0; i<txt0.size(); ++i) {
        if (txt.length()>=6)
            break;
        QString c = txt0.mid(i, 1);
        if ( c=="0" || c=="1" || c=="2" || c=="3" || c=="4" || c=="5" || c=="6" || c=="7" || c=="8" || c=="9" ) {
            txt += c;
        }
    }
    ui->ln_pers_pan_phone_code->setText(txt);
    ui->ln_pers_pan_phone_code->setCursorPosition(txt.length());
    data_pers.phone_code = txt;
    ui->lab_pers_pan_phones->setText(ui->ln_pers_pan_phone_cell->text());
}
void representers_wnd::on_ln_pers_pan_phone_cell_textChanged(const QString &arg1) {
    // уберём все разделители
    QString txt0 = arg1.trimmed().replace("(","").replace(")","").replace("-","").replace(" ",""), txt = "";

    // перебор цифр
    for (int i=0; i<txt0.size(); ++i) {
        bool jmp = false;
        if (txt.length()>=20)
            break;
        QString c = txt0.mid(i, 1);

        // первая 8-ка
        if (i==0) {
            if (c!="8") {
                txt += "8";
            } else {
                txt += c;
                jmp = true;
                // 1-я цифра потрачена
            }
        }

        // разделители
        switch (txt.length()) {
        case 1:
            txt += " (";
            break;
        case 6:
            txt += ") ";
            break;
        case 11:
            txt += "-";
            break;
        case 14:
            txt += "-";
            break;
        case 17:
            txt += " ";
            break;
        default:
            break;
        }

        if (jmp)
            continue;

        if ( c=="0" || c=="1" || c=="2" || c=="3" || c=="4" || c=="5" || c=="6" || c=="7" || c=="8" || c=="9" ) {
            txt += c;
        }
    }
    ui->ln_pers_pan_phone_cell->setText(txt);
    ui->ln_pers_pan_phone_cell->setCursorPosition(txt.length());
    data_pers.phone_cell = txt;
    ui->lab_pers_pan_phones->setText(ui->ln_pers_pan_phone_cell->text());
}
void representers_wnd::on_ln_pers_pan_phone_home_textChanged(const QString &arg1) {
    // уберём все разделители
    QString txt0 = arg1.trimmed().replace("(","").replace(")","").replace("-","").replace(" ",""), txt = "";

    // перебор цифр
    for (int i=0; i<txt0.size(); ++i) {
        bool jmp = false;
        if (txt.length()>=20)
            break;
        QString c = txt0.mid(i, 1);

        // первая 8-ка
        if (i==0) {
            if (c!="8") {
                txt += "8";
            } else {
                txt += c;
                jmp = true;
                // 1-я цифра потрачена
            }
        }

        // разделители
        switch (txt.length()) {
        case 1:
            txt += " (";
            break;
        case 6:
            txt += ") ";
            break;
        case 11:
            txt += "-";
            break;
        case 14:
            txt += "-";
            break;
        case 17:
            txt += " ";
            break;
        default:
            break;
        }

        if (jmp)
            continue;

        if ( c=="0" || c=="1" || c=="2" || c=="3" || c=="4" || c=="5" || c=="6" || c=="7" || c=="8" || c=="9" ) {
            txt += c;
        }
    }
    ui->ln_pers_pan_phone_home->setText(txt);
    ui->ln_pers_pan_phone_home->setCursorPosition(txt.length());
    data_pers.phone_home = txt;
    ui->lab_pers_pan_phones->setText(ui->ln_pers_pan_phone_home->text());
}
void representers_wnd::on_ln_pers_pan_phone_work_textChanged(const QString &arg1) {
    // уберём все разделители
    QString txt0 = arg1.trimmed().replace("(","").replace(")","").replace("-","").replace(" ",""), txt = "";

    // перебор цифр
    for (int i=0; i<txt0.size(); ++i) {
        bool jmp = false;
        if (txt.length()>=20)
            break;
        QString c = txt0.mid(i, 1);

        // первая 8-ка
        if (i==0) {
            if (c!="8") {
                txt += "8";
            } else {
                txt += c;
                jmp = true;
                // 1-я цифра потрачена
            }
        }

        // разделители
        switch (txt.length()) {
        case 1:
            txt += " (";
            break;
        case 6:
            txt += ") ";
            break;
        case 11:
            txt += "-";
            break;
        case 14:
            txt += "-";
            break;
        case 17:
            txt += " ";
            break;
        default:
            break;
        }

        if (jmp)
            continue;

        if ( c=="0" || c=="1" || c=="2" || c=="3" || c=="4" || c=="5" || c=="6" || c=="7" || c=="8" || c=="9" ) {
            txt += c;
        }
    }
    ui->ln_pers_pan_phone_work->setText(txt);
    ui->ln_pers_pan_phone_work->setCursorPosition(txt.length());
    data_pers.phone_work = txt;
    ui->lab_pers_pan_phones->setText(ui->ln_pers_pan_phone_work->text());
}
void representers_wnd::on_ln_pers_pan_email_textChanged(const QString &arg1) {
    data_pers.email = arg1;
    ui->lab_pers_pan_phones->setText(ui->ln_pers_pan_phone_cell->text());
}

void representers_wnd::on_bn_close_clicked() {
    if (ui->bn_pers_pers_pan_back->isVisible() &&
        QMessageBox::warning(this,
                             "Есть несохранённые изменения",
                             "На окне есть несохранённые изменения.\nВы действительно хотите закрыть окно без сохранения несохранённых данных?",
                             QMessageBox::Yes|QMessageBox::Cancel,
                             QMessageBox::Cancel)==QMessageBox::Cancel) {
        return;
    }
    if (QMessageBox::question(this,"Завершение работы", "Закрыть программу?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes) {
        QApplication::closeAllWindows();
        return;
    }
    b_need_save();
}

void representers_wnd::on_act_close_triggered() {
    on_bn_close_clicked();
}

void representers_wnd::  refresh_representers_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    QString representers_sql = "select e.id, e.fam, e.im, e.ot, e.sex, e.date_birth, e.plc_birth, "
                               "       case when e.date_death is NULL then cast('01.01.1900' as date) else e.date_death end as date_death, "
                               "       e.snils, e.category, '('||e.category||') '||v13.text as categ_text, e.bomj, e.phone_cell, e.phone_code, e.phone_home, e.phone_work, e.email, "
                               "       o2b.code as oksm_b, o2b.short_name as oksm_b_name, "
                               "       o2c.code as oksm_c, o2c.short_name as oksm_c_name, "
                               "       e.status, "
                               "       NULL as id_firm, NULL as firm_name, "
                               "       case when e.id_addr_reg is NULL then -1 else e.id_addr_reg end as id_addr_reg, "
                               "       case when e.date_reg is NULL then cast('01.01.1900' as date) else e.date_reg end as date_reg, "
                               "       case when e.id_addr_liv is NULL then -1 else e.id_addr_liv end as id_addr_liv, "
                               "       rep_id, rep_fio, rep_relation, dost, "
                               "       NULL, NULL, "
                               "       po.id as id_old, old_fam, old_im, old_ot, old_date_birth, old_plc_birth, old_enp, old_snils, old_sex, "
                               "       po.dt_ins as old_dt_ins, po.dt_upd as old_dt_upd, "
                               "       NULL, NULL, NULL, NULL, NULL, NULL, "
                               "       NULL, pol.pol_v, pol.f_polis, pol.vs_num, pol.enp, pol._id_last_point, "
                               "       points.point_regnum, points.point_name, pol._id_last_operator, operators.oper_fio, pol.date_get2hand "
                               "  from persons e "
                               "       left join spr_oksm o2b on(e.oksm_birth=o2b.code) "
                               "       left join spr_oksm o2c on(e.oksm_c=o2c.code) "
                               "       left join spr_v013 v13 on(e.category=v13.code) "
                               "       left join persons_old po on(po.id_person=e.id) "
            // действующий полис
                               "       left join polises pol on(pol.id=e.id_polis) "
                               "       left join polises pol_p  on(pol_p.id=e.id_polis  and pol_p.pol_v=3) "
                               "       left join polises pol_vs on(pol_vs.id=e.id_polis and pol_vs.pol_v=2) "
                               "       left join points on(points.id=pol._id_last_point) "
                               "       left join operators on(operators.id=pol._id_last_operator) ";
    if ( ui->ch_pers_fam->isChecked()   ||
         ui->ch_pers_im->isChecked()    ||
         ui->ch_pers_ot->isChecked()    ||
         ui->ch_pers_sex->isChecked()   ||
         ui->ch_pers_birth->isChecked() ||
         ui->ch_pers_snils->isChecked() ) {

        representers_sql += " WHERE /*st_person(e.id, CURRENT_DATE)>-100*/ ";
        bool f = false;
        if (ui->ch_pers_fam->isChecked()) {
            representers_sql += QString(f?" and ":" ") + " upper(e.fam) like(upper('" + ui->line_pers_fam->text() + "%')) ";
            f = true;
        }
        if (ui->ch_pers_im->isChecked()) {
            representers_sql += QString(f?" and ":" ") + " upper(e.im) like(upper('" + ui->line_pers_im->text() + "%')) ";
            f = true;
        }
        if (ui->ch_pers_ot->isChecked()) {
            representers_sql += QString(f?" and ":" ") + " upper(e.ot) like(upper('" + ui->line_pers_ot->text() + "%')) ";
            f = true;
        }
        if (ui->ch_pers_sex->isChecked()) {
            representers_sql += QString(f?" and ":" ") + " e.sex=" + QString::number(ui->combo_pers_sex->currentIndex()) + " ";
            f = true;
        }
        if (ui->ch_pers_birth->isChecked()) {
            representers_sql += QString(f?" and ":" ") + " e.date_birth='" + ui->date_pers_birth->date().toString("dd.MM.yyyy") + "' ";
            f = true;
        }
        if (ui->ch_pers_snils->isChecked()) {
            representers_sql += QString(f?" and ":" ") + " upper(e.snils) like(upper('" + ui->line_pers_snils->text() + "%')) ";
            f = true;
        }
    }
    representers_sql += " order by e.fam, e.im, e.ot, e.date_birth ; ";

    model_representers.setQuery(representers_sql,db);
    QString err2 = model_representers.lastError().driverText();

    // подключаем модель из БД
    ui->tab_representers->setModel(&model_representers);

    // обновляем таблицу
    ui->tab_representers->reset();

    // задаём ширину колонок
    ui->tab_representers->setColumnWidth( 0,  1);     // id,
    ui->tab_representers->setColumnWidth( 1, 90);     // fam,
    ui->tab_representers->setColumnWidth( 2, 90);     // im,
    ui->tab_representers->setColumnWidth( 3, 90);     // ot,
    ui->tab_representers->setColumnWidth( 4, 35);     // sex,
    ui->tab_representers->setColumnWidth( 5, 70);     // date_birth,
    ui->tab_representers->setColumnWidth( 6,  1);     // plc_birth,
    ui->tab_representers->setColumnWidth( 7,  1);     // date_death,
    ui->tab_representers->setColumnWidth( 8, 80);     // snils,
    ui->tab_representers->setColumnWidth( 9,  1);     // category,
    ui->tab_representers->setColumnWidth(10,200);     // categ_text,
    ui->tab_representers->setColumnWidth(11, 50);     // bomj,
    ui->tab_representers->setColumnWidth(12, 85);     // phone_cell,
    ui->tab_representers->setColumnWidth(13,  1);     // phone_code,
    ui->tab_representers->setColumnWidth(14,  1);     // phone_home,
    ui->tab_representers->setColumnWidth(15,  1);     // phone_work,
    ui->tab_representers->setColumnWidth(16,  1);     // email,
    ui->tab_representers->setColumnWidth(17,  1);     // oksm_b,
    ui->tab_representers->setColumnWidth(18, 80);     // oksm_b_name,
    ui->tab_representers->setColumnWidth(19,  1);     // oksm_c,
    ui->tab_representers->setColumnWidth(20, 80);     // oksm_c_name,
    ui->tab_representers->setColumnWidth(21,  1);     // org_id,
    ui->tab_representers->setColumnWidth(22,200);     // org_name,
    ui->tab_representers->setColumnWidth(24,  1);     // id_addr_reg,
    ui->tab_representers->setColumnWidth(25,  1);     // date_reg,
    ui->tab_representers->setColumnWidth(26,  1);     // id_addr_liv,
    ui->tab_representers->setColumnWidth(27,  1);     // rep_id,
    ui->tab_representers->setColumnWidth(28,  1);     // rep_fio,
    ui->tab_representers->setColumnWidth(29,  1);     // rep_relation,
    ui->tab_representers->setColumnWidth(30,  1);     // dost,
    ui->tab_representers->setColumnWidth(31,  1);     // po.id as id_old,
    ui->tab_representers->setColumnWidth(32,  1);     // old_fam,
    ui->tab_representers->setColumnWidth(33,  1);     // old_im,
    ui->tab_representers->setColumnWidth(34,  1);     // old_ot,
    ui->tab_representers->setColumnWidth(35,  1);     // old_date_birth,
    ui->tab_representers->setColumnWidth(36,  1);     // old_plc_birth,
    ui->tab_representers->setColumnWidth(37,  1);     // old_enp,
    ui->tab_representers->setColumnWidth(38,  1);     // old_snils,
    ui->tab_representers->setColumnWidth(39,  1);     // old_sex,
    ui->tab_representers->setColumnWidth(40,  1);     // old_dt_ins,
    ui->tab_representers->setColumnWidth(41,  1);     // old_dt_upd,

    // правим заголовки
    model_representers.setHeaderData( 1, Qt::Horizontal, ("фамилия"));
    model_representers.setHeaderData( 2, Qt::Horizontal, ("имя"));
    model_representers.setHeaderData( 3, Qt::Horizontal, ("отчество"));
    model_representers.setHeaderData( 4, Qt::Horizontal, ("пол"));
    model_representers.setHeaderData( 5, Qt::Horizontal, ("дата\nрождения"));
    model_representers.setHeaderData( 6, Qt::Horizontal, ("место рождения"));
    model_representers.setHeaderData( 7, Qt::Horizontal, ("дата\nсмерти"));
    model_representers.setHeaderData( 8, Qt::Horizontal, ("СНИЛС"));
    model_representers.setHeaderData(10, Qt::Horizontal, ("категория"));
    model_representers.setHeaderData(11, Qt::Horizontal, ("БОМЖ?"));
    model_representers.setHeaderData(12, Qt::Horizontal, ("телефон"));
    model_representers.setHeaderData(16, Qt::Horizontal, ("e-mail"));
    model_representers.setHeaderData(17, Qt::Horizontal, ("ОКСМ\nрожд"));
    model_representers.setHeaderData(18, Qt::Horizontal, ("страна\nрождения"));
    model_representers.setHeaderData(19, Qt::Horizontal, ("ОКСМ\nпрож"));
    model_representers.setHeaderData(20, Qt::Horizontal, ("гражд-во"));
    model_representers.setHeaderData(21, Qt::Horizontal, ("ID прдп."));
    model_representers.setHeaderData(22, Qt::Horizontal, ("предприятие"));
    model_representers.setHeaderData(24, Qt::Horizontal, ("ID а.рег"));
    model_representers.setHeaderData(25, Qt::Horizontal, ("дат рег"));
    model_representers.setHeaderData(26, Qt::Horizontal, ("ID а.прж."));
    model_representers.setHeaderData(27, Qt::Horizontal, ("ID предст."));
    model_representers.setHeaderData(28, Qt::Horizontal, ("ФИО представителя"));
    model_representers.setHeaderData(29, Qt::Horizontal, ("отношение\nпредст."));
    model_representers.setHeaderData(30, Qt::Horizontal, ("DOST"));
    ui->tab_representers->repaint();

    //refresh_assigs_tab();
    //refresh_links_tab();
    //refresh_addresses_tab();
    this->setCursor(Qt::ArrowCursor);
}

void representers_wnd::refresh_pers_data() {
    QDate dd_min = QDate(1920,1,1);

    ui->ch_dost->setChecked(false);
    ui->pan_dost->setVisible(false);

    refresh_pers_pan_category();
    refresh_docs_pan_type();

    if (data_pers.id<0) {

        ui->ln_pers_pan_fam->setText("");
        ui->ln_pers_pan_im->setText("");
        ui->ln_pers_pan_ot->setText("");
        ui->line_pers_pan_snils->setText("");
        ui->ln_pers_pan_plc_birth->setText("");

        ui->ln_pers_pan_fam_old->setText("");
        ui->ln_pers_pan_im_old->setText("");
        ui->ln_pers_pan_ot_old->setText("");
        ui->line_pers_pan_snils_old->setText("");
        ui->ln_pers_pan_plc_birth_old->setText("");
        ui->combo_pers_pan_sex_old->setCurrentIndex(0);
        ui->date_pers_pan_birth_old->setDate(QDate(1900,1,1));

        ui->ln_pers_pan_phone_code->setText("");
        ui->ln_pers_pan_phone_work->setText("");
        ui->ln_pers_pan_phone_home->setText("");
        ui->ln_pers_pan_phone_cell->setText("");
        ui->ln_pers_pan_email->setText("");
        ui->date_pers_pan_birth->setDate(QDate(1900,1,1));
        ui->date_pers_pan_death->setDate(QDate(1900,1,1));

        ui->spin_pers_pan_category->setValue(0);
        ui->combo_pers_pan_category->setCurrentIndex(0);

        // новый человек
        if (filter_pers.has_fam)
            ui->ln_pers_pan_fam->setText(filter_pers.fam);
        if (filter_pers.has_im)
            ui->ln_pers_pan_im->setText(filter_pers.im);
        if (filter_pers.has_ot)
            ui->ln_pers_pan_ot->setText(filter_pers.ot);

        if (filter_pers.has_sex)
            ui->combo_pers_pan_sex->setCurrentIndex(filter_pers.sex);
        if (filter_pers.has_birth)
            ui->date_pers_pan_birth->setDate(filter_pers.date_birth);
        if (filter_pers.has_ot)
            ui->line_pers_pan_snils->setText(filter_pers.snils);

        ui->ch_pers_pan_death->setChecked(false);
        ui->ch_pers_pan_snils->setChecked(true);

        ui->ch_pers_pan_ot->setChecked(true);

        ui->lab_pers_pan_dost->setText( "______" );
        ui->ch_dost_fam->setChecked(true);
        ui->ch_dost_im ->setChecked(true);
        ui->ch_dost_ot ->setChecked(true);
        ui->ch_dost_day->setChecked(true);
        ui->ch_dost_mon->setChecked(true);
        ui->ch_dost_year->setChecked(true);

        ui->ch_pers_pan_fam_old->setChecked(false);
        ui->ch_pers_pan_im_old->setChecked(false);
        ui->ch_pers_pan_ot_old->setChecked(false);
        ui->ch_pers_pan_sex_old->setChecked(false);
        ui->ch_pers_pan_date_birth_old->setChecked(false);
        ui->ch_pers_pan_snils_old->setChecked(false);
        ui->ch_pers_pan_plc_birth_old->setChecked(false);

        on_ch_pers_pan_ot_stateChanged(ui->ch_pers_pan_ot->isChecked() ? 2 : 0);

        on_ch_pers_pan_snils_stateChanged(ui->ch_pers_pan_snils->isChecked() ? 2 : 0);
        on_ch_pers_pan_death_stateChanged(ui->ch_pers_pan_death->isChecked() ? 2 : 0);

    } else {
        // данные человека - из его свойств
        ui->ln_pers_pan_fam->setText(data_pers.fam);
        ui->ln_pers_pan_im->setText(data_pers.im);
        ui->ch_pers_pan_ot->setChecked(!data_pers.ot.trimmed().isEmpty());
        ui->ln_pers_pan_ot->setText(data_pers.ot);
        //ui->lab_pers_pan_dost->setText(data_pers.dost);
        ui->lab_pers_pan_dost->setText( data_pers.dost );
        ui->ch_dost_fam->setChecked(data_pers.dost.indexOf("2")<0);
        ui->ch_dost_im ->setChecked(data_pers.dost.indexOf("3")<0);
        ui->ch_dost_ot ->setChecked(data_pers.dost.indexOf("1")<0);
        ui->ch_dost_day->setChecked(data_pers.dost.indexOf("4")<0);
        ui->ch_dost_mon->setChecked(data_pers.dost.indexOf("5")<0);
        ui->ch_dost_year->setChecked(data_pers.dost.indexOf("6")<0);


        ui->combo_pers_pan_sex->setCurrentIndex(data_pers.sex);
        ui->date_pers_pan_birth->setDate(data_pers.date_birth);
        ui->ch_pers_pan_snils->setChecked(!data_pers.snils.trimmed().isEmpty());
        ui->line_pers_pan_snils->setText(data_pers.snils);

        ui->ch_pers_pan_death->setChecked(data_pers.date_death>dd_min);
        ui->date_pers_pan_death->setDate(data_pers.date_death);

        ui->ln_pers_pan_plc_birth->setText(data_pers.plc_birth);
        ui->spin_pers_pan_category->setValue(data_pers.category);
        ui->combo_pers_pan_category->setCurrentIndex(ui->combo_pers_pan_category->findData(data_pers.category));

        ui->ch_pers_pan_fam_old->setChecked( !(data_pers.fam_old.isEmpty()) );
        ui->ln_pers_pan_fam_old->setText(data_pers.fam_old);
        ui->ch_pers_pan_im_old->setChecked( !(data_pers.im_old.isEmpty()) );
        ui->ln_pers_pan_im_old->setText(data_pers.im_old);
        ui->ch_pers_pan_ot_old->setChecked( !(data_pers.ot_old.isEmpty()) );
        ui->ln_pers_pan_ot_old->setText(data_pers.ot_old);
        ui->ch_pers_pan_sex_old->setChecked(data_pers.sex_old>0 && data_pers.sex_old<3);
        ui->combo_pers_pan_sex_old->setCurrentIndex(data_pers.sex_old);
        ui->ch_pers_pan_date_birth_old->setChecked(data_pers.date_birth_old>QDate(1920,1,1));
        ui->date_pers_pan_birth_old->setDate(data_pers.date_birth_old);
        ui->ch_pers_pan_snils_old->setChecked( !(data_pers.snils_old.isEmpty()) );
        ui->line_pers_pan_snils_old->setText(data_pers.snils_old);
        ui->ch_pers_pan_plc_birth_old->setChecked( !(data_pers.plc_birth_old.isEmpty()) );
        ui->ln_pers_pan_plc_birth_old->setText(data_pers.plc_birth_old);

        ui->ln_pers_pan_phone_code->setText(data_pers.phone_code);
        ui->ln_pers_pan_phone_cell->setText(data_pers.phone_cell);
        ui->ln_pers_pan_phone_home->setText(data_pers.phone_home);
        ui->ln_pers_pan_phone_work->setText(data_pers.phone_work);
        ui->ln_pers_pan_email->setText(data_pers.email);

        if (data_pers.has_fam_old)
              ui->ln_pers_pan_fam_old->setText(data_pers.fam_old);
        else  ui->ln_pers_pan_fam_old->setText("");
        if (data_pers.has_im_old)
            ui->ln_pers_pan_im_old->setText(data_pers.im_old);
        else  ui->ln_pers_pan_im_old->setText("");
        if (data_pers.has_ot_old)
            ui->ln_pers_pan_ot_old->setText(data_pers.ot_old);
        else  ui->ln_pers_pan_ot_old->setText("");
        if (data_pers.has_sex_old)
            ui->combo_pers_pan_sex_old->setCurrentIndex(data_pers.sex_old);
        else  ui->combo_pers_pan_sex_old->setCurrentIndex(0);
        if (data_pers.has_date_birth_old)
            ui->date_pers_pan_birth_old->setDate(data_pers.date_birth_old);
        else  ui->date_pers_pan_birth_old->setDate(QDate(1900,1,1));
        if (data_pers.has_snils_old)
            ui->line_pers_pan_snils_old->setText(data_pers.snils_old);
        else  ui->line_pers_pan_snils_old->setText("");
        if (data_pers.has_plc_birth_old)
            ui->ln_pers_pan_plc_birth_old->setText(data_pers.plc_birth_old);
        else  ui->ln_pers_pan_plc_birth_old->setText("");

        on_ch_pers_pan_ot_stateChanged(ui->ch_pers_pan_ot->isChecked() ? 2 : 0);
        on_ch_pers_pan_snils_stateChanged(ui->ch_pers_pan_snils->isChecked() ? 2 : 0);
        on_ch_pers_pan_death_stateChanged(ui->ch_pers_pan_death->isChecked() ? 2 : 0);

        // адрес регистрации
        this->setCursor(Qt::WaitCursor);
        QSqlQuery *query_reg = new QSqlQuery(db);
        QString sql_reg = "select post_index,ocato,ocato_full, "
                          "     cast(substr(kladr,1,2) as integer) as subj, "
                          "     cast(substr(kladr,3,3) as integer) as dstr, "
                          "     cast(substr(kladr,6,3) as integer) as city, "
                          "     cast(substr(kladr,9,3) as integer) as nasp, "
                          "     case when (kladr_street is null or character_length(kladr_street)<12) then 0 else cast(substr(kladr_street,12,4) as integer) end as strt, "
                          "     house,corp,quart "
                      " from addresses "
                      " where id=" + QString::number(data_pers.id_reg_addr) + " ; ";
        bool res_reg = mySQL.exec(this, sql_reg, QString("Адрес регистрации"), *query_reg, true, db, data_app);
        if (!res_reg) {
            delete query_reg;
            this->setCursor(Qt::ArrowCursor);
            QMessageBox::warning(this, "Ошибка при чтении адреса", "При чтении адреса регистрации произошла неожиданная ошибка");
        } else {
            query_reg->next();
            QString reg_post_index = query_reg->value(0).toString();
            int reg_subj = query_reg->value(3).toInt();
            int reg_dstr = query_reg->value(4).toInt();
            int reg_city = query_reg->value(5).toInt();
            int reg_nasp = query_reg->value(6).toInt();
            int reg_strt = query_reg->value(7).toInt();
            QString reg_house = QString::number(query_reg->value(8).toInt());
            QString reg_corp = query_reg->value(9).toString();
            QString reg_quart = query_reg->value(10).toString();

            delete query_reg;
            this->setCursor(Qt::ArrowCursor);
        }

        // адрес проживания
        this->setCursor(Qt::WaitCursor);
        QSqlQuery *query_liv = new QSqlQuery(db);
        QString sql_liv = "select post_index,ocato,ocato_full, "
                          "     cast(substr(kladr,1,2) as integer) as subj, "
                          "     cast(substr(kladr,3,3) as integer) as dstr, "
                          "     cast(substr(kladr,6,3) as integer) as city, "
                          "     cast(substr(kladr,9,3) as integer) as nasp, "
                          "     case when (kladr_street is null or character_length(kladr_street)<12) then 0 else cast(substr(kladr_street,12,4) as integer) end as strt, "
                          "     house,corp,quart "
                      " from addresses "
                      " where id=" + QString::number(data_pers.id_liv_addr) + " ; ";
        bool res_liv = mySQL.exec(this, sql_liv, QString("Адрес проживания"), *query_liv, true, db, data_app);
        if (!res_liv) {
            delete query_liv;
            this->setCursor(Qt::ArrowCursor);
            QMessageBox::warning(this, "Ошибка при чтении адреса", "При чтении адреса проживания произошла неожиданная ошибка");
        } else {
            query_liv->next();
            QString liv_post_index = query_liv->value(0).toString();
            int liv_subj = query_liv->value(3).toInt();
            int liv_dstr = query_liv->value(4).toInt();
            int liv_city = query_liv->value(5).toInt();
            int liv_nasp = query_liv->value(6).toInt();
            int liv_strt = query_liv->value(7).toInt();
            QString liv_house = QString::number(query_liv->value(8).toInt());
            QString liv_corp = query_liv->value(9).toString();
            QString liv_quart = query_liv->value(10).toString();

            delete query_liv;
            this->setCursor(Qt::ArrowCursor);
        }
    }
    test_for_UDL();
}


void representers_wnd::refresh_pers_pan_category() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка категорий застрахованных
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, '(' || code || ') ' || text "
                  " from public.spr_v013 s "
                  " order by code ; ";
    mySQL.exec(this, sql, QString("Справочник категорий застрахованных"), *query, true, db, data_app);
    ui->combo_pers_pan_category->clear();
    ui->combo_pers_pan_category->addItem(" - выберите категорию - ", -1);
    while (query->next()) {
        ui->combo_pers_pan_category->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_pers_pan_category->setCurrentIndex(0);
    delete query;
    this->setCursor(Qt::ArrowCursor);

    // документы можно вводить только при выданной категории
    ui->pan_docs_tab->setEnabled(false);
}

void representers_wnd::refresh_docs_pan_type() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка типов документов
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, '('||code||') '||text, mask_ser, mask_num "
                  " from public.spr_f011 s "
                  " order by code ; ";
    mySQL.exec(this, sql, QString("Справочник типов документов"), *query, true, db, data_app);
    ui->combo_docs_pan_type->clear();
    ui->combo_docs_pan_type->addItem(" - выберите тип документа - ", 0);
    doc_ser_mask.clear();
    doc_num_mask.clear();
    doc_ser_mask.append("");
    doc_num_mask.append("");
    while (query->next()) {
        doc_ser_mask.append(query->value(2).toString());
        doc_num_mask.append(query->value(3).toString());
        ui->combo_docs_pan_type->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    data_doc.mask_num = doc_num_mask.at(0);
    data_doc.mask_ser = doc_ser_mask.at(0);
    ui->combo_docs_pan_type->setCurrentIndex(0);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void representers_wnd::on_combo_docs_pan_type_activated(int index) {
    ui->spin_docs_pan_type->setValue(ui->combo_docs_pan_type->currentData().toInt());
    data_doc.doc_type = ui->combo_docs_pan_type->currentData().toInt();
    data_doc.mask_num = doc_num_mask.at(index);
    data_doc.mask_ser = doc_ser_mask.at(index);
    ui->lab_docs_pan_mask->setText(data_doc.mask_ser + " № " + data_doc.mask_num);
}

void representers_wnd::on_bn_pers_refresh_clicked() {
    refresh_representers_tab();
}

void representers_wnd::on_line_pers_fam_textChanged(const QString &arg1) {
    ui->ch_pers_fam->setChecked(!arg1.isEmpty());
}

void representers_wnd::on_line_pers_im_textChanged(const QString &arg1) {
    ui->ch_pers_im->setChecked(!arg1.isEmpty());
}

void representers_wnd::on_line_pers_ot_textChanged(const QString &arg1) {
    ui->ch_pers_ot->setChecked(!arg1.isEmpty());
}

void representers_wnd::on_line_pers_snils_textChanged(const QString &arg1) {
    ui->ch_pers_snils->setChecked(!arg1.isEmpty());
}

void representers_wnd::on_combo_pers_sex_currentIndexChanged(int index) {
    ui->ch_pers_sex->setChecked(index>0);
}

void representers_wnd::on_date_pers_birth_dateChanged(const QDate &date) {
    ui->ch_pers_birth->setChecked(date>QDate(1900,1,1));
}

void representers_wnd::on_ch_pers_birth_clicked(bool checked) {
    if (!checked)
        ui->date_pers_birth->setDate(QDate(1900,1,1));
}

void representers_wnd::on_bn_pers_clear_clicked() {
    ui->line_pers_fam->clear();
    ui->line_pers_im->clear();
    ui->line_pers_ot->clear();
    ui->line_pers_snils->clear();
    ui->combo_pers_sex->setCurrentIndex(0);
    ui->date_pers_birth->setDate(QDate(1900,1,1));
}

void representers_wnd::on_bn_pers_new_clicked() {
    data_pers.id = -1;
    filter_pers.has_fam = ui->ch_pers_fam->isChecked();
    filter_pers.has_im = ui->ch_pers_im->isChecked();
    filter_pers.has_ot = ui->ch_pers_ot->isChecked();
    filter_pers.has_sex = ui->ch_pers_sex->isChecked();
    filter_pers.has_birth = ui->ch_pers_birth->isChecked();
    filter_pers.has_snils = ui->ch_pers_snils->isChecked();

    filter_pers.fam = ui->line_pers_fam->text();
    filter_pers.im = ui->line_pers_im->text();
    filter_pers.ot = ui->line_pers_ot->text();
    filter_pers.sex = ui->combo_pers_sex->currentIndex();
    filter_pers.date_birth = ui->date_pers_birth->date();
    filter_pers.snils = ui->line_pers_snils->text();

    data_pers.has_fam_old = false;
    data_pers.has_im_old = false;
    data_pers.has_ot_old = false;
    data_pers.has_death_old = false;
    data_pers.has_snils_old = false;
    data_pers.has_enp_old = false;
    data_pers.fam_old = "";
    data_pers.im_old = "";
    data_pers.ot_old = "";
    data_pers.snils_old = "";
    data_pers.plc_birth_old = "";
    data_pers.enp_old = "";
    data_pers.has_sex_old = false;
    data_pers.has_date_birth_old = false;
    data_pers.has_plc_birth_old = false;
    data_pers.has_date_death_old = false;
    data_pers.sex_old = 0;
    data_pers.date_birth_old = QDate(1900,1,1);
    data_pers.date_death_old = QDate(1900,1,1);

    // скроем таблицу персон, покажем панель свойств персоны и панель документов
    ui->show_pers_tabl->setChecked(false);
    ui->show_pers_data->setChecked(true);
    ui->show_pers_docs->setChecked(false);
    on_show_pers_tabl_clicked(false);
    on_show_pers_data_clicked(true);
    on_show_pers_docs_clicked(false);
    ui->show_pers_tabl->setEnabled(false);
    ui->show_pers_data->setEnabled(true);
    ui->show_pers_docs->setEnabled(true);

    refresh_pers_data();
    refresh_pers_docs();

    QRect screen = QApplication::desktop()->screenGeometry();
    if (screen.width()==1024 && screen.height()==768) {
        //this->setWindowState( polices_w->windowState() | Qt::WindowFullScreen);

        on_ch_phones_clicked(ui->ch_phones->isChecked());
        on_ch_person_old_clicked(ui->ch_person_old->isChecked());

    } else {
        //this->setWindowState( polices_w->windowState() | Qt::WindowFullScreen);
        //this->resize(screen.width()-15, screen.height()-40);
        //this->move(3,3);

        ui->ch_person->setChecked(true);
        ui->ch_person_old->setChecked(true);
        on_ch_person_old_clicked(true);
        ui->ch_phones->setChecked(true);
        on_ch_phones_clicked(true);

    }

    b_need_save(true);
}

void representers_wnd::on_bn_pers_select_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_representers || !ui->tab_representers->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Поиск] и выберите человека из списка.");
        return;
    }

    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_representers->selectionModel()->selection().indexes();

    if (indexes.size()>0) {
        this->setCursor(Qt::WaitCursor);
        QModelIndex index = indexes.front();

        // данные застрахованного
        data_pers.id         = model_representers.data(model_representers.index(index.row(), 0), Qt::EditRole).toInt();       // e.id
        data_pers.fam        = model_representers.data(model_representers.index(index.row(), 1), Qt::EditRole).toString();    // e.fam
        data_pers.im         = model_representers.data(model_representers.index(index.row(), 2), Qt::EditRole).toString();    // e.im
        data_pers.ot         = model_representers.data(model_representers.index(index.row(), 3), Qt::EditRole).toString();    // e.ot
        data_pers.sex        =(model_representers.data(model_representers.index(index.row(), 4), Qt::EditRole).isNull() ? 1 : model_representers.data(model_representers.index(index.row(), 4), Qt::EditRole).toInt());    // e.sex
        data_pers.date_birth = model_representers.data(model_representers.index(index.row(), 5), Qt::EditRole).toDate();      // e.date_birth
        data_pers.plc_birth  = model_representers.data(model_representers.index(index.row(), 6), Qt::EditRole).toString();    // e.plc_birth
        data_pers.date_death = model_representers.data(model_representers.index(index.row(), 7), Qt::EditRole).toDate();      // date_death
        data_pers.snils      = model_representers.data(model_representers.index(index.row(), 8), Qt::EditRole).toString();    // e.snils
        data_pers.category   = model_representers.data(model_representers.index(index.row(), 9), Qt::EditRole).toInt();       // e.category
        data_pers.bomj       =(model_representers.data(model_representers.index(index.row(),11), Qt::EditRole).isNull() ? 0 : model_representers.data(model_representers.index(index.row(),11), Qt::EditRole).toInt());    // e.bomj
        data_pers.phone_cell = model_representers.data(model_representers.index(index.row(),12), Qt::EditRole).toString();    // e.phone_cell
        data_pers.phone_code = model_representers.data(model_representers.index(index.row(),13), Qt::EditRole).toString();    // e.phone_code
        data_pers.phone_home = model_representers.data(model_representers.index(index.row(),14), Qt::EditRole).toString();    // e.phone_home
        data_pers.phone_work = model_representers.data(model_representers.index(index.row(),15), Qt::EditRole).toString();    // e.phone_work
        data_pers.email      = model_representers.data(model_representers.index(index.row(),16), Qt::EditRole).toString();    // e.email
        data_pers.oksm_birth = model_representers.data(model_representers.index(index.row(),17), Qt::EditRole).toInt();       // oksm_b
        // oksm_b_name
        data_pers.oksm_c     = model_representers.data(model_representers.index(index.row(),20), Qt::EditRole).toInt();       // oksm_c
        // oksm_c_name
        // firm.firm_name
        data_pers.id_reg_addr=(model_representers.data(model_representers.index(index.row(),24), Qt::EditRole).isNull() ? -1 : model_representers.data(model_representers.index(index.row(),23), Qt::EditRole).toInt());    // id_addr_reg
        data_pers.has_reg_addr= (data_pers.id_reg_addr>0);
        data_pers.reg_date   = model_representers.data(model_representers.index(index.row(),25), Qt::EditRole).toDate();      // date_reg
        data_pers.id_liv_addr=(model_representers.data(model_representers.index(index.row(),26), Qt::EditRole).isNull() ? -1 : model_representers.data(model_representers.index(index.row(),25), Qt::EditRole).toInt());    // id_addr_liv
        data_pers.has_liv_addr= (data_pers.id_liv_addr>0);
        data_pers.rep_id     =(model_representers.data(model_representers.index(index.row(),27), Qt::EditRole).isNull() ? -1 : model_representers.data(model_representers.index(index.row(),26), Qt::EditRole).toInt());    // rep_id
        data_pers.rep_fio    = model_representers.data(model_representers.index(index.row(),28), Qt::EditRole).toString();    // rep_fio
        data_pers.rep_relation=model_representers.data(model_representers.index(index.row(),29), Qt::EditRole).toInt();       // rep_relation
        QString dost         = model_representers.data(model_representers.index(index.row(),30), Qt::EditRole).toString().replace("0","") + "_______";
        data_pers.dost       = dost.left(6) ;
        data_pers.id_old     = model_representers.data(model_representers.index(index.row(),31), Qt::EditRole).toInt();       // po.id as id_old,
        data_pers.fam_old    = model_representers.data(model_representers.index(index.row(),32), Qt::EditRole).toString();    // old_fam,
        data_pers.has_fam_old= !(data_pers.fam_old.isEmpty());
        data_pers.im_old     = model_representers.data(model_representers.index(index.row(),33), Qt::EditRole).toString();    // old_im,
        data_pers.has_im_old= !(data_pers.im_old.isEmpty());
        data_pers.ot_old     = model_representers.data(model_representers.index(index.row(),34), Qt::EditRole).toString();    // old_ot,
        data_pers.has_ot_old= !(data_pers.ot_old.isEmpty());
        data_pers.date_birth_old= model_representers.data(model_representers.index(index.row(),35), Qt::EditRole).toDate();   // old_date_birth,
        data_pers.has_date_birth_old= data_pers.date_birth_old>QDate(1920,1,1);
        data_pers.plc_birth_old= model_representers.data(model_representers.index(index.row(),36), Qt::EditRole).toString();  // old_plc_birth,
        data_pers.has_plc_birth_old= !(data_pers.plc_birth_old.isEmpty());
        data_pers.enp_old    = model_representers.data(model_representers.index(index.row(),37), Qt::EditRole).toString();    // old_enp,
        data_pers.has_enp_old= !(data_pers.enp_old.isEmpty());
        data_pers.snils_old  = model_representers.data(model_representers.index(index.row(),38), Qt::EditRole).toString();    // old_snils,
        data_pers.has_snils_old= !(data_pers.snils_old.isEmpty());
        data_pers.sex_old    = model_representers.data(model_representers.index(index.row(),39), Qt::EditRole).toInt();       // old_sex,
        data_pers.has_sex_old= (data_pers.sex_old>0 && data_pers.sex_old<3);
        data_pers.dt_ins_old = model_representers.data(model_representers.index(index.row(),40), Qt::EditRole).toDateTime();  // old_dt_ins,
        data_pers.dt_upd_old = model_representers.data(model_representers.index(index.row(),41), Qt::EditRole).toDateTime();  // old_dt_upd,

        // скроем таблицу персон, покажем панель свойств персоны и панель документов
        ui->show_pers_tabl->setChecked(false);
        ui->show_pers_data->setChecked(true);
        ui->show_pers_docs->setChecked(true);
        on_show_pers_tabl_clicked(false);
        on_show_pers_data_clicked(true);
        on_show_pers_docs_clicked(true);
        ui->show_pers_tabl->setEnabled(false);
        ui->show_pers_data->setEnabled(true);
        ui->show_pers_docs->setEnabled(true);

        refresh_pers_data();
        refresh_pers_docs();
        this->setCursor(Qt::ArrowCursor);

        ui->ln_pers_pan_fam->setFocus();

        ui->ch_person_old->setChecked( !(data_pers.fam_old.isEmpty() &&
                                         data_pers.im_old.isEmpty() &&
                                         data_pers.ot_old.isEmpty() &&
                                         data_pers.sex_old!=1 && data_pers.sex_old!=2 &&
                                         data_pers.date_birth_old<QDate(1920,1,1) &&
                                         data_pers.snils_old.isEmpty() &&
                                         data_pers.plc_birth_old.isEmpty()) );
        on_ch_person_old_clicked(ui->ch_person_old->isChecked());
        ui->ch_phones->setChecked(true);
        on_ch_phones_clicked(ui->ch_phones->isChecked());

        b_need_save(false);
    } else {
        QMessageBox::warning(this, "Ничего не выбрано!", "Ничего не выбрано!");
    }
}

void representers_wnd::on_tab_representers_activated(const QModelIndex &index) {
    on_bn_pers_select_clicked();
}

// активация/отключение поля отчества
void representers_wnd::on_ch_pers_pan_ot_stateChanged ( int state ) {
    ui->ln_pers_pan_ot->setEnabled(state>0);
    if (state==0)
        ui->ln_pers_pan_ot->clear();
}

// активация/отключение поля СНИЛС
void representers_wnd::on_ch_pers_pan_snils_stateChanged ( int state ) {
    ui->line_pers_pan_snils->setEnabled(state>0);
    if (state==0)
        ui->line_pers_pan_snils->clear();
}

// активация/отключение поля даты смерти
void representers_wnd::on_ch_pers_pan_death_stateChanged ( int state ) {
    ui->date_pers_pan_death->setEnabled(state>0);
    ui->date_pers_pan_death->setVisible(state>0);
    if (state==0)
        ui->date_pers_pan_death->setDate(QDate(1900,1,1));
}




void representers_wnd::on_bn_pers_save_clicked() {
    if ( !(ui->bn_pers_save->isVisible()) )
        return;

    try {
        QDate dd_min = QDate(1920,1,1);
        QDate dd_max = QDate::currentDate();

        // проверка заполнения полей
        if ( ui->ln_pers_pan_fam->text().isEmpty()
             && ui->ch_dost_fam->isChecked() ) {
            QMessageBox::warning(this, "Не хватает данных!", "Поле \"Фамилия\" не может быть пустым.\nДобавьте недостающие данные.");
            return;
        }
        if ( ui->ln_pers_pan_im->text().isEmpty()
             && ui->ch_dost_im->isChecked() ) {
            QMessageBox::warning(this, "Не хватает данных!", "Поле \"Имя\" не может быть пустым.\nДобавьте недостающие данные.");
            return;
        }
        if ( ui->ch_pers_pan_ot->isChecked()
             && ui->ln_pers_pan_ot->text().isEmpty() ) {
            QMessageBox::warning(this, "Не хватает данных!", "Введите отчество застрахованного лица \nили, если отчества нет, снимите флажок \"Отчество\".");
            return;
        }
        if (ui->combo_pers_pan_sex->currentIndex()==0) {
            QMessageBox::warning(this, "Не хватает данных!", "Выберите пол застрахованного (М/Ж).");
            return;
        }
        if (ui->date_pers_pan_birth->date()<dd_min) {
            QMessageBox::warning(this, "Не хватает данных!", "Дата рождения застрахованного не может быть раньше 1 января 1900 г.");
            return;
        }
        if (ui->date_pers_pan_birth->date()>dd_max) {
            QMessageBox::warning(this, "Не хватает данных!", "Дата, указанная в качестве даты рождения застрахованного ещё не наступила.");
            return;
        }
        if (ui->ch_pers_pan_snils->isChecked() && ui->line_pers_pan_snils->text().isEmpty()) {
            QMessageBox::warning(this, "Не хватает данных!", "Введите номер СНИЛС застрахованного лица в формате 99999999999 \nили, если отчества нет, снимите флажок \"СНИЛС\".");
            return;
        }
        if (ui->ch_pers_pan_death->isChecked() && ui->date_pers_pan_death->date()<=dd_min) {
            QMessageBox::warning(this, "Не хватает данных!", "Укажите дату смерти застрахованного позже 1 января 1920 г. или снимите флажок \"Дата смерти\".");
            return;
        }
        if (ui->ch_pers_pan_death->isChecked() && ui->date_pers_pan_death->date()>dd_max) {
            QMessageBox::warning(this, "Не хватает данных!", "Дата, указанная в качестве даты смерти застрахованного, ещё не наступила.");
            return;
        }
        if (ui->ln_pers_pan_plc_birth->text().isEmpty()) {
            QMessageBox::warning(this, "Не хватает данных!", "Поле \"Место рождения\" не может быть пустым. \nУкажите место рождения застрахованного полностью так же как это написано в паспорте.");
            return;
        }
        if (ui->ch_pers_pan_death->isChecked() && ui->date_pers_pan_birth->date()>ui->date_pers_pan_death->date()) {
            QMessageBox::warning(this, "Не хватает данных!", "Указанная дата смерти застрахованного раньше его даты рождения! \nЧеловек не может быть умереть до того как он родится.");
            return;
        }
        QString cell_s = ui->ln_pers_pan_phone_cell->text().replace("+7 (","").replace(" ","").replace(")","").replace("-","");
        if (cell_s.isEmpty()) {
            QMessageBox::warning(this, "Не хватает данных!", "Укажите номер личного (сотового) телефона для обратной связи.");
            return;
        }

        QString fam_old_s = ui->ln_pers_pan_fam_old->text().trimmed();
        if (ui->ch_pers_pan_fam_old->isChecked() && fam_old_s.isEmpty()) {
            QMessageBox::warning(this, "Не хватает данных!", "Укажите старую фамилию застрахованного или снимите флажок \"старая фамилия\".");
            return;
        }
        QString im_old_s = ui->ln_pers_pan_im_old->text().trimmed();
        if (ui->ch_pers_pan_im_old->isChecked() && im_old_s.isEmpty()) {
            QMessageBox::warning(this, "Не хватает данных!", "Укажите старое имя застрахованного или снимите флажок \"старое имя\".");
            return;
        }
        QString ot_old_s = ui->ln_pers_pan_ot_old->text().trimmed();
        if (ui->ch_pers_pan_ot_old->isChecked() && ot_old_s.isEmpty()) {
            QMessageBox::warning(this, "Не хватает данных!", "Укажите старое отчество застрахованного или снимите флажок \"старое отчество\".");
            return;
        }
        int sex_old_i = ui->combo_pers_pan_sex_old->currentIndex();
        if (ui->ch_pers_pan_sex_old->isChecked() && (sex_old_i>2 || sex_old_i<1) ) {
            QMessageBox::warning(this, "Не хватает данных!", "Укажите старый пол застрахованного или снимите флажок \"старый пол\".");
            return;
        }
        QDate birth_d = ui->date_pers_pan_birth_old->date();
        if (ui->ch_pers_pan_date_birth_old->isChecked() && (birth_d>QDate::currentDate() || birth_d<QDate(1900,1,1)) ) {
            QMessageBox::warning(this, "Не хватает данных!", "Укажите старую дату рождения застрахованного или снимите флажок \"старая дата рождения\".");
            return;
        }
        QString snils_old_s = ui->line_pers_pan_snils_old->text().trimmed().replace(" ","").replace("-","");
        if (ui->ch_pers_pan_snils_old->isChecked() && snils_old_s.isEmpty()) {
            QMessageBox::warning(this, "Не хватает данных!", "Укажите старый СНИЛС застрахованного или снимите флажок \"старый СНИЛС\".");
            return;
        }
        QString plc_birth_s = ui->ln_pers_pan_plc_birth_old->text().trimmed();
        if (ui->ch_pers_pan_plc_birth_old->isChecked() && plc_birth_s.isEmpty()) {
            QMessageBox::warning(this, "Не хватает данных!", "Укажите старое место рождения застрахованного или снимите флажок \"старое место рождения\".");
            return;
        }

        // применение настроек DOST
        data_pers.dost = QString(data_pers.dost + "______").left(6);
        if (data_pers.dost!="______") {
            QString dost_str = "В соответствии с настройками надёжности идентификации застрахованного \n(код DOST)\n\n";
            if (data_pers.dost.indexOf("1")>=0)
                dost_str += "  -  ОТЧЕСТВО не известно\n";
            if (data_pers.dost.indexOf("2")>=0)
                dost_str += "  -  ФАМИЛИЯ не известна\n";
            if (data_pers.dost.indexOf("3")>=0)
                dost_str += "  -  ИМЯ не известно\n";
            if (data_pers.dost.indexOf("4")>=0)
                dost_str += "  -  ДЕНЬ ДАТЫ РОЖДЕНИЯ не известен\n";
            if (data_pers.dost.indexOf("5")>=0)
                dost_str += "  -  МЕСЯЦ ДАТЫ РОЖДЕНИЯ не известен\n";
            if (data_pers.dost.indexOf("6")>=0)
                dost_str += "  -  дата рождения не соответствует календарю \n    (надо ввести ближайшую корректную дату)\n";
            dost_str += "\nПрименить соответствующие изменения к полям данных?\n";

            if (QMessageBox::question(this, "Нужно подтверждение ограничения надёжности данных", dost_str, QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel)==QMessageBox::Cancel) {
                QMessageBox::information(this, "Действие отменено", "Сохранение отменено пользователем.");
                return;
            }
            if (data_pers.dost.indexOf("1")>=0) {
                data_pers.ot = "";
                ui->ln_pers_pan_ot->clear();
            }
            if (data_pers.dost.indexOf("2")>=0) {
                data_pers.fam = "";
                ui->ln_pers_pan_fam->clear();
            }
            if (data_pers.dost.indexOf("3")>=0) {
                data_pers.im = "";
                ui->ln_pers_pan_im->clear();
            }
            if (data_pers.dost.indexOf("4")>=0) {
                data_pers.date_birth.setDate(data_pers.date_birth.year(), data_pers.date_birth.month(), 1);
                ui->date_pers_pan_birth->setDate(data_pers.date_birth);
            }
            if (data_pers.dost.indexOf("5")>=0) {
                data_pers.date_birth.setDate(data_pers.date_birth.year(), 1, data_pers.date_birth.day());
                ui->date_pers_pan_birth->setDate(data_pers.date_birth);
            }
        }

        /*if (!ui->ln_pers_pan_fam_old->text().trimmed().isEmpty() ||
            !ui->ln_pers_pan_im_old->text().trimmed().isEmpty() ||
            !ui->ln_pers_pan_ot_old->text().trimmed().isEmpty() ||
            !ui->combo_pers_pan_sex_old->currentIndex()>0 ||
            !ui->ln_pers_pan_fam_old->text().trimmed().isEmpty() ||
            !ui->ln_pers_pan_fam_old->text().trimmed().isEmpty() ||
            !ui->ln_pers_pan_fam_old->text().trimmed().isEmpty() ||
                                ) {
            // запрос подтверждения
            // проверка наличия такой же записи
            // сохранение записи
        }*/

        db.transaction();

        if ( ui->ch_pers_pan_date_birth_old->isChecked()
             && data_pers.date_birth_old<=QDate(1920,1,1) ) {
            ui->ch_pers_pan_date_birth_old->setChecked(false);
            data_pers.date_birth_old = data_pers.date_birth;
        }

        // сохранение/правка персоны
        QString snils = ui->line_pers_pan_snils->text().replace("-","").replace(" ","").trimmed();
        if (data_pers.id==-1) {
            // сохраним запись в базу
            QString sql = "insert into persons(fam,im,ot,sex,category,date_birth,plc_birth,date_death,snils,bomj,id_addr_reg,date_reg,id_addr_liv,oksm_birth,oksm_c,rep_id,rep_fio,rep_relation,dost, phone_code, phone_work, phone_home, phone_cell, email, _id_first_point, _id_first_operator, _id_last_point, _id_last_operator) ";
            sql += " values (";
            sql += " '" + ui->ln_pers_pan_fam->text().trimmed().toUpper() + "', ";
            sql += " '" + ui->ln_pers_pan_im->text().trimmed().toUpper() + "', ";
            sql += " "  + QString(ui->ch_pers_pan_ot->isChecked() ? QString("'" + ui->ln_pers_pan_ot->text().trimmed().toUpper() + "', ") : "NULL, ");
            sql += " "  + QString::number(ui->combo_pers_pan_sex->currentIndex()) + ", ";
            sql += " "  + QString::number(ui->combo_pers_pan_category->currentData().toInt()) + ", ";
            sql += " '" + ui->date_pers_pan_birth->date().toString("yyyy-MM-dd") + "', ";
            sql += " '" + ui->ln_pers_pan_plc_birth->text().trimmed().toUpper() + "', ";
            sql += " "  + QString(ui->ch_pers_pan_death->isChecked() ? ("'" + ui->date_pers_pan_death->date().toString("yyyy-MM-dd") + "', ") : "NULL, ");
            sql += " "  + QString(ui->ch_pers_pan_snils->isChecked() ? ("'" + snils + "', ") : "NULL, ");
            sql += " 0, ";
            sql += " NULL, ";
            sql += " NULL, ";
            sql += " NULL, ";
            sql += " NULL, ";
            sql += " NULL, ";
            sql += " NULL, ";
            sql += " NULL, ";
            sql += " NULL, ";
            sql += " " + (data_pers.dost=="______" ? QString(" NULL ") : " '" + QString(data_pers.dost + "______").left(6) + "' ") + ", ";
            QString phone_code = ui->ln_pers_pan_phone_code->text().trimmed();
            QString phone_work = ui->ln_pers_pan_phone_work->text().trimmed().replace(" ","").replace("-","").replace("(","").replace(")","").replace("+","");
            QString phone_home = ui->ln_pers_pan_phone_home->text().trimmed().replace(" ","").replace("-","").replace("(","").replace(")","").replace("+","");
            QString phone_cell = ui->ln_pers_pan_phone_cell->text().trimmed().replace(" ","").replace("-","").replace("(","").replace(")","").replace("+","");
            QString email = ui->ln_pers_pan_email->text().trimmed();
            sql += " " + (phone_code.isEmpty()?"NULL": ("'"+phone_code+"'")) + ", ";
            sql += " " + (phone_work.isEmpty()?"NULL": ("'"+phone_work+"'")) + ", ";
            sql += " " + (phone_home.isEmpty()?"NULL": ("'"+phone_home+"'")) + ", ";
            sql += " " + (phone_cell.isEmpty()?"NULL": ("'"+phone_cell+"'")) + ", ";
            sql += " " + (email.isEmpty()?"NULL": ("'"+email+"'")) + ", ";
            sql += " " + QString::number(data_app.id_point)    + ", ";
            sql += " " + QString::number(data_app.id_operator) + ", ";
            sql += " " + QString::number(data_app.id_point)    + ", ";
            sql += " " + QString::number(data_app.id_operator) + ") ";
            sql += " returning id ; ";

            QSqlQuery *query2 = new QSqlQuery(db);
            if (!mySQL.exec(this, sql, "Добавление новой персоны", *query2, true, db, data_app)) {
                db.rollback();
                QMessageBox::critical(this, "Непредвиденная ошибка", "Произошла ошибка при добавлении персоны в базу данных.\n\nСохранение отменено.");
                return;
            } else if (query2->size()>0) {
                query2->next();
                data_pers.id = query2->value(0).toInt();
                QMessageBox::information(this,"Запись добавлена","Запись добавлена");
            }
        } else {
            // изменим текущую запись
            QString sql = "update persons set ";
            sql += " _id_last_point=" + QString::number(data_app.id_point) + ", ";
            sql += " _id_last_operator=" + QString::number(data_app.id_operator) + ", ";
            sql += " fam='" + ui->ln_pers_pan_fam->text().trimmed().toUpper() + "', ";
            sql += " im='" + ui->ln_pers_pan_im->text().trimmed().toUpper() + "', ";
            sql += (ui->ch_pers_pan_ot->isChecked() ? (QString(" ot='") + ui->ln_pers_pan_ot->text().trimmed().toUpper() + "', ") : " ot=NULL, ");
            sql += " sex="  + QString::number(ui->combo_pers_pan_sex->currentIndex()) + ", ";
            sql += " category="  + QString::number(ui->combo_pers_pan_category->currentData().toInt()) + ", ";
            sql += " date_birth='" + ui->date_pers_pan_birth->date().toString("yyyy-MM-dd") + "', ";
            sql += " plc_birth='" + ui->ln_pers_pan_plc_birth->text().trimmed().toUpper() + "', ";
            sql += (ui->ch_pers_pan_death->isChecked() ? QString(" date_death='" + ui->date_pers_pan_death->date().toString("dd.MM.yyyy") + "', ") : " date_death=NULL, ");
            sql += " snils=NULL, ";
            sql += " bomj=1, ";
            sql += " id_addr_reg=NULL, ";
            sql += " date_reg=NULL, ";
            sql += " id_addr_liv=NULL, ";
            sql += " oksm_birth=NULL, ";
            sql += " oksm_c=NULL, ";
            sql += " rep_id=NULL, ";
            sql += " rep_fio=NULL, ";
            sql += " rep_relation=NULL, ";
            sql += " dost=" + (data_pers.dost=="______" ? QString(" NULL ") : " '" + QString(data_pers.dost + "______").left(6) + "' ") + ", ";

            QString phone_code = ui->ln_pers_pan_phone_code->text().trimmed();
            QString phone_work = ui->ln_pers_pan_phone_work->text().trimmed().replace(" ","").replace("-","").replace("(","").replace(")","").replace("+","");
            QString phone_home = ui->ln_pers_pan_phone_home->text().trimmed().replace(" ","").replace("-","").replace("(","").replace(")","").replace("+","");
            QString phone_cell = ui->ln_pers_pan_phone_cell->text().trimmed().replace(" ","").replace("-","").replace("(","").replace(")","").replace("+","");
            QString email = ui->ln_pers_pan_email->text().trimmed();
            sql += " phone_code=" + (phone_code.isEmpty()?"NULL":("'"+phone_code+"'")) + ", ";
            sql += " phone_work=" + (phone_work.isEmpty()?"NULL":("'"+phone_work+"'")) + ", ";
            sql += " phone_home=" + (phone_home.isEmpty()?"NULL":("'"+phone_home+"'")) + ", ";
            sql += " phone_cell=" + (phone_cell.isEmpty()?"NULL":("'"+phone_cell+"'")) + ", ";
            sql += " email=" + (email.isEmpty()?"NULL":("'"+email+"'")) + " ";
            sql += " where id=" + QString::number(data_pers.id);
            sql += " ; ";

            QSqlQuery *query2 = new QSqlQuery(db);
            if (!mySQL.exec(this, sql, "Изменение персоны", *query2, true, db, data_app)) {
                db.rollback();
                QMessageBox::critical(this, "Непредвиденная ошибка", "Произошла ошибка при изменении данных персоны в базе данных.\n\nСохранение отменено.");
                return;
            } else {
                query2->next();
                QMessageBox::information(this,"Запись изменена","Запись изменена");
            }

            // Генерация события П040
            // получим действующий полис единого образца
            QString sql_get_pol = "";
            sql_get_pol += "select id from polises p ";
            sql_get_pol += " where p.id_person=" + QString::number(data_pers.id) + " ";
            sql_get_pol += "   and p.date_begin is not NULL and p.date_begin<='" + QDate::currentDate().toString("yyyy-MM-dd") + "' ";
            sql_get_pol += "   and ((p.date_end is NULL) or (p.date_end>'" + QDate::currentDate().toString("yyyy-MM-dd") + "')) ";
            sql_get_pol += "   and ((p.date_stop is NULL) or (p.date_stop>'" + QDate::currentDate().toString("yyyy-MM-dd") + "')) ";
            sql_get_pol += "   and p.pol_v=3 ; ";       // только для полисов единого образца
            QSqlQuery *query_get_pol = new QSqlQuery(db);
            if (!mySQL.exec(this, sql_get_pol, "Получим полис изменяемой персоны", *query_get_pol, true, db, data_app)) {
                db.rollback();
                QMessageBox::critical(this, "Непредвиденная ошибка", "Произошла ошибка при запросе действующего полиса персоны для генерации события П040.\n\nСохранение отменено.");
                return;
            }
            // контроль повторных полисов
            if (query_get_pol->size()>1) {
                db.rollback();
                QMessageBox::critical(this, "Непредвиденная ошибка", "При запросе действующего полиса персоны для генерации события П040 оказалось что у застрахованного более 1-го действующего полиса.\n\nСохранение отменено.");
                return;
            }
            if (query_get_pol->size()==1) {
                // добавим событие П040
                query_get_pol->next();
                int id_polis = query_get_pol->value(0).toInt();
                QString sql_p040 = "";
                sql_p040 = "insert into events (id_point, id_operator, id_polis, id_vizit, event_code, event_dt, event_time, comment, status, id_person, id_person_old, id_udl, id_drp) ";
                sql_p040 += " values (" + QString::number(data_app.id_point) + ", ";
                sql_p040 += "  " + QString::number(data_app.id_operator) + ", ";
                sql_p040 += "  " + QString::number(id_polis) + ", ";
                sql_p040 += " -1, ";
                sql_p040 += " 'П040', ";
                sql_p040 += " '" + QDate::currentDate().toString("yyyy-MM-dd") + "', ";
                sql_p040 += " '" + QTime::currentTime().toString("hh:mm:ss") + "', ";
                sql_p040 += " 'П040 - Изменение идентифицирующих данных застрахованного лица', ";
                sql_p040 += "  " + QString::number(data_pers.id) + ", ";
                sql_p040 += "  " + (data_pers.id_old>0 ? QString::number(data_pers.id_old) : "NULL") + ", ";
                sql_p040 += "  " + (data_pers.id_udl>0 ? QString::number(data_pers.id_udl) : "NULL") + ", ";
                sql_p040 += "  " + (data_pers.id_drp>0 ? QString::number(data_pers.id_drp) : "NULL") + ") ";
                QSqlQuery *query_p040 = new QSqlQuery(db);
                if (!mySQL.exec(this, sql_p040, "Добавим событие П040", *query_p040, true, db, data_app)) {
                    db.rollback();
                    QMessageBox::critical(this, "Непредвиденная ошибка", "Произошла ошибка при генерации события П040.\n\nСохранение данных персоны отменено.");
                    return;
                }
            }
        }

        if ( ui->ch_pers_pan_fam_old->isChecked()      ||
             ui->ch_pers_pan_im_old->isChecked()       ||
             ui->ch_pers_pan_ot_old->isChecked()       ||
             ui->ch_pers_pan_sex_old->isChecked()      ||
             ui->ch_pers_pan_date_birth_old->isChecked()||
             ui->ch_pers_pan_snils_old->isChecked()    ||
             ui->ch_pers_pan_plc_birth_old->isChecked()  ) {

            // удалим старую запись persons_OLD
            QString sql_del = "delete from persons_old where id_person=" + QString::number(data_pers.id) + " ; ";
            QSqlQuery *query_del = new QSqlQuery(db);
            if (!mySQL.exec(this, sql_del, "Удаление старой записи persons_OLD", *query_del, true, db, data_app)) {
                db.rollback();
                QMessageBox::critical(this, "Непредвиденная ошибка", "При удалении старой записи persons_OLD произошла непредвиденная ошибка.\n\nСохранение данных персоны отменено.");
                return;
            }

            // добавим новую запись persons_OLD
            /*
            QString sql_ins = "insert into persons_old (id_person, old_fam, old_im, old_ot, old_sex, old_date_birth, old_snils, old_plc_birth) "
                              " values (" + QString::number(data_pers.id) + ", "
                                          + "'" + data_pers.fam_old.trimmed().toUpper() + "', "
                                          + "'" + data_pers.im_old.trimmed().toUpper() + "', "
                                          + "'" + data_pers.ot_old.trimmed().toUpper() + "', "
                                          + QString::number(data_pers.sex_old) + ", "
                                          + "'" + data_pers.date_birth_old.toString("yyyy-MM-dd") + "', "
                                          + "'" + data_pers.snils_old.replace("-","").replace(" ","") + "', "
                                          + "'" + data_pers.plc_birth_old.toUpper() + "') ; ";
                                          */
            QString sql_ins = "insert into persons_old (id_person, old_fam, old_im, old_ot, old_sex, old_date_birth, old_enp, old_snils, old_plc_birth) "
                              " values (" + QString::number(data_pers.id) + ", "
                                          + "'" + ( data_pers.fam_old.trimmed().toUpper().isEmpty() ? data_pers.fam.trimmed().toUpper() : data_pers.fam_old.trimmed().toUpper() ) + "', "
                                          + "'" + ( data_pers.im_old.trimmed().toUpper().isEmpty() ? data_pers.im.trimmed().toUpper() : data_pers.im_old.trimmed().toUpper() ) + "', "
                                          + "'" + ( data_pers.ot_old.trimmed().toUpper().isEmpty() ? data_pers.ot.trimmed().toUpper() : data_pers.ot_old.trimmed().toUpper() ) + "', "
                                          + ( (data_pers.sex_old!=1 && data_pers.sex_old!=2) ? QString::number(data_pers.sex) : QString::number(data_pers.sex_old) ) + ", "
                                          + ( ui->ch_pers_pan_date_birth_old->isChecked() ? ("'" + data_pers.date_birth_old.toString("yyyy-MM-dd") + "'") : ("'" + data_pers.date_birth.toString("yyyy-MM-dd") + "'") ) + ", "
                                          + (QString(data_pers.enp_old.replace("-","").replace(" ","")).isEmpty() ? "NULL" : QString("'" + data_pers.enp_old.replace("-","").replace(" ","") + "'")) + ", "
                                          + "'" + ( data_pers.snils_old.replace("-","").replace(" ","").isEmpty() ? data_pers.snils.replace("-","").replace(" ","") : data_pers.snils_old.replace("-","").replace(" ","") ) + "', "
                                          + "'" + data_pers.plc_birth_old.toUpper() + "') ; ";
            QSqlQuery *query_ins = new QSqlQuery(db);
            if (!mySQL.exec(this, sql_ins, "Вставка новой записи persons_OLD", *query_ins, true, db, data_app)) {
                db.rollback();
                QMessageBox::critical(this, "Непредвиденная ошибка", "При добавлении новой записи persons_OLD произошла непредвиденная ошибка.\n\nСохранение данных персоны отменено.");
                return;
            }
        }

        db.commit();
        test_for_UDL();
        b_need_save(false);

    } catch (...) {
        db.rollback();
        QMessageBox::critical(this, "Непредвиденная ошибка", "Проверьте корректность введённых данных");
    }
}

void representers_wnd::on_bn_pers_document_clicked() {
    // сохраним все изменения на панели
    on_bn_pers_save_clicked();
    // скроем таблицу персон и панель свойств персоны, покажем панель документов
    ui->show_pers_tabl->setChecked(false);
    ui->show_pers_data->setChecked(true);
    ui->show_pers_docs->setChecked(true);
    on_show_pers_tabl_clicked(false);
    on_show_pers_data_clicked(true);
    on_show_pers_docs_clicked(true);
    ui->show_pers_tabl->setEnabled(false);
    ui->show_pers_data->setEnabled(true);
    ui->show_pers_docs->setEnabled(true);

    refresh_pers_docs();
}

void representers_wnd::on_bn_pers_pers_pan_back_clicked() {
    if (ui->bn_pers_save->isVisible() &&
            QMessageBox::warning(this,
                                 "Есть несохранённые изменения",
                                 "На окне есть несохранённые изменения.\nВы действительно хотите закрыть окно без сохранения несохранённых данных?",
                                 QMessageBox::Yes|QMessageBox::Cancel,
                                 QMessageBox::Cancel)==QMessageBox::Cancel) {
        return;
    }
    this->setCursor(Qt::WaitCursor);
    // скроем панель свойств персоны и панель документов, покажем таблицу персон
    ui->show_pers_tabl->setChecked(true);
    ui->show_pers_data->setChecked(false);
    ui->show_pers_docs->setChecked(false);
    on_show_pers_tabl_clicked(true);
    on_show_pers_data_clicked(false);
    on_show_pers_docs_clicked(false);
    ui->show_pers_tabl->setEnabled(true);
    ui->show_pers_data->setEnabled(false);
    ui->show_pers_docs->setEnabled(false);

    refresh_representers_tab();
    this->setCursor(Qt::ArrowCursor);
}

void representers_wnd::on_bn_pers_doc_back_clicked() {
    this->setCursor(Qt::WaitCursor);
    // скроем панель документов и таблицу персон, покажем панель свойств персоны
    ui->show_pers_tabl->setChecked(false);
    ui->show_pers_data->setChecked(true);
    ui->show_pers_docs->setChecked(false);
    on_show_pers_tabl_clicked(false);
    on_show_pers_data_clicked(true);
    on_show_pers_docs_clicked(false);
    ui->show_pers_tabl->setEnabled(false);
    ui->show_pers_data->setEnabled(true);
    ui->show_pers_docs->setEnabled(true);

    refresh_representers_tab();
    this->setCursor(Qt::ArrowCursor);
}

void representers_wnd::refresh_pers_docs() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    QString pers_docs_sql = "select d.id, d.doc_type, d.doc_ser, d.doc_num, d.doc_date, d.doc_exp, f.id as id_doctype, f.text as type_text, d.status, d.org_code, d.doc_org, "
                          "         f.mask_ser as type_mask_ser, f.mask_num as type_mask_num, f.date_beg as type_date_beg, f.date_end as type_date_end, "
                          "         case when d.doc_exp is null then 1 else 0 end as has_doc_exp "
                          " from persons_docs d "
                          "      left join spr_f011 f on(d.doc_type=f.code) "
                          " where d.id_person=" + QString::number(data_pers.id);
    if (ui->ch_docs_tab_only_acts->isChecked())
        pers_docs_sql += "  and d.status=1 and d.doc_date<CURRENT_DATE and ((d.doc_exp is NULL) or (d.doc_exp>CURRENT_DATE)) ";
    pers_docs_sql += " order by status desc ; ";

    model_pers_docs.setQuery(pers_docs_sql,db);
    QString err2 = model_pers_docs.lastError().driverText();

    // подключаем модель из БД
    ui->tab_pers_docs->setModel(&model_pers_docs);

    // обновляем таблицу
    ui->tab_pers_docs->reset();

    // задаём ширину колонок
    ui->tab_pers_docs->setColumnWidth( 0,  1);     // d.id
    ui->tab_pers_docs->setColumnWidth( 1,  1);     // d.doc_type
    ui->tab_pers_docs->setColumnWidth( 2, 50);     // d.doc_ser
    ui->tab_pers_docs->setColumnWidth( 3, 80);     // d.doc_num
    ui->tab_pers_docs->setColumnWidth( 4, 80);     // d.doc_date
    ui->tab_pers_docs->setColumnWidth( 5, 80);     // d.doc_exp
    ui->tab_pers_docs->setColumnWidth( 6,  1);     // f.id as id_doctype
    ui->tab_pers_docs->setColumnWidth( 7,200);     // f.text as type_text
    ui->tab_pers_docs->setColumnWidth( 8,  1);     // d.status
    ui->tab_pers_docs->setColumnWidth( 9, 50);     // d.org_code
    ui->tab_pers_docs->setColumnWidth(10,200);     // d.doc_org
    ui->tab_pers_docs->setColumnWidth(11,  1);     // f.mask_ser as type_mask_ser
    ui->tab_pers_docs->setColumnWidth(12,  1);     // f.mask_num as type_mask_num
    ui->tab_pers_docs->setColumnWidth(13,  1);     // f.date_beg as type_date_beg
    ui->tab_pers_docs->setColumnWidth(14,  1);     // f.date_end as type_date_end
    ui->tab_pers_docs->setColumnWidth(15,  1);     // case when d.doc_exp is null then 1 else 0 end as has_doc_exp

    // правим заголовки
    model_pers_docs.setHeaderData( 2, Qt::Horizontal, ("сер."));
    model_pers_docs.setHeaderData( 3, Qt::Horizontal, ("номер"));
    model_pers_docs.setHeaderData( 4, Qt::Horizontal, ("дата выдачи"));
    model_pers_docs.setHeaderData( 5, Qt::Horizontal, ("срок дейст."));
    model_pers_docs.setHeaderData( 7, Qt::Horizontal, ("тип документа"));
    model_pers_docs.setHeaderData( 8, Qt::Horizontal, ("дейст"));
    model_pers_docs.setHeaderData( 9, Qt::Horizontal, ("код"));
    model_pers_docs.setHeaderData(10, Qt::Horizontal, ("выдан подразд."));
    model_pers_docs.setHeaderData(11, Qt::Horizontal, ("маска серии"));
    model_pers_docs.setHeaderData(12, Qt::Horizontal, ("маска номера"));
    model_pers_docs.setHeaderData(13, Qt::Horizontal, ("выдаётся с"));
    model_pers_docs.setHeaderData(14, Qt::Horizontal, ("выдавался до"));
    model_pers_docs.setHeaderData(15, Qt::Horizontal, ("огр. по дате?"));
    ui->tab_pers_docs->repaint();

    refresh_docs_pan_type();

    this->setCursor(Qt::ArrowCursor);
}

void representers_wnd::on_bn_docs_refresh_clicked() {
    refresh_pers_docs();
    ui->ch_docs_tab_only_acts->setEnabled(true);
}

void representers_wnd::on_bn_pers_docs_back_clicked() {
    // скроем таблицу персон и панель свойств персоны, покажем панель документов
    ui->show_pers_tabl->setChecked(false);
    ui->show_pers_data->setChecked(true);
    ui->show_pers_docs->setChecked(false);
    on_show_pers_tabl_clicked(false);
    on_show_pers_data_clicked(true);
    on_show_pers_docs_clicked(false);
    ui->show_pers_tabl->setEnabled(false);
    ui->show_pers_data->setEnabled(true);
    ui->show_pers_docs->setEnabled(true);
}

QString representers_wnd::myMASK(QString text0, QString mask0) {
    QString sss = "019АЯAZ -IVXLCRБ";
    QString aRus = "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
    QString aLat = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    QString aRom = "IVXLC";
    QString aNum = "0123456789";
    QString aAdd = "- ";
    if (text0.isEmpty())  return "";
    // очистим строку
    QString text = text0.toUpper().replace("\t"," ").replace("  "," ").replace("  "," ").replace("  "," ").replace("  "," ");
    QString mask = mask0.toUpper().replace("\t"," ").replace("  "," ").replace("  "," ").replace("  "," ").replace("  "," ");
    QString res_t = "";
    // проверим число 9 и 0
    int n0=0, n9=0;
    for (int i=0; i<mask.size(); i++) {
        if (mask.at(i)=='0')  n0++;
        if (mask.at(i)=='9')  n9++;
        if (mask.at(i)=='Ц')  n0++;
    }
    // проверим на число
    if (n0+n9==mask.size()) {
        // это число. Проверим, все ли символы строки - цифры и сколько этих цифр
        int n_num=0;
        for (int i=0; i<text.size(); i++) {
            if ( aNum.indexOf(text.at(i))>=0 )
            {
                n_num++;
            }
        }
        if (n_num==text.size() && n_num>=n9 && n_num<=(n0+n9)) {
            res_t = text;
            return res_t;
        }
    }
    // проверка на произвольную строку
    if (mask=="S1") {
        bool f = true;
        for (int i=0; i<text.size(); i++) {
            if ( aRus.indexOf(text.at(i))>=0 ||
                 aLat.indexOf(text.at(i))>=0 ||
                 aAdd.indexOf(text.at(i))>=0 ||
                 aNum.indexOf(text.at(i))>=0 )
            {
                res_t += text.at(i);
                f = true;
            } else  {
                f = false;
                break;
            }
        }
        if (f) {
            return res_t;
        } else {
            return res_t;
            //return "";
        }
    }
    // проверка на произвольную строку в одном алфавите
    if (mask=="S") {
        bool f = true;
        bool fR = false;
        bool fE = false;
        for (int i=0; i<text.size(); i++) {
            if (aRus.indexOf(text.at(i))>=0 && !fE) {
                res_t += text.at(i);
                f = true;
                fR = true;
            } else if (aLat.indexOf(text.at(i))>=0 && !fR) {
                res_t += text.at(i);
                f = true;
                fE = true;
            } else if (aAdd.indexOf(text.at(i))>=0) {
                res_t += text.at(i);
                f = true;
            } else if (aNum.indexOf(text.at(i))>=0) {
                res_t += text.at(i);
                f = true;
            } else  {
                f = false;
                break;
            }
        }
        if (f) {
            return res_t;
        } else {
            return res_t;
            //return "";
        }
    }
    // проверка на маску
    int pos_text = 0;
    int pos_mask = 0;
    int nR = 0;
    for (int i=0; i<text.size(); i++) {
        pos_text = i;
        if (pos_text>=text.length() || pos_mask>=mask.length())
            break;
        QChar c = text.at(pos_text);
        QChar m = mask.at(pos_mask);
        if (m==sss.at(15) && aRus.indexOf(QString(c))>=0 ) {
            res_t += c;
            pos_mask++;
            nR=0;
            continue;
        } else if (m==sss.at(2) && aNum.indexOf(QString(c))>=0 ) {
            res_t +=c;
            pos_mask++;
            nR=0;
            continue;
        } else if (m==sss.at(8) && c==sss.at(8)) {
            res_t += c;
            pos_mask++;
            nR=0;
            continue;
        } else if (m==sss.at(7) && c==sss.at(7)) {
            res_t += c;
            pos_mask++;
            nR=0;
            continue;
        } else if (m==sss.at(14) && aRom.indexOf(QString(c))>=0 ) {
            res_t += c;
            //pos_mask++;
            nR++;
        } else if (m==sss.at(14) && aRom.indexOf(QString(c))<0 ) {
            if (nR==0) {
                break;
                //return res_t;
            } else {
                pos_mask++;
                pos_text--;
                i--;
            }
        }
    }
    // проверим, надо ли добавлять разделитель ' ' или '-'
    //pos_text++;
    //pos_mask++;
    if (pos_mask<mask.length()) {
        if ( mask.at(pos_mask)==sss.at(7) || mask.at(pos_mask)==sss.at(8) ) {
            res_t += mask.at(pos_mask);
        }
    }
    // проверка на пустую серию разрешения на временное убежище
    if (mask=="ЦЦ" && res_t.trimmed().isEmpty()) {
        //res_t = "ВУ";
    }
    return res_t;
}

void representers_wnd::refresh_docs_pan_orgcodes() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка типов документов
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select org_code, org_name "
                  " from public.orgs i "
                  " order by org_code ; ";
    mySQL.exec(this, sql, QString("Справочник подразделений ФМС"), *query, true, db, data_app);
    ui->combo_docs_pan_orgcode->clear();
    ui->combo_docs_pan_orgcode->addItem(" - нет - ", "");
    while (query->next()) {
        QString org_code = query->value(0).toString();
        QString org_name = query->value(1).toString();
        ui->combo_docs_pan_orgcode->addItem(org_code, org_name);
    }
    if (data_doc.has_org_code) {
        ui->combo_docs_pan_orgcode->setCurrentIndex(ui->combo_docs_pan_orgcode->findData(data_doc.org_code));
    } else {
        ui->combo_docs_pan_orgcode->setCurrentIndex(0);
    }
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void representers_wnd::on_bn_docs_new_clicked() {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    if (QMessageBox::question(this,
                              "Новый документ?",
                              "Вы действительно хотите добавить новый документ, удостоверяющий личность выбранного застрахованного лица?",
                              QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes) {
        ui->tab_pers_docs->setEnabled(false);
        ui->bn_docs_refresh->setEnabled(false);
        ui->frame_bn_docs->setEnabled(false);
        ui->bn_docs_new->setEnabled(false);
        ui->bn_docs_edit->setEnabled(false);
        ui->bn_docs_save->setEnabled(true);
        ui->bn_docs_cansel->setEnabled(true);
        ui->bn_docs_del->setEnabled(false);
        ui->pan_document->setEnabled(true);

        // новый документ
        this->setCursor(Qt::WaitCursor);

        data_doc.id        = -1;
        data_doc.doc_type  = 0;
        data_doc.doc_ser   = "";
        data_doc.doc_num   = "";
        data_doc.doc_date  = QDate(1920,1,1);
        data_doc.doc_exp   = QDate(1920,1,1);
        data_doc.has_doc_exp= 0;
        data_doc.doc_org   = "";
        data_doc.status    = 1;
        data_doc.id_doctype= 0;
        data_doc.doc_name = "";
        data_doc.mask_ser  = doc_ser_mask.at(0);
        data_doc.mask_num  = doc_num_mask.at(0);
        data_doc.type_begin= QDate(1920,1,1);
        data_doc.type_end  = QDate(1920,1,1);

        ui->spin_docs_pan_type->setValue(0);
        ui->combo_docs_pan_type->setCurrentIndex(0);
        ui->lab_docs_pan_mask->setText(data_doc.mask_ser + " № " + data_doc.mask_num);

        ui->line_docs_pan_ser->setText(myMASK(data_doc.doc_ser, data_doc.mask_ser));
        ui->line_docs_pan_num->setText(myMASK(data_doc.doc_num, data_doc.mask_num));
        ui->date_docs_pan_start->setDate(data_doc.doc_date);
        ui->date_docs_pan_exp->setDate(data_doc.doc_exp);
        if (data_doc.has_org_code) {
            ui->combo_docs_pan_orgcode->setCurrentIndex(ui->combo_docs_pan_orgcode->findData(data_doc.org_code));
        } else {
            ui->combo_docs_pan_orgcode->setCurrentIndex(0);
        }
        ui->line_docs_pan_ogrname->setText(data_doc.doc_org);
        ui->ch_docs_pan_actual->setChecked(data_doc.status==1);

        refresh_docs_pan_orgcodes();

        this->setCursor(Qt::ArrowCursor);
    }
    refresh_pers_docs();
}

void representers_wnd::on_bn_docs_edit_clicked() {
    // проверим выделенную строку
    QModelIndexList indexes = ui->tab_pers_docs->selectionModel()->selection().indexes();

    if (indexes.size()==0) {
        QMessageBox::warning(this, "Ничего не выбрано!", "Выберите в списке документ, который хотите исправить.");
        return;
    }

    QString org_code = data_doc.org_code;
    bool has_org_code = data_doc.has_org_code;
    refresh_docs_pan_orgcodes();

    data_doc.has_org_code = has_org_code;
    data_doc.org_code = org_code;
    if (data_doc.has_org_code) {
        ui->combo_docs_pan_orgcode->setCurrentIndex(ui->combo_docs_pan_orgcode->findText(data_doc.org_code));
    } else {
        ui->combo_docs_pan_orgcode->setCurrentIndex(0);
    }

    ui->tab_pers_docs->setEnabled(false);
    ui->bn_docs_refresh->setEnabled(false);
    ui->frame_bn_docs->setEnabled(false);
    ui->bn_docs_new->setEnabled(false);
    ui->bn_docs_edit->setEnabled(false);
    ui->bn_docs_save->setEnabled(true);
    ui->bn_docs_cansel->setEnabled(true);
    ui->bn_docs_del->setEnabled(false);
    ui->pan_document->setEnabled(true);

    ui->lab_docs_pan_mask->setText(data_doc.mask_ser + " № " + data_doc.mask_num);

    ui->line_docs_pan_ser->setText(myMASK(data_doc.doc_ser, data_doc.mask_ser));
    ui->line_docs_pan_num->setText(myMASK(data_doc.doc_num, data_doc.mask_num));
    ui->date_docs_pan_start->setDate(data_doc.doc_date);
    ui->date_docs_pan_exp->setDate(data_doc.doc_exp);
    ui->line_docs_pan_ogrname->setText(data_doc.doc_org);
    ui->ch_docs_pan_actual->setChecked(data_doc.status==1);
}

void representers_wnd::on_bn_docs_save_clicked() {
    // Проверим заполнение полей
    if (ui->combo_docs_pan_orgcode->currentText().isEmpty() || ui->combo_docs_pan_orgcode->currentText()=="?") {
        QMessageBox::warning(this, "Не достаточно данных!", "Не внесен код подразделения, выдавшего документ.");
        return;
    }
    if (ui->line_docs_pan_ogrname->toPlainText().isEmpty()) {
        QMessageBox::warning(this, "Не достаточно данных!", "Не внесено название подразделения, выдавшего документ.");
        return;
    }
    if (ui->combo_docs_pan_type->currentIndex()==0) {
        QMessageBox::warning(this, "Не достаточно данных!", "Выберите тип докумера.");
        return;
    }
    if (QMessageBox::question(this,
                              "Сохранить?",
                              "Сохранить внесённые изменения?",
                              QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes) {
        // собственно сохранение
        ui->tab_pers_docs->setEnabled(true);
        ui->bn_docs_refresh->setEnabled(true);
        ui->frame_bn_docs->setEnabled(true);
        ui->bn_docs_new->setEnabled(true);
        ui->bn_docs_edit->setEnabled(true);
        ui->bn_docs_save->setEnabled(false);
        ui->bn_docs_cansel->setEnabled(false);
        ui->bn_docs_del->setEnabled(true);
        ui->pan_document->setEnabled(false);
        // сохраним документ
        if (data_doc.id==-1) {
            // сохраним запись в базу
            QString sql = "insert into persons_docs(id_person,doc_type,doc_ser,doc_num,doc_date,doc_exp,org_code,doc_org,status) ";
            sql += " values (";
            sql += " "  + QString::number(data_pers.id) + ", ";
            sql += " "  + QString::number(ui->combo_docs_pan_type->currentData().toInt()) + ", ";
            sql += " '" + ui->line_docs_pan_ser->text() + "', ";
            sql += " '" + ui->line_docs_pan_num->text() + "', ";
            sql += " '" + ui->date_docs_pan_start->date().toString("dd.MM.yyyy") + "', ";
            bool f = ui->ch_docs_pan_exp->isChecked();
            sql += " "  + (f ? QString("'" + ui->date_docs_pan_exp->date().toString("dd.MM.yyyy") + "', ") : "NULL, ");
            sql += ( ui->combo_docs_pan_orgcode->currentIndex()==0 ? (" NULL, ") : (" '" + ui->combo_docs_pan_orgcode->currentText() + "', ") );
            sql += " '" + ui->line_docs_pan_ogrname->toPlainText() + "', ";
            sql += " "  + QString::number(ui->ch_docs_pan_actual->isChecked() ? 1 : 0) + ") ";
            sql += " returning id ; ";

            QSqlQuery *query2 = new QSqlQuery(db);
            if (!mySQL.exec(this, sql, "Добавление нового документа, удостоверяющего личность застрахованного", *query2, true, db, data_app))
                return;
            if (query2->size()>0) {
                query2->next();
                data_doc.id = query2->value(0).toInt();
            }
            delete query2;
        } else {
            // изменим текущую запись
            QString sql = "update persons_docs set";
            sql += " id_person=" + QString::number(data_pers.id) + ", ";
            sql += " doc_type=" + QString::number(ui->combo_docs_pan_type->currentData().toInt()) + ", ";
            sql += " doc_ser='" + ui->line_docs_pan_ser->text() + "', ";
            sql += " doc_num='" + ui->line_docs_pan_num->text() + "', ";
            sql += " doc_date='" + ui->date_docs_pan_start->date().toString("dd.MM.yyyy") + "', ";
            sql += " doc_exp=" + (ui->ch_docs_pan_exp->isChecked() ? QString("'" + ui->date_docs_pan_exp->date().toString("dd.MM.yyyy") + "', ") : "NULL, ");
            sql += " org_code=" + ( ui->combo_docs_pan_orgcode->currentIndex()==0 ? (" NULL, ") : (" '" + ui->combo_docs_pan_orgcode->currentText() + "', ") );
            sql += " doc_org='" + ui->line_docs_pan_ogrname->toPlainText() + "', ";
            sql += " status="  + QString::number(ui->ch_docs_pan_actual->isChecked() ? 1 : 0) + " ";
            sql += " where id=" + QString::number(data_doc.id);
            sql += " ; ";

            QSqlQuery *query2 = new QSqlQuery(db);
            if (!mySQL.exec(this, sql, "Правка документа, удостоверяющего личность застрахованного", *query2, true, db, data_app))
                return;
            if (query2->size()>0) {
                query2->next();
                data_pers.id = query2->value(0).toInt();
            }
            delete query2;
        }
    }
    refresh_pers_docs();
    test_for_UDL();
}

void representers_wnd::on_bn_docs_cansel_clicked() {
    /*if (QMessageBox::question(this,
                              "Отмена?",
                              "Отменить все внесённые изменения?",
                              QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes)*/ {
        ui->tab_pers_docs->setEnabled(true);
        ui->bn_docs_refresh->setEnabled(true);
        ui->frame_bn_docs->setEnabled(true);
        ui->bn_docs_new->setEnabled(true);
        ui->bn_docs_edit->setEnabled(true);
        ui->bn_docs_save->setEnabled(false);
        ui->bn_docs_cansel->setEnabled(false);
        ui->bn_docs_del->setEnabled(true);
        ui->pan_document->setEnabled(false);
    }
    //refresh_pers_docs();
}

void representers_wnd::on_bn_docs_del_clicked() {
    if (QMessageBox::warning(this,
                             "Нужно подтверждение!",
                             "Вы действительно хотите удалить из базы данных информацию о документе, подтверждающем личность выбранного застрахованного лца?",
                             QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes) {
        // удалим документ
        QString sql = "delete from persons_docs ";
        sql += " where id=" + QString::number(data_doc.id);

        QSqlQuery *query = new QSqlQuery(db);
        if (!mySQL.exec(this, sql, "Удаление документа, удостоверяющего личность застрахованного", *query, true, db, data_app)) {
            delete query;
            return;
        }
        delete query;
    }
    refresh_pers_docs();
}

void representers_wnd::on_tab_pers_docs_clicked(const QModelIndex &index) {
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }
    if (!ui->tab_pers_docs || !ui->tab_pers_docs->selectionModel()) {
        QMessageBox::warning(this, "Таблица не загружена!",
                             "Таблица не загружена!\n"
                             "Нажмите кнопку [Обновить] и выберите документ из таблицы.");
        return;
    }
    this->setCursor(Qt::WaitCursor);

    data_doc.id        = model_pers_docs.data(model_pers_docs.index(index.row(), 0), Qt::EditRole).toInt();
    data_doc.doc_type  = model_pers_docs.data(model_pers_docs.index(index.row(), 1), Qt::EditRole).toInt();
    data_doc.doc_ser   = model_pers_docs.data(model_pers_docs.index(index.row(), 2), Qt::EditRole).toString();
    data_doc.doc_num   = model_pers_docs.data(model_pers_docs.index(index.row(), 3), Qt::EditRole).toString();
    data_doc.doc_date  = model_pers_docs.data(model_pers_docs.index(index.row(), 4), Qt::EditRole).toDate();
    data_doc.doc_exp   = model_pers_docs.data(model_pers_docs.index(index.row(), 5), Qt::EditRole).toDate();
    data_doc.id_doctype= model_pers_docs.data(model_pers_docs.index(index.row(), 6), Qt::EditRole).toInt();
    data_doc.doc_name  = model_pers_docs.data(model_pers_docs.index(index.row(), 7), Qt::EditRole).toString();
    data_doc.status    = model_pers_docs.data(model_pers_docs.index(index.row(), 8), Qt::EditRole).toInt();
    data_doc.has_org_code = !model_pers_docs.data(model_pers_docs.index(index.row(), 9), Qt::EditRole).isNull();
    data_doc.org_code  = model_pers_docs.data(model_pers_docs.index(index.row(), 9), Qt::EditRole).toString();
    data_doc.doc_org   = model_pers_docs.data(model_pers_docs.index(index.row(),10), Qt::EditRole).toString();
    data_doc.mask_ser  = model_pers_docs.data(model_pers_docs.index(index.row(),11), Qt::EditRole).toString();
    data_doc.mask_num  = model_pers_docs.data(model_pers_docs.index(index.row(),12), Qt::EditRole).toString();
    data_doc.type_begin= model_pers_docs.data(model_pers_docs.index(index.row(),13), Qt::EditRole).toDate();
    data_doc.type_end  = model_pers_docs.data(model_pers_docs.index(index.row(),14), Qt::EditRole).toDate();

    data_doc.has_doc_exp= !model_pers_docs.data(model_pers_docs.index(index.row(), 5), Qt::EditRole).toString().isEmpty();

    ui->spin_docs_pan_type->setValue(data_doc.doc_type);
    ui->combo_docs_pan_type->setCurrentIndex(ui->combo_docs_pan_type->findData(data_doc.doc_type));
    ui->line_docs_pan_ser->setText(data_doc.doc_ser);
    ui->line_docs_pan_num->setText(data_doc.doc_num);
    ui->date_docs_pan_start->setDate(data_doc.doc_date);
    ui->ch_docs_pan_exp->setChecked(data_doc.has_doc_exp);
    ui->date_docs_pan_exp->setEnabled(data_doc.has_doc_exp);
    ui->date_docs_pan_exp->setEnabled(data_doc.has_doc_exp);
    if (data_doc.has_org_code) {
        int n = ui->combo_docs_pan_orgcode->findText(data_doc.org_code);
        ui->combo_docs_pan_orgcode->setCurrentIndex(n);
    } else {
        ui->combo_docs_pan_orgcode->setCurrentIndex(0);
    }
    ui->line_docs_pan_ogrname->setText(data_doc.doc_org);
    ui->ch_docs_pan_actual->setChecked(data_doc.status==1);

    this->setCursor(Qt::ArrowCursor);
}

void representers_wnd::on_ch_docs_pan_exp_stateChanged(int arg1) {
    ui->date_docs_pan_exp->setEnabled(arg1>0);
}

void representers_wnd::on_line_docs_pan_ser_textEdited(const QString &arg1) {
    QString s1 = myMASK(arg1, data_doc.mask_ser);
    ui->line_docs_pan_ser->setText(s1);
    ui->line_docs_pan_ser->setCursorPosition(s1.size());
}

void representers_wnd::on_line_docs_pan_num_textEdited(const QString &arg1) {
    QString s1 = myMASK(arg1, data_doc.mask_num);
    ui->line_docs_pan_num->setText(s1);
    ui->line_docs_pan_num->setCursorPosition(s1.size());
}

/*bool representers_wnd::myBYTEA(QString &fname, QString& res_str) {
    QString ms = "0123456789ABCDEF";
    QFile f(fname);
    if (!f.open(QIODevice::ReadOnly)) {
        return false;
    }
    res_str = "\\x";
    int m = 0;
    char cd[1024];
    while (!f.atEnd()) {
        int n = f.read(cd, 1024);
        for (int i=0; i<n; i++) {
            unsigned int v = (unsigned char)(cd[i]);
            int n1 = v>>4;
            int n2 = v%16;
            res_str += ms.at(n1);
            res_str += ms.at(n2);
        }
        m++;
    }
    return true;
}
*/


void representers_wnd::test_for_UDL() {
    int index = ui->combo_pers_pan_category->currentIndex();
    // документы можно вводить только при выбранной категории
    ui->pan_docs_tab->setEnabled(index>0);

    data_pers.category = ui->combo_pers_pan_category->currentData().toInt();

    // получение списка действующих документов персоны
    this->setCursor(Qt::WaitCursor);
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select doc_type, doc_date, doc_exp, status "
                  " from public.persons_docs d "
                  " where id_person=" + QString::number(data_pers.id) + " and "
                  "       doc_date<=CURRENT_DATE and ((doc_exp is NULL) or (doc_exp>CURRENT_DATE)) and status=1 "
                  " order by doc_type ; ";
    mySQL.exec(this, sql, QString("Проверка действующих документов персоны"), *query, true, db, data_app);
    bool docs_array[30];
    QDate docs_date[30];
    for (int i=0; i<30; i++) {
        docs_array[i] = false;
        docs_date[i]  = QDate(1900,1,1);
    }
    while (query->next()) {
        docs_array[query->value(0).toInt()] = true;
        docs_date[query->value(0).toInt()]  = query->value(1).toDate();
    }
    delete query;

    // проверка, соответствуют ли документы застрахованного его категории
    bool docs_ok = false;
    bool allow_ok = false;
    // дельта с даты рождения
    int DY = d_date::dY(data_pers.date_birth, QDate::currentDate());
    int DM = d_date::dM(data_pers.date_birth, QDate::currentDate());
    int DD = d_date::dD(data_pers.date_birth, QDate::currentDate());

    int code = ui->combo_pers_pan_category->currentIndex();
    switch (code) {
    case 0: {
        docs_ok = false;
    }
    break;
    case 1:
    case 6:
    {
        bool d_0_14, d_14_20, d_20_45, d_45_;
        if ( DY<14 || (DY==14 && DM<=1 && DD<=30 ) ) {
            // до 14 лет
            d_0_14 = ( // проверка возраста и типа документа
                        ( ( docs_array[3] || docs_array[24] ) ) &&
                          ( ( // проверка даты выдачи документа 3
                              ( (d_date::dY(data_pers.date_birth, docs_date[3])>=0) ||
                                (d_date::dY(data_pers.date_birth, docs_date[3])==0 && d_date::dM(data_pers.date_birth, docs_date[3])>=0) ||
                                (d_date::dY(data_pers.date_birth, docs_date[3])==0 && d_date::dM(data_pers.date_birth, docs_date[3])==0 && d_date::dD(data_pers.date_birth, docs_date[3])>=0) ) &&
                              ( (d_date::dY(data_pers.date_birth, docs_date[3])<=14) ||
                                (d_date::dY(data_pers.date_birth, docs_date[3])==14 && d_date::dM(data_pers.date_birth, docs_date[3])<=1) ||
                                (d_date::dY(data_pers.date_birth, docs_date[3])==14 && d_date::dM(data_pers.date_birth, docs_date[3])==1 && d_date::dD(data_pers.date_birth, docs_date[3])<=30) )
                            ) ||
                            ( // проверка даты выдачи документа 24
                              ( (d_date::dY(data_pers.date_birth, docs_date[24])>=0) ||
                                (d_date::dY(data_pers.date_birth, docs_date[24])==0 && d_date::dM(data_pers.date_birth, docs_date[24])>=0) ||
                                (d_date::dY(data_pers.date_birth, docs_date[24])==0 && d_date::dM(data_pers.date_birth, docs_date[24])==0 && d_date::dD(data_pers.date_birth, docs_date[24])>=0) ) &&
                              ( (d_date::dY(data_pers.date_birth, docs_date[24])<=14) ||
                                (d_date::dY(data_pers.date_birth, docs_date[24])==14 && d_date::dM(data_pers.date_birth, docs_date[24])<=1) ||
                                (d_date::dY(data_pers.date_birth, docs_date[24])==14 && d_date::dM(data_pers.date_birth, docs_date[24])==1 && d_date::dD(data_pers.date_birth, docs_date[24])<=30) )
                            )
                         )
                      );
            if (!d_0_14)
                QMessageBox::warning(this,
                                     "Гражданин РФ - нет действующего УДЛ!",
                                     "Внимание!\nУ застрахованного гражданина РФ возрастом 0-14 лет нет действующего УДЛ.\n\n"
                                     "Проверьте, внесён ли в базу данных и не истёк ли срок действия документа:\n"
                                     "  1) свидетельство о рождении, выданное в Российской Федерации,\n"
                                     "  2) свидетельство о рождении, выданное не в Российской Федерации.\n");
        } else {
            d_0_14 = false;
        }

        if ( DY>=14 && (DY<20 || (DY==20 && DM<=1 && DD<=30 ) ) ) {
            // 14-20 лет
            d_14_20 = ( // проверка возраста и типа документа
                         ( ( docs_array[13] || docs_array[14] ) ) &&
                           ( ( // проверка даты выдачи документа 13
                               ( (d_date::dY(data_pers.date_birth, docs_date[13])>=14) ||
                                 (d_date::dY(data_pers.date_birth, docs_date[13])==14 && d_date::dM(data_pers.date_birth, docs_date[13])>=0) ||
                                 (d_date::dY(data_pers.date_birth, docs_date[13])==14 && d_date::dM(data_pers.date_birth, docs_date[13])==0 && d_date::dD(data_pers.date_birth, docs_date[13])>=0) ) &&
                               ( (d_date::dY(data_pers.date_birth, docs_date[13])<=20) ||
                                 (d_date::dY(data_pers.date_birth, docs_date[13])==20 && d_date::dM(data_pers.date_birth, docs_date[13])<=1) ||
                                 (d_date::dY(data_pers.date_birth, docs_date[13])==20 && d_date::dM(data_pers.date_birth, docs_date[13])==1 && d_date::dD(data_pers.date_birth, docs_date[13])<=30) )
                             ) ||
                             ( // проверка даты выдачи документа 14
                               ( (d_date::dY(data_pers.date_birth, docs_date[14])>=14) ||
                                 (d_date::dY(data_pers.date_birth, docs_date[14])==14 && d_date::dM(data_pers.date_birth, docs_date[14])>=0) ||
                                 (d_date::dY(data_pers.date_birth, docs_date[14])==14 && d_date::dM(data_pers.date_birth, docs_date[14])==0 && d_date::dD(data_pers.date_birth, docs_date[14])>=0) ) &&
                               ( (d_date::dY(data_pers.date_birth, docs_date[14])<=20) ||
                                 (d_date::dY(data_pers.date_birth, docs_date[14])==20 && d_date::dM(data_pers.date_birth, docs_date[14])<=1) ||
                                 (d_date::dY(data_pers.date_birth, docs_date[14])==20 && d_date::dM(data_pers.date_birth, docs_date[14])==1 && d_date::dD(data_pers.date_birth, docs_date[14])<=30) )
                             )
                          )
                       );
            if (!d_14_20)
                QMessageBox::warning(this,
                                     "Гражданин РФ - нет действующего УДЛ!",
                                     "Внимание!\nУ застрахованного гражданина РФ возрастом 14-20 лет нет действующего УДЛ.\n\n"
                                     "Проверьте, внесён ли в базу данных и не истёк ли срок действия документа:\n"
                                     "  1) паспорт гражданина Российской Федерации,\n"
                                     "  2) временное удостоверение личности гражданина Российской Федерации.\n");
        } else {
            d_14_20 = false;
        }

        if ( DY>=20 && (DY<45 || (DY==45 && DM<=1 && DD<=30 ) ) ) {
            // 20-45 лет
            bool b_doc = ( docs_array[13] || docs_array[14] );
            bool b_doc13 = ( // проверка даты выдачи документа 13
                             ( (d_date::dY(data_pers.date_birth, docs_date[13])>=20) ||
                               (d_date::dY(data_pers.date_birth, docs_date[13])==20 && d_date::dM(data_pers.date_birth, docs_date[13])>=0) ||
                               (d_date::dY(data_pers.date_birth, docs_date[13])==20 && d_date::dM(data_pers.date_birth, docs_date[13])==0 && d_date::dD(data_pers.date_birth, docs_date[13])>=0) ) &&
                             ( (d_date::dY(data_pers.date_birth, docs_date[13])<=45) ||
                               (d_date::dY(data_pers.date_birth, docs_date[13])==45 && d_date::dM(data_pers.date_birth, docs_date[13])<=1) ||
                               (d_date::dY(data_pers.date_birth, docs_date[13])==45 && d_date::dM(data_pers.date_birth, docs_date[13])==1 && d_date::dD(data_pers.date_birth, docs_date[13])<=30) )
                           );
            bool b_doc14 = ( // проверка даты выдачи документа 14
                             ( (d_date::dY(data_pers.date_birth, docs_date[14])>=20) ||
                               (d_date::dY(data_pers.date_birth, docs_date[14])==20 && d_date::dM(data_pers.date_birth, docs_date[14])>=0) ||
                               (d_date::dY(data_pers.date_birth, docs_date[14])==20 && d_date::dM(data_pers.date_birth, docs_date[14])==0 && d_date::dD(data_pers.date_birth, docs_date[14])>=0) ) &&
                             ( (d_date::dY(data_pers.date_birth, docs_date[14])<=45) ||
                               (d_date::dY(data_pers.date_birth, docs_date[14])==45 && d_date::dM(data_pers.date_birth, docs_date[14])<=1) ||
                               (d_date::dY(data_pers.date_birth, docs_date[14])==45 && d_date::dM(data_pers.date_birth, docs_date[14])==1 && d_date::dD(data_pers.date_birth, docs_date[14])<=30) )
                           );
            d_20_45 = // проверка возраста и типа документа
                      ( b_doc ) && ( b_doc13 || b_doc14 );
            if (!d_20_45 && DY<45)
                QMessageBox::warning(this,
                                     "Гражданин РФ - нет действующего УДЛ!",
                                     "Внимание!\nУ застрахованного гражданина РФ возрастом 20-45 лет нет действующего УДЛ.\n\n"
                                     "Проверьте, внесён ли в базу данных и не истёк ли срок действия документа:\n"
                                     "  1) паспорт гражданина Российской Федерации,\n"
                                     "  2) временное удостоверение личности гражданина Российской Федерации.\n");
        } else {
            d_20_45 = false;
        }

        if ( DY>=45 ) {
            bool b_doc = ( docs_array[13] || docs_array[14] || docs_array[1] );
            bool b_doc13 = ( // проверка даты выдачи документа 13
                             (d_date::dY(data_pers.date_birth, docs_date[13])>=45) ||
                             (d_date::dY(data_pers.date_birth, docs_date[13])==45 && d_date::dM(data_pers.date_birth, docs_date[13])>=0) ||
                             (d_date::dY(data_pers.date_birth, docs_date[13])==45 && d_date::dM(data_pers.date_birth, docs_date[13])==0 && d_date::dD(data_pers.date_birth, docs_date[13])>=0)
                           );
            bool b_doc14 = ( // проверка даты выдачи документа 14
                             (d_date::dY(data_pers.date_birth, docs_date[14])>=45) ||
                             (d_date::dY(data_pers.date_birth, docs_date[14])==45 && d_date::dM(data_pers.date_birth, docs_date[14])>=0) ||
                             (d_date::dY(data_pers.date_birth, docs_date[14])==45 && d_date::dM(data_pers.date_birth, docs_date[14])==0 && d_date::dD(data_pers.date_birth, docs_date[14])>=0)
                           );
            bool b_doc1  = ( // проверка даты выдачи документа 1
                             (d_date::dY(data_pers.date_birth, docs_date[1])>=45) ||
                             (d_date::dY(data_pers.date_birth, docs_date[1])==45 && d_date::dM(data_pers.date_birth, docs_date[1])>=0) ||
                             (d_date::dY(data_pers.date_birth, docs_date[1])==45 && d_date::dM(data_pers.date_birth, docs_date[1])==0 && d_date::dD(data_pers.date_birth, docs_date[1])>=0)
                           );
            d_45_ =  ( b_doc ) && ( b_doc13 || b_doc14 || b_doc1 );

            if (!d_45_ && !d_20_45)
                QMessageBox::warning(this,
                                     "Гражданин РФ - нет действующего УДЛ!",
                                     "Внимание!\nУ застрахованного гражданина РФ возрастом 45 лет и старше нет действующего УДЛ.\n\n"
                                     "Проверьте, внесён ли в базу данных и не истёк ли срок действия документа:\n"
                                     "  1) паспорт гражданина Российской Федерации,\n"
                                     "  2) паспорт гражданина СССР,\n"
                                     "  3) временное удостоверение личности гражданина Российской Федерации.\n");
        } else {
            d_45_ = false;
        }

        docs_ok =
              ( // до 14 лет +1 мес -- свидетельство о рождении
                d_0_14  ||
                // с 14 до 20 лет +1 мес - паспорт или справка, выданные не раньше 14 лет -1 мес.
                d_14_20 ||
                // с 20 до 45 лет +1 мес - паспорт или справка, выданные не раньше 20 лет -1 мес.
                d_20_45 ||
                // с 45 лет +1 мес - паспорт, справка или паспорт СССР, выданные не раньше 45 лет -1 мес.
                d_45_
              );
    }
    break;
    case 5:
    case 10:
    {
        docs_ok = ( docs_array[12] || docs_array[10] || docs_array[25] || docs_array[27] || docs_array[28] );
        if (!docs_ok)
            QMessageBox::warning(this,
                                 "Беженец - нет действующего УДЛ!",
                                 "Внимание!\nУ застрахованного беженца нет действующего УДЛ.\n\n"
                                 "Проверьте, внесён ли в базу данных и не истёк ли срок действия документа:\n"
                                 "  1) удостоверение беженца,\n"
                                 "  2) cвидетельство о регистрации ходатайства о признании иммигранта беженцем на территории Российской Федерации,\n"
                                 "  3) cвидетельство о предоставлении временного убежища на территории Российской Федерации,\n"
                                 "  4) копия жалобы о лишении статуса беженца,\n"
                                 "  5) иной документ, соответствующий свидетельству о предоставлении убежища на территории Российской Федерации.\n");
    }
    break;
    case 2:
    case 7:
    {
        docs_ok = ( docs_array[9] || docs_array[21] );
        if (!docs_ok)
            QMessageBox::warning(this,
                                 "Иностранный гражданин постоянно проживающий в РФ - нет действующего УДЛ!",
                                 "Внимание!\nУ застрахованного иностранного гражданина нет действующего УДЛ.\n\n"
                                 "Проверьте, внесён ли в базу данных и не истёк ли срок действия документа:\n"
                                 "  1) паспорт иностранного гражданина,\n"
                                 "  2) документ иностранного гражданина.\n");
        allow_ok = ( docs_array[11] );
        if (!allow_ok)
            QMessageBox::warning(this,
                                 "Иностранный гражданин постоянно проживающий в РФ - нет действующего ДРП!",
                                 "Внимание!\nУ застрахованного иностранного гражданина постоянно проживающего в РФ нет действующего вида на жительство.\n\n"
                                 "Проверьте, внесён ли в базу данных и не истёк ли срок действия документа:\n"
                                 "  1) вид на жительство.\n");
    }
    break;
    case 3:
    case 8:
    {
        docs_ok = ( docs_array[9] || docs_array[21] );
        if (!docs_ok)
            QMessageBox::warning(this,
                                 "Иностранный гражданин временно проживающий в РФ - нет действующего УДЛ!",
                                 "Внимание!\nУ застрахованного иностранного гражданина нет действующего УДЛ.\n\n"
                                 "Проверьте, внесён ли в базу данных и не истёк ли срок действия документа:\n"
                                 "  1) паспорт иностранного гражданина,\n"
                                 "  2) документ иностранного гражданина.\n");
        allow_ok = ( docs_array[23] );
        if (!allow_ok)
            QMessageBox::warning(this,
                                 "Иностранный гражданин временно проживающий в РФ - нет действующего ДРП!",
                                 "Внимание!\nУ застрахованного иностранного гражданина временно проживающего в РФ нет действующего ДРП.\n\n"
                                 "Проверьте, внесён ли в базу данных и не истёк ли срок действия документа:\n"
                                 "  1) разрешение на временное проживание.\n");
    }
    break;
    case 4:
    case 9:
    {
        docs_ok = ( docs_array[22] );
        if (!docs_ok)
            QMessageBox::warning(this,
                                 "Лицо без гражданства - нет действующего УДЛ!",
                                 "Внимание!\nУ застрахованного лица без гражданства нет действующего УДЛ.\n\n"
                                 "Проверьте, внесён ли в базу данных и не истёк ли срок действия документа:\n  1) документ лица без гражданства.");

    }
    break;
    }

    ui->bn_select_rep->setEnabled(docs_ok);

    /*if (!docs_ok)
        QMessageBox::warning(this, "Нет действующего УДЛ!", "Внимание!\nУ застрахованного нет действующего УДЛ.\n\n1) Возможно пришло время для плановой замены паспорта.\n2) Проверьте типы документов, даты их выдачи и сроки действия (если есть).");
    */
    this->setCursor(Qt::ArrowCursor);
}


void representers_wnd::on_spin_pers_pan_category_valueChanged(int arg1) {
    ui->combo_pers_pan_category->setCurrentIndex(ui->combo_pers_pan_category->findData(arg1));
}
void representers_wnd::on_combo_pers_pan_category_activated(int index) {
    //test_for_UDL();
    ui->spin_pers_pan_category->setValue(ui->combo_pers_pan_category->currentData().toInt());
    b_need_save();
}
void representers_wnd::on_line_docs_pan_ser_textChanged(const QString &arg1) {
    data_doc.doc_ser = arg1;
}
void representers_wnd::on_line_docs_pan_num_textChanged(const QString &arg1) {
    data_doc.doc_num = arg1;
}
void representers_wnd::on_combo_docs_pan_orgcode_currentTextChanged(const QString &arg1) {
    data_doc.has_org_code = (ui->combo_docs_pan_orgcode->currentIndex()>0);
    data_doc.org_code = arg1;
}
void representers_wnd::on_date_docs_pan_start_dateChanged(const QDate &date) {
    data_doc.doc_date = date;
}
void representers_wnd::on_date_docs_pan_exp_dateChanged(const QDate &date) {
    data_doc.doc_exp = date;
}
void representers_wnd::on_ch_docs_pan_actual_clicked(bool checked) {
    data_doc.status = checked;
}

void representers_wnd::on_tab_pers_docs_doubleClicked(const QModelIndex &index) {
    on_bn_docs_edit_clicked();
}

void representers_wnd::on_combo_pers_pan_category_activated(const QString &arg1) {
    on_bn_pers_save_clicked();
}

void representers_wnd::on_ch_docs_tab_only_acts_stateChanged(int arg1) {
    refresh_pers_docs();
}

#include "qt_windows.h"
#include "qwindowdefs_win.h"
#include <shellapi.h>


void representers_wnd::on_ch_pers_fam_clicked() {
    refresh_representers_tab();
}
void representers_wnd::on_ch_pers_im_clicked() {
    refresh_representers_tab();
}
void representers_wnd::on_ch_pers_ot_clicked() {
    refresh_representers_tab();
}
void representers_wnd::on_ch_pers_sex_clicked() {
    refresh_representers_tab();
}
void representers_wnd::on_ch_pers_birth_clicked() {
    refresh_representers_tab();
}
void representers_wnd::on_ch_pers_snils_clicked() {
    refresh_representers_tab();
}

void representers_wnd::on_combo_docs_pan_orgcode_currentIndexChanged(const QString &arg1) {
    ui->line_docs_pan_ogrname->setText(ui->combo_docs_pan_orgcode->currentData().toString());
    ui->bn_add_orgcode->setEnabled(ui->line_docs_pan_ogrname->toPlainText().isEmpty());
    ui->bn_del_orgcode->setEnabled( (ui->combo_docs_pan_orgcode->currentIndex()>0) && (!ui->combo_docs_pan_orgcode->currentText().isEmpty()) );
}
void representers_wnd::on_combo_docs_pan_orgcode_editTextChanged(const QString &arg1) {
    on_combo_docs_pan_orgcode_currentIndexChanged(arg1);
}

void representers_wnd::on_bn_add_orgcode_clicked() {
    if (ui->combo_docs_pan_orgcode->currentIndex()==0)
        return;
    if (ui->line_docs_pan_ogrname->toPlainText().trimmed().isEmpty())
        return;
    if (ui->combo_docs_pan_orgcode->currentText().trimmed().isEmpty())
        return;
    QString sql_tst = "select id from orgs where org_code='" + ui->combo_docs_pan_orgcode->currentText() + "' ; ";
    QSqlQuery *query_tst = new QSqlQuery(db);
    bool res_tst = mySQL.exec(this, sql_tst, "Проверка выдающего УДЛ или ДРП подразделения на дубли", *query_tst, true, db, data_app);
    if (query_tst->size()>0) {
        QMessageBox::warning(this, "Такой код уже есть!", "Такой код организации уже есть.\nОперация отменена.");
        return;
    }
    QString org_code = ui->combo_docs_pan_orgcode->currentText();
    QString sql_ins = "insert into orgs (org_code, org_name) "
                      " values('" + org_code + "', '" +
                                    ui->line_docs_pan_ogrname->toPlainText() + "') ; ";
    QSqlQuery *query_ins = new QSqlQuery(db);
    bool res_ins = mySQL.exec(this, sql_ins, "Добавление нового выдающего УДЛ или ДРП подразделения", *query_ins, true, db, data_app);
    refresh_docs_pan_orgcodes();
    ui->combo_docs_pan_orgcode->setCurrentIndex(ui->combo_docs_pan_orgcode->findText(org_code));
    ui->line_docs_pan_ogrname->setText(ui->combo_docs_pan_orgcode->currentData().toString());
    return;
}

void representers_wnd::on_bn_del_orgcode_clicked() {
    if (ui->combo_docs_pan_orgcode->currentIndex()==0)
        return;
    QString sql_del = "delete from orgs where org_code='" + ui->combo_docs_pan_orgcode->currentText() + "' ; ";
    QSqlQuery *query_del = new QSqlQuery(db);
    bool res_del = mySQL.exec(this, sql_del, "Удаления выдающего УДЛ или ДРП подразделения из базы", *query_del, true, db, data_app);
    refresh_docs_pan_orgcodes();
    return;
}

void representers_wnd::on_spin_docs_pan_type_valueChanged(int arg1) {
    ui->combo_docs_pan_type->setCurrentIndex(ui->combo_docs_pan_type->findData(arg1));
    on_combo_docs_pan_type_activated(arg1);
}


void representers_wnd::on_ln_pers_pan_plc_birth_editingFinished() {
    ui->ch_pers_pan_ot->setChecked(!(ui->ln_pers_pan_ot->text().isEmpty()));

    QString s = ui->line_pers_pan_snils->text().trimmed().replace(" ","").replace("-","");
    ui->ch_pers_pan_snils->setChecked(!s.isEmpty());
    b_need_save(true);
}

//void representers_wnd::on_ln_pers_pan_fam_editingFinished()
//{  b_need_save(true);  }
//void representers_wnd::on_ln_pers_pan_im_editingFinished()
//{  b_need_save(true);  }
//void representers_wnd::on_ln_pers_pan_ot_editingFinished()
//{  b_need_save(true);  }
void representers_wnd::on_date_pers_pan_birth_editingFinished()
{  b_need_save(true);  }
void representers_wnd::on_line_pers_pan_snils_editingFinished()
{  b_need_save(true);  }
void representers_wnd::on_date_pers_pan_death_editingFinished()
{  b_need_save(true);  }
void representers_wnd::on_ln_pers_pan_phone_code_editingFinished()
{  b_need_save(true);  }
void representers_wnd::on_ln_pers_pan_phone_work_editingFinished()
{  b_need_save(true);  }
void representers_wnd::on_ln_pers_pan_phone_home_editingFinished()
{  b_need_save(true);  }
void representers_wnd::on_ln_pers_pan_phone_cell_editingFinished()
{  b_need_save(true);  }
void representers_wnd::on_ln_pers_pan_email_editingFinished()
{  b_need_save(true);  }
void representers_wnd::on_spin_pers_pan_category_editingFinished()
{  b_need_save(true);  }


void representers_wnd::on_ch_docs_clicked(bool checked) {
    ui->pan_docs_tab->setVisible(checked);  }
void representers_wnd::on_ch_phones_clicked(bool checked) {
    ui->pan_phones->setVisible(checked);  }


void representers_wnd::on_ln_pers_pan_fam_old_textChanged(const QString &arg1) {
    data_pers.fam_old = arg1.trimmed();
    ui->ch_pers_pan_fam_old->setChecked(!(arg1.trimmed().isEmpty()));
    ui->lab_pers_pan_fio_old->setText( ui->ln_pers_pan_fam_old->text() + " " + ui->ln_pers_pan_im_old->text() + " " + ui->ln_pers_pan_ot_old->text() );
    b_need_save(true);
}
void representers_wnd::on_ln_pers_pan_im_old_textChanged(const QString &arg1) {
    data_pers.im_old = arg1.trimmed();
    ui->ch_pers_pan_im_old->setChecked(!(arg1.trimmed().isEmpty()));
    ui->lab_pers_pan_fio_old->setText( ui->ln_pers_pan_fam_old->text() + " " + ui->ln_pers_pan_im_old->text() + " " + ui->ln_pers_pan_ot_old->text() );
    b_need_save(true);
}
void representers_wnd::on_ln_pers_pan_ot_old_textChanged(const QString &arg1) {
    data_pers.ot_old = arg1.trimmed();
    ui->ch_pers_pan_ot_old->setChecked(!(arg1.trimmed().isEmpty()));
    ui->lab_pers_pan_fio_old->setText( ui->ln_pers_pan_fam_old->text() + " " + ui->ln_pers_pan_im_old->text() + " " + ui->ln_pers_pan_ot_old->text() );
    b_need_save(true);
}
void representers_wnd::on_combo_pers_pan_sex_old_currentIndexChanged(int index) {
    data_pers.sex_old = index;
    ui->ch_pers_pan_sex_old->setChecked(index!=0);
    ui->lab_pers_pan_fio_old->setText( ui->ln_pers_pan_fam_old->text() + " " + ui->ln_pers_pan_im_old->text() + " " + ui->ln_pers_pan_ot_old->text() );
    b_need_save(true);
}
void representers_wnd::on_date_pers_pan_birth_old_dateChanged(const QDate &date) {
    data_pers.date_birth_old = date;
    ui->ch_pers_pan_date_birth_old->setChecked(date>=QDate(1920,1,1));
    ui->lab_pers_pan_fio_old->setText( ui->ln_pers_pan_fam_old->text() + " " + ui->ln_pers_pan_im_old->text() + " " + ui->ln_pers_pan_ot_old->text() );
    b_need_save(true);
}
void representers_wnd::on_line_pers_pan_snils_old_textChanged(const QString &arg1) {
    data_pers.snils_old = arg1.trimmed().replace(" ","").replace("-","");
    ui->ch_pers_pan_snils_old->setChecked(!(data_pers.snils_old.isEmpty()));
    ui->lab_pers_pan_fio_old->setText( ui->ln_pers_pan_fam_old->text() + " " + ui->ln_pers_pan_im_old->text() + " " + ui->ln_pers_pan_ot_old->text() );
    b_need_save(true);
}
void representers_wnd::on_ln_pers_pan_plc_birth_old_textChanged(const QString &arg1) {
    data_pers.plc_birth_old = arg1.trimmed();
    ui->ch_pers_pan_plc_birth_old->setChecked(!(arg1.trimmed().isEmpty()));
    ui->lab_pers_pan_fio_old->setText( ui->ln_pers_pan_fam_old->text() + " " + ui->ln_pers_pan_im_old->text() + " " + ui->ln_pers_pan_ot_old->text() );
    b_need_save(true);
}

void representers_wnd::on_ch_pers_pan_fam_old_clicked(bool checked) {
    if (!checked) ui->ln_pers_pan_fam_old->clear();  }
void representers_wnd::on_ch_pers_pan_im_old_clicked(bool checked) {
    if (!checked) ui->ln_pers_pan_im_old->clear();  }
void representers_wnd::on_ch_pers_pan_ot_old_clicked(bool checked) {
    if (!checked) ui->ln_pers_pan_ot_old->clear();  }
void representers_wnd::on_ch_pers_pan_sex_old_clicked(bool checked) {
    if (!checked) ui->combo_pers_pan_sex_old->setCurrentIndex(0);  }
void representers_wnd::on_ch_pers_pan_date_birth_old_clicked(bool checked) {
    if (!checked) ui->date_pers_pan_birth_old->setDate(QDate(1900,1,1));  }
void representers_wnd::on_ch_pers_pan_snils_old_clicked(bool checked) {
    if (!checked) ui->line_pers_pan_snils_old->clear();  }
void representers_wnd::on_ch_pers_pan_plc_birth_old_clicked(bool checked) {
    if (!checked) ui->ln_pers_pan_plc_birth_old->clear();  }


void representers_wnd::on_ch_person_old_clicked(bool checked) {
    ui->pan_person_old->setVisible(checked);
}


void representers_wnd::on_bn_select_rep_clicked() {
    on_bn_pers_save_clicked();

    rep_id  = data_pers.id;
    rep_sex = data_pers.sex;
    rep_fio = data_pers.fam + " " + data_pers.im + " " + data_pers.ot;

    accepted();
    close();
}

void representers_wnd::on_line_pers_snils_textEdited(const QString &arg1) {
    QString txt0 = arg1.trimmed(), txt = "";
    for (int i=0; i<txt0.size(); ++i) {
        if (txt.length()>=14)
            break;
        QString c = txt0.mid(i, 1);
        if ( c=="0" || c=="1" || c=="2" || c=="3" || c=="4" || c=="5" || c=="6" || c=="7" || c=="8" || c=="9" ) {
            txt += c;
        }
    }
    ui->line_pers_snils->setText(txt);
    ui->line_pers_snils->setCursorPosition(txt.length());
}


// редактирование поля DOST
void representers_wnd::on_ch_dost_clicked(bool checked) {
    ui->pan_dost->setVisible(checked);
}

void representers_wnd::on_ch_dost_fam_clicked(bool checked) {
    data_pers.dost = data_pers.dost.left(1) + QString(checked ? "_" : "2") + data_pers.dost.right(4);
    ui->lab_pers_pan_dost->setText(data_pers.dost);
    if (checked) {
        ui->ch_dost_im->setEnabled(true);
    } else {
        ui->ch_dost_im->setEnabled(false);
        ui->ch_dost_im->setChecked(true);
        on_ch_dost_im_clicked(true);
    }
    b_need_save(true);
}
void representers_wnd::on_ch_dost_im_clicked(bool checked) {
    data_pers.dost = data_pers.dost.left(2) + QString(checked ? "_" : "3") + data_pers.dost.right(3);
    ui->lab_pers_pan_dost->setText(data_pers.dost);
    if (checked) {
        ui->ch_dost_fam->setEnabled(true);
    } else {
        ui->ch_dost_fam->setEnabled(false);
        ui->ch_dost_fam->setChecked(true);
        on_ch_dost_fam_clicked(true);
    }
    b_need_save(true);
}
void representers_wnd::on_ch_dost_ot_clicked(bool checked) {
    data_pers.dost = QString(checked ? "_" : "1") + data_pers.dost.right(5);
    ui->ch_pers_pan_ot->setChecked(checked);
    on_ch_pers_pan_ot_clicked(checked);
    ui->lab_pers_pan_dost->setText(data_pers.dost);
    b_need_save(true);
}
void representers_wnd::on_ch_dost_day_clicked(bool checked) {
    data_pers.dost = data_pers.dost.left(3) + QString(checked ? "_" : "4") + data_pers.dost.right(2);
    ui->lab_pers_pan_dost->setText(data_pers.dost);
    b_need_save(true);
}
void representers_wnd::on_ch_dost_mon_clicked(bool checked) {
    data_pers.dost = data_pers.dost.left(4) + QString(checked ? "_" : "5") + data_pers.dost.right(1);
    ui->ch_dost_day->setEnabled(checked);
    if (!checked)  {
        ui->ch_dost_day->setChecked(false);
        data_pers.dost = data_pers.dost.left(3) + "4" + data_pers.dost.right(2);
    }
    ui->lab_pers_pan_dost->setText(data_pers.dost);
    b_need_save(true);
}
void representers_wnd::on_ch_dost_year_clicked(bool checked) {
    data_pers.dost = data_pers.dost.left(5) + QString(checked ? "_" : "6");
    ui->lab_pers_pan_dost->setText(data_pers.dost);
    b_need_save(true);
}
