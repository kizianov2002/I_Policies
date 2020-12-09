#include "print_db_pol_wnd.h"
#include "ui_print_db_pol_wnd.h"

print_db_pol_wnd::print_db_pol_wnd(QSqlDatabase &db, s_data_app &data_app, s_data_pers &data_pers, s_data_vizit &data_vizit, s_data_polis &data_polis, QSettings &settings, QWidget *parent)
 : db(db), data_app(data_app), data_pers(data_pers), data_vizit(data_vizit), data_polis(data_polis), QDialog(parent), settings(settings), ui(new Ui::print_db_pol_wnd)
{
    ui->setupUi(this);

    ui->ch_rep->setEnabled( !(data_pers.rep_fio.isEmpty()) );
    ui->ch_rep->setChecked( !(data_pers.rep_fio.isEmpty()) );

    ui->ch_reply_data->setChecked( false );
    ui->ch_reply_data_2->setEnabled( !(data_pers.rep_fio.isEmpty()) );
    ui->ch_reply_data_2->setChecked( false );

    ui->ln_smo_name_full_rp->setText(data_app.smo_name_full_rp);
    ui->ln_smo_name->setText(data_app.smo_name);
    ui->ln_smo_name_full->setText(data_app.smo_name_full);
    ui->ln_smo_address->setText(data_app.smo_address);
    ui->ln_smo_ogrn->setText(data_app.smo_ogrn);
    ui->ln_signer->setText(data_app.point_signer);
}

print_db_pol_wnd::~print_db_pol_wnd() {
    delete ui;
}

void print_db_pol_wnd::on_bn_close_clicked() {
    reject();
}

QString print_db_pol_wnd::date_to_str(QDate date) {
    QString res = QString::number(date.day()) + " ";
    switch (date.month()) {
    case 1:
        res += "января ";
        break;
    case 2:
        res += "февраля ";
        break;
    case 3:
        res += "марта ";
        break;
    case 4:
        res += "апреля ";
        break;
    case 5:
        res += "мая ";
        break;
    case 6:
        res += "июня ";
        break;
    case 7:
        res += "июля ";
        break;
    case 8:
        res += "августа ";
        break;
    case 9:
        res += "сентября ";
        break;
    case 10:
        res += "октября ";
        break;
    case 11:
        res += "ноября ";
        break;
    case 12:
        res += "декабря ";
        break;
    default:
        break;
    }
    res += QString::number(date.year()) + " г.";
    return res;
}


#include "qt_windows.h"
#include "qwindowdefs_win.h"
#include <shellapi.h>

