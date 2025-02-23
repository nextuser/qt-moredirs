#include "fileutil.h"
#include <QFile>
#include <QTextCodec>
#include <QTextStream>
#include <QDesktopServices>
#include <QList>
#include <QUrl>
#include <QDir>
#include <QFileInfo>
#include <QByteArray>
#include <QImageReader>
const QString FileUtil::TIME_FORMAT = "yyyy-MM-dd hh:mm:ss";

bool FileUtil::isParentOf(QString leftPath,QString rightPath){
    QFileInfo left(leftPath);
    QString leftStr = left.absoluteFilePath() + QDir::separator();
    QFileInfo right(rightPath);
    QString rightStr = right.absoluteFilePath() + QDir::separator();
    return rightStr.contains(leftStr) ;
}

qint64 FileUtil::countDirSize(const QDir &dir,int &process){
    qint64 ret = 0;
    for(auto& info: dir.entryInfoList(SubFileDirFilter)){
        ++ process;
        if(FileUtil::isLocalDir(info)){
            ret += countDirSize(QDir(info.absoluteFilePath()),process);
        }
        else{
            ret += info.size();
        }
    }
    return ret;
}

qint64 FileUtil::countFileSize(QString path)
{
    qint64 fsize = 0;
    QFileInfo info(path);
    int process = 0;
    if(!info.exists()) return 0;
    if(info.isDir() && !info.isSymbolicLink() && !info.isSymLink()){
        QString filePath = info.absoluteFilePath();
        fsize += countDirSize(QDir(filePath),process);
    }
    else{
        fsize += info.size();
    }
    return fsize;
};



bool FileUtil::copySymbolicLink(const QString &sourceLink, const QString &targetLink) {
    QFile sourceFile(sourceLink);
    QFile targetFile(targetLink);

    // 获取源符号链接指向的路径
    return targetFile.link(targetFile.symLinkTarget());

}

EncodingFormat FileUtil::FileCharacterEncoding(const QString &fileName)
{
    //假定默认编码utf8
    EncodingFormat code = EncodingFormat::UTF8;

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly))
    {
        //读取3字节用于判断
        QByteArray buffer = file.read(3);
        quint8 sz1st = buffer.at(0);
        quint8 sz2nd = buffer.at(1);
        quint8 sz3rd = buffer.at(2);
        if (sz1st == 0xFF && sz2nd == 0xFE)
        {
            code = EncodingFormat::UTF16LE;
        }
        else if (sz1st == 0xFE && sz2nd == 0xFF)
        {
            code = EncodingFormat::UTF16BE;
        }
        else if (sz1st == 0xEF && sz2nd == 0xBB && sz3rd == 0xBF)
        {
            code = EncodingFormat::UTF8BOM;
        }
        else
        {
            //尝试用utf8转换,如果无效字符数大于0,则表示是ansi编码
            QTextCodec::ConverterState cs;
            QTextCodec* tc = QTextCodec::codecForName("utf-8");
            tc->toUnicode(buffer.constData(), (int)buffer.size(), &cs);
            code = (cs.invalidChars > 0) ? EncodingFormat::ANSI : EncodingFormat::UTF8;
        }

        file.close();
    }

    return code;
}

void FileUtil::readParaFile(QString filePath,AppendInterface *ap)
{

    if (ap == nullptr) {
        ///qDebug() << "ap is null!";
        return;
    }

    //读取ansi编码格式文件
    ap->clear();

    //    filePath = "E:/work/ImageManageSys/utf8/0000_051623_162252_05_004_00001_00008_00.txt";
    EncodingFormat code = FileCharacterEncoding(filePath);
    ////qDebug() << "code=" << (int)code;

    //读取ANSI编码格式文件
    if(code == EncodingFormat::ANSI)
    {
        QString txtFile = filePath.left(filePath.size() -3);
        txtFile += "txt";
        //
        QFile file(filePath);
        if(file.open(QIODevice::ReadOnly)) {
            //        qDebug() << file.errorString();
            QTextCodec::setCodecForLocale(QTextCodec::codecForName("gb2312"));//中文转码声明
            QString temStr;
            while(!file.atEnd())
            {
                QByteArray arr = file.readAll();
                arr.replace(0x0B,0x0D);
                temStr = QString::fromLocal8Bit(arr, arr.length());//Window下的QByteArray转QString
                ap->append(temStr);
            }
        }
        file.close();
    }

    //读取UTF-8编码格式文件
    if(code == EncodingFormat::UTF8)
    {

        QFile file(filePath);
        if(!file.open(QIODevice::ReadOnly)) {
            ///qDebug() << file.errorString();
            return;
        }

        QTextStream in(&file);
        ///in.setCodec("UTF-8");  // 设置编码为UTF-8
        in.setAutoDetectUnicode(true);

        while(!in.atEnd()) {
            QString line = in.readLine();
            ap->append(line); // 添加到QTextEdit控件中
        }

        file.close();
    }
}

