#include "filethread.h"
#include <QFile>
#include <QDesktopServices>
#include <QList>
#include <QUrl>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include "copyoptions.h"
#include "fileutil.h"
FileThread::FileThread(QObject *parent)
    : QThread{parent}
{}


constexpr static QDir::Filters SubFileDirFilter = QDir::NoDotAndDotDot| QDir::Files | QDir::Dirs;


void FileThread::countDirSize(const QDir &dir,int &process,int &dirCount ,quint64 &fsize){
    if(m_stop) return;
    for(auto& info: dir.entryInfoList(SubFileDirFilter)){
        ++ process;
        if(FileUtil::isLocalDir(info)){
            ++ dirCount;
            countDirSize(QDir(info.absoluteFilePath()),process,dirCount,fsize);
        }
        else{
            fsize += info.size();
        }
        if(process % COUNT_PROCESS_IN_1MS == 0){
            emit countSizeProcessInd(process,dirCount,fsize,false);
        }

    }
    return ;
}


quint64 FileThread::countFileSize(QStringList paths)
{
    quint64 fsize = 0;
    int process = 0;
    int dirCount = 0;
    for(auto &path :paths)
    {
        QFileInfo info(path);

        if(!info.exists()) {
            QString text = tr("文件[%1]不存在 ").arg(info.absoluteFilePath());
            QMessageBox::warning((QWidget*)parent(),tr("错误"),text);
            emit countSizeProcessInd(process,dirCount,fsize,false);
            continue;
        }
        if(FileUtil::isLocalDir(info)){
            QString filePath = info.absoluteFilePath();
            countDirSize(QDir(filePath),process,dirCount,fsize);
            ++ dirCount;
        }
        else{
           fsize += info.size();
        }
        ++ process;
    }
    emit countSizeProcessInd(process,dirCount,fsize,true);

    return fsize;
}

void FileThread::startCountFile(QStringList filePaths)
{
    this->m_taskType = Task_CountSize;
    m_countList = filePaths;

    start();
}

void FileThread::startPasteFiles(const QMap<QString,QString>& copyMap)
{
    this->m_taskType = Task_Copy;
    m_copyMap = copyMap;
    m_countList = copyMap.keys();

    start();
}

void FileThread::setCountPercent(int percent)
{
    m_countPercent = percent;
}

void FileThread::doStop()
{
    this->m_stop = true;
}


void FileThread::copyDir(const QDir&  srcDir,const QDir& dstDir,int & fileCount,int &dirCount,quint64 &processSize)
{
    if(m_stop){
        emit copyProcessInd(fileCount,dirCount,processSize,srcDir.absolutePath(),true);
        return;
    }

    for(auto & entry : srcDir.entryInfoList(SubFileDirFilter)){
        QString fileName = entry.fileName();
        QString targetPath = dstDir.absolutePath() + "/" + fileName;
        QString srcPath = entry.absoluteFilePath();
        if(FileUtil::isLocalDir(entry)){
            dstDir.mkdir(fileName);
            ++ dirCount;
            incCopy(srcPath,fileCount,dirCount,processSize);
            copyDir(QDir(srcPath),QDir(targetPath), fileCount,dirCount,processSize);
        }
        else if(entry.isSymbolicLink())
        {
            FileUtil::copySymbolicLink(srcPath,targetPath);
            incCopy(srcPath,fileCount,dirCount,processSize);
        }
        else
        {
            QFile::copy(srcPath,targetPath);

            incCopy(srcPath,fileCount,dirCount,processSize,entry.size());
        }
    }
}

void FileThread::copyFile(const QMap<QString,QString> &copyMap){
    int count = 0;
    int dirCount = 0;
    quint64 fsize = 0;
    QString path ;
    for(auto &srcPath : copyMap.keys())
    {
        path = srcPath;
        QFileInfo srcInfo(srcPath);
        QString   targetPath = copyMap[srcPath];
        QFileInfo dstInfo(targetPath);



        if(FileUtil::isLocalDir(srcInfo)){
            QDir(dstInfo.absolutePath()).mkdir(dstInfo.fileName());
            ++ dirCount;
            incCopy(srcPath,count,dirCount,fsize);

            copyDir(QDir(srcPath),QDir(targetPath),count,dirCount,fsize);

        }
        else if(srcInfo.isSymbolicLink())
        {
            FileUtil::copySymbolicLink(srcPath,targetPath);
            incCopy(srcPath,count,dirCount,fsize);
        }

        else{
            QFile::copy(srcPath,targetPath);

            incCopy(srcInfo.absoluteFilePath(),count,dirCount,fsize,srcInfo.size());
        }
    }

    emit  copyProcessInd(count,dirCount,fsize,path);
}

static inline int  getSizeStep(quint64 size){
    return size >> 23;
}
void FileThread::incCopy(const QString &curPath,int &count,int dirCount,quint64 &processSize, int incSize)
{
    ++ count;
    if(incSize > 0){
        processSize += incSize;
        int newStep = getSizeStep(processSize);
        if(m_sizeStep != newStep){
            m_sizeStep = newStep;
            emit copyProcessInd(count,dirCount,processSize,curPath);
        }
    }
    if(count % COPY_STEP_SIZE == 0 ){
        emit copyProcessInd(count,dirCount,processSize,curPath);
    }
}


void FileThread::run()
{

    if(m_taskType == Task_CountSize || m_taskType == Task_Copy){
      countFileSize(m_copyMap.keys());
    }


    if(m_taskType == Task_Copy){
        copyFile(m_copyMap);
    }

};

