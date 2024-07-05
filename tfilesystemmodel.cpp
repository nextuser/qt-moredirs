#include "tfilesystemmodel.h"
#include "fileutil.h"
#include <QFileIconProvider>
TFileSystemModel::TFileSystemModel(QObject *parent)
    : QFileSystemModel{parent}
{}
enum ColumnIndex{
    ColName,
    ColModifiedTime,

    ColType,
    ColSize,
    ColCount
};

QVariant TFileSystemModel::data(const QModelIndex &index, int role) const
{
    QFileInfo fileInfo(filePath(index));
    if(role == Qt::ToolTipRole){
        return QVariant(fileInfo.fileName());
    }

    if(role == Qt::DecorationRole && index.column() == 0){

        if(!m_previewImage)
            return m_iconProvider.icon(fileInfo);

        if(FileUtil::isImage(fileInfo.absoluteFilePath())){
            auto image  = QImage(fileInfo.absoluteFilePath());
            if(image.size().width()> image.size().height())
            {
                image = image.scaledToWidth(m_iconSize);
            }else{
                image = image.scaledToHeight(m_iconSize);
            }
            return image;
        }
        else{
            return m_iconProvider.icon(fileInfo);
        }
    }

    if(role == Qt::DisplayRole){
        switch(index.column()){
        case ColName:
            return fileInfo.fileName();
            break;
        case ColSize:
            return FileUtil::sizeFormat(fileInfo.size());
            break;
        case ColType:
            return QFileSystemModel::data(index,role);
            break;
        case ColModifiedTime:
            return FileUtil::timeStr(fileInfo.lastModified());
            break;
        }
    }

    return QFileSystemModel::data(index,role);
}

void TFileSystemModel::setPreviewable(bool bPreView, int iconSize)
{
    m_previewImage = bPreView;
    m_iconSize = iconSize;
}

QVariant TFileSystemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal){
        switch(section){
            case ColName:
            return "文件名";
            break;
            case ColModifiedTime:
                return "修改日期";
                break;
            case ColSize:
                return "大小";
                break;
            case ColType:
                return "文件类型";
                break;
        }
    }
    return QFileSystemModel::headerData(section,orientation,role);
}
