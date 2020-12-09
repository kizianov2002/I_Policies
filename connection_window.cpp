#include "connection_window.h"
#include "ui_connection_window.h"
#include "persons_wnd.h"
#include <QMessageBox>
#include <QDesktopWidget>


ConnectionWindow::ConnectionWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ConnectionWindow)
{
    ui->setupUi(this);

    ui->tc_pass->setEchoMode(QLineEdit::Password);
    ui->tc_op_pass->setEchoMode(QLineEdit::Password);
    ui->lab_start->setVisible(false);

    // версия программы
    data_app.version = "0.4.1";

    // проверим разрешение экрана
    QDesktopWidget *scr_widget = QApplication::desktop();
    data_app.screen_w = scr_widget->width();
    data_app.screen_h = scr_widget->height();
    data_app.screen_small = data_app.screen_w<1500;

    // обнулим дочерние окна
    persons_w = NULL;

    data_app.id_operator = -1;
    data_app.id_point    = -1;
    data_app.id_session  = -1;
    data_app.terminal_id = -1;

    // ini-файл
    data_app.ini_path = QApplication::applicationDirPath() + "/inko_polises_2.ini";
    settings = new QSettings(data_app.ini_path, QSettings::IniFormat , NULL);
    // [polisesDB]
    data_app.host = settings->value("polisesDB/host", "localhost").toString();
    data_app.port = settings->value("polisesDB/port", 5432).toInt();
    data_app.base = settings->value("polisesDB/base", "inko_polises").toString();

    // расшифруем пароль
    QString user = "belg", user2 = settings->value("polisesDB/user", "postgres").toString();
    QString pass = "belg", pass2 = settings->value("polisesDB/pass", "650325").toString();
    if ((user2=="belg" || user2=="kursk_user" || user2=="belg_user") && (pass2=="belg" || pass2=="50ub@_4wr" || pass2=="50uzb@$_2")) {
        if (data_app.base=="inko_polises_belgorod") {
            user = "belg_user";
            pass = "50ub@_4wr";
        } else if (data_app.base=="inko_polises_kursk") {
            user = "kursk_user";
            pass = "50uzb@$_2";
        } else if (data_app.base=="inko_polises_voronej") {
            user = "voro_user";
            pass = "50uzb@$_1";
        }
    } else {
        mySQL.myUnBYTEA2(pass2, pass);
        user = user2;
    }
    data_app.user = user;
    data_app.pass = pass;

    // [scoresDB]
    data_app.scores_host = settings->value("scoresDB/scores_host", "172.20.250.56").toString();
    data_app.scores_port = settings->value("scoresDB/scores_port", 0).toInt();
    data_app.scores_base = settings->value("scoresDB/scores_base", (data_app.ocato=="38000" ? "kursk" : "belgorod")).toString();
    data_app.scores_user = settings->value("scoresDB/scores_user", "SYSDBA").toString();
    data_app.scores_pass = settings->value("scoresDB/scores_pass", "masterkey").toString();
    //QMessageBox::information(this, "info", "host=" + data_app.host + " port=" + QString::number(data_app.port) + " base=" + data_app.base + " user=" + data_app.user + " pass=" + data_app.pass + " " );
    // [flags]
    data_app.is_tech                = settings->value("flags/is_tech",  (true)).toBool();
    data_app.is_head                = settings->value("flags/is_head",  (true)).toBool();
    data_app.is_locl                = settings->value("flags/is_locl",  (true)).toBool();
    data_app.is_call                = settings->value("flags/is_call",  (true)).toBool();
    data_app.f_gen_regVS            = settings->value("flags/gen_regVS", (data_app.ocato=="38000" ? false : true)).toBool();
    data_app.f_gen_regPol           = settings->value("flags/gen_regPol",(data_app.ocato=="38000" ? false : true)).toBool();
    data_app.f_send2fond_xmls       = settings->value("flags/send_to_fond_xmls",(data_app.ocato=="38000" ? false : false)).toBool();
    data_app.f_send2fond_zips       = settings->value("flags/send_to_fond_zips",(data_app.ocato=="38000" ? true : true)).toBool();
    data_app.f_send2fond_1ZIP       = settings->value("flags/send_to_fond_1ZIP",(data_app.ocato=="38000" ? false : true)).toBool();
    data_app.f_send2fond_new_IM     = settings->value("flags/send_to_fond_new_IM",(data_app.ocato=="38000" ? false : true)).toBool();
    data_app.f_send2fond_new_OT     = settings->value("flags/send_to_fond_new_OT",(data_app.ocato=="38000" ? false : true)).toBool();
    data_app.f_send2fond_new_assigs = settings->value("flags/send_to_fond_new_assigs",(data_app.ocato=="38000" ? false : true)).toBool();
    data_app.use_folders            = settings->value("flags/use_folders",  (false)).toBool();
    data_app.default_folder         = QString::fromUtf8(settings->value("flags/default_folder", data_app.default_folder).toByteArray());
    // [paths]
    data_app.path_install    = QString::fromUtf8(settings->value("paths/path_install", "D:/INKO/").toByteArray());
    data_app.path_arch       = QString::fromUtf8(settings->value("paths/path_arch",    data_app.path_install + "7-Zip/").toByteArray());
    data_app.path_reports    = QString::fromUtf8(settings->value("paths/path_reports", data_app.path_install + "reports/").toByteArray());
    data_app.path_temp       = QString::fromUtf8(settings->value("paths/path_temp",    "D:/TEMP/").toByteArray());
    data_app.path_foto       = QString::fromUtf8(settings->value("paths/path_foto",    "D:/TEMP/").toByteArray());
    data_app.path_sign       = QString::fromUtf8(settings->value("paths/path_sign",    "D:/TEMP/").toByteArray());
    data_app.path_out        = QString::fromUtf8(settings->value("paths/path_out",     "D:/INKO/_OUT_/").toByteArray());
    data_app.path_in         = QString::fromUtf8(settings->value("paths/path_in",      "D:/INKO/_IN_/").toByteArray());
    data_app.path_dbf        = QString::fromUtf8(settings->value("paths/path_dbf",     "D:/INKO/_DBF_/").toByteArray());
    data_app.path_acts       = QString::fromUtf8(settings->value("paths/path_acts",    "D:/INKO/_ACTS_/").toByteArray());
    data_app.path_to_TFOMS   = QString::fromUtf8(settings->value("paths/path_to_TFOMS",     "D:/INKO/_TFOMS_to_/").toByteArray());
    data_app.path_from_TFOMS = QString::fromUtf8(settings->value("paths/path_from_TFOMS",     "D:/INKO/_TFOMS_from_/").toByteArray());

    if (data_app.path_in=="D:/INKO/_OUT_/")
        data_app.path_in ="D:/INKO/_IN_/";

    if (data_app.path_to_TFOMS=="D:/INKO/_DBF_/")
        data_app.path_to_TFOMS ="D:/INKO/_TFOMS_to_/";
    if (data_app.path_from_TFOMS=="D:/INKO/_DBF_/")
        data_app.path_from_TFOMS ="D:/INKO/_TFOMS_from_/";

    if (data_app.path_to_TFOMS=="D:/INKO/_TFOMS_from_/")
        data_app.path_to_TFOMS ="D:/INKO/_TFOMS_to_/";
    if (data_app.path_from_TFOMS=="D:/INKO/_TFOMS_to_/")
        data_app.path_from_TFOMS ="D:/INKO/_TFOMS_from_/";

    // [smo]
    data_app.ocato       = settings->value("smo/terr_ocato", "14000").toString();
    data_app.ocato_text  = QString::fromUtf8(settings->value("smo/terr_ocato_text", "Белгородская область").toByteArray());
    data_app.smo_ogrn         = settings->value("smo/smo_ogrn", "1043600049041").toString();
    data_app.smo_regnum       = settings->value("smo/smo_regnum", "46006").toString();
    data_app.smo_logo1        = QString::fromUtf8(settings->value("smo/smo_logo1",        "308000, г.Белгород, ул. Н.Островского, д.22").toByteArray());
    data_app.smo_logo2        = QString::fromUtf8(settings->value("smo/smo_logo2",        "Телефон   (4722) 20-28-76,   8-800-100-36-03").toByteArray());
    data_app.smo_logo3        = QString::fromUtf8(settings->value("smo/smo_logo3",        "inkobelgorod@inko-med.ru, www.inko-med.ru").toByteArray());
    data_app.smo_short        = QString::fromUtf8(settings->value("smo/smo_short",        "ИНКО-МЕД").toByteArray());
    data_app.smo_name         = QString::fromUtf8(settings->value("smo/smo_name",         "OOO \"Медицинская страховая компания \"ИНКО-МЕД\"").toByteArray());
    data_app.smo_name_full    = QString::fromUtf8(settings->value("smo/smo_name_full",    "Общество с ограниченной ответственностью \"Медицинская страховая компания \"ИНКО-МЕД\"").toByteArray());
    data_app.smo_name_full_rp = QString::fromUtf8(settings->value("smo/smo_name_full_rp", "Медицинскую страховую компанию \"ИНКО-МЕД\"").toByteArray());
    data_app.smo_director     = QString::fromUtf8(settings->value("smo/smo_director",     "Серёгина А.А.").toByteArray());
    data_app.smo_oms_chif     = QString::fromUtf8(settings->value("smo/smo_oms_chif",     "Проценко Елена Александровна").toByteArray());
    data_app.smo_address      = QString::fromUtf8(settings->value("smo/smo_address",      "394018, г. Воронеж, ул. Платонова, д. 14, тел. (473)2-550-170, 2-298-025").toByteArray());
    data_app.smo_contact_info = QString::fromUtf8(settings->value("smo/smo_contact_info", "Телефон контакт-центра: 8 800 100 36 03").toByteArray());
    data_app.smo_contact_info = "Телефон контакт-центра: 8-800-100-36-03";

    data_app.point_regnum     = settings->value("smo/point_regnum", QString(data_app.ocato=="38000" ? "001" : "101")).toString();
    data_app.point_name       = QString::fromUtf8(settings->value("smo/point_name",       " - тестовый пункт выдачи - ").toByteArray());
    data_app.operator_fio     = QString::fromUtf8(settings->value("smo/operator_fio",     " - ИНКО-тест - ").toByteArray());
    data_app.point_signer     = QString::fromUtf8(settings->value("smo/point_signer",     "").toByteArray());
    // [filial]
    data_app.terr_name          = QString::fromUtf8(settings->value("filial/terr_name",          (data_app.ocato=="38000" ? "Курская область" : "Белгородская область")).toByteArray());
    data_app.terr_name_rp       = QString::fromUtf8(settings->value("filial/terr_name_rp",       (data_app.ocato=="38000" ? "Курской области" : "Белгородской области")).toByteArray());
    data_app.tfoms_director     = QString::fromUtf8(settings->value("filial/tfoms_director",     (data_app.ocato=="38000" ? "Директор ТФОМС Курской области" : "Директор ТФОМС Белгородской области")).toByteArray());
    data_app.tfoms_director_2   = QString::fromUtf8(settings->value("filial/tfoms_director_2",   (data_app.ocato=="38000" ? "" : "")).toByteArray());
    data_app.tfoms_director_fio = QString::fromUtf8(settings->value("filial/tfoms_director_fio", (data_app.ocato=="38000" ? "Калашников Николай Михайлович" : "Курцев Андрей Владимирович")).toByteArray());
    data_app.tfoms_chif         = QString::fromUtf8(settings->value("filial/tfoms_chif",         (data_app.ocato=="38000" ? "Калашников Н.М." : "Курцев А. В.")).toByteArray());
    data_app.filial_name        = QString::fromUtf8(settings->value("filial/filial_name",        (data_app.ocato=="38000" ? "Курский филиал" : "Белгородский филиал")).toByteArray());
    data_app.filial_name_rp     = QString::fromUtf8(settings->value("filial/filial_name_rp",     (data_app.ocato=="38000" ? "Курского филиала" : "Белгородского филиала")).toByteArray());
    data_app.filial_city        = QString::fromUtf8(settings->value("filial/filial_city",        (data_app.ocato=="38000" ? "г. Курск" : "г. Белгород")).toByteArray());
    data_app.filial_by_city     = QString::fromUtf8(settings->value("filial/filial_by_city",     (data_app.ocato=="38000" ? "филиал ООО «МСК «ИНКО-МЕД» в г. Курске" : "филиал ООО «МСК «ИНКО-МЕД» в г. Белгороде")).toByteArray());
    data_app.filial_by_city_tp  = QString::fromUtf8(settings->value("filial/filial_by_city_tp",  (data_app.ocato=="38000" ? "филиалом ООО «МСК «ИНКО-МЕД» в г. Курске" : "филиалом ООО «МСК «ИНКО-МЕД» в г. Белгороде")).toByteArray());
    data_app.filial_director    = QString::fromUtf8(settings->value("filial/filial_director",    (data_app.ocato=="38000" ? "Заместитель генерального директора" : "Заместитель генерального директора")).toByteArray());
    data_app.filial_director_2  = QString::fromUtf8(settings->value("filial/filial_director_2",  (data_app.ocato=="38000" ? "директор Курского филиала ООО \"МСК \"ИНКО-МЕД\"" : "директор Белгородского филиала ООО \"МСК \"ИНКО-МЕД\"")).toByteArray());
    data_app.filial_director_fio= QString::fromUtf8(settings->value("filial/filial_director_fio",(data_app.ocato=="38000" ? "Бакуневич Дмитрий Олегович" : "Гапеев Александр Григорьевич")).toByteArray());
    data_app.filial_chif        = QString::fromUtf8(settings->value("filial/filial_chif",        (data_app.ocato=="38000" ? "Бакуневич Д.О." : "Гапеев А.Г.")).toByteArray());
    data_app.rep_doer           = QString::fromUtf8(settings->value("filial/rep_doer",           (data_app.ocato=="38000" ? "Баркова Г.А." : "Дралова Т.С.")).toByteArray());
    data_app.filial_phone       = QString::fromUtf8(settings->value("filial/filial_phone",       (data_app.ocato=="38000" ? "8(4712) 39-04-19" : "8(4722) 20-28-76")).toByteArray());
    data_app.filial_phone_2     = QString::fromUtf8(settings->value("filial/filial_phone_2",     (data_app.ocato=="38000" ? "" : "")).toByteArray());
    data_app.filial_email       = QString::fromUtf8(settings->value("filial/filial_email",       (data_app.ocato=="38000" ? "inkokursk@inko-med.ru" : "inkobelgorod@inko-med.ru")).toByteArray());
    data_app.filial_email_2     = QString::fromUtf8(settings->value("filial/filial_email_2",     (data_app.ocato=="38000" ? "" : "")).toByteArray());
    // [msg]
    data_app.msg_timeout_back  = settings->value("msg/msg_timeout_back",  "5000").toInt();
    data_app.msg_timeout_front = settings->value("msg/msg_timeout_front", "1000").toInt();
    // [assig]
    data_app.assig_vers        = settings->value("assig/assig_vers",  (data_app.ocato=="38000" ? 0 : 1)).toInt();
    data_app.assig_csv_headers = settings->value("assig/csv_headers", (data_app.ocato=="38000" ? false : true)).toBool();
    // [log]
    data_app.log_bord = settings->value("log/log_bord", 1000).toInt();
    // [xml]
    data_app.xml_vers             = settings->value("xml/xml_vers",             (data_app.ocato=="38000" ? "2.1a" : "2.1")).toString();
    data_app.snils_format         = settings->value("xml/snils_format",         (data_app.ocato=="38000" ? 1 : 0)).toInt();
    data_app.split_by_ot          = settings->value("xml/split_by_ot",          (data_app.ocato=="38000" ? 1 : 0)).toInt();
    data_app.send_p02_in_p06      = settings->value("xml/send_p02_in_p06",      0).toInt();
    data_app.pol_start_date_by_vs = settings->value("xml/pol_start_date_by_vs", (data_app.ocato=="38000" ? 1 : 0)).toInt();
    data_app.f_zip_packs          = settings->value("xml/zip_packs",            (data_app.ocato=="38000" ? false : true)).toBool();
    // [VS]
    data_app.vs_blank_width_mm  = settings->value("VS/blank_width_mm",  205).toInt();
    data_app.vs_blank_height_mm = settings->value("VS/blank_height_mm", 295).toInt();
    data_app.vs_field_left_mm   = settings->value("VS/field_left_mm",    18).toInt();
    data_app.vs_field_right_mm  = settings->value("VS/field_right_mm",    5).toInt();
    data_app.vs_field_top_mm    = settings->value("VS/field_top_mm",     18).toInt();
    data_app.vs_field_bottom_mm = settings->value("VS/field_bottom_mm",   5).toInt();
    // [form8]
    data_app.form8_doc1_num      = settings->value("form8/doc1_num", "").toString();
    data_app.form8_doc1_date     = QDate::fromString(settings->value("form8/doc1_date", "1900-01-01").toString(), "yyyy-MM-dd");
    data_app.form8_has_doc2      = (settings->value("form8/has_doc2", "0").toInt() > 0);
    data_app.form8_doc2_num      = settings->value("form8/doc2_num", "").toString();
    data_app.form8_doc2_date     = QDate::fromString(settings->value("form8/doc2_date", "1900-01-01").toString(), "yyyy-MM-dd");
    data_app.form8_codename1     = settings->value("form8/codename1", "отчитывающейся организации по ОКПО").toString();
    data_app.form8_codename2     = settings->value("form8/codename2", "").toString();
    data_app.form8_codename3     = settings->value("form8/codename3", "").toString();
    data_app.form8_code1         = settings->value("form8/code1",     "").toString();
    data_app.form8_code2         = settings->value("form8/code2",     "").toString();
    data_app.form8_code3         = settings->value("form8/code3",     "").toString();
    data_app.tfoms_worker        = settings->value("form8/tfoms_worker",        "").toString();
    data_app.tfoms_worker2_place = settings->value("form8/tfoms_worker2_place", "").toString();
    data_app.tfoms_worker2_fio   = settings->value("form8/tfoms_worker2_fio",   "").toString();
    data_app.tfoms_worker2_phone = settings->value("form8/tfoms_worker2_phone", "").toString();

    // [barcode]
    data_app.barcodes_portname = settings->value("barcode/portname", "COM4").toString();
    data_app.barcodes_file     = settings->value("barcode/file", "d:/temp/_inko_scan.csv").toString();

    // [blank_acts]
    data_app.act_dog_num            = settings->value("blank_acts/dog_num", "").toString();
    data_app.act_dog_date           = QDate::fromString(settings->value("blank_acts/dog_date", "2016-01-01").toString(),"yyyy-MM-dd");
    data_app.act_filial_chif_post   = settings->value("blank_acts/filial_chif_post", "").toString();
    data_app.act_filial_chif_fio    = settings->value("blank_acts/filial_chif_fio", "").toString();
    data_app.act_point_chif_post    = settings->value("blank_acts/point_chif_post", "").toString();
    data_app.act_point_chif_fio     = settings->value("blank_acts/point_chif_fio", "").toString();

    data_app.act_point_org_name     = settings->value("blank_acts/point_org_name", "").toString();
    data_app.act_point_address      = settings->value("blank_acts/point_address", "").toString();
    data_app.act_point_requisits_1  = settings->value("blank_acts/point_requisits_1", "").toString();
    data_app.act_point_requisits_2  = settings->value("blank_acts/point_requisits_2", "").toString();
    data_app.act_point_okpo         = settings->value("blank_acts/point_okpo", "").toString();

    data_app.act_filial_org_name    = settings->value("blank_acts/filial_org_name", "").toString();
    data_app.act_filial_address     = settings->value("blank_acts/filial_address", "").toString();
    data_app.act_filial_requisits_1 = settings->value("blank_acts/filial_requisits_1", "").toString();
    data_app.act_filial_requisits_2 = settings->value("blank_acts/filial_requisits_2", "").toString();
    data_app.act_filial_okpo        = settings->value("blank_acts/filial_okpo", "").toString();

    data_app.act_base_doc_name      = settings->value("blank_acts/base_doc_name", "").toString();
    data_app.act_base_doc_num       = settings->value("blank_acts/base_doc_num", "").toString();
    data_app.act_base_doc_date      = QDate::fromString(settings->value("blank_acts/base_doc_date", "2016-01-01").toString(),"yyyy-MM-dd");
    data_app.act_num                = settings->value("blank_acts/act_num", "").toString();
    data_app.act_date               = QDate::fromString(settings->value("blank_acts/act_date", "").toString(),"yyyy-MM-dd");
    data_app.act_count              = settings->value("blank_acts/act_count", "").toString();
    data_app.act_tabel_num          = settings->value("blank_acts/act_tabel_num", "").toString();

    // [filials]
    ui->combo_filial->clear();
    data_app.filials_list = settings->value("filials/filials_list", "kursk;belg;voro;test").toString();
    if (data_app.filials_list.indexOf("kursk")>=0) {
        data_app.kursk_host = settings->value("kursk/host", "172.20.250.15").toString();
        data_app.kursk_port = settings->value("kursk/port", 5432).toInt();
        data_app.kursk_base = settings->value("kursk/base", "inko_polises_kursk").toString();
        //data_app.kursk_user = settings->value("kursk/user", "belg").toString();
        data_app.kursk_user = "kursk_user";
        // расшифруем пароль
        /*QString pass, pass2 = settings->value("kursk/pass", "belg").toString();
        mySQL.myUnBYTEA2(pass2, pass);
        data_app.kursk_pass = pass;*/
        data_app.kursk_pass = "50uzb@$_2";
        //data_app.belg_pass = settings->value("kursk/pass", "belg").toString();
        ui->combo_filial->addItem("филиал, г. Курск", 8);
    }
    if (data_app.filials_list.indexOf("belg")>=0) {
        data_app.belg_host = settings->value("belg/host", "172.20.250.15").toString();
        data_app.belg_port = settings->value("belg/port", 5432).toInt();
        data_app.belg_base = settings->value("belg/base", "inko_polises_belgorod").toString();
        //data_app.belg_user = settings->value("belg/user", "belg").toString();
        data_app.belg_user = "belg_user";
        // расшифруем пароль
        /*QString pass, pass2 = settings->value("belg/pass", "belg").toString();
        mySQL.myUnBYTEA2(pass2, pass);
        data_app.belg_pass = pass;*/
        data_app.belg_pass = "50ub@_4wr";
        //data_app.belg_pass = settings->value("belg/pass", "belg").toString();
        ui->combo_filial->addItem("филиал, г. Белгород", 64);
    }
    if (data_app.filials_list.indexOf("voro")>=0) {
        data_app.voro_host = settings->value("voro/host", "172.20.250.15").toString();
        data_app.voro_port = settings->value("voro/port", 5432).toInt();
        data_app.voro_base = settings->value("voro/base", "inko_polises_voronej").toString();
        //data_app.voro_user = settings->value("voro/user", "voro").toString();
        data_app.voro_user = "voro_user";
        // расшифруем пароль
        /*QString pass, pass2 = settings->value("voro/pass", "voro").toString();
        mySQL.myUnBYTEA2(pass2, pass);
        data_app.voro_pass = pass;*/
        data_app.voro_pass = "50uzb@$_1";
        //data_app.voro_pass = settings->value("voro/pass", "voro").toString();
        ui->combo_filial->addItem("ООО \"МСК \"ИНКО-МЕД\", г. Воронеж", 1);
    }
    if (data_app.filials_list.indexOf("test")>=0) {
        data_app.test_host = settings->value("test/host", "172.20.250.15").toString();
        data_app.test_port = settings->value("test/port", 5432).toInt();
        data_app.test_base = settings->value("test/base", "inko_polises_test").toString();
        //data_app.test_user = settings->value("test/user", "belg").toString();
        data_app.test_user = "belg";
        // расшифруем пароль
        /*QString pass, pass2 = settings->value("test/pass", "belg").toString();
        mySQL.myUnBYTEA2(pass2, pass);
        data_app.test_pass = pass;*/
        data_app.test_pass = "belg";
        //data_app.test_pass = settings->value("test/pass", "belg").toString();
        ui->combo_filial->addItem("тестовая база, г. Воронеж", 0);
    }
    if (data_app.filials_list.indexOf("work")>=0) {
        data_app.work_host = settings->value("work/host", "127.0.0.1").toString();
        data_app.work_port = settings->value("work/port", 5432).toInt();
        data_app.work_base = settings->value("work/base", "inko_polises").toString();
        //data_app.work_user = settings->value("work/user", "postgres").toString();
        data_app.work_user = "postgres";
        // расшифруем пароль
        /*QString pass, pass2 = settings->value("work/pass", "650325").toString();
        mySQL.myUnBYTEA2(pass2, pass);
        data_app.work_pass = pass;*/
        data_app.work_pass = "650325";
        //data_app.work_pass = settings->value("work/pass", "650325").toString();
        ui->combo_filial->addItem(" - рабочая база - ", -1);
    }


    // принудительная инициализация пустых полей

    // [smo]
    if (data_app.ocato.isEmpty())
        data_app.ocato         = "14000";
    if (data_app.ocato_text.isEmpty())
        data_app.ocato_text    = "Белгородская область";
    if (data_app.smo_ogrn.isEmpty())
        data_app.smo_ogrn           = settings->value("smo/smo_ogrn", "1043600049041").toString();
    if (data_app.smo_regnum.isEmpty())
        data_app.smo_regnum         = settings->value("smo/", "46006").toString();

    if (data_app.ocato=="14000") {
        data_app.smo_logo1          = "308000, г. Белгород, ул. Николая Островского, д.22";
        data_app.smo_logo2          = "Телефон Контакт-центра 8-800-100-36-03";
        data_app.smo_logo3          = "info@inko-med.ru, inkobelgorod@inko-med.ru, www.inko-med.ru";
    } else if (data_app.ocato=="38000") {
        data_app.smo_logo1          = "305000, г. Курск, ул. Радищева 79а";
        data_app.smo_logo2          = "Телефон Контакт-центра 8-800-100-36-03";
        data_app.smo_logo3          = "info@inko-med.ru  www.inko-med.ru";
    } else if (data_app.ocato=="2000") {
        data_app.smo_logo1          = "305000, г. Воронеж, ул. Платонова 14";
        data_app.smo_logo2          = "Телефон Контакт-центра 8-800-100-36-03";
        data_app.smo_logo3          = "info@inko-med.ru  www.inko-med.ru";
    }

    if (data_app.smo_short.isEmpty())
        data_app.smo_short          = "ИНКО-МЕД";
    if (data_app.smo_name.isEmpty())
        data_app.smo_name           = "OOO \"Медицинская страховая компания \"ИНКО-МЕД\"";
    if (data_app.smo_name_full.isEmpty())
        data_app.smo_name_full      = "Общество с ограниченной ответственностью \"Медицинская страховая компания \"ИНКО-МЕД\"";
    if (data_app.smo_name_full_rp.isEmpty())
        data_app.smo_name_full_rp   = "Медицинскую страховую компанию \"ИНКО-МЕД\"";
    if (data_app.smo_oms_chif.isEmpty())
        data_app.smo_oms_chif       = "Проценко Елена Александровна";
    if (data_app.smo_director.isEmpty())
        data_app.smo_director       = "Серёгина А.А.";
    if (data_app.smo_oms_chif.isEmpty())
        data_app.smo_oms_chif       = " ";
    if (data_app.smo_address.isEmpty())
        data_app.smo_address        = "394018, г. Воронеж, ул. Платонова, д. 14, тел. (473)2-550-170, 2-298-025";
    if (data_app.smo_contact_info.isEmpty())
        data_app.smo_contact_info   = "Телефон горячей линии: 8 800 100 36 03";

    if (data_app.point_regnum.isEmpty())
        data_app.point_regnum       = settings->value("smo/", QString(data_app.ocato=="38000" ? "001" : "101")).toString();
    if (data_app.point_name.isEmpty())
        data_app.point_name         = " - тестовый пункт выдачи - ";
    if (data_app.operator_fio.isEmpty())
        data_app.operator_fio       = " - ИНКО-тест - ";
    if (data_app.point_signer.isEmpty())
        data_app.point_signer       = "";
        // [filial]
    if (data_app.terr_name.isEmpty())
        data_app.terr_name          = (data_app.ocato=="38000" ? "Курская область" : "Белгородская область");
    if (data_app.terr_name_rp.isEmpty())
        data_app.terr_name_rp       = (data_app.ocato=="38000" ? "Курской области" : "Белгородской области");
    if (data_app.tfoms_director.isEmpty())
        data_app.tfoms_director     = (data_app.ocato=="38000" ? "Директор ТФОМС Курской области" : "Директор ТФОМС Белгородской области");
    if (data_app.tfoms_director_2.isEmpty())
        data_app.tfoms_director_2   = (data_app.ocato=="38000" ? "" : "");
    if (data_app.tfoms_director_fio.isEmpty())
        data_app.tfoms_director_fio = (data_app.ocato=="38000" ? "Калашников Николай Михайлович" : "Курцев Андрей Владимирович");
    if (data_app.tfoms_chif.isEmpty())
        data_app.tfoms_chif         = (data_app.ocato=="38000" ? "Калашников Н.М." : "Курцев А. В.");
    if (data_app.filial_name.isEmpty())
        data_app.filial_name        = (data_app.ocato=="38000" ? "Курский филиал" : "Белгородский филиал");
    if (data_app.filial_name_rp.isEmpty())
        data_app.filial_name_rp     = (data_app.ocato=="38000" ? "Курского филиала" : "Белгородского филиала");
    if (data_app.filial_city.isEmpty())
        data_app.filial_city        = (data_app.ocato=="38000" ? "г. Курск" : "г. Белгород");
    if (data_app.filial_by_city.isEmpty())
        data_app.filial_by_city     = (data_app.ocato=="38000" ? "филиал ООО «МСК «ИНКО-МЕД» в г. Курске" : "филиал ООО «МСК «ИНКО-МЕД» в г. Белгороде");
    if (data_app.filial_by_city_tp.isEmpty())
        data_app.filial_by_city_tp  = (data_app.ocato=="38000" ? "филиалом ООО «МСК «ИНКО-МЕД» в г. Курске" : "филиалом ООО «МСК «ИНКО-МЕД» в г. Белгороде");
    if (data_app.filial_director.isEmpty())
        data_app.filial_director    = (data_app.ocato=="38000" ? "Заместитель генерального директора" : "Заместитель генерального директора");
    if (data_app.filial_director_2.isEmpty())
        data_app.filial_director_2  = (data_app.ocato=="38000" ? "директор Курского филиала ООО \"МСК \"ИНКО-МЕД\"" : "директор Белгородского филиала ООО \"МСК \"ИНКО-МЕД\"");
    if (data_app.filial_director_fio.isEmpty())
        data_app.filial_director_fio= (data_app.ocato=="38000" ? "Бакуневич Дмитрий Олегович" : "Гапеев Александр Григорьевич");
    if (data_app.filial_chif.isEmpty())
        data_app.filial_chif        = (data_app.ocato=="38000" ? "Бакуневич Д.О." : "Гапеев А.Г.");
    if (data_app.rep_doer.isEmpty())
        data_app.rep_doer           = (data_app.ocato=="38000" ? "Баркова Г.А." : "Дралова Т.С.");
    if (data_app.filial_phone.isEmpty())
        data_app.filial_phone       = (data_app.ocato=="38000" ? "8(4712) 39-04-19" : "8(4722) 20-28-76");
    if (data_app.filial_phone_2.isEmpty())
        data_app.filial_phone_2     = (data_app.ocato=="38000" ? "" : "");
    if (data_app.filial_email.isEmpty())
        data_app.filial_email       = (data_app.ocato=="38000" ? "inkokursk@inko-med.ru" : "inkobelgorod@inko-med.ru");
    if (data_app.filial_email_2.isEmpty())
        data_app.filial_email_2     = (data_app.ocato=="38000" ? "" : "");


    ui->frame_bn_messages->setVisible(data_app.is_tech);
    ui->text_comments->setReadOnly(!data_app.is_tech);

    this->move(100,100);
    refresh_connection();
    ui->tc_op_pass->setFocus();
}

