#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tsubwindow.h"
#include "dirform.h"
#include <QClipboard>
#include <QMimeData>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->mdiArea);
    ui->mdiArea->setViewMode(QMdiArea::ViewMode::SubWindowView);
    int subCount = 4;
    // DirForm *form ;
    m_settings = new QSettings(QSettings::Format::IniFormat,QSettings::Scope::UserScope,"ljlhome","moredirs",this);
    m_bookmarkMgr.loadSettings(m_settings);
    for(int i = 0; i < subCount ; ++ i){
        addSubWin(i);
    }
    ui->mdiArea->tileSubWindows();
    loadSettings();
}

const QString KEY_COLUMN_HEADERS = "ColumnHeaders_%1";
void MainWindow::loadSettings(){
    ///QSettings settings(QSettings::Format::IniFormat,QSettings::Scope::UserScope,"ljlhome","moredirs",this);
    for(auto &form : formList){
        QList<QVariant> lens = m_settings->value(QString(KEY_COLUMN_HEADERS).arg(form->index())).toList();
        form->updateHeaderLens(lens);
    }

}

void MainWindow::saveSettings(){
    ///QSettings settings(QSettings::Format::IniFormat,QSettings::Scope::UserScope,"ljlhome","moredirs",this);
    QList<QVariant> list;
    for(DirForm * form : formList){

        m_settings->setValue(QString(KEY_COLUMN_HEADERS).arg(form->index()),form->getHeaderLens());
    }


}

MainWindow::~MainWindow()
{
    delete ui;
}

QMenu *MainWindow::createTitleMenu(QWidget *target)
{
    QMenu *menu = new QMenu(target);
    menu->addAction(ui->actionCloseTab);
    menu->addAction(ui->actionCloseOtherTab);
    menu->addAction(ui->actionCloseAllTab);
    return menu;
}



void MainWindow::on_actionSwitch_View_triggered(bool isTabbed)
{
    if (isTabbed) {//Tab多页显示模式
        ui->mdiArea->setViewMode(QMdiArea::TabbedView); //Tab多页显示模式
        ui->mdiArea->setTabsClosable(true);
        ui->mdiArea->setTabsMovable(true);
    }
    else{ //子窗口模式
        ui->mdiArea->setViewMode(QMdiArea::SubWindowView); //子窗口模式
    }
}

DirForm * MainWindow::createForm(QWidget *parent,int index){
    DirForm *doc = new DirForm(parent,&this->m_bookmarkMgr,index);   //指定父窗口，必须在父窗口为Widget窗口提供一个显示区域;
    connect(doc,&DirForm::copyUrlsToClip,&m_clip,&FileClipboard::on_copyUrls);
    connect(doc,&DirForm::cutUrlsToClip,&m_clip,&FileClipboard::on_cutUrls);
    connect(doc,&DirForm::pasteFromClip,&m_clip,&FileClipboard::on_paste);
    connect(&m_bookmarkMgr,&BookmarkMgr::bookmarkChanged,doc,&DirForm::updateBookmarks);
    return doc;
}

DirForm* MainWindow::addSubWin(int index)
{

    TSubWindow *subWin = new TSubWindow(ui->mdiArea,this);
    subWin->setWindowFlag(Qt::FramelessWindowHint);
    DirForm    *formDoc = createForm(subWin,index);
    this->formList.append(formDoc);

    subWin->setWidget(formDoc);
    ui->mdiArea->addSubWindow(subWin);

    return formDoc;
}


void MainWindow::on_actionCascade_triggered()
{
    ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
    ui->mdiArea->cascadeSubWindows();
}


void MainWindow::on_actionTileWindow_triggered()
{
    ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
    ui->mdiArea->tileSubWindows();
}


void MainWindow::on_actionSwitch_View_triggered()
{

}

DirForm *MainWindow::getDirForm(int formIndex)
{
    for(auto & form :formList){
        if(formIndex == form->index()){
            return form;
        }
    }
    return nullptr;
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    m_bookmarkMgr.saveSettings(m_settings);
    saveSettings();
    QMainWindow::closeEvent(event);
}





