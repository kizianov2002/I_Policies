#include "export2csv_wnd.h"
#include "ui_export2csv_wnd.h"
#include <QMessageBox>
#include <QFileDialog>

export2csv_wnd::export2csv_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent)
   : db(db), data_app(data_app), settings(settings), QDialog(parent), ui(new Ui::export2csv_wnd)
{
    ui->setupUi(this);
}

export2csv_wnd::~export2csv_wnd() {
    delete ui;
}

void export2csv_wnd::on_pushButton_clicked() {
    close();
}

void export2csv_wnd::on_ch_pers_fio_clicked() {
    ui->ch_pers_fio->setChecked(true);
}

void export2csv_wnd::on_ch_persons_clicked(bool checked) {
    ui->ch_pers_fio->setChecked(checked);
    ui->ch_pers_country->setChecked(checked);
    ui->ch_pers_adr_liv->setChecked(checked);
    ui->ch_pers_adr_reg->setChecked(checked);
    ui->ch_pers_UDL->setChecked(checked);
    ui->ch_pers_DRP->setChecked(checked);
    ui->ch_pers_photo->setChecked(checked);
    ui->ch_pers_firm->setChecked(checked);
    ui->ch_pers_phone->setChecked(checked);

    if (!checked) {
        ui->ch_reps->setChecked(checked);
        ui->ch_rep_fio->setChecked(checked);
        ui->ch_rep_country->setChecked(checked);
        ui->ch_rep_adr_reg->setChecked(checked);
        ui->ch_rep_adr_liv->setChecked(checked);
        ui->ch_rep_UDL->setChecked(checked);
        ui->ch_rep_DRP->setChecked(checked);
        ui->ch_rep_phone->setChecked(checked);

        ui->ch_assigs->setChecked(checked);
        ui->ch_data_assig->setChecked(checked);

        ui->ch_polises->setChecked(checked);
        ui->ch_pol_type->setChecked(checked);
        ui->ch_pol_sernom->setChecked(checked);
        ui->ch_pol_dates->setChecked(checked);
        ui->ch_pol_blank_VS->setChecked(checked);
        ui->ch_pol_blank_pol->setChecked(checked);

        ui->ch_vizits->setChecked(checked);
        ui->ch_data_vizit->setChecked(checked);

        ui->ch_events->setChecked(checked);
        ui->ch_data_evt->setChecked(checked);
    }
}

void export2csv_wnd::on_ch_reps_clicked(bool checked) {
    ui->ch_reps->setChecked(checked);
    ui->ch_rep_fio->setChecked(checked);
    ui->ch_rep_country->setChecked(checked);
    ui->ch_rep_adr_reg->setChecked(checked);
    ui->ch_rep_adr_liv->setChecked(checked);
    ui->ch_rep_UDL->setChecked(checked);
    ui->ch_rep_DRP->setChecked(checked);
    ui->ch_rep_phone->setChecked(checked);
}

void export2csv_wnd::on_ch_assigs_clicked(bool checked) {
    ui->ch_data_assig->setChecked(checked);
}

void export2csv_wnd::on_ch_polises_clicked(bool checked) {
    ui->ch_pol_type->setChecked(checked);
    ui->ch_pol_sernom->setChecked(checked);
    ui->ch_pol_dates->setChecked(checked);
    ui->ch_pol_blank_VS->setChecked(checked);
    ui->ch_pol_blank_pol->setChecked(checked);

    if (!checked) {
        ui->ch_vizits->setChecked(checked);
        ui->ch_data_vizit->setChecked(checked);

        ui->ch_events->setChecked(checked);
        ui->ch_data_evt->setChecked(checked);
    }
}

void export2csv_wnd::on_ch_vizits_clicked(bool checked) {
    ui->ch_vizits->setChecked(checked);
    ui->ch_data_vizit->setChecked(checked);

    if (checked) {
        ui->ch_events->setChecked(!checked);
        ui->ch_data_evt->setChecked(!checked);
    }
}

