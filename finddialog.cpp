#include "finddialog.h"
#include "ui_finddialog.h"
#include "fileutil.h"
#include <QFileInfo>
#include <QTableWidgetItem>
#include <QFile>
#include "dirform.h"


FindDialog::FindDialog(QWidget *parent,QString location)
    : QDialog(parent)
    , ui(new Ui::FindDialog),m_thread(nullptr)
{
    ui->setupUi(this);
    m_model = new TResultItemModel(this);
    ui->tableViewResult->setModel(m_model);
    ui->tableViewResult->setColumnWidth(0,240);
    ui->tableViewResult->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->comboBoxDir->addItem(location);
    ui->comboBoxDir->setCurrentText(location);
    changeState(State_NotStart);
    connect(ui->tableViewResult,&QTableView::doubleClicked,
            this, &FindDialog::on_tableCellDoubleClicked);

}

FindDialog::~FindDialog()
{
    delete ui;
    void releaseThread();
}
enum Column{
    Col_Name = 0,
    Col_Size,

    Col_Date,
    Col_Type,
    Col_Count
};

QStandardItem * createItem(QString text, QVariant& data){
    QStandardItem *item  = new QStandardItem(text);
    item->setData(data);
    return item;
}

void FindDialog::appendRows(const QStringList& files){

    /**for(auto & file : files)
    {
        QFileInfo info(file);
        QList<QStandardItem*> tableRow;
        QVariant data(file);
        tableRow.append(createItem(file,data));
        tableRow.append(createItem(FileUtil::sizeFormat(info.size()),data));
        tableRow.append(createItem(FileUtil::fileType(file),data));
        tableRow.append(createItem(FileUtil::timeStr(info.lastModified()),data));

        m_model->appendRow(tableRow);

    }
    m_count += files.size();**/
    m_model->addFiles(files);

}

void FindDialog::changeState(FindState state)
{
    switch(state){
    case State_NotStart:
        ui->pushButtonFind->setEnabled(true);
        ui->pushButtonStop->setEnabled(false);
        ui->tableViewResult->setSortingEnabled(true);
        releaseThread();
        break;
    case State_Finding:
        ui->pushButtonFind->setEnabled(false);
        ui->pushButtonStop->setEnabled(true);
        ui->tableViewResult->setSortingEnabled(false);
        break;
    }
}
#include <QTimer>
void FindDialog::on_fileFounded(QStringList files , bool finished)
{
    appendRows(files);


    if(finished){


        changeState(State_NotStart);
    }
}

void FindDialog::releaseThread(){
    if(m_thread != nullptr){
        m_thread->stop();
        m_thread->terminate();
        disconnect(m_thread,&SearchThread::file_found,this,&FindDialog::on_fileFounded);
        if(m_thread->isRunning()){
            m_thread->msleep(100);
        }
        //delete m_thread;
        m_thread = nullptr;
    }
    else{
        qDebug() << "thread nullptr to release";
    }
}

void FindDialog::on_pushButtonFind_clicked()
{

    changeState(State_Finding);
    m_model->clear();
    ui->comboBoxDir->addItem(ui->comboBoxDir->currentText());
    m_thread = new SearchThread(this);
    connect(m_thread,&SearchThread::file_found,this,&FindDialog::on_fileFounded);
    m_thread->findFile(ui->comboBoxDir->currentText(),ui->comboBoxNameFilter->currentText());
}



void FindDialog::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    releaseThread();
}

#include <QFileDialog>
void FindDialog::on_pushButtonBrowse_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this,tr("选择查询目录位置"),ui->comboBoxDir->currentText());
    if(path.isEmpty()) return;

    ui->comboBoxDir->addItem(path);
    ui->comboBoxDir->setCurrentText(path);
}

#include <QDesktopServices>
#include <QUrl>
void FindDialog::on_tableCellDoubleClicked(const QModelIndex &index)
{

    QString filePath = m_model->filePath(index);

    QFileInfo info(filePath);
    if(info.isDir()){
        ((DirForm*)parent())->loadDir(filePath);
    }
    else{
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }
}


void FindDialog::on_pushButtonStop_clicked()
{
    changeState(State_NotStart);
}

