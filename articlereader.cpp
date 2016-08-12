#include "articlereader.h"
#include <QEventLoop>
ArticleReader::ArticleReader(QObject *parent) : QObject(parent)
{
}



void ArticleReader::startRequest(QUrl url)
{

    qnam = new QNetworkAccessManager(this);
    connect(qnam, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(open()));
    qDebug() << "request Started";
    reply = qnam->get(QNetworkRequest(url));
    qDebug() << url.toString();
    articleSource = url.toString();

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
    if(reply->error()){
            qDebug() << "ERROR!";
            qDebug() << reply->errorString();
        }
    else{

        articleHtml.setContent(reply->readAll());
        articleTitle = articleHtml.firstChild().firstChildElement("head").elementsByTagName("title").at(0).toElement().text();
        QDomNodeList metaList = articleHtml.firstChild().firstChildElement("head").elementsByTagName("meta");
        QDomAttr content;
        for(int i = 0; i < metaList.length(); i++){
            QDomAttr property = metaList.at(i).toElement().attributeNode("property");
            if(property.value() == "og:image"){
               content = metaList.at(i).toElement().attributeNode("content");
               break;
            }
        }
        imageSource = content.value();
    }
    reply->deleteLater();
    qnam->deleteLater();

}

