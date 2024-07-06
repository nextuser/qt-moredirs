#ifndef COPYPROCESSDIALOG_H
#define COPYPROCESSDIALOG_H

#include <QDialog>
#include "filethread.h"
#include "QCoreApplication"
namespace Ui {
class CopyProcessDialog;
}

class CopyProcessDialog : public QDialog
{

    Q_OBJECT

    /*inline static QString tr2(const char *msg){
        return QCoreApplication::translate("CopyProcessDialog",msg,nullptr);
    }*/
public:
    explicit CopyProcessDialog(QWidget *parent ,FileThread *thread);
    ~CopyProcessDialog();
public slots:
    void on_countSizeProcessInd(int count, int dirCount ,quint64 fileSize, bool bFinished);
    void on_copyProcessInd(int count ,int dirCount,quint64 fileSize,QString curPath, bool stopped = false);
private slots:
    void on_pushButtonCancel_clicked();


    void on_pushButtonClose_clicked();

private:
    Ui::CopyProcessDialog *ui;
    FileThread *m_thread;
    quint64 m_totalSize;
    int m_totalCount;
    int m_dirCount ;
    bool m_showed = false;
    void ensureShow(int count, quint64 size);
    QString countTpl;
    QString copyTpl;

    // QWidget interface
protected:


    // QDialog interface
public slots:
    void accept();
};

#endif // COPYPROCESSDIALOG_H
