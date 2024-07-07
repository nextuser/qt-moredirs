#include "copyprocessdialog.h"
#include "ui_copyprocessdialog.h"
#include "fileutil.h"
#include <QSettings>

const static int PERCENT_COUNT_SIZE = 5;
const static QString KEY_SAVE_AUTO = "SaveAuto";

CopyProcessDialog::CopyProcessDialog(QWidget *parent,FileThread * thread)
    : QDialog(parent)
    , ui(new Ui::CopyProcessDialog),m_thread(thread)
{
    ui->setupUi(this);
    this->setVisible(false);
    ui->progressBar->setValue(0);

    connect(thread,&FileThread::countSizeProcessInd,this,&CopyProcessDialog::on_countSizeProcessInd);
    connect(thread,&FileThread::copyProcessInd,this,&CopyProcessDialog::on_copyProcessInd);

    QSettings settings;
    bool checked = settings.value(KEY_SAVE_AUTO).toBool();
    ui->checkBoxAutoClose->setChecked(checked);
    copyTpl = ui->labelCopyStat->text();
    countTpl = ui->labelCountStat->text();
    ui->labelCopyStat->setText("");
    ui->labelCountStat->setText("");

}

CopyProcessDialog::~CopyProcessDialog()
{
    delete ui;
}

const quint64 LIMIT_SPACE = 1 << 23;
const int LIMIT_COUNT = 3000;


void CopyProcessDialog::ensureShow(int count, quint64 size){
    if(count < LIMIT_COUNT && size < LIMIT_SPACE) return;

    if(!m_showed){
        this->setVisible(true);
        m_showed = true;
        this->setModal(false);
        this->show();
    }
}


void CopyProcessDialog::accept()
{
    QSettings settings;
    settings.setValue(KEY_SAVE_AUTO,QVariant(ui->checkBoxAutoClose->isChecked()));
    QDialog::accept();
}


void CopyProcessDialog::on_countSizeProcessInd(int count, int dirCount ,quint64 fileSize, bool bFinished)
{
    ensureShow(count,fileSize);

    QString sizeStr = FileUtil::sizeFormat(fileSize);
    if(bFinished){
        m_totalSize = fileSize;
        m_totalCount = count;
        m_dirCount = dirCount;
        ui->progressBar->setValue(PERCENT_COUNT_SIZE);
        //统计完成: 文件数%1 目录数%2  文件占用%3
        ui->labelCountStat->setText(tr("统计完成: 文件数%1 目录数%2  文件占用%3").arg(count-dirCount).arg(dirCount).arg(sizeStr));
    }
    else{
        int val = ui->progressBar->value();
        ++ val;
        if(val < PERCENT_COUNT_SIZE){
            ui->progressBar->setValue(val);
        }
        //ui->labelStatus->setText("正在统计");
        //"正在统计: 文件数%1 目录数%2  文件占用%3"

        ui->labelCountStat->setText(tr("正在统计: 文件数%1 目录数%2  文件占用%3").arg(count-dirCount).arg(dirCount).arg(sizeStr));
    }
}

const static int SIZE_LIMIT = 1 << 23;//8MB
void CopyProcessDialog::on_copyProcessInd(int copyCount,int dirCount,quint64 fileSize, QString curPath, bool stopped)
{
    Q_UNUSED(stopped);
    ensureShow(copyCount,fileSize);
    QString sizeStr = FileUtil::sizeFormat(fileSize);
    if(m_totalSize > SIZE_LIMIT && m_totalCount > copyCount){
        int percent = PERCENT_COUNT_SIZE +  ((float)fileSize / m_totalSize) * (100 - PERCENT_COUNT_SIZE);
        ui->progressBar->setValue(percent);
        ui->labelFileName->setText(curPath);
        //正在拷贝...:文件数%1 目录数%2  文件占用%3
        ui->labelCopyStat->setText(tr("正在拷贝...:文件数 %1 目录数 %2  文件占用 %3").arg(copyCount-dirCount).arg(dirCount).arg(sizeStr));
    }
    else{
        int percent = PERCENT_COUNT_SIZE +  ((float)copyCount / m_totalCount) * (100 - PERCENT_COUNT_SIZE);
        ui->labelFileName->setText(tr("拷贝文件:") + (curPath));
        //拷贝完成:文件数%1 目录数%2  文件占用%3
        ui->labelCopyStat->setText(tr("拷贝完成:文件数%1 目录数%2  文件占用%3").arg(copyCount-dirCount).arg(dirCount).arg(sizeStr));
        ui->progressBar->setValue(percent);
        if(ui->checkBoxAutoClose->isChecked()){
            accept();
        }

    }
}

void CopyProcessDialog::on_pushButtonCancel_clicked()
{
    m_thread->doStop();

}


void CopyProcessDialog::on_pushButtonClose_clicked()
{
    m_thread->doStop();
    accept();
}

