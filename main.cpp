#include "server.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    server serv;
    serv.StartServer();
    return a.exec();
}
