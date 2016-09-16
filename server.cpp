#include "server.h"

server::server(QObject *parent) : QTcpServer(parent)
{

}

void server::StartServer()
{
    if(!this->listen(QHostAddress::Any, 8080)){
        qDebug() << "Could not start server";
        qDebug() << errorString();
    }
    else{
        qDebug() << "Listening";
        qDebug() << serverAddress().toString();
    }
}

void server::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << socketDescriptor << "Incoming Connection";
    updater *thread = new updater(socketDescriptor, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();

}
