#include "ki_funcs.h"

#include <QDate>
#include <QSqlQuery>
#include <QTextCodec>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QSqlQueryModel>
#include <QSqlRecord>
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

QString month_to_str_vp(QDate date) {
    QString res;
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
    return res;
}

QString month_to_str(QDate date) {
    QString res;
    switch (date.month()) {
    case 1:
        res += "январь ";
        break;
    case 2:
        res += "февраль ";
        break;
    case 3:
        res += "март ";
        break;
    case 4:
        res += "апрель ";
        break;
    case 5:
        res += "май ";
        break;
    case 6:
        res += "июнь ";
        break;
    case 7:
        res += "июль ";
        break;
    case 8:
        res += "август ";
        break;
    case 9:
        res += "сентябрь ";
        break;
    case 10:
        res += "октябрь ";
        break;
    case 11:
        res += "ноябрь ";
        break;
    case 12:
        res += "декабрь ";
        break;
    default:
        break;
    }
    return res;
}


QString date_to_str(QDate date) {
    QString date_s = " «" + QString::number(date.day()) + "» " + month_to_str_vp(date) + " " + QString::number(date.year()) + " г.";
    return date_s;
}


QString monthyear_to_str(QDate date) {
    QString date_s = month_to_str(date) + " " + QString::number(date.year()) + " года";
    return date_s;
}


bool table2csv(QSqlDatabase &db, QString sql, QSqlQueryModel &model, QTableView *table, QString path, QWidget *parent, QStringList names_old, QStringList names_new) {
    QString fname_s = QFileDialog::getSaveFileName(parent,
                                                   "Куда сохранить файл?",
                                                   path,
                                                   "файлы данных (*.csv);;простой текст (*.txt)");
    if (fname_s.isEmpty()) {
        QMessageBox::warning(parent,
                             "Не выбрано имя файла",
                             "Имя целевого файла не задано.\n\n"
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
                if (query_export->value(j).isNull()) {
                    value = "";
                } else if (query_export->value(j).type()==QMetaType::QDate) {
                    value = query_export->value(j).toDate().toString("dd.MM.yyyy");
                } else if ( query_export->value(j).type()==QMetaType::Int
                            || query_export->value(j).type()==QMetaType::Long
                            || query_export->value(j).type()==QMetaType::LongLong
                            || query_export->value(j).type()==QMetaType::ULong
                            || query_export->value(j).type()==QMetaType::ULongLong ) {
                    qlonglong n = query_export->value(j).toLongLong();
                    if (n==0)  value = "";
                    else  value = QString::number(n);
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




bool sql2csv(QSqlDatabase &db, QString sql, QString fname_s, QWidget *parent, QStringList names_old, QStringList names_new) {
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
        QString fname_s = QFileDialog::getSaveFileName(parent,
                                                       "Куда сохранить файл?",
                                                       fname_s,
                                                       "файлы данных (*.csv);;простой текст (*.txt)");
        if (fname_s.isEmpty()) {
            QMessageBox::warning(parent,
                                 "Не выбрано имя файла",
                                 "Имя целевого файла не задано.\n\n"
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
    }

    // подготовим поток для сохранения файла
    QTextStream stream(&file);
    QString codec_name = "Windows-1251";
    QTextCodec *codec = QTextCodec::codecForName(codec_name.toLocal8Bit());
    stream.setCodec(codec);

    // переберём полученные данные и сохраним в файл
    QSqlQuery *query_export = new QSqlQuery(db);
    query_export->exec( sql );

    QString head;
    QString head_word;

    QSqlRecord rec = query_export->record();
    int cnt_head = rec.count();
    for (int i=0; i<cnt_head; i++) {
        head_word = rec.fieldName(i);
        // проверим, надо ли заменить заголовок
        for (int i=0; i<names_old.size() && i<names_new.size(); i++) {
            if (head_word==names_old.at(i))
                head_word = names_new.at(i);
        }
        head += head_word;
        if (i<cnt_head-1)  head += ";";
    }
    stream << head << "\n";

    while (query_export->next()) {
        for (int j=0; j<cnt_head; j++) {

            QString value;
            if (query_export->value(j).isNull()) {
                value = "";
            } else if (query_export->value(j).type()==QMetaType::QDate) {
                value = query_export->value(j).toDate().toString("dd.MM.yyyy");
            } else if ( query_export->value(j).type()==QMetaType::Int
                        || query_export->value(j).type()==QMetaType::Long
                        || query_export->value(j).type()==QMetaType::LongLong
                        || query_export->value(j).type()==QMetaType::ULong
                        || query_export->value(j).type()==QMetaType::ULongLong ) {
                qlonglong n = query_export->value(j).toLongLong();
                if (n==0)  value = "";
                else  value = QString::number(n);
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
        stream << "\n";
    }
    file.close();

    return true;
}



bool read_csvData(QString &text_s, QVector<QStringList> &data_lists, int &data_lists_cnt, QWidget *parent) {
    // очистим массив данных
    for (int i=0; (i<data_lists.size()); i++)
        data_lists.value(i).clear();
    data_lists_cnt = data_lists.size();

    // скопируем данные из текстового поля
    QStringList text_list = text_s.split("\n", QString::SkipEmptyParts);

    if (text_list.size()<1) {
        QMessageBox::warning(parent, "Данные не найдены",
                             "Поле текста пустое?\n\n"
                             "Скопируйте данные для загрузки в текстовое поле и повторите операцию. \n\n"
                             "Операция отменена.");
        return false;
    }

    QString div_fields = "\t";

    // перебор строк
    int line_counter = 0;
    for (int i=0; i<text_list.size(); i++) {
        QString values_str = text_list.at(i);
        values_str = values_str.trimmed();
        QString s1 = values_str.left(31);
        QString s2 = values_str.left(2);

        if ( values_str.isEmpty() )
            continue;

        // разберём список полей
        QString value;
        int len = values_str.length(), pos = 0;
        int n_value = 0;

        while (pos<len && pos!=-1) {
            int pos1 = values_str.indexOf(div_fields, pos);
            if (pos1<0)  pos1 = len;
            value = values_str.mid(pos, pos1-pos).simplified().trimmed();
            value = value.replace("\"", "").replace("'", "");

            // если надо - добавим столбец
            while (data_lists_cnt<=n_value) {
                data_lists.append(QStringList());
                data_lists_cnt = data_lists.size();
            }

            // добавим значение
            data_lists[n_value].append(value);

            if (pos1<0 || pos1==len) break;
            else pos = pos1 + div_fields.length();

            n_value++;
        }
        line_counter++;
    }

    return true;
}
