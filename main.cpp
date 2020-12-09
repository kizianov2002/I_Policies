#include <QApplication>
#include <QtPlugin>
#include <QImageReader>
#include <QMessageBox>
#include <QDesktopWidget>

#include "connection_window.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QStringList PathList = a.libraryPaths();
    PathList.insert(PathList.begin(), a.applicationDirPath() + "/imageformats");
    PathList.append(a.applicationDirPath() + "/plugins");
    a.setLibraryPaths(PathList);

    QString s = "";
    QList<QByteArray> l= QImageReader::supportedImageFormats ();
    int n = l.size();
    for (int i=0; i<n; i++) {
        s.append("   - ");
        s.append(l.at(i));
        s.append("\n");
    }

    ConnectionWindow *w = new ConnectionWindow(NULL);

    if (n<10)
        QMessageBox::information(w,
                                 "imageformats",
                                 "Внимание!\n"
                                 "Доступно ограниченное количество графических форматов.\n"
                                 "Проверьте доступность папки imageformats в рабочей папке программы.\n\n"
                                 "Доступны форматы:\n" + s);

    // проверим разрешение экрана
    QDesktopWidget *scr_widget = QApplication::desktop();
    int scr_w = scr_widget->width();
    int scr_h = scr_widget->height();
    if (scr_w<1280 || scr_h<1024) {
        QMessageBox::warning(w, "Слишком низкое разрешение экрана!",
                             "Внимание!\n\n"
                             "Минимальное необходимое разрешение экрана - 1280 x 1024\n"
                             "Разрешение вашего монитора - " + QString::number(scr_w) + " x " + QString::number(scr_h) + "\n\n"
                             "Стабильная работа программы и корректное отображение элементов экранного интерфейса не гарантируется.\nПовысьте разрешение экрана вашего монитора и перезапустите программу.");
        w->setWindowState( w->windowState() | Qt::WindowFullScreen);
    }
    w->show();

    return a.exec();
}
