#include "ki_funcs.h"

#include <QDate>
#include <QSqlQuery>
#include <QTextCodec>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QSqlQueryModel>
#include <QProcess>

#include "qt_windows.h"
#include "qwindowdefs_win.h"
#include <shellapi.h>
#include <winuser.h>

int max(int i, int j) {
    if (i>j) return i;
    else return j;
}

QString clear_num(QString str) {
    QString str1;
    for (int i=0; i<str.size(); i++) {
        QString c = str.at(i);
        if ( c=="0"
             || c=="1"
             || c=="2"
             || c=="3"
             || c=="4"
             || c=="5"
             || c=="6"
             || c=="7"
             || c=="8"
             || c=="9" ) {
            // всё в порядке
            str1 = str1 + c;
        } else {
            break;
        }
    }
    return str1;
}

QString show_num(int num) {
    QString str, str1;
    int len;

    if (num>=0) {
        str=QString::number(num);
        len = str.length();
        str1 = str.right(3);
        if (num>999)  str1 = str.left(len-3).right(3) + "'" + str1;
        if (num>999999)  str1 = str.left(len-6).right(6) + "'" + str1;
        if (num>999999999)  str1 = str.left(len-9).right(9) + "'" + str1;
        if (num>999999999999)  str1 = str.left(len-12).right(12) + "'" + str1;
        if (num>999999999999999)  str1 = str.left(len-15).right(15) + "'" + str1;
    } else {
        str=QString::number(0-num);
        len = str.length();
        str1 = str.right(3);
        if (num<-999)  str1 = str.left(len-3).right(3) + "'" + str1;
        if (num<-999999)  str1 = str.left(len-6).right(6) + "'" + str1;
        if (num<-999999999)  str1 = str.left(len-9).right(9) + "'" + str1;
        if (num<-999999999999)  str1 = str.left(len-12).right(12) + "'" + str1;
        if (num<-999999999999999)  str1 = str.left(len-15).right(15) + "'" + str1;
        str1 = "-" + str1;
    }

    return str1;
}

bool table2csv(QSqlDatabase &db, QString sql, QSqlQueryModel &model, QTableView *table, QString path, QWidget *parent, QStringList names_old, QStringList names_new) {
    QString fname_s = QFileDialog::getSaveFileName(parent,
                                                   "Куда сохранить файл?",
                                                   path,
                                                   "файлы данных (*.csv);;простой текст (*.txt)");
    if (fname_s.isEmpty()) {
        QMessageBox::warning(parent,
                             "Не выбрано имя файла",
                             "Имя цедевого файла не задано.\n\n"
                             "Операция отменена.");
        return false;
    }
    if (QFile::exists(fname_s)) {
        if (!QFile::remove(fname_s)) {
            QMessageBox::warning(parent,
                                 "Ошибка при удалении старого файла",
                                 "При удалении старого файла произошла непредвиденная ошибка.\n\n"
                                 "Операция отменена.");
            return false;
        }
    }

    // сохраним данные в файл
    QFile file(fname_s);
    QFileInfo file_info(fname_s);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(parent,
                             "Ошибка создания файла",
                             "При создании целевого файла произошла ошибка.\n\n"
                             "Операция отменена.");
        return false;
    }

    // подготовим поток для сохранения файла
    QTextStream stream(&file);
    QString codec_name = "Windows-1251";
    QTextCodec *codec = QTextCodec::codecForName(codec_name.toLocal8Bit());
    stream.setCodec(codec);

    QString head;
    QString head_word;
    int cnt_head = model.columnCount();
    for (int i=0; i<cnt_head; i++) {
        if (table->columnWidth(i)>5) {
            head_word = model.headerData(i,Qt::Horizontal).toString().replace("\n"," ").trimmed();
            // проверим, надо ли заменить заголовок
            for (int i=0; i<names_old.size() && i<names_new.size(); i++) {
                if (head_word==names_old.at(i))
                    head_word = names_new.at(i);
            }
            head += head_word;
            if (i<cnt_head-1)  head += ";";
        }
    }
    stream << head << "\n";

    // переберём полученные данные и сохраним в файл
    QSqlQuery *query_export = new QSqlQuery(db);
    query_export->exec( sql );
    while (query_export->next()) {
        for (int j=0; j<cnt_head; j++) {
            if (table->columnWidth(j)>5) {
                /*QString value = query_export->value(j).toString().trimmed().replace(";",",");
                if (!value.isEmpty()) {
                    if (j==51)  stream << "'";
                    stream << value;
                    if (j==51)  stream << "'";
                }
                if (j<cnt_head-1)  stream << ";";*/

                QString value;
                if (query_export->value(j).type()==QMetaType::QDate) {
                    value = query_export->value(j).toDate().toString("dd.MM.yyyy");
                } else {
                    value = query_export->value(j).toString().trimmed().replace(";",",");
                }
                if (!value.isEmpty()) {
                    //stream << "'";
                    stream << value;
                    //stream << "'";
                }
                if (j<cnt_head-1)  stream << ";";
            }
        }
        stream << "\n";
    }
    file.close();

    // ===========================
    // открытие полученного CSV
    // ===========================

    long result = (long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_s.utf16()), 0, 0, SW_NORMAL);

    return true;
}
