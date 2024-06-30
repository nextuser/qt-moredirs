#include "bookmarkMgr.h"
#include <QSettings>
BookmarkMgr::BookmarkMgr(int maxCount):m_maxCount(maxCount) {

}


void BookmarkMgr::addBookmark(QString filePath)
{
    m_bookmarkList.append(filePath);
    if(m_bookmarkList.size() > m_maxCount){
        m_bookmarkList.pop_front();
    }
}

QVariantList BookmarkMgr::bookmarkList()
{
    return m_bookmarkList;
}

#include <QSettings>
const QString KEY_BOOK_MARKS;
void BookmarkMgr::loadSettings()
{
    QSettings settings;
    m_bookmarkList = settings.value(KEY_BOOK_MARKS).toList();
}

void BookmarkMgr::saveSettings()
{
    QSettings settings;
    settings.setValue(KEY_BOOK_MARKS,m_bookmarkList);
}
