#include "tsubwindow.h"
#include <QMenu>
#include "mainwindow.h"
TSubWindow::TSubWindow(QWidget *parent,MainWindow * window):QMdiSubWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose,false);
    ///setContextMenuPolicy(Qt::CustomContextMenu);
    setSystemMenu(window->createTitleMenu(this));

}

