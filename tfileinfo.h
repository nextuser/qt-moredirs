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
    qint64 size;
    QString sizeStr;
    quint64 lastModifiedMs;
    QString lastModifiedStr;
    QString location;


};

#endif // TFILEINFO_H
