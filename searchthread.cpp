#include "searchthread.h"
#include "stringutil.h"
#include <QFileInfo>
#include <QDir>
#include <regex>
SearchThread::SearchThread() {}

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



void SearchThread::findInDir(const QDir &dir, const QList<std::regex>& regs){
    if(m_stop) return;

    for(auto &fileInfo: dir.entryInfoList(QDir::NoDotAndDotDot|QDir::Files|QDir::Dirs)){
        if(StringUtil::match(regs,fileInfo.fileName())){
            emit file_found(fileInfo.absoluteFilePath());
        }

        if(fileInfo.isDir() && !fileInfo.isSymbolicLink()){
            findInDir(QDir(fileInfo.absoluteFilePath()),regs);
        }

    }
}

void SearchThread::run()
{
    QList<std::regex> regList;
    QList<QString> filters = m_filter.split(";");
    for(QString filter: filters){
        regList.append(StringUtil::wildchardRex(filter.toStdString()));
    }

    findInDir(QDir(m_location),regList);

    emit find_finished();


}
