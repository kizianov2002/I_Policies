#include "add_blank_wnd.h"
#include "ui_add_blank_wnd.h"
#include "blanks_wnd.h"

add_blank_wnd::add_blank_wnd(QSqlDatabase &db, s_data_app &data_app, s_data_new_blank &data_new_blank, QSettings &settings, QWidget *parent)
    : db(db), data_app(data_app), data_new_blank(data_new_blank), settings(settings), QDialog(parent), ui(new Ui::add_blank_wnd)
{
    ui->setupUi(this);

    refresh_blanks_forms();
    refresh_blanks_statuses();

    on_bn_now_clicked();

    // внесём переданные значения в поля ввода

    QString s1 = data_new_blank.fam;
    s1 = s1.replace("A","А")
           .replace("C","С")
           .replace("E","Е")
           .replace("H","Н")
           .replace("K","К")
           .replace("M","М")
           .replace("O","О")
           .replace("P","Р")
           .replace("T","Т")
           .replace("X","Х")
           .replace("Y","У");
    ui->ln_filter_fam->setText(s1);

    QString s2 = data_new_blank.im;
    s2 = s2.replace("A","А")
           .replace("C","С")
           .replace("E","Е")
           .replace("H","Н")
           .replace("K","К")
           .replace("M","М")
           .replace("O","О")
           .replace("P","Р")
           .replace("T","Т")
           .replace("X","Х")
           .replace("Y","У");
    ui->ln_filter_im->setText(s2);

    QString s3 = data_new_blank.ot;
    s3 = s3.replace("A","А")
           .replace("C","С")
           .replace("E","Е")
           .replace("H","Н")
           .replace("K","К")
           .replace("M","М")
           .replace("O","О")
           .replace("P","Р")
           .replace("T","Т")
           .replace("X","Х")
           .replace("Y","У");
    ui->ln_filter_ot->setText(s3);
    refresh_persons();

    if (data_new_blank.fam.trimmed().isEmpty())
        data_new_blank.fam = "-";
    if (data_new_blank.im.trimmed().isEmpty())
        data_new_blank.im = "-";
    if (data_new_blank.ot.trimmed().isEmpty())
        data_new_blank.ot = "-";
    QString s = data_new_blank.fam.trimmed().toUpper() + " " +
                data_new_blank.im.trimmed().toUpper()  + " " +
                data_new_blank.ot.trimmed().toUpper();
    if (s=="- - -") {
        //s += ", " + data_new_blank.date_birth.toString("yyyy-MM-dd");
        ui->lab_person->setText("      <<  выберите из списка  >>      ");
    } else {
        s += ", " + data_new_blank.date_birth.toString("yyyy-MM-dd");
        ui->lab_person->setText(s);
    }

    ui->ln_pol_ser->setCursorPosition(0);
    ui->ln_pol_num->setCursorPosition(0);
    ui->ln_enp->setCursorPosition(0);

    //ui->combo_fpolis->setCurrentIndex(0);
    ui->combo_fpolis->setCurrentIndex(ui->combo_fpolis->findData(1));
    ui->combo_status->setCurrentIndex(ui->combo_status->findData(0));
    //ui->combo_status->setCurrentIndex(2);

    ui->ln_pol_ser->setText(data_new_blank.pol_ser);
    ui->ln_pol_num->setText(data_new_blank.pol_num);
    ui->ln_enp->setText(data_new_blank.enp);
    ui->date_pol_get->setDate(QDate::currentDate());
    ui->date_pol_exp->setDate(data_new_blank.date_exp);
    ui->ch_pol_exp->setChecked( data_new_blank.date_exp.isValid() && data_new_blank.date_exp>QDate::currentDate() );
    on_ch_pol_exp_clicked(ui->ch_pol_exp->isChecked());
    ui->lab_eds->setText(data_new_blank.eds);

    if ( data_new_blank.f_polis!=1 &&
         data_new_blank.f_polis!=2 &&
         data_new_blank.f_polis!=3 ) {
        data_new_blank.f_polis = data_new_blank.pol_ser.left(2).toInt();
    }
    if (data_new_blank.f_polis==0) {
        data_new_blank.f_polis =1;
    }
    ui->combo_fpolis->setCurrentIndex(ui->combo_fpolis->findData(data_new_blank.f_polis));

    if (ui->ln_pol_ser->text().trimmed().isEmpty()) {
        ui->ln_pol_ser->setFocus();
        ui->ln_pol_ser->setCursorPosition(0);
    } else if (ui->ln_pol_num->text().trimmed().isEmpty()) {
        ui->ln_pol_num->setFocus();
        ui->ln_pol_num->setCursorPosition(0);
    }
}

