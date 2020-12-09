#include "get_from_fond_wnd.h"
#include "ui_get_from_fond_wnd.h"
#include <QFileDialog>
#include <QXmlStreamReader>

get_from_fond_wnd::get_from_fond_wnd(QSqlDatabase &db, s_data_app &data_app, QSettings &settings, QWidget *parent)
    : db(db), data_app(data_app), settings(settings), QDialog(parent), ui(new Ui::get_from_fond_wnd)
{
    ui->setupUi(this);
    ui->bn_process->setVisible(false);

    on_bn_files_clicked();
}


get_from_fond_wnd::~get_from_fond_wnd() {
    delete ui;
}

void get_from_fond_wnd::on_bn_close_clicked() {
    close();
}

void get_from_fond_wnd::on_bn_files_clicked() {
    // запрос файлов для обработки
    ui->lis_files->clear();
    QString selFilter="ZIP-архивы (*.zip)";
    QStringList fnames = QFileDialog::getOpenFileNames(this, "Выберите файлы из фонда для загрузки", data_app.path_in, "ZIP-архивы (*.zip)",&selFilter);
    for (int i=0; i<fnames.size(); i++) {
        ui->lis_files->addItem(fnames.at(i));
    }
    ui->bn_process->setVisible(fnames.size()>0);
}

QStringList get_from_fond_wnd::getDirFiles(const QString& dirName) {
    QDir dir( dirName );
    if ( !dir.exists() ) {
        QMessageBox::critical(this,"Ошибка при открытии папки", "Папка не найдена: " + dirName );
        return QStringList();
    }
    QStringList fileNames;
    QStringList fileList = dir.entryList( QDir::Files );
    for ( QStringList::Iterator fit = fileList.begin(); fit != fileList.end(); ++fit )
        fileNames.append( dir.path() + "/" + *fit );
    QStringList filters;
    filters << "*.zip";
    dir.setNameFilters(filters);
    QStringList dirList = dir.entryList(filters, QDir::NoDotAndDotDot, QDir::Name);
    for ( QStringList::Iterator dit = dirList.begin(); dit != dirList.end(); ++dit ) {
        QDir curDir = dir;
        curDir.cd( *dit );
        QStringList curList = getDirFiles( curDir.path() );
        for ( QStringList::Iterator it = curList.begin(); it != curList.end(); ++it )
            fileNames.append( QFileInfo(*it).filePath() );
    }
    return fileNames;
}

void get_from_fond_wnd::on_bn_process_clicked() {
    /*this->setCursor(Qt::WaitCursor);

    int count = ui->lis_files->count();
    for (int i=0; i<count; i++) {
        ui->lab_count->setText(QString::number(i) + " из " + QString::number(count));
        // распаковка архива
        /*if (data_app.ocato=="14000") {
            temp_dir_str += "rep_vs_belg";
        } else if (data_app.ocato=="38000") {
            temp_dir_str += "rep_vs_kursk";
        } else {
            temp_dir_str += "rep_vs";
        }* /
        QString fullname_zip = ui->lis_files->item(i)->text();
        ui->te_log->appendPlainText("-");
        ui->te_log->appendPlainText(fullname_zip);

        if (!QFile::exists(fullname_zip)) {
            QMessageBox::warning(this,
                                 "Файл не найден",
                                 "Не найден файл посылки из фонда: \n" + fullname_zip +
                                 "\n\nФайл пропущен.");
            ui->te_log->appendPlainText("\tНе найден файл посылки из фонда: " + fullname_zip + "  -> Файл пропущен.");
            continue;
        }
        // подготовка к созданию временной папки
        QString fname_zip = QFileInfo(fullname_zip).baseName();

        ui->lab_out->setText(fname_zip);
        QApplication::processEvents();

        QString temp_dir_str = data_app.path_temp + fname_zip;
        QDir temp_dir = QDir(temp_dir_str);
        if (temp_dir.exists() && !temp_dir.removeRecursively()) {
            QMessageBox::warning(this, "Ошибка очистки старых данных",
                                 "При попытке удалить старую временнную папку произошла непредвиденная ошибка.\n" +
                                 temp_dir_str +
                                 "\n\nФайл пропущен.");
            ui->te_log->appendPlainText("\tПри попытке удалить старую временнную папку произошла непредвиденная ошибка: " + temp_dir_str + "  -> Файл пропущен.");
        }
        temp_dir.mkpath(temp_dir_str);
        // распакуем архив
        QProcess myProcess;
        QString com = "\"" + data_app.path_arch + "7z.exe\" e \"" + fullname_zip + "\" -o\"" + temp_dir_str + "\"";
        if (myProcess.execute(com)!=0) {
            QMessageBox::warning(this,
                                 "Ошибка распаковки посылки",
                                 "Не удалось распаковать файл посылки\n" +
                                 fname_zip + "\n\n" +
                                 com +
                                 "\n\nФайл пропущен.");
            ui->te_log->appendPlainText("\tНе удалось распаковать файл посылки: " + temp_dir_str + "  -> Файл пропущен.");
            continue;
        }
        // перебор файлов в посылке
        QStringList files_in = getDirFiles(temp_dir_str);
        for (int j=0; j<files_in.size(); j++) {
            QString ftype = QFileInfo(files_in.at(j)).fileName().left(1).toUpper();

            ui->lab_out->setText(fname_zip + "  ->>  " + files_in.at(j) + "  (" + ftype + ")");
            QApplication::processEvents();

            // открытие xml-файла посылки в зависимости от типа
            if (ftype=="P") {
                if (files_in.at(j).right(6)=="_1.xml") {
                    // обработка файла ошибок
                } else {
                    // обработка протокола
                    process_P_file (files_in.at(j));
                }
            } else
            if (ftype=="S") {
                // обработка стоп-файла
                process_S_file (files_in.at(j));
            } else
            if (ftype=="K") {
                // обработка корректировки
                process_K_file (files_in.at(j));
            } else
            if (ftype=="F") {
                // обработка файла ФЛК
                process_F_file (files_in.at(j));
            } else
            if (ftype=="U") {
                // обработка файла УЭК
                process_U_file (files_in.at(j));
            }
        }
        // удаление временной папки
        if (temp_dir.exists() && !temp_dir.removeRecursively()) {
            QMessageBox::warning(this, "Ошибка очистки временных данных",
                                 "При попытке удалить временнную папку произошла ошибка.\n" +
                                 temp_dir_str +
                                 "\n\nПапка не удалена, работа продолжена.");
            ui->te_log->appendPlainText("\tПри попытке удалить временнную папку произошла ошибка: " + temp_dir_str + "  -> Папка не удалена, работа продолжена.");
        }
        // перенос обработанного файла в резервную папку
        if (!QFile::rename(fullname_zip, data_app.path_in_backup+QFileInfo(fullname_zip).fileName())) {
            QMessageBox::warning(this, "Ошибка перемещения обработанного файла в резервную папку",
                                 "При попытке переместить обработанный файл в резервную папку произошла ошибка.\n" +
                                 data_app.path_in_backup+fname_zip +
                                 "\n\nПапка не удалена, работа продолжена.");
            ui->te_log->appendPlainText("\tПри попытке переместить обработанный файл в резервную папку произошла ошибка: " + data_app.path_in_backup+fname_zip + "  -> Папка не удалена, работа продолжена.");
        }
    }

    ui->lab_out->setText("ОБРАБОТКА ЗАВЕРШЕНА");
    QApplication::processEvents();

    this->setCursor(Qt::ArrowCursor);*/
}
