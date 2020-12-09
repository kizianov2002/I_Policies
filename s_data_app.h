#ifndef S_DATA_APP
#define S_DATA_APP

#include <QDate>
#include <QPoint>


typedef struct {
    bool screen_small;
    int screen_w, screen_h;

    // register
    int terminal_id;
    int terminal_id_point;
    QString terminal_name;
    int terminal_status;



    // данные подключения к БД
    QString host, base, user, pass;
    int port;

    int font_size;

    // данные подключения к базе счетов
    QString scores_host, scores_base, scores_user, scores_pass;
    int     scores_port;

    int     id_session;

    QString db_host;
    QString db_name;
    QString db_vers;

    QString path_install;
    QString path_arch;
    QString path_reports;
    QString path_temp;
    QString path_foto;
    QString path_sign;
    QString path_in;
    QString path_out;
    QString path_dbf;
    QString path_acts;

    QString path_to_TFOMS;      // папка автопроцессинга для отправки запросов в ТФОМС - проверка новых клиентов в региональном сегменте
    QString path_from_TFOMS;    // папка автопроцессинга для приёма ответов от ТФОМС   - проверка новых клиентов в региональном сегменте
    //QDate   TFOMS_date_send;
    //int     TFOMS_cnt_send;

    int msg_timeout_back;
    int msg_timeout_front;
    int msg_timeout_counter;




    // ЭТИ ДАННЫЕ НАДО ЧИТАТЬБ ИЗ БАЗЫ ДАННЫХ
    // данные филиала
    QString smo_ogrn;
    QString smo_regnum;
    QString smo_short;
    QString smo_name;
    QString smo_name_full;
    QString smo_name_full_rp;
    QString smo_director;
    QString smo_oms_chif;
    QString smo_address;
    QString smo_contact_info;
    QString smo_logo1;
    QString smo_logo2;
    QString smo_logo3;

    QString terr_name;
    QString terr_name_rp;

    QString tfoms_director;
    QString tfoms_director_2;
    QString tfoms_director_fio;
    QString tfoms_chif;

    QString filial_name;
    QString filial_name_rp;
    QString filial_city;
    QString filial_by_city;
    QString filial_by_city_tp;
    QString filial_director;
    QString filial_director_2;
    QString filial_director_fio;
    QString filial_chif;
    QString filial_phone;
    QString filial_phone_2;
    QString filial_email;
    QString filial_email_2;

    // подключение к ФИАС
    bool FIAS_flag;     // признак, что в выгрузке для ТФОМС надо использовать ФИАС
    QString FIAS_db_drvr;
    QString FIAS_db_host;
    QString FIAS_db_name;
    QString FIAS_db_port;
    QString FIAS_db_user;
    QString FIAS_db_pass;
    QString FIAS_db_admn;
    QString FIAS_db_admp;
    QString FIAS_db_connstring;



    QDateTime dt_last_act;     // дата/время последнего движения мыши
    QPoint    pt_last_act;     // координаты послеюнего положения мыши



    // ЭТИ ДАННЫЕ ДОЛЖНЫ ХРАНИТЬСЯ ЛОКАЛЬНО В INI-ФАЙЛЕ
    QString ini_path;
    QString version;

    // доступные базы данных
    QString filials_list;
    QString voro_host, voro_base, voro_user, voro_pass;
    int voro_port;
    QString kursk_host, kursk_base, kursk_user, kursk_pass;
    int kursk_port;
    QString belg_host, belg_base, belg_user, belg_pass;
    int belg_port;
    QString test_host, test_base, test_user, test_pass;
    int test_port;
    QString work_host, work_base, work_user, work_pass;

    // данные этого ПВП
    int id_point;
    int point_status;
    int point_rights;
    QString point_name;
    QString point_regnum;
    QString point_address;
    QString point_signer;   // ФИО сотрудника с правом подписи ВС

    int id_operator;
    QString operator_fio;
    QString rep_doer;

    bool is_tech;
    bool is_head;
    bool is_locl;
    bool is_call;
    bool f_gen_regVS;
    bool f_gen_regPol;
    bool f_send2fond_xmls;
    bool f_send2fond_zips;
    bool f_send2fond_1ZIP;
    bool f_send2fond_new_IM;
    bool f_send2fond_new_OT;
    bool f_send2fond_new_assigs;

    bool use_folders;
    QString default_folder;

    // техническая информация
    QString contragent;        // контрагент для некоторых отчётов

    int  assig_vers;           // 0 - простая структура - Курск,  1 - развёрнуюая структура - Белгород
    bool assig_csv_headers;    // 0 - в CSV-файле прикреплений не добавлять заголовки

    int log_bord;       // минимальное времы испольнения запроса для занесения в журнал

    QString xml_vers;
    QString ocato;
    QString ocato_text;
    int snils_format;          // 0 - только цифры ХХХХХХХХХХХ, 1 - с разделителями ХХХ-ХХХ-ХХХ ХХ
    int split_by_ot;           // 0 - все данные одним файлом, 1 - люди без отчества отдельным файлом
    int send_p02_in_p06;       // 0 - при П06 не отсылать П02 на закрываемый полис, 1 - при закрытии полиса всегда шлём П02
    int pol_start_date_by_vs;  // 0 - при дата актиации полиса единого образца - дате выдачи ВС, 1 - у каждого полиса своя дата выдачи
    bool f_zip_packs;

    int vs_blank_width_mm;
    int vs_blank_height_mm;
    int vs_field_left_mm;
    int vs_field_right_mm;
    int vs_field_top_mm;
    int vs_field_bottom_mm;

    // форма №8
    QString form8_doc1_num;
    QDate form8_doc1_date;
    bool form8_has_doc2;
    QString form8_doc2_num;
    QDate form8_doc2_date;

    QString form8_codename1;
    QString form8_codename2;
    QString form8_codename3;
    QString form8_code1;
    QString form8_code2;
    QString form8_code3;

    QString tfoms_worker;
    QString tfoms_worker2_place;
    QString tfoms_worker2_fio;
    QString tfoms_worker2_phone;

    // сканирование штрих-кодов
    int work_port;
    QString barcodes_portname;
    QString barcodes_file;

    // акты приёма-передачи банков полисов
    QString head_act_regnum;
    QString head_act_obtainer;
    QString point_act_regnum;
    QString point_act_obtainer;

    // данные для печати акта передачи полисов на ПВП
    QString act_dog_num;
    QDate   act_dog_date;
    QString act_filial_chif_post;
    QString act_filial_chif_fio;
    QString act_point_chif_post;
    QString act_point_chif_fio;

    QString act_point_org_name;
    QString act_point_address;
    QString act_point_requisits_1;
    QString act_point_requisits_2;
    QString act_point_okpo;

    QString act_filial_org_name;
    QString act_filial_address;
    QString act_filial_requisits_1;
    QString act_filial_requisits_2;
    QString act_filial_okpo;

    QString act_base_doc_name;
    QString act_base_doc_num;
    QDate   act_base_doc_date;
    QString act_num;
    QDate   act_date;
    QString act_count;
    QString act_tabel_num;

} s_data_app;

#endif // S_DATA_APP

