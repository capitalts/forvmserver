#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QThread>
#include <QDomDocument>
#include <QDomNode>
#include <QDomElement>
#include <articlereader.h>
class updater : public QThread
{
    Q_OBJECT
public:
    explicit updater(int ID, QObject * parent = 0);
    void run();
    void addArticles();
    void biasChange();
    void fairChange();
    void post();
signals:
    void error(QTcpSocket::SocketError socketerror);

public slots:
    void readyRead();
    void disconnected();
private:
    QTcpSocket *socket;
    int socketDescriptor;
    ArticleReader reader;
    QDomDocument inDoc;
    QDomDocument outDoc;
};

#endif // UPDATER_H
