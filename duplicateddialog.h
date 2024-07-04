#ifndef DUPLICATEDDIALOG_H
#define DUPLICATEDDIALOG_H
#include "copyoptions.h"
#include <QDialog>

namespace Ui {
class DuplicatedDialog;
}

class DuplicatedDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DuplicatedDialog(QWidget *parent ,CopyOptions *option);
    ~DuplicatedDialog();

private slots:
    void on_pushButtonOk_clicked();

    void on_pushButtonCancel_clicked();

private:
    Ui::DuplicatedDialog *ui;
    CopyOptions *copyOptions;
};

#endif // DUPLICATEDDIALOG_H