void print_db_pol_wnd::on_bn_print_clicked() {
    ui->bn_print->setEnabled(false);
    this->setCursor(Qt::WaitCursor);

    // распаковка шаблона
    QString rep_folder = data_app.path_reports;
    // распаковка шаблона
    QString tmp_folder = data_app.path_out + "_STATEMENTS_/temp/";
    QString rep_template = rep_folder + "req_db_pol_new.odt";


    if (!QFile::exists(rep_template)) {
        QMessageBox::warning(this,
                             "Шаблон не найден",
                             "Не найден шаблон печатной формы заявления: \n" + rep_template +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    QDir tmp_dir(tmp_folder);
    if (tmp_dir.exists(tmp_folder) && !tmp_dir.removeRecursively()) {
        QMessageBox::warning(this,
                             "Ошибка при очистке папки",
                             "Не удалось очистить папку временных файлов: \n" + tmp_folder +
                             "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
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
        ui->bn_print->setEnabled(true);
        return;
    }
    com = "\"" + data_app.path_arch + "7z.exe\" e \"" + rep_template + "\" -o\"" + tmp_folder + "\" styles.xml";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка распаковки шаблона",
                             "Не удалось распаковать файл стилей шаблона \n" + rep_template + " -> styles.xml"
                             "\nпечатной формы во временную папку\n" + tmp_folder +
                             "\n\nПроверьте наличие и доступность программного интерфейса 7Z и доступность временной папки." + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
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
        ui->bn_print->setEnabled(true);
        return;
    }

    // срок действия документа
    QDate drp_date_from = data_pers.drp_date;
    QDate drp_date_to = data_pers.drp_date_exp;
    if ( data_pers.category==5 ||
         data_pers.category==10 ) {
        drp_date_from = data_pers.udl_date;
        drp_date_to = data_pers.udl_date_exp;
    }

    // правка полей контента шаблона
    s_content = s_content.
        replace("#SMO_NAME_FULL_RP#", data_app.smo_name_full_rp).
        replace("#SMO_NAME#", data_app.smo_name).
        replace("#SMO_ADDRESS#", data_app.smo_address).
        replace("#SMO_NAME_FULL#", data_app.smo_name_full).
        replace("#SMO_OGRN#", data_app.smo_ogrn).
        replace("#OPERATOR#", data_app.point_signer).

        replace("#P1#", ( (ui->combo_rdbl->currentIndex()==0 ? "V" : " ")) ).
        replace("#P2#", ( (ui->combo_rdbl->currentIndex()==1 ? "V" : " ")) ).

        replace("#F1#", ( (data_vizit.f_polis==1) ? "V" : " ") ).
        replace("#F2#", ( (data_vizit.f_polis==2) ? "V" : " ") ).
        replace("#F3#", ( (data_vizit.f_polis==3) ? "V" : " ") ).
        replace("#F4#", ( (data_vizit.f_polis==0) ? "V" : " ") ).

        replace("#R1#", ( (data_vizit.r_polis==1) ? "V" : " ") ).
        replace("#R2#", ( (data_vizit.r_polis==2) ? "V" : " ") ).
        replace("#R3#", ( (data_vizit.r_polis==3) ? "V" : " ") ).
        replace("#R4#", ( (data_vizit.r_polis==4) ? "V" : " ") ).
        replace("#R5#", ( (data_vizit.r_polis==5) ? "V" : " ") ).

        replace("#X#", ( (ui->ch_reply_data->isChecked()) ? "V" : " ") ).

        replace("#FAM#", data_pers.fam).
        replace("#IM#",  data_pers.im).
        replace("#OT#",  data_pers.ot).
        replace("#IM1#",  data_pers.im.trimmed().left(1).toUpper()+".").
        replace("#OT1#",  data_pers.ot.trimmed().left(1).toUpper()+".").

        replace("#V1#", ( (data_pers.category== 1) ? "V" : " ") ).
        replace("#V2#", ( (data_pers.category== 2) ? "V" : " ") ).
        replace("#V3#", ( (data_pers.category== 3) ? "V" : " ") ).
        replace("#V4#", ( (data_pers.category== 4) ? "V" : " ") ).
        replace("#V5#", ( (data_pers.category== 5) ? "V" : " ") ).
        replace("#V6#", ( (data_pers.category== 6) ? "V" : " ") ).
        replace("#V7#", ( (data_pers.category== 7) ? "V" : " ") ).
        replace("#V8#", ( (data_pers.category== 8) ? "V" : " ") ).
        replace("#V9#", ( (data_pers.category== 9) ? "V" : " ") ).
        replace("#V10#",( (data_pers.category==10) ? "V" : " ") ).
        replace("#V11#",( (data_pers.category==11) ? "V" : " ") ).
        replace("#V12#",( (data_pers.category==12) ? "V" : " ") ).
        replace("#V13#",( (data_pers.category==13) ? "V" : " ") ).
        replace("#V14#",( (data_pers.category==14) ? "V" : " ") ).

        replace("#BR_DATE#",  data_pers.date_birth.toString("dd.MM.yyyy")).
        replace("#BR_PLACE#", data_pers.plc_birth).

        replace("#UDL_TYPE#", data_pers.udl_type_text).
        replace("#UDL_SER#", data_pers.udl_ser).
        replace("#UDL_NUM#", data_pers.udl_num).
        replace("#UDL_ORGCODE#", data_pers.udl_org_code).
        replace("#UDL_ORG#", data_pers.udl_org).
        replace("#UDL_DATE#", data_pers.udl_date.toString("dd.MM.yyyy")).

        replace("#COUNTRY#", data_pers.oksm_c_text).

        replace("#I1", ( (data_pers.reg_index.isEmpty() || data_pers.reg_index.length()< 1) ? QString(" ") : QString(data_pers.reg_index.at( 0)) ) ).
        replace("#I2", ( (data_pers.reg_index.isEmpty() || data_pers.reg_index.length()< 2) ? QString(" ") : QString(data_pers.reg_index.at( 1)) ) ).
        replace("#I3", ( (data_pers.reg_index.isEmpty() || data_pers.reg_index.length()< 3) ? QString(" ") : QString(data_pers.reg_index.at( 2)) ) ).
        replace("#I4", ( (data_pers.reg_index.isEmpty() || data_pers.reg_index.length()< 4) ? QString(" ") : QString(data_pers.reg_index.at( 3)) ) ).
        replace("#I5", ( (data_pers.reg_index.isEmpty() || data_pers.reg_index.length()< 5) ? QString(" ") : QString(data_pers.reg_index.at( 4)) ) ).
        replace("#I6", ( (data_pers.reg_index.isEmpty() || data_pers.reg_index.length()< 6) ? QString(" ") : QString(data_pers.reg_index.at( 5)) ) ).

        replace("#SUBJ_REG#", data_pers.reg_subj_text).
        replace("#DSTR_REG#", data_pers.reg_dstr_text).
        replace("#CITY_REG#", data_pers.reg_city_text).
        replace("#NASP_REG#", data_pers.reg_nasp_text).
        replace("#STRT_REG#", data_pers.reg_strt_text).
        replace("#H_REG#", (data_pers.reg_hous==0 ? QString("") : QString::number(data_pers.reg_hous))).
        replace("#C_REG#", data_pers.reg_corp).
        replace("#Q_REG#", data_pers.reg_qart).
        replace("#DATE_REG#", data_pers.reg_date.toString("dd.MM.yyyy")).
        replace("#BOMJ#",( (data_pers.bomj) ? "V" : " ") ).

        replace("#J1", ( (data_pers.liv_index.isEmpty() || data_pers.liv_index.length()< 1) ? QString(" ") : QString(data_pers.liv_index.at( 0)) ) ).
        replace("#J2", ( (data_pers.liv_index.isEmpty() || data_pers.liv_index.length()< 2) ? QString(" ") : QString(data_pers.liv_index.at( 1)) ) ).
        replace("#J3", ( (data_pers.liv_index.isEmpty() || data_pers.liv_index.length()< 3) ? QString(" ") : QString(data_pers.liv_index.at( 2)) ) ).
        replace("#J4", ( (data_pers.liv_index.isEmpty() || data_pers.liv_index.length()< 4) ? QString(" ") : QString(data_pers.liv_index.at( 3)) ) ).
        replace("#J5", ( (data_pers.liv_index.isEmpty() || data_pers.liv_index.length()< 5) ? QString(" ") : QString(data_pers.liv_index.at( 4)) ) ).
        replace("#J6", ( (data_pers.liv_index.isEmpty() || data_pers.liv_index.length()< 6) ? QString(" ") : QString(data_pers.liv_index.at( 5)) ) ).

        replace("#SUBJ_LIV#", data_pers.liv_subj_text).
        replace("#DSTR_LIV#", data_pers.liv_dstr_text).
        replace("#CITY_LIV#", data_pers.liv_city_text).
        replace("#NASP_LIV#", data_pers.liv_nasp_text).
        replace("#STRT_LIV#", data_pers.liv_strt_text).
        replace("#H_LIV#", (data_pers.liv_hous==0 ? QString("") : QString::number(data_pers.liv_hous))).
        replace("#C_LIV#", data_pers.liv_corp).
        replace("#Q_LIV#", data_pers.liv_qart).

        replace("#DRP_TYPE#", data_pers.drp_type_text).
        replace("#DRP_SER#", data_pers.drp_ser).
        replace("#DRP_NUM#", data_pers.drp_num).
        replace("#DRP_ORGCODE#", data_pers.drp_org_code).
        replace("#DRP_ORG#", data_pers.drp_org).
        replace("#DRP_DATE#", data_pers.drp_date.toString("dd.MM.yyyy")).
        //replace("#DRP_DATE_FROM#", data_pers.drp_date.toString("dd.MM.yyyy")).     // data_pers.drp_date_from.toString("dd.MM.yyyy")).
        //replace("#DRP_DATE_TO#", data_pers.drp_date_exp.toString("dd.MM.yyyy")).
        replace("#DRP_DATE_FROM#", drp_date_from.toString("dd.MM.yyyy")).      //data_pers.drp_date_from.toString("dd.MM.yyyy")).
        //replace("#DRP_DATE_TO#", doc_date_to.toString("dd.MM.yyyy")).
        replace("#DRP_DATE_TO#", data_pers.drp_date_exp_f ? drp_date_to.toString("dd.MM.yyyy") : "").

            replace("#EAES_DOGOVOR#", data_pers.has_eaes_contract ? data_pers.eaes_contract : "").
            replace("#EAES_DOG_DAT#", data_pers.has_eaes_contract ? data_pers.eaes_contract_date.toString("dd.MM.yyyy") : "").
            replace("#EAES_DOG_EXP#", data_pers.has_eaes_contract ? data_pers.eaes_contract_exp.toString("dd.MM.yyyy") : "").
            replace("#EAES_DOCUMENT#", data_pers.has_eaes_document ? data_pers.eaes_document : "").
            replace("#EAES_DOC_DAT#", data_pers.has_eaes_document ? data_pers.eaes_document_date.toString("dd.MM.yyyy") : "").
            replace("#EAES_DOC_EXP#", data_pers.has_eaes_document ? data_pers.eaes_document_exp.toString("dd.MM.yyyy") : "").
            replace("#EAES_RESIDENCE#", data_pers.eaes_residence).
            replace("#EAES_RESIDE_EXP#", data_pers.eaes_residence_exp.toString("dd.MM.yyyy")).
            replace("#EAES_CATEGORY#", data_pers.eaes_category_text).

        replace("#SNILS#", data_pers.snils.replace("--","")).
        replace("#PHONE_HOME#", data_pers.phone_home).
        replace("#PHONE_WORK#", data_pers.phone_work).
        replace("#PHONE_CELL#", data_pers.phone_cell).
        replace("#EMAIL#", data_pers.email).

        replace("#XR", ( (ui->ch_reply_data_2->isChecked()) ? "V" : " ") ).

        //replace("#CURR_DATE#", QDate::currentDate().toString("dd.MM.yyyy")).
        replace("#CURR_DATE#", data_vizit.date.toString("dd.MM.yyyy")).
        replace("#VS_NUM#", data_polis.vs_num).
        //replace("#VS_DATE#", data_polis.date_begin.toString("dd.MM.yyyy"));
        replace("#VS_DATE#", data_vizit.date.toString("dd.MM.yyyy"));


    if (data_vizit.r_polis==1 || data_vizit.r_polis==2) {
        s_content = s_content.replace("#FAM_OLD#", data_pers.fam_old).
            replace("#IM_OLD#", data_pers.im_old).
            replace("#OT_OLD#", data_pers.ot_old).
            replace("#BR_PLACE_OLD#", data_pers.plc_birth_old).
            replace("#BR_DATE_OLD#", data_pers.date_birth_old.toString("dd.MM.yyyy"));
        if (data_pers.sex_old==1) {
            s_content = s_content.replace("#S1O", "V").replace("#S2O", " ");
        } else if (data_pers.sex_old==2) {
            s_content = s_content.replace("#S1O", " ").replace("#S2O", "V");
        } else {
            s_content = s_content.replace("#S1O", " ").replace("#S2O", " ");
        }
    } else {
        s_content = s_content.replace("#FAM_OLD#", " ").
            replace("#IM_OLD#", " ").
            replace("#OT_OLD#", " ").
            replace("#BR_PLACE_OLD#", " ").
            replace("#BR_DATE_OLD#", " ").
            replace("#S1O", " ").
            replace("#S2O", " ");
    }

    if (data_pers.sex==1) {
        s_content = s_content.replace("#S1#", "V").replace("#S2#", " ");
    } else if (data_pers.sex==2) {
        s_content = s_content.replace("#S1#", " ").replace("#S2#", "V");
    } else {
        s_content = s_content.replace("#S1#", " ").replace("#S2#", " ");
    }

    if (ui->ch_rep->isChecked()) {
        s_content = s_content.
            replace("#REP_FAM#", data_pers.rep_fam).
            replace("#REP_IM#",  data_pers.rep_im).
            replace("#REP_OT#",  data_pers.rep_ot).
            replace("#REP_UDL_TYPE#", data_pers.rep_udl_type_text).
            replace("#REP_UDL_SER#", data_pers.rep_udl_ser).
            replace("#REP_UDL_NUM#", data_pers.rep_udl_num).
            replace("#REP_UDL_DATE#", data_pers.rep_udl_date.toString("dd.MM.yyyy")).
            replace("#REP_PHONE_CODE#", data_pers.rep_phone_code).
            replace("#REP_PHONE_HOME#", data_pers.rep_phone_home).
            replace("#REP_PHONE_CELL#", data_pers.rep_phone_cell).
            replace("#REP_PHONE_WORK#", data_pers.rep_phone_work);

        if (data_pers.rep_id>0 && data_pers.rep_relation==1) {
            s_content = s_content.replace("#RR2#", "V").replace("#RR1#", " ").replace("#RR0#", " ");
        } else if (data_pers.rep_id>0 && data_pers.rep_relation==2) {
            s_content = s_content.replace("#RR2#", " ").replace("#RR1#", "V").replace("#RR0#", " ");
        } else if (data_pers.rep_id>0) {
            s_content = s_content.replace("#RR2#", " ").replace("#RR1#", " ").replace("#RR0#", "V");
        } else {
            s_content = s_content.replace("#RR2#", " ").replace("#RR1#", " ").replace("#RR0#", " ");
        }
    } else {
        s_content = s_content.
            replace("#REP_FAM#", " ").
            replace("#REP_IM#",  " ").
            replace("#REP_OT#",  " ").
            replace("#REP_UDL_TYPE#", " ").
            replace("#REP_UDL_SER#", " ").
            replace("#REP_UDL_NUM#", " ").
            replace("#REP_UDL_DATE#", " ").
            replace("#REP_PHONE_CODE#", " ").
            replace("#REP_PHONE_HOME#", " ").
            replace("#REP_PHONE_CELL#", " ").
            replace("#REP_PHONE_WORK#", " ").
            replace("#RR2#", " ").
            replace("#RR1#", " ").
            replace("#RR0#", " ");
    }
    // правка имени подписандта
    QString fam_head = (ui->ch_rep->isChecked() ? data_pers.rep_fam : data_pers.fam);
    QString im_head = (ui->ch_rep->isChecked() ? data_pers.rep_im : data_pers.im);
    QString ot_head = (ui->ch_rep->isChecked() ? data_pers.rep_ot : data_pers.ot);
    QString im_h1 = (im_head.isEmpty() ? "" : QString(im_head.left(1)+"."));
    QString ot_h1 = (ot_head.isEmpty() ? "" : QString(ot_head.left(1)+"."));
    s_content = s_content.
                    replace("#FAM_HEAD#", fam_head).
                    replace("#IM_HEAD#", im_head).
                    replace("#OT_HEAD#", ot_head).
                    replace("#IM_H1#", im_h1).
                    replace("#OT_H1#", ot_h1);

    // сохранение контента шаблона
    QFile f_content_save(tmp_folder + "/content.xml");
    f_content_save.open(QIODevice::WriteOnly);
    f_content_save.write(s_content.toUtf8());
    f_content_save.close();


    // ===========================
    // архивация шаблона
    // ===========================
    QString fname_res = data_app.path_out + "_STATEMENTS_/req_db_smo - " + data_pers.fam + " " + data_pers.im + " " + data_pers.ot + " - " + date_to_str(data_pers.date_birth) + "odt";


    QFile::remove(fname_res);
    while (QFile::exists(fname_res)) {
        if (QMessageBox::warning(this,
                                 "Ошибка сохранения печатной формы заявления",
                                 "Не удалось сохранить шаблон печатной формы заявления: \n" + rep_template +
                                 "\n\nво временную папку\n" + fname_res +
                                 "\n\nПроверьте, не открыт ли целевой файл в сторонней программе и повторите операцию.",
                                 QMessageBox::Retry|QMessageBox::Abort,
                                 QMessageBox::Retry)==QMessageBox::Abort) {
            this->setCursor(Qt::ArrowCursor);
            ui->bn_print->setEnabled(true);
            return;
        }
        QFile::remove(fname_res);
    }
    if (!QFile::copy(rep_template, fname_res) || !QFile::exists(fname_res)) {
        QMessageBox::warning(this, "Ошибка копирования шаблона", "Не удалось скопировать шаблон печатной формы заявления: \n" + rep_template + "\n\nво временную папку\n" + fname_res + "\n\nОперация прервана.");
        this->setCursor(Qt::ArrowCursor);
        ui->bn_print->setEnabled(true);
        return;
    }
    com = "\"" + data_app.path_arch + "7z.exe\" a \"" + fname_res + "\" \"" + tmp_folder + "/content.xml\"";
    if (myProcess.execute(com)!=0) {
        QMessageBox::warning(this,
                             "Ошибка обновления контента",
                             "При добавлении нового контента в шаблон печатной формы заявления произошла непредвиденная ошибка\n\nОпреация прервана.");
    }


    // ===========================
    // собственно открытие шаблона
    // ===========================
    // открытие полученного ODT
    long result = (long long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_res.utf16()), 0, 0, SW_NORMAL);
    this->setCursor(Qt::ArrowCursor);
    if (result<32) {
        QMessageBox::warning(this,
                             "Ошибка при открытии файла печатной формы заявления",
                             "Файл печатной формы заявления не найден:\n\n" + fname_res +
                             "\nКод ошибки: " + QString::number(result) +
                             "\n\nПопробуйте открыть этот файл вручную.\n\nЕсли файл не создан или ошибка будет повторяться - обратитесь к разработчику.");
    }
    this->setCursor(Qt::ArrowCursor);
    ui->bn_print->setEnabled(true);
    close();
}

void print_db_pol_wnd::on_bn_edit_clicked(bool checked) {
    if (checked) {
        ui->bn_edit->setText("Сохранить");
        ui->lab_smo_name->setEnabled(true);
        ui->lab_smo_name_full->setEnabled(true);
        ui->lab_smo_name_full_rp->setEnabled(true);
        ui->lab_smo_address->setEnabled(true);
        ui->lab_smo_ogrn->setEnabled(true);
        ui->lab_signer->setEnabled(true);

        ui->ln_smo_name->setEnabled(true);
        ui->ln_smo_name_full->setEnabled(true);
        ui->ln_smo_name_full_rp->setEnabled(true);
        ui->ln_smo_address->setEnabled(true);
        ui->ln_smo_ogrn->setEnabled(true);
        ui->ln_signer->setEnabled(true);

        ui->bn_print->setEnabled(false);
    } else {
        data_app.smo_name_full_rp = ui->ln_smo_name_full_rp->text();
        data_app.smo_name = ui->ln_smo_name->text();
        data_app.smo_name_full = ui->ln_smo_name_full->text();
        data_app.smo_address = ui->ln_smo_address->text();
        data_app.smo_ogrn = ui->ln_smo_ogrn->text();
        data_app.point_signer = ui->ln_signer->text();
        if (QMessageBox::question(this,
                                  "Нужно подтверждение",
                                  "Сохранить сделанные изменения в настройках программы?\n\n"
                                  "В случае согласия новые данные будут сохранены и останутся доступны после перезагрузки программы.\n"
                                  "В случае отказа эти изменения будут доступны только до первого перезапуска программы.",
                                  QMessageBox::Yes|QMessageBox::No,
                                  QMessageBox::No)==QMessageBox::Yes)
        {
            // [req_ch_smo]
            settings.beginGroup("smo");
            settings.setValue("smo_name", data_app.smo_name.toUtf8());
            settings.setValue("smo_name_full", data_app.smo_name_full.toUtf8());
            settings.setValue("smo_name_full_rp", data_app.smo_name_full_rp.toUtf8());
            settings.setValue("smo_address", data_app.smo_address.toUtf8());
            settings.setValue("smo_ogrn", data_app.smo_ogrn.toUtf8());
            settings.setValue("point_signer", data_app.point_signer.toUtf8());
            settings.endGroup();
            settings.sync();
        }
        ui->bn_edit->setText("Изменить");

        ui->lab_smo_name->setEnabled(false);
        ui->lab_smo_name_full->setEnabled(false);
        ui->lab_smo_name_full_rp->setEnabled(true);
        ui->lab_smo_address->setEnabled(false);
        ui->lab_smo_ogrn->setEnabled(false);
        ui->lab_signer->setEnabled(false);

        ui->ln_smo_name->setEnabled(false);
        ui->ln_smo_name_full->setEnabled(false);
        ui->ln_smo_name_full_rp->setEnabled(false);
        ui->ln_smo_address->setEnabled(false);
        ui->ln_smo_ogrn->setEnabled(false);
        ui->ln_signer->setEnabled(false);

        ui->bn_print->setEnabled(true);
    }
}

void print_db_pol_wnd::on_bn_cansel_clicked() {
    reject();
}
