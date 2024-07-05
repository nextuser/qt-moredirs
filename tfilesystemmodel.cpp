#include "tfilesystemmodel.h"
#include "fileutil.h"
#include <QFileIconProvider>
TFileSystemModel::TFileSystemModel(QObject *parent)
    : QFileSystemModel{parent}
{}

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

    return QFileSystemModel::data(index,role);
}

void TFileSystemModel::setPreviewable(bool bPreView, int iconSize)
{
    m_previewImage = bPreView;
    m_iconSize = iconSize;
}
