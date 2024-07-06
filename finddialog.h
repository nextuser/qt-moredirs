#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>
#include "searchthread.h"
#include <QStandardItemModel>
namespace Ui {
class FindDialog;
}

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindDialog(QWidget *parent ,QString location);
    ~FindDialog();
    enum FindState{
        State_NotStart,
        State_Finding

    };
private:
    Ui::FindDialog *ui;
    SearchThread * m_thread;
    QStandardItemModel *m_model;
    int m_count = 0;
    void initColumns();
    void appendRows(const QStringList& files);
    FindState m_state;
    void changeState(FindState state);
private slots:


    void on_fileFounded(QStringList file,bool bFinished);
    void on_pushButtonFind_clicked();

    // QWidget interface
    void on_pushButtonBrowse_clicked();
    void on_tableCellDoubleClicked(const QModelIndex &index);

    void on_pushButtonStop_clicked();

protected:
    void closeEvent(QCloseEvent *event);
    void releaseThread();
};

#endif // FINDDIALOG_H
