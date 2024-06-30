#ifndef BOOKMARKMGR_H
#define BOOKMARKMGR_H
#include <QString>
#include <QVariantList>
class BookmarkMgr
{
public:
    BookmarkMgr(int maxCount = 20);
    void addBookmark(QString filePath) ;

    QVariantList bookmarkList() ;
    void loadSettings();

    void saveSettings();

private :
    QVariantList m_bookmarkList;
    const int m_maxCount;
};

#endif // BOOKMARKMGR_H
