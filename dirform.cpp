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
#include <QMenu>
#include <QSettings>
#include <QPointF>
#include <QPoint>
#include <QAbstractItemView>
#include <QModelIndex>
#include <QDropEvent>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QItemSelectionRange>
#include "stringutil.h"
#include "finddialog.h"
#include "fileutil.h"
#include "tsubwindow.h"


DirForm::DirForm(QWidget *parent,BookmarkMgr * bookMgr,int index)
    : QWidget(parent)
    , ui(new Ui::DirForm),m_combModifying(false),m_bookmarkMgr(bookMgr),m_index(index)
{
    ui->setupUi(this);
    initToolButtons();
    initViewMenu();
    bool comboItemChanged = true;
    loadDir(QDir::homePath(),comboItemChanged);
    m_history.addItem(m_curDir);
    connect(ui->comboBoxDir, &QComboBox::currentIndexChanged, this,&DirForm::on_comboDirIndexChange);
    connect(&m_fileWatcher, &QFileSystemWatcher::directoryChanged,this, &DirForm::on_dirChanged);
    connect(&m_fileWatcher, &QFileSystemWatcher::fileChanged,this, &DirForm::on_fileChanged);
    connect(QApplication::clipboard(),&QClipboard::dataChanged ,this , &DirForm::on_clipDataChanged);
    connect(&m_fileModel,&QAbstractItemModel::rowsInserted,this,&DirForm::on_rowsInserted);
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
        w->setContextMenuPolicy(Qt::CustomContextMenu);

        connect(w->selectionModel(),&QItemSelectionModel::selectionChanged,this,&DirForm::on_selectedFileChanged);
        connect(w,SIGNAL(doubleClicked(QModelIndex)),
                this,SLOT(on_fileItemDblClicked(QModelIndex)));
        connect(w,&QAbstractItemView::customContextMenuRequested,
                this,&DirForm::on_customContextMenuRequested);
    }

    ui->listView->setViewMode(QListView::IconMode);
    ui->listView->setResizeMode(QListView::Adjust);
    ui->listView->setMovement(QListView::Static);
    ui->listView->setTextElideMode(Qt::ElideRight);

    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->clearSpans();
    ui->tableView->setSortingEnabled(true);
    auto font = ui->tableView->font();
    ///qDebug()<< "fontsize=" << font.pointSize() << " pixel size" << font.pixelSize();
    font.setPointSize(11);
    ///qDebug()<< "fontsize=" << font.pointSize() << " pixel size" << font.pixelSize();
    ui->tableView->setFont(font);


    QMenu *menu = new QMenu(this);
    menu->addAction(ui->actionViewLargeIcon);
    menu->addAction(ui->actionViewMiddleIcon);
    menu->addAction(ui->actionViewSmallIcon);
    menu->addAction(ui->actionViewDetailTable);

    ui->toolButtonSwitchView->setPopupMode(QToolButton::MenuButtonPopup);
    ui->toolButtonSwitchView->setMenu(menu);
    ui->tableView->viewport()->setAcceptDrops(false);
    ui->tableView->setDragEnabled(true);
    this->setAcceptDrops(true);

    switchViewType(ViewType_DetailList);
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
    ui->toolButtonRename->setDefaultAction(ui->actionRenameSelect);
    ui->toolButtonFind->setDefaultAction(ui->actionFind);
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

