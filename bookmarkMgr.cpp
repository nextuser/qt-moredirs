#include "bookmarkMgr.h"
#include <QSettings>
BookmarkMgr::BookmarkMgr(int maxCount)
    :QObject{nullptr},m_maxCount(maxCount) {

}


void BookmarkMgr::addBookmark(QString filePath)
{
    m_bookmarkList.append(filePath);
    if(m_bookmarkList.size() > m_maxCount){
        m_bookmarkList.pop_front();
    }
    emit bookmarkChanged();
}

QVariantList BookmarkMgr::bookmarkList()
{
    return m_bookmarkList;
}


const QString KEY_BOOK_MARKS = "bookmark_list";
void BookmarkMgr::loadSettings(QSettings * settings)
{

    auto value = settings->value(KEY_BOOK_MARKS);
    m_bookmarkList = value.toList();
}

void BookmarkMgr::saveSettings(QSettings *settings)
{

    settings->setValue(KEY_BOOK_MARKS,m_bookmarkList);
}
