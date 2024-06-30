#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUrl>
#include "bookmarkMgr.h"
#include "fileclipboard.h"
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
    DirForm* addSubWin();
    DirForm * createForm(QWidget* form);
    void on_actionCascade_triggered();

    void on_actionTileWindow_triggered();

    void on_actionSwitch_View_triggered();




private:
    Ui::MainWindow *ui;
    BookmarkMgr m_bookmarkMgr;
    void doCopyUrls(const QList<QUrl> &urls,QString text,bool isCut);
    FileClipboard m_clip;
    QSettings *m_settings;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);

};
#endif // MAINWINDOW_H
