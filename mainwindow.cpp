#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tsubwindow.h"
#include "dirform.h"
#include <QClipboard>
#include <QMimeData>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),m_clip(this),statusLabel(this)
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
    connect(ui->mdiArea,&QMdiArea::subWindowActivated,this,&MainWindow::on_subWindowActivated);
    loadSettings();
    this->statusBar()->addWidget(&statusLabel);
    connect(&statusLabel,&QLabel::linkActivated, this, &MainWindow::on_statusLinkActivate);
}

const QString KEY_COLUMN_HEADERS = "ColumnHeaders_%1";
const QString KEY_FORM_FILE_PATH = "FilePath_%1";
void MainWindow::loadSettings(){
    ///QSettings settings(QSettings::Format::IniFormat,QSettings::Scope::UserScope,"ljlhome","moredirs",this);
    for(auto &form : formList){
        QList<QVariant> lens = m_settings->value(QString(KEY_COLUMN_HEADERS).arg(form->index())).toList();
        form->updateHeaderLens(lens);
        QString filePath = m_settings->value(QString(KEY_FORM_FILE_PATH).arg(form->index())).toString();
        bool changeCombo = true;
        if(!filePath.isEmpty())form->loadDir(filePath,changeCombo);
    }

}

void MainWindow::saveSettings(){
    ///QSettings settings(QSettings::Format::IniFormat,QSettings::Scope::UserScope,"ljlhome","moredirs",this);
    QList<QVariant> list;

    for(DirForm * form : formList){
        QString filePathKey = form->curDir();
        m_settings->setValue(QString(KEY_COLUMN_HEADERS).arg(form->index()),form->getHeaderLens());
        m_settings->setValue(QString(KEY_FORM_FILE_PATH).arg(form->index()),form->curDir());
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
    connect(doc,&DirForm::statusChanged,this,&MainWindow::on_statusChanged);
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
    subwinList.append(subWin);

    return formDoc;
}


void MainWindow::on_actionCascade_triggered()
{
    ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
    showSubWin(4);
    setSubWindowFrameLess(false);
    ui->mdiArea->cascadeSubWindows();

}

void MainWindow::setSubWindowFrameLess(bool frameless){
    for(int i = 0; i < 4; ++ i){
        subwinList[i]->setWindowFlag(Qt::FramelessWindowHint,frameless);
    }
}

void MainWindow::showSubWin(int count)
{

    for(int i = 0 ; i < count && i < subwinList.count(); ++ i ){
        subwinList[i]->setVisible(true);
    }
    for(int i = count ; i < subwinList.count(); ++ i){
        subwinList[i]->setVisible(false);
    }
}

void MainWindow::on_statusLinkActivate(const QString &link)
{
    this->formList[this->m_statusFormIndex]->loadDir(link,true);
}

void MainWindow::on_actionTileWindow_triggered()
{
    ui->mdiArea->setViewMode(QMdiArea::SubWindowView);

    showSubWin(4);

    setSubWindowFrameLess(true);
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






void MainWindow::on_action_wLeftRight_triggered()
{
    ui->mdiArea->setViewMode(QMdiArea::SubWindowView);

     showSubWin(2);

    setSubWindowFrameLess(true);
    ui->mdiArea->tileSubWindows();
}


void MainWindow::on_action_wUpDown_triggered()
{
    ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
    ui->mdiArea->tileSubWindows();
    showSubWin(2);

    setSubWindowFrameLess(true);
    ui->mdiArea->tileSubWindows();
}

#include "fileutil.h"
void MainWindow::on_statusChanged(QString filePath,int index)
{
    m_statusFormIndex = index;
    QString text = FileUtil::generateFileLink(filePath);
    statusLabel.setText(text);
}

void MainWindow::on_subWindowActivated(QMdiSubWindow * w)
{
    if(w == nullptr) return;
    QWidget * widget = w->widget();
    if(widget == nullptr) return;
    DirForm * form = (DirForm*) widget;
    this->on_statusChanged(form->curDir(),form->index());
}

