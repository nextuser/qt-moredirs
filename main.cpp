#include "mainwindow.h"

#include <QApplication>
#include "stringutil.h"
#include <QTranslator>
QTranslator translator;
int main(int argc, char *argv[])
{
    ////qDebug() << "matched" << StringUtil::matchWildcard("a*3*.c","abd3f.c");

    QApplication a(argc, argv);
    MainWindow w;
    bool succ = translator.load("moredirs_en.qm");
    if(succ) {
        a.installTranslator(&translator);
    }
    // QString str = "\xE6\x96\x87\xE4\xBB\xB6\xE5\xB7\xB2\xE7\xBB\x8F\xE5\xAD\x98\xE5\x9C\xA8\n";
    // qDebug() << str;
    w.show();
    a.setOrganizationName("ljlhome");
    a.setApplicationName("moredirs");
    a.setWindowIcon(QIcon(":/new/icons/icons/moredirs-48.png"));
    return a.exec();
}
