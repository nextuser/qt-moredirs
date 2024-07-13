#ifndef SEARCHTHREAD_H
#define SEARCHTHREAD_H

#include <QThread>
#include <QDir>
#include <string>
#include <regex>
#include <QElapsedTimer>
class SearchThread : public QThread
{
    Q_OBJECT
public:
    SearchThread();
    void  stop();
    void  findFile(QString location, QString filter);

Q_SIGNALS:

    void  file_found(QStringList files,bool finished);


private :
    volatile bool m_stop;
    QString m_filter;
    QString m_location;
    QElapsedTimer m_elapsedTimer;
    void findInDir(const QDir &dir, const QList<std::regex>& regs,int &processCount ,QStringList& results);
    void incFound(int &processCount,QStringList& results, QString newFile,bool finished = false);
    // QThread interface
protected:
    void run();


};

#endif // SEARCHTHREAD_H
