#ifndef FILECLIPBOARD_H
#define FILECLIPBOARD_H

#include <QObject>
#include <QList>
#include <QUrl>

class FileClipboard : public QObject
{
    Q_OBJECT
public:
    explicit FileClipboard();


signals:


public slots:
    void on_copyUrls(QList<QUrl> urls,QString text);

    void on_cutUrls(QList<QUrl> urls,QString text);

    void on_paste(QString destDir);
private :
    QList<QUrl> m_cutUrls;
    void  doCopyUrls(const QList<QUrl> &urls,QString text,bool isCut);
};

#endif // FILECLIPBOARD_H
