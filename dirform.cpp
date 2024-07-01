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
#include <QFileDialog>
#include <QMenu>
#include "fileutil.h"
#include <QSettings>

DirForm::DirForm(QWidget *parent,BookmarkMgr * bookMgr,int index)
    : QWidget(parent)
    , ui(new Ui::DirForm),m_combModifying(false),m_bookmarkMgr(bookMgr),m_index(index)
{
    ui->setupUi(this);


    layoutFileContent = new QVBoxLayout(ui->frameContent);
    layoutFileContent->setObjectName("verticalLayout_content");
    layoutFileContent->setContentsMargins(0, 0, 0, 0);
    initToolButtons();
    initViewMenu();
    bool comboItemChanged = true;
    loadDir(QDir::homePath(),comboItemChanged);
    m_history.addItem(m_curDir);
    connect(ui->comboBoxDir, &QComboBox::currentIndexChanged, this,&DirForm::on_comboDirIndexChange);
    connect(&m_fileWatcher, &QFileSystemWatcher::directoryChanged,this, &DirForm::on_dirChange);
    connect(QApplication::clipboard(),&QClipboard::dataChanged ,this , &DirForm::on_clipDataChanged);
    updateBookmarks();



}

void DirForm::initViewMenu(){
    m_fileModel.setRootPath(m_curDir);
    m_views[ViewIndexList] = ui->listView;
    m_views[ViewIndexTable] = ui->tableView;
    m_views[ViewIndexTree] = ui->treeView;

    for(int i = 0 ; i < ViewIndexCount; ++ i)
    {
        QAbstractItemView * w = m_views[i];
        w->setModel(&m_fileModel);
        w->setSelectionMode(QAbstractItemView::ExtendedSelection);
        w->setSelectionBehavior(QAbstractItemView::SelectRows);
        connect(w->selectionModel(),&QItemSelectionModel::selectionChanged,this,&DirForm::on_selectedFileChanged);
        connect(w,SIGNAL(doubleClicked(QModelIndex)),
                this,SLOT(on_fileItemDblClicked(QModelIndex)));
    }

    ui->listView->setViewMode(QListView::IconMode);
    ui->listView->setResizeMode(QListView::Adjust);
    ui->listView->setMovement(QListView::Static);

    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->clearSpans();
    ui->tableView->setSortingEnabled(true);
    ////ui->tableView->horizontalHeader()->set
    auto font = ui->tableView->font();
    qDebug()<< "fontsize=" << font.pointSize() << " pixel size" << font.pixelSize();
    font.setPointSize(9);
    qDebug()<< "fontsize=" << font.pointSize() << " pixel size" << font.pixelSize();
    ui->tableView->setFont(font);


    QMenu *menu = new QMenu(this);
    menu->addAction(ui->actionViewLargeIcon);
    menu->addAction(ui->actionViewMiddleIcon);
    menu->addAction(ui->actionViewSmallIcon);
    menu->addAction(ui->actionViewDetailTable);

    ui->toolButtonSwitchView->setPopupMode(QToolButton::MenuButtonPopup);
    ui->toolButtonSwitchView->setMenu(menu);

    switchViewType(ViewType_DetailList);
}

#include <QCloseEvent>
void DirForm::closeEvent(QCloseEvent *event)
{

    QWidget::closeEvent(event);
}
void DirForm::initToolButtons(){
    ui->toolButtonPrev->setDefaultAction(ui->actionPrev);
    ui->toolButtonNext->setDefaultAction(ui->actionNext);
    ui->toolButtonUp->setDefaultAction(ui->actionUP);
    ui->toolButtonCut->setDefaultAction(ui->actionCutSelect);
    ui->toolButtonCopy->setDefaultAction(ui->actionCopySelect);
    ui->toolButtonDel->setDefaultAction(ui->actionMoveToTrash);
    ui->toolButtonPaste->setDefaultAction(ui->actionPasteSelect);
    ui->toolButtonBookMarkList->setDefaultAction(ui->actionBookmarkList);
    ui->toolButtonAddBookmark->setDefaultAction(ui->actionAdd_Bookmark);
    ui->toolButtonBrowse->setDefaultAction(ui->actionOpenDir);
    ui->toolButtonBookMarkList->setPopupMode(QToolButton::MenuButtonPopup);
    ui->toolButtonNew->setDefaultAction(ui->actionNew_Folder);
}

QList<QVariant> DirForm::getHeaderLens()
{
    QList<QVariant> list;
    for(int i = 0; i < ui->tableView->horizontalHeader()->count(); ++ i)
    {
        list.append(ui->tableView->columnWidth(i));
    }
    return list;
}

void DirForm::updateHeaderLens(QList<QVariant> headerLens)
{
    for(int i = 0; i < headerLens.length() && i < ui->tableView->horizontalHeader()->count(); ++ i)
    {
        ui->tableView->setColumnWidth(i,headerLens[i].toInt());
    }
}


DirForm::~DirForm()
{
    delete ui;
}


