#include "show_tab_wnd.h"
#include "ui_show_tab_wnd.h"

show_tab_wnd::show_tab_wnd(QString header, QString &sql, QSqlDatabase &db, s_data_app &data_app, QWidget *parent, QString default_folder, bool f_hist) :
    header(header),
    sql(sql),
    db(db),
    data_app(data_app),
    QDialog(parent),
    default_folder(default_folder),
    f_hist(f_hist),
    ui(new Ui::show_tab_wnd)
{
    ui->setupUi(this);

    ui->splitter->setStretchFactor(0,5);
    ui->splitter->setStretchFactor(1,1);

    sql_show = sql;
    ui->te_sql->setText(sql_show);

    sql = sql.replace(";"," ");
    sql = sql.replace("\n"," ");

    this->setWindowTitle(header);
    refresh_tab();
}

show_tab_wnd::~show_tab_wnd() {
    delete ui;
}

void show_tab_wnd::refresh_tab() {
    this->setCursor(Qt::WaitCursor);
    if (!db.isOpen()) {
        QMessageBox::warning(this, ("Нет доступа к базе данных"), ("Нет открытого соединения к базе данных?\n Это ненормальная ситуация и вы не должны были увидеть этого сообщения..(\n Перезапустите программу. Если это сообщение будет повторяться - сообщите разработчику."));
        return;
    }

    if (f_hist) {
        model_hist.setQuery(sql,db);
        QString err2 = model_hist.lastError().driverText();

        // подключаем модель из БД
        ui->tab_sql->setModel(&model_hist);
    } else {
        model_tab.setQuery(sql,db);
        QString err2 = model_tab.lastError().driverText();

        // подключаем модель из БД
        ui->tab_sql->setModel(&model_tab);
    }

    // обновляем таблицу
    ui->tab_sql->reset();
    ui->tab_sql->repaint();


    // подсчёт числа строк
    QSqlQuery *query = new QSqlQuery(db);
    QString sql_cnt = "select count(*) from (" + sql + ") q ;";

    if (!(mySQL.exec(this, sql_cnt, QString("Подсчёт числа строк в выборке"), *query, true, db, data_app))) {
        QMessageBox::warning(this, "Ошибка при подсчёте числа строк в выборке",
                             "При подсчёте числа строк в выборке произошла неожиданная ошибка.\n" + sql_cnt + "\n\nОперация отменена.");
    }
    query->next();
    int cnt = query->value(0).toInt();
    ui->lab_cnt->setText(QString::number(cnt));
    delete query;

    this->setCursor(Qt::ArrowCursor);
}

void show_tab_wnd::on_bn_close_clicked() {
    close();
}

void show_tab_wnd::on_bn_toCSV_clicked() {
    QString sql = this->sql_show;

    sql = sql.replace(";"," ");
    sql = sql.replace("\n"," ");

    QString fname_s = QFileDialog::getSaveFileName(this,
                                                   "Куда сохранить файл?",
                                                   (default_folder.isEmpty() ? data_app.path_out : default_folder) + QDate::currentDate().toString("yyyy-MM-dd") + "  -  " + data_app.smo_short + " - " + data_app.filial_name + "  -  " + this->windowTitle().replace(":","=").replace("*","@").replace("!","|") + ".csv",
                                                   "файлы данных (*.csv);;простой текст (*.txt)");
    if (fname_s.isEmpty()) {
        QMessageBox::warning(this,
                             "Не выбрано имя файла",
                             "Имя цедевого файла не задано.\n\n"
                             "Операция отменена.");
        return;
    }
    if (QFile::exists(fname_s)) {
        if (QMessageBox::question(this,
                                  "Удалить старый файл?",
                                  "Файл уже существует.\n"
                                  "Удалить старый файл для того чтобы сохранить новый?",
                                  QMessageBox::Yes|QMessageBox::Cancel)==QMessageBox::Yes) {
            if (!QFile::remove(fname_s)) {
                QMessageBox::warning(this,
                                     "Ошибка при удалении старого файла",
                                     "При удалении старого файла произошла непредвиденная ошибка.\n\n"
                                     "Операция отменена.");
                return;
            }
        }
    }


    // сохраним данные в файл
    QFile file;
    file.setFileName(fname_s);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this,
                             "Ошибка создания файла",
                             "При создании целевого файла произошла ошибка.\n\n"
                             "Операция отменена.");
        return;
    }

    // подготовим поток для сохранения файла
    QTextStream stream(&file);
    QString codec_name = "Windows-1251";
    QTextCodec *codec = QTextCodec::codecForName(codec_name.toLocal8Bit());
    stream.setCodec(codec);


    QString head;
    int cnt_head = 0;
    if (f_hist) {
        cnt_head = model_hist.columnCount();
        for (int i=0; i<cnt_head; i++) {
            if (ui->tab_sql->columnWidth(i)>1) {
                head += model_hist.headerData(i,Qt::Horizontal).toString().replace("\n"," ");
                if (i<cnt_head-1)  head += ";";
            }
        }
    } else {
        cnt_head = model_tab.columnCount();
        for (int i=0; i<cnt_head; i++) {
            if (ui->tab_sql->columnWidth(i)>1) {
                head += model_tab.headerData(i,Qt::Horizontal).toString().replace("\n"," ");
                if (i<cnt_head-1)  head += ";";
            }
        }
    }
    stream << head << "\n";

    // переберём полученные данные и сохраним в файл
    QSqlQuery *query_export = new QSqlQuery(db);
    query_export->exec(sql);
    while (query_export->next()) {
        for (int j=0; j<cnt_head; j++) {
            if (ui->tab_sql->columnWidth(j)>1) {
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
    // собственно открытие шаблона
    // ===========================
    /*long result = (long)*/ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(fname_s.utf16()), 0, 0, SW_NORMAL);
}
