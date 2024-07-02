#include "finddialog.h"
#include "ui_finddialog.h"
#include "fileutil.h"
#include <QFileInfo>
#include <QTableWidgetItem>
#include <QFile>
#include "dirform.h"
FindDialog::FindDialog(QWidget *parent,QString location)
    : QDialog(parent)
    , ui(new Ui::FindDialog)
{
    ui->setupUi(this);
    ui->tableWidgetResult->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->comboBoxDir->addItem(location);
    ui->comboBoxDir->setCurrentText(location);
    connect(ui->tableWidgetResult,&QTableWidget::cellDoubleClicked,
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


void FindDialog::on_fileFounded(QString file)
{
    QFileInfo info(file);
    int newRow = ui->tableWidgetResult->rowCount();
    ui->tableWidgetResult->insertRow(newRow);
    ui->tableWidgetResult->setItem(newRow,Col_Name, new QTableWidgetItem(file));
    ui->tableWidgetResult->setItem(newRow,Col_Size,new QTableWidgetItem(FileUtil::sizeFormat(info.size())));
    ui->tableWidgetResult->setItem(newRow,Col_Date,new QTableWidgetItem(FileUtil::timeStr(info.lastModified())));
    ui->tableWidgetResult->setItem(newRow,Col_Type,new QTableWidgetItem(FileUtil::fileType(file)));

}

void FindDialog::releaseThread(){
    if(m_thread != nullptr){
        m_thread->stop();

        delete m_thread;
        m_thread = nullptr;
    }
}

void FindDialog::on_pushButtonFind_clicked()
{
    ui->pushButtonFind->setEnabled(false);
    releaseThread();
    ui->tableWidgetResult->clearContents();
    ui->tableWidgetResult->setRowCount(0);
    ui->comboBoxDir->addItem(ui->comboBoxDir->currentText());
    m_thread = new SearchThread();
    connect(m_thread,&SearchThread::file_found,this,&FindDialog::on_fileFounded);
    connect(m_thread,&SearchThread::find_finished,this, &FindDialog::on_findFinished);
    m_thread->findFile(ui->comboBoxDir->currentText(),ui->comboBoxNameFilter->currentText());
}

void FindDialog::on_findFinished()
{
    ui->pushButtonFind->setEnabled(true);
}

void FindDialog::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    if(m_thread != nullptr){
        m_thread->stop();

        delete m_thread;
        m_thread = nullptr;
    }
}

#include <QFileDialog>
void FindDialog::on_pushButtonBrowse_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this,"选择查询的父目录",ui->comboBoxDir->currentText());
    if(path.isEmpty()) return;

    ui->comboBoxDir->addItem(path);
    ui->comboBoxDir->setCurrentText(path);
}

#include <QDesktopServices>
#include <QUrl>
void FindDialog::on_tableCellDoubleClicked(int row, int col)
{
    Q_UNUSED(col);
    QTableWidgetItem * item = ui->tableWidgetResult->itemAt(row,0);;
    QString filePath = item->text();
    QFileInfo info(filePath);
    if(info.isDir()){
        ((DirForm*)parent())->loadDir(filePath);
    }
    else{
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }
}