add_blank_wnd::~add_blank_wnd() {
    delete ui;
}

void add_blank_wnd::on_bn_now_clicked()
{   ui->date_pol_get->setDate(QDate::currentDate());   }
void add_blank_wnd::on_bn_exp_now_clicked()
{   ui->date_pol_exp->setDate(QDate::currentDate());   }


void add_blank_wnd::on_ch_pol_exp_clicked(bool checked) {
    ui->date_pol_exp->setEnabled(checked);
    ui->bn_exp_now->setEnabled(checked);
}

void add_blank_wnd::on_bn_cansel_clicked() {
    close();
}

void add_blank_wnd::refresh_blanks_forms() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка форм выпуска полисов
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, text "
                  " from spr_fpolis s "
                  " where s.code>0 "
                  " order by s.code ; ";
    mySQL.exec(this, sql, QString("формы выпуска бланков"), *query, true, db, data_app);
    ui->combo_fpolis->clear();
    while (query->next()) {
        int n = query->value(0).toInt();
        ui->combo_fpolis->addItem("(" + QString::number(n) + ") " + query->value(1).toString(), n);
    }
    ui->combo_fpolis->setCurrentIndex(ui->combo_fpolis->findData(1));
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void add_blank_wnd::refresh_blanks_statuses() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка статусов
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, text "
                  " from spr_blank_status s "
                  " order by s.code ; ";
    mySQL.exec(this, sql, QString("список статусов бланков"), *query, true, db, data_app);
    ui->combo_status->clear();
    while (query->next()) {
        int n = query->value(0).toInt();
        ui->combo_status->addItem("(" + QString::number(n) + ") " + query->value(1).toString(), n);
    }
    ui->combo_status->setCurrentIndex(ui->combo_status->findData(0));
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void add_blank_wnd::refresh_persons() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка застрахованных
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select id, COALESCE(fam,'-')||' '||COALESCE(im,'-')||' '||COALESCE(ot,'-')||', '||date_birth || '  (ID=' || id || ')' as pers_fio "
                  " from persons s "
                  " where fam like ('" + ui->ln_filter_fam->text().trimmed().toUpper() + "%') "
                  "   and im  like ('" + ui->ln_filter_im->text().trimmed().toUpper()  + "%') "
                  "   and COALESCE(ot,'-')  like ('" + ui->ln_filter_ot->text().trimmed().toUpper()  + "%') "
                  "   and s.status>-100 "
                  " order by COALESCE(fam,'-')||' '||COALESCE(im,'-')||' '||COALESCE(ot,'-')||', '||date_birth || '  (ID=' || id || ')' ; ";
    mySQL.exec(this, sql, QString("список персон"), *query, true, db, data_app);
    ui->combo_person->clear();
    persons_list.clear();
    int id = -1;
    while (query->next()) {
        id = query->value(0).toInt();
        ui->combo_person->addItem(query->value(1).toString(), id);
        persons_list.append(query->value(1).toString());
    }
    ui->combo_person->setCurrentIndex(ui->combo_person->findData(id));
    delete query;
    this->setCursor(Qt::ArrowCursor);
}


void add_blank_wnd::on_ln_pol_ser_textChanged(const QString &arg1) {
    ui->ln_pol_ser->setText(ui->ln_pol_ser->text().trimmed().replace(" ", ""));
    ui->ln_pol_ser->setCursorPosition(ui->ln_pol_ser->text().size());
}

void add_blank_wnd::on_ln_pol_num_textChanged(const QString &arg1) {
    ui->ln_pol_num->setText(ui->ln_pol_num->text().trimmed().replace(" ", ""));
    ui->ln_pol_num->setCursorPosition(ui->ln_pol_num->text().size());
}

void add_blank_wnd::on_ln_enp_textChanged(const QString &arg1) {
    ui->ln_enp->setText(ui->ln_enp->text().trimmed().replace(" ", ""));
    ui->ln_enp->setCursorPosition(ui->ln_enp->text().size());
}



