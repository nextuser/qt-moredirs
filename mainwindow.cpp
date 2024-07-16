#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tsubwindow.h"
#include "dirform.h"
#include <QClipboard>
#include <QMimeData>
#include <QActionGroup>
#include <QTranslator>
#include "taboutdialog.h"
QTranslator translator;
const static QString KEY_LOCAL_QM_FILE = "locale/qmfile";
void MainWindow::initLocale(const QSettings *settings){
    QString qmFile = settings->value(KEY_LOCAL_QM_FILE).toString();
    if(qmFile.isEmpty()){
        const QLocale locale = QLocale::system();

        if(locale.language() == QLocale::Chinese){
            qmFile = QM_FILE_CN;
        }
        else{
            qmFile = QM_FILE_EN;
        }
    }
    bool succ = translator.load(qmFile);
    if(succ) {
        QCoreApplication::instance()->installTranslator(&translator);
        m_qmFile = qmFile;
    }

}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),m_clip(this),statusLabel(this)
{
    m_settings = new QSettings(QSettings::Format::IniFormat,QSettings::Scope::UserScope,"ljlhome","moredirs",this);

    initLocale(m_settings);
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
    QActionGroup *group = new QActionGroup(this);
    group->addAction(ui->actionTile2Window);
    group->addAction(ui->actionTileWindow);
    group->addAction(ui->actionCascade);
    group->addAction(ui->actionSwitchTab);

    QActionGroup *langGroup = new QActionGroup(this);
    langGroup->addAction(ui->actionTranslateEng);
    langGroup->addAction(ui->actionTranslateSimpleCn);
    langGroup->addAction(ui->actionTranslateTraditionalCn);

    ui->mdiArea->tileSubWindows();
    connect(ui->mdiArea,&QMdiArea::subWindowActivated,this,&MainWindow::on_subWindowActivated);
    loadSettings();
    this->statusBar()->addWidget(&statusLabel);
    updateQmActionState();
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

    m_settings->setValue(KEY_LOCAL_QM_FILE,QVariant(m_qmFile));

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


DirForm * MainWindow::createForm(QWidget *parent,int index){
    DirForm *doc = new DirForm(parent,&this->m_bookmarkMgr,index);   //指定父窗口，必须在父窗口为Widget窗口提供一个显示区域;
    connect(doc,&DirForm::copyUrlsToClip,&m_clip,&FileClipboard::on_copyUrls);
    connect(doc,&DirForm::cutUrlsToClip,&m_clip,&FileClipboard::on_cutUrls);
    connect(doc,&DirForm::pasteFromClip,&m_clip,&FileClipboard::on_paste);
    connect(doc,&DirForm::pasteFiles,&m_clip,&FileClipboard::copyInProces);
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
    ((QAction*)sender())->setChecked(true);
    ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
    showSubWin(4);
    setSubWindowFrameLess(false);
    ui->mdiArea->cascadeSubWindows();
    ///ui->mdiArea->update(ui->mdiArea->geometry());

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
        subwinList[i]->update(subwinList[i]->geometry());/// to fix the bug border not shown propertly
    }
    for(int i = count ; i < subwinList.count(); ++ i){
        subwinList[i]->setVisible(false);
    }
}

void MainWindow::on_statusLinkActivate(const QString &link)
{
    this->formList[this->m_statusFormIndex]->loadDir(link,true);
}

void MainWindow::translateUi()
{
    for(auto & form :this->formList){
        form->translateUi();
    }
    ui->retranslateUi(this);
}
#include <QTranslator>
extern QTranslator translator;
void MainWindow::switchUi(QString qmFile)
{
    bool succ = translator.load(qmFile);
    if(succ) {
        QCoreApplication::instance()->installTranslator(&translator);
        m_qmFile = qmFile;
    }
    updateQmActionState();
    translateUi();
}

void MainWindow::updateQmActionState(){
    if(m_qmFile == QM_FILE_CN){
        ui->actionTranslateSimpleCn->setChecked(true);
    }
    else if(m_qmFile == QM_FILE_EN){
        ui->actionTranslateEng->setChecked(true);
    }
}

void MainWindow::on_actionTileWindow_triggered()
{
    ((QAction*)sender())->setChecked(true);
    ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
    showSubWin(4);
    setSubWindowFrameLess(true);
    ui->mdiArea->tileSubWindows();

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


void MainWindow::on_actionTile2Window_triggered()
{
    ((QAction*)sender())->setChecked(true);
    ui->mdiArea->setViewMode(QMdiArea::SubWindowView);

    showSubWin(2);

    setSubWindowFrameLess(true);
    ui->mdiArea->tileSubWindows();
}





void MainWindow::on_actionSwitchTab_triggered()
{
    ((QAction*)sender())->setChecked(true);
    ui->mdiArea->setViewMode(QMdiArea::TabbedView); //Tab多页显示模式
    showSubWin(4);
    ui->mdiArea->setTabsClosable(true);
    ui->mdiArea->setTabsMovable(true);
}



void MainWindow::on_actionTranslateEng_triggered()
{
    switchUi(QM_FILE_EN);
}


void MainWindow::on_actionTranslateSimpleCn_triggered()
{
    switchUi(QM_FILE_CN);
}


void MainWindow::on_actionAbout_triggered()
{
    TAboutDialog dlg;
    dlg.exec();
}

