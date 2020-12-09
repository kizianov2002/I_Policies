#-------------------------------------------------
#
# Project created by QtCreator 2015-01-20T12:50:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = _INKO_POLISES_2
TEMPLATE = app
QT += sql
QT += serialport

CONFIG -= embeded_manifest_dll

RC_FILE = inko_pol2.rc

SOURCES += main.cpp\
        persons_wnd.cpp \
    connection_window.cpp \
    polices_wnd.cpp \
    photoeditor_wnd.cpp \
    print_vs_wnd.cpp \
    representers_wnd.cpp \
    ki_sqlquerymodel.cpp \
    print_ch_smo_wnd.cpp \
    print_notice_pol_wnd.cpp \
    pvp_n_vs_wnd.cpp \
    blanks_wnd.cpp \
    work_calendar_wnd.cpp \
    settings_wnd.cpp \
    print_db_pol_wnd.cpp \
    send_to_fond_wnd.cpp \
    get_from_fond_wnd.cpp \
    send_pol_sms_wnd.cpp \
    export2csv_wnd.cpp \
    medorgs_wnd.cpp \
    export_assig_wnd.cpp \
    print_form_8_wnd.cpp \
    print_vs_by_date_wnd.cpp \
    print_enp_by_date_wnd.cpp \
    print_pers_sexage_wnd.cpp \
    export_assig_by_mo_wnd.cpp \
    print_assig_sexage_by_list_wnd.cpp \
    print_assig_letters_by_list_wnd.cpp \
    sessions_wnd.cpp \
    print_pol_pg_wnd.cpp \
    print_pol_monitoring_wnd.cpp \
    change_vizit_wnd.cpp \
    add_blank_wnd.cpp \
    change_polis_wnd.cpp \
    protocols_wnd.cpp \
    list_persons_terrs_wnd.cpp \
    list_assig_medorgs_wnd.cpp \
    add_medsite_ter_wnd.cpp \
    add_blank_scan_wnd.cpp \
    add_blanks_box_wnd.cpp \
    export_assig_bystep_wnd.cpp \
    info_pers_doubles_wnd.cpp \
    assig_gaps_wnd.cpp \
    quest2fond_wnd.cpp \
    print_pays2mo_wnd.cpp \
    print_pays2mo_rep_pt_wnd.cpp \
    folder_wnd.cpp \
    name_to_sex_wnd.cpp \
    namepat_to_sex_wnd.cpp \
    match_assig_tfoms_wnd.cpp \
    info_not_sended_wnd.cpp \
    export_pers_by_distrs_wnd.cpp \
    msg_wnd.cpp \
    terminals_wnd.cpp \
    show_text_wnd.cpp \
    show_tab_wnd.cpp \
    match_tfoms_wnd.cpp \
    talks_wnd.cpp \
    add_talk_wnd.cpp \
    packs_wnd.cpp \
    statistic_acts_wnd.cpp \
    tfomsFiles_wnd.cpp \
    fiasFiles_wnd.cpp \
    unite_persons_wnd.cpp \
    blanks_pol_act_wnd.cpp \
    print_blanks_pol_act_wnd.cpp \
    print_subconto_wnd.cpp \
    print_vs_register_by_date_wnd.cpp \
    print_monitoring_pers_by_date_wnd.cpp \
    get_date_wnd.cpp \
    print_expired_vs_statistic_wnd.cpp \
    acts_wnd.cpp \
    act_pol2point_wnd.cpp \
    act_vs2point_wnd.cpp \
    act_pol2smo_wnd.cpp \
    print_blanks_act_wnd.cpp \
    finder_wnd.cpp \
    beep_sql.cpp \
    beep_guten_morgen_wnd.cpp \
    spr_assig_errors_wnd.cpp \
    spr_insure_errors_wnd.cpp \
    change_error_wnd.cpp \
    print_dispanserize_plans_wnd.cpp \
    act_blanknums_wnd.cpp \
    resend_events_wnd.cpp \
    get_year_wnd.cpp \
    disp_plans_wnd.cpp \
    ki_funcs.cpp \
    print_assiglay_todate_wnd.cpp \
    print_operator_timeline_wnd.cpp \
    beep_30_min_wnd.cpp \
    print_1st_aid_sexage_by_list_wnd.cpp \
    print_notic2_wnd.cpp \
    blanks_bso_wnd.cpp

