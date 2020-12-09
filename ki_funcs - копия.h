#ifndef KI_FUNCS
#define KI_FUNCS

#include <QString>
#include <QSqlDatabase>
#include <QTableView>
#include <QSqlQueryModel>


int max(int i, int j);

QString clear_num(QString str);

QString show_num(int num);

bool table2csv(QSqlDatabase &db, QString sql, QSqlQueryModel &model, QTableView *table, QString path="", QWidget *parent=NULL, QStringList names_old=QStringList(), QStringList names_new=QStringList());

#endif // KI_FUNCS

