#include "tfileinfo.h"
#include "fileutil.h"

TFileInfo::TFileInfo(QString path,QDir parentDir) :filePath(path){

    QFileInfo fileInfo(filePath);
    size = fileInfo.size();

    location = parentDir.relativeFilePath(fileInfo.absolutePath());

    fileName = fileInfo.fileName();
    lastModifiedMs = fileInfo.lastModified().toMSecsSinceEpoch();
    lastModifiedStr = FileUtil::timeStr(fileInfo.lastModified());

}

