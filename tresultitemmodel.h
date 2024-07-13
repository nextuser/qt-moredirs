#ifndef TRESULTITEMMODEL_H
#define TRESULTITEMMODEL_H

#include <QAbstractItemModel>
#include <QList>
#include <QString>
#include <QFileIconProvider>

class TResultItemModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit TResultItemModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    // QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    // QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    // Fetch data dynamically:
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;



    void  addFiles(QList<QString> files);
    void  clear();
    bool  isRootIndex(const QModelIndex & index);
    QString filePath(const QModelIndex & index);
    QList<QString> m_fileList;
    QFileIconProvider m_iconProvider;
private:

    // QAbstractItemModel interface
public:
    // QVariant data(const QModelIndex &index, int role) ;

    Q_INVOKABLE virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;


    // QAbstractItemModel interface
public:
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
};

#endif // TRESULTITEMMODEL_H
