#include "mainwindow.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Set application icon
    a.setWindowIcon(QIcon(":/icons/app_icon.ico"));
    
    MainWindow w;
    w.show();
    return a.exec();
}
