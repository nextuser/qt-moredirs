#include "taboutdialog.h"
#include "ui_taboutdialog.h"
#include <QDesktopServices>

const QString icon_url = R"(<a target="_blank" href="https://icons8.com/icon/48501/external-link">icons</a> provided by <a target="_blank" href="https://icons8.com">icons8.com</a>)";
const QString author_url = R"(<a href="mailto:lijinglin323@163.com">lijinglin323@163.com</a>)";

TAboutDialog::TAboutDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TAboutDialog)
{
    ui->setupUi(this);
    ui->labelIcon->setText(icon_url);
    ui->labelAuthor->setText(author_url);
}

TAboutDialog::~TAboutDialog()
{
    delete ui;
}

void TAboutDialog::on_labelIcon_linkActivated(const QString &link)
{
    QDesktopServices::openUrl(QUrl(link));
}


void TAboutDialog::on_labelAuthor_linkActivated(const QString &link)
{
    QDesktopServices::openUrl(QUrl(link));
}