// void DirForm:: refreshView(QString dirPath)
// {
//     QListWidget *listWidget = (QListWidget*)m_filesWidget;
//     listWidget->setIconSize(QSize(48,48));
//     listWidget->clear();
//     listWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
//     listWidget->setViewMode(QListView::IconMode);
//     listWidget->setGridSize(QSize(64,64));
//     listWidget->setResizeMode(QListView::Adjust);
//     listWidget->setMovement(QListView::Static);

//     QFileIconProvider provider ;
//     QDir dir(dirPath);

//     QFileInfoList list = dir.entryInfoList(QDir::NoDotAndDotDot|QDir::Files|QDir::Dirs);
//     int count = list.size();

//     m_filesWidget->selectionModel()->clearSelection();
//     for(int i = 0; i < count ; ++ i){
//         QFileInfo info = list[i];
//         QListWidgetItem * b = new QListWidgetItem();

//         b->setText(info.fileName());
//         b->setIcon(provider.icon(info));

//         b->setData(Qt::UserRole,QVariant(info.absoluteFilePath()));
//         auto font = b->font();
//         font.setPointSize(8);
//         b->setFont(font);

//         listWidget->addItem(b);
//     }

//     updateButtonState();
// }

// QAbstractItemView * DirForm::createFileIconsView(QString dirPath,bool replaceView)
// {
//     if(replaceView || m_filesWidget == nullptr){

//         if(m_filesWidget != nullptr){
//             layoutFileContent->removeWidget(m_filesWidget);
//             delete m_filesWidget;
//         }

//         QListWidget *w = new QListWidget(ui->frameContent);
//         m_filesWidget = w;
//         layoutFileContent->addWidget(m_filesWidget);
//         ///m_filesWidget->setContextMenuPolicy(Qt::CustomContextMenu);
//         connect(w,&QListWidget::itemDoubleClicked,this,&DirForm::on_fileItemOpen);
//         connect(w->selectionModel(),&QItemSelectionModel::selectionChanged,this,&DirForm::on_selectedFileChanged);
//     }

//     refreshView(dirPath);
//     return m_filesWidget;
// }

#include <QTableView>
// QAbstractItemView * DirForm::createTableView(QString dirPath,bool replaceView)
// {
//     if(replaceView || m_filesWidget == nullptr){

//         if(m_filesWidget != nullptr){
//             layoutFileContent->removeWidget(m_filesWidget);
//             delete m_filesWidget;
//         }

//         QTableView *w = new QTableView(ui->frameContent);
//         m_filesWidget = w;
//         layoutFileContent->addWidget(m_filesWidget);
//         ///m_filesWidget->setContextMenuPolicy(Qt::CustomContextMenu);
//         ////connect(w,&QListWidget::itemDoubleClicked,this,&DirForm::on_fileItemOpen);
//         m_fileModel.setRootPath(dirPath);
//         w->setModel(&m_fileModel);


//         connect(w->selectionModel(),&QItemSelectionModel::selectionChanged,this,&DirForm::on_selectedFileChanged);
//         connect(w,SIGNAL(doubleClicked(QModelIndex)),
//                 this,SLOT(on_fileItemDblClicked(QModelIndex)));;
//     }

//     m_filesWidget->setRootIndex(m_fileModel.index(dirPath));

//     /////refreshView2(dirPath);
//     return m_filesWidget;
// }

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

bool DirForm::loadDir(QString filePath,bool changeComboItem )
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

    m_fileModel.setRootPath(m_curDir);
    auto modelIndex = m_fileModel.index(m_curDir);
    m_curItemView->setRootIndex(modelIndex);

    return true;
}

void DirForm::on_comboDirIndexChange(int index)
{
    if(!m_combModifying && index < ui->comboBoxDir->count())
    {
        QString filePath = ui->comboBoxDir->itemData(index).toString();

        loadDir(filePath);
        m_history.addItem(m_curDir);
    }
}


void DirForm::on_bookmarkSelected()
{
    QString path = ((QAction *)sender())->data().toString();
    bool  changeCombo = true;
    loadDir(path,changeCombo);
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
        bool  changeCombo = true;
        loadDir(info.absoluteFilePath(),changeCombo);
        m_history.addItem(m_curDir);
    }
}