HEADERS  +=persons_wnd.h \
    connection_window.h \
    polices_wnd.h \
    photoeditor_wnd.h \
    s_data_doc.h \
    s_data_app.h \
    s_data_polis.h \
    print_vs_wnd.h \
    representers_wnd.h \
    d_date.h \
    ki_sqlquerymodel.h \
    print_ch_smo_wnd.h \
    s_data_vizit.h \
    s_data_pers.h \
    s_filter_pers.h \
    print_notice_pol_wnd.h \
    pvp_n_vs_wnd.h \
    blanks_wnd.h \
    work_calendar_wnd.h \
    settings_wnd.h \
    print_db_pol_wnd.h \
    send_to_fond_wnd.h \
    get_from_fond_wnd.h \
    send_pol_sms_wnd.h \
    export2csv_wnd.h \
    s_data_assig.h \
    export_assig_wnd.h \
    print_form_8_wnd.h \
    print_vs_by_date_wnd.h \
    print_enp_by_date_wnd.h \
    print_pers_sexage_wnd.h \
    export_assig_by_mo_wnd.h \
    print_assig_sexage_by_list_wnd.h \
    print_assig_letters_by_list_wnd.h \
    sessions_wnd.h \
    print_pol_pg_wnd.h \
    print_pol_monitoring_wnd.h \
    change_vizit_wnd.h \
    add_blank_wnd.h \
    change_polis_wnd.h \
    protocols_wnd.h \
    s_data_file_p.h \
    list_assig_medorgs_wnd.h \
    list_persons_terrs_wnd.h \
    s_data_file_s.h \
    s_data_file_f.h \
    add_medsite_ter_wnd.h \
    add_blank_scan_wnd.h \
    add_blanks_box_wnd.h \
    ki_exec_query.h \
    export_assig_bystep_wnd.h \
    info_pers_doubles_wnd.h \
    assig_gaps_wnd.h \
    quest2fond_wnd.h \
    print_pays2mo_wnd.h \
    print_pays2mo_rep_pt_wnd.h \
    folder_wnd.h \
    name_to_sex_wnd.h \
    namepat_to_sex_wnd.h \
    match_assig_tfoms_wnd.h \
    info_not_sended_wnd.h \
    medorgs_wnd.h \
    export_pers_by_distrs_wnd.h \
    msg_wnd.h \
    terminals_wnd.h \
    show_text_wnd.h \
    show_tab_wnd.h \
    match_tfoms_wnd.h \
    s_data_blank.h \
    talks_wnd.h \
    add_talk_wnd.h \
    s_data_tals.h \
    packs_wnd.h \
    statistic_acts_wnd.h \
    tfomsFiles_wnd.h \
    fiasFiles_wnd.h \
    unite_persons_wnd.h \
    blanks_pol_act_wnd.h \
    print_blanks_pol_act_wnd.h \
    print_subconto_wnd.h \
    print_vs_register_by_date_wnd.h \
    print_monitoring_pers_by_date_wnd.h \
    get_date_wnd.h \
    print_expired_vs_statistic_wnd.h \
    acts_wnd.h \
    act_pol2point_wnd.h \
    act_vs2point_wnd.h \
    act_pol2smo_wnd.h \
    print_blanks_act_wnd.h \
    finder_wnd.h \
    beep_sql.h \
    beep_guten_morgen_wnd.h \
    spr_assig_errors_wnd.h \
    spr_insure_errors_wnd.h \
    change_error_wnd.h \
    s_data_event.h \
    print_dispanserize_plans_wnd.h \
    act_blanknums_wnd.h \
    resend_events_wnd.h \
    get_year_wnd.h \
    disp_plans_wnd.h \
    ki_funcs.h \
    print_assiglay_todate_wnd.h \
    print_operator_timeline_wnd.h \
    beep_30_min_wnd.h \
    print_1st_aid_sexage_by_list_wnd.h \
    print_notic2_wnd.h \
    blanks_bso_wnd.h