void add_blank_wnd::on_bn_add_clicked() {
    ui->ln_pol_ser->setCursorPosition(0);
    ui->ln_pol_num->setCursorPosition(0);
    ui->ln_enp->setCursorPosition(0);

    // Проверим заполнение полей
    if (ui->ln_pol_ser->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Не хватает данных!", "Серия бланка полиса не должна былть пустой.");
        return;
    }
    if (ui->ln_pol_ser->text().trimmed().length()!=4 && QMessageBox::warning(this, "Ошибка в данных!", "Серия бланка полиса должна состоять из 4 цифр.")) {
        return;
    }
    if (ui->ln_pol_num->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Не хватает данных!", "Номер бланка полиса не может быть пустым.");
        return;
    }
    if (ui->combo_person->currentData().toInt()<=0) {
        QMessageBox::warning(this, "Не хватает данных!", "Выберите застрахованного, на которого выпущен этот бланк.");
        return;
    }
    if (ui->ln_enp->text().trimmed().isEmpty() && QMessageBox::warning(this, "Не хватает данных!", "Единый номер полиса (ЕНП) не может быть пустым.")) {
        return;
    }
    if (ui->ln_enp->text().trimmed().length()!=16 && QMessageBox::warning(this, "Ошибка в данных!", "Единый номер полиса должен состоять из 16 цифр.")) {
        return;
    }
    if (ui->date_pol_get->date()<QDate::currentDate().addMonths(-1) || ui->date_pol_get->date()>QDate::currentDate().addMonths(1)) {
        QMessageBox::warning(this, "Ошибка в данных!", "Дата добавления нового бланка полиса не может отстоять от текущей даты больше чем на 1 месяц.");
        return;
    }
    // проверим соответствие серии форме полиса
    if (ui->ln_pol_ser->text().left(ui->ln_pol_ser->text().size()-2).toInt() != ui->combo_fpolis->currentData().toInt()){
        QMessageBox::warning(this,
                             "Ошибка в данных!",
                             "Введённая серия полиса не соответствует выбранной форме выпуска полиса.\n\n" +
                             ui->ln_pol_ser->text() +
                             "\n\n" +
                             ui->ln_pol_ser->text().left(ui->ln_pol_ser->text().size()-2));
        return;
    }
    /*if (ui->combo_blank_pol_status->currentData().toInt()==0) {
        QMessageBox::warning(this, "Не хватает данных!", "Выберите статус бланка полиса.");
        return;
    }*/
    // проверим серию и номер бланка полиса на уникальность
    QString sql_tst = "select count(*) from blanks_pol where pol_ser='" +ui->ln_pol_ser->text()+ "' and pol_num='" +ui->ln_pol_num->text()+ "' ; ";
    QSqlQuery *query_test = new QSqlQuery(db);
    mySQL.exec(this, sql_tst, "Проверка серии и номера на уникальность", *query_test, true, db, data_app);
    if (query_test->next() && query_test->value(0).toInt()>0) {
        QMessageBox::warning(this, "Бланк с такой серией и номером уже есть в системе",
                             "В базе данных уже есть бланк с такой серией и номером. \n"
                             "Возможно, полис уже был добавлен ранее.\n\n"
                             "Операция отменена.");
        delete query_test;
        accept();
    } else {
        delete query_test;
    }

    // собственно сохранение
    QString sql = "insert into blanks_pol(pol_ser, pol_num, f_polis, enp, date_exp, eds, id_point, id_operator, id_person, id_polis, date_add, date_scan_enp, status) ";
    sql += " values (";
    sql += " '" + ui->ln_pol_ser->text().trimmed() + "', ";
    sql += " '" + ui->ln_pol_num->text().trimmed() + "', ";
    sql += " "  + QString::number(ui->combo_fpolis->currentData().toInt()) + ", ";
    sql += " '" + ui->ln_enp->text().trimmed() + "', ";
    sql += " "  + (ui->ch_pol_exp->isChecked() ? QString("'" + ui->date_pol_get->date().toString("yyyy-MM-dd") + "'") : "NULL" ) + ", ";
    sql += " "  + (ui->lab_eds->text().isEmpty() ? "NULL" : QString("'" + ui->lab_eds->text() + "'") ) + ", ";
    sql += " "  + QString::number(data_app.id_point) + ", ";
    sql += " "  + QString::number(data_app.id_operator) + ", ";
    sql += " "  + QString::number(ui->combo_person->currentData().toInt()) + ", ";
    sql += " NULL, ";
    sql += " '" + ui->date_pol_get->date().toString("yyyy-MM-dd") + "', ";
    sql += " '" + ui->date_pol_get->date().toString("yyyy-MM-dd") + "', ";
    //sql += " " + (data_new_blank.date_scan_enp>=QDate::currentDate().addMonths(-1) ? QString("'" + data_new_blank.date_scan_enp.toString("yyyy-MM-dd") + "'") : "NULL" ) + ", ";
    sql += " "  + QString::number(ui->combo_status->currentData().toInt()) + ") ; ";


    QSqlQuery *query = new QSqlQuery(db);
    if (!mySQL.exec(this, sql, "Добавление записи бланка полиса", *query, true, db, data_app) ) {
        QMessageBox::warning(this, "Не удалось добавить бланк",
                             "Новый бланк полиса единого образца с заданными параметрами добавить в базу данных не удалось\n\n"
                             "Проверьте правильность введённых данных и повторите попытку\n"
                             "или нажмите кнопку [Пропустить], чтобы перейти к следующему бланку в списке.");
    }
    delete query;

    // обновим окно бланков
    /*blanks_wnd *parent_w = (blanks_wnd*)parent();
    parent_w->refresh_blanks_pol_tab();
    QApplication::processEvents();*/

    /*// сбросим данные
    ui->combo_fpolis->setCurrentIndex(0);
    ui->combo_status->setCurrentIndex(2);
    ui->combo_person->setCurrentIndex(-1);
    ui->ln_pol_num->setText("");
    ui->ln_pol_ser->setText("");
    ui->ln_enp->setText("");
    ui->date_pol_get->setDate(QDate::currentDate());*/

    // закроем окно
    accept();
}

