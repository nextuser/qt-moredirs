#ifndef FILECLIPBOARD_H
#define FILECLIPBOARD_H

#include <QObject>
#include <QList>
#include <QUrl>
#include <QWidget>
#include "filethread.h"
#include "copyprocessdialog.h"
class FileClipboard : public QObject
{
    Q_OBJECT
public:
    explicit FileClipboard(QWidget *parent =nullptr);
    ~FileClipboard(){
        releaseThread();
    }

signals:


public slots:
    void on_copyUrls(QList<QUrl> urls,QString text);

    void on_cutUrls(QList<QUrl> urls,QString text);

    void on_paste(QString destDir);
private :
    QList<QUrl> m_cutUrls;
    FileThread *m_fileThread = nullptr;

    CopyProcessDialog *m_dlg = nullptr;
    void  doCopyUrls(const QList<QUrl> &urls,QString text,bool isCut);
    void  copyInProces(QString srcPath, QString targetParentDir);
    void  releaseThread();
};

#endif // FILECLIPBOARD_H
