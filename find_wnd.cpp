#include "find_wnd.h"
#include "ui_find_wnd.h"
#include "persons_wnd.h"

find_wnd::find_wnd(QSqlDatabase &db, QSqlDatabase &db_FIAS, s_data_app &data_app, QSettings &settings, /*persons_wnd *persons_w,*/ QWidget *parent)
    : db(db), db_FIAS(db_FIAS), data_app(data_app), QDialog(parent), settings(settings),/* persons_w(persons_w),*/ ui(new Ui::find_wnd)
{
    ui->setupUi(this);

    ui->tabWidget->setCurrentIndex(0);

    refresh_fias_address(0,0,0,0,0);
    refresh_docs_pan_type();
}

find_wnd::~find_wnd() {
    delete ui;
}

void find_wnd::refresh_docs_pan_type() {
    this->setCursor(Qt::WaitCursor);
    // обновление выпадающего списка типов документов
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select code, '('||code||') '||text, mask_ser, mask_num "
                  " from public.spr_f011 s "
                  " order by code ; ";
    mySQL.exec(this, sql, QString("Справочник типов документов"), *query, true, db, data_app);
    ui->combo_doc_type->clear();
    ui->combo_doc_type->addItem(" - выберите тип документа - ", 0);
    while (query->next()) {
        ui->combo_doc_type->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_doc_type->setCurrentIndex(0);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void find_wnd::refresh_fias_address(int subj, int dstr, int city, int nasp, int strt) {
    QApplication::processEvents();

    this->setCursor(Qt::WaitCursor);

    QString regioncode = subj>=0 ? QString("00" + QString::number(subj)).right(2)
                                : QString("00" + QString::number(ui->combo_subj->currentData().toInt())).right(2);
    QString areacode   = dstr>=0 ? QString("000" + QString::number(dstr)).right(3)
                                : QString("000" + QString::number(ui->combo_dstr->currentData().toInt())).right(3);
    QString citycode   = city>=0 ? QString("000" + QString::number(city)).right(3)
                                : QString("000" + QString::number(ui->combo_city->currentData().toInt())).right(3);
    QString placecode  = nasp>=0 ? QString("000" + QString::number(nasp)).right(3)
                                : QString("000" + QString::number(ui->combo_nasp->currentData().toInt())).right(3);
    QString streetcode = strt>=0 ? QString("0000" + QString::number(strt)).right(4)
                                : QString("0000" + QString::number(ui->combo_strt->currentData().toInt())).right(4);

    // регион регистрации
    if (subj>=0) {
        // обновление выпадающего списка регионов
        QPalette pal = ui->lab_subj->palette();
        pal.setColor(QPalette::Text, QColor(0,0,255));
        ui->lab_subj->setPalette(pal);
        QFont font = ui->lab_subj->font();
        font.setBold(true);
        ui->lab_subj->setFont(font);
        QApplication::processEvents();

        ui->combo_subj->clear();
        ui->combo_subj->addItem(" - нет - ", " -//- ");

        QSqlQuery *query = new QSqlQuery(db_FIAS);
        QString sq_ = "select s.regioncode, formalname||' '||shortname as name, RIGHT('00000000000'||TRIM(okato),11) as okato, code, trim(postalcode) as post_index, aoguid, aoid "
                      "  from public.addrobj s "
                      " where s.aolevel=1 "
                      "   and currstatus=0 "
                      " order by s.formalname ; ";
        query->exec( sq_ );
        while (query->next()) {
            ui->combo_subj->addItem(query->value(1).toString(), query->value(0).toString());
        }
        delete query;
        // зададим текущее значение
        if (subj>0) ui->combo_subj->setCurrentIndex(ui->combo_subj->findData(regioncode));
        else        ui->combo_subj->setCurrentIndex(0);

        pal = ui->lab_subj->palette();
        pal.setColor(QPalette::Text, QColor(0,0,0));
        ui->lab_subj->setPalette(pal);
        font = ui->lab_subj->font();
        font.setBold(false);
        ui->lab_subj->setFont(font);
        QApplication::processEvents();
    }
    if (ui->combo_subj->currentIndex()<0)
        ui->combo_subj->setCurrentIndex(0);

    //район регистрации
    if (dstr>=0) {
        // обновление выпадающего списка районов
        QPalette pal = ui->lab_dstr->palette();
        pal.setColor(QPalette::Text, QColor(0,0,255));
        ui->lab_dstr->setPalette(pal);
        QFont font = ui->lab_dstr->font();
        font.setBold(true);
        ui->lab_dstr->setFont(font);
        QApplication::processEvents();

        ui->combo_dstr->clear();
        ui->combo_dstr->addItem(" - нет - ", " -//- ");

        if (subj!=0) {
            QSqlQuery *query = new QSqlQuery(db_FIAS);
            QString sq_ = "select s.areacode, formalname||' '||shortname as name, RIGHT('00000000000'||TRIM(okato),11) as okato, code, trim(postalcode) as post_index, aoguid, aoid "
                          "  from public.addrobj s "
                          " where s.regioncode='" + regioncode + "' "
                          "   and s.aolevel=3 "
                          "   and currstatus=0 "
                          " order by s.formalname ; ";
            query->exec( sq_ );
            while (query->next()) {
                ui->combo_dstr->addItem(query->value(1).toString(), query->value(0).toString());
            }
            delete query;
        }
        // зададим текущее значение
        if (dstr>0) ui->combo_dstr->setCurrentIndex(ui->combo_dstr->findData(areacode));
        else        ui->combo_dstr->setCurrentIndex(0);

        pal = ui->lab_dstr->palette();
        pal.setColor(QPalette::Text, QColor(0,0,0));
        ui->lab_dstr->setPalette(pal);
        font = ui->lab_dstr->font();
        font.setBold(false);
        ui->lab_dstr->setFont(font);
        QApplication::processEvents();
    }
    if (ui->combo_dstr->currentIndex()<0)
        ui->combo_dstr->setCurrentIndex(0);

    // город регистрации
    if (city>=0) {
        QPalette pal = ui->lab_city->palette();
        pal.setColor(QPalette::Text, QColor(0,0,255));
        ui->lab_city->setPalette(pal);
        QFont font = ui->lab_city->font();
        font.setBold(true);
        ui->lab_city->setFont(font);
        QApplication::processEvents();

        ui->combo_city->clear();
        ui->combo_city->addItem(" - нет - ", " -//- ");

        if (subj!=0) {
            // обновление выпадающего списка городов
            QSqlQuery *query = new QSqlQuery(db_FIAS);
            QString sq_ = "select s.citycode, formalname||' '||shortname as name, RIGHT('00000000000'||TRIM(okato),11) as okato, code, trim(postalcode) as post_index, aoguid, aoid "
                          "  from public.addrobj s "
                          " where s.regioncode='" + regioncode + "' "
                          "   and s.areacode='"   + areacode   + "' "
                          "   and s.aolevel=4 "
                          "   and currstatus=0 "
                          " order by s.formalname ; ";
            query->exec( sq_ );
            while (query->next()) {
                ui->combo_city->addItem(query->value(1).toString(), query->value(0).toString());
            }
            delete query;
        }
        // зададим текущее значение
        if (city>0) ui->combo_city->setCurrentIndex(ui->combo_city->findData(citycode));
        else        ui->combo_city->setCurrentIndex(0);

        pal = ui->lab_city->palette();
        pal.setColor(QPalette::Text, QColor(0,0,0));
        ui->lab_city->setPalette(pal);
        font = ui->lab_city->font();
        font.setBold(false);
        ui->lab_city->setFont(font);
        QApplication::processEvents();
    }
    if (ui->combo_city->currentIndex()<0)
        ui->combo_city->setCurrentIndex(0);

    // нас.пункт регистрации
    if (nasp>=0) {
        QPalette pal = ui->lab_nasp->palette();
        pal.setColor(QPalette::Text, QColor(0,0,255));
        ui->lab_nasp->setPalette(pal);
        QFont font = ui->lab_nasp->font();
        font.setBold(true);
        ui->lab_nasp->setFont(font);
        QApplication::processEvents();

        ui->combo_nasp->clear();
        ui->combo_nasp->addItem(" - нет - ", " -//- ");

        if (subj!=0) {
            // обновление выпадающего списка нас.пунктов
            QString sq_ = "select s.placecode, formalname||' '||shortname as name, RIGHT('00000000000'||TRIM(okato),11) as okato, code, trim(postalcode) as post_index, aoguid, aoid "
                          "  from public.addrobj s "
                          " where s.regioncode='" + regioncode + "' "
                          "   and s.areacode='"   + areacode   + "' "
                          "   and s.citycode='"   + citycode   + "' "
                          "   and s.aolevel=6 "
                          "   and currstatus=0 "
                          " order by s.formalname ; ";
            QSqlQuery *query = new QSqlQuery(db_FIAS);
            query->exec( sq_ );
            while (query->next()) {
                ui->combo_nasp->addItem(query->value(1).toString(), query->value(0).toString());
            }
            delete query;
        }
        // зададим текущее значение
        if (nasp>0) ui->combo_nasp->setCurrentIndex(ui->combo_nasp->findData(placecode));
        else        ui->combo_nasp->setCurrentIndex(0);

        pal = ui->lab_nasp->palette();
        pal.setColor(QPalette::Text, QColor(0,0,0));
        ui->lab_nasp->setPalette(pal);
        font = ui->lab_nasp->font();
        font.setBold(false);
        ui->lab_nasp->setFont(font);
        QApplication::processEvents();
    }
    if (ui->combo_nasp->currentIndex()<0)
        ui->combo_nasp->setCurrentIndex(0);

    // улица регистрации
    if (strt>=0) {
        QPalette pal = ui->lab_strt->palette();
        pal.setColor(QPalette::Text, QColor(0,0,255));
        ui->lab_strt->setPalette(pal);
        QFont font = ui->lab_strt->font();
        font.setBold(true);
        ui->lab_strt->setFont(font);
        QApplication::processEvents();

        ui->combo_strt->clear();
        ui->combo_strt->addItem(" - нет - ", " -//- ");

        if (subj!=0) {
            // обновление выпадающего списка улиц
            QSqlQuery *query = new QSqlQuery(db_FIAS);
            QString sq_ = "select s.streetcode, formalname||' '||shortname as name, RIGHT('00000000000'||TRIM(okato),11) as okato, code, trim(postalcode) as post_index, aoguid, aoid "
                          "  from public.addrobj s "
                          " where s.regioncode='" + regioncode + "' "
                          "   and s.areacode='"   + areacode   + "' "
                          "   and s.citycode='"   + citycode   + "' "
                          "   and s.placecode='"  + placecode  + "' "
                          "   and s.aolevel=7 "
                          "   and currstatus=0 "
                          " order by s.formalname ; ";
            query->exec( sq_ );
            while (query->next()) {
                ui->combo_strt->addItem(query->value(1).toString(), query->value(0).toString());
            }
            delete query;
        }
        // зададим текущее значение
        int i = ui->combo_strt->findData(streetcode,Qt::UserRole,Qt::MatchExactly);
        //int i = ui->combo_strt->findData(strt,Qt::UserRole,Qt::MatchExactly);
        QString str = ui->combo_strt->itemText(i);
        if (strt>0) ui->combo_strt->setCurrentIndex(i);
        else        ui->combo_strt->setCurrentIndex(0);

        pal = ui->lab_strt->palette();
        pal.setColor(QPalette::Text, QColor(0,0,0));
        ui->lab_strt->setPalette(pal);
        font = ui->lab_strt->font();
        font.setBold(false);
        ui->lab_strt->setFont(font);
        QApplication::processEvents();
    }
    if (ui->combo_strt->currentIndex()<0)
        ui->combo_strt->setCurrentIndex(0);

    this->setCursor(Qt::ArrowCursor);

    QApplication::processEvents();
}

void find_wnd::on_combo_subj_activated(int index) {
    refresh_fias_address(-1,0,0,0,0);
}
void find_wnd::on_combo_dstr_activated(int index) {
    refresh_fias_address(-1,-1,0,0,0);
}
void find_wnd::on_combo_city_activated(int index) {
    refresh_fias_address(-1,-1,-1,0,0);
}
void find_wnd::on_combo_nasp_activated(int index) {
    refresh_fias_address(-1,-1,-1,-1,0);
}
void find_wnd::on_combo_strt_activated(int index) {
    refresh_fias_address(-1,-1,-1,-1,-1);
}

void find_wnd::on_bn_close_clicked() {
    reject();
}

void find_wnd::on_tabWidget_currentChanged(int index) {
    QString capt = "Поиск\n";
    switch (ui->tabWidget->currentIndex()) {
    case 0:
        capt += "по ВС / полису";
        break;
    case 1:
        capt += "по адресу";
        break;
    case 2:
        capt += "по документу";
        break;
    default:
        capt += "???";
        break;
    }
}

void find_wnd::on_bn_find_clicked() {
    // сформируем пул выбранных записей
    switch (ui->tabWidget->currentIndex()) {
    case 0:

        break;
    default:
        break;
    }
    // обновим таблицу персон
    ((persons_wnd*)parent())->refresh_persons_tab();
}

void find_wnd::on_line_vs_num_editingFinished() {
    ui->ch_vs_num->setChecked(!ui->line_vs_num->text().trimmed().isEmpty());
}
void find_wnd::on_line_pol_ser_editingFinished() {
    ui->ch_polis->setChecked(!ui->line_pol_ser->text().trimmed().isEmpty() || !ui->line_pol_num->text().trimmed().isEmpty());
}
void find_wnd::on_line_enp_editingFinished() {
    ui->ch_enp->setChecked(!ui->line_enp->text().trimmed().isEmpty());
}
void find_wnd::on_combo_doc_type_activated(const QString &arg1) {
    ui->ch_doc_type->setChecked(true);
}
void find_wnd::on_line_doc_ser_editingFinished() {
    ui->ch_doc_ser->setChecked(!ui->line_doc_ser->text().trimmed().isEmpty());
}
void find_wnd::on_line_doc_num_editingFinished() {
    ui->ch_doc_num->setChecked(!ui->line_doc_num->text().trimmed().isEmpty());
}
void find_wnd::on_date_doc_start_editingFinished() {
    ui->ch_doc_start->setChecked(true);
}
void find_wnd::on_date_doc_exp_editingFinished() {
    ui->ch_doc_exp->setChecked(true);
}
void find_wnd::on_ch_doc_is_act_clicked() {
    ui->ch_doc_status->setChecked(true);
}
