#ifndef TFILEINFO_H
#define TFILEINFO_H
#include <QString>
#include <QDir>
struct TFileInfo
{
public:
    TFileInfo(QString filePath,QDir parentDir);
    QString fileName;
    QString filePath;
    quint64 size;
    QString sizeStr;
    quint64 lastModifiedMs;
    QString lastModifiedStr;
    QString location;


    // const TFileInfo & operator = (const TFileInfo & other){
    //     filePath = other.filePath;
    //     fileName = other.fileName;
    //     size = other.size;
    //     sizeStr = other.sizeStr;
    //     lastModifiedMs = other.lastModifiedMs;
    //     lastModifiedStr = other.lastModifiedStr;
    //     location = other.location;
    //     return *this;
    // }
};

#endif // TFILEINFO_H
