#include "fileclipboard.h"
#include <QClipboard>
#include <QFileInfo>
#include <QMimeData>
#include <QApplication>
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

bool isParentOf(QString leftPath,QString rightPath){
    QFileInfo left(leftPath);
    QString leftStr = left.absoluteFilePath() + "/";
    QFileInfo right(rightPath);
    QString rightStr = right.absoluteFilePath() + "/";
    return rightStr.contains(leftStr) && (rightStr != leftStr);
}


void FileClipboard::on_paste(QString destDir)
{
    QList<QUrl> urls = QApplication::clipboard()->mimeData()->urls();
    bool isCut = false;
    if(urls == m_cutUrls){
        isCut = true;
    }
    QFileInfo fileInfo(destDir);
    if(!fileInfo.exists() ||  !fileInfo.isDir()){
        return;
    }
    QString destPath = fileInfo.absoluteFilePath();

    for(auto &url : urls){
        QFileInfo srcFile(url.toLocalFile());
        QString srcFilePath = srcFile.absoluteFilePath();
        if(!srcFile.exists()){
            qDebug() << "file not exist in copy clipboard" << srcFilePath ;
            continue;
        }
        QString destFilePath = destPath + "/" + srcFile.fileName();
        QFileInfo destFile(destFilePath);

        if(isParentOf(srcFilePath,destFilePath)){
            qDebug() << "src is parent of  dest";
            continue;
        }
        if(destFile.exists()) {
            qDebug()<< "file exits!" << destFile.absoluteFilePath();
            continue;
        }
        if(isCut){
            QFile::rename(srcFilePath,destFilePath);
        }
        else{
            QFile::copy(srcFilePath,destFilePath);
        }
    }
}

