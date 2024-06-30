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


    layoutFileContent = new QVBoxLayout(ui->frameContent);
    layoutFileContent->setObjectName("verticalLayout_content");
    layoutFileContent->setContentsMargins(0, 0, 0, 0);
    bool replaceView = true, comboItemChanged = true;

    ui->toolButtonPrev->setDefaultAction(ui->actionPrev);
    ui->toolButtonNext->setDefaultAction(ui->actionNext);
    ui->toolButtonUp->setDefaultAction(ui->actionUP);
    ui->toolButtonCut->setDefaultAction(ui->actionCutSelect);
    ui->toolButtonCopy->setDefaultAction(ui->actionCopySelect);
    ui->toolButtonDel->setDefaultAction(ui->actionMoveToTrash);
    ui->toolButtonPaste->setDefaultAction(ui->actionPasteSelect);


    loadDir(QDir::homePath(),replaceView,comboItemChanged);
    m_history.addItem(m_curDir);
    connect(ui->comboBoxDir, &QComboBox::currentIndexChanged, this,&DirForm::on_comboDirIndexChange);
    connect(&m_fileWatcher, &QFileSystemWatcher::directoryChanged,this, &DirForm::on_dirChange);

    connect(QApplication::clipboard(),&QClipboard::dataChanged ,this , &DirForm::on_clipDataChanged);


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

    m_filesWidget->selectionModel()->clearSelection();
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

    updateButtonState();
}

QAbstractItemView * DirForm::createFileIconsView(QString dirPath,bool replaceView)
{
    if(replaceView || m_filesWidget == nullptr){

        if(m_filesWidget != nullptr){
            layoutFileContent->removeWidget(m_filesWidget);
            delete m_filesWidget;

        }

        QListWidget *w = new QListWidget(ui->frameContent);
        m_filesWidget = w;
        layoutFileContent->addWidget(m_filesWidget);
        ///m_filesWidget->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(w,&QListWidget::itemDoubleClicked,this,&DirForm::on_fileItemOpen);

        connect(w->selectionModel(),&QItemSelectionModel::selectionChanged,this,&DirForm::on_selectedFileChanged);

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


    if(changeComboItem)
    {
        if(!isFileComboContains(dirPath)){
            addFileComboItems(dirPath);
        }
        ui->comboBoxDir->setCurrentText(dir.dirName());

    }
    ui->toolButtonBookMarkList->setPopupMode(QToolButton::MenuButtonPopup);
    createFileIconsView(dirPath,replaceView);

    return true;
}

void DirForm::on_comboDirIndexChange(int index)
{
    if(!m_combModifying && index < ui->comboBoxDir->count())
    {
        QString filePath = ui->comboBoxDir->itemData(index).toString();
        bool replaceView = false, comboItemChanged = false;
        loadDir(filePath,replaceView,comboItemChanged);
        m_history.addItem(m_curDir);
    }
}


void DirForm::on_bookmarkSelected()
{
    QString path = ((QAction *)sender())->data().toString();
    bool replaceView = false, changeCombo = true;
    loadDir(path,replaceView,changeCombo);
    m_history.addItem(m_curDir);
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
        m_history.addItem(m_curDir);
    }
}

#include <QMenu>

void DirForm:: updateBookmarks(){
    QMenu *menu = new QMenu(this);
    for(auto c : m_bookmarkMgr->bookmarkList()){

        QFileInfo file(c.toString());

        QAction *action = new QAction(file.fileName());
        action->setData(c);
        connect(action,&QAction::triggered,this,&DirForm::on_bookmarkSelected);
        menu->addAction(action);
    }
    ui->toolButtonBookMarkList->setMenu(menu);
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





void DirForm::on_dirChange(const QString &path)
{
    refreshView(this->m_curDir);
    qDebug()<< "dirChange" << path;
}

void DirForm::on_fileChange(const QString &path)
{
    qDebug() << "fileChange" << path;
}




void DirForm::on_actionCopySelect_triggered()
{
    copyToClipboard(false);
}


void DirForm::on_actionCutSelect_triggered()
{
    copyToClipboard(true);
}


void DirForm::on_actionPasteSelect_triggered()
{
    QString destPath = getPastePath();
    emit pasteFromClip(destPath);
}


void DirForm::on_actionMoveToTrash_triggered()
{
    auto indexes = m_filesWidget->selectionModel()->selectedIndexes();
    for(auto index: indexes){
        QString path = m_filesWidget->model()->data(index,Qt::UserRole).toString();
        QFile::moveToTrash(path);
    }
}


void DirForm::on_actionPrev_triggered()
{
    QString file = m_history.prev();
    loadDir(file);
}



void DirForm::on_actionNext_triggered()
{
    QString file = m_history.next();
    loadDir(file);
}


void DirForm::on_actionUP_triggered()
{
    QFileInfo info(m_curDir);
    loadDir(info.absolutePath());
    m_history.addItem(m_curDir);
}



void DirForm::updateButtonState()
{
    ui->actionNext->setEnabled(m_history.nextable());
    ui->actionPrev->setEnabled(m_history.prevable());


    auto indexes = this->m_filesWidget->selectionModel()->selectedIndexes();
    bool selected = !indexes.isEmpty();
    ui->actionCopySelect->setEnabled(selected);
    ui->actionCutSelect->setEnabled(selected);
    ui->actionMoveToTrash->setEnabled(selected);
    updatePasteAction();
}


void DirForm::on_toolButtonNew_triggered(QAction *arg1)
{
    Q_UNUSED(arg1);
}


void DirForm::on_addBookmark_triggered(QAction *arg1)
{
    Q_UNUSED(arg1);
    m_bookmarkMgr->addBookmark(this->m_curDir);
    updateBookmarks();
}

void DirForm::on_selectedFileChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);
    updateButtonState();

}

QString DirForm::getPastePath(){
    QString destPath = this->m_curDir;
    auto indexes = m_filesWidget->selectionModel()->selectedIndexes();
    if(indexes.count() > 0){
        destPath = m_filesWidget->model()->data(indexes.at(0),Qt::UserRole).toString();
        QFileInfo info(destPath);
        if(!info.isDir()){
            destPath = info.absoluteFilePath();
        }
    }
    return destPath;
}

void DirForm:: updatePasteAction(){
    const QMimeData * mime = QApplication::clipboard()->mimeData();

    QString pastePath = getPastePath();
    if(mime->hasUrls()){
        for(QUrl & url :mime->urls()){
            QString  srcPath = url.toLocalFile();
            if(FileUtil::isParentOf(srcPath,pastePath)){
                ui->actionPasteSelect->setEnabled(false);
                return;
            }
        }

    }
    ui->actionPasteSelect->setEnabled(mime->hasUrls());
}
void DirForm::on_clipDataChanged()
{
    updatePasteAction();
}

