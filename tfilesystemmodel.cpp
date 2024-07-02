#include "tfilesystemmodel.h"
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

        QList<QByteArray> formats = QImageReader::supportedImageFormats();
        QByteArray format = fileInfo.suffix().toLower().toUtf8();
        if(formats.contains(format)){
            auto image  = QImage(fileInfo.absoluteFilePath());
            image = image.scaled(QSize(m_iconSize,m_iconSize));
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
