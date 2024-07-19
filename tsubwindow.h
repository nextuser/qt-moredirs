#ifndef TSUBWINDOW_H
#define TSUBWINDOW_H

#include <QMdiSubWindow>
class MainWindow;
class TSubWindow : public QMdiSubWindow
{
    Q_OBJECT
public:
    TSubWindow(QWidget *parent,MainWindow *window);



    // QWidget interface
protected:

private :
    MainWindow *mainWindow;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // TSUBWINDOW_H