ConnectionWindow::~ConnectionWindow() {
    delete ui;
}

void ConnectionWindow::refresh_connection() {
    ui->lab_DB_no->hide();
    ui->lab_DB_ok->hide();

    settings->sync();

    if (settings->status()==0) {
        // загрузим из инишника параметры подключения и проч.
        ui->tc_host->setText(data_app.host);
        ui->spin_port->setValue(data_app.port);
        ui->tc_base->setText(data_app.base);
        ui->tc_user->setText(data_app.user);
        ui->tc_pass->setText(data_app.pass);

        ui->ch_change_pass->setVisible(data_app.is_tech);
    }

    // создадим подключение к базе обновлений
    db = QSqlDatabase::addDatabase("QPSQL", "My Database");

    db.setConnectOptions();
    // сервер, на котором расположена БД - у нас localhost
    db.setHostName(ui->tc_host->text());
    // порт
    db.setDatabaseName(ui->tc_base->text());
    // имя базы данных
    db.setPort(ui->spin_port->value());
    // имя пользователя - postgres
    db.setUserName(ui->tc_user->text());
    // пароль для подключения к БД
    db.setPassword(ui->tc_pass->text());

    while (!db.open()) {
        // подключение не удалось
        ui->lab_DB_no->show();
        ui->lab_DB_ok->hide();
        ui->pan_DB_params->show();
        ui->pan_login->hide();
        ui->pan_messages->hide();
        //ui->plain_log->hide();
        QString pass_ = data_app.pass;
        if (QMessageBox::question(this,
                                  "Подключение не удалось",
                                  "Подключение не удалось\n\nDB: " + db.lastError().databaseText() +
                                  "\n\ndriver: "+db.lastError().driverText()+
                                  "\n\nПараметры: h=" + data_app.host + ":" + QString::number(data_app.port) + ", u=" + data_app.user + ", p=" + pass_.replace(QRegExp("[A-Z,a-z,0-9]"),"*") +
                                  "\n\nПовторить попытку?",
                                  QMessageBox::Yes|QMessageBox::No)==QMessageBox::No) {
            if (this->windowState()==Qt::WindowFullScreen) {
                //this->resize(300,300);
            } else {
                this->resize(300,300);
            }
            break;
        }

        if (this->windowState()==Qt::WindowFullScreen) {
            //this->resize(300,300);
        } else {
            this->resize(300,300);
        }
    }
    if (db.isOpen()) {
        // подключение готово
        ui->lab_DB_no->hide();
        ui->lab_DB_ok->show();
        ui->pan_DB_params->hide();
        ui->pan_login->show();
        ui->pan_messages->show();
        if (this->windowState()==Qt::WindowFullScreen) {
            //this->resize(1000,800);
        } else {
            this->resize(1000,800);
        }
        //ui->plain_log->hide();



        // запись терминала
        QString sql_terminal = "select id "
                               " from db_terminals "
                               " where name='INKO_POLISES  --  " + data_app.point_name + "  --  " + data_app.operator_fio + "' ; ";
        QSqlQuery *query_terminal = new QSqlQuery(db);
        /*bool res_terminal =*/ mySQL.exec(this, sql_terminal, "Получим ID терминала", *query_terminal, true, db, data_app);
        if (!query_terminal->next()) {
            sql_terminal = "insert into db_terminals(name) "
                           " values('INKO_POLISES  --  " + data_app.point_name + "  --  " + data_app.operator_fio + "') "
                           " returning id ; ";
            /*res_terminal =*/ mySQL.exec(this, sql_terminal, "Добавим терминал", *query_terminal, true, db, data_app);
        };
        data_app.terminal_id = query_terminal->value(0).toInt();



        // дата/время подключения терминала
        sql_terminal = "update db_terminals "
                       " set dt_last=CURRENT_TIMESTAMP "
                       " where id=" + QString::number(data_app.terminal_id) + " ; ";
        /*res_terminal =*/ mySQL.exec(this, sql_terminal, "Добавим терминал", *query_terminal, true, db, data_app);

        // сформируем запись сессии
        QString sql_session = "insert into db_sessions(id_point, id_operator, id_terminal, dt_start, dt_last, status, dt_end) "
                              " values(" + QString::number(data_app.id_point) + ", " + QString::number(data_app.id_operator) + ", " + QString::number(data_app.terminal_id) + ", CURRENT_TIMESTAMP, CURRENT_TIMESTAMP, 1, NULL) "
                              " returning id ; ";
        QSqlQuery *query_session = new QSqlQuery(db);
        /*bool res_session =*/ mySQL.exec(this, sql_session, "Добавим сессию", *query_session, true, db, data_app);
        query_session->next();
        data_app.id_session = query_session->value(0).toInt();

        refresh_points();
        ui->tc_op_pass->setText("");

        // загрузим описание
        QString sql_load = "select text from messages where version='" + data_app.version + "' ; ";
        QSqlQuery *query_load = new QSqlQuery(db);
        /*bool res_load =*/ mySQL.exec(this, sql_load, "Загрузка описания программы", *query_load, true, db, data_app);
        QString s = "версия программы : " + data_app.version + "\n\n";
        while (query_load->next()) {
            s += query_load->value(0).toString() + "\n\n";
        }
        ui->text_comments->setPlainText(s);
        delete query_load;




        QSqlQuery query = QSqlQuery(db);
        QString sql;

        // проверка сервера
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='host_name' ; ";
        /*bool res_h =*/ query.exec( sql );
        s = "";
        /*int n = query.size();*/
        while (query.next()) {
            s += "  " + query.value(1).toString();
        }
        ui->lab_host->setText(s);
        data_app.db_host = s.trimmed();

        // проверка имени БД
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='DB_name' ; ";
        /*bool res_db =*/ query.exec( sql );
        s = "";
        //n = query.size();
        while (query.next()) {
            s += "  " + query.value(1).toString();
        }
        data_app.db_name = s.trimmed();

        // проверка версии
        sql = "select d.name, d.\"value\" "
              "  from db_data d "
              " where d.name='DB_version' ; ";
        bool res_v = query.exec( sql );
        s = "";
        int m = query.size();
        while (query.next()) {
            s += "  " + query.value(1).toString();
        }
        ui->lab_vers->setText(s);
        data_app.db_vers = s.trimmed();


        // получим параметры подключения к базе ФИАС
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='fias_flag' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().toLower().trimmed();
        data_app.FIAS_flag = !(s.replace("false","").replace("0","").trimmed().isEmpty());

        sql = "select d.name, d.\"value\" "
              "  from db_data d "
              "  where d.name='fias_db_drvr' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s += "  " + query.value(1).toString();
        data_app.FIAS_db_drvr = s.trimmed();
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='fias_db_host' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s += "  " + query.value(1).toString();
        data_app.FIAS_db_host = s.trimmed();

        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='fias_db_name' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s += "  " + query.value(1).toString();
        data_app.FIAS_db_name = s.trimmed();
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='fias_db_port' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s += "  " + query.value(1).toString();
        data_app.FIAS_db_port = s.trimmed();
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='fias_db_user' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s += "  " + query.value(1).toString();
        data_app.FIAS_db_user = s.trimmed();
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='fias_db_pass' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s += "  " + query.value(1).toString();
        data_app.FIAS_db_pass = s.trimmed();
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='fias_db_admn' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s += "  " + query.value(1).toString();
        data_app.FIAS_db_admn = s.trimmed();
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='fias_db_admp' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s += "  " + query.value(1).toString();
        data_app.FIAS_db_admp = s.trimmed();
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='fias_db_connstring' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s += "  " + query.value(1).toString();
        data_app.FIAS_db_connstring= s.trimmed();







        // реестер
        data_app.id_point = ui->combo_point->currentData().toInt();

        winreg = new QSettings(QSettings::UserScope, "МСК 'ИНКО-МЕД'", "ИНКО-МЕД - Полисы ОМС", NULL);
        data_app.terminal_id  = winreg->value(data_app.db_host + " - " + data_app.db_name + " - terminal_num", -1).toInt();
        data_app.terminal_name = winreg->value(data_app.db_host + " - " + data_app.db_name + " - terminal_name", " ").toString();

        db.transaction();

        // если новый терминал - получим новый terminal_num из базы
        if (data_app.terminal_id<0) {
            QString sql_terminal = "insert into terminals(terminal_name, status, id_point, dt_last) "
                                   "values (' ', 1, " + QString::number(data_app.id_point) + ", CURRENT_TIMESTAMP) "
                                   " returning id ; ";
            QSqlQuery *query_terminal = new QSqlQuery(db);
            if ( !query_terminal->exec(sql_terminal) ||
                 !query_terminal->next() ) {

                delete query_terminal;
                db.rollback();

                QMessageBox::warning(this, "Ошибка регистрации терминала",
                                     "При попытке регистрации нового терминала произошла ошибка");
            } else {
                data_app.terminal_id = query_terminal->value(0).toInt();
                data_app.terminal_name = " ";
                data_app.terminal_status = 1;

                winreg->setValue(data_app.db_host + " - " + data_app.db_name + " - terminal_num", data_app.terminal_id);
                winreg->setValue(data_app.db_host + " - " + data_app.db_name + " - terminal_name", data_app.terminal_name);

                winreg->sync();

                delete query_terminal;
                db.commit();
            }
        } else {
            // терминал уже известен - обновим его данные из базы
            QString sql_terminal = "select id_point, terminal_name, status "
                                   "  from terminals "
                                   " where id=" + QString::number(data_app.terminal_id) + " ";
            QSqlQuery *query_terminal = new QSqlQuery(db);
            if ( !query_terminal->exec(sql_terminal) ) {

                delete query_terminal;
                db.rollback();

                QMessageBox::warning(this, "Ошибка чтения данных терминала",
                                     "При попытке чтения данных терминала произошла ошибка");
            } else {
                query_terminal->next();

                data_app.terminal_id_point = query_terminal->value(0).toInt();
                data_app.terminal_name = query_terminal->value(1).toString();
                data_app.terminal_status = query_terminal->value(2).toInt();

                winreg->setValue(data_app.db_host + " - " + data_app.db_name + " - terminal_num", data_app.terminal_id);
                winreg->setValue(data_app.db_host + " - " + data_app.db_name + " - terminal_name", data_app.terminal_name);

                winreg->sync();

                delete query_terminal;
                db.commit();
            }
        }

        // обновим в базе закешированный ini-файл
        db.transaction();
        QFile ini_f(data_app.ini_path);
        ini_f.open(QIODevice::ReadOnly);
        QString ini_s = ini_f.readAll();
        ini_s = ini_s.replace("'","\"").replace("\n","[br]");
        ini_f.close();
        QString sql_ini = "update terminals "
                          "   set ini='" + ini_s + "', "
                          "       dt_last=CURRENT_TIMESTAMP, "
                          "       id_point=" + QString::number(data_app.id_point) + " "
                          " where id=" + QString::number(data_app.terminal_id) + " ; ";
        QSqlQuery *query_ini = new QSqlQuery(db);

        query_ini->exec(sql_ini);

        delete query_ini;
        db.commit();

    }

    if (ui->ch_change_pass->isChecked()) {
        ui->pan_DB_params->show();
    }

    // ini-файл
    // [polisesDB]
    settings->beginGroup("polisesDB");
    settings->setValue("host", data_app.host);
    settings->setValue("port", data_app.port);
    settings->setValue("base", data_app.base);
    settings->setValue("user", data_app.user);
    // зашифруем пароль
    QString pass, pass2 = data_app.pass;
    mySQL.myBYTEA2(pass2, pass);
    settings->setValue("pass", pass);
    settings->endGroup();
    // [scoresDB]
    settings->beginGroup("scoresDB");
    settings->setValue("scores_host", data_app.scores_host);
    settings->setValue("scores_port", data_app.scores_port);
    settings->setValue("scores_base", data_app.scores_base);
    settings->setValue("scores_user", data_app.scores_user);
    settings->setValue("scores_pass", data_app.scores_pass);
    settings->endGroup();
    // [flags]
    settings->beginGroup("flags");
    settings->setValue("is_tech", data_app.is_tech);
    settings->setValue("is_head", data_app.is_head);
    settings->setValue("is_locl", data_app.is_locl);
    settings->setValue("is_call", data_app.is_call);
    settings->setValue("gen_regVS", data_app.f_gen_regVS);
    settings->setValue("gen_regPol", data_app.f_gen_regPol);
    settings->setValue("send_to_fond_xmls", data_app.f_send2fond_xmls);
    settings->setValue("send_to_fond_zips", data_app.f_send2fond_zips);
    settings->setValue("send_to_fond_1ZIP", data_app.f_send2fond_1ZIP);
    settings->setValue("send_to_fond_new_IM", data_app.f_send2fond_new_IM);
    settings->setValue("send_to_fond_new_OT", data_app.f_send2fond_new_OT);
    settings->setValue("send_to_fond_new_assigs", data_app.f_send2fond_new_assigs);
    settings->setValue("use_folders", data_app.use_folders);
    settings->endGroup();
    // [paths]
    settings->beginGroup("paths");
    settings->setValue("path_install", data_app.path_install.toUtf8());
    settings->setValue("path_arch", data_app.path_arch.toUtf8());
    settings->setValue("path_reports", data_app.path_reports.toUtf8());
    settings->setValue("path_temp", data_app.path_temp.toUtf8());
    settings->setValue("path_foto", data_app.path_foto.toUtf8());
    settings->setValue("path_sign", data_app.path_sign.toUtf8());
    settings->setValue("path_in", data_app.path_in.toUtf8());
    settings->setValue("path_out", data_app.path_out.toUtf8());
    settings->setValue("path_dbf", data_app.path_dbf.toUtf8());
    settings->setValue("path_acts", data_app.path_acts.toUtf8());
    settings->setValue("path_to_TFOMS", data_app.path_to_TFOMS.toUtf8());
    settings->setValue("path_from_TFOMS", data_app.path_from_TFOMS.toUtf8());
    //settings->setValue("path_in_backup", data_app.path_in_backup.toUtf8());
    //settings->setValue("path_out_backup", data_app.path_out_backup.toUtf8());
    settings->endGroup();
    // [resent]
    settings->beginGroup("resent");
    settings->setValue("point", ui->combo_point->currentText().toUtf8());
    settings->setValue("operator", ui->combo_operator->currentText().toUtf8());
    settings->endGroup();
    // [smo]
    settings->beginGroup("smo");
    settings->setValue("terr_ocato", data_app.ocato);
    settings->setValue("terr_ocato_text", data_app.ocato_text.toUtf8());
    settings->setValue("smo_ogrn", data_app.smo_ogrn);
    settings->setValue("smo_regnum", data_app.smo_regnum);
    settings->setValue("smo_logo1", data_app.smo_logo1.toUtf8());
    settings->setValue("smo_logo2", data_app.smo_logo2.toUtf8());
    settings->setValue("smo_logo3", data_app.smo_logo3.toUtf8());
    settings->setValue("smo_short", data_app.smo_short.toUtf8());
    settings->setValue("smo_name", data_app.smo_name.toUtf8());
    settings->setValue("smo_name_full", data_app.smo_name_full.toUtf8());
    settings->setValue("smo_director", data_app.smo_director.toUtf8());
    settings->setValue("smo_oms_chif", data_app.smo_oms_chif.toUtf8());
    settings->setValue("smo_name_full_rp", data_app.smo_name_full_rp.toUtf8());
    settings->setValue("smo_address", data_app.smo_address.toUtf8());
    settings->setValue("smo_contact_info", data_app.smo_contact_info.toUtf8());
    settings->setValue("point_regnum", data_app.point_regnum);
    settings->setValue("point_name", data_app.point_name.toUtf8());
    settings->setValue("operator_fio", ui->combo_operator->currentText().toUtf8());
    settings->setValue("point_signer", data_app.point_signer.toUtf8());
    settings->endGroup();
    // [smo]
    settings->beginGroup("smo");
    settings->setValue("msg_timeout_back",  data_app.msg_timeout_back);
    settings->setValue("msg_timeout_front", data_app.msg_timeout_front);
    settings->endGroup();
    // [filial]
    settings->beginGroup("filial");
    settings->setValue("terr_name",           data_app.terr_name);
    settings->setValue("terr_name_rp",        data_app.terr_name_rp);
    settings->setValue("tfoms_director",      data_app.tfoms_director);
    settings->setValue("tfoms_director_2",    data_app.tfoms_director_2);
    settings->setValue("tfoms_director_fio",  data_app.tfoms_director_fio);
    settings->setValue("tfoms_chif",          data_app.tfoms_chif);
    settings->setValue("filial_name",         data_app.filial_name);
    settings->setValue("filial_name_rp",      data_app.filial_name_rp);
    settings->setValue("filial_city",         data_app.filial_city);
    settings->setValue("filial_by_city",      data_app.filial_by_city);
    settings->setValue("filial_by_city_tp",   data_app.filial_by_city_tp);
    settings->setValue("filial_director",     data_app.filial_director);
    settings->setValue("filial_director_2",   data_app.filial_director_2);
    settings->setValue("filial_director_fio", data_app.filial_director_fio);
    settings->setValue("filial_chif",         data_app.filial_chif);
    settings->setValue("rep_doer",            data_app.rep_doer);
    settings->setValue("filial_phone",        data_app.filial_phone);
    settings->setValue("filial_phone_2",      data_app.filial_phone_2);
    settings->setValue("filial_email",        data_app.filial_email);
    settings->setValue("filial_email_2",      data_app.filial_email_2);
    settings->endGroup();
    // [assig]
    settings->beginGroup("assig");
    settings->setValue("assig_vers", data_app.assig_vers);
    settings->setValue("csv_headers", data_app.assig_csv_headers);
    settings->endGroup();
    // [log]
    settings->beginGroup("log");
    settings->setValue("log_bord", data_app.log_bord);
    settings->endGroup();
    // [xml]
    settings->beginGroup("xml");
    settings->setValue("xml_vers", data_app.xml_vers);
    settings->setValue("snils_format", QString::number(data_app.snils_format));
    settings->setValue("split_by_ot", QString::number(data_app.split_by_ot));
    settings->setValue("send_p02_in_p06", QString::number(data_app.send_p02_in_p06));
    settings->setValue("pol_start_date_by_vs", QString::number(data_app.pol_start_date_by_vs));
    settings->setValue("zip_packs", data_app.f_zip_packs);
    settings->endGroup();
    // [VS]
    settings->beginGroup("VS");
    //settings->setValue("inko_name", data_app.vs_inko_name);
    //settings->setValue("inko_address", data_app.vs_inko_address);
    settings->setValue("blank_width_mm", QString::number(data_app.vs_blank_width_mm));
    settings->setValue("blank_height_mm", QString::number(data_app.vs_blank_height_mm));
    settings->setValue("field_left_mm", QString::number(data_app.vs_field_left_mm));
    settings->setValue("field_right_mm", QString::number(data_app.vs_field_right_mm));
    settings->setValue("field_top_mm", QString::number(data_app.vs_field_top_mm));
    settings->setValue("field_bottom_mm", QString::number(data_app.vs_field_bottom_mm));
    settings->endGroup();

    // [form8]
    settings->beginGroup("form8");
    settings->setValue("doc1_num",  data_app.form8_doc1_num);
    settings->setValue("doc1_date", data_app.form8_doc1_date.toString("yyyy-MM-dd"));
    settings->setValue("has_doc2",  data_app.form8_has_doc2 ? "1" : "0");
    settings->setValue("doc2_num",  data_app.form8_doc2_num);
    settings->setValue("doc2_date", data_app.form8_doc2_date.toString("yyyy-MM-dd"));
    settings->setValue("codename1", data_app.form8_codename1);
    settings->setValue("codename2", data_app.form8_codename2);
    settings->setValue("codename3", data_app.form8_codename3);
    settings->setValue("code1",     data_app.form8_code1);
    settings->setValue("code2",     data_app.form8_code2);
    settings->setValue("code3",     data_app.form8_code3);
    settings->setValue("tfoms_worker",        data_app.tfoms_worker);
    settings->setValue("tfoms_worker2_place", data_app.tfoms_worker2_place);
    settings->setValue("tfoms_worker2_fio",   data_app.tfoms_worker2_fio);
    settings->setValue("tfoms_worker2_phone", data_app.tfoms_worker2_phone);
    settings->endGroup();

    // [barcode]
    settings->beginGroup("barcodes");
    settings->setValue("portname", data_app.barcodes_portname);
    settings->setValue("file",     data_app.barcodes_file);
    settings->endGroup();


    // [blank_acts]
    settings->beginGroup("blank_acts");
    settings->setValue("dog_num",            data_app.act_dog_num );
    settings->setValue("dog_date",           data_app.act_dog_date.toString("yyyy-MM-dd"));
    settings->setValue("filial_chif_post",   data_app.act_filial_chif_post );
    settings->setValue("filial_chif_fio",    data_app.act_filial_chif_fio );
    settings->setValue("point_chif_post",    data_app.act_point_chif_post );
    settings->setValue("point_chif_fio",     data_app.act_point_chif_fio );

    settings->setValue("point_org_name",     data_app.act_point_org_name );
    settings->setValue("point_address",      data_app.act_point_address );
    settings->setValue("point_requisits_1",  data_app.act_point_requisits_1 );
    settings->setValue("point_requisits_2",  data_app.act_point_requisits_2 );
    settings->setValue("point_okpo",         data_app.act_point_okpo );

    settings->setValue("filial_org_name",    data_app.act_filial_org_name );
    settings->setValue("filial_address",     data_app.act_filial_address );
    settings->setValue("filial_requisits_1", data_app.act_filial_requisits_1 );
    settings->setValue("filial_requisits_2", data_app.act_filial_requisits_2 );
    settings->setValue("filial_okpo",        data_app.act_filial_okpo );

    settings->setValue("base_doc_name",      data_app.act_base_doc_name );
    settings->setValue("base_doc_num",       data_app.act_base_doc_num );
    settings->setValue("base_doc_date",      data_app.act_base_doc_date.toString("yyyy-MM-dd"));
    settings->setValue("act_num",            data_app.act_num );
    settings->setValue("act_date",           data_app.act_date.toString("yyyy-MM-dd"));
    settings->setValue("act_count",          data_app.act_count );
    settings->setValue("act_tabel_num",      data_app.act_tabel_num );
    settings->endGroup();


    // [filials]
    settings->beginGroup("filials");
    settings->setValue("filials_list", data_app.filials_list);
    settings->endGroup();
    if (data_app.filials_list.indexOf("kursk")>=0) {
        // [kursk]
        settings->beginGroup("kursk");
        settings->setValue("host", data_app.kursk_host);
        settings->setValue("port", QString::number(data_app.kursk_port));
        settings->setValue("base", data_app.kursk_base);
        settings->setValue("user", data_app.kursk_user);
        // зашифруем пароль
        QString pass, pass2 = data_app.kursk_pass;
        mySQL.myBYTEA2(pass2, pass);
        settings->setValue("pass", pass);
        settings->endGroup();
    }
    if (data_app.filials_list.indexOf("belg")>=0) {
        // [belg]
        settings->beginGroup("belg");
        settings->setValue("host", data_app.belg_host);
        settings->setValue("port", QString::number(data_app.belg_port));
        settings->setValue("base", data_app.belg_base);
        settings->setValue("user", data_app.belg_user);
        // зашифруем пароль
        QString pass, pass2 = data_app.belg_pass;
        mySQL.myBYTEA2(pass2, pass);
        settings->setValue("pass", pass);
        settings->endGroup();
    }
    if (data_app.filials_list.indexOf("voro")>=0) {
        // [voro]
        settings->beginGroup("voro");
        settings->setValue("host", data_app.voro_host);
        settings->setValue("port", QString::number(data_app.voro_port));
        settings->setValue("base", data_app.voro_base);
        settings->setValue("user", data_app.voro_user);
        // зашифруем пароль
        QString pass, pass2 = data_app.voro_pass;
        mySQL.myBYTEA2(pass2, pass);
        settings->setValue("pass", pass);
        settings->endGroup();
    }
    if (data_app.filials_list.indexOf("test")>=0) {
        // [test]
        settings->beginGroup("test");
        settings->setValue("host", data_app.test_host);
        settings->setValue("port", QString::number(data_app.test_port));
        settings->setValue("base", data_app.test_base);
        settings->setValue("user", data_app.test_user);
        // зашифруем пароль
        QString pass, pass2 = data_app.test_pass;
        mySQL.myBYTEA2(pass2, pass);
        settings->setValue("pass", pass);
        settings->endGroup();
    }
    if (data_app.filials_list.indexOf("work")>=0) {
        // [work]
        settings->beginGroup("work");
        settings->setValue("host", data_app.work_host);
        settings->setValue("port", QString::number(data_app.work_port));
        settings->setValue("base", data_app.work_base);
        settings->setValue("user", data_app.work_user);
        // зашифруем пароль
        QString pass, pass2 = data_app.work_pass;
        mySQL.myBYTEA2(pass2, pass);
        settings->setValue("pass", pass);
        settings->endGroup();
    }


    settings->sync();

    // TEMP
    QDir p_temp(data_app.path_temp);
    if (!p_temp.exists()) p_temp.mkpath(data_app.path_temp);
    // FOTO
    QDir p_foto(data_app.path_foto);
    if (!p_foto.exists()) p_foto.mkpath(data_app.path_foto);
    // SIGN
    QDir p_sign(data_app.path_sign);
    if (!p_sign.exists()) p_sign.mkpath(data_app.path_sign);
    // IN
    QDir p_in(data_app.path_in);
    if (!p_in.exists()) p_in.mkpath(data_app.path_in);
    // OUT
    QDir p_out(data_app.path_out);
    if (!p_out.exists()) p_out.mkpath(data_app.path_out);
    QDir p_out_vs(data_app.path_out + "/_POL_VS_");
    if (!p_out_vs.exists()) p_out_vs.mkpath(data_app.path_out + "/_POL_VS_");
    QDir p_out_notices(data_app.path_out + "/_POL_NOTICES_");
    if (!p_out_notices.exists()) p_out_notices.mkpath(data_app.path_out + "/_POL_NOTICES_");
    QDir p_out_assig_data(data_app.path_out + "/_ASSIG_DATA_");
    if (!p_out_assig_data.exists()) p_out_assig_data.mkpath(data_app.path_out + "/_ASSIG_DATA_");
    QDir p_out_reports(data_app.path_out + "/_REPORTS_");
    if (!p_out_reports.exists()) p_out_reports.mkpath(data_app.path_out + "/_REPORTS_");
    QDir p_out_registry(data_app.path_out + "/_REGISTRY_");
    if (!p_out_registry.exists()) p_out_registry.mkpath(data_app.path_out + "/_REGISTRY_");
    QDir p_out_trancfer_acts(data_app.path_out + "/_TRANSFER_ACTS_");
    if (!p_out_trancfer_acts.exists()) p_out_trancfer_acts.mkpath(data_app.path_out + "/_TRANSFER_ACTS_");
    QDir p_out_statements(data_app.path_out + "/_STATEMENTS_");
    if (!p_out_statements.exists()) p_out_statements.mkpath(data_app.path_out + "/_STATEMENTS_");
    // DBF
    QDir p_dbf(data_app.path_dbf);
    if (!p_dbf.exists()) p_dbf.mkpath(data_app.path_dbf);
    // ACTS
    QDir p_acts(data_app.path_acts);
    if (!p_acts.exists()) p_acts.mkpath(data_app.path_acts);
    // TFOMS
    QDir p_to_TFOMS(data_app.path_to_TFOMS);
    if (!p_to_TFOMS.exists()) p_to_TFOMS.mkpath(data_app.path_to_TFOMS);
    QDir p_from_TFOMS(data_app.path_from_TFOMS);
    if (!p_from_TFOMS.exists()) p_from_TFOMS.mkpath(data_app.path_from_TFOMS);
    QDir p_from_TFOMS_backup(data_app.path_from_TFOMS + "/backup");
    if (!p_from_TFOMS_backup.exists()) p_from_TFOMS_backup.mkpath(data_app.path_from_TFOMS + "/backup");
    // OUT_BACKUP
    QDir p_out_backup(data_app.path_out + "/backup");
    if (!p_out_backup.exists()) p_out_backup.mkpath(data_app.path_out + "/backup");
    // INSTALL
    QDir p_install(data_app.path_install);
    if (!p_install.exists()) p_install.mkpath(data_app.path_install);
    // ARCH
    QDir p_arch(data_app.path_arch);
    if (!p_arch.exists()) QMessageBox::warning(this, "Отсутствует модуль!", "ВНИМАНИЕ!\nОтстутствует предопределённый путь к папке модуля архивирования:\n" + data_app.path_arch);
    // REPORTS
    QDir p_reports(data_app.path_reports);
    if (!p_reports.exists()) QMessageBox::warning(this, "Отсутствует модуль!", "ВНИМАНИЕ!\nОтстутствует предопределённый путь к папке шаблонов отчётов:\n" + data_app.path_reports);
    // LOG
    QDir p_log(data_app.path_install + "/_LOG_");
    if (!p_log.exists()) p_log.mkpath(data_app.path_install + "/_LOG_");
    // диспансеризация
    QDir p_clinic_examination(data_app.path_out + "/_ДИСПАНСЕРИЗАЦИЯ_");
    if (!p_clinic_examination.exists()) p_clinic_examination.mkpath(data_app.path_out + "/_ДИСПАНСЕРИЗАЦИЯ_");
}

