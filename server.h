#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QDebug>
#include "updater.h"
class server : public QTcpServer
{
    Q_OBJECT
public:
   explicit server(QObject *parent = 0);
   void StartServer();
signals:

public slots:

protected:
   void incomingConnection(qintptr socketDescriptor);
};

#endif
