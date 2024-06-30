#ifndef BOOKMARKMGR_H
#define BOOKMARKMGR_H
#include <QObject>
#include <QString>
#include <QSettings>
#include <QVariantList>
class BookmarkMgr :public QObject
{
    Q_OBJECT
public:
    explicit BookmarkMgr(QObject* parent = nullptr,int maxCount = 20);

    void addBookmark(QString filePath) ;

    QVariantList bookmarkList() ;
    void loadSettings(QSettings *settings);
    void saveSettings(QSettings * settings);

signals:
    void bookmarkChanged();
private :
    QVariantList m_bookmarkList;
    const int m_maxCount;


};

#endif // BOOKMARKMGR_H