///#include <QTableView>
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
        // for(auto info : QDir(m_curDir).entryInfoList()){
        //     m_fileWatcher.removePath(info.absoluteFilePath());
        // }
    }

    m_curDir = dirPath;
    m_fileWatcher.addPath(m_curDir);
    // for(auto info : QDir(dirPath).entryInfoList() ){
    //     m_fileWatcher.addPath(info.absoluteFilePath());
    // }
    ((TSubWindow*)parent())->setWindowTitle(QFileInfo(dirPath).fileName());

    if(changeComboItem)
    {
        disconnect(ui->comboBoxDir, &QComboBox::currentIndexChanged, this,&DirForm::on_comboDirIndexChange);
        if(!isFileComboContains(dirPath)){
            addFileComboItems(dirPath);
        }
        ui->comboBoxDir->setCurrentText(dir.dirName());
        connect(ui->comboBoxDir, &QComboBox::currentIndexChanged, this,&DirForm::on_comboDirIndexChange);
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

void DirForm::selectPath(QString filePath)
{
    QModelIndex index = m_fileModel.index(filePath);
    m_curItemView->scrollTo(index);
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





void DirForm::on_dirChanged(const QString &path)
{
    Q_UNUSED(path);
   // qDebug()<< "dirChange" << path;
}

void DirForm::on_fileChanged(const QString &path){
    Q_UNUSED(path);
   /// qDebug() << "fileChanged " <<path;
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
    m_viewModified = true;
    ////auto index = m_fileModel.index(destPath)
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
    bool selectOne = this->m_curItemView->selectionModel()->selectedRows().count() == 1;
    ui->actionCopySelect->setEnabled(selected);
    ui->actionCutSelect->setEnabled(selected);
    ui->actionMoveToTrash->setEnabled(selected);
    ui->actionPasteSelect->setEnabled(selected);
    ui->actionRenameSelect->setEnabled(selectOne);
    updatePasteAction();
}

void DirForm::setListView(QListView *listView,int iconSize){
    listView->setIconSize(QSize(iconSize ,iconSize ));
    int gridSize = iconSize * 1.3;
    listView->setGridSize(QSize(gridSize,gridSize));

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
            m_iconSize = 64;
            setListView(ui->listView,m_iconSize);
            ui->listView->setViewMode(QListView::IconMode);

            break;
        case ViewType_LargIcon:
            index = ViewIndexList;
            m_iconSize = 96;
            setListView(ui->listView,m_iconSize);
            ui->listView->setViewMode(QListView::IconMode);
            break;
        case ViewType_DetailList:

        default:
            index = ViewIndexTable;
            m_iconSize = 20;

    };

    QAbstractItemView *newView = m_views[index];
    if(newView != m_curItemView ){
        m_curItemView = newView;
        m_curItemView->setIconSize(QSize(m_iconSize,m_iconSize));
        ui->stackedWidget->setCurrentIndex(index);

    }

    if(m_curItemView == ui->listView){
        m_fileModel.setPreviewable(ui->listView->viewMode() == QListView::IconMode,m_iconSize);
    }

    auto modelIndex = m_fileModel.index(m_curDir);
    m_curItemView->setRootIndex(modelIndex);
    updateButtonState();
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
            destPath = info.absolutePath();
        }
        break;
    }
    return destPath;
}

void DirForm:: updatePasteAction(){
    const QMimeData * mime = QApplication::clipboard()->mimeData();

    QString pastePath = getTargetPath();
    bool valid = false;
    if(mime->hasUrls()){
        for(QUrl & url :mime->urls()){
            if(!url.isValid()) continue;
            QString  srcPath = url.toLocalFile();
            if(!srcPath.isEmpty() && FileUtil::isParentOf(srcPath,pastePath)){
                valid = false;
            }
            else{
                valid = true;
            }
            break;
        }

    }
    ui->actionPasteSelect->setEnabled(valid);
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


void DirForm::on_actionNew_Folder_triggered()
{
    QString fileName = QInputDialog::getText(this,"新建文件夹","请输入文件夹名",QLineEdit::EchoMode::Normal,"新文件夹");
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
        ///QModelIndex index = m_fileModel.index(newName);
        ///m_curItemView->scrollTo(index);
        m_viewModified = true;
        scrollToPath(m_curItemView,newName);
    }


}


void DirForm::on_actionRenameSelect_triggered()
{
    QModelIndexList rows = m_curItemView->selectionModel()->selectedRows();
    if(rows.count() == 1){
        QString filePath = m_fileModel.filePath(rows.at(0));
        QFileInfo srcInfo(filePath);
        QString newName = QInputDialog::getText(this,"变更名称","输入新名字",QLineEdit::EchoMode::Normal,srcInfo.fileName());
        if(!newName.isEmpty() && newName != srcInfo.fileName()){
            QString destPath = srcInfo.absolutePath() + "/" + newName;
            if(QFileInfo(destPath).exists()){
                QMessageBox::information(this,"错误","文件已存在");
            }
            else{
                QFile::rename(srcInfo.absoluteFilePath(),destPath);
                scrollToPath(m_curItemView,destPath);
            }
        }
    }
}

void DirForm::on_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos);
    QMenu menu(this);
    menu.addAction(ui->actionCutSelect);
    menu.addAction(ui->actionCopySelect);

    menu.addAction(ui->actionPasteSelect);
    menu.addAction(ui->actionRenameSelect);
    menu.addAction(ui->actionMoveToTrash);

    menu.exec(QCursor::pos());
}