void add_blank_wnd::on_bn_next_clicked() {
    accept();
}

void add_blank_wnd::on_bn_clear_clicked() {
    ui->ln_filter_fam->clear();
    ui->ln_filter_im->clear();
    ui->ln_filter_ot->clear();
    //refresh_persons();
}

void add_blank_wnd::on_ln_filter_fam_returnPressed() {
    //refresh_persons();
}

void add_blank_wnd::on_ln_filter_im_returnPressed() {
    //refresh_persons();
}

void add_blank_wnd::on_ln_filter_ot_returnPressed() {
    //refresh_persons();
}

void add_blank_wnd::on_bn_clear_clicked(bool checked) {
    ui->ln_filter_fam->clear();
    ui->ln_filter_im->clear();
    ui->ln_filter_ot->clear();
    //refresh_persons();
}

void add_blank_wnd::on_ln_filter_fam_editingFinished() {
    QString s = ui->ln_filter_fam->text();
    s = s.replace("A","А")
         .replace("C","С")
         .replace("E","Е")
         .replace("H","Н")
         .replace("K","К")
         .replace("M","М")
         .replace("O","О")
         .replace("P","Р")
         .replace("T","Т")
         .replace("X","Х")
         .replace("Y","У");
    ui->ln_filter_fam->setText(s);
    refresh_persons();
}

void add_blank_wnd::on_ln_filter_im_editingFinished() {
    QString s = ui->ln_filter_im->text();
    s = s.replace("A","А")
         .replace("C","С")
         .replace("E","Е")
         .replace("H","Н")
         .replace("K","К")
         .replace("M","М")
         .replace("O","О")
         .replace("P","Р")
         .replace("T","Т")
         .replace("X","Х")
         .replace("Y","У");
    ui->ln_filter_im->setText(s);
    refresh_persons();
}

void add_blank_wnd::on_ln_filter_ot_editingFinished() {
    QString s = ui->ln_filter_ot->text();
    s = s.replace("A","А")
         .replace("C","С")
         .replace("E","Е")
         .replace("H","Н")
         .replace("K","К")
         .replace("M","М")
         .replace("O","О")
         .replace("P","Р")
         .replace("T","Т")
         .replace("X","Х")
         .replace("Y","У");
    ui->ln_filter_ot->setText(s);
    refresh_persons();
}

void add_blank_wnd::on_ln_enp_textEdited(const QString &arg1) {
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

void add_blank_wnd::on_ln_pol_ser_textEdited(const QString &arg1) {
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

void add_blank_wnd::on_ln_pol_num_textEdited(const QString &arg1) {
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
