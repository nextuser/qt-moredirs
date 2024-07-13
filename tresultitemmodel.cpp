#include "tresultitemmodel.h"
#include <QFileInfo>
#include "fileutil.h"
enum ColumnIndex{
    ColName,
    ColModifiedTime,
    ColSize,
 //   ColType,

    ColCount
};

TResultItemModel::TResultItemModel(QObject *parent)
    : QAbstractItemModel(parent)
{}



// QModelIndex TResultItemModel::index(int row, int column, const QModelIndex &parent) const
// {
//     if(!parent.isValid()){

//     }
//     // FIXME: Implement me!
// }

// QModelIndex TResultItemModel::parent(const QModelIndex &index) const
// {
//     // FIXME: Implement me!
// }

int TResultItemModel::rowCount(const QModelIndex &parent) const
{
        return parent.isValid() ?  0 : m_fileList.count() ;
}

int TResultItemModel::columnCount(const QModelIndex &parent) const
{
    return ColCount ;
}

bool TResultItemModel::hasChildren(const QModelIndex &parent) const
{
    return !parent.isValid();
}

void TResultItemModel::addFiles(QList<QString> files)
{
    int count = m_fileList.count();
    beginInsertRows(QModelIndex(),count , count +  files.count() - 1);
    for(auto & file : files){
        m_fileList.append(file);
    }
    endInsertRows();
}

void TResultItemModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, m_fileList.size() - 1);
    m_fileList.clear();
    endRemoveRows();

}

bool TResultItemModel::isRootIndex(const QModelIndex &index)
{
    return !index.isValid();
}

QString TResultItemModel::filePath(const QModelIndex &index)
{
    QString ret;
    int row = index.row();

    if(row >= 0 && row < m_fileList.count()){
        ret = m_fileList[row];
    }
    return ret;

}

QVariant TResultItemModel::data(const QModelIndex &index, int role) const
{

    QVariant ret;
    if(!index.isValid()) return ret;
    if(index.row() >= m_fileList.size() || index.column() >= ColCount )
        return ret;

    QFileInfo fileInfo(m_fileList[index.row()]);
    if(role == Qt::ToolTipRole){
        return QVariant(fileInfo.fileName());
    }

    if(role == Qt::DecorationRole && index.column() == 0){

        return m_iconProvider.icon(fileInfo);

    }

    if(role == Qt::DisplayRole){
        switch(index.column()){
        case ColName:
            return fileInfo.fileName();
            break;
        case ColSize:
            return FileUtil::sizeFormat(fileInfo.size());
            break;
        // case ColType:
        //     return fileInfo.suffix().toUpper();
        //     break;
        case ColModifiedTime:
            return FileUtil::timeStr(fileInfo.lastModified());
            break;
        }
    }

    return ret;
}

QModelIndex TResultItemModel::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex(row,column, &this->m_fileList);
}

QModelIndex TResultItemModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}


QVariant TResultItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal){
        switch(section){
        case ColName:
            return tr("文件名");
            break;
        case ColModifiedTime:
            return tr("修改时间");
            break;
        case ColSize:
            return tr("大小");
            break;
        // case ColType:
        //     return tr("文件类型");
        //     break;
        }
    }
    return QAbstractItemModel::headerData(section,orientation,role);
}
