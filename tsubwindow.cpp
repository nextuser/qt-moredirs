#include "tsubwindow.h"
#include <QMenu>
#include "mainwindow.h"
#include <QCloseEvent>
TSubWindow::TSubWindow(QWidget *parent,MainWindow * window):QMdiSubWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose,false);
    ///setContextMenuPolicy(Qt::CustomContextMenu);
    setSystemMenu(window->createTitleMenu(this));

}

void TSubWindow::closeEvent(QCloseEvent *event)
{
    this->setVisible(false);
    event->ignore();
}

