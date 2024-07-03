#include "filethread.h"
#include <QFile>
#include <QDesktopServices>
#include <QList>
#include <QUrl>
#include <QDir>
#include <QFileInfo>
#include "fileutil.h"
FileThread::FileThread(QObject *parent)
    : QThread{parent}
{}

const static QDir::Filters SubFileDirFilter = QDir::NoDotAndDotDot| QDir::Files | QDir::Dirs;

inline static bool isLocalDir(const QFileInfo & info ){
    return info.isDir() && !info.isSymbolicLink() && !info.isSymLink();
}

void FileThread::countDirSize(const QDir &dir,int &process,int &dirCount ,quint64 &fsize){
    if(m_stop) return;
    for(auto& info: dir.entryInfoList(SubFileDirFilter)){
        ++ process;
        if(isLocalDir(info)){
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

quint64 FileThread::countFileSize(QString path)
{
    quint64 fsize = 0;
    int process = 0;
    int dirCount = 0;
    QFileInfo info(path);

    if(!info.exists()) return 0;
    if(isLocalDir(info)){
        QString filePath = info.absoluteFilePath();
        countDirSize(QDir(filePath),process,dirCount,fsize);
        ++ dirCount;
    }
    else{
       fsize += info.size();
    }
    ++ process;

    emit countSizeProcessInd(process,dirCount,fsize,true);

    return fsize;
}

void FileThread::startCountFile(QString filePath)
{
    this->m_taskType = Task_CountSize;
    m_srcPath = filePath;
    start();
}

void FileThread::startCopyFile(QString srcPath,QString dstPath)
{
    this->m_taskType = Task_Copy;
    m_srcPath = srcPath;
    m_targetParentPath = dstPath;
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
        if(isLocalDir(entry)){
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

void FileThread::copyFile(QFileInfo  srcInfo,const QFileInfo& dstDirInfo){
    QString targetPath = dstDirInfo.absoluteFilePath() + "/" + srcInfo.fileName();
    QString srcPath = srcInfo.absoluteFilePath();
    int count = 0;
    int dirCount = 0;
    quint64 fsize;
    if(isLocalDir(srcInfo)){
        QDir(dstDirInfo.absoluteFilePath()).mkdir(srcInfo.fileName());
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
    emit  copyProcessInd(count,dirCount,fsize,srcPath);
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
      countFileSize(m_srcPath);
    }


    if(m_taskType == Task_Copy){
        copyFile(QFileInfo(m_srcPath),QFileInfo(m_targetParentPath));
    }

};

