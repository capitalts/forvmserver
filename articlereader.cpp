#include "articlereader.h"
#include <QEventLoop>
ArticleReader::ArticleReader(QObject *parent) : QObject(parent)
{
    qnam = new QNetworkAccessManager(this);

   connect(qnam, SIGNAL(finished(QNetworkReply*)), this, SLOT(open()));

}



void ArticleReader::startRequest(QUrl url)
{
    reply = qnam->get(QNetworkRequest(url));
    articleSource = url;

}

void ArticleReader::setArticleTitle()
{
    int titleStart = articleHtml.indexOf("<title>");
    int titleEnd = articleHtml.indexOf("</title>");

    for(int i = titleStart+7; i < titleEnd; i++){
        articleTitle.append(articleHtml[i]);
    }
    articleTitle.remove(QString("&#39;"));
    qDebug() << articleTitle.simplified();
    setImageSource();
}

void ArticleReader::setImageSource()
{

    int imageStart = articleHtml.indexOf("og:image");
    QString rightStart = articleHtml.remove(0, imageStart);
    int imageEnd = rightStart.indexOf(".jpg");
    QString imgSource;

    for(int i = 0; i <imageEnd+4; i++){
        imgSource.append(articleHtml[i]);
    }

    imgSource = imgSource.remove(imgSource.indexOf("og:image"), imgSource.indexOf("http"));

    imageSource = imgSource;

}

QUrl ArticleReader::getArticleSource()
{
    return articleSource;
}

QString ArticleReader::getImageSource()
{
    return imageSource;
}

QString ArticleReader::getArticleTitle()
{
    return articleTitle;
}

void ArticleReader::open()
{
        // display contents
    articleHtml = reply->readAll();

    reply->deleteLater();
    qnam->deleteLater();
    setArticleTitle();
}

