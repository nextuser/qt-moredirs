#include "tresultitemmodel.h"
#include <QFileInfo>
#include "fileutil.h"
#include "tfileinfo.h"
#include <QDir>
TResultItemModel::TResultItemModel(QObject *parent)
    : QAbstractItemModel(parent)
{}

int TResultItemModel::rowCount(const QModelIndex &parent) const
{
        return parent.isValid() ?  0 : (int)m_fileList.count() ;
}

int TResultItemModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ColCount ;
}

bool TResultItemModel::hasChildren(const QModelIndex &parent) const
{
    return !parent.isValid();
}

void TResultItemModel::addFiles(QList<QString> files,QDir dir)
{
    int count = (int)m_fileList.count();
    beginInsertRows(QModelIndex(),count ,(int)( count +  files.count() - 1));
    for(auto & file : files){
        m_fileList.append(TFileInfo(file,dir));
    }
    endInsertRows();
}

void TResultItemModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, (int)(m_fileList.size() - 1));
    m_fileList.clear();
    endRemoveRows();

}

bool TResultItemModel::isRootIndex(const QModelIndex &index)
{
    return !index.isValid();
}

QString TResultItemModel::filePath(const QModelIndex &index)
{  
    int row = index.row();

    if(row >= 0 && row < m_fileList.count()){
        return  m_fileList[row].filePath;
    }
    return "";
}
QString getLocation(QString path, int parentLen){
    QString ret = path;
    if(parentLen > 0 && path.length() >= parentLen){
        ret = path.sliced(parentLen);
        if(ret.startsWith(QDir::separator())){
            ret = ret.sliced(1);
        }
    }
    return ret;
}
QVariant TResultItemModel::data(const QModelIndex &index, int role) const
{

    QVariant ret;
    if(!index.isValid()) return ret;
    if(index.row() >= m_fileList.size() || index.column() >= ColCount )
        return ret;

    const TFileInfo &fileInfo = m_fileList[index.row()];

    if(role == Qt::ToolTipRole){
        return QVariant(fileInfo.filePath);
    }

    if(role == Qt::DecorationRole && index.column() == 0){
        return m_iconProvider.icon(QFileInfo(fileInfo.filePath));
    }

    if(role == Qt::DisplayRole){
        switch(index.column()){
        case ColName:
            return fileInfo.fileName;
            break;
        case ColSize:
            return fileInfo.sizeStr;
            break;
        case ColPath:
            return fileInfo.location;
        case ColModifiedTime:
            return fileInfo.lastModifiedStr;
            break;
        }
    }

    return ret;
}

QModelIndex TResultItemModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return createIndex(row,column, &this->m_fileList);
}

QModelIndex TResultItemModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}


qint64 compString(const QString& left,const QString &right,Qt::SortOrder order){
    return order == Qt::AscendingOrder?  left.compare(right) : right.compare(left);
}

qint64 compInt(const qint64 left,const qint64 right,Qt::SortOrder order){
    if(order == Qt::AscendingOrder){
        return left - right;
    }
    else{
        return right -left;
    }
}

qint64 comp(QList<TFileInfo>& fileList,int l, int r,int column, Qt::SortOrder order){
    TFileInfo & left = (fileList[l]);
    TFileInfo & right= (fileList[r]);
    if(column == TResultItemModel::ColModifiedTime){
        return  compInt((left).lastModifiedMs
                       ,(right).lastModifiedMs,order);
    }
    else if(column == TResultItemModel::ColName){
        return compString((left).fileName,(right).fileName,order);
    }
    else if(column == TResultItemModel::ColSize){
        return compInt((left).size,(right).size,order);
    }
    else if(column == TResultItemModel::ColPath){
        return compString((left).location, (right).location,order);
    }
    return 0;
}



void mergeSort(QList<TFileInfo> &fileList,int start, int end, int column, Qt::SortOrder order )
{
    if(end <= start  ) return;
    int half = (end - start)/2 + start;
    int start1 = start, end1 = half;
    int start2 = half + 1,end2 = end;

    if(end1 > start1) mergeSort(fileList,start1,end1,column,order);
    if(end2 > start2) mergeSort(fileList,start2,end2,column,order);

    QList<TFileInfo> recList;
    int i1 = start1,i2 = start2 ;
    while( i1 <= end1 && i2 <= end2){
        if(comp(fileList,i1,i2,column,order) < 0){
            recList.append(fileList[i1 ++]);
        }
        else{
            recList.append(fileList[i2 ++]);
        }
    }
    while(i1 <= end1){
        recList.append(fileList[i1 ++]);
    }
    while(i2 <= end2 ){
        recList.append(fileList[i2 ++]);
    }
    for(int i = start; i <= end; ++ i){
        fileList[i] = recList[i - start];
    }


}
void TResultItemModel::sort(int column, Qt::SortOrder order)
{
    mergeSort(m_fileList,0, (int)(m_fileList.count() - 1), column,order);
    emit layoutChanged();
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
        case ColPath:
            return tr("目录");
            break;
        }
    }
    return QAbstractItemModel::headerData(section,orientation,role);
}
