#ifndef medorgs_wnd_H
#define medorgs_wnd_H

#include <QDialog>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include "ki_exec_query.h"
#include "s_data_app.h"
#include "s_data_assig.h"
#include "sessions_wnd.h"
#include "add_medsite_ter_wnd.h"

namespace Ui {
class medorgs_wnd;
}

class medorgs_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;
    s_data_app &data_app;
    s_data_assig &data_assig;
    QSettings &settings;

    QSqlQueryModel model_medorgs;
    QSqlQueryModel model_medparts;
    QSqlQueryModel model_medters;
    QSqlQueryModel model_medsites;
    QSqlQueryModel model_medsite_ters;
    exec_SQL mySQL;

    QLabel *statusLab;
    add_medsite_ter_wnd *add_medsite_ter_w;

    bool f_assig;

    void refresh_medorgs_tab();
    void refresh_head_mdorgs();

    void refresh_medparts_tab();
    void refresh_medpart_type();

    void refresh_medters_tab();
    void refresh_medter_mo();
    void refresh_medter_spec();

    void refresh_medsites_tab();
    void refresh_medsite_mo();
    void refresh_medsite_mp();
    void refresh_medsite_type();

    void refresh_medsite_ters_tab();

    QString medorgs_sql;

public:
    explicit medorgs_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, s_data_assig &data_assig, QWidget *parent = 0);
    ~medorgs_wnd();

private slots:

    void on_bn_refresh_clicked();
    void on_bn_add_clicked();
    void on_bn_edit_clicked();
    void on_tab_medorgs_clicked(const QModelIndex &index);
    void on_bn_delete_clicked();
    void on_bn_ocato_clicked();

    /*void on_ln_ocato_editingFinished();
    void on_ln_code_mo_editingFinished();
    void on_ln_name_mo_editingFinished();
    void on_ln_name_mo_full_editingFinished();
    void on_combo_head_mo_activated(int index);
    void on_ln_phone_editingFinished();
    void on_ln_fax_editingFinished();
    void on_ln_email_editingFinished();
    void on_ln_www_editingFinished();
    void on_ln_comment_editingFinished();*/

    void on_ch_is_head_clicked(bool checked);
    void on_bn_refresh_medsites_clicked();
    void on_tab_medsites_clicked(const QModelIndex &index);
    void on_bn_add_medsite_clicked();
    void on_bn_edit_medsite_clicked();
    void on_ch_medsites_mo_clicked();
    void on_bn_delete_medsite_clicked();
    void on_bn_refresh_medters_clicked();
    void on_tab_medters_clicked(const QModelIndex &index);
    void on_bn_add_medter_clicked();
    void on_combo_medter_mo_currentIndexChanged(int index);
    void on_bn_edit_medter_clicked();
    void on_bn_delete_medter_clicked();
    void on_bn_filter_clicked();
    void on_bn_clear_clicked();
    void on_ln_filter_editingFinished();

    void on_bn_refresh_medparts_clicked();
    void on_bn_add_medpart_clicked();
    void on_bn_edit_medpart_clicked();
    void on_bn_delete_medpart_clicked();
    void on_tab_medparts_clicked(const QModelIndex &index);


    void on_ch_medsites_mo_clicked(bool checked);
    void on_combo_medsite_mo_activated(const QString &arg1);

    void on_ch_medters_mo_clicked(bool checked);

    void on_combo_medter_mo_activated(const QString &arg1);



    void on_bn_refresh_medsite_ters_clicked();
    void on_ch_medsite_ters_now_clicked();
    void on_bn_add_medsite_ter_clicked();
    void on_bn_delete_medsite_ter_clicked();

    void on_bn_ok_clicked();

    void on_bn_close_clicked();

    void on_bn_toCSV_clicked();

    void on_ch_assig_settings_clicked(bool checked);
    void on_ch_mo_assig_data_clicked(bool checked);
    void on_ch_show_mo_clicked(bool checked);
    void on_ch_show_letters_clicked(bool checked);

private:
    Ui::medorgs_wnd *ui;
};

#endif // medorgs_wnd_H
