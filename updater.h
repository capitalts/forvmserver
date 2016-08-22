#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QThread>
#include <QDomDocument>
#include <QDomNode>
#include <QDomElement>
#include <QtNetwork>

class updater : public QThread
{
    Q_OBJECT
public:
    explicit updater(qintptr ID, QObject * parent = 0);
    void start();
signals:
    void error(QTcpSocket::SocketError socketerror);

public slots:
    void readyRead();
    void bytesWritten(qint64 bytes);
    void disconnect();
    void parseArticle();
private:
    void addArticles();
    void biasChange();
    void fairChange();
    void post();
    void newThread();
    QTcpSocket *socket;
    int socketDescriptor;
    QDomDocument inDoc;
    QDomDocument outDoc;
    QString fileName;
    QNetworkAccessManager *qnam;
    QDomDocument articleHtml;
    QNetworkReply* reply;
};

#endif // UPDATER_H