void ConnectionWindow::refresh_points() {
    this->setCursor(Qt::WaitCursor);

    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "";
    sql += QString("select p.id, '('||p.point_regnum||') '||p.point_name as name"
                   " from points p "
                   " where status>0 "
                   " order by p.point_regnum, id ; ");
    query->exec( sql );
    ui->combo_point->clear();

    // заполнение списка пунктов выдачи
    while (query->next()) {
        ui->combo_point->addItem( query->value(1).toString(),
                                  query->value(0).toInt() );
    }
    query->finish();
    delete query;

    // восстановим прежнее значение
    if (settings->value("resent/point", "").toString()!="") {
        ui->combo_point->setCurrentIndex(ui->combo_point->findText(QString::fromUtf8(settings->value("resent/point", " ").toByteArray())));
    } else {
        ui->combo_point->setCurrentIndex(0);
    }

    this->setCursor(Qt::ArrowCursor);

    refresh_operators(ui->combo_point->currentData().toInt());
}

void ConnectionWindow::refresh_operators(int id_point) {
    this->setCursor(Qt::WaitCursor);

    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "";
    sql += "select u.id, u.oper_fio ";
    sql += " from operators u "
           " where u.status>0 ";

    // фильтрация по ПВП только если надо
    if (ui->ch_oper_by_point->isChecked())
        sql += (id_point==-1) ? (QString(" ")) : (QString(" and u.id_point=") + QString::number(id_point));

    sql += " order by u.oper_fio ; ";
    query->exec( sql );
    ui->combo_operator->clear();

    // заполнение списка операторов
    while (query->next()) {
        ui->combo_operator->addItem( query->value(1).toString(),
                                     query->value(0).toInt() );
    }
    query->finish();
    delete query;

    // восстановим прежнее значение
    if (settings->value("resent/operator", "").toString()!="") {
        QString oper = QString::fromUtf8(settings->value("resent/operator", " ").toByteArray());
        int n = ui->combo_operator->findText(oper);
        ui->combo_operator->setCurrentIndex(n);
    } else {
        ui->combo_operator->setCurrentIndex(0);
    }

    this->setCursor(Qt::ArrowCursor);
}

