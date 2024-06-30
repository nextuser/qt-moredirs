#include "dirform.h"
#include "ui_dirform.h"
#include <QFileInfo>
#include <QFileIconProvider>
#include <QDir>
#include <QListWidgetItem>
#include <QDesktopServices>
#include <QUrl>
#include <QMimeData>
#include <QClipboard>
#include "fileutil.h"
DirForm::DirForm(QWidget *parent,BookmarkMgr * bookMgr)
    : QWidget(parent)
    , ui(new Ui::DirForm),m_combModifying(false),m_filesWidget(nullptr),m_bookmarkMgr(bookMgr)
{
    ui->setupUi(this);
    ui->toolButtonNext->setDefaultAction(ui->actionNext);

    layoutFileContent = new QVBoxLayout(ui->frameContent);
    layoutFileContent->setObjectName("verticalLayout_content");
    layoutFileContent->setContentsMargins(0, 0, 0, 0);
    bool replaceView = true, comboItemChanged = true;
    loadDir(QDir::homePath(),replaceView,comboItemChanged);
    connect(ui->comboBoxDir, &QComboBox::currentIndexChanged, this,&DirForm::on_comboDirIndexChange);
    connect(&m_fileWatcher, &QFileSystemWatcher::directoryChanged,this, &DirForm::on_dirChange);
    updateBookmarks();
}

DirForm::~DirForm()
{
    delete ui;
}

int getItemWidth(){
    return 48;
}

int getItemHeight(){
    return 64;
}

void DirForm:: refreshView(QString dirPath)
{
    QListWidget *listWidget = (QListWidget*)m_filesWidget;
    listWidget->clear();
    QDir dir(dirPath);
    QFileIconProvider provider ;

    QFileInfoList list = dir.entryInfoList(QDir::NoDotAndDotDot|QDir::Files|QDir::Dirs);
    int count = list.size();


    for(int i = 0; i < count ; ++ i){
        QFileInfo info = list[i];
        QListWidgetItem * b = new QListWidgetItem();

        b->setText(info.fileName());
        b->setIcon(provider.icon(info));

        b->setData(Qt::UserRole,QVariant(info.absoluteFilePath()));
        auto font = b->font();
        font.setPointSize(8);
        b->setFont(font);

        listWidget->addItem(b);
    }
}

QAbstractItemView * DirForm::createFileIconsView(QString dirPath,bool replaceView)
{
    if(replaceView || m_filesWidget == nullptr){

        if(m_filesWidget != nullptr){
            layoutFileContent->removeWidget(m_filesWidget);
            delete m_filesWidget;

        }

        m_filesWidget = new QListWidget(ui->frameContent);
        connect((QListWidget*)m_filesWidget,&QListWidget::itemDoubleClicked,this,&DirForm::on_fileItemOpen);

        layoutFileContent->addWidget(m_filesWidget);
    }

    refreshView(dirPath);




    return m_filesWidget;
}

// QWidget *DirForm::createSmallsIconsView(QString dirPath)
// {
//     QWidget * widget = ui->scrollArea->widget();
//     if(widget !=nullptr) delete widget;

//     QDir dir(dirPath);

//     QFileIconProvider provider ;
//     QFrame *frame = new QFrame();

//     QGridLayout *gridLayout = new QGridLayout(frame);
//     gridLayout->setObjectName("gridLayout");
//     gridLayout->setContentsMargins(0, 0, 0, 0);

//     ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

//     auto g2 = ui->scrollArea->viewport()->geometry();
//     int width = g2.width();
//     int itemWidth = getItemWidth();
//     int itemHeight = getItemHeight();
//     QFileInfoList list = dir.entryInfoList(QDir::NoDotAndDotDot|QDir::Files|QDir::Dirs);
//     int count = list.size();
//     int colCount = 1;


//     for(int i = 0; i < count && colCount > 0; ++ i){
//         QFileInfo info = list[i];
//         QToolButton * b = new QToolButton(frame);
//         int row = i / colCount;
//         int col = i % colCount;
//         b->setText(info.fileName());
//         b->setIcon(provider.icon(info));
//         b->setIconSize(QSize(itemWidth,itemWidth));
//         b->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
//         b->
//         ////b->setMaximumSize(itemWidth,itemHeight);
//         auto font = b->font();
//         font.setPointSize(8);
//         b->setFont(font);
//         gridLayout->addWidget(b,row,col,1,1);
//         if(info.isDir()){
//             connect(b,&QAbstractButton::clicked,this,&DirForm::on_dirItem_click);

//         }
//         else{
//             connect(b,&QAbstractButton::clicked,this,&DirForm::on_fileItemOpen);
//         }

//     }
//     return frame;
// }