void export2csv_wnd::on_ch_events_clicked(bool checked) {
    ui->ch_events->setChecked(checked);
    ui->ch_data_evt->setChecked(checked);

    if (checked) {
        ui->ch_vizits->setChecked(!checked);
        ui->ch_data_vizit->setChecked(!checked);
    }
}

#include "qt_windows.h"
#include "qwindowdefs_win.h"
#include <shellapi.h>

void export2csv_wnd::on_bn_go_clicked() {
    QString fname_s = ui->ln_fname->text();

    // проверка введённых полей
    if (fname_s.isEmpty()) {
        QMessageBox::warning(this,
                             "Не выбрано имя файла",
                             "Имя целевого файла не задано.\n\n"
                             "Операция отменена.");
        return;
    }
    QFile file;
    file.setFileName(fname_s);
    if (file.exists()) {
        if (QMessageBox::question(this,
                                  "Удалить старый файл?",
                                  "Файл уже существует.\n"
                                  "Удалить старый файл для того чтобы сохранить новый?",
                                  QMessageBox::Yes|QMessageBox::Cancel)==QMessageBox::Yes) {
            if (!QFile::remove(fname_s)) {
                QMessageBox::warning(this,
                                     "Ошибка при удалении старого файла",
                                     "При удалении старого файла произошла непредвиденная ошибка.\n\n"
                                     "Операция отменена.");
                return;
            }
        }
    }

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this,
                             "Ошибка создания файла",
                             "При создании целевого файла произошла ошибка.\n\n"
                             "Операция отменена.");
        return;
    }


    // Выберем данные для сохранения
    ui->ch_pers_fio->setChecked(true);

    QString s_encoding = ui->combo_encoding->currentText();
    QString dl = (ui->combo_div_lines->currentIndex()==0 ? "\n\r" : (ui->combo_div_lines->currentIndex()==1 ? "\n" : "\r"));
    QString df = (ui->combo_div_fields->currentIndex()==0 ? ";" : "\t");

    QString tables;
    QStringList fields;
    QStringList formats;
    QStringList orders;
    QStringList headers;

    if (!ui->ch_persons->isChecked()) {
        QMessageBox::warning(this, "Ничего не выбрано", "Ничего не выбрано");
        return;
    }

    tables.append("persons e");
    orders.append("e.fam");
    orders.append("e.im");
    orders.append("e.ot");
    orders.append("e.date_birth");

    fields.append("e.id");
    formats.append("I");
    headers.append("ID");

    // данные персоны
    if (ui->ch_pers_fio->isChecked()) {
        tables += " left join spr_v013 v013 on(v013.code=e.category) ";
        fields.append("e.fam");
        headers.append("фамилия");
        formats.append("S");
        fields.append("e.im");
        headers.append("имя");
        formats.append("S");
        fields.append("e.ot");
        headers.append("отчество");
        formats.append("S");
        fields.append("e.sex");
        headers.append("пол");
        formats.append("I");
        fields.append("e.date_birth");
        headers.append("дат.рожд.");
        formats.append("D");
        fields.append("e.category");
        headers.append("код катег.");
        formats.append("I");
        fields.append("v013.text");
        headers.append("категория застрахованного");
        formats.append("S");
    }
    if (ui->ch_pers_country->isChecked()) {
        tables += " left join spr_oksm oksm_b on(oksm_b.code=e.oksm_birth) "
                  " left join spr_oksm oksm_c on(oksm_c.code=e.oksm_c) ";
        fields.append("oksm_b.short_name as oksm_name_b");
        headers.append("страна происх.");
        formats.append("S");
        fields.append("oksm_b.alfa3 as oksm_alpha_b");
        headers.append("А3 ржд");
        formats.append("S");
        fields.append("oksm_c.short_name as oksm_name_c");
        headers.append("гражданство");
        formats.append("S");
        fields.append("oksm_c.alfa3 as oksm_alpha_c");
        headers.append("А3 гр");
        formats.append("S");
    }
    if (ui->ch_pers_adr_reg->isChecked()) {
        tables += " left join addresses adr_r on(adr_r.id=e.id_addr_reg) ";
        fields.append("adr_r.ocato as ocato_r");
        headers.append("ОКАТО рег.");
        formats.append("S");
        fields.append("adr_r.kladr as kladr_r");
        headers.append("КЛАДР рег.");
        formats.append("S");
    }
    if (ui->ch_pers_adr_liv->isChecked()) {
        tables += " left join addresses adr_l on(adr_l.id=e.id_addr_liv) ";
        fields.append("adr_l.ocato as ocato_l");
        headers.append("ОКАТО прж.");
        formats.append("S");
        fields.append("adr_l.kladr as kladr_l");
        headers.append("КЛАДР прж.");
        formats.append("S");
    }
    if (ui->ch_pers_UDL->isChecked()) {
        tables += " left join persons_docs udl on(udl.id=e.id_udl) "
                  " left join spr_f011 f11 on(f11.code=udl.doc_type) ";
        fields.append("f11.code as udl_code");
        headers.append("код УДЛ");
        formats.append("I");
        fields.append("f11.text as udl_type");
        headers.append("тип УДЛ");
        formats.append("S");
        fields.append("udl.doc_ser");
        headers.append("серия УДЛ");
        formats.append("S");
        fields.append("udl.doc_num");
        headers.append("номер УДЛ");
        formats.append("S");
        fields.append("udl.doc_date");
        headers.append("дата выдачи УДЛ");
        formats.append("D");
        fields.append("udl.doc_exp");
        headers.append("срок действия УДЛ");
        formats.append("D");
        fields.append("udl.org_code");
        headers.append("код орг. УДЛ");
        formats.append("S");
        fields.append("udl.doc_org");
        headers.append("выдающая организация УДЛ");
        formats.append("S");
    }
    if (ui->ch_pers_DRP->isChecked()) {
        tables += " left join persons_docs drp on(drp.id=e.id_drp) "
                  " left join spr_f011 f11p on(f11p.code=drp.doc_type) ";
        fields.append("f11p.code as drp_code");
        headers.append("код ДРП");
        formats.append("I");
        fields.append("f11p.text as drp_type");
        headers.append("тип ДРП");
        formats.append("S");
        fields.append("drp.doc_ser");
        headers.append("серия ДРП");
        formats.append("S");
        fields.append("drp.doc_num");
        headers.append("номер ДРП");
        formats.append("S");
        fields.append("drp.doc_date");
        headers.append("дата выдачи ДРП");
        formats.append("D");
        fields.append("drp.doc_exp");
        headers.append("срок действия ДРП");
        formats.append("D");
        fields.append("drp.org_code");
        headers.append("код орг. ДРП");
        formats.append("S");
        fields.append("drp.doc_org");
        headers.append("выдающая организация ДРП");
        formats.append("S");
    }
    if (ui->ch_pers_photo->isChecked()) {
        tables += " left join persons_photo pht on(pht.id_person=e.id and pht.type=2) "
                  " left join persons_photo sgn on(sgn.id_person=e.id and sgn.type=3) ";
        fields.append("(pht.bin is not NULL) as has_sign");
        headers.append("есть фото?");
        formats.append("B");
        fields.append("(sgn.bin is not NULL) as has_sign");
        headers.append("есть подпись?");
        formats.append("B");
    }
    if (ui->ch_pers_phone->isChecked()) {
        fields.append("e.phone_cell");
        headers.append("тел.личный (сотовый)");
        formats.append("S");
        fields.append("e.phone_code");
        headers.append("тел.код");
        formats.append("S");
        fields.append("e.phone_home");
        headers.append("тел.домашний");
        formats.append("S");
        fields.append("e.phone_work");
        headers.append("тел.рабочий");
        formats.append("S");
    }


    // данные представителя
    if (ui->ch_reps->isChecked()) {
        tables +=  " left join persons rep on(rep.id=e.rep_id) ";
    }
    if (ui->ch_rep_fio->isChecked()) {
        fields.append("e.rep_relation");
        headers.append("отношение предст. к застрахованному");
        formats.append("I");
        fields.append("rep.fam");
        headers.append("фамилия предст.");
        formats.append("S");
        fields.append("rep.im");
        headers.append("имя предст.");
        formats.append("S");
        fields.append("rep.ot");
        headers.append("отчество предст.");
        formats.append("S");
        fields.append("rep.sex");
        headers.append("пол предст.");
        formats.append("I");
        fields.append("rep.date_birth");
        headers.append("дат.рожд. предст.");
        formats.append("D");
    }
    if (ui->ch_rep_country->isChecked()) {
        tables += " left join spr_oksm oksm_b_rep on(oksm_b_rep.code=rep.oksm_birth) "
                  " left join spr_oksm oksm_c_rep on(oksm_c_rep.code=rep.oksm_c) ";
        fields.append("oksm_b_rep.short_name as oksm_name_b_rep");
        headers.append("страна происх. предст.");
        formats.append("S");
        fields.append("oksm_b_rep.alfa3 as oksm_alpha_b_rep");
        headers.append("А3 ржд предст.");
        formats.append("S");
        fields.append("oksm_c_rep.short_name as oksm_name_c_rep");
        headers.append("гражданство предст.");
        formats.append("S");
        fields.append("oksm_c_rep.alfa3 as oksm_alpha_c_rep");
        headers.append("А3 гр предст.");
        formats.append("S");
    }
    if (ui->ch_rep_adr_reg->isChecked()) {
        tables += " left join addresses adr_r_rep on(adr_r_rep.id=rep.id_addr_reg) ";
        fields.append("adr_r_rep.ocato as ocato_r_rep");
        headers.append("ОКАТО рег. предст.");
        formats.append("S");
        fields.append("adr_r_rep.kladr as kladr_r_rep");
        headers.append("КЛАДР рег. предст.");
        formats.append("S");
    }
    if (ui->ch_rep_adr_liv->isChecked()) {
        tables += " left join addresses adr_l_rep on(adr_l_rep.id=rep.id_addr_liv) ";
        fields.append("adr_l_rep.ocato as ocato_l_rep");
        headers.append("ОКАТО прж. предст.");
        formats.append("S");
        fields.append("adr_l_rep.kladr as kladr_l_rep");
        headers.append("КЛАДР прж. предст.");
        formats.append("S");
    }
    if (ui->ch_rep_UDL->isChecked()) {
        tables += " left join persons_docs udl_rep on(udl_rep.id=rep.id_udl) "
                  " left join spr_f011 f11_rep on(f11_rep.code=udl_rep.doc_type) ";
        fields.append("f11_rep.code as udl_code_rep");
        headers.append("код УДЛ предст.");
        formats.append("I");
        fields.append("f11_rep.text as udl_type_rep");
        headers.append("тип УДЛ предст.");
        formats.append("S");
        fields.append("udl_rep.doc_ser as doc_ser_rep");
        headers.append("серия УДЛ предст.");
        formats.append("S");
        fields.append("udl_rep.doc_num");
        headers.append("номер УДЛ предст.");
        formats.append("S");
        fields.append("udl_rep.doc_date");
        headers.append("дата выдачи УДЛ предст.");
        formats.append("D");
        fields.append("udl_rep.doc_exp");
        headers.append("срок действия УДЛ предст.");
        formats.append("D");
        fields.append("udl_rep.org_code");
        headers.append("код орг. УДЛ предст.");
        formats.append("S");
        fields.append("udl_rep.doc_org");
        headers.append("выдающая организация УДЛ предст.");
        formats.append("S");
    }
    if (ui->ch_rep_DRP->isChecked()) {
        tables += " left join persons_docs drp_rep on(drp_rep.id=e.id_drp) "
                  " left join spr_f011 f11p_rep on(f11p_rep.code=drp_rep.doc_type) ";
        fields.append("f11p_rep.code as drp_code_rep");
        headers.append("код ДРП предст.");
        formats.append("I");
        fields.append("f11p_rep.text as drp_type_rep");
        headers.append("тип ДРП предст.");
        formats.append("S");
        fields.append("drp_rep.doc_ser");
        headers.append("серия ДРП предст.");
        formats.append("S");
        fields.append("drp_rep.doc_num");
        headers.append("номер ДРП предст.");
        formats.append("S");
        fields.append("drp_rep.doc_date");
        headers.append("дата выдачи ДРП предст.");
        formats.append("D");
        fields.append("drp_rep.doc_exp");
        headers.append("срок действия ДРП предст.");
        formats.append("D");
        fields.append("drp_rep.org_code");
        headers.append("код орг. ДРП предст.");
        formats.append("S");
        fields.append("drp_rep.doc_org");
        headers.append("выдающая организация ДРП предст.");
        formats.append("S");
    }
    if (ui->ch_rep_phone->isChecked()) {
        fields.append("rep.phone_cell");
        headers.append("тел.личный (сотовый) предст.");
        formats.append("S");
        fields.append("rep.phone_code");
        headers.append("тел.код предст.");
        formats.append("S");
        fields.append("rep.phone_home");
        headers.append("тел.домашний предст.");
        formats.append("S");
        fields.append("rep.phone_work");
        headers.append("тел.рабочий предст.");
        formats.append("S");
    }


    // данные прикрепления
    if (ui->ch_data_assig->isChecked()) {
        tables += "  left join link_persons_asg la on(la.id_person=e.id) "
                  "  left join persons_assig asg on(asg.id=la._id_asg) "
                  "  left join spr_medorgs mo on(asg.code_mo=mo.code_mo and mo.is_head=1) ";
        fields.append("asg.assig_type");
        headers.append("способ прикрепления");
        formats.append("I");
        fields.append("asg.code_mo");
        headers.append("код МО");
        formats.append("S");
        fields.append("mo.name_mo");
        headers.append("название МО");
        formats.append("S");
        fields.append("asg.assig_date");
        headers.append("дата прикрепления");
        formats.append("D");
        fields.append("asg.unsig_date");
        headers.append("дата открепления");
        formats.append("D");
        fields.append("asg.is_actual");
        headers.append("акт.");
        formats.append("I");
        orders.append("asg.is_actual");
    }


    // данные полиса
    if (ui->ch_polises->isChecked()) {
        tables += " left join polises p on(p.id=e.id_polis) "
                  " left join spr_f008 f08 on(f08.code=p.pol_v) ";
    }
    if (ui->ch_pol_type->isChecked()) {
        fields.append("p.pol_v");
        headers.append("форма полиса");
        formats.append("I");
        fields.append("f08.text as polv_text");
        headers.append("форма выпуска полиса - текст");
        formats.append("S");
        fields.append("p.pol_type");
        headers.append("тип полиса");
        formats.append("S");
    }
    if (ui->ch_pol_sernom->isChecked()) {
        fields.append("p.vs_num");
        headers.append("номер ВС");
        formats.append("S");
        fields.append("p.pol_ser");
        headers.append("серия полиса");
        formats.append("S");
        fields.append("p.pol_num");
        headers.append("номер полиса");
        formats.append("S");
        fields.append("p.uec_num");
        headers.append("номер УЭК");
        formats.append("S");
        fields.append("p.enp");
        headers.append("ЕНП");
        formats.append("S");
    }
    if (ui->ch_pol_dates->isChecked()) {
        fields.append("p.date_begin");
        headers.append("дата выдачи полиса");
        formats.append("D");
        fields.append("p.date_end");
        headers.append("плановая дата окончания");
        formats.append("D");
        fields.append("p.date_stop");
        headers.append("дата изъятия полиса");
        formats.append("D");
        fields.append("p.date_sms_pol");
        headers.append("дата отправки СМС");
        formats.append("D");
        fields.append("p.tfoms_date");
        headers.append("дата заявки на ЕНП");
        formats.append("D");
        fields.append("p.date_activate");
        headers.append("дата активации ЕНП");
        formats.append("D");
        fields.append("p.date_get2hand");
        headers.append("дата выдачи на руки");
        formats.append("D");
    }
    if (ui->ch_pol_blank_VS->isChecked()) {
        tables += " left join blanks_vs b_vs on(b_vs.vs_num=p.vs_num) "
                  " left join spr_blank_status b_vs_s on(b_vs.status=b_vs_s.code) ";
        fields.append("b_vs.date_add as b_vs_date");
        headers.append("дата получения банка ВС");
        formats.append("D");
        fields.append("b_vs.status as b_vs_status");
        headers.append("статус бланка ВС");
        formats.append("I");
        fields.append("b_vs_s.text as b_vs_status_text");
        headers.append("статус ВС - текст");
        formats.append("S");
    }
    if (ui->ch_pol_blank_pol->isChecked()) {
        tables += " left join blanks_pol b_pol on(b_pol.pol_ser=p.pol_ser and b_pol.pol_num=p.pol_num) "
                  " left join spr_blank_status b_pol_s on(b_pol.status=b_pol_s.code) "
                  " left join spr_fpolis b_fpol on(b_fpol.code=b_pol.f_polis) ";
        fields.append("b_pol.date_add as b_pol_date");
        headers.append("дата получения банка полиса");
        formats.append("D");
        fields.append("b_pol.status as b_pol_status");
        headers.append("статус бланка полиса");
        formats.append("I");
        fields.append("b_pol_s.text as b_pol_status_text");
        headers.append("статус полиса - текст");
        formats.append("S");
        fields.append("b_pol.f_polis as b_pol_fpolis");
        headers.append("форма полиса");
        formats.append("I");
        fields.append("b_fpol.text as b_pol_fpolis_text");
        headers.append("форма полиса - текст");
        formats.append("S");
        fields.append("b_pol.enp as enp_blank");
        headers.append("ЕНП" );
        formats.append("S");
        fields.append("b_pol.date_scan_enp");
        headers.append("дата скан. ЕНП");
        formats.append("D");
    }


    // данные визита
    if (ui->ch_data_vizit->isChecked()) {
        tables += " left join vizits viz on(viz.id_person=e.id) "
                  " left join spr_rsmo rsmo on(rsmo.code=viz.r_smo) "
                  " left join spr_rpolis rpol on(rpol.code=viz.r_polis) "
                  " left join spr_fpolis fpol on(fpol.code=viz.f_polis) "
                  " left join spr_method vmet on(vmet.code=viz.v_method) ";
        fields.append("viz.v_date");
        headers.append("дата посещения");
        formats.append("D");
        fields.append("viz.has_petition as v_petition");
        headers.append("ходатайство?");
        formats.append("B");
        fields.append("viz.v_method as v_method");
        headers.append("method");
        formats.append("I");
        fields.append("vmet.text as v_method_text");
        headers.append("способ посещения - текст");
        formats.append("S");
        fields.append("viz.r_smo as v_rsmo");
        headers.append("r_smo");
        formats.append("I");
        fields.append("rsmo.text as v_rsmo_text");
        headers.append("причина выбора СМО - текст");
        formats.append("S");
        fields.append("viz.r_polis as v_rpolis");
        headers.append("r_pol");
        formats.append("I");
        fields.append("rpol.text as v_rpolis_text");
        headers.append("причина перевыдачи полиса - текст");
        formats.append("S");
        fields.append("viz.f_polis as v_fpolis");
        headers.append("f_pol");
        formats.append("I");
        fields.append("fpol.text as v_fpolis_text");
        headers.append("форма выпуска полиса - текст");
        formats.append("S");

        orders.append("viz.v_date");
    }


    // данные событий
    if (ui->ch_data_evt->isChecked()) {
        tables += " left join events evt on(evt.id_polis=p.id) "
                  " left join spr_r001 r01 on(r01.code=evt.event_code) ";
        fields.append("evt.event_code");
        headers.append("событие");
        formats.append("S");
        fields.append("r01.text as event_text");
        headers.append("событие - текст");
        formats.append("S");
        fields.append("evt.event_dt");
        headers.append("дата события");
        formats.append("D");
        fields.append("evt.event_time");
        headers.append("время события");
        formats.append("T");
        fields.append("evt.status as event_status");
        headers.append("статус события");
        formats.append("I");
        fields.append("evt.send_sms as event_sms");
        headers.append("СМС?");
        formats.append("I");
        fields.append("evt.comment as event_comment");
        headers.append("комментарий");
        formats.append("S");

        orders.append("evt.event_dt");
    }

    // сформируем SQL
    int cnt = fields.count();
    QString sql = "select ";
    for (int i=0; i<cnt; i++) {
        sql += fields.at(i) ;
        if (i<cnt-1)  sql += ", ";
    }
    sql += " from " + tables;
    sql += " order by ";
    int cnt_ord = orders.count();
    for (int i=0; i<cnt_ord; i++) {
        sql += orders.at(i);
        if (i<cnt_ord-1)  sql += ", ";
    }
    sql += " ; ";


    // запросим данные
    QSqlQuery *query = new QSqlQuery(db);
    bool res = mySQL.exec(this, sql, QString("Выбор данных прикрепления на заданное число"), *query, true, db, data_app);
    if (!res) {
        QMessageBox::warning(this, "Данные не получены", "При выборе данных прикрепления произошла неожиданная ошибка.");
        delete query;
        return;
    }
    if (query->size()>0) {
        // подготовим поток для сохранения файла
        QTextStream stream(&file);
        QString codec_name = ui->combo_encoding->currentText();
        QTextCodec *codec = QTextCodec::codecForName(codec_name.toLocal8Bit());
        stream.setCodec(codec);

        QString head;
        int cnt_head = headers.count();
        for (int i=0; i<cnt_head; i++) {
            head += headers.at(i);
            if (i<cnt_head-1)  head += df;
        }
        stream << head << dl;

        // переберём полученные данные и сохраним в файл
        int cnt = query->size();
        while (query->next()) {
            for (int j=0; j<cnt_head; j++) {
                if (!query->value(j).isNull()) {
                    QString format = formats.at(j);
                    if (format=="S") {
                        stream << "\"" << query->value(j).toString().trimmed().simplified() << "\"";
                    } else if (format=="I") {
                        stream << QString::number(query->value(j).toInt());
                    } else if (format=="B") {
                        stream << QString(query->value(j).toBool() ? "да" : "нет");
                    } else if (format=="D") {
                        stream << query->value(j).toDate().toString("dd.MM.yyyy");
                    } else if (format=="T") {
                        stream << query->value(j).toTime().toString("hh:mm:ss");
                    }
                }
                if (j<cnt_head-1)  stream << df;
            }
            stream << dl;
        }
        file.close();

        // ===========================
        // собственно открытие шаблона
        // ===========================
        // открытие полученного ODT
        long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_s.utf16()), 0, 0, SW_NORMAL);

    } else {
        QMessageBox::warning(this,
                             "Ничего не выбрано",
                             "Файл не был созан, поскольку в него нечего сохранять.\n\n"
                             "Операция отменена.");
    }
}

void export2csv_wnd::on_bn_fname_clicked() {
    QString fname_s = QFileDialog::getSaveFileName(this,
                                                   "Куда сохранить файл прикреплений?",
                                                   QString(),
                                                   "файлы данных (*.csv);;простой текст (*.txt)");
    if (fname_s.isEmpty()) {
        QMessageBox::warning(this,
                             "Не выбрано имя файла",
                             "Имя цедевого файла не задано.\n\n"
                             "Операция отменена.");
        return;
    }
    if (QFile::exists(fname_s)) {
        if (QMessageBox::question(this,
                                  "Удалить старый файл?",
                                  "Файл уже существует.\n"
                                  "Удалить старый файл для того чтобы сохранить новый?",
                                  QMessageBox::Yes|QMessageBox::Cancel)==QMessageBox::Yes) {
            if (!QFile::remove(fname_s)) {
                QMessageBox::warning(this,
                                     "Ошибка при удалении старого файла",
                                     "При удалении старого файла произошла непредвиденная ошибка.\n\n"
                                     "Операция отменена.");
                return;
            }
        }
    }
    ui->ln_fname->setText(fname_s);
}
