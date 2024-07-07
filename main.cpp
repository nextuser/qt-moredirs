#include "mainwindow.h"

#include <QApplication>
#include "stringutil.h"

#include <QLocale>

int main(int argc, char *argv[])
{
    ////qDebug() << "matched" << StringUtil::matchWildcard("a*3*.c","abd3f.c");

    QApplication a(argc, argv);

    //qDebug() << "langcode:" <<  QLocale::languageToCode(locale.language()) << "terrcode:" << QLocale::territoryToCode(locale.territory());
    //qDebug() << "langcode:" <<  QLocale::languageToCode(QLocale::English) << "terricode" << QLocale::territoryToCode(QLocale::UnitedStates);
    //qDebug() << "langcode:" <<  QLocale::languageToCode(QLocale::Chinese) << "terricode" << QLocale::territoryToCode(QLocale::Taiwan);

    a.setOrganizationName("ljlhome");
    a.setApplicationName("moredirs");
    a.setWindowIcon(QIcon(":/new/icons/icons/moredirs-48.png"));


    MainWindow w(nullptr);
    w.show();

    return a.exec();
}
