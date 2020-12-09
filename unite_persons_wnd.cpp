#include "unite_persons_wnd.h"
#include "ui_unite_persons_wnd.h"

unite_persons_wnd::unite_persons_wnd(QSqlDatabase &db, s_data_pers_unite &data_pers_unite, s_data_app &data_app, QSettings &settings, QWidget *parent) :
    db(db), data_app(data_app), data_pers_unite(data_pers_unite), settings(settings), QDialog(parent), ui(new Ui::unite_persons_wnd)
{
    ui->setupUi(this);

    ui->lab_person1->setText(data_pers_unite.person1_fio);
    ui->ln_filter_fam->setText(data_pers_unite.fam);
    ui->ln_filter_im->setText(data_pers_unite.im);
    ui->ln_filter_ot->setText(data_pers_unite.ot);

    refresh_persons();
}

void unite_persons_wnd::refresh_persons() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка застрахованных
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select id, COALESCE(fam,'-')||' '||COALESCE(im,'-')||' '||COALESCE(ot,'-')||', '||date_birth || '  (ID=' || id || ') ' || ' status: ' || s.status as pers_fio, s.status "
                  " from persons s "
                  " where id<>" + QString::number(data_pers_unite.id_person1) + " "
                  "   and fam like ('" + ui->ln_filter_fam->text().trimmed().toUpper() + "%') "
                  "   and im  like ('" + ui->ln_filter_im->text().trimmed().toUpper()  + "%') "
                  "   and ot  like ('" + ui->ln_filter_ot->text().trimmed().toUpper()  + "%') "
                  "   and s.status>-100 "
                  " order by COALESCE(fam,'-')||' '||COALESCE(im,'-')||' '||COALESCE(ot,'-')||', '||date_birth || '  (ID=' || id || ')' ; ";
    mySQL.exec(this, sql, QString("список персон"), *query, true, db, data_app);
    ui->combo_person2->clear();
    while (query->next()) {
        int id = query->value(0).toInt();
        ui->combo_person2->addItem(query->value(1).toString(), id);
    }
    ui->combo_person2->setCurrentIndex(0);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

unite_persons_wnd::~unite_persons_wnd() {
    delete ui;
}

void unite_persons_wnd::on_bn_cansel_clicked() {
    reject();
}

void unite_persons_wnd::on_ln_filter_fam_editingFinished() {
    refresh_persons();
}

void unite_persons_wnd::on_ln_filter_im_editingFinished() {
    refresh_persons();
}

void unite_persons_wnd::on_ln_filter_ot_editingFinished() {
    refresh_persons();
}

void unite_persons_wnd::on_bn_process_clicked() {
    data_pers_unite.id_person2 = ui->combo_person2->currentData().toInt();
    accept();
}
