#include "fileclipboard.h"
#include <QClipboard>
#include <QFileInfo>
#include <QMimeData>
#include <QApplication>
#include <QMap>
#include "fileutil.h"
#include "duplicateddialog.h"

FileClipboard::FileClipboard(QWidget *parent)
    : QObject{parent}
{


}


void FileClipboard:: doCopyUrls(const QList<QUrl> &urls,QString text,bool isCut){
    QClipboard *clip = QApplication::clipboard();

    QMimeData *mime = new QMimeData();
    mime->setUrls(urls);
    mime->setText(text);
    clip->setMimeData(mime);
    if(isCut) {
        m_cutUrls = urls;
        m_copyUrls.clear();
    }
    else {
        m_copyUrls = urls;
        m_cutUrls.clear();
    }
}

void FileClipboard::on_copyUrls(const QList<QUrl> urls,QString text)
{
    doCopyUrls(urls,text,false);
}

void FileClipboard::on_cutUrls(const QList<QUrl> urls,QString text)
{
    doCopyUrls(urls,text,true);
}

void FileClipboard::releaseThread(){
    if(m_dlg != nullptr){
        delete m_dlg;
    }

    if(m_fileThread != nullptr)
    {
        m_fileThread->doStop();
        delete m_fileThread;
    }


}

void FileClipboard::copyInProces(const QMap<QString,QString>& copyMap){

    releaseThread();
    m_fileThread = new FileThread(parent());

    m_dlg = new CopyProcessDialog((QWidget*)parent(),m_fileThread);
    m_fileThread->startPasteFiles(copyMap);
}


void FileClipboard::on_paste(QString destDir)
{
    QList<QUrl> urls = m_copyUrls;
    if(urls.isEmpty()) urls =   QApplication::clipboard()->mimeData()->urls();
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
    QMap<QString,QString> copyFiles ;
    CopyOptions curOption;
    QList<CopyOptions> pastePations;
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
        if(destFile.exists() && ! curOption.applyAll ) {
            DuplicatedDialog dlg((QWidget*)parent(),&curOption);
            int ret = dlg.exec();
            if(ret == QDialog::Rejected){
                break;
            }
        }

        if(destFile.exists())
        {
            if(curOption.dupOption == CopyOptions::Skip){
                continue;
            }
            else if(curOption.dupOption == CopyOptions::Replace)
            {
                QFile::moveToTrash(destFilePath);
            }
            else if(curOption.dupOption == CopyOptions::Rename){
                destFilePath = FileUtil::getNewFile(destPath,srcFile.fileName());
            }
        }

        if(isCut){
            QFile::rename(srcFilePath,destFilePath);
            QApplication::clipboard()->clear();
        }
        else{
            copyFiles[srcFilePath] = destFilePath;

        }

    }

    copyInProces(copyFiles);
}

