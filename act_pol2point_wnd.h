#ifndef ACT_POL2POINT_WND_H
#define ACT_POL2POINT_WND_H

#include <QDialog>

#include "s_data_app.h"
#include "ki_exec_query.h"

#include "act_blanknums_wnd.h"

// параметры акта передачи полисов на ПВП
typedef struct {
    int     id;         // ID акта
    int     id_point;   // ID целевого ПВП
    QString act_num;
    QDate   act_date;
} s_data_act_pol2point;

namespace Ui {
class act_pol2point_wnd;
}

class act_pol2point_wnd : public QDialog
{
    Q_OBJECT
    QSqlDatabase &db;    // это - наша база данных
    QSqlQueryModel model_act_polises;
    exec_SQL mySQL;

    QSettings &settings;
    s_data_app &data_app;
    s_data_act_pol2point &data_act;

    act_blanknums_wnd *act_blanknums_w;

    void refresh_combo_acts();
    void refresh_combo_move_acts();
    void refresh_tab_act_blanks();

public:
    explicit act_pol2point_wnd(QSqlDatabase &db, s_data_app &data_app, s_data_act_pol2point &data_act, QSettings &settings, QWidget *parent = 0);
    ~act_pol2point_wnd();

private slots:
    void on_bn_close_clicked();
    void on_bn_refresh_clicked();
    void on_combo_act_currentIndexChanged(int index);

    void on_ch_date_scan_enp_clicked(bool checked);
    void on_date_scan_enp_dateChanged(const QDate &date);

    void on_bn_add_blanks_clicked();
    void on_bn_move_blanks_clicked();
    void on_bn_remove_blanks_clicked();

    void on_ch_act_clicked(bool checked);

    void on_combo_order_activated(const QString &arg1);

private:
    Ui::act_pol2point_wnd *ui;
};

#endif // ACT_POL2POINT_WND_H
