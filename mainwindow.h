#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUrl>
#include "bookmarkMgr.h"
#include "fileclipboard.h"
#include <QAbstractItemView>
#include <QSettings>
class TFormDoc;
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
class DirForm;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QMenu *createTitleMenu(QWidget *target);


private slots:
    void on_actionSwitch_View_triggered(bool bTab);
    DirForm* addSubWin(int index);
    DirForm * createForm(QWidget* form,int index);
    void on_actionCascade_triggered();

    void on_actionTileWindow_triggered();

    void on_actionSwitch_View_triggered();
private:
    Ui::MainWindow *ui;
    BookmarkMgr m_bookmarkMgr;
    void doCopyUrls(const QList<QUrl> &urls,QString text,bool isCut);
    FileClipboard m_clip;
    QList<DirForm*> formList;
    QSettings *m_settings;
    int m_curIndex = 0;

    void loadSettings();
    void saveSettings();

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);

};
#endif // MAINWINDOW_H
