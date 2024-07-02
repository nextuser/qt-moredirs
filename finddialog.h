#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>
#include "searchthread.h"
namespace Ui {
class FindDialog;
}

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindDialog(QWidget *parent ,QString location);
    ~FindDialog();

private:
    Ui::FindDialog *ui;
    SearchThread * m_thread;
private slots:


    void on_fileFounded(QString file);
    void on_pushButtonFind_clicked();
    void on_findFinished();


    // QWidget interface
    void on_pushButtonBrowse_clicked();
    void on_tableCellDoubleClicked(int row,int col);

protected:
    void closeEvent(QCloseEvent *event);
    void releaseThread();
};

#endif // FINDDIALOG_H
