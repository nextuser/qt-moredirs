#include "mainwindow.h"

#include <QApplication>
#include "stringutil.h"
int main(int argc, char *argv[])
{
    ////qDebug() << "matched" << StringUtil::matchWildcard("a*3*.c","abd3f.c");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    a.setOrganizationName("ljlhome");
    a.setApplicationName("moredirs");
    a.setWindowIcon(QIcon(":/icons/icons/icons8-file-explorer-48.png"));
    return a.exec();
}