void ConnectionWindow::on_tc_pass_returnPressed() {
    on_bn_save_DB_params_clicked();
}

void ConnectionWindow::on_bn_save_DB_params_clicked() {
    data_app.host = ui->tc_host->text();
    data_app.port = ui->spin_port->value();
    data_app.base = ui->tc_base->text();
    data_app.user = ui->tc_user->text();
    data_app.pass = ui->tc_pass->text();

    // сохраним параметры подключения к базе обновлений
    settings->beginGroup("polisesDB");
    settings->setValue("host", data_app.host);
    settings->setValue("port", data_app.port);
    settings->setValue("base", data_app.base);
    settings->setValue("user", data_app.user);
    // зашифруем пароль
    QString pass, pass2 = data_app.pass;
    mySQL.myBYTEA2(pass2, pass);
    settings->setValue("pass", pass);
    settings->endGroup();

    // автоправка данных региона по имени базы данных
    if (data_app.base=="inko_polises_voronej") {
        settings->beginGroup("smo");
        data_app.ocato = "20000";
        settings->setValue("terr_ocato", data_app.ocato);
        data_app.ocato_text = "Воронежская область";
        settings->setValue("terr_ocato_text", data_app.ocato_text);
        settings->endGroup();

        settings->beginGroup("filial");
        data_app.terr_name = "Воронежская область";
        settings->setValue("terr_name", data_app.terr_name);
        data_app.terr_name_rp = "Воронежской области";
        settings->setValue("terr_name_rp", data_app.terr_name_rp);
        settings->endGroup();
    }

    if (data_app.base=="inko_polises_belgorod") {
        settings->beginGroup("smo");
        data_app.ocato = "14000";
        settings->setValue("terr_ocato", data_app.ocato);
        data_app.ocato_text = "Белгородская область";
        settings->setValue("terr_ocato_text", data_app.ocato_text);
        settings->endGroup();

        settings->beginGroup("filial");
        data_app.terr_name = "Белгородская область";
        settings->setValue("terr_name", data_app.terr_name);
        data_app.terr_name_rp = "Белгородской области";
        settings->setValue("terr_name_rp", data_app.terr_name_rp);
        settings->endGroup();
    }

    if (data_app.base=="inko_polises_kursk") {
        settings->beginGroup("smo");
        data_app.ocato = "38000";
        settings->setValue("terr_ocato", data_app.ocato);
        data_app.ocato_text = "Курская область";
        settings->setValue("terr_ocato_text", data_app.ocato_text);
        settings->endGroup();

        settings->beginGroup("filial");
        data_app.terr_name = "Курская область";
        settings->setValue("terr_name", data_app.terr_name);
        data_app.terr_name_rp = "Курской области";
        settings->setValue("terr_name_rp", data_app.terr_name_rp);
        settings->endGroup();
    }

    settings->sync();

    // обновим подключение к базе обновлений
    refresh_connection();
}

