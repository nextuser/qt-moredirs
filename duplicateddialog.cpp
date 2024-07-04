#include "duplicateddialog.h"
#include "ui_duplicateddialog.h"

DuplicatedDialog::DuplicatedDialog(QWidget *parent,CopyOptions *options)
    : QDialog(parent)
    , ui(new Ui::DuplicatedDialog),copyOptions(options)
{
    ui->setupUi(this);
    ui->checkBoxApplyAll->setChecked(true);
    if(options->dupOption == CopyOptions::Rename){
        ui->radioButtonRename->setChecked(true);
    }
    else if(options->dupOption == CopyOptions::Replace){
        ui->radioButtonReplace->setChecked(true);
    }
    else{
        ui->radioSkip->setChecked(true);
    }
}

DuplicatedDialog::~DuplicatedDialog()
{
    delete ui;
}

void DuplicatedDialog::on_pushButtonOk_clicked()
{
    copyOptions->applyAll = ui->checkBoxApplyAll->isChecked();
    if(ui->radioButtonRename->isChecked()){
        copyOptions->dupOption = CopyOptions::Rename;
    }
    else if(ui->radioButtonReplace->isChecked()){
        copyOptions->dupOption = CopyOptions::Replace;
    }
    else{
        copyOptions->dupOption = CopyOptions::Skip;
    }
    accept();
}


void DuplicatedDialog::on_pushButtonCancel_clicked()
{
    reject();
}

