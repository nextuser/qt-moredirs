#include "mainwindow.h"

#include <QApplication>
#include "stringutil.h"
#include <QTranslator>
#include <QLocale>
QTranslator translator;
int main(int argc, char *argv[])
{
    ////qDebug() << "matched" << StringUtil::matchWildcard("a*3*.c","abd3f.c");

    QApplication a(argc, argv);
    const QLocale locale = QLocale::system();
    QString qmFile = QM_FILE_EN;
    if(locale.language() == QLocale::Chinese){
        qmFile = QM_FILE_CN;
    }
    //qDebug() << "langcode:" <<  QLocale::languageToCode(locale.language()) << "terrcode:" << QLocale::territoryToCode(locale.territory());
    //qDebug() << "langcode:" <<  QLocale::languageToCode(QLocale::English) << "terricode" << QLocale::territoryToCode(QLocale::UnitedStates);
    //qDebug() << "langcode:" <<  QLocale::languageToCode(QLocale::Chinese) << "terricode" << QLocale::territoryToCode(QLocale::Taiwan);

    a.setOrganizationName("ljlhome");
    a.setApplicationName("moredirs");
    a.setWindowIcon(QIcon(":/new/icons/icons/moredirs-48.png"));

    bool succ = translator.load(qmFile);
    if(succ) {
        a.installTranslator(&translator);
    }

    MainWindow w;
    w.show();

    return a.exec();
}