void ConnectionWindow::on_combo_point_activated(int index) {
    refresh_operators(ui->combo_point->itemData(ui->combo_point->currentIndex()).toInt());
    data_app.point_name = ui->combo_point->currentText();
    QString name = ui->combo_point->currentText();
    data_app.point_regnum = QString("000" + QString(name.mid(name.indexOf("(")+1, name.indexOf(")")-name.indexOf("(")-1))).right(3);
}

void ConnectionWindow::on_tc_op_pass_returnPressed() {
    on_bn_start_clicked();
}

#include "persons_wnd.h"

void ConnectionWindow::on_bn_start_clicked() {
    // проверим пароль оператора
    QSqlQuery *query = new QSqlQuery(db);
    QString sql = "";
    sql += QString("select u.id, u.user_pass, u.oper_rights "
                   " from operators u "
                   " where u.id=");
    sql += QString::number(ui->combo_operator->currentData().toInt()) + " ; ";
    query->exec( sql );
    query->next();
    QString pass = query->value(1).toString();
    int rights = query->value(2).toInt();
    if (rights<1 || rights>4)
        rights = 4;

    if (pass==ui->tc_op_pass->text()) {
        ui->tc_op_pass->setText("");

        // получим данне филиала
        QString sql, s;
        QSqlQuery query(db);

        //QString smo_ogrn;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='smo_ogrn' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.smo_ogrn = s;
        //QString smo_regnum;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='smo_regnum' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.smo_regnum = s;
        //QString smo_short;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='smo_short' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.smo_short = s;
        //QString smo_name;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='smo_name' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.smo_name = s;
        //QString smo_name_full;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='smo_name_full' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.smo_name_full = s;
        //QString smo_name_full_rp;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='smo_name_full_rp' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.smo_name_full_rp = s;
        //QString smo_director;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='smo_director' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.smo_director = s;
        //QString smo_oms_chif;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='smo_oms_chif' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.smo_oms_chif = s;
        //QString smo_address;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='smo_address' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.smo_address = s;
        //QString smo_contact_info;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='smo_contact_info' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.smo_contact_info = s;
        //QString smo_logo1;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='smo_logo1' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.smo_logo1 = s;
        //QString smo_logo2;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='smo_logo2' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.smo_logo2 = s;
        //QString smo_logo3;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='smo_logo3' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.smo_logo3 = s;
        //QString terr_name;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='terr_name' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.terr_name = s;
        //QString terr_name_rp;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='terr_name_rp' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.terr_name_rp = s;
        //QString tfoms_director;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='tfoms_director' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.tfoms_director = s;
        //QString tfoms_director_2;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='tfoms_director_2' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.tfoms_director_2 = s;
        //QString tfoms_director_fio;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='tfoms_director_fio' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.tfoms_director_fio = s;
        //QString tfoms_chif;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='tfoms_chif' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.tfoms_chif = s;
        //QString filial_name;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='filial_name' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.filial_name = s;
        //QString filial_name_rp;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='filial_name_rp' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.filial_name_rp = s;
        //QString filial_city;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='filial_city' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.filial_city = s;
        //QString filial_by_city;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='filial_by_city' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.filial_by_city = s;
        //QString filial_by_city_tp;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='filial_by_city_tp' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.filial_by_city_tp = s;
        //QString filial_director;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='filial_director' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.filial_director = s;
        //QString filial_director_2;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='filial_director_2' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.filial_director_2 = s;
        //QString filial_director_fio;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='filial_director_fio' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.filial_director_fio = s;
        //QString filial_chif;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='filial_chif' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.filial_chif = s;
        //QString filial_phone;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='filial_phone' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.filial_phone = s;
        //QString filial_phone_2;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='filial_phone_2' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.filial_phone_2 = s;
        //QString filial_email;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='filial_email' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.filial_email = s;
        //QString filial_email_2;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='filial_email_2' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.filial_email_2 = s;
        //QString ocato;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='terr_ocato' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.ocato = s;
        //QString ocato_text;
        sql = "select d.name, d.\"value\" "
              "  from db_data d  "
              " where d.name='terr_ocato_text' ; ";
        s = "";
        query.exec(sql);
        query.next();
        s = query.value(1).toString().trimmed();
        data_app.ocato_text = s;




        // [polisesDB]
        settings->beginGroup("polisesDB");
        settings->setValue("host", data_app.host);
        settings->setValue("port", data_app.port);
        settings->setValue("base", data_app.base);
        settings->setValue("user", data_app.user);
        // зашифруем пароль
        QString pass, pass2 = data_app.pass;
        mySQL.myBYTEA2(pass2, pass);
        settings->setValue("pass", pass);
        settings->endGroup();
        // [scoresDB]
        settings->beginGroup("scoresDB");
        settings->setValue("scores_host", data_app.scores_host);
        settings->setValue("scores_port", data_app.scores_port);
        settings->setValue("scores_base", data_app.scores_base);
        settings->setValue("scores_user", data_app.scores_user);
        settings->setValue("scores_pass", data_app.scores_pass);
        settings->endGroup();
        // [flags]
        data_app.is_tech = (rights==1);
        data_app.is_head = (rights==2);
        data_app.is_locl = (rights==3);
        data_app.is_call = (rights==4);
        settings->beginGroup("flags");
        settings->setValue("is_tech", data_app.is_tech);
        settings->setValue("is_head", data_app.is_head);
        settings->setValue("is_locl", data_app.is_locl);
        settings->setValue("is_call", data_app.is_call);
        settings->setValue("gen_regVS", data_app.f_gen_regVS);
        settings->setValue("gen_regPol", data_app.f_gen_regPol);
        settings->setValue("send_to_fond_xmls", data_app.f_send2fond_xmls);
        settings->setValue("send_to_fond_zips", data_app.f_send2fond_zips);
        settings->setValue("send_to_fond_1ZIP", data_app.f_send2fond_1ZIP);
        settings->setValue("send_to_fond_new_IM", data_app.f_send2fond_new_IM);
        settings->setValue("send_to_fond_new_OT", data_app.f_send2fond_new_OT);
        settings->setValue("send_to_fond_new_assigs", data_app.f_send2fond_new_assigs);
        settings->setValue("use_folders", data_app.use_folders);
        settings->setValue("default_folder", data_app.default_folder.toUtf8());
        settings->endGroup();
        // [paths]
        settings->beginGroup("paths");
        settings->setValue("path_install", data_app.path_install.toUtf8());
        settings->setValue("path_arch", data_app.path_arch.toUtf8());
        settings->setValue("path_reports", data_app.path_reports.toUtf8());
        settings->setValue("path_temp", data_app.path_temp.toUtf8());
        settings->setValue("path_foto", data_app.path_foto.toUtf8());
        settings->setValue("path_sign", data_app.path_sign.toUtf8());
        settings->setValue("path_in", data_app.path_in.toUtf8());
        settings->setValue("path_out", data_app.path_out.toUtf8());
        settings->setValue("path_dbf", data_app.path_dbf.toUtf8());
        settings->setValue("path_acts", data_app.path_acts.toUtf8());
        settings->setValue("path_to_TFOMS", data_app.path_to_TFOMS.toUtf8());
        settings->setValue("path_from_TFOMS", data_app.path_from_TFOMS.toUtf8());
        //settings->setValue("path_in_backup", data_app.path_in_backup.toUtf8());
        //settings->setValue("path_out_backup", data_app.path_out_backup.toUtf8());
        settings->endGroup();
        // [resent]
        settings->beginGroup("resent");
        settings->setValue("point", ui->combo_point->currentText().toUtf8());
        settings->setValue("operator", ui->combo_operator->currentText().toUtf8());
        settings->endGroup();
        // [smo]
        settings->beginGroup("smo");
        settings->setValue("terr_ocato", data_app.ocato);
        settings->setValue("terr_ocato_text", data_app.ocato_text.toUtf8());
        settings->setValue("smo_ogrn", data_app.smo_ogrn);
        settings->setValue("smo_regnum", data_app.smo_regnum);
        settings->setValue("smo_logo1", data_app.smo_logo1.toUtf8());
        settings->setValue("smo_logo2", data_app.smo_logo2.toUtf8());
        settings->setValue("smo_logo3", data_app.smo_logo3.toUtf8());
        settings->setValue("smo_short", data_app.smo_short.toUtf8());
        settings->setValue("smo_name", data_app.smo_name.toUtf8());
        settings->setValue("point_regnum", data_app.point_regnum.toUtf8());
        settings->setValue("smo_name_full", data_app.smo_name_full.toUtf8());
        settings->setValue("smo_name_full_rp", data_app.smo_name_full_rp.toUtf8());
        settings->setValue("smo_director", data_app.smo_director.toUtf8());
        settings->setValue("smo_oms_chif", data_app.smo_oms_chif.toUtf8());
        settings->setValue("smo_address", data_app.smo_address.toUtf8());
        settings->setValue("smo_contact_info", data_app.smo_contact_info.toUtf8());
        settings->setValue("point_name", data_app.point_name.toUtf8());
        settings->setValue("operator_fio", data_app.operator_fio.toUtf8());
        settings->endGroup();
        // [smo]
        settings->beginGroup("smo");
        settings->setValue("msg_timeout_back",  data_app.msg_timeout_back);
        settings->setValue("msg_timeout_front", data_app.msg_timeout_front);
        settings->endGroup();
        // [assig]
        settings->beginGroup("assig");
        settings->setValue("assig_vers", data_app.assig_vers);
        settings->setValue("csv_headers", data_app.assig_csv_headers);
        settings->endGroup();
        // [log]
        settings->beginGroup("log");
        settings->setValue("log_bord", data_app.log_bord);
        settings->endGroup();
        // [filial]
        settings->beginGroup("filial");
        settings->setValue("terr_name",          data_app.terr_name);
        settings->setValue("terr_name_rp",       data_app.terr_name_rp);
        settings->setValue("tfoms_director",     data_app.tfoms_director);
        settings->setValue("tfoms_director_2",   data_app.tfoms_director_2);
        settings->setValue("tfoms_director_fio", data_app.tfoms_director_fio);
        settings->setValue("tfoms_chif",         data_app.tfoms_chif);
        settings->setValue("filial_name",        data_app.filial_name);
        settings->setValue("filial_name_rp",     data_app.filial_name_rp);
        settings->setValue("filial_city",        data_app.filial_city);
        settings->setValue("filial_by_city",     data_app.filial_by_city);
        settings->setValue("filial_by_city_tp",  data_app.filial_by_city_tp);
        settings->setValue("filial_director",    data_app.filial_director);
        settings->setValue("filial_director_2",  data_app.filial_director_2);
        settings->setValue("filial_director_fio",data_app.filial_director_fio);
        settings->setValue("filial_chif",        data_app.filial_chif);
        settings->setValue("rep_doer",           data_app.rep_doer);
        settings->setValue("filial_phone",       data_app.filial_phone);
        settings->setValue("filial_phone_2",     data_app.filial_phone_2);
        settings->setValue("filial_email",       data_app.filial_email);
        settings->setValue("filial_email_2",     data_app.filial_email_2);
        settings->endGroup();
        // [xml]
        settings->beginGroup("xml");
        settings->setValue("xml_vers", data_app.xml_vers);
        settings->setValue("snils_format", QString::number(data_app.snils_format));
        settings->setValue("split_by_ot", QString::number(data_app.split_by_ot));
        settings->setValue("send_p02_in_p06", QString::number(data_app.send_p02_in_p06));
        settings->setValue("pol_start_date_by_vs", QString::number(data_app.pol_start_date_by_vs));
        settings->setValue("zip_packs", data_app.f_zip_packs);
        settings->endGroup();
        // [VS]
        settings->beginGroup("VS");
        settings->setValue("blank_width_mm", QString::number(data_app.vs_blank_width_mm));
        settings->setValue("blank_height_mm", QString::number(data_app.vs_blank_height_mm));
        settings->setValue("field_left_mm", QString::number(data_app.vs_field_left_mm));
        settings->setValue("field_right_mm", QString::number(data_app.vs_field_right_mm));
        settings->setValue("field_top_mm", QString::number(data_app.vs_field_top_mm));
        settings->setValue("field_bottom_mm", QString::number(data_app.vs_field_bottom_mm));
        settings->endGroup();

        // [form8]
        settings->beginGroup("form8");
        settings->setValue("doc1_num",  data_app.form8_doc1_num);
        settings->setValue("doc1_date", data_app.form8_doc1_date.toString("yyyy-MM-dd"));
        settings->setValue("has_doc2",  data_app.form8_has_doc2 ? "1" : "0");
        settings->setValue("doc2_num",  data_app.form8_doc2_num);
        settings->setValue("doc2_date", data_app.form8_doc2_date.toString("yyyy-MM-dd"));
        settings->setValue("codename1", data_app.form8_codename1);
        settings->setValue("codename2", data_app.form8_codename2);
        settings->setValue("codename3", data_app.form8_codename3);
        settings->setValue("code1",     data_app.form8_code1);
        settings->setValue("code2",     data_app.form8_code2);
        settings->setValue("code3",     data_app.form8_code3);
        settings->setValue("tfoms_worker",        data_app.tfoms_worker);
        settings->setValue("tfoms_worker2_place", data_app.tfoms_worker2_place);
        settings->setValue("tfoms_worker2_fio",   data_app.tfoms_worker2_fio);
        settings->setValue("tfoms_worker2_phone", data_app.tfoms_worker2_phone);
        settings->endGroup();

        // [barcode]
        settings->beginGroup("barcode");
        settings->setValue("portname", data_app.barcodes_portname);
        settings->setValue("file", data_app.barcodes_file);
        settings->endGroup();


        // [blank_acts]
        settings->beginGroup("blank_acts");
        settings->setValue("dog_num",            data_app.act_dog_num );
        settings->setValue("dog_date",           data_app.act_dog_date.toString("yyyy-MM-dd"));
        settings->setValue("filial_chif_post",   data_app.act_filial_chif_post );
        settings->setValue("filial_chif_fio",    data_app.act_filial_chif_fio );
        settings->setValue("point_chif_post",    data_app.act_point_chif_post );
        settings->setValue("point_chif_fio",     data_app.act_point_chif_fio );

        settings->setValue("point_org_name",     data_app.act_point_org_name );
        settings->setValue("point_address",      data_app.act_point_address );
        settings->setValue("point_requisits_1",  data_app.act_point_requisits_1 );
        settings->setValue("point_requisits_2",  data_app.act_point_requisits_2 );
        settings->setValue("point_okpo",         data_app.act_point_okpo );

        settings->setValue("filial_org_name",    data_app.act_filial_org_name );
        settings->setValue("filial_address",     data_app.act_filial_address );
        settings->setValue("filial_requisits_1", data_app.act_filial_requisits_1 );
        settings->setValue("filial_requisits_2", data_app.act_filial_requisits_2 );
        settings->setValue("filial_okpo",        data_app.act_filial_okpo );

        settings->setValue("base_doc_name",      data_app.act_base_doc_name );
        settings->setValue("base_doc_num",       data_app.act_base_doc_num );
        settings->setValue("base_doc_date",      data_app.act_base_doc_date.toString("yyyy-MM-dd"));
        settings->setValue("act_num",            data_app.act_num );
        settings->setValue("act_date",           data_app.act_date.toString("yyyy-MM-dd"));
        settings->setValue("act_count",          data_app.act_count );
        settings->setValue("act_tabel_num",      data_app.act_tabel_num );
        settings->endGroup();


        // [filials]
        settings->beginGroup("filials");
        settings->setValue("filials_list", data_app.filials_list);
        settings->endGroup();
        if (data_app.filials_list.indexOf("kursk")>=0) {
            // [kursk]
            settings->beginGroup("kursk");
            settings->setValue("host", data_app.kursk_host);
            settings->setValue("port", QString::number(data_app.kursk_port));
            settings->setValue("base", data_app.kursk_base);
            //settings->setValue("user", data_app.kursk_user);
            settings->setValue("user", "kursk_user");
            // зашифруем пароль
            QString pass, pass2 = "50uzb@$_2";
            //QString pass, pass2 = data_app.kursk_pass;
            mySQL.myBYTEA2(pass2, pass);
            settings->setValue("pass", pass);
            settings->endGroup();
        }
        if (data_app.filials_list.indexOf("belg")>=0) {
            // [belg]
            settings->beginGroup("belg");
            settings->setValue("host", data_app.belg_host);
            settings->setValue("port", QString::number(data_app.belg_port));
            settings->setValue("base", data_app.belg_base);
            //settings->setValue("user", data_app.belg_user);
            settings->setValue("user", "belg_user");
            // зашифруем пароль
            QString pass, pass2 = "50ub@_4wr";
            //QString pass, pass2 = data_app.belg_pass;
            mySQL.myBYTEA2(pass2, pass);
            settings->setValue("pass", pass);
            settings->endGroup();
        }
        if (data_app.filials_list.indexOf("voro")>=0) {
            // [voro]
            settings->beginGroup("voro");
            settings->setValue("host", data_app.voro_host);
            settings->setValue("port", QString::number(data_app.voro_port));
            settings->setValue("base", data_app.voro_base);
            //settings->setValue("user", data_app.voro_user);
            settings->setValue("user", "voro_user");
            // зашифруем пароль
            QString pass, pass2 = "50uzb@$_1";
            //QString pass, pass2 = data_app.voro_pass;
            mySQL.myBYTEA2(pass2, pass);
            settings->setValue("pass", pass);
            settings->endGroup();
        }
        if (data_app.filials_list.indexOf("test")>=0) {
            // [test]
            settings->beginGroup("test");
            settings->setValue("host", data_app.test_host);
            settings->setValue("port", QString::number(data_app.test_port));
            settings->setValue("base", data_app.test_base);
            settings->setValue("user", data_app.test_user);
            // зашифруем пароль
            QString pass, pass2 = data_app.test_pass;
            mySQL.myBYTEA2(pass2, pass);
            settings->setValue("pass", pass);
            settings->endGroup();
        }
        if (data_app.filials_list.indexOf("work")>=0) {
            // [work]
            settings->beginGroup("work");
            settings->setValue("host", data_app.work_host);
            settings->setValue("port", QString::number(data_app.work_port));
            settings->setValue("base", data_app.work_base);
            settings->setValue("user", data_app.work_user);
            // зашифруем пароль
            QString pass, pass2 = data_app.work_pass;
            mySQL.myBYTEA2(pass2, pass);
            settings->setValue("pass", pass);
            //settings->setValue("pass", data_app.work_pass);
            settings->endGroup();
        }

        settings->sync();

        data_app.id_point = ui->combo_point->currentData().toInt();
        data_app.id_operator = ui->combo_operator->currentData().toInt();
        data_app.operator_fio = ui->combo_operator->currentText();
        data_app.font_size = 8;

        // уточним параметры ПВП
        QString sql_point_rights = "select id, point_name, point_regnum, point_rights, status, address "
                                   "  from points "
                                   " where id=" + QString::number(data_app.id_point) + " ; ";
        QSqlQuery *query_point_rights = new QSqlQuery(db);
        bool res_point_rights = mySQL.exec(this, sql_point_rights, "уточним параметры ПВП ", *query_point_rights, true, db, data_app);

        if (res_point_rights && query_point_rights->next()) {
            data_app.point_name    = query_point_rights->value(1).toString();
            data_app.point_regnum  = query_point_rights->value(2).toString();
            data_app.point_rights  = query_point_rights->value(3).toInt();
            data_app.point_status  = query_point_rights->value(4).toInt();
            data_app.point_address = query_point_rights->value(5).toString();
        } else {
            QMessageBox::warning(this, "Ошибка чтения данных ПВП",
                                 "При попытке чтения данных выбранного ПВП произошла ошибка.\n"
                                 "Операция отменена.\n"
                                 "Дальнейшая работа невозможна.");
        }

        // обновление данных сессии
        QString sql_upd_transaction = "update db_sessions "
                                      "   set status=1,"
                                      "       id_point=" + QString::number(data_app.id_point) + ", "
                                      "       id_operator=" + QString::number(data_app.id_operator) + " "
                                      " where id=" + QString::number(data_app.id_session) + " ; ";
        QSqlQuery *query_upd_transaction = new QSqlQuery(db);
        bool res_upd_transaction = mySQL.exec(this, sql_upd_transaction, "Добавим сессию", *query_upd_transaction, true, db, data_app);

        // откроем окно полисов
        if (persons_w==NULL) {
            persons_w = new persons_wnd(db, data_app, *settings);
            persons_w->setWindowState( persons_w->windowState() | Qt::WindowFullScreen);
        }

        ui->lab_start->setVisible(true);

        try_update_inko_starter();

        clear_log_files();

        persons_w->show();
        this->hide();

    } else {
        QMessageBox::warning(this, QString("Неверный пароль"), QString("Неверный пароль\n"));
    }
    ui->tc_op_pass->setText("");
}

