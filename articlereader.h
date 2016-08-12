#ifndef ARTICLEREADER_H
#define ARTICLEREADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QFile>
#include <QFileDialog>
#include <QDialog>
#include <QDomDocument>

class ArticleReader : public QObject
{
    Q_OBJECT
public:
    explicit ArticleReader(QObject *parent = 0);
    void startRequest(QUrl);
    QUrl getArticleSource();
    QString getImageSource();
    QString getArticleTitle();
public slots:
    void open();
private:
    QNetworkAccessManager *qnam;
    QUrl articleSource;
    QString articleTitle;
    QString imageSource;
    QDomDocument articleHtml;
    QNetworkReply* reply;
};

#endif // ARTICLEREADER_H
