#ifndef TABOUTDIALOG_H
#define TABOUTDIALOG_H

#include <QDialog>

namespace Ui {
class TAboutDialog;
}

class TAboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TAboutDialog(QWidget *parent = nullptr);
    ~TAboutDialog();

private slots:
    void on_labelIcon_linkActivated(const QString &link);

    void on_labelAuthor_linkActivated(const QString &link);

private:
    Ui::TAboutDialog *ui;
};

#endif // TABOUTDIALOG_H
