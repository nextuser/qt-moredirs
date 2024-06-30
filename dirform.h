#ifndef DIRFORM_H
#define DIRFORM_H

#include <QWidget>
#include <QListWidgetItem>
#include "thistory.h"
#include "bookmarkMgr.h"
#include <QVBoxLayout>
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
protected slots:

    void on_comboDirIndexChange(int index);
    void on_fileItemOpen(QListWidgetItem * item);
private slots:
    void on_toolButtonPrev_clicked();

    void on_toolButtonNext_clicked();

    void on_toolButtonUp_clicked();

    void on_bookmark_selected();



    QWidget *createFileIconsView(QString dirPath,bool replaceView);
    // QWidget *createSmallsIconsView(QString dirPath);

    void on_addBookmark_clicked();

private:
    void updateBookmarks();
    Ui::DirForm *ui;
    QString m_curDir ;
    THistory m_history;
    bool m_combModifying;
    void updateButtonState();
    QVBoxLayout *layoutFileContent;
    QWidget *m_filesWidget;
    BookmarkMgr * m_bookmarkMgr;

};

#endif // DIRFORM_H
