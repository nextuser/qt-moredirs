#ifndef FILETHREAD_H
#define FILETHREAD_H

#include <QObject>
#include <QThread>
#include <QList>
#include <QUrl>
#include <QDir>
#include <QFileInfo>

class QDir;
class FileThread : public QThread
{
    Q_OBJECT

    enum TaskType{
        Task_Copy,
        Task_CountSize
    };
public:
    explicit FileThread(QObject *parent = nullptr);

    void setCountPercent(int percent);
    void doStop();

    void startCountFile(QStringList filePaths);

    void startPasteFiles(QStringList srcPaths,QString dstPath);

Q_SIGNALS:
    void countSizeProcessInd(int count,int dirCount,quint64 fileSize, bool bFinished );
    void copyProcessInd(int count ,int dirCount,quint64 fileSize,QString curPath, bool stopped = false);

    // QThread interface
protected:
    void run();
    static const int COUNT_PROCESS_IN_1MS = 10000;
    const static int COPY_STEP_COUNT =500;
    const static quint64 COPY_STEP_SIZE = 1 << 23; //(8MB)
private :
    TaskType m_taskType;
    QStringList m_srcPaths;
    QString m_targetParentPath;
    int     m_countPercent;
    bool    m_stop;
    int     m_sizeStep = 0;
    void countDirSize(const QDir &dir,int &process,int &dirCount ,quint64& fsize);
    quint64 countFileSize(QStringList paths);


    void copyDir(const QDir&  srcDir,const QDir& dstDir,int & fileCount,int &dirCount,quint64 &processSize);
    //void copyFile(QFileInfo  srcInfo,const QFileInfo& dstDirInfo,int &fileCount,quint64 &processSize);
    void incCopy(const QString &curPath,int &count,int dirCount,quint64 &processSize, int incSize = -1);

    void copyFile(QStringList  srcpaths,const QFileInfo& dstDirInfo);

};

#endif // FILETHREAD_H
