#ifndef FILEUTIL_H
#define FILEUTIL_H
class QString;
#include <QDateTime>
#include <QFileInfo>
#include <QDir>
enum class EncodingFormat:int
{
    ANSI = 0,//GBK
    UTF16LE,
    UTF16BE,
    UTF8,
    UTF8BOM,
};

class AppendInterface{
public:
    virtual void append(QString line) = 0;
    virtual void clear() = 0;
};
typedef std::pair<QString,QString>  NameUrl;

class FileUtil
{
private:
        FileUtil() = delete;
public:
    static void readParaFile(QString filePath,AppendInterface *ap);
    static EncodingFormat FileCharacterEncoding(const QString &fileName);
    static QString  sizeFormat(qint64 size);
    static const  QString TIME_FORMAT ;
    static inline QString timeStr(const QDateTime & datetime){
        return datetime.toString(TIME_FORMAT);
    }
    static QString fileType(QString filePath);
    static bool isImage(QString filePath);
    static void  defaultOpen(QString filepaths);
    static QList<NameUrl> generatePathUrls(QString filePath);
    static QString generateFileLink(QString filePath);
    static bool isParentOf(QString leftPath,QString rightPath);
    static quint64 countFileSize(QString path);
    static quint64 countDirSize(const QDir &dir,int &process);
    static bool copySymbolicLink(const QString &sourceLink, const QString &targetLink);
    constexpr static QDir::Filters SubFileDirFilter = QDir::NoDotAndDotDot| QDir::Files | QDir::Dirs;
    inline static bool isLocalDir(const QFileInfo & info ){
        return info.isDir() && !info.isSymbolicLink() && !info.isSymLink();
    }

};



#endif // FILEUTIL_H
