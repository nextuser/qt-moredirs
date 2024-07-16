#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUrl>
#include "bookmarkMgr.h"
#include "fileclipboard.h"
#include <QAbstractItemView>
#include <QSettings>
#include <QLabel>
#include "tsubwindow.h"
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
    MainWindow(QWidget *parent);
    ~MainWindow();
    QMenu *createTitleMenu(QWidget *target);


private slots:

    DirForm* addSubWin(int index);
    DirForm * createForm(QWidget* form,int index);
    void on_actionCascade_triggered();

    void on_actionTileWindow_triggered();

    void on_statusChanged(QString filePath,int index);
    void on_subWindowActivated(QMdiSubWindow *);

    void on_actionTile2Window_triggered();

    void on_actionSwitchTab_triggered();


    void on_actionTranslateEng_triggered();

    void on_actionTranslateSimpleCn_triggered();

    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
    BookmarkMgr m_bookmarkMgr;
    void doCopyUrls(const QList<QUrl> &urls,QString text,bool isCut);
    FileClipboard m_clip;
    QList<DirForm*> formList;
    QList<TSubWindow*> subwinList;
    QSettings *m_settings;
    int m_curIndex = 0;
    int m_statusFormIndex = 0;
    QLabel statusLabel;
    QString m_qmFile ;
    DirForm *getDirForm(int formIndex);
    void loadSettings();
    void saveSettings();
    void setSubWindowFrameLess(bool frameless);
    void showSubWin(int count);
    void on_statusLinkActivate(const QString& link);
    void translateUi();
    void switchUi(QString qmFile);
    void updateQmActionState();
    void initLocale(const QSettings *settings);
    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);

};

const static QString QM_FILE_EN = "moredirs_en.qm";
const static QString QM_FILE_CN = "moredirs_cn.qm";
#endif // MAINWINDOW_H
