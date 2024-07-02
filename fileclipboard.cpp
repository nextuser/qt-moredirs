#include "fileclipboard.h"
#include <QClipboard>
#include <QFileInfo>
#include <QMimeData>
#include <QApplication>
#include "fileutil.h"
FileClipboard::FileClipboard(QObject *parent)
    : QObject{parent}
{


}


void FileClipboard:: doCopyUrls(const QList<QUrl> &urls,QString text,bool isCut){
    QClipboard *clip = QApplication::clipboard();

    QMimeData *mime = new QMimeData();
    mime->setUrls(urls);
    mime->setText(text);
    clip->setMimeData(mime);
    if(isCut) m_cutUrls = urls;
    else m_cutUrls.clear();
}

void FileClipboard::on_copyUrls(const QList<QUrl> urls,QString text)
{
    doCopyUrls(urls,text,false);
}

void FileClipboard::on_cutUrls(const QList<QUrl> urls,QString text)
{
    doCopyUrls(urls,text,true);
}




void FileClipboard::on_paste(QString destDir)
{
    QList<QUrl> urls = QApplication::clipboard()->mimeData()->urls();
    bool isCut = false;
    bool eq = true;
    if(urls.count() == m_cutUrls.count() ){
        for(auto &e : m_cutUrls){
            if(!urls.contains(e)){
                eq = false;
            }
        }
    }
    else{
        eq = false;
    }
    isCut = eq;
    QFileInfo fileInfo(destDir);
    if(!fileInfo.exists() ){
        return;
    }
    if(!fileInfo.isDir()){
        destDir = fileInfo.absolutePath();
    }
    QString destPath = fileInfo.absoluteFilePath();

    for(auto &url : urls){

        QString srcUrl = url.toLocalFile();
        if(srcUrl.isEmpty()) continue;
        QFileInfo srcFile(srcUrl);

        QString srcFilePath = srcFile.absoluteFilePath();
        if(!srcFile.exists()){
            qDebug() << "file not exist in copy clipboard" << srcFilePath ;
            continue;
        }
        QString destFilePath = destPath + "/" + srcFile.fileName();
        QFileInfo destFile(destFilePath);

        if(FileUtil::isParentOf(srcFilePath,destFilePath)){
            qDebug() << "src is parent of  dest";
            continue;
        }
        if(destFile.exists()) {
            qDebug()<< "file exits!" << destFile.absoluteFilePath();
            continue;
        }
        if(isCut){
            QFile::rename(srcFilePath,destFilePath);
            QApplication::clipboard()->clear();
        }
        else{
            QFile::copy(srcFilePath,destFilePath);
        }

    }
}