void  FileUtil::defaultOpen(QString filepaths)
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(filepaths));
}

QString FileUtil::sizeFormat(qint64 size)
{
    const qint64 GB = 1 << 30;
    const qint64 MB = 1 << 20;
    const qint64 KB = 1 << 10;
    QString ret = "";
    if(size > GB){
        ret += QString::asprintf("%3.3f GB",size / (float)GB);
    }
    else if(size > MB){
        ret += QString::asprintf("%3.3f MB",size / (float)MB);
    }
    else if(size > KB){
        ret += QString::asprintf("%3.3f KB",size / (float)KB);
    }
    else{
        ret += tr("%1 字节").arg(size);
    }
    return ret;
}

QString FileUtil::fileType(QString filePath)
{
    QFileInfo info(filePath);
    QString suffix =  info.suffix();

    if(info.isDir()){
        return tr("目录");
    }
    else if(isImage(filePath)){
        return tr("图片");
    }
    else{
        return suffix;
    }
}

bool FileUtil::isImage(QString filePath)
{
    QList<QByteArray> formats = QImageReader::supportedImageFormats();
    QByteArray format = QFileInfo(filePath).suffix().toLower().toUtf8();
    return formats.contains(format);
}

QList<NameUrl> FileUtil::generatePathUrls(QString filePath){

    QList<NameUrl> retList;

    QFileInfo fileInfo(filePath);
    QString curPath;
    QString parentPath;
    do{
        curPath = fileInfo.absoluteFilePath();
        parentPath = fileInfo.absolutePath();
        QString name = fileInfo.fileName();
        if(name.isEmpty()){
            name = curPath;
        }
        retList.push_front(std::make_pair(name,curPath));
        fileInfo = QFileInfo(parentPath);
    }while(curPath != parentPath);
    return retList;
}
#include <QDir>
#include <stringutil.h>
QString FileUtil::generateFileLink(QString filePath){
    static QString linkTemplate = R"(<a href="%1">%2</a>)";
    //QString sep = QDir::separator();
    QString sep = " > ";
    QList<NameUrl>  nameUrls = generatePathUrls(filePath);
    QString ret = "&nbsp;&nbsp;";
    for(NameUrl &nameUrl : nameUrls){
        QString name = nameUrl.first;

        QFileInfo info(nameUrl.second);
        if(info.isDir() ){
            name = nameUrl.first + sep;
            name = name.toHtmlEscaped();
            std::string str = name.toStdString();
            StringUtil::replaceAll(str,"/","&frasl;");
            name = QString::fromStdString(str);
            ret += linkTemplate.arg(nameUrl.second).arg(name);
        }
        else{
            ret += nameUrl.first;
        }
    }
    return ret;
}

QString FileUtil::getNewFile(QString path, QString fileName)
{
    QString newPath = path + QDir::separator() + fileName;
    QFile* currFile = new QFile(newPath);
    int i = 0;
    while(currFile->exists()){
        delete currFile;
        int index = (int)fileName.lastIndexOf(".");
        QString baseName = fileName;
        QString extName = "";
        if(index >= 0 ){

            baseName = fileName.left(index);
            extName = fileName.right(fileName.size() - index - 1);
        }
        newPath = path + QDir::separator() + baseName + QString::number(i++) ;
        if(!extName.isEmpty()) newPath += "." +  extName;
        currFile = new QFile(newPath);
    }

    delete currFile;
    return newPath;
}
