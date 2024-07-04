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

#include <QMessageBox>
quint64 FileThread::countFileSize(QStringList paths)
{
    quint64 fsize = 0;
    int process = 0;
    int dirCount = 0;
    for(auto &path :paths)
    {
        QFileInfo info(path);

        if(!info.exists()) {
            QString text = QString("文件[%1]不存在 ").arg(info.absoluteFilePath());
            QMessageBox::warning((QWidget*)parent(),QString("错误"),text);
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

void FileThread::startCountFile(QStringList filePath)
{
    this->m_taskType = Task_CountSize;
    m_srcPaths = filePath;
    start();
}

void FileThread::startPasteFiles(QStringList srcPath,QString dstPath)
{
    this->m_taskType = Task_Copy;
    m_srcPaths = srcPath;
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

void FileThread::copyFile(QStringList  srcPaths,const QFileInfo& dstDirInfo){
    int count = 0;
    int dirCount = 0;
    quint64 fsize = 0;

    for(auto path :srcPaths)
    {
        QFileInfo srcInfo(path);
        QString targetPath = dstDirInfo.absoluteFilePath() + "/" + srcInfo.fileName();


        if(FileUtil::isLocalDir(srcInfo)){
            QDir(dstDirInfo.absoluteFilePath()).mkdir(srcInfo.fileName());
            ++ dirCount;
            incCopy(path,count,dirCount,fsize);

            copyDir(QDir(path),QDir(targetPath),count,dirCount,fsize);

        }
        else if(srcInfo.isSymbolicLink())
        {
            FileUtil::copySymbolicLink(path,targetPath);
            incCopy(path,count,dirCount,fsize);
        }

        else{
            QFile::copy(path,targetPath);

            incCopy(srcInfo.absoluteFilePath(),count,dirCount,fsize,srcInfo.size());
        }
    }
    QString lastPath = srcPaths.count() > 0 ? srcPaths.last() : "";
    emit  copyProcessInd(count,dirCount,fsize,lastPath);
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
      countFileSize(m_srcPaths);
    }


    if(m_taskType == Task_Copy){
        copyFile(m_srcPaths,QFileInfo(m_targetParentPath));
    }

};

