#include "mainwindow.h"
#include <QApplication>
#include "QCommandLinkButton"
#include "QEvent"
#include "workerthread.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    w.setWindowTitle("Snake");
    w.resize(800,400);

    return a.exec();
}
