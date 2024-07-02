#ifndef SEARCHTHREAD_H
#define SEARCHTHREAD_H

#include <QThread>
#include <QDir>
#include <string>
#include <regex>
class SearchThread : public QThread
{
    Q_OBJECT
public:
    SearchThread();
    void  stop();
    void  findFile(QString location, QString filter);

Q_SIGNALS:

    void  file_found(QString files);
    void  find_finished();

private :
    bool m_stop;
    QString m_filter;
    QString m_location;
    void findInDir(const QDir &dir, const QList<std::regex>& regs);
    // QThread interface
protected:
    void run();


};

#endif // SEARCHTHREAD_H
