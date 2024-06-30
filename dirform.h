#ifndef DIRFORM_H
#define DIRFORM_H

#include <QWidget>
#include <QListWidgetItem>
#include "thistory.h"
#include "bookmarkMgr.h"
#include <QVBoxLayout>
#include <QUrl>
#include <QFileSystemWatcher>
namespace Ui {
class DirForm;
}

class DirForm : public QWidget
{
    Q_OBJECT

public:
    explicit DirForm(QWidget *parent ,BookmarkMgr * bookmarkMgr);
    ~DirForm();
    bool loadDir(QString filePath,bool replaceView = false,bool changeCombItem=false);

Q_SIGNALS:
    void copyUrlsToClip(QList<QUrl> urls,QString text);
    void cutUrlsToClip(QList<QUrl> urls,QString text);
    void pasteFromClip(QString destDir);
protected slots:

    void on_comboDirIndexChange(int index);
    void on_fileItemOpen(QListWidgetItem * item);
    void on_bookmarkSelected();
private slots:


    QAbstractItemView *createFileIconsView(QString dirPath,bool replaceView);
    // QWidget *createSmallsIconsView(QString dirPath);

    void on_dirChange(const QString &path);
    void on_fileChange(const QString &path);


    void on_actionCopySelect_triggered();

    void on_actionCutSelect_triggered();

    void on_actionPasteSelect_triggered();

    void on_actionMoveToTrash_triggered();

    void on_actionPrev_triggered();

    void on_actionNext_triggered();

    void on_actionUP_triggered();

    void on_toolButtonNew_triggered(QAction *arg1);

    void on_addBookmark_triggered(QAction *arg1);
    void on_selectedFileChanged(const QItemSelection &selected, const QItemSelection &deselected) ;
    void on_clipDataChanged();
private:
    void updateBookmarks();
    void addFileComboItems(QString dirPath);
    bool isFileComboContains(QString filePath);
    void copyToClipboard(bool isCut = false);
    void refreshView(QString dirPath);
    QString getPastePath();
    void updatePasteAction();

    QFileSystemWatcher m_fileWatcher;

    Ui::DirForm *ui;
    QString m_curDir ;
    THistory m_history;
    bool m_combModifying;
    void updateButtonState();
    QVBoxLayout *layoutFileContent;
    QAbstractItemView *m_filesWidget;
    BookmarkMgr * m_bookmarkMgr;
    QList<QUrl> m_cutUrls;



};

#endif // DIRFORM_H