void DirForm::addFileComboItems(QString dirPath)
{
    int itemCount = ui->comboBoxDir->count();
    for(int i = itemCount -1 ; i >= 0; --i){
        ui->comboBoxDir->removeItem(i);
    }
    ////disconnect(ui->comboBoxDir, &QComboBox::currentIndexChanged, this,&DirForm::on_comboDirIndexChange);
    m_combModifying = true;
    ui->comboBoxDir->clear();
    QList<NameUrl> paths = FileUtil::generatePathUrls(dirPath);
    for(auto nu : paths){
        ui->comboBoxDir->addItem(nu.first,nu.second);
    }

    //connect(ui->comboBoxDir, &QComboBox::currentIndexChanged, this,&DirForm::on_comboDirIndexChange);
    m_combModifying = false;
}

bool DirForm::isFileComboContains(QString filePath)
{
    int count = ui->comboBoxDir->count();
    for(int i = 0 ;  i < count ;++ i )
    {
        if(ui->comboBoxDir->itemData(i).toString() == filePath)
            return true;
    }
    return false;
}

bool DirForm::loadDir(QString filePath,bool replaceView ,bool changeComboItem )
{

    QFileInfo fileInfo(filePath);
    if(!fileInfo.exists()) return false;
    QString dirPath  = fileInfo.isDir() ? fileInfo.absoluteFilePath() : fileInfo.absolutePath();
    QDir dir(dirPath);
    if(!m_curDir.isEmpty()){
        m_fileWatcher.removePath(m_curDir);
    }

    m_curDir = dirPath;
    m_fileWatcher.addPath(m_curDir);
    m_history.addItem(m_curDir);

    if(changeComboItem)
    {
        if(!isFileComboContains(dirPath)){
            addFileComboItems(dirPath);
        }
        ui->comboBoxDir->setCurrentText(dir.dirName());

    }



    ui->toolButtonBookMarkList->setPopupMode(QToolButton::MenuButtonPopup);


    createFileIconsView(dirPath,replaceView);

    updateButtonState();
    return true;
}





void DirForm::on_comboDirIndexChange(int index)
{
    if(!m_combModifying && index < ui->comboBoxDir->count())
    {
        QString filePath = ui->comboBoxDir->itemData(index).toString();
        bool replaceView = false, comboItemChanged = false;
        loadDir(filePath,replaceView,comboItemChanged);
    }
}




void DirForm::on_toolButtonPrev_clicked()
{
    QString file = m_history.prev();
    loadDir(file);
}


void DirForm::on_toolButtonNext_clicked()
{
    QString file = m_history.next();
    loadDir(file);
}

void DirForm::updateButtonState()
{
    ui->toolButtonNext->setEnabled(m_history.nextable());
    ui->toolButtonPrev->setEnabled(m_history.prevable());
}


void DirForm::on_toolButtonUp_clicked()
{
    QFileInfo info(m_curDir);
    loadDir(info.absolutePath());
}

void DirForm::on_bookmark_selected()
{
    QString path = ((QAction *)sender())->data().toString();
    bool replaceView = false, changeCombo = true;
    loadDir(path,replaceView,changeCombo);
}

void DirForm::on_fileItemOpen(QListWidgetItem *item)
{
    if(item == nullptr) return;
    QString text  = item->data(Qt::UserRole).toString();
    QFileInfo info(text);
    if(!info.isDir()){
        QDesktopServices::openUrl(QUrl::fromLocalFile(item->data(Qt::UserRole).toString()));
    }else{
        bool replaceView = false, changeCombo = true;
        loadDir(info.absoluteFilePath(),replaceView,changeCombo);
    }
}

#include <QMenu>

void DirForm:: updateBookmarks(){
    QMenu *menu = new QMenu(this);
    for(auto c : m_bookmarkMgr->bookmarkList()){

        QFileInfo file(c.toString());

        QAction *action = new QAction(file.fileName());
        action->setData(c);
        connect(action,&QAction::triggered,this,&DirForm::on_bookmark_selected);
        menu->addAction(action);
    }
    ui->toolButtonBookMarkList->setMenu(menu);
}

#include <QClipboard>
void DirForm::on_addBookmark_clicked()
{
    m_bookmarkMgr->addBookmark(this->m_curDir);
    updateBookmarks();

}

void DirForm::copyToClipboard(bool isCut)
{
    auto indexes = m_filesWidget->selectionModel()->selectedIndexes();
    QList<QUrl> urls;
    QString text;
    for(auto &index :indexes){
        QString path = m_filesWidget->model()->data(index,Qt::UserRole).toString();
        urls.append(QUrl::fromLocalFile(path));
        text += (path) + "\n";
    }

    if(isCut){
        emit cutUrlsToClip(urls,text);
    }
    else{
        emit copyUrlsToClip(urls,text);
    }
}



void DirForm::on_toolButtonCut_clicked()
{
     copyToClipboard(true);

}



void DirForm::on_toolButtonCopy_clicked()
{
    copyToClipboard();

}


void DirForm::on_toolButtonPaste_clicked()
{
    emit pasteFromClip(this->m_curDir);

    ////refreshView(this->m_curDir);

}

void DirForm::on_dirChange(const QString &path)
{
    refreshView(this->m_curDir);
    qDebug()<< "dirChange" << path;
}

void DirForm::on_fileChange(const QString &path)
{
    qDebug() << "fileChange" << path;
}



