#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tsubwindow.h"
#include "dirform.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->mdiArea);
    ui->mdiArea->setViewMode(QMdiArea::ViewMode::SubWindowView);
    int subCount = 4;
    // DirForm *form ;


    for(int i = 0; i < subCount ; ++ i){
        addSubWin();
    }


    ui->mdiArea->tileSubWindows();
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

DirForm * MainWindow::createForm(QWidget *parent){
    DirForm *doc = new DirForm(parent,&this->m_bookmarkMgr);   //指定父窗口，必须在父窗口为Widget窗口提供一个显示区域;
    // connect(doc->codeEditor,&CodeEditor::selectionChanged,this,&MainWindow::on_selectionChanged) ;
    // connect(doc->codeEditor,&CodeEditor::copyAvailable,this,&MainWindow::on_copyAvailable) ;
    // connect(doc->codeEditor,&CodeEditor::undoAvailable,this,&MainWindow::on_undoAvailable) ;
    // connect(doc->codeEditor,&CodeEditor::redoAvailable,this,&MainWindow::on_redoAvailable) ;
    return doc;
}

DirForm* MainWindow::addSubWin()
{

    TSubWindow *subWin = new TSubWindow(ui->mdiArea,this);
    DirForm    *formDoc = createForm(subWin);
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