void ConnectionWindow::on_ch_change_pass_clicked(bool checked) {
    ui->pan_DB_params->setVisible(checked);
    on_combo_filial_activated("");
}

void ConnectionWindow::on_combo_operator_activated(const QString &arg1) {
    ui->tc_op_pass->setText("");
}

void ConnectionWindow::on_tc_op_pass_editingFinished() {
    //on_bn_start_clicked();
}

void ConnectionWindow::on_bn_exit_clicked() {
    if (data_app.id_session>0) {
        // закрытие сессии
        QString sql_close = "update db_sessions "
                            " set status=-1,"
                            "     dt_end=CURRENT_TIMESTAMP "
                            " where id=" + QString::number(data_app.id_session) + " ; ";
        QSqlQuery *query_close = new QSqlQuery(db);
        bool res_close = mySQL.exec(this, sql_close, "Закроем сессию", *query_close, true, db, data_app);
    }

    QApplication::closeAllWindows();
}

void ConnectionWindow::on_combo_operator_currentIndexChanged(const QString &arg1) {
    data_app.operator_fio = arg1;
}

void ConnectionWindow::on_bn_messages_clicked() {
    // удалим старое описание
    QString sql_del = "delete from messages where version='" + data_app.version + "' ; ";
    QSqlQuery *query_del = new QSqlQuery(db);
    bool res_del = mySQL.exec(this, sql_del, "Очистка старого описания программы", *query_del, true, db, data_app);

    // сохраним текст в базу
    QString s = ui->text_comments->toPlainText();
    s = s.right(s.size()-s.indexOf("\n\n")-2);
    QString sql_ins = "insert into messages(version, text) "
                      "values('" + data_app.version + "','" + s + "') ; ";
    QSqlQuery *query_ins = new QSqlQuery(db);
    bool res_ins = mySQL.exec(this, sql_ins, "Сохранение нового описания программы", *query_ins, true, db, data_app);
}

