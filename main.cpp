#include "mainwindow.h"
#include "server.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    server *serv = new server();
    serv->StartServer();
    return a.exec();
}
