#ifndef EXEC_SQL
#define EXEC_SQL

#include <QtSql>
#include <QMessageBox>

#include "s_data_app.h"
#include <QApplication>
#include <QSqlQuery>
#include <QTableView>
#include <QFileDialog>

//#include "beep_sql.h"

#include "qt_windows.h"
#include "qwindowdefs_win.h"
#include <shellapi.h>


class exec_SQL {
public:
    // проверка соединения с базой данных
    bool prepare(QWidget *parent, QSqlDatabase &db, s_data_app &data_app) {

        bool try_connect = false;
        int try_cnt=0;

        while (!try_connect) {
            // обновим собственную сессию
            // закроем устаревшие сессии
            QString sql_upd_session = "update db_sessions "
                                      " set dt_last=CURRENT_TIMESTAMP, "
                                      "     status=1, "
                                      "     dt_end=NULL "
                                      " where id=" + QString::number(data_app.id_session) + " ; "

                                      "update db_sessions "
                                      " set dt_end=CURRENT_TIMESTAMP, "
                                      "     status=-2 "
                                      " where status>0 and (dt_last + '30 minutes'::INTERVAL)<CURRENT_TIMESTAMP ; ";

            QSqlQuery query_upd_session(db);
            try_connect = query_upd_session.exec(sql_upd_session);
            query_upd_session.clear();

            if (!try_connect) {
                QString msg = "Доступ к базе полисов ограничен. " + QString(try_cnt>0 ? (QString("(") + QString::number(try_cnt) + "х)") : "") + "\n\n"
                              "Рекомендации:\n";

                if (try_cnt>-1) {
                    msg += "  1) Подождите пару минут и повторите попытку - нажмите кнопку [Retry].\n\n";
                }
                if (try_cnt>1) {
                    msg += "  2) Возможно нет доступа к сети Internet - попробуйте зайти на сайт wwww.ya.ru \n"
                           "     с помощью доступного браузера или обратитесь к сетевому администратору.\n\n";
                }
                if (try_cnt>3) {
                    msg += "  3) Возможно соединение с базой полисов потеряно в связи с перегрузкой \n"
                           "     сервера ИНКО-МЕД - попробуйте пересоздать соединение кнопкой [Reset].\n"
                           "     (это может привести к потере последних изменеий)\n\n";
                }
                if (try_cnt>5) {
                    msg += "  4) Возможно, в данный момент сервер полисов ИНКО-МЕД не работает.\n"
                           "     Свяжитесь с технической поддержкой по телефону 8(920)449-54-88\n";
                }

                int bn = QMessageBox::critical(parent, " ~ ~  Нет доступа к базе полисов!!!  ~ ~ ", msg,
                                               QMessageBox::Retry|QMessageBox::Reset|QMessageBox::Abort,
                                               QMessageBox::Retry);

                if (bn==QMessageBox::Reset) {
                    if (QMessageBox::critical(parent, "Нужно подтверждение",
                                              "Вы действительно хотите перезапустить подключение к базе полисов?\n"
                                              "Последние изменения могут быть не сохранены!",
                                              QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes) {

                        return reset_connection(db, data_app);
                    }
                }
                if (bn==QMessageBox::Abort) {
                    return false;
                }
            }

            try_cnt++;
        }

        return true;
    }

    bool reset_connection(QSqlDatabase &db, s_data_app &data_app) {
        db.close();

        // создадим подключение к базе полисов
        db.setConnectOptions();
        // сервер, на котором расположена БД
        db.setHostName(data_app.host);
        // порт
        db.setPort(data_app.port);
        // имя базы данных
        db.setDatabaseName(data_app.base);
        // имя пользователя - postgres
        db.setUserName(data_app.user);
        // пароль для подключения к БД
        db.setPassword(data_app.pass);

        while (!db.open()) {
            // подключение не удалось
            QString pass_ = data_app.pass;
            if (QMessageBox::question(NULL,
                                      "Подключение не удалось",
                                      "Подключение не удалось\n\nDB: " + db.lastError().databaseText() +
                                      "\n\ndriver: "+db.lastError().driverText()+
                                      "\n\nПараметры: h=" + data_app.host + ":" + QString::number(data_app.port) + ", u=" + data_app.user + ", p=" + pass_.replace(QRegExp("[A-Z,a-z,0-9]"),"*") +
                                      "\n\nПовторить попытку?",
                                      QMessageBox::Yes|QMessageBox::No)==QMessageBox::No) {
                break;
            }

        }
        if (db.isOpen()) {

            exec_SQL mySQL;

            // запись терминала
            QString sql_terminal = "select id "
                                   " from db_terminals "
                                   " where name='INKO_POLISES  --  " + data_app.point_name + "  --  " + data_app.operator_fio + "' ; ";
            QSqlQuery *query_terminal = new QSqlQuery(db);
            /*bool res_terminal =*/ mySQL.exec(NULL, sql_terminal, "Получим ID терминала", *query_terminal, true, db, data_app);
            if (!query_terminal->next()) {
                sql_terminal = "insert into db_terminals(name) "
                               " values('INKO_POLISES  --  " + data_app.point_name + "  --  " + data_app.operator_fio + "') "
                               " returning id ; ";
                /*res_terminal =*/ mySQL.exec(NULL, sql_terminal, "Добавим терминал", *query_terminal, true, db, data_app);
            };
            data_app.terminal_id = query_terminal->value(0).toInt();


            // дата/время подключения терминала
            sql_terminal = "update db_terminals "
                           " set dt_last=CURRENT_TIMESTAMP "
                           " where id=" + QString::number(data_app.terminal_id) + " ; ";
            /*res_terminal =*/ mySQL.exec(NULL, sql_terminal, "Добавим терминал", *query_terminal, true, db, data_app);

            // сформируем запись сессии
            QString sql_session = "insert into db_sessions(id_point, id_operator, id_terminal, dt_start, dt_last, status, dt_end) "
                                  " values(" + QString::number(data_app.id_point) + ", " + QString::number(data_app.id_operator) + ", " + QString::number(data_app.terminal_id) + ", CURRENT_TIMESTAMP, CURRENT_TIMESTAMP, 1, NULL) "
                                  " returning id ; ";
            QSqlQuery *query_session = new QSqlQuery(db);
            /*bool res_session =*/ mySQL.exec(NULL, sql_session, "Добавим сессию", *query_session, true, db, data_app);
            query_session->next();
            data_app.id_session = query_session->value(0).toInt();


            // закрытие устаревших сессий
            QString sql_close_old = "update db_sessions "
                                    " set status=-2,"
                                    "     dt_end=CURRENT_TIMESTAMP "
                                    " where (dt_last + '3 hour'::INTERVAL)<CURRENT_TIMESTAMP ; ";
            QSqlQuery *query_close_old = new QSqlQuery(db);
            /*bool res_close_old = */mySQL.exec(NULL, sql_close_old, "Добавим сессию", *query_close_old, true, db, data_app);

            return true;
        } else {
            return false;
        }
    }

    // перевод массива байт в строку bytea PostgreSQL
    bool myBYTEA(QString &fname, QString& res_str) {
        try {
            QString ms = "0123456789ABCDEF";
            QFile f(fname);
            if (!f.open(QIODevice::ReadOnly)) {
                return false;
            }
            res_str = "\\x";
            int m = 0;
            char cd[1024];
            while (!f.atEnd()) {
                int n = f.read(cd, 1024);
                for (int i=0; i<n; i++) {
                    unsigned int v = (unsigned char)(cd[i]);
                    int n1 = v>>4;
                    int n2 = v%16;
                    res_str += ms.at(n1);
                    res_str += ms.at(n2);
                }
                m++;
            }
            return true;

        } catch (...) {
            return false;
        }
    }

    bool myUnBYTEA(QString& str, QString &res_fname) {
        try {
            //QString ms = "0123456789ABCDEF";
            QFile f(res_fname);
            if (!f.open(QIODevice::WriteOnly)) {
                return false;
            }
            char* array = (char*) malloc(1024);
            int n_arr = 0;
            for (int i=0; i<str.size(); i+=2) {
                if (n_arr>=1024) {
                    // запись массива
                    f.write(QByteArray((const char*)array, n_arr));
                    // очистка массива
                    for (int i=0; i<1024; i++)  array[i] = 0;
                    n_arr = 0;
                }

                QString  c0 = str.at(i);
                int n = 0;
                if (c0=="0")      n+= 0;
                else if (c0=="1") n+= 1;
                else if (c0=="2") n+= 2;
                else if (c0=="3") n+= 3;
                else if (c0=="4") n+= 4;
                else if (c0=="5") n+= 5;
                else if (c0=="6") n+= 6;
                else if (c0=="7") n+= 7;
                else if (c0=="8") n+= 8;
                else if (c0=="9") n+= 9;
                else if (c0=="A") n+=10;
                else if (c0=="B") n+=11;
                else if (c0=="C") n+=12;
                else if (c0=="D") n+=13;
                else if (c0=="E") n+=14;
                else if (c0=="F") n+=15;
                n<<=4;
                QString  c1 = str.at(i+1);
                if (c1=="0")      n+= 0;
                else if (c1=="1") n+= 1;
                else if (c1=="2") n+= 2;
                else if (c1=="3") n+= 3;
                else if (c1=="4") n+= 4;
                else if (c1=="5") n+= 5;
                else if (c1=="6") n+= 6;
                else if (c1=="7") n+= 7;
                else if (c1=="8") n+= 8;
                else if (c1=="9") n+= 9;
                else if (c1=="A") n+=10;
                else if (c1=="B") n+=11;
                else if (c1=="C") n+=12;
                else if (c1=="D") n+=13;
                else if (c1=="E") n+=14;
                else if (c1=="F") n+=15;
                array[n_arr] = (INT8)n;
                n_arr++;
            }
            if (n_arr>0) {
                // запись массива
                f.write(QByteArray(array, n_arr));
            }
            f.close();
            return true;

        } catch (...) {
            return false;
        }
    }



    // перевод массива байт в строку bytea PostgreSQL - 2
    bool myBYTEA2(QString str, QString& res_str) {
        try {
            QString ms = "@!UWYSG7$%0'^#=M";
            res_str = "";
            QByteArray cd = str.toLocal8Bit();
            int n = str.size(); //f.read(cd, 1024);
            for (int i=0; i<n; i++) {
                unsigned int v = cd.at(i);
                int n1 = v>>4;
                int n2 = v%16;
                res_str += ms.at(n1);
                res_str += ms.at(n2);
            }
            return true;
        } catch (...) {
            return false;
        }
    }

    bool myUnBYTEA2(QString& str, QString& str_res) {
        try {
            //QString ms = "@!UWYSG7$%0'^#=M";
            QByteArray array;
            array.clear();  //= QByteArray(1024,(char)0);    //(char*) malloc(1024);

            int n_arr = 0;
            for (int i=0; i<str.size() && i<2044; i+=2) {
                int n = 0;
                QString  c0 = str.at(i);
                if (c0=="@")      n+= 0;
                else if (c0=="!") n+= 1;
                else if (c0=="U") n+= 2;
                else if (c0=="W") n+= 3;
                else if (c0=="Y") n+= 4;
                else if (c0=="S") n+= 5;
                else if (c0=="G") n+= 6;
                else if (c0=="7") n+= 7;
                else if (c0=="$") n+= 8;
                else if (c0=="%") n+= 9;
                else if (c0=="0") n+=10;
                else if (c0=="'") n+=11;
                else if (c0=="^") n+=12;
                else if (c0=="#") n+=13;
                else if (c0=="=") n+=14;
                else if (c0=="M") n+=15;
                n<<=4;
                QString  c1 = str.at(i+1);
                if (c1=="@")      n+= 0;
                else if (c1=="!") n+= 1;
                else if (c1=="U") n+= 2;
                else if (c1=="W") n+= 3;
                else if (c1=="Y") n+= 4;
                else if (c1=="S") n+= 5;
                else if (c1=="G") n+= 6;
                else if (c1=="7") n+= 7;
                else if (c1=="$") n+= 8;
                else if (c1=="%") n+= 9;
                else if (c1=="0") n+=10;
                else if (c1=="'") n+=11;
                else if (c1=="^") n+=12;
                else if (c1=="#") n+=13;
                else if (c1=="=") n+=14;
                else if (c1=="M") n+=15;
                array.append((char)n);
                n_arr++;
            }
            array[n_arr] = (INT8)0;
            str_res = QString::fromLocal8Bit(array); //(QByteArray(array));
            return true;

        } catch (...) {
            return false;
        }
    }





    // запись кода ОКАТО в db_recent для пишущих запросов
    bool init_update(QWidget *parent, QSqlQuery &query, s_data_app &data_app) {
        QString sql = "truncate db_recent ; "
                      "insert into db_recent(ocato, id_point, id_operator, dt_act) "
                      "values( '" + data_app.ocato + "', "
                      "         " + QString::number(data_app.id_point) + ", "
                      "         " + QString::number(data_app.id_operator) + ", "
                      "         CURRENT_TIMESTAMP ) ; ";
        bool res = query.exec(sql);
        if (!res || !query.next()) {
            QMessageBox::critical(parent,
                                  "Ошибка инициализации пишущего запроса!",
                                  "В процессе инициализации пишущего запроса произошла ошибка.\n"
                                  "Возможно, нарушеа структура таблицы db_recent базы полисов.\n"
                                  "Проверьте соединение с сетью Интернет и работоспособность сервера полисов.");
            query.clear();
            return false;
        }
        query.clear();
        return true;
    }

    bool exec(QWidget *parent, QString &sql, QString caption, QSqlQuery &query, bool f_ret, QSqlDatabase &db, s_data_app &data_app) {
        // проверка стабильности соединения с БД
        if (!prepare(parent, db, data_app)) {
            return false;
        }
        // подготовка записи в журнал
        QString log_s = QTime::currentTime().toString("hh:mm:ss.zzz") + " - " + caption;
        QString log_s2 = "";
        bool log_f = false;

        // запись кода ОКАТО в db_recent для пишущих запросов
        QRegExp exp("\b(insert|update|delete|truncate)\b");
        if (exp.indexIn(sql)>=0) {
            if (!init_update(parent, query, data_app))
                return false;
        }

        //beep_sql_wnd *beep_sql_w = new beep_sql_wnd(NULL);

        // собственно пробуем выполнить запрос sql
        try {
            bool res = false;
            if (f_ret) {
                do {
                    QTime time_0;
                    time_0.start();
    m_repeate:
                    res = query.exec(sql);
                    int elapsed = time_0.elapsed();

                    if (elapsed>data_app.log_bord) {
                        // журналирование
                        int el_min = elapsed/60000;
                        int el_sec = (elapsed - el_min*60000)/1000;
                        int el_mls = (elapsed - el_min*60000 - el_sec*1000);
                        log_s2 += "  [R]  >  "
                                  + QString::number(el_min) + ":"
                                  + QString("00" + QString::number(el_sec)).right(2) + "."
                                  + QString("0000" + QString::number(el_mls)).right(4)
                                  + "  >  " + QString(res ? "Ok" : "failed") + ";  ";
                        log_f = true;
                    }
                    if (res)
                        break;

                    int bn = QMessageBox::warning (parent, "Ошибка SQL ...", caption + "\n\n"
                                              "Запрос:\n" +
                                              sql + "\n\nОшибка:\ndriver: " +
                                              query.lastError().databaseText() + "\nDB: " +
                                              query.lastError().driverText() + "\nТелефон для связи с разработчиком: 8(920)449-54-88 \n\n"
                                              "Попробуйте выполнить операцию ещё раз.\n"
                                              "Если ошибка повторится, снимите скриншот с этого окна и старшего окна, и направьте разаработчику с описанием ситуации, в которой произошла ошибка.\n\n",
                                              QMessageBox::Retry|QMessageBox::Reset|QMessageBox::Abort, QMessageBox::Retry);
                    if (bn==QMessageBox::Reset) {
                        reset_connection(db,data_app);
                    }
                    if (bn==QMessageBox::Abort) {
                        //beep_sql_w->hide();
                        //delete beep_sql_w;
                        return false;
                    }
                } while (!res);

            } else {
                QTime time_0;
                time_0.start();

                res = query.exec(sql);
                int elapsed = time_0.elapsed();

                if (elapsed>data_app.log_bord) {
                    // журналирование
                    int el_min = elapsed/60000;
                    int el_sec = (elapsed - el_min*60000)/1000;
                    int el_mls = (elapsed - el_min*60000 - el_sec*1000);
                    log_s2 += "  [1]  >  "
                              + QString::number(el_min) + ":"
                              + QString("00" + QString::number(el_sec)).right(2) + "."
                              + QString("0000" + QString::number(el_mls)).right(4)
                              + "  >  " + QString(res ? "Ok" : "failed") + ";  ";
                    log_f = true;
                }

                if (!res) {
                    QMessageBox::critical (parent, "Нестабильное соединение или ошибка SQL", caption + "\n\nПри выполнении запроса SQL произошла ошибка.\n\nЗапрос\n" +
                                           sql + "\n\nОшибка\n" +
                                           query.lastError().databaseText() + "\n" +
                                           query.lastError().driverText() + "\n\nТелефон для связи с разработчиком: 8(920)449-54-88 \n");
                }
            }

            if (log_f) {
                // сохранение записи в журнал
                char br[] = {13,10};
                QDate date = QDate::currentDate();
                QFile log(data_app.path_install + "/_LOG_/" + date.toString("yyyy-MM-dd") + ".log");
                log.open(QIODevice::Append);
                log.write(log_s.toLocal8Bit());
                log.write(br,2);
                log.write(sql.toLocal8Bit());
                log.write(br,2);
                log.write(log_s2.toLocal8Bit());
                log.write(br,2);
                log.write(br,2);
                log.close();
            }

            //beep_sql_w->hide();
            //delete beep_sql_w;
            return res;

        } catch (...) {
            return false;
        }
    }

    bool exec(QWidget *parent, QString &sql, QString caption, QSqlQuery *query, bool f_ret, QSqlDatabase &db, s_data_app &data_app) {
        return exec(parent, sql, caption, *query, f_ret, db, data_app);
    }



    // стандартная процедурка для сохранения таблицы в CSV
    bool tab_to_CSV(QSqlQueryModel &model, QTableView *table) {
        QString fname_s = QFileDialog::getSaveFileName(NULL,
                                                       "Куда сохранить файл?",
                                                       QString(),
                                                       "файлы данных (*.csv);;простой текст (*.txt)");
        if (fname_s.isEmpty()) {
            QMessageBox::warning(NULL, "Не выбрано имя файла",
                                 "Имя цедевого файла не задано.\n\n"
                                 "Операция отменена.");
            return false;
        }
        if (QFile::exists(fname_s)) {
            if (QMessageBox::question(NULL, "Удалить старый файл?",
                                      "Файл уже существует.\n"
                                      "Удалить старый файл для того чтобы сохранить новый?",
                                      QMessageBox::Yes|QMessageBox::Cancel)==QMessageBox::Yes) {
                if (!QFile::remove(fname_s)) {
                    QMessageBox::warning(NULL, "Ошибка при удалении старого файла",
                                         "При удалении старого файла произошла непредвиденная ошибка.\n\n"
                                         "Операция отменена.");
                    return false;
                }
            }
        }

        try {
            // сохраним данные в файл
            QFile file;
            file.setFileName(fname_s);

            if (!file.open(QIODevice::WriteOnly)) {
                QMessageBox::warning(NULL, "Ошибка создания файла",
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
            int cnt_head = model.columnCount();
            for (int i=0; i<cnt_head; i++) {
                if (table->columnWidth(i)>1) {
                    head += model.headerData(i,Qt::Horizontal).toString().replace("\n"," ");
                    if (i<cnt_head-1)  head += ";";
                }
            }
            stream << head << "\n";

            // переберём полученные данные и сохраним в файл
            int cnt_rows = model.rowCount();
            for (int i=0; i<cnt_rows; i++) {
                for (int j=0; j<cnt_head; j++) {
                    if (table->columnWidth(j)>1) {
                        // данные застрахованного
                        QString s = model.data(model.index(i, j), Qt::EditRole).toString().replace("\n"," ");
                        stream << s;
                        if (j<cnt_head-1)  stream << ";";
                    }
                }
                stream << "\n";
            }
            file.close();

            // ===========================
            // собственно открытие шаблона
            // ===========================
            // открытие полученного ODT
            long result = (long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_s.utf16()), 0, 0, SW_NORMAL);
            if (result<32) {
                QMessageBox::warning(NULL,
                                     "Ошибка при открытии сохранённого файла",
                                     "Сохранённый файл CSV не найден:\n\n" + fname_s +
                                     "\nКод ошибки: " + QString::number(result) +
                                     "\n\nПопробуйте открыть этот файл вручную.\n\nЕсли файл не создан или ошибка будет повторяться - обратитесь к разработчику.");
            }
            return true;

        } catch (...) {
            return false;
        }
    }

};


#endif // EXEC_SQL
