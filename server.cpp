#include "server.h"

server::server(QObject *parent) : QTcpServer(parent)
{

}

void server::StartServer()
{
    if(!this->listen(QHostAddress::Any, 1234)){
        qDebug() << "Could not start server";

    }
    else{
        qDebug() << "Listening";
    }
}

void server::incomingConnection(qintptr socketDescriptor)
{
    updater *thread = new updater(socketDescriptor, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}
