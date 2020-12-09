#include "settings_wnd.h"
#include "ui_settings_wnd.h"


settings_wnd::settings_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent)
 : db(db), data_app(data_app), settings(settings), QDialog(parent), ui(new Ui::settings_wnd) {
    ui->setupUi(this);

    refresh_folders();

    ui->lab_ini->setText(data_app.ini_path);

    // отображения данных
    ui->ln_host->setText(data_app.host);
    ui->spin_port->setValue(data_app.port);
    ui->ln_base->setText(data_app.base);
    ui->ln_user->setText(data_app.user);
    ui->ln_pass->setText(data_app.pass);

    ui->rb_tech->setChecked(data_app.is_tech);
    ui->rb_head->setChecked(data_app.is_head);
    ui->rb_locl->setChecked(data_app.is_locl);
    ui->rb_call->setChecked(data_app.is_call);
    ui->ch_folder->setChecked(data_app.use_folders);

    ui->rb_xmls->setChecked(data_app.f_send2fond_xmls);
    ui->rb_zips->setChecked(data_app.f_send2fond_zips);
    ui->rb_1ZIP->setChecked(data_app.f_send2fond_1ZIP);
    ui->ch_new_IM->setChecked(data_app.f_send2fond_new_IM);
    ui->ch_new_OT->setChecked(data_app.f_send2fond_new_OT);
    ui->ch_new_assigs->setChecked(data_app.f_send2fond_new_assigs);

    ui->ch_gen_regVS->setChecked(data_app.f_gen_regVS);
    ui->ch_gen_regPol->setChecked(data_app.f_gen_regPol);

    ui->ln_path_install->setText(data_app.path_install);
    ui->ln_path_arch->setText(data_app.path_arch);
    ui->ln_path_reports->setText(data_app.path_reports);
    ui->ln_path_temp->setText(data_app.path_temp);
    ui->ln_path_photo->setText(data_app.path_foto);
    ui->ln_path_sign->setText(data_app.path_sign);
    ui->ln_path_in->setText(data_app.path_in);
    ui->ln_path_out->setText(data_app.path_out);
    ui->ln_path_dbf->setText(data_app.path_dbf);
    ui->ln_path_acts->setText(data_app.path_acts);
    ui->ln_path_to_TFOMS->setText(data_app.path_to_TFOMS);
    ui->ln_path_from_TFOMS->setText(data_app.path_from_TFOMS);
    //ui->ln_path_in_backup->setText(data_app.path_in_backup);
    //ui->ln_path_out_backup->setText(data_app.path_out_backup);

    refresh_ocato();
    ui->combo_ocato->setCurrentIndex(ui->combo_ocato->findData(data_app.ocato.toInt()));

    ui->ln_ogrn->setText(data_app.smo_ogrn);
    ui->ln_smo_short->setText(data_app.smo_short);
    ui->ln_smo_name->setText(data_app.smo_name);
    ui->ln_smo_name_full->setText(data_app.smo_name_full);
    ui->ln_smo_name_full_rp->setText(data_app.smo_name_full_rp);
    ui->ln_smo_director->setText(data_app.smo_director);
    ui->ln_smo_oms_chif->setText(data_app.smo_oms_chif);

    ui->ln_tfoms_director->setText(data_app.tfoms_director);
    ui->ln_tfoms_director_2->setText(data_app.tfoms_director_2);
    ui->ln_tfoms_director_fio->setText(data_app.tfoms_director_fio);
    ui->ln_tfoms_chif->setText(data_app.tfoms_chif);
    ui->ln_terr_name->setText(data_app.terr_name);
    ui->ln_terr_name_rp->setText(data_app.terr_name_rp);
    ui->ln_filial_city->setText(data_app.filial_city);
    ui->ln_filial_name->setText(data_app.filial_name);
    ui->ln_filial_name_rp->setText(data_app.filial_name_rp);
    ui->ln_filial_by_city->setText(data_app.filial_by_city);
    ui->ln_filial_by_city_tp->setText(data_app.filial_by_city_tp);
    ui->ln_filial_director->setText(data_app.filial_director);
    ui->ln_filial_director_2->setText(data_app.filial_director_2);
    ui->ln_filial_director_fio->setText(data_app.filial_director_fio);
    ui->ln_filial_chif->setText(data_app.filial_chif);
    ui->ln_smo_regnum->setText(data_app.smo_regnum);
    ui->ln_smo_address->setText(data_app.smo_address);
    ui->ln_smo_contact_info->setText(data_app.smo_contact_info);
    ui->ln_rep_doer->setText(data_app.rep_doer);
    ui->ln_filial_phone->setText(data_app.filial_phone);
    ui->ln_filial_phone_2->setText(data_app.filial_phone_2);
    ui->ln_filial_email->setText(data_app.filial_email);
    ui->ln_filial_email_2->setText(data_app.filial_email_2);

    ui->ln_point_regnum->setText(data_app.point_regnum);
    ui->ln_point_name->setText(data_app.point_name);
    ui->ln_point_signer->setText(data_app.point_signer);

    ui->ln_smo_logo1->setText(data_app.smo_logo1);
    ui->ln_smo_logo2->setText(data_app.smo_logo2);
    ui->ln_smo_logo3->setText(data_app.smo_logo3);

    ui->combo_spin_format->setCurrentIndex(data_app.snils_format);
    ui->combo_xml_scheme->setCurrentIndex(ui->combo_xml_scheme->findText(data_app.xml_vers));
    ui->combo_split_by_ot->setCurrentIndex(data_app.split_by_ot);

    ui->ln_filials_list->setText(data_app.filials_list);

    // филиалы
    ui->ln_belg_host->setText(data_app.belg_host);
    ui->spin_belg_port->setValue(data_app.belg_port);
    ui->ln_belg_base->setText(data_app.belg_base);
    ui->ln_belg_user->setText(data_app.belg_user);
    ui->ln_belg_pass->setText(data_app.belg_pass);

    ui->ln_kursk_host->setText(data_app.kursk_host);
    ui->spin_kursk_port->setValue(data_app.kursk_port);
    ui->ln_kursk_base->setText(data_app.kursk_base);
    ui->ln_kursk_user->setText(data_app.kursk_user);
    ui->ln_kursk_pass->setText(data_app.kursk_pass);

    ui->ln_voro_host->setText(data_app.voro_host);
    ui->spin_voro_port->setValue(data_app.voro_port);
    ui->ln_voro_base->setText(data_app.voro_base);
    ui->ln_voro_user->setText(data_app.voro_user);
    ui->ln_voro_pass->setText(data_app.voro_pass);

    ui->ln_test_host->setText(data_app.test_host);
    ui->spin_test_port->setValue(data_app.test_port);
    ui->ln_test_base->setText(data_app.test_base);
    ui->ln_test_user->setText(data_app.test_user);
    ui->ln_test_pass->setText(data_app.test_pass);

    ui->ln_work_host->setText(data_app.work_host);
    ui->spin_work_port->setValue(data_app.work_port);
    ui->ln_work_base->setText(data_app.work_base);
    ui->ln_work_user->setText(data_app.work_user);
    ui->ln_work_pass->setText(data_app.work_pass);
}

