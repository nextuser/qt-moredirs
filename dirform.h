#ifndef DIRFORM_H
#define DIRFORM_H

#include <QWidget>
#include <QListWidgetItem>
#include "thistory.h"
#include "bookmarkMgr.h"
#include <QVBoxLayout>
#include <QUrl>
#include <QFileSystemWatcher>
#include <QImageReader>
#include <QMap>
#include "tfilesystemmodel.h"

namespace Ui {
class DirForm;
}

class DirForm : public QWidget
{
    Q_OBJECT

public:
    explicit DirForm(QWidget *parent ,BookmarkMgr * bookmarkMgr,int index);
    ~DirForm();
    bool loadDir(QString filePath,bool fromHist = false);
    QList<QVariant> getHeaderLens();
    inline int index(){ return m_index;}
    void updateHeaderLens(QList<QVariant> headerLens );
    inline QString curDir(){
        return m_curDir;
    }

Q_SIGNALS:
    void copyUrlsToClip(QList<QUrl> urls,QString text);
    void cutUrlsToClip(QList<QUrl> urls,QString text);
    void pasteFromClip(QString destDir);
    void pasteFiles(QMap<QString,QString> copyMap);

    void statusChanged(QString filePath,int formIndex);
public slots:
    void updateBookmarks();
    void selectPath(QString filePath);



protected slots:

    void on_comboDirIndexChange(int index);
    void on_fileItemOpen(QListWidgetItem * item);
    void on_bookmarkSelected();


private slots:


    // QAbstractItemView *createFileIconsView(QString dirPath,bool replaceView);
    // QWidget *createSmallsIconsView(QString dirPath);

    void on_dirChanged(const QString &path);
    void on_fileChanged(const QString &path);


    void on_actionCopySelect_triggered();

    void on_actionCutSelect_triggered();

    void on_actionPasteSelect_triggered();

    void on_actionMoveToTrash_triggered();

    void on_actionPrev_triggered();

    void on_actionNext_triggered();

    void on_actionUP_triggered();

    void on_toolButtonNew_triggered(QAction *arg1);


    void on_selectedFileChanged(const QItemSelection &selected, const QItemSelection &deselected) ;
    void on_clipDataChanged();
    void on_actionOpenDir_triggered();

    void on_actionAdd_Bookmark_triggered();
    void on_fileItemDblClicked(QModelIndex index);

    void on_actionViewLargeIcon_triggered();

    void on_actionViewMiddleIcon_triggered();

    void on_actionViewSmallIcon_triggered();

    void on_actionViewDetailTable_triggered();


    void on_actionNew_Folder_triggered();

    void on_actionRenameSelect_triggered();

    void on_customContextMenuRequested(const QPoint &pos);
    void on_comboBoxFilter_currentTextChanged(const QString &arg1);

    void on_actionFind_triggered();

    void on_rowsInserted(const QModelIndex &parent, int first, int last);

    void on_actionViewSuperLargeIcon_triggered();

private:
    enum ViewIndex{
        ViewIndexTable = 0,
        ViewIndexList,
        ViewIndexTree,
        ViewIndexCount,
    };

    enum ViewType{
        ViewType_DetailList,
        ViewType_SmallIcon ,
        ViewType_MiddleIcon,
        ViewType_LargIcon,
        ViewType_SuperLargeIcon,

        ViewType_Count
    };
    QAction *m_viewMenuActions[ViewType_Count];
    void setListView(QListView *listView,int iconSize);
    int addFileComboItems(QString dirPath);
    int findCombItemIndex(QString filePath);
    void copyToClipboard(bool isCut = false);
    // void refreshView(QString dirPath);
    QString getTargetPath();
    void updatePasteAction();
    void initToolButtons();
    void initViewMenuAction(QMenu *men);
    void toggleMenu(ViewType type);

    QAbstractItemView * createTableView(QString dirPath,bool replaceView);

    QFileSystemWatcher m_fileWatcher;

    Ui::DirForm *ui;
    QString m_curDir ;
    THistory m_history;
    bool m_combModifying;
    void updateButtonState();
    BookmarkMgr * m_bookmarkMgr;
    QList<QUrl> m_cutUrls;
    TFileSystemModel m_fileModel;
    QAbstractItemView *m_curItemView;
    bool m_viewModified = false;

    int m_iconSize;
    int m_index;

    void switchViewType(ViewType viewTable);

    void initViewMenu();


    QAbstractItemView * m_views[3] ;
    QList<QString> comboItems;




    // QWidget interface
protected:


    // QWidget interface
protected:
    void dropEvent(QDropEvent *event);


    // QWidget interface
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void scrollToPath(QAbstractItemView *itemView,QString newFile );

};


#endif // DIRFORM_H