void DirForm::on_comboBoxFilter_currentTextChanged(const QString &text)
{
    QDir dir(m_curDir);
    QString filter = text.trimmed();
    if(filter.isEmpty()) return;
    m_curItemView->selectionModel()->clear();
    bool notScrolled = true;
    for(auto file : dir.entryInfoList()){
        if(StringUtil::matchWildcard(filter,file.fileName())){
            auto index = m_fileModel.index(file.absoluteFilePath());
            m_curItemView->selectionModel()->select(index,QItemSelectionModel::Select);
            if(notScrolled){
                m_curItemView->scrollTo(index);
                notScrolled = false;
            }
        }
    }
}



void DirForm::on_actionFind_triggered()
{
    FindDialog dlg(this,this->m_curDir);
    dlg.exec();
}

void DirForm::on_rowsInserted(const QModelIndex &parent, int first, int last)
{
    if(m_viewModified){
        int col = parent.column();
        //////todo m_curItemView->scrollTo(parent);
        m_viewModified = false;
    }
}

bool inRect(const QPointF &p, qreal x,qreal y, int w ,int h)
{
    bool ret = p.x() > x && p.x() < x + w  && p.y() > y  && p.y() < y + h;
    return ret;

}

QString getDropPath(QAbstractItemView *treeView,const QPointF &globalDropPoint ){
    QPointF p = treeView->viewport()->mapFromGlobal(globalDropPoint);
    qDebug() << "event in treeview point:" << p.x() << "," << p.y();
    QModelIndex index = treeView->indexAt(p.toPoint());
    QString path = ((QFileSystemModel*)treeView->model())->fileInfo(index).absoluteFilePath();

    if(path.isEmpty()){
        path = ((QFileSystemModel*)treeView->model())->filePath( treeView->rootIndex());
    }
    qDebug() << "target path: " << path;

    return path;
}
bool eventInWidget(const QPointF & globalPointF, QWidget *widget)
{

    int x,y,w,h;
    widget->geometry().getRect(&x,&y,&w,&h);
    QPointF pointArea = widget->parentWidget()->mapToGlobal(QPoint(x,y));
    return inRect(globalPointF,pointArea.x(),pointArea.y(),w,h);
}

void DirForm::scrollToPath(QAbstractItemView *itemView,QString newFile ){
    if(newFile.isEmpty()) return;

    ///treeView->setSortingEnabled(false);
    QFileSystemModel * fileModel = (QFileSystemModel*) itemView->model();
    QFileInfo info(newFile);
    QModelIndex index;
    QString parentPath = info.absolutePath();
    if(parentPath == this->m_curDir){
        index = fileModel->index(newFile);
    }
    else if(QFileInfo(parentPath).absolutePath() == this->m_curDir ){
        index = fileModel->index(parentPath);
    }


    if(index.isValid()){

        itemView->selectionModel()->select(index,QItemSelectionModel::SelectionFlag::Select);
        itemView->scrollTo(index);
    }
}

void DirForm::dropEvent(QDropEvent *event)
{
    qDebug()<< "widget drop";

    QPointF globalPoint = this->mapToGlobal(event->position());
    if(eventInWidget(globalPoint,m_curItemView)){

        QString target = getDropPath(m_curItemView,globalPoint);
        QFileInfo targetInfo(target);
        QString targetDir = targetInfo.isDir() ? targetInfo.absoluteFilePath() : targetInfo.absolutePath();
        ui->tableView->selectionModel()->clear();
        for(auto & url : event->mimeData()->urls()){
            QFileInfo srcInfo(url.path());
            qDebug()<< "mimeData url" << url.path();
            if(srcInfo.absolutePath() != targetDir)
            {
                QString newPath = targetDir + "/" + srcInfo.fileName();
                if(QFile::rename(srcInfo.absoluteFilePath(), newPath))
                {
                    scrollToPath(m_curItemView,newPath);
                }
                qDebug()<<"rename:" << srcInfo.absoluteFilePath() << "=>" << newPath;
            }
        }
        qDebug() << "left drop,target=" << target;
    }
    m_viewModified = true;
    event->accept();

}

void DirForm::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls()){
        event->accept();
    }
    else{
        event->ignore();
    }
}




