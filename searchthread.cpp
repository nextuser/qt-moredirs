#include "searchthread.h"
#include "stringutil.h"
#include <QFileInfo>
#include <QDir>
#include <regex>
SearchThread::SearchThread(QObject *parent):QThread(parent) {

}

SearchThread::~SearchThread()
{
}

void SearchThread::stop()
{
    this->m_stop = true;

}

void SearchThread::findFile(QString location, QString filter)
{
    m_location = location;
    m_filter = filter.toLower();

    start();
}

void SearchThread::incFound(int &processCount,QStringList& results, QString newFile,bool finished){
    ++ processCount;
    if(!newFile.isEmpty()){
        results.append(newFile);
    }
    qint64 costMs = m_elapsedTimer.elapsed();
    if(costMs >= 100){

        emit file_found(results,finished);
        results.clear();
        m_elapsedTimer.restart();
    }

}

#include "fileutil.h"
void SearchThread::findInDir(const QDir &dir, const QList<std::regex>& regs, int &processCount ,QStringList& results ){
    if(m_stop) return;

    for(auto &fileInfo: dir.entryInfoList(QDir::NoDotAndDotDot|QDir::Files|QDir::Dirs)){
        if(StringUtil::match(regs,fileInfo.fileName())){
            incFound(processCount,results,fileInfo.absoluteFilePath());
        }

        if(FileUtil::isLocalDir(fileInfo)){
            findInDir(QDir(fileInfo.absoluteFilePath()),regs,processCount,results);
        }

    }
}

void SearchThread::run()
{
    QList<std::regex> regList;
    QList<QString> filters = m_filter.split(";");
    m_elapsedTimer.start();
    for(QString filter: filters){
        regList.append(StringUtil::wildchardRex(filter.toStdString()));
    }
    int processCount = 0;
    QStringList results;
    findInDir(QDir(m_location),regList,processCount, results);
    emit file_found(results,true);
}