FORMS    += persons_wnd.ui \
    connection_window.ui \
    polices_wnd.ui \
    photoeditor_wnd.ui \
    print_vs_wnd.ui \
    representers_wnd.ui \
    print_ch_smo_wnd.ui \
    print_notice_pol_wnd.ui \
    pvp_n_vs_wnd.ui \
    blanks_wnd.ui \
    work_calendar_wnd.ui \
    settings_wnd.ui \
    print_db_pol_wnd.ui \
    send_to_fond_wnd.ui \
    get_from_fond_wnd.ui \
    send_pol_sms_wnd.ui \
    export2csv_wnd.ui \
    medorgs_wnd.ui \
    export_assig_wnd.ui \
    print_form_8_wnd.ui \
    print_vs_by_date_wnd.ui \
    print_enp_by_date_wnd.ui \
    print_pers_sexage_wnd.ui \
    export_assig_by_mo_wnd.ui \
    print_assig_sexage_by_list_wnd.ui \
    print_assig_letters_by_list_wnd.ui \
    sessions_wnd.ui \
    print_pol_pg_wnd.ui \
    print_pol_monitoring_wnd.ui \
    change_vizit_wnd.ui \
    add_blank_wnd.ui \
    change_polis_wnd.ui \
    protocols_wnd.ui \
    list_assig_medorgs_wnd.ui \
    list_persons_terrs_wnd.ui \
    add_medsite_ter_wnd.ui \
    add_blank_scan_wnd.ui \
    add_blanks_box_wnd.ui \
    export_assig_bystep_wnd.ui \
    info_pers_doubles_wnd.ui \
    assig_gaps_wnd.ui \
    quest2fond_wnd.ui \
    print_pays2mo_wnd.ui \
    print_pays2mo_rep_pt_wnd.ui \
    folder_wnd.ui \
    name_to_sex_wnd.ui \
    namepat_to_sex_wnd.ui \
    match_assig_tfoms_wnd.ui \
    info_not_sended_wnd.ui \
    export_pers_by_distrs_wnd.ui \
    msg_wnd.ui \
    terminals_wnd.ui \
    show_text_wnd.ui \
    show_tab_wnd.ui \
    match_tfoms_wnd.ui \
    talks_wnd.ui \
    add_talk_wnd.ui \
    packs_wnd.ui \
    statistic_acts_wnd.ui \
    fiasFiles_wnd.ui \
    unite_persons_wnd.ui \
    blanks_pol_act_wnd.ui \
    print_blanks_pol_act_wnd.ui \
    print_subconto_wnd.ui \
    print_vs_register_by_date_wnd.ui \
    print_monitoring_pers_by_date_wnd.ui \
    get_date_wnd.ui \
    print_expired_vs_statistic_wnd.ui \
    acts_wnd.ui \
    act_pol2point_wnd.ui \
    act_vs2point_wnd.ui \
    act_pol2smo_wnd.ui \
    print_blanks_act_wnd.ui \
    finder_wnd.ui \
    beep_guten_morgen_wnd.ui \
    beep_sql.ui \
    spr_assig_errors_wnd.ui \
    spr_insure_errors_wnd.ui \
    change_error_wnd.ui \
    print_dispanserize_plans_wnd.ui \
    tfomsFiles_wnd.ui \
    act_blanknums_wnd.ui \
    resend_events_wnd.ui \
    get_year_wnd.ui \
    disp_plans_wnd.ui \
    print_assiglay_todate_wnd.ui \
    print_operator_timeline_wnd.ui \
    beep_30_min_wnd.ui \
    print_1st_aid_sexage_by_list_wnd.ui \
    print_notic2_wnd.ui \
    blanks_bso_wnd.ui

PUBLIC_HEADERS         += $$PWD/COM/qextserialport.h \
                          $$PWD/COM/qextserialenumerator.h \
                          $$PWD/COM/qextserialport_global.h

HEADERS                += $$PUBLIC_HEADERS \
                          $$PWD/COM/qextserialport_p.h \
                          $$PWD/COM/qextserialenumerator_p.h \

SOURCES                += $$PWD/COM/qextserialport.cpp \
                          $$PWD/COM/qextserialenumerator.cpp

win32:SOURCES          += $$PWD/COM/qextserialport_win.cpp \
                          $$PWD/COM/qextserialenumerator_win.cpp

win32:LIBS             += -lsetupapi -ladvapi32 -luser32

DISTFILES += \
    inko_pol2.rc \
    inko-polis.ico

RESOURCES += \
    _inko_pol2_res.qrc