settings_wnd::~settings_wnd() {
    delete ui;
}

void settings_wnd::refresh_folders() {
    this->setCursor(Qt::WaitCursor);
    // обновление списка папок
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "select id, folder_name "
                  " from public.folders "
                  " where date_open<='" + QDate::currentDate().toString("yyyy-MM-dd") + "' "
                  "   and status>=0 "
                  " order by folder_name ; ";
    mySQL.exec(this, sql, QString("Список папок"), *query, true, db, data_app);
    ui->combo_folder->clear();
    ui->combo_folder->addItem(" - без папки - ", 0);
    while (query->next()) {
        ui->combo_folder->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    delete query;
    int n = 0;
    ui->combo_folder->setCurrentIndex(n);
    this->setCursor(Qt::ArrowCursor);
}

void settings_wnd::on_bn_cansel_clicked() {
    reject();
}

void settings_wnd::refresh_ocato() {
    this->setCursor(Qt::WaitCursor);
    // обновление списка форм изготовления полиса
    QString sql = "select code_ocato, subj_name "
                  " from public.spr_ocato s "
                  " order by subj_name ; ";
    QSqlQuery *query = new QSqlQuery(db);
    mySQL.exec(this, sql, QString("Обновление справочника ОКАТО регионов"), *query, true, db, data_app);
    ui->combo_ocato->clear();
    while (query->next()) {
        ui->combo_ocato->addItem(query->value(1).toString(), query->value(0).toInt());
    }
    ui->combo_ocato->setCurrentIndex(0);
    delete query;
    this->setCursor(Qt::ArrowCursor);
}

void settings_wnd::on_combo_ocato_currentIndexChanged(int index) {
    ui->spin_ocato->setValue(ui->combo_ocato->currentData().toInt());
}

void settings_wnd::on_bn_save_clicked() {
    if (QMessageBox::question(this, "Нужно подтверждение", "Сохранить все сделанные изменеия в INI-файл?", QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel)==QMessageBox::Cancel)
        return;

    // сохраним данные в settings
    data_app.host = ui->ln_host->text();
    data_app.port = ui->spin_port->value();
    data_app.base = ui->ln_base->text();
    data_app.user = ui->ln_user->text();
    data_app.pass = ui->ln_pass->text();

    data_app.is_tech = ui->rb_tech->isChecked();
    data_app.is_head = ui->rb_head->isChecked();
    data_app.is_locl = ui->rb_locl->isChecked();
    data_app.is_call = ui->rb_call->isChecked();
    data_app.use_folders = ui->ch_folder->isChecked();
    data_app.default_folder = ui->combo_folder->currentText();

    data_app.f_send2fond_xmls = ui->rb_xmls->isChecked();
    data_app.f_send2fond_zips = ui->rb_zips->isChecked();
    data_app.f_send2fond_1ZIP = ui->rb_1ZIP->isChecked();
    data_app.f_send2fond_new_IM = ui->ch_new_IM->isChecked();
    data_app.f_send2fond_new_OT = ui->ch_new_OT->isChecked();
    data_app.f_send2fond_new_assigs = ui->ch_new_assigs->isChecked();

    data_app.f_gen_regVS = ui->ch_gen_regVS->isChecked();
    data_app.f_gen_regPol = ui->ch_gen_regPol->isChecked();

    data_app.path_install = ui->ln_path_install->text();
    data_app.path_arch = ui->ln_path_arch->text();
    data_app.path_reports = ui->ln_path_reports->text();
    data_app.path_temp = ui->ln_path_temp->text();
    data_app.path_foto = ui->ln_path_photo->text();
    data_app.path_sign = ui->ln_path_sign->text();
    data_app.path_in = ui->ln_path_in->text();
    data_app.path_out = ui->ln_path_out->text();
    data_app.path_dbf = ui->ln_path_dbf->text();
    data_app.path_acts = ui->ln_path_acts->text();
    data_app.path_to_TFOMS = ui->ln_path_to_TFOMS->text();
    data_app.path_from_TFOMS = ui->ln_path_from_TFOMS->text();
    //data_app.path_in_backup = ui->ln_path_in_backup->text();
    //data_app.path_out_backup = ui->ln_path_out_backup->text();

    data_app.ocato = QString::number(ui->combo_ocato->currentData().toInt());
    data_app.ocato_text = ui->combo_ocato->currentText();

    data_app.smo_ogrn = ui->ln_ogrn->text();
    data_app.smo_short = ui->ln_smo_short->text();
    data_app.smo_name = ui->ln_smo_name->text();
    data_app.smo_name_full = ui->ln_smo_name_full->text();
    data_app.smo_name_full_rp = ui->ln_smo_name_full_rp->text();
    data_app.smo_director = ui->ln_smo_director->text();
    data_app.smo_oms_chif = ui->ln_smo_oms_chif->text();

    data_app.tfoms_director = ui->ln_tfoms_director->text();
    data_app.tfoms_director_2 = ui->ln_tfoms_director_2->text();
    data_app.tfoms_director_fio = ui->ln_tfoms_director_fio->text();
    data_app.tfoms_chif = ui->ln_tfoms_chif->text();

    data_app.terr_name = ui->ln_terr_name->text();
    data_app.terr_name_rp = ui->ln_terr_name_rp->text();
    data_app.filial_city = ui->ln_filial_city->text();
    data_app.filial_name = ui->ln_filial_name->text();
    data_app.filial_name_rp = ui->ln_filial_name_rp->text();
    data_app.filial_by_city = ui->ln_filial_by_city->text();
    data_app.filial_by_city_tp = ui->ln_filial_by_city_tp->text();
    data_app.filial_director = ui->ln_filial_director->text();
    data_app.filial_director_2 = ui->ln_filial_director_2->text();
    data_app.filial_director_fio = ui->ln_filial_director_fio->text();
    data_app.filial_chif = ui->ln_filial_chif->text();
    data_app.smo_regnum = ui->ln_smo_regnum->text();
    data_app.smo_address = ui->ln_smo_address->text();
    data_app.smo_contact_info = ui->ln_smo_contact_info->text();
    data_app.rep_doer = ui->ln_rep_doer->text();
    data_app.filial_phone = ui->ln_filial_phone->text();
    data_app.filial_phone_2 = ui->ln_filial_phone_2->text();
    data_app.filial_email = ui->ln_filial_email->text();
    data_app.filial_email_2 = ui->ln_filial_email_2->text();

    data_app.point_regnum = ui->ln_point_regnum->text();
    data_app.point_name = ui->ln_point_name->text();
    data_app.point_signer = ui->ln_point_signer->text();

    data_app.smo_logo1 = ui->ln_smo_logo1->text();
    data_app.smo_logo2 = ui->ln_smo_logo2->text();
    data_app.smo_logo3 = ui->ln_smo_logo3->text();

    data_app.snils_format = ui->combo_spin_format->currentIndex();
    data_app.xml_vers = ui->combo_xml_scheme->currentText();
    data_app.split_by_ot = ui->combo_split_by_ot->currentIndex();
    data_app.f_zip_packs = ui->ch_zip_packs->isChecked();

    data_app.filials_list = ui->ln_filials_list->text();

    // филиалы
    data_app.belg_host = ui->ln_belg_host->text();
    data_app.belg_port = ui->spin_belg_port->value();
    data_app.belg_base = ui->ln_belg_base->text();
    data_app.belg_user = ui->ln_belg_user->text();
    data_app.belg_pass = ui->ln_belg_pass->text();

    data_app.kursk_host = ui->ln_kursk_host->text();
    data_app.kursk_port = ui->spin_kursk_port->value();
    data_app.kursk_base = ui->ln_kursk_base->text();
    data_app.kursk_user = ui->ln_kursk_user->text();
    data_app.kursk_pass = ui->ln_kursk_pass->text();

    data_app.voro_host = ui->ln_voro_host->text();
    data_app.voro_port = ui->spin_voro_port->value();
    data_app.voro_base = ui->ln_voro_base->text();
    data_app.voro_user = ui->ln_voro_user->text();
    data_app.voro_pass = ui->ln_voro_pass->text();

    data_app.test_host = ui->ln_test_host->text();
    data_app.test_port = ui->spin_test_port->value();
    data_app.test_base = ui->ln_test_base->text();
    data_app.test_user = ui->ln_test_user->text();
    data_app.test_pass = ui->ln_test_pass->text();

    data_app.work_host = ui->ln_work_host->text();
    data_app.work_port = ui->spin_work_port->value();
    data_app.work_base = ui->ln_work_base->text();
    data_app.work_user = ui->ln_work_user->text();
    data_app.work_pass = ui->ln_work_pass->text();

    //сохраним данные в ini-файл
    // [polisesDB]
    settings.beginGroup("polisesDB");
    settings.setValue("host", data_app.host);
    settings.setValue("port", data_app.port);
    settings.setValue("base", data_app.base);
    settings.setValue("user", data_app.user);
    settings.setValue("pass", data_app.pass);
    settings.endGroup();
    // [scoresDB]
    settings.beginGroup("scoresDB");
    settings.setValue("scores_host", data_app.scores_host);
    settings.setValue("scores_port", data_app.scores_port);
    settings.setValue("scores_base", data_app.scores_base);
    settings.setValue("scores_user", data_app.scores_user);
    settings.setValue("scores_pass", data_app.scores_pass);
    settings.endGroup();
    // [flags]
    settings.beginGroup("flags");
    settings.setValue("is_tech", data_app.is_tech);
    settings.setValue("is_head", data_app.is_head);
    settings.setValue("is_locl", data_app.is_locl);
    settings.setValue("gen_regVS", data_app.f_gen_regVS);
    settings.setValue("gen_regPol", data_app.f_gen_regPol);
    settings.setValue("send_to_fond_xmls", data_app.f_send2fond_xmls);
    settings.setValue("send_to_fond_zips", data_app.f_send2fond_zips);
    settings.setValue("send_to_fond_1ZIP", data_app.f_send2fond_1ZIP);
    settings.setValue("send_to_fond_new_IM", data_app.f_send2fond_new_IM);
    settings.setValue("send_to_fond_new_OT", data_app.f_send2fond_new_OT);
    settings.setValue("send_to_fond_new_assigs", data_app.f_send2fond_new_assigs);
    settings.setValue("use_folders", data_app.use_folders);
    settings.endGroup();
    // [paths]
    settings.beginGroup("paths");
    settings.setValue("path_install", data_app.path_install.toUtf8());
    settings.setValue("path_arch", data_app.path_arch.toUtf8());
    settings.setValue("path_reports", data_app.path_reports.toUtf8());
    settings.setValue("path_temp", data_app.path_temp.toUtf8());
    settings.setValue("path_foto", data_app.path_foto.toUtf8());
    settings.setValue("path_sign", data_app.path_sign.toUtf8());
    settings.setValue("path_in", data_app.path_in.toUtf8());
    settings.setValue("path_out", data_app.path_out.toUtf8());
    settings.setValue("path_dbf", data_app.path_dbf.toUtf8());
    settings.setValue("path_acts", data_app.path_acts.toUtf8());
    settings.setValue("path_to_TFOMS", data_app.path_to_TFOMS.toUtf8());
    settings.setValue("path_from_TFOMS", data_app.path_from_TFOMS.toUtf8());
    //settings.setValue("path_in_backup", data_app.path_in_backup.toUtf8());
    //settings.setValue("path_out_backup", data_app.path_out_backup.toUtf8());
    settings.endGroup();
    // [smo]
    settings.beginGroup("smo");
    settings.setValue("terr_ocato", data_app.ocato);
    settings.setValue("terr_ocato_text", data_app.ocato_text.toUtf8());
    settings.setValue("smo_ogrn", data_app.smo_ogrn);
    settings.setValue("smo_regnum", data_app.smo_regnum);
    settings.setValue("smo_logo1", data_app.smo_logo1.toUtf8());
    settings.setValue("smo_logo2", data_app.smo_logo2.toUtf8());
    settings.setValue("smo_logo3", data_app.smo_logo3.toUtf8());
    settings.setValue("smo_short", data_app.smo_short.toUtf8());
    settings.setValue("smo_name", data_app.smo_name.toUtf8());
    settings.setValue("smo_name_full", data_app.smo_name_full.toUtf8());
    settings.setValue("smo_director", data_app.smo_director.toUtf8());
    settings.setValue("smo_oms_chif", data_app.smo_oms_chif.toUtf8());
    settings.setValue("smo_name_full_rp", data_app.smo_name_full_rp.toUtf8());
    settings.setValue("smo_address", data_app.smo_address.toUtf8());
    settings.setValue("smo_contact_info", data_app.smo_contact_info.toUtf8());
    settings.setValue("point_regnum", data_app.point_regnum);
    settings.setValue("point_name", data_app.point_name.toUtf8());
    settings.setValue("point_signer", data_app.point_signer.toUtf8());
    settings.endGroup();
    // [smo]
    settings.beginGroup("smo");
    settings.setValue("msg_timeout_back",  data_app.msg_timeout_back);
    settings.setValue("msg_timeout_front", data_app.msg_timeout_front);
    settings.endGroup();
    // [filial]
    settings.beginGroup("filial");
    settings.setValue("terr_name",           data_app.terr_name);
    settings.setValue("terr_name_rp",        data_app.terr_name_rp);
    settings.setValue("tfoms_director",      data_app.tfoms_director);
    settings.setValue("tfoms_director_2",    data_app.tfoms_director_2);
    settings.setValue("tfoms_director_fio",  data_app.tfoms_director_fio);
    settings.setValue("tfoms_chif",          data_app.tfoms_chif);
    settings.setValue("filial_name",         data_app.filial_name);
    settings.setValue("filial_name_rp",      data_app.filial_name_rp);
    settings.setValue("filial_city",         data_app.filial_city);
    settings.setValue("filial_by_city",      data_app.filial_by_city);
    settings.setValue("filial_by_city_tp",   data_app.filial_by_city_tp);
    settings.setValue("filial_director",     data_app.filial_director);
    settings.setValue("filial_director_2",   data_app.filial_director_2);
    settings.setValue("filial_director_fio", data_app.filial_director_fio);
    settings.setValue("filial_chif",         data_app.filial_chif);
    settings.setValue("rep_doer",            data_app.rep_doer);
    settings.setValue("filial_phone",        data_app.filial_phone);
    settings.setValue("filial_phone_2",      data_app.filial_phone_2);
    settings.setValue("filial_email",        data_app.filial_email);
    settings.setValue("filial_email_2",      data_app.filial_email_2);
    settings.endGroup();
    // [assig]
    settings.beginGroup("assig");
    settings.setValue("assig_vers", data_app.assig_vers);
    settings.setValue("csv_headers", data_app.assig_csv_headers);
    settings.endGroup();
    // [log]
    settings.beginGroup("log");
    settings.setValue("log_bord", data_app.log_bord);
    settings.endGroup();
    // [xml]
    settings.beginGroup("xml");
    settings.setValue("xml_vers", data_app.xml_vers);
    settings.setValue("snils_format", QString::number(data_app.snils_format));
    settings.setValue("split_by_ot", QString::number(data_app.split_by_ot));
    settings.setValue("send_p02_in_p06", QString::number(data_app.send_p02_in_p06));
    settings.setValue("pol_start_date_by_vs", QString::number(data_app.pol_start_date_by_vs));
    settings.setValue("zip_packs", data_app.f_zip_packs);
    settings.endGroup();
    // [VS]
    settings.beginGroup("VS");
    //settings.setValue("inko_name", data_app.vs_inko_name);
    //settings.setValue("inko_address", data_app.vs_inko_address);
    settings.setValue("blank_width_mm", QString::number(data_app.vs_blank_width_mm));
    settings.setValue("blank_height_mm", QString::number(data_app.vs_blank_height_mm));
    settings.setValue("field_left_mm", QString::number(data_app.vs_field_left_mm));
    settings.setValue("field_right_mm", QString::number(data_app.vs_field_right_mm));
    settings.setValue("field_top_mm", QString::number(data_app.vs_field_top_mm));
    settings.setValue("field_bottom_mm", QString::number(data_app.vs_field_bottom_mm));
    settings.endGroup();
    // [barcode]
    settings.beginGroup("barcode");
    settings.setValue("portname", data_app.barcodes_portname);
    settings.setValue("file", data_app.barcodes_file);
    settings.endGroup();
    // [req_ch_smo]
    //settings.beginGroup("req_ch_smo");
    //settings.setValue("req_smo_ogrn", data_app.req_smo_ogrn);
    //settings.endGroup();
    // [notice_pol]
    //settings.beginGroup("notice_pol");
    //settings.endGroup();
    // [filials]
    settings.beginGroup("filials");
    settings.setValue("filials_list", data_app.filials_list);
    settings.endGroup();
    if (data_app.filials_list.indexOf("kursk")>=0) {
        // [kursk]
        settings.beginGroup("kursk");
        settings.setValue("host", data_app.kursk_host);
        settings.setValue("port", QString::number(data_app.kursk_port));
        settings.setValue("base", data_app.kursk_base);
        settings.setValue("user", data_app.kursk_user);
        settings.setValue("pass", data_app.kursk_pass);
        settings.endGroup();
    }
    if (data_app.filials_list.indexOf("belg")>=0) {
        // [belg]
        settings.beginGroup("belg");
        settings.setValue("host", data_app.belg_host);
        settings.setValue("port", QString::number(data_app.belg_port));
        settings.setValue("base", data_app.belg_base);
        settings.setValue("user", data_app.belg_user);
        settings.setValue("pass", data_app.belg_pass);
        settings.endGroup();
    }
    if (data_app.filials_list.indexOf("voro")>=0) {
        // [voro]
        settings.beginGroup("voro");
        settings.setValue("host", data_app.voro_host);
        settings.setValue("port", QString::number(data_app.voro_port));
        settings.setValue("base", data_app.voro_base);
        settings.setValue("user", data_app.voro_user);
        settings.setValue("pass", data_app.voro_pass);
        settings.endGroup();
    }
    if (data_app.filials_list.indexOf("test")>=0) {
        // [test]
        settings.beginGroup("test");
        settings.setValue("host", data_app.test_host);
        settings.setValue("port", QString::number(data_app.test_port));
        settings.setValue("base", data_app.test_base);
        settings.setValue("user", data_app.test_user);
        settings.setValue("pass", data_app.test_pass);
        settings.endGroup();
    }
    if (data_app.filials_list.indexOf("work")>=0) {
        // [work]
        settings.beginGroup("work");
        settings.setValue("host", data_app.work_host);
        settings.setValue("port", QString::number(data_app.work_port));
        settings.setValue("base", data_app.work_base);
        settings.setValue("user", data_app.work_user);
        settings.setValue("pass", data_app.work_pass);
        settings.endGroup();
    }
    settings.sync();


    // для главного оператора - запишем данные филиала в базу данных
    //f (data_app.is_head) {
        QString sql;
        QSqlQuery query(db);

        //QString smo_ogrn;
        sql = "update db_data"
              "   set \"value\"='" + data_app.smo_ogrn + "'  "
              " where name='smo_ogrn' ; ";
        query.exec(sql);
        //QString smo_regnum;
        sql = "update db_data"
              "   set \"value\"='" + data_app.smo_regnum + "'  "
              " where name='smo_regnum' ; ";
        query.exec(sql);
        //QString smo_short;
        sql = "update db_data"
              "   set \"value\"='" + data_app.smo_short + "'  "
              " where name='smo_short' ; ";
        query.exec(sql);
        sql = "update db_data"
              "   set \"value\"='" + data_app.smo_name + "'  "
              " where name='smo_name' ; ";
        query.exec(sql);
        //QString smo_name_full;
        sql = "update db_data"
              "   set \"value\"='" + data_app.smo_name_full + "'  "
              " where name='smo_name_full' ; ";
        query.exec(sql);
        //QString smo_name_full_rp;
        sql = "update db_data"
              "   set \"value\"='" + data_app.smo_name_full_rp + "'  "
              " where name='smo_name_full_rp' ; ";
        query.exec(sql);
        //QString smo_director;
        sql = "update db_data"
              "   set \"value\"='" + data_app.smo_director + "'  "
              " where name='smo_director' ; ";
        query.exec(sql);
        //QString smo_oms_chif;
        sql = "update db_data"
              "   set \"value\"='" + data_app.smo_oms_chif + "'  "
              " where name='smo_oms_chif' ; ";
        query.exec(sql);
        //QString smo_address;
        sql = "update db_data"
              "   set \"value\"='" + data_app.smo_address + "'  "
              " where name='smo_address' ; ";
        query.exec(sql);
        //QString smo_contact_info;
        sql = "update db_data"
              "   set \"value\"='" + data_app.smo_contact_info + "'  "
              " where name='smo_contact_info' ; ";
        query.exec(sql);
        //QString smo_logo1;
        sql = "update db_data"
              "   set \"value\"='" + data_app.smo_logo1 + "'  "
              " where name='smo_logo1' ; ";
        query.exec(sql);
        //QString smo_logo2;
        sql = "update db_data"
              "   set \"value\"='" + data_app.smo_logo2 + "'  "
              " where name='smo_logo2' ; ";
        query.exec(sql);
        //QString smo_logo3;
        sql = "update db_data"
              "   set \"value\"='" + data_app.smo_logo3 + "'  "
              " where name='smo_logo3' ; ";
        query.exec(sql);
        //QString terr_name;
        sql = "update db_data"
              "   set \"value\"='" + data_app.terr_name + "'  "
              " where name='terr_name' ; ";
        query.exec(sql);
        //QString terr_name_rp;
        sql = "update db_data"
              "   set \"value\"='" + data_app.terr_name_rp + "'  "
              " where name='terr_name_rp' ; ";
        query.exec(sql);
        //QString tfoms_director;
        sql = "update db_data"
              "   set \"value\"='" + data_app.tfoms_director + "'  "
              " where name='tfoms_director' ; ";
        query.exec(sql);
        //QString tfoms_director_2;
        sql = "update db_data"
              "   set \"value\"='" + data_app.tfoms_director_2 + "'  "
              " where name='tfoms_director_2' ; ";
        query.exec(sql);
        //QString tfoms_director_fio;
        sql = "update db_data"
              "   set \"value\"='" + data_app.tfoms_director_fio + "'  "
              " where name='tfoms_director_fio' ; ";
        query.exec(sql);
        //QString tfoms_chif;
        sql = "update db_data"
              "   set \"value\"='" + data_app.tfoms_chif + "'  "
              " where name='tfoms_chif' ; ";
        query.exec(sql);
        //QString filial_name;
        sql = "update db_data"
              "   set \"value\"='" + data_app.filial_name + "'  "
              " where name='filial_name' ; ";
        query.exec(sql);
        //QString filial_name_rp;
        sql = "update db_data"
              "   set \"value\"='" + data_app.filial_name_rp + "'  "
              " where name='filial_name_rp' ; ";
        query.exec(sql);
        //QString filial_city;
        sql = "update db_data"
              "   set \"value\"='" + data_app.filial_city + "'  "
              " where name='filial_city' ; ";
        query.exec(sql);
        //QString filial_by_city;
        sql = "update db_data"
              "   set \"value\"='" + data_app.filial_by_city + "'  "
              " where name='filial_by_city' ; ";
        query.exec(sql);
        //QString filial_by_city_tp;
        sql = "update db_data"
              "   set \"value\"='" + data_app.filial_by_city_tp + "'  "
              " where name='filial_by_city_tp' ; ";
        query.exec(sql);
        //QString filial_director;
        sql = "update db_data"
              "   set \"value\"='" + data_app.filial_director + "'  "
              " where name='filial_director' ; ";
        query.exec(sql);
        //QString filial_director_2;
        sql = "update db_data"
              "   set \"value\"='" + data_app.filial_director_2 + "'  "
              " where name='filial_director_2' ; ";
        query.exec(sql);
        //QString filial_director_fio;
        sql = "update db_data"
              "   set \"value\"='" + data_app.filial_director_fio + "'  "
              " where name='filial_director_fio' ; ";
        query.exec(sql);
        //QString filial_chif;
        sql = "update db_data"
              "   set \"value\"='" + data_app.filial_chif + "'  "
              " where name='filial_chif' ; ";
        query.exec(sql);
        //QString filial_phone;
        sql = "update db_data"
              "   set \"value\"='" + data_app.filial_phone + "'  "
              " where name='filial_phone' ; ";
        query.exec(sql);
        //QString filial_phone_2;
        sql = "update db_data"
              "   set \"value\"='" + data_app.filial_phone_2 + "'  "
              " where name='filial_phone_2' ; ";
        query.exec(sql);
        //QString filial_email;
        sql = "update db_data"
              "   set \"value\"='" + data_app.filial_email + "'  "
              " where name='filial_email' ; ";
        query.exec(sql);
        //QString filial_email_2;
        sql = "update db_data"
              "   set \"value\"='" + data_app.filial_email_2 + "'  "
              " where name='filial_email_2' ; ";
        query.exec(sql);
        //QString ocato;
        sql = "update db_data"
              "   set \"value\"='" + data_app.ocato + "'  "
              " where name='terr_ocato' ; ";
        query.exec(sql);
        //QString ocato_text;
        sql = "update db_data"
              "   set \"value\"='" + data_app.ocato_text + "'  "
              " where name='terr_ocato_text' ; ";
        query.exec(sql);
    //}

    close();
}