void ConnectionWindow::on_combo_filial_activated(const QString &arg1) {
    switch (ui->combo_filial->currentData().toInt()) {
    case -1: {
            ui->tc_host->setText(data_app.work_host);
            ui->spin_port->setValue(data_app.work_port);
            ui->tc_base->setText(data_app.work_base);
            ui->tc_user->setText(data_app.work_user);
            ui->tc_pass->setText(data_app.work_pass);
        }
        break;
    case 0: {
            ui->tc_host->setText(data_app.test_host);
            ui->spin_port->setValue(data_app.test_port);
            ui->tc_base->setText(data_app.test_base);
            ui->tc_user->setText(data_app.test_user);
            ui->tc_pass->setText(data_app.test_pass);
        }
        break;
    case 1: {
            ui->tc_host->setText(data_app.voro_host);
            ui->spin_port->setValue(data_app.voro_port);
            ui->tc_base->setText(data_app.voro_base);
            ui->tc_user->setText(data_app.voro_user);
            ui->tc_pass->setText(data_app.voro_pass);
        }
        break;
    case 8: {
            ui->tc_host->setText(data_app.kursk_host);
            ui->spin_port->setValue(data_app.kursk_port);
            ui->tc_base->setText(data_app.kursk_base);
            ui->tc_user->setText(data_app.kursk_user);
            ui->tc_pass->setText(data_app.kursk_pass);
        }
        break;
    case 64: {
            ui->tc_host->setText(data_app.belg_host);
            ui->spin_port->setValue(data_app.belg_port);
            ui->tc_base->setText(data_app.belg_base);
            ui->tc_user->setText(data_app.belg_user);
            ui->tc_pass->setText(data_app.belg_pass);
        }
        break;
    default:
        break;
    }
}

