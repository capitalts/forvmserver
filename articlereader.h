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

class ArticleReader : public QObject
{
    Q_OBJECT
public:
    explicit ArticleReader(QObject *parent = 0);
    void startRequest(QUrl);
    void setArticleTitle();
    void setImageSource();
    QUrl getArticleSource();
    QString getImageSource();
    QString getArticleTitle();
public slots:
    void open();
private:
    QNetworkAccessManager *qnam;
    QNetworkReply *reply;
    QUrl articleSource;
    QString articleTitle;
    QString imageSource;
    QString articleHtml;
};

#endif // ARTICLEREADER_H