void DirForm::on_fileItemDblClicked(QModelIndex index)
{
    if(index.column() != 0) return;
    QString text  = m_fileModel.filePath(index);

    QFileInfo info(text);
    if(!info.isDir()){
        QDesktopServices::openUrl(QUrl::fromLocalFile(info.absoluteFilePath()));
    }else{
        bool changeCombo = true;
        loadDir(info.absoluteFilePath(),changeCombo);
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
    auto indexes = m_curItemView->selectionModel()->selectedIndexes();
    QList<QUrl> urls;
    QString text;
    for(auto &index :indexes){
        if(index.column() != 0) continue;
        QString path = this->m_fileModel.filePath(index);
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
    QString destPath = getTargetPath();
    emit pasteFromClip(destPath);
}


void DirForm::on_actionMoveToTrash_triggered()
{
    auto indexes = m_curItemView->selectionModel()->selectedIndexes();
    for(auto index: indexes){
        if(index.column() != 0) return;
        QString path = m_fileModel.filePath(index);
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


    auto indexes = this->m_curItemView->selectionModel()->selectedIndexes();
    bool selected = !indexes.isEmpty();
    ui->actionCopySelect->setEnabled(selected);
    ui->actionCutSelect->setEnabled(selected);
    ui->actionMoveToTrash->setEnabled(selected);
    updatePasteAction();
}

void setListView(QListView *listView,int iconSize){
    int extraSize = 12;
    listView->setIconSize(QSize(iconSize ,iconSize ));
    listView->setGridSize(QSize(iconSize + extraSize,iconSize + extraSize));
}
void DirForm::switchViewType(ViewType viewTable)
{
    ViewIndex index;

    switch(viewTable){
        case ViewType_SmallIcon:
            index = ViewIndexList;
            m_iconSize = 16;
            setListView(ui->listView,m_iconSize);
            ui->listView->setViewMode(QListView::ListMode);
            break;

        case ViewType_MiddleIcon:
            index = ViewIndexList;
            m_iconSize = 32;
            setListView(ui->listView,m_iconSize);
            ui->listView->setViewMode(QListView::IconMode);
            break;
        case ViewType_LargIcon:
            index = ViewIndexList;
            m_iconSize = 64;
            setListView(ui->listView,m_iconSize);
            ui->listView->setViewMode(QListView::IconMode);
            break;
        case ViewType_DetailList:

        default:
            index = ViewIndexTable;

    };

    QAbstractItemView *newView = m_views[index];
    if(newView != m_curItemView ){
        m_curItemView = newView;
        ui->stackedWidget->setCurrentIndex(index);
    }


    auto modelIndex = m_fileModel.index(m_curDir);
    m_curItemView->setRootIndex(modelIndex);
}


void DirForm::on_toolButtonNew_triggered(QAction *arg1)
{
    Q_UNUSED(arg1);
}




void DirForm::on_selectedFileChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);
    updateButtonState();

}

QString DirForm::getTargetPath(){
    QString destPath = this->m_curDir;
    auto indexes = m_curItemView->selectionModel()->selectedIndexes();
    for(auto &index:indexes){
        if(index.column() != 0) continue;
        destPath = m_fileModel.filePath(index);
        QFileInfo info(destPath);
        if(!info.isDir()){
            destPath = info.absoluteFilePath();
        }
        break;
    }
    return destPath;
}

void DirForm:: updatePasteAction(){
    const QMimeData * mime = QApplication::clipboard()->mimeData();

    QString pastePath = getTargetPath();
    int validCount =0;
    if(mime->hasUrls()){
        for(QUrl & url :mime->urls()){
            if(!url.isValid()) continue;
            QString  srcPath = url.toLocalFile();
            if(!srcPath.isEmpty() && FileUtil::isParentOf(srcPath,pastePath)){
                ui->actionPasteSelect->setEnabled(false);
                return;
            }
            else{
                validCount ++;
            }
        }

    }
    ui->actionPasteSelect->setEnabled(validCount > 0);
}
void DirForm::on_clipDataChanged()
{
    updatePasteAction();
}


void DirForm::on_actionOpenDir_triggered()
{
    QString filePath = QFileDialog::getExistingDirectory(this,"打开目录","选择目录打开");
    if(!filePath.isEmpty()){
        bool comboChanged = true;
        loadDir(filePath,comboChanged);
        m_history.addItem(filePath);
    }

}

#include <QFileDialog>

void DirForm::on_actionAdd_Bookmark_triggered()
{

    m_bookmarkMgr->addBookmark(this->m_curDir);
    ///updateBookmarks();
}






void DirForm::on_actionViewLargeIcon_triggered()
{
    switchViewType(ViewType_LargIcon);
}


void DirForm::on_actionViewMiddleIcon_triggered()
{
    switchViewType(ViewType_MiddleIcon);
}


void DirForm::on_actionViewSmallIcon_triggered()
{
    switchViewType(ViewType_SmallIcon);
}


void DirForm::on_actionViewDetailTable_triggered()
{
    switchViewType(ViewType_DetailList);
}



#include <QInputDialog>
#include <QMessageBox>
void DirForm::on_actionNew_Folder_triggered()
{
    QString fileName = QInputDialog::getText(this,"新建文件夹","请输入文件夹名");
    QString path = getTargetPath();
    QString newName = path + "/" + fileName;
    if(QFileInfo(newName).exists()){
        QMessageBox::information(this,"错误","文件已存在");
        return;
    }
    if(!QDir(path).mkdir(newName)){
        QMessageBox::information(this,"错误","重建文件夹失败");
            return;
    }
    else{
        QModelIndex index = m_fileModel.index(newName);
        m_curItemView->scrollTo(index);

    }

}

