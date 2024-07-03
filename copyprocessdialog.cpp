#include "copyprocessdialog.h"
#include "ui_copyprocessdialog.h"
#include "fileutil.h"
const static int PERCENT_COUNT_SIZE = 5;


CopyProcessDialog::CopyProcessDialog(QWidget *parent,FileThread * thread)
    : QDialog(parent)
    , ui(new Ui::CopyProcessDialog),m_thread(thread)
{
    ui->setupUi(this);
    this->setVisible(false);
    ui->progressBar->setValue(0);
    connect(thread,&FileThread::copyProcessInd,this,&CopyProcessDialog::on_copyProcessInd);
    connect(thread,&FileThread::countSizeProcessInd,this,&CopyProcessDialog::on_countSizeProcessInd);
}

CopyProcessDialog::~CopyProcessDialog()
{
    delete ui;
}

void CopyProcessDialog::ensureShow(){
    if(!m_showed){
        this->setVisible(true);
        m_showed = true;
        exec();
    }
}

void CopyProcessDialog::on_countSizeProcessInd(int count, int dirCount ,quint64 fileSize, bool bFinished)
{
    if(!bFinished) ensureShow();

    QString sizeStr = FileUtil::sizeFormat(fileSize);
    if(bFinished){
        m_totalSize = fileSize;
        m_totalCount = count;
        m_dirCount = dirCount;
        ui->progressBar->setValue(PERCENT_COUNT_SIZE);

        ui->labelCountStat->setText(QString("统计完成:文件数%1 目录数%2  文件占用%3").arg(count-dirCount).arg(dirCount).arg(sizeStr));

    }
    else{
        int val = ui->progressBar->value();
        ++ val;
        if(val < PERCENT_COUNT_SIZE){
            ui->progressBar->setValue(val);
        }
        //ui->labelStatus->setText("正在统计");
        ui->labelCountStat->setText(QString("正在统计:文件数%1 目录数%2  文件占用%3").arg(count-dirCount).arg(dirCount).arg(sizeStr));
    }
}

const static int SIZE_LIMIT = 1 << 23;//8MB
void CopyProcessDialog::on_copyProcessInd(int copyCount,int dirCount,quint64 fileSize, QString curPath, bool stopped)
{
    if(!stopped) ensureShow();
    QString sizeStr = FileUtil::sizeFormat(fileSize);
    if(m_totalSize > SIZE_LIMIT ){
        int percent = PERCENT_COUNT_SIZE +  ((float)fileSize / m_totalSize) * (100 - PERCENT_COUNT_SIZE);
        ui->progressBar->setValue(percent);
        ui->labelFileName->setText(curPath);

        ui->labelCopyStat->setText(QString("正在拷贝...:文件数%1 目录数%2  文件占用%3").arg(copyCount-dirCount).arg(dirCount).arg(sizeStr));
    }
    else{
        int percent = PERCENT_COUNT_SIZE +  ((float)copyCount / m_totalCount) * (100 - PERCENT_COUNT_SIZE);
        ui->labelFileName->setText(QString("完成拷贝%1").arg(curPath));

        ui->labelCopyStat->setText(QString("拷贝完成:文件数%1 目录数%2  文件占用%3").arg(copyCount-dirCount).arg(dirCount).arg(sizeStr));
        ui->progressBar->setValue(percent);

    }


}

void CopyProcessDialog::on_pushButtonCancel_clicked()
{
    m_thread->doStop();

}


void CopyProcessDialog::on_pushButtonClose_clicked()
{
    m_thread->doStop();
    QDialog::reject();
}