void ConnectionWindow::on_ch_oper_by_point_clicked(bool checked) {
    refresh_operators(ui->combo_point->currentData().toInt());
}

//  удаление старых журналов производительности
void ConnectionWindow::clear_log_files() {
    ui->lab_updater->setText("Очистка журнала произодительности");
    QApplication::processEvents();

    QString log_dir_s = data_app.path_dbf;
    log_dir_s = log_dir_s.replace("_DBF_", "_LOG_");
    QDir dir(log_dir_s);

    QStringList filters;
    filters << "*.log" << "*.txt";
    dir.setNameFilters(filters);

    QFileInfoList list = dir.entryInfoList();
    QDate date_week = QDate::currentDate().addDays(-7);

    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileinfo(list.at(i));
        if (fileinfo.created().date()<date_week) {
            QString fname = list.at(i).filePath();
            QFile file(fname);
            bool res = file.remove();

            ui->lab_updater->setText(fname);
            QApplication::processEvents();
        }
    };

    ui->lab_updater->setText(" -//- ");
    QApplication::processEvents();
}

// обновление INKO_STARTER, если надо
void ConnectionWindow::try_update_inko_starter() {

    try {

        // обновим соединение с базой обновлений
        QString host, base, user, pass;
        host = "172.20.250.19";
        base = "inko_update";
        user = "SYSDBA";
        pass = "masterkey";

        db_update = QSqlDatabase::addDatabase("QIBASE", "My UpdateDB");

        // сервер, на котором расположена БД - у нас localhost
        db_update.setHostName(host);
        // имя базы данных
        db_update.setDatabaseName(base);
        // имя пользователя - postgres
        db_update.setUserName(user);
        // пароль для подключения к БД - 1234
        db_update.setPassword(pass);

        if (db_update.open()) {

            // обновим файлы INKO_STARTER
            QSqlQuery *query_vers = new QSqlQuery(db_update);
            QString sql_vers =
                    "select v.id, v.vers_n "
                    " from  "
                    " ( select v.id_prog, max(v.vers_n) as max_n "
                    "    from upd_prog_vers v "
                    "    where v.id_prog=0 "
                    "    group by v.id_prog "
                    " ) q left join upd_prog_vers v on(v.id_prog=q.id_prog and q.max_n=v.vers_n) ; ";
            query_vers->exec(sql_vers);
            query_vers->next();
            int id_vers = query_vers->value(0).toInt();
            int n_vers = query_vers->value(1).toInt();
            query_vers->finish();
            delete query_vers;

            // синхраонизация локальных и серверных файлов
            QString sql =
                    "select f.id, f.f_name, f.f_size, f.f_date, f.f_hash "
                    " from upd_files f "
                    " where f.id_prog_vers=" + QString::number(id_vers) + " ; ";
            QSqlQuery *query = new QSqlQuery(db_update);
            query->exec (sql);
            while (query->next()) {
                int     f_id   = query->value(0).toInt();
                QString f_name = query->value(1).toString();
                int     f_size = query->value(2).toInt();
                QDate   f_date = query->value(3).toDate();
                QString f_hash = query->value(4).toString();

                // проверим наличие и параметры файла на клиенте
                QString f_local_name = data_app.path_install + f_name;
                QFileInfo fi (f_local_name);

                ui->lab_updater->setText( f_local_name );
                QApplication::processEvents();

                if ( !fi.exists() ||
                     ( fi.exists() &&
                       fi.suffix()!="dll" &&                // dll загружаем только при их отсутствии
                       fi.suffix()!="bat" &&                // bat загружаем только при их отсутствии
                       ( fi.size()!=f_size  ||              // разный размер
                         file_hash32(f_local_name)!=f_hash  // разный хэш
                   ) ) ) {
                    ui->lab_updater->setText( ui->lab_updater->text() + "  ->  reload" );
                    QApplication::processEvents();

                    // удалим старый файл
                    bool res = QFile::remove(f_local_name);

                    // выгрузим файл из базы обновлений
                    sql = "select f.f_data from upd_files f ";
                    sql += " where f.id=" + QString::number(f_id) + " ; ";
                    QSqlQuery *query = new QSqlQuery(db_update);
                    query->exec(sql);
                    query->next();
                    QByteArray f_data = query->value(0).toByteArray();

                    // подготовим папку для сохранения файла
                    QFileInfo fi(f_local_name);
                    QDir d(fi.absolutePath());
                    d.mkpath(fi.absolutePath());

                    // сохраним файл на диск
                    QFile f(f_local_name);
                    if (f.open(QIODevice::WriteOnly)) {
                        f.write(f_data);
                        f.close();
                    } else {
                        QMessageBox::warning(this, "Ошибка обновления INKO-STARTER",
                                             "Не удалось обновить утилиту обновления INKO_STARTER.\n\n"
                                             "Всё в порядке - просто закройте это сообщение и продолжайте \n"
                                             "работать как обычно, но потом, когда будет свободное время, \n"
                                             "свяжитесь со мной по телефону: \n\n8-920-449-54-88. \n\nАлександр.", QMessageBox::Close, QMessageBox::Close);
                        break;
                    }

                    ui->lab_updater->setText( ui->lab_updater->text() +  "  ->  OK" );
                }
            }
            query->finish();
            delete query;
        }
        db_update.close();

    } catch (...) {
    }
}

char c_hex[17] = "0123456789abcdef";

QString ConnectionWindow::file_hash32(QString &full_name) {
    QString h_str = "";
    QFile f(full_name);

    if (f.open(QIODevice::ReadOnly)) {
        QDataStream in(&f);    // read the data serialized from the file
        in.setVersion(QDataStream::Qt_4_2);

        uchar hash[32];
        for (int n=0; n<32; n++) {
            hash[n] = 0;
        }

        uchar data[32];
        bool f_end = false;

        while (!f_end) {
            for (int n=0; n<32; n++) {
                if (!f_end)
                    in>>data[n];
                else
                    data[n]=0;

                if (in.atEnd())
                    f_end = true;
            }
            for (int n=0; n<32; n++) {
                hash[n] ^= data[n];
            }
        }
        // преобразуем хеш в строку
        for (int n=0; n<32; n++) {
            h_str += QChar(c_hex[hash[n]>>4]);
            h_str += QChar(c_hex[hash[n]&uchar(15)]);
        }
    }
    return h_str;
}
