#ifndef TFILESYSTEMMODEL_H
#define TFILESYSTEMMODEL_H

#include <QFileSystemModel>
#include <QImageReader>
#include <QFileIconProvider>
class TFileSystemModel : public QFileSystemModel
{
    Q_OBJECT
public:
    explicit TFileSystemModel(QObject *parent = nullptr);


    // QAbstractItemModel interface
public:
    QVariant data(const QModelIndex &index, int role) const;
    void setPreviewable(bool bPreView, int iconSize);


private :

    QImageReader imageReader;
    QFileIconProvider m_iconProvider;
    bool m_previewImage;
    int  m_iconSize;

    // QAbstractItemModel interface
public:
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
};

#endif // TFILESYSTEMMODEL_H
